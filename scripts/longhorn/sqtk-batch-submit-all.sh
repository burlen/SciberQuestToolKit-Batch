#!/bin/bash

# following parameters can be modifed here for different users
ACCT=TG-ATM090046
QUEUE=request
export PV_PATH=/home/01237/bloring/apps/PV3-next-IVR/lib/paraview-3.98/
export LD_LIBRARY_PATH=$PV_PATH:$LD_LIBRARY_PATH
output=mtopo.pvtp

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
MAX_JOBS_IN_QUEUE=5

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
echo "MAX_JOBS_IN_QUEUE=$MAX_JOBS_IN_QUEUE"

for i in `seq 1 $required_n_args`
do
  shift
done

trap "echo exiting; exit -1;" INT TERM

# job for first dependency
#qsub -A $ACCT -V -N first-dep -q $QUEUE -P vis -pe 1way 8 \
#   -l h_rt=00:05:00 $PV_PATH/sqtk-batch-longhorn.qsub
#JID=first-dep

for field in "$@"
do
  # process steps in reverse order
  # later steps are typically more interetsing
  steps=`ls $SRC_DIR/$field[x]*\_[0-9]*.gda | cut -d_ -f2 | cut -d. -f1 | sort -rn`
  for step in $steps
  do
    # skip the run if the output dir already
    # exists
    stepdir=`printf %010.f $step`
    if [ -e $DEST_DIR/$stepdir/$output ]
    then
      # already run
      echo "skipped submission for $field $step"
    else
      # submit a job for this file
      # only allowed 20 jobs in the queue
      over_limit=1
      while [ "$over_limit" -eq 1 ]
      do
        njobs=`qstat | wc -l`
        if [ "$njobs" -lt 10 ]
        then
          over_limit=0
        fi
      done

      echo "submitted for $field $step"

      #echo \
      qsub -A $ACCT -V -N $field-$step -q $QUEUE -P vis -pe $N_CPUS_PER_NODE\way $N_CPUS -l h_rt=$RUN_TIME \
           $PV_PATH/sqtk-batch-longhorn.qsub \
           $PV_PATH \
           $PV_PATH/$EXE \
           $CONFIG_FILE \
           $SRC_DIR/$BOV_FILE \
           $DEST_DIR \
           $step
      #     -hold_jid $JID \
      #JID=$field-$step
    fi
  done
done
