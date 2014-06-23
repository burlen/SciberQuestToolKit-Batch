#!/bin/bash

rm -rf ./example-output/*

MPIRUN="${MPIRUN:=mpiexec}"
NP="${NP:=8}"

echo
echo
echo "Running topology example..."
echo "========================================================"
time \
  $MPIRUN -n $NP \
    ./bin/execs/TopologyBatch \
    ./configs/templates/mtopo-template.xml \
    ./example-data/SmallVector/SmallVector.bovm \
    ./example-output/topo-smvec \
    0
exit 0

echo
echo
echo "Running poincare mapper example..."
echo "========================================================"
time \
  $MPIRUN -n $NP \
    ./bin/execs/PoincareBatch \
    ./configs/templates/poincare-batch-template.xml \
    ./example-data/MagneticIslands/MagneticIslands.bov \
    ./example-output/poincare-magi \
    0


echo
echo
echo "Running ftle example..."
echo "========================================================"
time \
  $MPIRUN -n $NP \
    ./bin/execs/FTLEBatch \
    ./configs/templates/ftle-batch-template.xml \
    ./example-data/Gyres/Gyres.bov \
    ./example-output/ftle-gyres \
    0

echo
echo
echo "Running gaussian smoothing example..."
echo "========================================================"
time \
  $MPIRUN -n $NP \
    ./bin/execs/SmoothBatch \
    ./configs/templates/smooth-batch-template.xml \
    ./example-data/Asym2D/Asym2D.bov  \
    ./example-output/smoothing-asym \
    292500

echo
echo
