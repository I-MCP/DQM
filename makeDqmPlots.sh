#!/bin/sh

if [ -z "$ROOTSYS" ]; then
    echo "Please source root environment: source $HOME/root/bin/thisroot.sh"
    exit 1
else
    echo "ROOT already configured"
fi
 
runFile=$1
runName=`basename $runFile`

mkdir -p dataTree
mkdir -p log

echo "Running DQM analysis on $runFile"
bin/readBinary -f $1 -o dataTree/${runName%%.*}.root 2>&1 > log/convertRawData_${runName%%.*}.log 
./makeDqmPlots.py -i dataTree/${runName%%.*}.root -o dqmOut -p dqmPlots 2>&1 > log/dqmAnalysis_${runName%%.*}.log 