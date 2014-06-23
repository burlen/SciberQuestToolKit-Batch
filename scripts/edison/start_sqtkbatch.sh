#!/bin/bash

if [ $# != 10 ]
then
  echo "Usage: start_pvserver.sh"
  echo
  echo "  NCPUS     - number of processes in mutiple of 24."
  echo "  NCPUS_PER_NODE - number of processes per node."
  echo "  WALLTIME  - wall time in HH:MM:SS format."
  echo "  ACCOUNT   - account name to run the job against."
  echo "  QUEUE     - the queue to use."
  echo "  EXEC      - path to the batch program."
  echo "  CONFIG    - path to the xml config."
  echo "  BOV       - path to the input dataset."
  echo "  OUTPUT    - path to the output dir."
  echo "  TIME      - time to process."
  echo
  exit
fi

NCPUS=$1
#NCPUS_PER_SOCKET=$2
#(( NCPUS_PER_SOCKET = NCPUS_PER_SOCKET>12 ? 12 : $NCPUS_PER_SOCKET ))
#(( NCPUS_PER_SOCKET = NCPUS_PER_SOCKET<1  ? 1  : $NCPUS_PER_SOCKET ))
#NCPUS_PER_NODE=`echo 2*$NCPUS_PER_SOCKET | bc`
NCPUS_PER_NODE=$2
(( NCPUS_PER_NODE = NCPUS_PER_NODE>24 ? 24 : $NCPUS_PER_NODE ))
(( NCPUS_PER_NODE = NCPUS_PER_NODE<1  ? 1  : $NCPUS_PER_NODE ))
NNODES=`echo $NCPUS/$NCPUS_PER_NODE | bc`
(( NNODES = NNODES==0 ? 1 : $NNODES ))
MEM=`echo 32*$NNODES | bc`
WALLTIME=$3
ACCOUNT=$4
if [[ "$ACCOUNT" == "default" ]]
then
  ACCOUNT=`/usr/common/usg/bin/getnim -U $USER -D | cut -d" " -f1`
fi
ACCOUNTS=`/usr/common/usg/bin/getnim -U $USER | cut -d" " -f1 | tr '\n' ' '`
QUEUE=$5
EXEC=$6
CONFIG=$7
BOV=$8
OUTPUT=$9
TIME=${10}

LOGIN_HOST=`/bin/hostname`

PV_HOME=/usr/common/graphics/ParaView/next-mom-so

# modules envirtonment isn't setup when using the ssh command.
LD_LIBRARY_PATH=$PV_HOME/lib:$PV_HOME/lib/paraview-3.14:/usr/common/usg/python/2.7.1/lib:/opt/gcc/mpc/0.8.1/lib:/opt/gcc/mpfr/2.4.2/lib:/opt/gcc/gmp/4.3.2/lib:/opt/gcc/4.6.1/snos/lib64:/opt/moab/6.1.4/lib:/opt/cray/atp/1.4.1/lib
export LD_LIBRARY_PATH

PATH=$PV_HOME/lib:$PV_HOME/lib/paraview-3.14:$PV_HOME/bin:/usr/common/usg/python/2.7.1/bin:/opt/cray/atp/1.4.1/bin:/opt/cray/xt-asyncpe/5.04/bin:/opt/cray/pmi/3.0.0-1.0000.8661.28.2807.gem/bin:/opt/gcc/4.6.1/bin:/usr/common/usg/bin:/usr/common/mss/bin:/usr/common/nsg/bin:/opt/moab/6.1.4/bin:/opt/torque/2.5.9/sbin:/opt/torque/2.5.9/bin:/opt/cray/llm/default/bin:/opt/cray/llm/default/etc:/opt/cray/xpmem/0.1-2.0400.30792.5.6.gem/bin:/opt/cray/dmapp/3.2.1-1.0400.3965.10.63.gem/bin:/opt/cray/ugni/2.3-1.0400.4127.5.20.gem/bin:/opt/cray/udreg/2.3.1-1.0400.3911.5.13.gem/bin:/opt/cray/lustre-cray_gem_s/1.8.4_2.6.32.45_0.3.2_1.0400.6336.8.1-1.0400.30879.1.81/sbin:/opt/cray/lustre-cray_gem_s/1.8.4_2.6.32.45_0.3.2_1.0400.6336.8.1-1.0400.30879.1.81/bin:/opt/cray/MySQL/5.0.64-1.0000.4667.20.1/sbin:/opt/cray/MySQL/5.0.64-1.0000.4667.20.1/bin:/opt/cray/sdb/1.0-1.0400.31073.9.3.gem/bin:/opt/cray/nodestat/2.2-1.0400.29866.4.3.gem/bin:/opt/modules/3.2.6.6/bin:/usr/lpp/mmfs/bin:/usr/local/bin:/usr/bin:/bin:/usr/bin/X11:/usr/X11R6/bin:/usr/games:/usr/lib64/jvm/jre/bin:/usr/lib/mit/bin:/usr/lib/mit/sbin:/opt/pathscale/bin:.
export PATH

NCAT_PATH=/usr/common/graphics/ParaView/nmap-5.51/bin

echo '================================================================'
echo '         ________  ________ __    ___       __      __  '
echo '        / __/ __ \/_  __/ //_/___/ _ )___ _/ /_____/ /  '
echo '       _\ \/ /_/ / / / / ,< /___/ _  / _ `/ __/ __/ _ \ '
echo '      /___/\___\_\/_/ /_/|_|   /____/\_,_/\__/\__/_//_/ '
echo '================================================================'
echo
echo "Setting environment..."
#echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
#echo "PATH=$PATH"
echo "ACCOUNTS=$ACCOUNTS"
echo "ACCOUNT=$ACCOUNT"
echo "NCPUS=$NCPUS"
echo "NCPUS_PER_NODE=$NCPUS_PER_NODE"
echo "NCPUS_PER_SOCKET=$NCPUS_PER_SOCKET"
echo "NNODES=$NNODES"
echo "MEM=$MEM\GB"
echo "WALLTIME=$WALLTIME"
echo "PORT=$PORT"
echo "ACCOUNT=$ACCOUNT"
echo "QUEUE=$QUEUE"
echo "LOGIN_HOST=$LOGIN_HOST"
echo "LOGIN_PORT=$LOGIN_PORT"
echo "EXEC="`which $EXEC`
echo "CONFIG=$CONFIG"
echo "BOV=$BOV"
echo "OUTPUT=$OUTPUT"
echo "TIME=$TIME"

echo "Starting job via qsub..."

# pass these to the script
export CRAY_ROOTFS=DSL
export MPICH_GNI_DATAGRAM_TIMEOUT=-1
export PV_NCAT_PATH=$NCAT_PATH
export PV_PORT=$PORT
export PV_NCPUS=$NCPUS
export PV_NCPUS_PER_SOCKET=$NCPUS_PER_SOCKET
export PV_NCPUS_PER_NODE=$NCPUS_PER_NODE
export PV_LOGIN_HOST=$LOGIN_HOST
export SQTK_EXEC=$EXEC
export SQTK_CONFIG=$CONFIG
export SQTK_BOV=$BOV
export SQTK_OUTPUT=$OUTPUT
export SQTK_TIME=$TIME
#export PV_LOGIN_PORT=$LOGIN_PORT
JOB_NAME=`basename $EXEC`
JID=`qsub -V -N $JOB_NAME -A $ACCOUNT -q $QUEUE -l mppwidth=$NCPUS -l mppnppn=$NCPUS_PER_NODE -l walltime=$WALLTIME $PV_HOME/start_sqtkbatch.qsub`
ERRNO=$?
if [ $ERRNO == 0 ]
then
echo "Job $JID submitted succesfully."
qstat $JID
echo
else
echo "ERROR $ERRNO: in job submission."
fi
