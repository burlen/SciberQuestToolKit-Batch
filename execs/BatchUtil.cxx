/*
   ____    _ __           ____               __    ____
  / __/___(_) /  ___ ____/ __ \__ _____ ___ / /_  /  _/__  ____
 _\ \/ __/ / _ \/ -_) __/ /_/ / // / -_|_-</ __/ _/ // _ \/ __/
/___/\__/_/_.__/\__/_/  \___\_\_,_/\__/___/\__/ /___/_//_/\__(_)

Copyright 2012 SciberQuest Inc.
*/
#include "BatchUtil.h"
#include "vtkPVXMLElement.h"
#include "vtkPVXMLParser.h"
#include "vtkMultiProcessController.h"
#include "vtkMPIController.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkAlgorithm.h"
#include "vtkDataSetAttributes.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkSQLog.h"
#include "SQMacros.h"
#include "postream.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <cfloat>
#include <iostream>
using std::cerr;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <sstream>
using std::istringstream;
using std::ostringstream;
#include <mpi.h>

//*****************************************************************************
vtkMultiProcessController *Initialize(int *argc, char ***argv)
{
  vtkMPIController *controller=vtkMPIController::New();
  controller->Initialize(argc,argv,0);

  vtkMultiProcessController::SetGlobalController(controller);

  vtkCompositeDataPipeline* cexec=vtkCompositeDataPipeline::New();
  vtkAlgorithm::SetDefaultExecutivePrototype(cexec);
  cexec->Delete();

  vtkSQLog *log=vtkSQLog::GetGlobalInstance();
  log->StartEvent(0,"TotalRunTime");

  return controller;
}

//*****************************************************************************
int Finalize(vtkMultiProcessController* controller, int code)
{
  vtkSQLog *log=vtkSQLog::GetGlobalInstance();
  log->EndEvent(0,"TotalRunTime");

  if (code==0)
    {
    log->Update();
    log->Write();
    }

  vtkAlgorithm::SetDefaultExecutivePrototype(0);
  controller->Finalize();
  controller->Delete();

  return code;
}

//*****************************************************************************
int IndexOf(double value, double *values, int first, int last)
{
  int mid=(first+last)/2;
  if (values[mid]==value)
    {
    return mid;
    }
  else
  if (mid!=first && values[mid]>value)
    {
    return IndexOf(value,values,first,mid-1);
    }
  else
  if (mid!=last && values[mid]<value)
    {
    return IndexOf(value,values,mid+1,last);
    }
  return -1;
}

// ****************************************************************************
void Broadcast(vtkMultiProcessController *controller, string &s, int root)
{
  int worldRank=controller->GetLocalProcessId();

  vector<char> buffer;

  if (worldRank==root)
    {
    buffer.assign(s.begin(),s.end());

    vtkIdType bufferSize=buffer.size();
    controller->Broadcast(&bufferSize,1,0);
    controller->Broadcast(&buffer[0],bufferSize,0);
    }
  else
    {
    vtkIdType bufferSize=0;
    controller->Broadcast(&bufferSize,1,0);

    buffer.resize(bufferSize,'\0');
    controller->Broadcast(&buffer[0],bufferSize,0);

    s.assign(buffer.begin(),buffer.end());
    }
}

// ****************************************************************************
string Mkdir(
      vtkMultiProcessController *controller,
      string &path,
      double time)
{
  Mkdir(controller,path);

  ostringstream fns;
  fns
    << path
    << "/"
    << setfill('0') << setw(10) << time;

  Mkdir(controller,fns.str().c_str());

  return fns.str();
}


// ****************************************************************************
void Mkdir(
      vtkMultiProcessController *controller,
      const string &path)
{
  int worldRank=controller->GetLocalProcessId();
  if (worldRank==0)
    {
    // mkdir (if needed)
    int iErr=mkdir(path.c_str(),S_IRWXU|S_IXGRP);
    if (iErr<0 && (errno!=EEXIST))
      {
      char *sErr=strerror(errno);
      sqErrorMacro(pCerr(),
          << "Failed to mkdir " << path << "." << endl
          << "Error: " << sErr << ".");
      MPI_Abort(MPI_COMM_WORLD,-1);
      }

    }
  // don't use the dir until it's been made.
  controller->Barrier();
}

