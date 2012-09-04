/*
   ____    _ __           ____               __    ____
  / __/___(_) /  ___ ____/ __ \__ _____ ___ / /_  /  _/__  ____
 _\ \/ __/ / _ \/ -_) __/ /_/ / // / -_|_-</ __/ _/ // _ \/ __/
/___/\__/_/_.__/\__/_/  \___\_\_,_/\__/___/\__/ /___/_//_/\__(_)

Copyright 2008 SciberQuest Inc.
*/
#include "vtkMultiProcessController.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkInformation.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkSmartPointer.h"
#include "vtkXMLPDataSetWriter.h"
#include "vtkPVXMLElement.h"
#include "BatchUtil.h"
#include "SQMacros.h"
#include "postream.h"
#include "vtkSQLog.h"
#include "vtkSQBOVMetaReader.h"
#include "vtkSQFieldTracer.h"
#include "vtkSQPlaneSource.h"
#include "vtkSQVolumeSource.h"
#include "vtkSQHemisphereSource.h"

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

#include <mpi.h>

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  // initialize mpi
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
  root.TakeReference(ParseConfiguration(controller,config,"PoincareBatch"));

  // intialize the log
  vtkSQLog *log=vtkSQLog::GetGlobalInstance();
  log->SetFileName("PoincareBatch.log");
  log->Initialize(root);

  // build the pipeline
  vtkSQBOVMetaReader *r=vtkSQBOVMetaReader::New();
  vtkSQPlaneSource *ps=vtkSQPlaneSource::New();
  vtkSQPlaneSource *ms=vtkSQPlaneSource::New();
  vtkSQFieldTracer *ft=vtkSQFieldTracer::New();
  vtkSmartPointer<vtkXMLPDataSetWriter> w=vtkSmartPointer<vtkXMLPDataSetWriter>::New();
  ft->AddVectorInputConnection(r->GetOutputPort(0));
  ft->AddSeedPointInputConnection(ps->GetOutputPort(0));
  ft->AddTerminatorInputConnection(ms->GetOutputPort(0));
  w->AddInputConnection(0,ft->GetOutputPort(0));
  r->Delete();
  ps->Delete();
  ms->Delete();
  ft->Delete();

  // initialize reader
  vector<string> arrays;
  r->SetLogLevel(1);
  if (r->Initialize(root,bov.c_str(),arrays))
    {
    sqErrorMacro(pCerr(),"Failed to initialize reader.");
    return Finalize(controller,-1);
    }
  if (arrays.size()!=1)
    {
    sqErrorMacro(pCerr(),"Expected one vector array, got " << arrays.size());
    return Finalize(controller,-1);
    }
  r->SetPointArrayStatus(arrays[0].c_str(),1);

  // initialize seed source
  ps->SetLogLevel(1);
  if (ps->Initialize(root))
    {
    sqErrorMacro(pCerr(),"Failed to initialize seeds.");
    return Finalize(controller,-1);
    }

  // intialize map surface
  ms->SetOrigin(ps->GetOrigin());
  ms->SetPoint1(ps->GetPoint1());
  ms->SetPoint2(ps->GetPoint2());
  ms->SetResolution(1,1);

  // initialize poincare mapper
  ft->SetLogLevel(1);
  if (ft->Initialize(root))
    {
    sqErrorMacro(pCerr(),"Failed to initialize field tracer.");
    return Finalize(controller,-1);
    }
  ft->SetMode(vtkSQFieldTracer::MODE_POINCARE);
  ft->SetSqueezeColorMap(0);
  ft->SetInputArrayToProcess(
        0,
        0,
        0,
        vtkDataObject::FIELD_ASSOCIATION_POINTS,
        arrays[0].c_str());

  // intialize the pipeline
  ConfigureExecutive(controller,ft,time);

  int worldRank=controller->GetLocalProcessId();
  int worldSize=controller->GetNumberOfProcesses();

  // initialize the writer
  w->SetDataModeToAppended();
  w->SetEncodeAppendedData(0);
  w->SetCompressorTypeToNone();
  w->SetNumberOfPieces(worldSize);
  w->SetStartPiece(worldRank);
  w->SetEndPiece(worldRank);
  w->UpdateInformation();

  string outFile=outputDir+"/poincareMap.pvtp";
  w->SetFileName(outFile.c_str());
  w->Write();
  w=0;

  if (worldRank==0)
    {
    pCerr() << "Wrote: " << outFile.c_str() << "." << endl;
    }

  return Finalize(controller,0);
}
