#!/usr/bin/env python   

import sys
import os
from array import *

import ntpath
def path_leaf(path):
    head, tail = ntpath.split(path)
    return tail or ntpath.basename(head)

from optparse import OptionParser
parser=OptionParser()
parser.add_option("-i","--inputFile")
parser.add_option("-o","--outputDir")
parser.add_option("-p","--plotsDir")

(options,args)=parser.parse_args()
import ROOT as r
r.gROOT.SetBatch(1)
#r.gStyle.SetOptStat(0)
#r.gStyle.SetOptTitle(0)

#r.gROOT.ProcessLine(".L drawRawData.C+")
r.gROOT.ProcessLine(".L  fastDQM_CeF3_BTF.C+")
file = r.TFile.Open(options.inputFile)

if (not file.IsOpen()):
    print "Cannot open "+ options.inputFile
    exit(-1)

tree = file.Get("eventRawData")
os.system('mkdir -p %s'%options.outputDir)
a=r.fastDQM_CeF3_BTF(tree)
a.outFile=options.outputDir+"/"+path_leaf(options.inputFile)+"_dqmPlots.root"
a.Loop()

outFile = r.TFile.Open(options.outputDir+"/"+path_leaf(options.inputFile)+"_dqmPlots.root")

if (not outFile.IsOpen()):
    print "Cannot open "+ options.inputFile
    exit(-1)

types = {}
for key in outFile.GetListOfKeys():
    name = key.GetName()
    if (len(name.split('_'))>1):
        type=name.split('_')[0]
        if type not in types.keys():
            types[type]=[]
        types[type].append(name)
#print types

os.system('mkdir -p %s'%options.plotsDir)
#c=r.TCanvas("c","c",1500,900)
#c.Divide(8,4,0,0)
#for i in range(1,33):
#    c.cd(i)
#    outFile.Get("ADC_adcSpectra_%d"%(i-1)).Draw()
#c.SaveAs(options.outputFile+".png")
#    
