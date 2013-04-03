/*
   ____    _ __           ____               __    ____
  / __/___(_) /  ___ ____/ __ \__ _____ ___ / /_  /  _/__  ____
 _\ \/ __/ / _ \/ -_) __/ /_/ / // / -_|_-</ __/ _/ // _ \/ __/
/___/\__/_/_.__/\__/_/  \___\_\_,_/\__/___/\__/ /___/_//_/\__(_)

Copyright 2008 SciberQuest Inc.
*/
#include "vtkMultiProcessController.h"
#include "vtkMPIController.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkInformation.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkSmartPointer.h"

#include "vtkPVXMLElement.h"
#include "vtkPVXMLParser.h"
#include "vtkXMLPDataSetWriter.h"

#include "SQMacros.h"
#include "postream.h"
#include "vtkSQBOVMetaReader.h"
#include "vtkSQFieldTracer.h"
#include "vtkSQPlaneSource.h"
#include "vtkSQVolumeSource.h"
#include "vtkSQHemisphereSource.h"
#include "XMLUtils.h"
#include "BatchUtil.h"
#include "vtkSQLog.h"

#include <sstream>
using std::ostringstream;
#include <iostream>
using std::cerr;
using std::endl;
#include <iomanip>
using std::setfill;
using std::setw;
#include <vector>
using std::vector;
#include <string>
using std::string;

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  vtkMultiProcessController *controller=Initialize(&argc,&argv);

  // distribute the configuration file name and time range
  string config;
  string bov;
  string outputDir;
  double time;
  BroadcastConfiguration(
        controller,
        argc,
        argv,
        config,
        bov,
        outputDir,
        time);

  outputDir=Mkdir(controller,outputDir,time);

  // parse config
  vtkSmartPointer<vtkPVXMLElement> root;
  root.TakeReference(ParseConfiguration(controller,config,"MagnetosphereTopologyBatch"));

  // intialize log
  vtkSQLog *log=vtkSQLog::GetGlobalInstance();
  log->SetGlobalLevel(1);
  log->SetFileName(outputDir+"/MagnetosphereTopologyBatch.log");
  log->Initialize(root);


  /// build the pipeline
  vtkPVXMLElement *elem;

  // set up reader
  vector<string> arrays;
  vtkSmartPointer<vtkSQBOVMetaReader> r
    = vtkSmartPointer<vtkSQBOVMetaReader>::New();
  if (r->Initialize(root,bov.c_str(),arrays))
    {
    sqErrorMacro(pCerr(),"Failed to initialize reader.");
    return -1;
    }
  if (arrays.size()!=1)
    {
    sqErrorMacro(pCerr(),"Expected one vector array, got " << arrays.size());
    return -1;
    }
  r->SetPointArrayStatus(arrays[0].c_str(),1);

  // terminator surfaces
  vtkSmartPointer<vtkSQHemisphereSource> hs
    = vtkSmartPointer<vtkSQHemisphereSource>::New();

  if (!hs->Initialize(root))
    {
    hs=NULL;
    }

  // seed source
  int iErr=0;
  vtkAlgorithm *ss=0;

  vtkSmartPointer<vtkSQPlaneSource> ps
    = vtkSmartPointer<vtkSQPlaneSource>::New();

  vtkSmartPointer<vtkSQVolumeSource> vs
    = vtkSmartPointer<vtkSQVolumeSource>::New();

  const char *outFileExt;
  if (!ps->Initialize(root))
    {
    // 2D source
    outFileExt=".pvtp";
    ss=ps;
    }
  else
  if (!vs->Initialize(root))
    {
    // 3D source
    outFileExt=".pvtu";
    ss=vs;
    }
  else
    {
    sqErrorMacro(pCerr(),"Failed to initialize seeds.");
    return -1;
    }

  // field topology mapper
  vtkSmartPointer<vtkSQFieldTracer> ftm
    = vtkSmartPointer<vtkSQFieldTracer>::New();

  if (ftm->Initialize(root))
    {
    sqErrorMacro(pCerr(),"Failed to initialize field tracer.");
    return -1;
    }
  ftm->SetSqueezeColorMap(0);

  ftm->AddVectorInputConnection(r->GetOutputPort(0));
  if (hs)
    {
    ftm->AddTerminatorInputConnection(hs->GetOutputPort(0));
    ftm->AddTerminatorInputConnection(hs->GetOutputPort(1));
    }

  ftm->AddSeedPointInputConnection(ss->GetOutputPort(0));
  ftm->SetInputArrayToProcess(
        0,
        0,
        0,
        vtkDataObject::FIELD_ASSOCIATION_POINTS,
        arrays[0].c_str());

  // intialize the pipeline
  ConfigureExecutive(controller,ftm,time);

  int worldRank=controller->GetLocalProcessId();
  int worldSize=controller->GetNumberOfProcesses();

  // initialize the writer
  vtkSmartPointer<vtkXMLPDataSetWriter> w
    = vtkSmartPointer<vtkXMLPDataSetWriter>::New();

  w->AddInputConnection(0,ftm->GetOutputPort(0));
  w->SetDataModeToAppended();
  w->SetEncodeAppendedData(0);
  w->SetCompressorTypeToNone();
  w->SetNumberOfPieces(worldSize);
  w->SetStartPiece(worldRank);
  w->SetEndPiece(worldRank);
  w->UpdateInformation();

  string outFile=outputDir+"/mtopo.pvtp";
  w->SetFileName(outFile.c_str());
  w->Write();
  w=NULL;

  if (worldRank==0)
    {
    pCerr() << "Wrote: " << outFile.c_str() << "." << endl;
    }

  return Finalize(controller,0);
}
