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
parser.add_option("-m","--macro")
parser.add_option("-d","--dir") # DQM_HOME directory
parser.add_option("-o","--outputDir")
parser.add_option("-p","--plotsDir")

(options,args)=parser.parse_args()
import ROOT as r
r.gROOT.SetBatch(1)
r.gStyle.SetOptStat(111111)
#r.gStyle.SetOptTitle(0)

#r.gROOT.ProcessLine(".L drawRawData.C+")
#r.gROOT.ProcessLine(".L  fastDQM_CeF3_BTF.C+")
r.gROOT.ProcessLine(".L  "+str(options.dir)+"/"+str(options.macro)+".C+")
file = r.TFile.Open(options.inputFile)

if (not file.IsOpen()):
    print "Cannot open "+ options.inputFile
    exit(-1)

tree = file.Get("eventRawData")
os.system('mkdir -p %s'%options.outputDir)
a=eval("r."+str(options.macro)+"(tree)")
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
os.system('cp %s/index.php %s'%(str(options.dir),plotDir))

r.gStyle.SetPalette(55) #Dark Body Radiator color map in linear scale
r.gStyle.SetNumberContours(255)

for t in types:
    #GO+1
    matrix=0
    c=r.TCanvas(str(t),str(t),1500,1000)
    wavedumps=0
    if (t.find("WAVEDUMP") != -1):
        wavedumps=1
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
        if (wavedumps):
            r.gPad.SetLogz()
            histo.SetStats(0)
            histo.Draw("COLZ")
        else:
            if ( (t.find("HODOXCOR") != -1) or
                 (t.find("HODOXRAW") != -1) or
                 (t.find("HODOYRAW") != -1) or
                 (t.find("HODOYCOR") != -1) or
                 (t.find("TIMEDIFF") != -1) or
                 (t.find("TDCVALRAW") != -1) or
                 (t == "SCINT" )
                 ):
                r.gPad.SetLogy()
            if ( (t.find("TIMEPROFILE") != -1) ):
                histo.SetMarkerStyle(20)
                histo.SetMarkerSize(1.2)
                histo.SetLineColor(r.kBlack)
                histo.SetMarkerColor(r.kBlack)
            histo.Draw()
    c.SaveAs("%s/%s.png"%(plotDir,str(t)))
    
