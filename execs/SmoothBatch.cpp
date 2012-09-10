/*
   ____    _ __           ____               __    ____
  / __/___(_) /  ___ ____/ __ \__ _____ ___ / /_  /  _/__  ____
 _\ \/ __/ / _ \/ -_) __/ /_/ / // / -_|_-</ __/ _/ // _ \/ __/
/___/\__/_/_.__/\__/_/  \___\_\_,_/\__/___/\__/ /___/_//_/\__(_)

Copyright 2008 SciberQuest Inc.
*/
#include "vtkMultiProcessController.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkSmartPointer.h"
#include "vtkDataObject.h"
#include "vtkPVXMLElement.h"
#include "SQMacros.h"
#include "postream.h"
#include "vtkSQBOVReader.h"
#include "vtkSQBOVWriter.h"
#include "vtkSQImageGhosts.h"
#include "vtkSQKernelConvolution.h"
#include "vtkSQLog.h"
#include "FsUtils.h"
#include "DebugUtil.h"
#include "BatchUtil.h"

#include <iostream>
using std::cin;
using std::cerr;
using std::endl;
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

  Mkdir(controller,outputDir);

  // parse config
  vtkSmartPointer<vtkPVXMLElement> root;
  root.TakeReference(ParseConfiguration(controller,config,"SmoothBatch"));

  // intialize the log
  vtkSQLog *log=vtkSQLog::GetGlobalInstance();
  log->SetGlobalLevel(1);
  log->SetFileName(outputDir+"/SmoothBatch.log");
  log->Initialize(root);

  /// build the pipeline
  vtkSQBOVReader *r=vtkSQBOVReader::New();
  vtkSQImageGhosts *ig=vtkSQImageGhosts::New();
  vtkSQKernelConvolution *kconv=vtkSQKernelConvolution::New();
  vtkSmartPointer<vtkSQBOVWriter> w=vtkSmartPointer<vtkSQBOVWriter>::New();
  ig->AddInputConnection(r->GetOutputPort(0));
  kconv->AddInputConnection(ig->GetOutputPort(0));
  w->AddInputConnection(0,kconv->GetOutputPort(0));
  r->Delete();
  ig->Delete();
  kconv->Delete();

  // set up reader
  vector<string> arrays;
  if (r->Initialize(root,bov.c_str(),arrays)<0)
    {
    sqErrorMacro(pCerr(),"Failed to initialize reader.");
    return Finalize(controller,-1);
    }
  int nArrays=arrays.size();

  // set up ghost cell generator
  if(ig->Initialize(root)<0)
    {
    sqErrorMacro(pCerr(),"Failed to initialize ghost generator.");
    return Finalize(controller,-1);
    }

  // set up smoothing filter
  if (kconv->Initialize(root)<0)
    {
    sqErrorMacro(pCerr(),"Failed to initialize smoothing filter.");
    return Finalize(controller,-1);
    }
  int fwid=kconv->GetKernelWidth();

  // set up the writer
  ostringstream bovId;
  bovId << "_sm_" << fwid << ".bov";
  string outputBovFileName
    = outputDir + "/" +
        StripExtensionFromFileName(StripPathFromFileName(bov)) +
          bovId.str();

  if(w->Initialize(root)<0)
    {
    sqErrorMacro(pCerr(),"Failed to initialize writer.");
    return Finalize(controller,-1);
    }
  w->SetFileName(outputBovFileName.c_str());

  // intialize the pipeline
  ConfigureExecutive(controller,kconv,time);

  int worldRank=controller->GetLocalProcessId();
  int worldSize=controller->GetNumberOfProcesses();

  // execute the pipeline for each array
  for (int vecIdx=0; vecIdx<nArrays; ++vecIdx)
    {
    const string &vecName=arrays[vecIdx];

    r->ClearPointArrayStatus();
    r->SetPointArrayStatus(vecName.c_str(),1);

    kconv->ClearInputArrays();
    kconv->AddInputArray(vecName.c_str());

    w->Update();

    if (worldRank==0)
      {
      pCerr() << "Wrote " << vecName << endl;
      }
    }
  w->WriteMetaData();
  w=0;

  return Finalize(controller,0);
}
