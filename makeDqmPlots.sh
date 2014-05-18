#!/bin/sh

if [ $# -lt 1 ]; then
    echo "Usage: makeDqmPlots.sh <raw binary file> <DQM_config>"
    exit
fi

if [ -z "$ROOTSYS" ]; then
    echo "Please source root environment: source $HOME/root/bin/thisroot.sh"
    exit 1
#else
#    echo "ROOT already configured"
fi
 
runFile=$1
runName=`basename $runFile`

#Reading config file if available
if [ $# -eq 2 ]; then
    DQM_CONFIG=$2
else
    DQM_CONFIG=/home/cmsdaq/DQM/DQM_config.txt    
fi

if [ ! -f ${DQM_CONFIG} ]; then
    echo "ERROR: ${DQM_CONFIG} does not exists"
    exit
fi

echo "Sourcing DQM config ${DQM_CONFIG}"
. ${DQM_CONFIG}

mkdir -p ${DQM_OUT_DIR}/dataTree
mkdir -p ${DQM_OUT_DIR}/log


echo "============================================================="
echo "       Running DQM analysis on $runFile"
echo "============================================================="


echo "===> Converting $runFile in ROOT format"
${DQM_HOME}/bin/readBinary -f $1 -o ${DQM_OUT_DIR}/dataTree/${runName%%.*}.root 2>&1 > ${DQM_OUT_DIR}/log/convertRawData_${runName%%.*}.log 
echo "Converted $runFile into ROOT: ${DQM_OUT_DIR}/dataTree/${runName%%.*}.root"


echo "===> Producing DQM plots"
${DQM_HOME}/makeDqmPlots.py -d ${DQM_HOME} -m ${PLOT_MACRO} -i ${DQM_OUT_DIR}/dataTree/${runName%%.*}.root -o  ${DQM_OUT_DIR}/dqmOut -p  ${DQM_OUT_DIR}/dqmPlots 2>&1 >  ${DQM_OUT_DIR}/log/dqmAnalysis_${runName%%.*}.log 
echo "DQM output histograms available at ${DQM_OUT_DIR}/dqmOut"
echo "DQM output plots available at ${DQM_OUT_DIR}/dqmPlots"