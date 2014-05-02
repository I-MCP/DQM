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
a.outFile=options.outputDir+"/"+os.path.splitext(path_leaf(options.inputFile))[0]+"_dqmPlots.root"
a.Loop()

outFile = r.TFile.Open(a.outFile)

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
print types

plotDir='%s/%s'%(options.plotsDir,os.path.splitext(path_leaf(options.inputFile))[0])
os.system('mkdir -p %s'%plotDir)
os.system('cp index.php %s'%plotDir)
#GO+1
matrix=0
for t in types:
    c=r.TCanvas(str(t),str(t),1500,1000)
    if len(types[t])<3:
        c.Divide(1,2,0,0)
    elif len(types[t])<5:
        c.Divide(2,2,0,0)
    elif len(types[t])<7:
        c.Divide(2,3,0,0)
    elif len(types[t])<10:
        c.Divide(3,3,0,0)
        #GO+1
        if (t == "BGORAW"): matrix=1
    else:
        c.Divide(4,4,0,0)
        
    for i in range(1,len(types[t])+1):
        c.cd(i)
        #GO+1
        if ((i>=5) & (matrix==1)): c.cd(i+1)
        r.gPad.SetBottomMargin(1.2)
        r.gPad.SetLeftMargin(1.2)
        r.gPad.SetRightMargin(1.2)
        histo = outFile.Get(types[t][i-1])
        histo.SetLineColor(r.kRed)
        histo.SetLineWidth(2)
        histo.Draw()
    c.SaveAs("%s/%s.png"%(plotDir,str(t)))
    
