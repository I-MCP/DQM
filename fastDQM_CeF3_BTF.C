#define fastDQM_CeF3_BTF_cxx
#include "fastDQM_CeF3_BTF.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>

#include <iostream>

float bgoIntercalibration[BGO_CHANNELS] = { 1., 1., 1., 1., 1., 1., 1., 1.};

float cef3Intercalibration[BGO_CHANNELS] = { 1., 1., 1., 1.};

float hodoXIntercalibration[HODOX_CHANNELS] = { 1., 1., 1., 1., 1., 1., 1., 1.};
float hodoYIntercalibration[HODOY_CHANNELS] = { 1., 1., 1., 1., 1., 1., 1., 1.};

float centerXTaggerIntercalibration[CENTERX_TAGGER_CHANNELS] = { 1., 1.};
float centerYTaggerIntercalibration[CENTERY_TAGGER_CHANNELS] = { 1.};

// These channel numbers HAVE TO BE CHECKED! Left is defined looking at the matrix from the beam side
bool isInBGOUpRow(int bgoChannel)
{
  return (bgoChannel==0 || bgoChannel==1 ||bgoChannel==2);
}

bool isInBGODownRow(int bgoChannel)
{
  return (bgoChannel==6 || bgoChannel==7 ||bgoChannel==8);
}

bool isInBGORightRow(int bgoChannel)
{
  return (bgoChannel==0 || bgoChannel==3 ||bgoChannel==5);
}

bool isInBGOLeftRow(int bgoChannel)
{
  return (bgoChannel==2 || bgoChannel==4 ||bgoChannel==7);
}

// 24h hours 5min intervals
#define TIME_MAX_INTERVALS 248  
#define TIME_INTERVAL_SIZE 5*60*1000 //interval size is 5min expressed in msec 

// A simple struct to simplify time average code
struct timeAverage
{
  double timeAverage_array[TIME_MAX_INTERVALS];
  double timeRms_array[TIME_MAX_INTERVALS];
  unsigned int   nMeasures_per_interval[TIME_MAX_INTERVALS];
  
  timeAverage()
  {
    for (unsigned int i(0);i<TIME_MAX_INTERVALS;++i)
      {
	timeAverage_array[i]=0;
	timeRms_array[i]=0;
	nMeasures_per_interval[i]=0;
      }
  }

  void addMeasure(unsigned int interval, float value)
  {
    if (interval<TIME_MAX_INTERVALS)
      {
	timeAverage_array[interval]+=value;
	timeRms_array[interval]+=value*value;
	nMeasures_per_interval[interval]++;
      }
    else
      {
	std::cout << "ERROR! Adding measurement at time " << interval << " which is outside the TIME_MAX_INTERVALS" << std::endl;
      }
  }

  void calculateAverages()
  {
    for(unsigned int i(0);i<TIME_MAX_INTERVALS;++i)
      {
	if (nMeasures_per_interval[i]>0)
	  {
	    timeAverage_array[i]=timeAverage_array[i]/(float)nMeasures_per_interval[i];
	    timeRms_array[i]=TMath::Sqrt((timeRms_array[i]/(float)nMeasures_per_interval[i])-timeAverage_array[i]*timeAverage_array[i]);
	  }
      }
  }
};

