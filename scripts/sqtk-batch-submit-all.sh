#!/bin/bash

# following parameters can be modifed here for different users
ACCT=TG-ATM090046
QUEUE=long
export PV_PATH=/home/01237/bloring/apps/PV3-next-IVR/lib/paraview-3.98/
export LD_LIBRARY_PATH=$PV_PATH:$LD_LIBRARY_PATH

n_args=$#
required_n_args=8

if [ "$n_args" -lt "$required_n_args" ]
then
  echo "n_args=$n_args"
  echo "$@"
  echo "expected the following command line arguments:"
  echo "N_CPUS"
  echo "N_CPUS_PER_NODE"
  echo "RUN_TIME"
  echo "EXE"
  echo "SRC_DIR"
  echo "CONFIG_FILE"
  echo "BOV_FILE"
  echo "DEST_DIR"
  echo "FIELD_NAME_1"
  echo "..."
  echo "FIELD_NAME_N"
  exit -1
fi

N_CPUS=$1
N_CPUS_PER_NODE=$2
RUN_TIME=$3
EXE=$4
SRC_DIR=$5
CONFIG_FILE=$6
BOV_FILE=$7
DEST_DIR=$8

echo "PV_PATH=$PV_PATH"
echo "ACCT=$ACCT"
echo "QUEUE=$QUEUE"
echo "N_CPUS=$N_CPUS"
echo "N_CPUS=$N_CPUS_PER_NODE"
echo "RUN_TIME=$RUN_TIME"
echo "EXE=$EXE"
echo "SRC_DIR=$SRC_DIR"
echo "CONFIG_FILE=$CONFIG_FILE"
echo "BOV_FILE=$BOV_FILE"
echo "DEST_DIR=$DEST_DIR"

for i in `seq 1 $required_n_args`
do
  shift
done

trap "echo exiting; exit -1;" INT TERM

for field in "$@"
do
  for file in `ls  $SRC_DIR/$field[xyz]*\_[0-9]*.gda | xargs -n1 basename`
  do
    #if [[ ! -a $DEST_DIR/$file ]]
    #then
      # only allowed 20 jobs in the queue
      over_limit=1
      while [ "$over_limit" -eq 1 ]
      do
        njobs=`qstat | wc -l`
        if [ "$njobs" -lt 19 ]
        then
          over_limit=0
        fi
      done

      step=`echo $file | cut -d_ -f2 | cut -d. -f1`
      echo "submitted for $field $step"

      echo \
      qsub -A $ACCT -V -N $field-$step -q $QUEUE -P vis -pe $N_CPUS_PER_NODE\way $N_CPUS -l h_rt=$RUN_TIME \
           $PV_PATH/sqtk-batch-longhorn.qsub \
           $PV_PATH \
           $PV_PATH/$EXE \
           $CONFIG_FILE \
           $SRC_DIR/$BOV_FILE \
           $DEST_DIR \
           $step
    #fi
  done
done

