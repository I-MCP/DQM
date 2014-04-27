#!/bin/sh

if [ -z "$ROOTSYS" ]; then
    echo "Please source root environment: source $HOME/root/bin/thisroot.sh"
    exit 1
else
    echo "ROOT already configured"
fi
 
runFile=$1
runName=`basename $runFile`

echo $runFile ${runName%%.*}.root
bin/readBinary -f $1 -o ${runName%%.*}.root
./makeDqmPlots.py -i  ${runName%%.*}.root -o dqmOut