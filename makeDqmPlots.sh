#!/bin/sh

if [ $# -lt 1 ]; then
    echo "Usage: makeDqmPlots.sh <raw binary file> <DQM_config>"
    exit
fi

if [ -z "$ROOTSYS" ]; then
    source /home/cmsdaq/root/bin/thisroot.sh
    echo "Please source root environment: source /home/cmsdaq/root/bin/thisroot.sh"
    #exit 1
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


mkdir -p ${DATATREE_DIR}
mkdir -p ${DQM_OUT_DIR}/log

rootFile=${DATATREE_DIR}/${runName%%.*}.root
convLogFile=${DQM_OUT_DIR}/log/convertRawData_${runName%%.*}.log


echo "============================================================="
echo "       Running DQM analysis on $runFile"
echo "============================================================="


echo "===> Converting $runFile in ROOT format at $rootFile"
${DQM_HOME}/bin/readBinary -f $1 -o ${rootFile} 2>&1 > ${convLogFile} 
echo "Converted $runFile into ROOT: ${rootFile}"

dqmLogFile=${DQM_OUT_DIR}/log/dqmAnalysis_${runName%%.*}.log

echo "===> Producing DQM plots"
${DQM_HOME}/makeDqmPlots.py -d ${DQM_HOME} -m ${PLOT_MACRO} -i ${rootFile} -o  ${DQM_OUT_DIR}/dqmOut -p  ${DQM_OUT_DIR}/dqmPlots 2>&1 >  ${dqmLogFile}
echo "DQM output histograms available at ${DQM_OUT_DIR}/dqmOut"
echo "DQM output plots available at ${DQM_OUT_DIR}/dqmPlots"

chmod -R uog+w $rootFile 
chmod uog+w $convLogFile $dqmLogFile

echo "Now look at the plots at the following link"

