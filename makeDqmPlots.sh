#!/bin/sh

DQM_HOME=/home/cmsdaq/DQM
DQM_OUT_DIR=/home/cmsdaq/DQM
PLOT_MACRO=fastDQM_CeF3_BTF

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
    echo "Sourcing DQM config $2"
    . $2
fi

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