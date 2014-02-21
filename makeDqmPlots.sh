#!/bin/sh

#source $HOME/root/bin/thisroot.sh
 
runFile=$1
runName=`basename $runFile`

echo $runFile ${runName%%.*}.root
bin/readBinary -f $1 -o ${runName%%.*}.root -7 #use -7 if the file is taken with the ADC792
./makeDqmPlots.py -i  ${runName%%.*}.root -o  ${runName%%.*}_dqmPlots