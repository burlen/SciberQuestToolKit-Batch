SciberQuestToolKit-Batch
========================
Batch programs that make use of the SciberQuestToolKit ParaView plugin.

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
There is a bash script located in the root directory that will
execute each of the batch programs on an example dataset. This
file shows how the programs are used. see: examples.sh

Usage:
========================
Each batch program takes 4 command line areguments:

1) full path to a configuration, see configs dir for templates
2) full path to a bov file
3) path to the output directory (if Lustre then you should lfs setstripe -c -1 -s 512m)
4) the time step to process

It's important to plan your runs carefully.