void fillTimeProfile(const timeAverage& timeAvg, TH1F* timeProfile)
{
  for (unsigned int i(0);i<TIME_MAX_INTERVALS;++i)
    {
      if (timeAvg.nMeasures_per_interval[i]==0)
	continue;

      timeProfile->SetBinContent(i+1,timeAvg.timeAverage_array[i]);
      timeProfile->SetBinError(i+1,timeAvg.timeRms_array[i]);
    }
}

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


  float nTriggers[TIME_MAX_INTERVALS];
  for (unsigned int i(0);i<TIME_MAX_INTERVALS;++i)
    nTriggers[i]=0;

  timeAverage rate_TimeAverage;

  //BGO histograms
  TH1F* h_bgoRawSpectrum[BGO_CHANNELS];
  for (int i=0;i<BGO_CHANNELS;++i)
    {
      h_bgoRawSpectrum[i]=new TH1F(Form("bgoRawSpectrum_%d",i),Form("bgoRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("BGORAW_")+TString(h_bgoRawSpectrum[i]->GetName())]=(TObject*)h_bgoRawSpectrum[i]; 
    }

  TH1F* h_bgoEnergy;
  h_bgoEnergy=new TH1F("bgoEnergy","bgoEnergy",4096,-0.5,4095.5);
  outObjects[TString("BGO_")+TString(h_bgoEnergy->GetName())]=(TObject*)h_bgoEnergy; 

  timeAverage bgoEnergy_TimeAverage;

  TH1F* h_bgoEnergyProfile;
  h_bgoEnergyProfile=new TH1F("bgoEnergyProfile","bgoEnergyProfile",BGO_CHANNELS,-0.5,BGO_CHANNELS-0.5);
  outObjects[TString("BGO_")+TString(h_bgoEnergyProfile->GetName())]=(TObject*)h_bgoEnergyProfile; 

  TH1F* h_bgoEnergyUp;  
  h_bgoEnergyUp=new TH1F("bgoEnergyUp","bgoEnergyUp",4096,-0.5,4095.5);
  outObjects[TString("BGO_")+TString(h_bgoEnergyUp->GetName())]=(TObject*)h_bgoEnergyUp; 

  TH1F* h_bgoEnergyDown;
  h_bgoEnergyDown=new TH1F("bgoEnergyDown","bgoEnergyDown",4096,-0.5,4095.5);
  outObjects[TString("BGO_")+TString(h_bgoEnergyDown->GetName())]=(TObject*)h_bgoEnergyDown; 

  TH1F* h_bgoEnergyLeft;
  h_bgoEnergyLeft=new TH1F("bgoEnergyLeft","bgoEnergyLeft",4096,-0.5,4095.5);
  outObjects[TString("BGO_")+TString(h_bgoEnergyLeft->GetName())]=(TObject*)h_bgoEnergyLeft; 

  TH1F* h_bgoEnergyRight;
  h_bgoEnergyRight=new TH1F("bgoEnergyRight","bgoEnergyRight",4096,-0.5,4095.5);
  outObjects[TString("BGO_")+TString(h_bgoEnergyRight->GetName())]=(TObject*)h_bgoEnergyRight; 

  //CeF3 histograms
  TH1F* h_cef3RawSpectrum[CEF3_CHANNELS];
  for (int i=0;i<CEF3_CHANNELS;++i)
    {
      h_cef3RawSpectrum[i]=new TH1F(Form("cef3RawSpectrum_%d",i),Form("cef3RawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("CEF3RAW_")+TString(h_cef3RawSpectrum[i]->GetName())]=(TObject*)h_cef3RawSpectrum[i]; 
    }

  timeAverage cef3Energy_TimeAverage;

  TH1F* h_cef3Energy;
  h_cef3Energy=new TH1F("cef3Energy","cef3Energy",4096,-0.5,4095.5);
  outObjects[TString("CEF3_")+TString(h_cef3Energy->GetName())]=(TObject*)h_cef3Energy; 

  TH1F* h_cef3EnergyProfile;
  h_cef3EnergyProfile=new TH1F("cef3EnergyProfile","cef3EnergyProfile",CEF3_CHANNELS,-0.5,CEF3_CHANNELS-0.5);
  outObjects[TString("CEF3_")+TString(h_cef3EnergyProfile->GetName())]=(TObject*)h_cef3EnergyProfile; 

  //Full calo energy
  TH1F* h_caloEnergy;
  h_caloEnergy=new TH1F("caloEnergy","caloEnergy",4096,-0.5,4095.5);
  outObjects[TString("CALO_")+TString(h_caloEnergy->GetName())]=(TObject*)h_caloEnergy; 

  timeAverage caloEnergy_TimeAverage;

  //HodoX histograms
  TH1F* h_hodoXRawSpectrum[HODOX_CHANNELS];
  for (int i=0;i<HODOX_CHANNELS;++i)
    {
      h_hodoXRawSpectrum[i]=new TH1F(Form("hodoXRawSpectrum_%d",i),Form("hodoXRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("HODOXRAW_")+TString(h_hodoXRawSpectrum[i]->GetName())]=(TObject*)h_hodoXRawSpectrum[i]; 
    }

  TH1F* h_hodoXEnergy;
  h_hodoXEnergy=new TH1F("hodoXEnergy","hodoXEnergy",4096,-0.5,4095.5);
  outObjects[TString("HODO_")+TString(h_hodoXEnergy->GetName())]=(TObject*)h_hodoXEnergy; 

  TH1F* h_hodoXPos;
  h_hodoXPos=new TH1F("hodoXPos","hodoXPos",200,0,HODOX_CHANNELS);
  outObjects[TString("HODO_")+TString(h_hodoXPos->GetName())]=(TObject*)h_hodoXPos; 

  timeAverage hodoXEnergy_TimeAverage;
  timeAverage hodoXPos_TimeAverage;

  TH1F* h_hodoXEnergyProfile;
  h_hodoXEnergyProfile=new TH1F("hodoXEnergyProfile","hodoXEnergyProfile",HODOX_CHANNELS,-0.5,HODOX_CHANNELS-0.5);
  outObjects[TString("HODO_")+TString(h_hodoXEnergyProfile->GetName())]=(TObject*)h_hodoXEnergyProfile; 

  //HodoY histograms
  TH1F* h_hodoYRawSpectrum[HODOY_CHANNELS];
  for (int i=0;i<HODOY_CHANNELS;++i)
    {
      h_hodoYRawSpectrum[i]=new TH1F(Form("hodoYRawSpectrum_%d",i),Form("hodoYRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("HODOYRAW_")+TString(h_hodoYRawSpectrum[i]->GetName())]=(TObject*)h_hodoYRawSpectrum[i]; 
    }

  TH1F* h_hodoYEnergy;
  h_hodoYEnergy=new TH1F("hodoYEnergy","hodoYEnergy",4096,-0.5,4095.5);
  outObjects[TString("HODO_")+TString(h_hodoYEnergy->GetName())]=(TObject*)h_hodoYEnergy; 

  TH1F* h_hodoYPos;
  h_hodoYPos=new TH1F("hodoYPos","hodoYPos",200,0,HODOY_CHANNELS);
  outObjects[TString("HODO_")+TString(h_hodoYPos->GetName())]=(TObject*)h_hodoYPos; 

  timeAverage hodoYEnergy_TimeAverage;
  timeAverage hodoYPos_TimeAverage;

  TH1F* h_hodoYEnergyProfile;
  h_hodoYEnergyProfile=new TH1F("hodoYEnergyProfile","hodoYEnergyProfile",HODOY_CHANNELS,-0.5,HODOY_CHANNELS-0.5);
  outObjects[TString("HODO_")+TString(h_hodoYEnergyProfile->GetName())]=(TObject*)h_hodoYEnergyProfile; 

  //CenterX tagger histograms
  TH1F* h_centerXTaggerRawSpectrum[CENTERX_TAGGER_CHANNELS];
  for (int i=0;i<CENTERX_TAGGER_CHANNELS;++i)
    {
      h_centerXTaggerRawSpectrum[i]=new TH1F(Form("centerXTaggerRawSpectrum_%d",i),Form("centerXTaggerRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("CENTERTAGGER_")+TString(h_centerXTaggerRawSpectrum[i]->GetName())]=(TObject*)h_centerXTaggerRawSpectrum[i]; 
    }

  TH1F* h_centerXTaggerEnergy;
  h_centerXTaggerEnergy=new TH1F("centerXTaggerEnergy","centerXTaggerEnergy",4096,-0.5,4095.5);
  outObjects[TString("CENTERTAGGER_")+TString(h_centerXTaggerEnergy->GetName())]=(TObject*)h_centerXTaggerEnergy; 

  timeAverage centerXTaggerEnergy_TimeAverage;

  TH1F* h_centerXTaggerEnergyProfile;
  h_centerXTaggerEnergyProfile=new TH1F("centerXTaggerEnergyProfile","centerXTaggerEnergyProfile",CENTERX_TAGGER_CHANNELS,-0.5,CENTERX_TAGGER_CHANNELS-0.5);
  outObjects[TString("CENTERTAGGER_")+TString(h_centerXTaggerEnergyProfile->GetName())]=(TObject*)h_centerXTaggerEnergyProfile; 

  //CenterY tagger histograms
  TH1F* h_centerYTaggerRawSpectrum[CENTERY_TAGGER_CHANNELS];
  for (int i=0;i<CENTERY_TAGGER_CHANNELS;++i)
    {
      h_centerYTaggerRawSpectrum[i]=new TH1F(Form("centerYTaggerRawSpectrum_%d",i),Form("centerYTaggerRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("CENTERTAGGER_")+TString(h_centerYTaggerRawSpectrum[i]->GetName())]=(TObject*)h_centerYTaggerRawSpectrum[i]; 
    }

  TH1F* h_centerYTaggerEnergy;
  h_centerYTaggerEnergy=new TH1F("centerYTaggerEnergy","centerYTaggerEnergy",4096,-0.5,4095.5);
  outObjects[TString("CENTERTAGGER_")+TString(h_centerYTaggerEnergy->GetName())]=(TObject*)h_centerYTaggerEnergy; 

  timeAverage centerYTaggerEnergy_TimeAverage;

  TH1F* h_centerYTaggerEnergyProfile;
  h_centerYTaggerEnergyProfile=new TH1F("centerYTaggerEnergyProfile","centerYTaggerEnergyProfile",CENTERY_TAGGER_CHANNELS,-0.5,CENTERY_TAGGER_CHANNELS-0.5);
  outObjects[TString("CENTERTAGGER_")+TString(h_centerYTaggerEnergyProfile->GetName())]=(TObject*)h_centerYTaggerEnergyProfile; 

  //Scintillator histograms
  TH1F* h_scintFrontRawSpectrum;
  h_scintFrontRawSpectrum=new TH1F("scintFrontRawSpectrum","scintFrontRawSpectrum",4096,-0.5,4095.5);
  outObjects[TString("SCINT_")+TString(h_scintFrontRawSpectrum->GetName())]=(TObject*)h_scintFrontRawSpectrum; 

  timeAverage scintFrontEnergy_TimeAverage;

  TH1F* h_scintBackRawSpectrum;
  h_scintBackRawSpectrum=new TH1F("scintBackRawSpectrum","scintBackRawSpectrum",4096,-0.5,4095.5);
  outObjects[TString("SCINT_")+TString(h_scintBackRawSpectrum->GetName())]=(TObject*)h_scintBackRawSpectrum; 
  timeAverage scintBackEnergy_TimeAverage;

  //Correlation plots
  TH2F* h_bgo_vs_cef3;
  h_bgo_vs_cef3=new TH2F("bgo_vs_cef3","bgo_vs_cef3",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("CORR_")+TString(h_bgo_vs_cef3->GetName())]=(TObject*)h_bgo_vs_cef3; 

  TH2F* h_hodoX_vs_calo;
  h_hodoX_vs_calo=new TH2F("hodoX_vs_calo","hodoX_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("CORR_")+TString(h_hodoX_vs_calo->GetName())]=(TObject*)h_hodoX_vs_calo; 

  TH2F* h_hodoXPos_vs_bgoLRAsymm;
  h_hodoXPos_vs_bgoLRAsymm=new TH2F("hodoX_vs_bgoLRAsymm","hodoXPos_vs_bgoLRAsymm",200,-1.,1.,200,0.,HODOX_CHANNELS);
  outObjects[TString("HODO_")+TString(h_hodoXPos_vs_bgoLRAsymm->GetName())]=(TObject*)h_hodoXPos_vs_bgoLRAsymm; 

  TH2F* h_hodoY_vs_calo;
  h_hodoY_vs_calo=new TH2F("hodoY_vs_calo","hodoY_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("CORR_")+TString(h_hodoY_vs_calo->GetName())]=(TObject*)h_hodoY_vs_calo; 

  TH2F* h_hodoYPos_vs_bgoUDAsymm;
  h_hodoYPos_vs_bgoUDAsymm=new TH2F("hodoY_vs_bgoUDAsymm","hodoYPos_vs_bgoUDAsymm",200,-1.,1.,200,0.,HODOY_CHANNELS);
  outObjects[TString("HODO_")+TString(h_hodoYPos_vs_bgoUDAsymm->GetName())]=(TObject*)h_hodoYPos_vs_bgoUDAsymm; 

  TH2F* h_centerXTagger_vs_calo;
  h_centerXTagger_vs_calo=new TH2F("centerXTagger_vs_calo","centerXTagger_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("CORR_")+TString(h_centerXTagger_vs_calo->GetName())]=(TObject*)h_centerXTagger_vs_calo; 

  TH2F* h_centerYTagger_vs_calo;
  h_centerYTagger_vs_calo=new TH2F("centerYTagger_vs_calo","centerYTagger_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("CORR_")+TString(h_centerYTagger_vs_calo->GetName())]=(TObject*)h_centerYTagger_vs_calo; 

  TH2F* h_scintFront_vs_calo;
  h_scintFront_vs_calo=new TH2F("scintFront_vs_calo","scintFront_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("CORR_")+TString(h_scintFront_vs_calo->GetName())]=(TObject*)h_scintFront_vs_calo; 

  TH2F* h_scintBack_vs_calo;
  h_scintBack_vs_calo=new TH2F("scintBack_vs_calo","scintBack_vs_calo",4096,-0.5,4095.5,4096,-0.5,4095.5);
  outObjects[TString("CORR_")+TString(h_scintBack_vs_calo->GetName())]=(TObject*)h_scintBack_vs_calo; 

  //Stability plots (1 point every 5 minutes)
  TH1F* h_nTriggersTimeProfile;
  h_nTriggersTimeProfile=new TH1F("nTriggersTimeProfile","nTriggersTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_nTriggersTimeProfile->GetName())]=(TObject*)h_nTriggersTimeProfile; 

  TH1F* h_rateTimeProfile;
  h_rateTimeProfile=new TH1F("rateTimeProfile","rateTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_rateTimeProfile->GetName())]=(TObject*)h_rateTimeProfile; 


  TH1F* h_bgoEnergyTimeProfile;
  h_bgoEnergyTimeProfile=new TH1F("bgoEnergyTimeProfile","bgoEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_bgoEnergyTimeProfile->GetName())]=(TObject*)h_bgoEnergyTimeProfile; 

  TH1F* h_cef3EnergyTimeProfile;
  h_cef3EnergyTimeProfile=new TH1F("cef3EnergyTimeProfile","cef3EnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_cef3EnergyTimeProfile->GetName())]=(TObject*)h_cef3EnergyTimeProfile; 

  TH1F* h_caloEnergyTimeProfile;
  h_caloEnergyTimeProfile=new TH1F("caloEnergyTimeProfile","caloEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_caloEnergyTimeProfile->GetName())]=(TObject*)h_caloEnergyTimeProfile; 

  TH1F* h_hodoXEnergyTimeProfile;
  h_hodoXEnergyTimeProfile=new TH1F("hodoXEnergyTimeProfile","hodoXEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_hodoXEnergyTimeProfile->GetName())]=(TObject*)h_hodoXEnergyTimeProfile; 

  TH1F* h_hodoXPosTimeProfile;
  h_hodoXPosTimeProfile=new TH1F("hodoXPosTimeProfile","hodoXPosTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_hodoXPosTimeProfile->GetName())]=(TObject*)h_hodoXPosTimeProfile; 

  TH1F* h_hodoYEnergyTimeProfile;
  h_hodoYEnergyTimeProfile=new TH1F("hodoYEnergyTimeProfile","hodoYEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_hodoYEnergyTimeProfile->GetName())]=(TObject*)h_hodoYEnergyTimeProfile; 

  TH1F* h_hodoYPosTimeProfile;
  h_hodoYPosTimeProfile=new TH1F("hodoYPosTimeProfile","hodoYPosTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_hodoYPosTimeProfile->GetName())]=(TObject*)h_hodoYPosTimeProfile; 

  TH1F* h_centerXTaggerEnergyTimeProfile;
  h_centerXTaggerEnergyTimeProfile=new TH1F("centerXTaggerEnergyTimeProfile","centerXTaggerEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_centerXTaggerEnergyTimeProfile->GetName())]=(TObject*)h_centerXTaggerEnergyTimeProfile; 

  TH1F* h_centerYTaggerEnergyTimeProfile;
  h_centerYTaggerEnergyTimeProfile=new TH1F("centerYTaggerEnergyTimeProfile","centerYTaggerEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_centerYTaggerEnergyTimeProfile->GetName())]=(TObject*)h_centerYTaggerEnergyTimeProfile; 

  TH1F* h_scintFrontEnergyTimeProfile;
  h_scintFrontEnergyTimeProfile=new TH1F("scintFrontEnergyTimeProfile","scintFrontEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_scintFrontEnergyTimeProfile->GetName())]=(TObject*)h_scintFrontEnergyTimeProfile; 

  TH1F* h_scintBackEnergyTimeProfile;
  h_scintBackEnergyTimeProfile=new TH1F("scintBackEnergyTimeProfile","scintBackEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILE_")+TString(h_scintBackEnergyTimeProfile->GetName())]=(TObject*)h_scintBackEnergyTimeProfile; 


  std::cout << "==================== Booked histograms =======================" << std::endl;
   for (std::map<TString,TObject*>::const_iterator out=outObjects.begin();out!=outObjects.end();++out)
     std::cout << out->second->GetName() << std::endl;
  std::cout << "==================== Loop over events =======================" << std::endl;
   
  
  if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;

   unsigned int startTimeStamp=0;
   unsigned int oldTime=0;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {

      Long64_t ientry = LoadTree(jentry);
      if (jentry%1000==0)
	std::cout << "Analysing event " << jentry << std::endl;

      if (ientry < 0) break;

      nb = fChain->GetEntry(jentry);   nbytes += nb;

      unsigned int itime=0;

      if (jentry==0)
	{
	  startTimeStamp=evtTime;
	  std::cout << "First event taken @ timestamp (days from 01/04/2014) " <<  ((float)startTimeStamp/1000.)/86400. << std::endl;
	}

      if (startTimeStamp>0 && evtTime>0)
	itime=(evtTime-startTimeStamp)/(TIME_INTERVAL_SIZE); //binning time 5minutes interval (expect of the order od 25Hz*60*5 triggers in this range)

      if (jentry%100==0 && jentry>0)
	{
	  rate_TimeAverage.addMeasure(itime,1.e2*1.e3/(double)(evtTime-oldTime));
	  oldTime=evtTime;
	}

      nTriggers[itime]++;

      float bgoEnergy=0,bgoLeftEnergy=0,bgoRightEnergy=0,bgoUpEnergy=0,bgoDownEnergy=0,
	cef3Energy=0,caloEnergy=0,
	hodoXEnergy=0,hodoYEnergy=0,centerXTaggerEnergy=0,centerYTaggerEnergy=0,scintFrontEnergy=0,scintBackEnergy=0;
      float hodoXPos=0,hodoYPos=0;

      //Loop over adc channels
      for (int i=0;i<NUM_ADC_CHANNELS;++i)
	{
	  //Checking that everything makes sense 
	  if (adcData[i]<0 || adcData[i]>4095)
	    std::cout << "WARNING ADC value outside ADC 12bit range!" << std::endl;

	  if (adcBoard[i]>1 && adcBoard[i]<0)
	    std::cout << "WARNING ADC board is unknown!" << std::endl;

	  if (adcChannel[i]<0)
	    std::cout << "WARNING ADC channel is unknown!" << std::endl;

	  if ( (adcBoard[i]==0 && adcChannel[i]>7) || (adcBoard[i]==1 && adcChannel[i]>31))
	    std::cout << "WARNING ADC channel is unknown!" << std::endl;

	  if (adcBoard[i]==BGO_ADC_BOARD && (adcChannel[i]>=BGO_ADC_START_CHANNEL && adcChannel[i]<=BGO_ADC_END_CHANNEL) )
	    {
	      bgoEnergy+=adcData[i]*bgoIntercalibration[adcChannel[i]-BGO_ADC_START_CHANNEL];
	      if (isInBGOUpRow(adcChannel[i]-BGO_ADC_START_CHANNEL))
		bgoUpEnergy+=adcData[i]*bgoIntercalibration[adcChannel[i]-BGO_ADC_START_CHANNEL];
	      if (isInBGODownRow(adcChannel[i]-BGO_ADC_START_CHANNEL))
		bgoDownEnergy+=adcData[i]*bgoIntercalibration[adcChannel[i]-BGO_ADC_START_CHANNEL];
	      if (isInBGORightRow(adcChannel[i]-BGO_ADC_START_CHANNEL))
		bgoRightEnergy+=adcData[i]*bgoIntercalibration[adcChannel[i]-BGO_ADC_START_CHANNEL];
	      if (isInBGOLeftRow(adcChannel[i]-BGO_ADC_START_CHANNEL))
		bgoLeftEnergy+=adcData[i]*bgoIntercalibration[adcChannel[i]-BGO_ADC_START_CHANNEL];
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
	      hodoXPos+=(adcChannel[i]-HODOX_ADC_START_CHANNEL)*adcData[i]*hodoXIntercalibration[adcChannel[i]-HODOX_ADC_START_CHANNEL];
	      h_hodoXRawSpectrum[adcChannel[i]-HODOX_ADC_START_CHANNEL]->Fill(adcData[i]);
	    }

	  else if (adcBoard[i]==HODOY_ADC_BOARD && (adcChannel[i]>=HODOY_ADC_START_CHANNEL && adcChannel[i]<=HODOY_ADC_END_CHANNEL) )
	    {
	      hodoYEnergy+=adcData[i]*hodoYIntercalibration[adcChannel[i]-HODOY_ADC_START_CHANNEL];
	      hodoYPos+=(adcChannel[i]-HODOY_ADC_START_CHANNEL)*adcData[i]*hodoYIntercalibration[adcChannel[i]-HODOY_ADC_START_CHANNEL];
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
      hodoXPos=hodoXPos/hodoXEnergy;
      hodoYPos=hodoYPos/hodoYEnergy;

      //Fill energy plots
      h_bgoEnergy->Fill(bgoEnergy);
      h_bgoEnergyUp->Fill(bgoUpEnergy);
      h_bgoEnergyDown->Fill(bgoDownEnergy);
      h_bgoEnergyLeft->Fill(bgoLeftEnergy);
      h_bgoEnergyRight->Fill(bgoRightEnergy);
      h_cef3Energy->Fill(cef3Energy);
      h_caloEnergy->Fill(caloEnergy);
      h_hodoXEnergy->Fill(hodoXEnergy);
      h_hodoYEnergy->Fill(hodoYEnergy);
      h_centerXTaggerEnergy->Fill(centerXTaggerEnergy);
      h_centerYTaggerEnergy->Fill(centerYTaggerEnergy);

      //Fill position plots
      h_hodoXPos->Fill(hodoXPos);
      h_hodoYPos->Fill(hodoYPos);

      //Fill time averages
      bgoEnergy_TimeAverage.addMeasure(itime,bgoEnergy);
      cef3Energy_TimeAverage.addMeasure(itime,cef3Energy);
      caloEnergy_TimeAverage.addMeasure(itime,caloEnergy);
      hodoXEnergy_TimeAverage.addMeasure(itime,hodoXEnergy);
      hodoYEnergy_TimeAverage.addMeasure(itime,hodoYEnergy);
      hodoXPos_TimeAverage.addMeasure(itime,hodoXPos);
      hodoYPos_TimeAverage.addMeasure(itime,hodoYPos);
      centerXTaggerEnergy_TimeAverage.addMeasure(itime,centerXTaggerEnergy);
      centerYTaggerEnergy_TimeAverage.addMeasure(itime,centerYTaggerEnergy);
      scintFrontEnergy_TimeAverage.addMeasure(itime,scintFrontEnergy);
      scintBackEnergy_TimeAverage.addMeasure(itime,scintBackEnergy);

      //Fill correlation plots
      h_bgo_vs_cef3->Fill(cef3Energy,bgoEnergy);
      h_hodoX_vs_calo->Fill(caloEnergy,hodoXEnergy);
      h_hodoXPos_vs_bgoLRAsymm->Fill((bgoLeftEnergy-bgoRightEnergy)/(bgoLeftEnergy+bgoRightEnergy),hodoXPos);
      h_hodoYPos_vs_bgoUDAsymm->Fill((bgoUpEnergy-bgoDownEnergy)/(bgoUpEnergy+bgoDownEnergy),hodoYPos);
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

   //Calculate & fill time profile histograms
   for (unsigned int i(0);i<TIME_MAX_INTERVALS;++i)
     if (nTriggers[i]>0)
       {
	 h_nTriggersTimeProfile->SetBinContent(i+1,nTriggers[i]);
	 h_nTriggersTimeProfile->SetBinError(i+1,TMath::Sqrt(nTriggers[i]));
       }

   rate_TimeAverage.calculateAverages();
   fillTimeProfile(rate_TimeAverage,h_rateTimeProfile);
   bgoEnergy_TimeAverage.calculateAverages();
   fillTimeProfile(bgoEnergy_TimeAverage,h_bgoEnergyTimeProfile);
   cef3Energy_TimeAverage.calculateAverages();
   fillTimeProfile(cef3Energy_TimeAverage,h_cef3EnergyTimeProfile);
   caloEnergy_TimeAverage.calculateAverages();
   fillTimeProfile(caloEnergy_TimeAverage,h_caloEnergyTimeProfile);
   hodoXEnergy_TimeAverage.calculateAverages();
   fillTimeProfile(hodoXEnergy_TimeAverage,h_hodoXEnergyTimeProfile);
   hodoXPos_TimeAverage.calculateAverages();
   fillTimeProfile(hodoXPos_TimeAverage,h_hodoXPosTimeProfile);
   hodoYEnergy_TimeAverage.calculateAverages();
   fillTimeProfile(hodoYEnergy_TimeAverage,h_hodoYEnergyTimeProfile);
   hodoYPos_TimeAverage.calculateAverages();
   fillTimeProfile(hodoYPos_TimeAverage,h_hodoYPosTimeProfile);
   centerXTaggerEnergy_TimeAverage.calculateAverages();
   fillTimeProfile(centerXTaggerEnergy_TimeAverage,h_centerXTaggerEnergyTimeProfile);
   centerYTaggerEnergy_TimeAverage.calculateAverages();
   fillTimeProfile(centerYTaggerEnergy_TimeAverage,h_centerYTaggerEnergyTimeProfile);
   scintFrontEnergy_TimeAverage.calculateAverages();
   fillTimeProfile(scintFrontEnergy_TimeAverage,h_scintFrontEnergyTimeProfile);
   scintBackEnergy_TimeAverage.calculateAverages();
   fillTimeProfile(scintBackEnergy_TimeAverage,h_scintBackEnergyTimeProfile);

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
