SciberQuestToolKit-Batch
========================

Batch programs that make use of the SciberQuestToolKit ParaView plugin.

Building:
$ cmake -DParaView_DIR=/work/ext/ParaView/sqtk-pv/PV/ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/path/to/pv/install ../
$ make -j 8 install

Examples:
$ mpiexec -np 8 SmoothBatch smooth-batch-template.xml /work2/data/2d-asym/asym2d.bov ./2d-asym-g19/ 52650 52650
$ mpiexec -np 8 VortexDetectBatch vortex-batch-template.xml /work2/data/2d-asym/asym2d.bov ./2d-asym-vd/ 52650 52650
