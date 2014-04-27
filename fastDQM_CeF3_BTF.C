#define fastDQM_CeF3_BTF_cxx
#include "fastDQM_CeF3_BTF.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>

float bgoIntercalibration[BGO_CHANNELS] = { 1., 1., 1., 1., 1., 1., 1., 1.};

float cef3Intercalibration[BGO_CHANNELS] = { 1., 1., 1., 1.};

float hodoXIntercalibration[HODOX_CHANNELS] = { 1., 1., 1., 1., 1., 1., 1., 1.};
float hodoYIntercalibration[HODOY_CHANNELS] = { 1., 1., 1., 1., 1., 1., 1., 1.};

float centerXTaggerIntercalibration[CENTERX_TAGGER_CHANNELS] = { 1., 1.};
float centerYTaggerIntercalibration[CENTERY_TAGGER_CHANNELS] = { 1.};

void fastDQM_CeF3_BTF::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L fastDQM_CeF3_BTF.C
//      Root > fastDQM_CeF3_BTF t
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


  //BGO histograms
  TH1F* h_bgoRawSpectrum[BGO_CHANNELS];
  for (int i=0;i<BGO_CHANNELS;++i)
    {
      h_bgoRawSpectrum[i]=new TH1F(Form("bgoRawSpectrum_%d",i),Form("bgoRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("ADC_")+TString(h_bgoRawSpectrum[i]->GetName())]=(TObject*)h_bgoRawSpectrum[i]; 
    }

  TH1F* h_bgoEnergy;
  h_bgoEnergy=new TH1F("bgoEnergy","bgoEnergy",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_bgoEnergy->GetName())]=(TObject*)h_bgoEnergy; 

  TH1F* h_bgoEnergyProfile;
  h_bgoEnergyProfile=new TH1F("bgoEnergyProfile","bgoEnergyProfile",BGO_CHANNELS,-0.5,BGO_CHANNELS-0.5);
  outObjects[TString("ADC_")+TString(h_bgoEnergyProfile->GetName())]=(TObject*)h_bgoEnergyProfile; 

  TH1F* h_bgoEnergyUp;  
  h_bgoEnergyUp=new TH1F("bgoEnergyUp","bgoEnergyUp",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_bgoEnergyUp->GetName())]=(TObject*)h_bgoEnergyUp; 

  TH1F* h_bgoEnergyDown;
  h_bgoEnergyDown=new TH1F("bgoEnergyDown","bgoEnergyDown",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_bgoEnergyUp->GetName())]=(TObject*)h_bgoEnergyDown; 

  TH1F* h_bgoEnergyLeft;
  h_bgoEnergyLeft=new TH1F("bgoEnergyLeft","bgoEnergyLeft",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_bgoEnergyLeft->GetName())]=(TObject*)h_bgoEnergyLeft; 

  TH1F* h_bgoEnergyRight;
  h_bgoEnergyRight=new TH1F("bgoEnergyRight","bgoEnergyRight",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_bgoEnergyUp->GetName())]=(TObject*)h_bgoEnergyRight; 

  //CeF3 histograms
  TH1F* h_cef3RawSpectrum[CEF3_CHANNELS];
  for (int i=0;i<CEF3_CHANNELS;++i)
    {
      h_cef3RawSpectrum[i]=new TH1F(Form("cef3RawSpectrum_%d",i),Form("cef3RawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("ADC_")+TString(h_cef3RawSpectrum[i]->GetName())]=(TObject*)h_cef3RawSpectrum[i]; 
    }

  TH1F* h_cef3Energy;
  h_cef3Energy=new TH1F("cef3Energy","cef3Energy",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_cef3Energy->GetName())]=(TObject*)h_cef3Energy; 

  TH1F* h_cef3EnergyProfile;
  h_cef3EnergyProfile=new TH1F("cef3EnergyProfile","cef3EnergyProfile",CEF3_CHANNELS,-0.5,CEF3_CHANNELS-0.5);
  outObjects[TString("ADC_")+TString(h_cef3EnergyProfile->GetName())]=(TObject*)h_cef3EnergyProfile; 

  //Full calo energy
  TH1F* h_caloEnergy;
  h_caloEnergy=new TH1F("caloEnergy","caloEnergy",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_caloEnergy->GetName())]=(TObject*)h_caloEnergy; 

  //HodoX histograms
  TH1F* h_hodoXRawSpectrum[HODOX_CHANNELS];
  for (int i=0;i<HODOX_CHANNELS;++i)
    {
      h_hodoXRawSpectrum[i]=new TH1F(Form("hodoXRawSpectrum_%d",i),Form("hodoXRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("ADC_")+TString(h_hodoXRawSpectrum[i]->GetName())]=(TObject*)h_hodoXRawSpectrum[i]; 
    }

  TH1F* h_hodoXEnergy;
  h_hodoXEnergy=new TH1F("hodoXEnergy","hodoXEnergy",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_hodoXEnergy->GetName())]=(TObject*)h_hodoXEnergy; 

  TH1F* h_hodoXEnergyProfile;
  h_hodoXEnergyProfile=new TH1F("hodoXEnergyProfile","hodoXEnergyProfile",HODOX_CHANNELS,-0.5,HODOX_CHANNELS-0.5);
  outObjects[TString("ADC_")+TString(h_hodoXEnergyProfile->GetName())]=(TObject*)h_hodoXEnergyProfile; 

  //HodoY histograms
  TH1F* h_hodoYRawSpectrum[HODOY_CHANNELS];
  for (int i=0;i<HODOY_CHANNELS;++i)
    {
      h_hodoYRawSpectrum[i]=new TH1F(Form("hodoYRawSpectrum_%d",i),Form("hodoYRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("ADC_")+TString(h_hodoYRawSpectrum[i]->GetName())]=(TObject*)h_hodoYRawSpectrum[i]; 
    }

  TH1F* h_hodoYEnergy;
  h_hodoYEnergy=new TH1F("hodoYEnergy","hodoYEnergy",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_hodoYEnergy->GetName())]=(TObject*)h_hodoYEnergy; 

  TH1F* h_hodoYEnergyProfile;
  h_hodoYEnergyProfile=new TH1F("hodoYEnergyProfile","hodoYEnergyProfile",HODOY_CHANNELS,-0.5,HODOY_CHANNELS-0.5);
  outObjects[TString("ADC_")+TString(h_hodoYEnergyProfile->GetName())]=(TObject*)h_hodoYEnergyProfile; 

  //CenterX tagger histograms
  TH1F* h_centerXTaggerRawSpectrum[CENTERX_TAGGER_CHANNELS];
  for (int i=0;i<CENTERX_TAGGER_CHANNELS;++i)
    {
      h_centerXTaggerRawSpectrum[i]=new TH1F(Form("centerXTaggerRawSpectrum_%d",i),Form("centerXTaggerRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("ADC_")+TString(h_centerXTaggerRawSpectrum[i]->GetName())]=(TObject*)h_centerXTaggerRawSpectrum[i]; 
    }

  TH1F* h_centerXTaggerEnergy;
  h_centerXTaggerEnergy=new TH1F("centerXTaggerEnergy","centerXTaggerEnergy",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_centerXTaggerEnergy->GetName())]=(TObject*)h_centerXTaggerEnergy; 

  TH1F* h_centerXTaggerEnergyProfile;
  h_centerXTaggerEnergyProfile=new TH1F("centerXTaggerEnergyProfile","centerXTaggerEnergyProfile",CENTERX_TAGGER_CHANNELS,-0.5,CENTERX_TAGGER_CHANNELS-0.5);
  outObjects[TString("ADC_")+TString(h_centerXTaggerEnergyProfile->GetName())]=(TObject*)h_centerXTaggerEnergyProfile; 

  //CenterY tagger histograms
  TH1F* h_centerYTaggerRawSpectrum[CENTERY_TAGGER_CHANNELS];
  for (int i=0;i<CENTERY_TAGGER_CHANNELS;++i)
    {
      h_centerYTaggerRawSpectrum[i]=new TH1F(Form("centerYTaggerRawSpectrum_%d",i),Form("centerYTaggerRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("ADC_")+TString(h_centerYTaggerRawSpectrum[i]->GetName())]=(TObject*)h_centerYTaggerRawSpectrum[i]; 
    }

  TH1F* h_centerYTaggerEnergy;
  h_centerYTaggerEnergy=new TH1F("centerYTaggerEnergy","centerYTaggerEnergy",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_centerYTaggerEnergy->GetName())]=(TObject*)h_centerYTaggerEnergy; 

  TH1F* h_centerYTaggerEnergyProfile;
  h_centerYTaggerEnergyProfile=new TH1F("centerYTaggerEnergyProfile","centerYTaggerEnergyProfile",CENTERY_TAGGER_CHANNELS,-0.5,CENTERY_TAGGER_CHANNELS-0.5);
  outObjects[TString("ADC_")+TString(h_centerYTaggerEnergyProfile->GetName())]=(TObject*)h_centerYTaggerEnergyProfile; 

  //Scintillator histograms
  TH1F* h_scintFrontRawSpectrum;
  h_scintFrontRawSpectrum=new TH1F("scintFrontRawSpectrum","scintFrontRawSpectrum",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_scintFrontRawSpectrum->GetName())]=(TObject*)h_scintFrontRawSpectrum; 

  TH1F* h_scintBackRawSpectrum;
  h_scintBackRawSpectrum=new TH1F("scintBackRawSpectrum","scintBackRawSpectrum",4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_scintBackRawSpectrum->GetName())]=(TObject*)h_scintBackRawSpectrum; 

  //Correlation plots
  TH2F* h_bgo_vs_cef3;
  h_bgo_vs_cef3=new TH2F("bgo_vs_cef3","bgo_vs_cef3",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_bgo_vs_cef3->GetName())]=(TObject*)h_bgo_vs_cef3; 

  TH2F* h_hodoX_vs_calo;
  h_hodoX_vs_calo=new TH2F("hodoX_vs_calo","hodoX_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_hodoX_vs_calo->GetName())]=(TObject*)h_hodoX_vs_calo; 

  TH2F* h_hodoY_vs_calo;
  h_hodoY_vs_calo=new TH2F("hodoY_vs_calo","hodoY_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_hodoY_vs_calo->GetName())]=(TObject*)h_hodoY_vs_calo; 

  TH2F* h_centerXTagger_vs_calo;
  h_centerXTagger_vs_calo=new TH2F("centerXTagger_vs_calo","centerXTagger_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_centerXTagger_vs_calo->GetName())]=(TObject*)h_centerXTagger_vs_calo; 

  TH2F* h_centerYTagger_vs_calo;
  h_centerYTagger_vs_calo=new TH2F("centerYTagger_vs_calo","centerYTagger_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_centerYTagger_vs_calo->GetName())]=(TObject*)h_centerYTagger_vs_calo; 

  TH2F* h_scintFront_vs_calo;
  h_scintFront_vs_calo=new TH2F("scintFront_vs_calo","scintFront_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_scintFront_vs_calo->GetName())]=(TObject*)h_scintFront_vs_calo; 

  TH2F* h_scintBack_vs_calo;
  h_scintBack_vs_calo=new TH2F("scintBack_vs_calo","scintBack_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("ADC_")+TString(h_scintBack_vs_calo->GetName())]=(TObject*)h_scintBack_vs_calo; 

  //Stability plots (1 point every 5 minutes)


  std::cout << "==================== Booked histograms =======================" << std::endl;
   for (std::map<TString,TObject*>::const_iterator out=outObjects.begin();out!=outObjects.end();++out)
     std::cout << out->second->GetName() << std::endl;
  std::cout << "==================== Loop over events =======================" << std::endl;
   

  if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;

   unsigned int startTimeStamp=0;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (jentry%1000==0)
	std::cout << "Analysing event " << jentry << std::endl;

      if (jentry==0)
	{
	  startTimeStamp=evtTime;
	  std::cout << "First event taken @ timestamp (msec from 01/04/2014)" <<  startTimeStamp << std::endl;
	}
      if (startTimeStamp!=0)
	std::cout << evtTime - startTimeStamp << std::endl;


      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      float bgoEnergy=0,bgoLeftEnergy=0,bgoRightEnergy=0,bgoUpEnergy=0,bgoDownEnergy=0,
	cef3Energy=0,caloEnergy=0,
	hodoXEnergy=0,hodoYEnergy=0,centerXTaggerEnergy=0,centerYTaggerEnergy=0,scintFrontEnergy=0,scintBackEnergy=0;

      //Loop over adc channels
      for (int i=0;i<NUM_ADC_CHANNELS;++i)
	{
	  if (adcBoard[i]==BGO_ADC_BOARD && (adcChannel[i]>=BGO_ADC_START_CHANNEL && adcChannel[i]<=BGO_ADC_END_CHANNEL) )
	    {
	      bgoEnergy+=adcData[i]*bgoIntercalibration[adcChannel[i]-BGO_ADC_START_CHANNEL];
	      h_bgoRawSpectrum[adcChannel[i]-BGO_ADC_START_CHANNEL]->Fill(adcData[i]);
	    }

	  else if (adcBoard[i]==CEF3_ADC_BOARD && (adcChannel[i]>=CEF3_ADC_START_CHANNEL && adcChannel[i]<=CEF3_ADC_END_CHANNEL) )
	    {
	      cef3Energy+=adcData[i]*cef3Intercalibration[adcChannel[i]-CEF3_ADC_START_CHANNEL];
	      h_cef3RawSpectrum[adcChannel[i]-CEF3_ADC_START_CHANNEL]->Fill(adcData[i]);
	    }

	  else if (adcBoard[i]==HODOX_ADC_BOARD && (adcChannel[i]>=HODOX_ADC_START_CHANNEL && adcChannel[i]<=HODOX_ADC_END_CHANNEL) )
	    {
	      hodoXEnergy+=adcData[i]*hodoXIntercalibration[adcChannel[i]-HODOX_ADC_START_CHANNEL];
	      h_hodoXRawSpectrum[adcChannel[i]-HODOX_ADC_START_CHANNEL]->Fill(adcData[i]);
	    }

	  else if (adcBoard[i]==HODOY_ADC_BOARD && (adcChannel[i]>=HODOY_ADC_START_CHANNEL && adcChannel[i]<=HODOY_ADC_END_CHANNEL) )
	    {
	      hodoYEnergy+=adcData[i]*hodoYIntercalibration[adcChannel[i]-HODOY_ADC_START_CHANNEL];
	      h_hodoYRawSpectrum[adcChannel[i]-HODOY_ADC_START_CHANNEL]->Fill(adcData[i]);
	    }

	  else if (adcBoard[i]==CENTERX_TAGGER_ADC_BOARD && (adcChannel[i]>=CENTERX_TAGGER_ADC_START_CHANNEL && adcChannel[i]<=CENTERX_TAGGER_ADC_END_CHANNEL) )
	    {
	      centerXTaggerEnergy+=adcData[i]*centerXTaggerIntercalibration[adcChannel[i]-CENTERX_TAGGER_ADC_START_CHANNEL];
	      h_centerXTaggerRawSpectrum[adcChannel[i]-CENTERX_TAGGER_ADC_START_CHANNEL]->Fill(adcData[i]);
	    }

	  else if (adcBoard[i]==CENTERY_TAGGER_ADC_BOARD && (adcChannel[i]>=CENTERY_TAGGER_ADC_START_CHANNEL && adcChannel[i]<=CENTERY_TAGGER_ADC_END_CHANNEL) )
	    {
	      centerYTaggerEnergy+=adcData[i]*centerYTaggerIntercalibration[adcChannel[i]-CENTERY_TAGGER_ADC_START_CHANNEL];
	      h_centerYTaggerRawSpectrum[adcChannel[i]-CENTERY_TAGGER_ADC_START_CHANNEL]->Fill(adcData[i]);
	    }
	  else if (adcBoard[i]==SCINT_FRONT_ADC_BOARD && adcChannel[i]==SCINT_FRONT_ADC_CHANNEL )
	    {
	      scintFrontEnergy=adcData[i];
	      h_scintFrontRawSpectrum->Fill(adcData[i]);
	    }
	  else if (adcBoard[i]==SCINT_BACK_ADC_BOARD && adcChannel[i]==SCINT_BACK_ADC_CHANNEL )
	    {
	      scintBackEnergy=adcData[i];
	      h_scintBackRawSpectrum->Fill(adcData[i]);
	    }
	  else
	    {
	      //unused channel
	    }
	}

      caloEnergy=bgoEnergy+cef3Energy;
      //Fill energy plots
      h_bgoEnergy->Fill(bgoEnergy);
      h_cef3Energy->Fill(cef3Energy);
      h_caloEnergy->Fill(caloEnergy);
      h_hodoXEnergy->Fill(hodoXEnergy);
      h_hodoYEnergy->Fill(hodoYEnergy);
      h_centerXTaggerEnergy->Fill(centerXTaggerEnergy);
      h_centerYTaggerEnergy->Fill(centerYTaggerEnergy);

      //Fill correlation plots
      h_bgo_vs_cef3->Fill(cef3Energy,bgoEnergy);
      h_hodoX_vs_calo->Fill(caloEnergy,hodoXEnergy);
      h_hodoY_vs_calo->Fill(caloEnergy,hodoYEnergy);
      h_centerXTagger_vs_calo->Fill(caloEnergy,centerXTaggerEnergy);
      h_centerYTagger_vs_calo->Fill(caloEnergy,centerYTaggerEnergy);
      h_scintFront_vs_calo->Fill(caloEnergy,scintFrontEnergy);
      h_scintBack_vs_calo->Fill(caloEnergy,scintBackEnergy);
   }

   //Fill profile histograms
   for (unsigned int i(0); i<BGO_CHANNELS;++i)
     {
       float mean=h_bgoRawSpectrum[i]->GetMean();
       float rms=h_bgoRawSpectrum[i]->GetRMS();
       h_bgoEnergyProfile->SetBinContent(i+1,mean);
       h_bgoEnergyProfile->SetBinError(i+1,rms);
     }


   for (unsigned int i(0); i<CEF3_CHANNELS;++i)
     {
       float mean=h_cef3RawSpectrum[i]->GetMean();
       float rms=h_cef3RawSpectrum[i]->GetRMS();
       h_cef3EnergyProfile->SetBinContent(i+1,mean);
       h_cef3EnergyProfile->SetBinError(i+1,rms);
     }

   for (unsigned int i(0); i<HODOX_CHANNELS;++i)
     {
       float mean=h_hodoXRawSpectrum[i]->GetMean();
       float rms=h_hodoXRawSpectrum[i]->GetRMS();
       h_hodoXEnergyProfile->SetBinContent(i+1,mean);
       h_hodoXEnergyProfile->SetBinError(i+1,rms);
     }


   for (unsigned int i(0); i<HODOY_CHANNELS;++i)
     {
       float mean=h_hodoYRawSpectrum[i]->GetMean();
       float rms=h_hodoYRawSpectrum[i]->GetRMS();
       h_hodoYEnergyProfile->SetBinContent(i+1,mean);
       h_hodoYEnergyProfile->SetBinError(i+1,rms);
     }


   for (unsigned int i(0); i<CENTERX_TAGGER_CHANNELS;++i)
     {
       float mean=h_centerXTaggerRawSpectrum[i]->GetMean();
       float rms=h_centerXTaggerRawSpectrum[i]->GetRMS();
       h_centerXTaggerEnergyProfile->SetBinContent(i+1,mean);
       h_centerXTaggerEnergyProfile->SetBinError(i+1,rms);
     }


   for (unsigned int i(0); i<CENTERY_TAGGER_CHANNELS;++i)
     {
       float mean=h_centerYTaggerRawSpectrum[i]->GetMean();
       float rms=h_centerYTaggerRawSpectrum[i]->GetRMS();
       h_centerYTaggerEnergyProfile->SetBinContent(i+1,mean);
       h_centerYTaggerEnergyProfile->SetBinError(i+1,rms);
     }

   TFile *fOut=new TFile(outFile.c_str(),"RECREATE");
   if (!fOut || !fOut->IsOpen()) {
     std::cout << "outFile " << outFile << " cannot be  opened. Giving up" << std::endl;
     return;
   }
   std::cout << "==================== Saving histograms =======================" << std::endl;
   std::cout << "outFile " << outFile << " opened" << std::endl;
   fOut->cd();
   for (std::map<TString,TObject*>::const_iterator out=outObjects.begin();out!=outObjects.end();++out)
     out->second->Write(out->first);
   fOut->Close();
   std::cout << "outFile " << outFile << " closed" << std::endl;
   std::cout << "==================== DQM analysis is done =======================" << std::endl;
}
