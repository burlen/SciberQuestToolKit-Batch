SciberQuestToolKit-Batch
========================

Batch programs that make use of the SciberQuestToolKit ParaView plugin.

Building:
$ cmake -DParaView_DIR=/work/ext/ParaView/sqtk-pv/PV/ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/path/to/pv/install ../
$ make -j 8 install

Examples:
$ mpiexec -np 8 SmoothBatch smooth-batch-template.xml asym2d.bov ./2d-asym-g19/ 52650
$ mpiexec -np 8 PoincareBatch poincare-batch-template.xml MagneticIslands.bov  ./magi/ 0
$ mpiexec -np 8 VortexDetectBatch vortex-batch-template.xml asym2d.bov ./2d-asym-vd/ 52650