// ****************************************************************************
void BroadcastConfiguration(
      vtkMultiProcessController *controller,
      int argc,
      char **argv,
      string &config,
      string &bov,
      string &outputDir,
      double &time)
{
  int ok;
  int worldRank=controller->GetLocalProcessId();
  if (worldRank==0)
    {
    if (argc!=5)
      {
      cerr
        << "Error: Command tail." << endl
        << " 1) /path/to/runConfig.xml" << endl
        << " 2) /path/to/file.bovm" << endl
        << " 3) /path/to/output/" << endl
        << " 4) time (floating point)" << endl
        << endl;
      MPI_Abort(MPI_COMM_WORLD,-1);
      }

    config=argv[1];
    bov=argv[2];
    outputDir=argv[3];
    time=atof(argv[4]);

    // read the metadata file into a buffer.
    ifstream fs(config.c_str());
    if (!fs.good())
      {
      cerr << "Error, failed to open " << config << endl;
      MPI_Abort(MPI_COMM_WORLD,-1);
      }
    fs.seekg(0,ios::end);
    size_t len=fs.tellg();
    fs.seekg(0,ios::beg);
    char *configFile=(char *)malloc(len+1);
    fs.read(configFile,len);
    if (fs.fail()||fs.bad())
      {
      fs.close();
      free(configFile);
      cerr << "Error, failed to read " << config << endl;
      MPI_Abort(MPI_COMM_WORLD,-1);
      }
    fs.close();
    configFile[len]='\0';
    config=configFile;
    free(configFile);
    }

  // commuinate the command line configuration
  Broadcast(controller,config);
  Broadcast(controller,bov);
  Broadcast(controller,outputDir);
  controller->Broadcast(&time,1,0);

  vtkSQLog *log=vtkSQLog::GetGlobalInstance();
  if (log->GetGlobalLevel())
    {
    log->GetHeader()
      << "# configName=" << config << "\n"
      << "# bovFileName=" << bov << "\n"
      << "# outputPath=" << outputDir << "\n"
      << "# time=" << time << "\n";
    }
}

// ****************************************************************************
vtkPVXMLElement *ParseConfiguration(
      vtkMultiProcessController *controller,
      string &config,
      const string &requiredType)
{
  // parse the xml
  istringstream xml(config);

  vtkPVXMLParser *parser=vtkPVXMLParser::New();
  parser->SetStream(&xml);
  parser->Parse();

  // check for the semblance of a valid configuration hierarchy
  vtkPVXMLElement *root=parser->GetRootElement();
  if (root==0)
    {
    sqErrorMacro(pCerr(),
      << "Invalid XML in file " << endl << config << endl);
    MPI_Abort(MPI_COMM_WORLD,-1);
    }
  root->Register(0);
  parser->Delete();

  const char *foundType=root->GetName();
  if (foundType==0 || foundType!=requiredType)
    {
    sqErrorMacro(pCerr(),
      << "This is not a valid " << requiredType << " XML hierarchy.");
    MPI_Abort(MPI_COMM_WORLD,-1);
    }

  return root;
}

// ****************************************************************************
vtkStreamingDemandDrivenPipeline *ConfigureExecutive(
        vtkMultiProcessController *controller,
        vtkAlgorithm *a,
        double t)
{
  int worldRank=controller->GetLocalProcessId();
  int worldSize=controller->GetNumberOfProcesses();

  vtkStreamingDemandDrivenPipeline* exec
     = dynamic_cast<vtkStreamingDemandDrivenPipeline*>(a->GetExecutive());
  if (exec==NULL)
    {
    sqErrorMacro(pCerr(),"Executive not a vtkStreamingDemandDrivenPipeline");
    return NULL;
    }

  vtkInformation *info  = exec->GetOutputInformation(0);

  exec->SetUpdateNumberOfPieces(info,worldSize);
  exec->SetUpdatePiece(info,worldRank);
  exec->UpdateInformation();
  exec->SetUpdateExtent(info,worldRank,worldSize,0);
  exec->SetUpdateTimeStep(0,t);
  exec->PropagateUpdateExtent(0);

  return exec;
}

// ****************************************************************************
string NativePath(string path)
{
  string nativePath;
  #ifdef WIN32
  size_t n=path.size();
  for (size_t i=0; i<n; ++i)
    {
    if (path[i]=='/')
      {
      nativePath+='\\';
      continue;
      }
    nativePath+=path[i];
    }
  return nativePath;
  #else
  return path;
  #endif

  return "";
}
