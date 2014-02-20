#define drawRawData_cxx
#include "drawRawData.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>
void drawRawData::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L drawRawData.C
//      Root > drawRawData t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch

  TH1F* h_adcSpectra[NUM_ADC_CHANNELS];
  for (int i=0;i<NUM_ADC_CHANNELS;++i)
    {
      h_adcSpectra[i]=new TH1F(Form("adcSpectra_%d",i),Form("adcSpectra_%d",i),4096,-0.5,4095.5);
      outObjects[TString("ADC_")+TString(h_adcSpectra[i]->GetName())]=(TObject*)h_adcSpectra[i]; 
    }

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      for (int i=0;i<NUM_ADC_CHANNELS;++i)
	  h_adcSpectra[adcChannel[i]]->Fill(adcData[i]);
      // if (Cut(ientry) < 0) continue;
   }

   TFile *fOut=new TFile(outFile.c_str(),"RECREATE");
   if (!fOut || !fOut->IsOpen()) {
     std::cout << "outFile " << outFile << " cannot be  opened. Giving up" << std::endl;
     return;
   }
   std::cout << "outFile " << outFile << " opened" << std::endl;
   fOut->cd();
   for (std::map<TString,TObject*>::const_iterator out=outObjects.begin();out!=outObjects.end();++out)
     out->second->Write(out->first);
   fOut->Close();
   std::cout << "outFile " << outFile << " closed" << std::endl;
}
