SciberQuestToolKit-Batch
========================
Batch programs that make use of the SciberQuestToolKit ParaView plugin.

Downloading:
========================
$ git clone --recursive git://paraview.org/ParaView.git
$ cd ParaView
$ ./Utilities/SetupForDevelopment.sh
$ git remote add SQTK-Dev https://github.com/burlen/SciberQuestToolKit-Dev.git
$ git fetch SQTK-Dev
$ git checkout --track -b sqtk-testing remotes/SQTK-Dev/sqtk-testing
$ cd ..
$ git clone https://github.com/burlen/SciberQuestToolKit-Batch.git

Building:
========================
$ cd SciberQuestToolKit-Batch/bin
$ cmake \
    -DParaView_DIR=/work/ext/ParaView/sqtk-pv/PV/ \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=/path/to/pv/install \
    ../

$ make -j 8 install

Examples:
========================
There is a bash script located in the root directory that will
execute each of the batch programs on an example dataset producing
aexample output data that can be visualized in paraview. This
script shows how the programs are used. see: examples.sh. It
may require some minor modification to work on your system.

Usage:
========================
Each batch program takes 4 command line areguments:

1) full path to a configuration, see configs dir for templates
2) full path to a bov file
3) path to the output directory (if Lustre then you should lfs setstripe -c -1 -s 512m)
4) the time step to process

Running on hopper:
========================
It's important to plan your runs carefully. One challenge is to schedule the job with enough run time as if the job is killed there's currently no restart mechanism.

* for FTLE integration occurs across points, a worst case estimate of the number of integrations is nx*ny*2+2, where nx, and ny are the number of cells in your seed plane.
* for FTLE there are a large number of shorter integrations, for example use the cross diagonal for the arc length.
* for Poincare map the integration must make a number of passes through the domain. for example 4 times the cross diagonal for the arc length.
* for Poincare map there are fewer longer integrations, work is parallelized over seeds so there's no point in requesting more cores than seed points.
* for both FTLE and Poincare map the integrator relies on caching data, it helps to spread the mpi-ranks across more nodes. plan for the cahce size to be 16GB/nRanksPerNode*fac, a good choice of fac is 50%.
* for both FTLE and Poincare map integration goes forward and backward effectively doubling the requested arc length.

there are helper scripts for launching jobs on hopper, run the script with no arguments to see a help message. for example:

/usr/common/graphics/ParaView/next-mom-so/start_sqtkbatch.sh 392 1 48:00:00 m1303 regular PoincareBatch /usr/common/graphics/ParaView/builds/SciberQuestToolKit-Batch/configs/poincare-batch-mime.xml /scratch/scratchdirs/loring/mime-g19/mime1_sm_19.bov /scratch/scratchdirs/loring/mime-g19-poincare 25714

/usr/common/graphics/ParaView/next-mom-so/start_sqtkbatch.sh 4096 1 48:00:00 m1303 regular FTLEBatch /usr/common/graphics/ParaView/builds/SciberQuestToolKit-Batch/configs/ftle-batch-mime.xml /scratch/scratchdirs/loring/mime-g19/mime1_sm_19.bov /scratch/scratchdirs/loring/mime-g19-ftle-3000 25714
