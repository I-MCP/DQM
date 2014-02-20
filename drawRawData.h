//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb 20 23:56:33 2014 by ROOT version 5.34/12
// from TTree eventRawData/VME event RAW Data
// found on file: run0000021_20140220.root
//////////////////////////////////////////////////////////

#ifndef drawRawData_h
#define drawRawData_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>

#define NUM_ADC_CHANNELS 32
// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class drawRawData {
public :
  std::map<TString,TObject*> outObjects;
  std::string          outFile;
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          evtNumber;
   UInt_t          evtTimeDist;
   UInt_t          evtTimeStart;
   UInt_t          evtTime;
   UInt_t          boardTriggerBit;
   UInt_t          triggerWord;
   UInt_t          nAdcChannels;
   UInt_t          adcBoard[NUM_ADC_CHANNELS];   //[nAdcChannels]
   UInt_t          adcChannel[NUM_ADC_CHANNELS];   //[nAdcChannels]
   UInt_t          adcData[NUM_ADC_CHANNELS];   //[nAdcChannels]
   UInt_t          nTdcChannels;
   UInt_t          tdcBoard[1];   //[nTdcChannels]
   UInt_t          tdcChannel[1];   //[nTdcChannels]
   UInt_t          tdcData[1];   //[nTdcChannels]

   // List of branches
   TBranch        *b_evtNumber;   //!
   TBranch        *b_evtTimeDist;   //!
   TBranch        *b_evtTimeStart;   //!
   TBranch        *b_evtTime;   //!
   TBranch        *b_boardTriggerBit;   //!
   TBranch        *b_triggerWord;   //!
   TBranch        *b_nAdcChannels;   //!
   TBranch        *b_adcBoard;   //!
   TBranch        *b_adcChannel;   //!
   TBranch        *b_adcData;   //!
   TBranch        *b_nTdcChannels;   //!
   TBranch        *b_tdcBoard;   //!
   TBranch        *b_tdcChannel;   //!
   TBranch        *b_tdcData;   //!

   drawRawData(TTree *tree=0);
   virtual ~drawRawData();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef drawRawData_cxx
drawRawData::drawRawData(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("run0000021_20140220.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("run0000021_20140220.root");
      }
      f->GetObject("eventRawData",tree);

   }
   Init(tree);
}

drawRawData::~drawRawData()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t drawRawData::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t drawRawData::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void drawRawData::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("evtNumber", &evtNumber, &b_evtNumber);
   fChain->SetBranchAddress("evtTimeDist", &evtTimeDist, &b_evtTimeDist);
   fChain->SetBranchAddress("evtTimeStart", &evtTimeStart, &b_evtTimeStart);
   fChain->SetBranchAddress("evtTime", &evtTime, &b_evtTime);
   fChain->SetBranchAddress("boardTriggerBit", &boardTriggerBit, &b_boardTriggerBit);
   fChain->SetBranchAddress("triggerWord", &triggerWord, &b_triggerWord);
   fChain->SetBranchAddress("nAdcChannels", &nAdcChannels, &b_nAdcChannels);
   fChain->SetBranchAddress("adcBoard", adcBoard, &b_adcBoard);
   fChain->SetBranchAddress("adcChannel", adcChannel, &b_adcChannel);
   fChain->SetBranchAddress("adcData", adcData, &b_adcData);
   fChain->SetBranchAddress("nTdcChannels", &nTdcChannels, &b_nTdcChannels);
   fChain->SetBranchAddress("tdcBoard", &tdcBoard, &b_tdcBoard);
   fChain->SetBranchAddress("tdcChannel", &tdcChannel, &b_tdcChannel);
   fChain->SetBranchAddress("tdcData", &tdcData, &b_tdcData);
   Notify();
}

Bool_t drawRawData::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void drawRawData::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t drawRawData::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef drawRawData_cxx
