SciberQuestToolKit-Batch
========================
Batch programs that make use of the SciberQuestToolKit ParaView plugin.


Usage:
========================
Each batch program takes 4 command line areguments:

1) full path to a configuration, see configs dir for templates
2) full path to a bov file
3) path to the output directory, if it doesn't exist it will be created.
4) the time step

It's important to plan your runs carefully. For example one should
figure out how much memory a vector field will require prior to
making a run config. A vector field will need (nx*ny*nz*12/2.0**30)GB
to fit entirely into memory. Size your run accordingly. Also keep
in mind that the programs that use the BOV Meta reader (eg. Poincare Map)
each have a "greedy" block cache that can hold as much as the entire
vector field. The block cache size needs to be set such that, on each
compute node, there is enough ram to hold all of the local mpi rank's
block cahe. For example, a data set with nx=1024,ny=1024,nz=1024 on
Hopper, where there are 32GB RAM per node and a max of 24 cores, and
we want to run 1 core per socket or 4 per node giving each MPI rank
at most 8GB, a reasonable block cache size would be 3GB. We want to
set the decomosition such that there are many smallish blocks. This
makes I/O fast and efficient when their are patterns in the field.
For this dataset a decomp of nx=16,ny=16,nz=16 blocks result in 4096
blocks of nx=64, ny=64,nz=64. To set the block cache size we make the
following calculation:

vector size: 1024**3*12/2**30 = 12 GB
ram per core: 3 GB
block cache size: (3 GB/12 GB)4096 = 1049 Blocks

If you experience OOM condiitions wheh running the programs that use
the BOV Meta reader the block cahce size is very likely the issue.

Building:
========================
$ cmake \
    -DParaView_DIR=/work/ext/ParaView/sqtk-pv/PV/ \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=/path/to/pv/install \
    ../

$ make -j 8 install

Examples:
========================
$ mpiexec -np 8 SmoothBatch \
    smooth-batch-template.xml \
    asym2d.bov \
    ./2d-asym-g19/ \
    52650

$ mpiexec -np 8 PoincareBatch \
    poincare-batch-template.xml \
    MagneticIslands.bov \
    ./magi/ \
    0

$ mpiexec -np 8 FTLEBatch \
    ftle-batch-template.xml \
    Gyres.bov \
    ./ftle \
    0


$ mpiexec -np 8 VortexDetectBatch \
    vortex-batch-template.xml \
    asym2d.bov \
    ./2d-asym-vd/ \
    52650
