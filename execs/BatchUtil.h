/*
   ____    _ __           ____               __    ____
  / __/___(_) /  ___ ____/ __ \__ _____ ___ / /_  /  _/__  ____
 _\ \/ __/ / _ \/ -_) __/ /_/ / // / -_|_-</ __/ _/ // _ \/ __/
/___/\__/_/_.__/\__/_/  \___\_\_,_/\__/___/\__/ /___/_//_/\__(_)

Copyright 2012 SciberQuest Inc.
*/
#ifndef TestUtils_h
#define TestUtils_h

class vtkMultiProcessController;
class vtkRenderer;
class vtkDataSet;
class vtkPolyData;
class vtkActor;
class vtkAlgorithm;
class vtkStreamingDemandDrivenPipeline;
class vtkPVXMLElement;
#include <string>
using std::string;

/**
Initialize MPI and the pipeline.
*/
vtkMultiProcessController *Initialize(int *argc, char ***argv);

/**
Finalize MPI and the pipleine.
*/
int Finalize(vtkMultiProcessController* controller, int code);

/**
Binary search for index of value in an array of doubles.
*/
int IndexOf(double value, double *values, int first, int last);

/**
Broadcast the string.
*/
void Broadcast(
      vtkMultiProcessController *controller,
      string &s,
      int root=0);

/**
Broadcast the test configuration.
*/
void BroadcastConfiguration(
      vtkMultiProcessController *controller,
      int argc,
      char **argv,
      string &xml,
      string &bov,
      string &outputDir,
      double &time);


/**
Parse xml config contained in a string into a vtk xml object.
*/
vtkPVXMLElement *ParseConfiguration(
      vtkMultiProcessController *controller,
      string &config,
      const string &requiredType);

/**
Get the pipeline executive for parallel execution.
*/
vtkStreamingDemandDrivenPipeline *ConfigureExecutive(
        vtkMultiProcessController *controller,
        vtkAlgorithm *a,
        double t);

/**
Convert unix to windows paths on windows.
*/
string NativePath(string path);

/**
Make a directory (if it doesn't exist)
*/
void Mkdir(
      vtkMultiProcessController *controller,
      const string &path);

/**
Make a directory "path/time" (if it doesn't exist)
*/
string Mkdir(
      vtkMultiProcessController *controller,
      string &path,
      double time);
/**
Return the base path
*/
string StripFileNameFromPath(const string fileName);

/**
Return the filename without extension
*/
string StripExtensionFromFileName(const string fileName);


/**
Return the filename without it's path
*/
string StripPathFromFileName(const string fileName);
#endif
