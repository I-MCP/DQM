#define fastDQM_iMCP_BTF_cxx
#include "fastDQM_iMCP_BTF.h"
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>

#include <iostream>

float hodoXIntercalibration[HODOX_CHANNELS] = { 1., 1., 1., 1., 1., 1., 1., 1.};
float hodoYIntercalibration[HODOY_CHANNELS] = { 1., 1., 1., 1., 1., 1., 1., 1.};

// float centerXTaggerIntercalibration[CENTERX_TAGGER_CHANNELS] = { 1., 1.};
// float centerYTaggerIntercalibration[CENTERY_TAGGER_CHANNELS] = { 1.};


float hodoXPedMean[HODOX_CHANNELS] = { 113.263, 96.3056, 129.509, 119.84, 95.1994, 136.194, 135.549, 162.077};
//float hodoXPedRMS[HODOX_CHANNELS] = { };
float hodoYPedMean[HODOY_CHANNELS] = { 108.752, 101.68, 118.983, 137.645, 94.2584, 99.2072, 89.6646, 99.2206};
//float hodoYPedRMS[HODOY_CHANNELS] = { };



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

void fastDQM_iMCP_BTF::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L fastDQM_iMCP_BTF.C
//      Root > fastDQM_iMCP_BTF t
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

  //HodoX histograms
  TH1F* h_hodoXRawSpectrum[HODOX_CHANNELS];
  TH1F* h_hodoXCorSpectrum[HODOX_CHANNELS];
  for (int i=0;i<HODOX_CHANNELS;++i)
    {
      h_hodoXRawSpectrum[i]=new TH1F(Form("hodoXRawSpectrum_%d",i),Form("hodoXRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("HODOXRAW_")+TString(h_hodoXRawSpectrum[i]->GetName())]=(TObject*)h_hodoXRawSpectrum[i]; 
      h_hodoXCorSpectrum[i]=new TH1F(Form("hodoXCorSpectrum_%d",i),Form("hodoXCorSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("HODOXCOR_")+TString(h_hodoXCorSpectrum[i]->GetName())]=(TObject*)h_hodoXCorSpectrum[i]; 
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
  TH1F* h_hodoYCorSpectrum[HODOY_CHANNELS];
  for (int i=0;i<HODOY_CHANNELS;++i)
    {
      h_hodoYRawSpectrum[i]=new TH1F(Form("hodoYRawSpectrum_%d",i),Form("hodoYRawSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("HODOYRAW_")+TString(h_hodoYRawSpectrum[i]->GetName())]=(TObject*)h_hodoYRawSpectrum[i]; 
      h_hodoYCorSpectrum[i]=new TH1F(Form("hodoYCorSpectrum_%d",i),Form("hodoYCorSpectrum_%d",i),4096,-0.5,4095.5);
      outObjects[TString("HODOYCOR_")+TString(h_hodoYCorSpectrum[i]->GetName())]=(TObject*)h_hodoYCorSpectrum[i]; 
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

  //Scintillator histograms
  TH1F* h_scintFrontRawSpectrum;
  h_scintFrontRawSpectrum=new TH1F("scintFrontRawSpectrum","scintFrontRawSpectrum",4096,-0.5,4095.5);
  outObjects[TString("SCINT_")+TString(h_scintFrontRawSpectrum->GetName())]=(TObject*)h_scintFrontRawSpectrum; 

  timeAverage scintFrontEnergy_TimeAverage;

  TH1F* h_scintBackRawSpectrum;
  h_scintBackRawSpectrum=new TH1F("scintBackRawSpectrum","scintBackRawSpectrum",4096,-0.5,4095.5);
  outObjects[TString("SCINT_")+TString(h_scintBackRawSpectrum->GetName())]=(TObject*)h_scintBackRawSpectrum; 
  timeAverage scintBackEnergy_TimeAverage;

  //Stability plots (1 point every 5 minutes)
  TH1F* h_nTriggersTimeProfile;
  h_nTriggersTimeProfile=new TH1F("nTriggersTimeProfile","nTriggersTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILETRIG_")+TString(h_nTriggersTimeProfile->GetName())]=(TObject*)h_nTriggersTimeProfile; 

  TH1F* h_rateTimeProfile;
  h_rateTimeProfile=new TH1F("rateTimeProfile","rateTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILETRIG_")+TString(h_rateTimeProfile->GetName())]=(TObject*)h_rateTimeProfile; 

  TH1F* h_hodoXEnergyTimeProfile;
  h_hodoXEnergyTimeProfile=new TH1F("hodoXEnergyTimeProfile","hodoXEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILEHODO_")+TString(h_hodoXEnergyTimeProfile->GetName())]=(TObject*)h_hodoXEnergyTimeProfile; 

  TH1F* h_hodoXPosTimeProfile;
  h_hodoXPosTimeProfile=new TH1F("hodoXPosTimeProfile","hodoXPosTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILEHODO_")+TString(h_hodoXPosTimeProfile->GetName())]=(TObject*)h_hodoXPosTimeProfile; 

  TH1F* h_hodoYEnergyTimeProfile;
  h_hodoYEnergyTimeProfile=new TH1F("hodoYEnergyTimeProfile","hodoYEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILEHODO_")+TString(h_hodoYEnergyTimeProfile->GetName())]=(TObject*)h_hodoYEnergyTimeProfile; 

  TH1F* h_hodoYPosTimeProfile;
  h_hodoYPosTimeProfile=new TH1F("hodoYPosTimeProfile","hodoYPosTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILEHODO_")+TString(h_hodoYPosTimeProfile->GetName())]=(TObject*)h_hodoYPosTimeProfile; 

  TH1F* h_scintFrontEnergyTimeProfile;
  h_scintFrontEnergyTimeProfile=new TH1F("scintFrontEnergyTimeProfile","scintFrontEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILESCINT_")+TString(h_scintFrontEnergyTimeProfile->GetName())]=(TObject*)h_scintFrontEnergyTimeProfile; 

  TH1F* h_scintBackEnergyTimeProfile;
  h_scintBackEnergyTimeProfile=new TH1F("scintBackEnergyTimeProfile","scintBackEnergyTimeProfile",TIME_MAX_INTERVALS,-0.5,TIME_MAX_INTERVALS-0.5);
  outObjects[TString("TIMEPROFILESCINT_")+TString(h_scintBackEnergyTimeProfile->GetName())]=(TObject*)h_scintBackEnergyTimeProfile; 


  //Persistency plots waveforms for various channels
  TH2F* h_mcp_0_waveDump;
  h_mcp_0_waveDump=new TH2F("h_mcp_0_waveDump","h_mcp_0_waveDump",1024,-0.5,1023.5,4096,-0.5,4095.5);
  outObjects[TString("MCPWAVEDUMP_")+TString(h_mcp_0_waveDump->GetName())]=(TObject*)h_mcp_0_waveDump; 

  TH2F* h_mcp_1_waveDump;
  h_mcp_1_waveDump=new TH2F("h_mcp_1_waveDump","h_mcp_1_waveDump",1024,-0.5,1023.5,4096,-0.5,4095.5);
  outObjects[TString("MCPWAVEDUMP_")+TString(h_mcp_1_waveDump->GetName())]=(TObject*)h_mcp_1_waveDump; 

  TH2F* h_mcp_2_waveDump;
  h_mcp_2_waveDump=new TH2F("h_mcp_2_waveDump","h_mcp_2_waveDump",1024,-0.5,1023.5,4096,-0.5,4095.5);
  outObjects[TString("MCPWAVEDUMP_")+TString(h_mcp_2_waveDump->GetName())]=(TObject*)h_mcp_2_waveDump; 

  TH2F* h_mcp_3_waveDump;
  h_mcp_3_waveDump=new TH2F("h_mcp_3_waveDump","h_mcp_3_waveDump",1024,-0.5,1023.5,4096,-0.5,4095.5);
  outObjects[TString("MCPWAVEDUMP_")+TString(h_mcp_3_waveDump->GetName())]=(TObject*)h_mcp_3_waveDump; 

  TH2F* h_mcp_4_waveDump;
  h_mcp_4_waveDump=new TH2F("h_mcp_4_waveDump","h_mcp_4_waveDump",1024,-0.5,1023.5,4096,-0.5,4095.5);
  outObjects[TString("MCPWAVEDUMP_")+TString(h_mcp_4_waveDump->GetName())]=(TObject*)h_mcp_4_waveDump; 

  TH2F* h_scintFront_waveDump;
  h_scintFront_waveDump=new TH2F("h_scintFront_waveDump","h_scintFront_waveDump",1024,-0.5,1023.5,4096,-0.5,4095.5);
  outObjects[TString("SCINTWAVEDUMP_")+TString(h_scintFront_waveDump->GetName())]=(TObject*)h_scintFront_waveDump; 

  TH2F* h_scintBack_waveDump;
  h_scintBack_waveDump=new TH2F("h_scintBack_waveDump","h_scintBack_waveDump",1024,-0.5,1023.5,4096,-0.5,4095.5);
  outObjects[TString("SCINTWAVEDUMP_")+TString(h_scintBack_waveDump->GetName())]=(TObject*)h_scintBack_waveDump; 

  TH2F* h_tr0_waveDump;
  h_tr0_waveDump=new TH2F("h_tr0_waveDump","h_tr0_waveDump",1024,-0.5,1023.5,4096,-0.5,4095.5);
  outObjects[TString("TRIGGERWAVEDUMP_")+TString(h_tr0_waveDump->GetName())]=(TObject*)h_tr0_waveDump; 
  

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

      float hodoXEnergy=0,hodoYEnergy=0,scintFrontEnergy=0,scintBackEnergy=0;
      float hodoXPos=0,hodoYPos=0;

      //Loop over adc channels
      for (int i=0;i<nAdcChannels;++i)
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

	  if (adcBoard[i]==HODOX_ADC_BOARD && (adcChannel[i]>=HODOX_ADC_START_CHANNEL && adcChannel[i]<=HODOX_ADC_END_CHANNEL) )
	    {
	      hodoXEnergy+=(adcData[i]-hodoXPedMean[adcChannel[i]-HODOX_ADC_START_CHANNEL])
                            *hodoXIntercalibration[adcChannel[i]-HODOX_ADC_START_CHANNEL];
	      hodoXPos+=(adcChannel[i]-HODOX_ADC_START_CHANNEL)*((adcData[i]-hodoXPedMean[adcChannel[i]-HODOX_ADC_START_CHANNEL])*hodoXIntercalibration[adcChannel[i]-HODOX_ADC_START_CHANNEL]);
	      h_hodoXRawSpectrum[adcChannel[i]-HODOX_ADC_START_CHANNEL]->Fill(adcData[i]);
	      h_hodoXCorSpectrum[adcChannel[i]-HODOX_ADC_START_CHANNEL]->Fill(adcData[i]-hodoXPedMean[adcChannel[i]-HODOX_ADC_START_CHANNEL]);
	    }

	  else if (adcBoard[i]==HODOY_ADC_BOARD && (adcChannel[i]>=HODOY_ADC_START_CHANNEL && adcChannel[i]<=HODOY_ADC_END_CHANNEL) )
	    {
	      hodoYEnergy+=(adcData[i]-hodoYPedMean[adcChannel[i]-HODOY_ADC_START_CHANNEL])
                            *hodoYIntercalibration[adcChannel[i]-HODOY_ADC_START_CHANNEL];
	      hodoYPos+=(adcChannel[i]-HODOY_ADC_START_CHANNEL)*((adcData[i]-hodoYPedMean[adcChannel[i]-HODOY_ADC_START_CHANNEL])*hodoYIntercalibration[adcChannel[i]-HODOY_ADC_START_CHANNEL]);
	      h_hodoYRawSpectrum[adcChannel[i]-HODOY_ADC_START_CHANNEL]->Fill(adcData[i]);
	      h_hodoYCorSpectrum[adcChannel[i]-HODOY_ADC_START_CHANNEL]->Fill(adcData[i]-hodoYPedMean[adcChannel[i]-HODOY_ADC_START_CHANNEL]);
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
      
      
      
      //Loop over adc channels
      for (unsigned int i=0;i<nDigiSamples;++i)
     {
       //Checking that everything makes sense 
       if (digiSampleValue[i]<-3 || digiSampleValue[i]>4100)
	 std::cout << "WARNING DIGI sample value outside ADC 12bit range!" << std::endl;

       if (digiSampleIndex[i]>1023)
	 std::cout << "WARNING DIGI sample index outside digitizer buffer range" << std::endl;
       
       if (digiGroup[i]>3)
	 std::cout << "WARNING DIGI group is unknown!" << std::endl;
       
       if (digiChannel[i]>8 )
	 std::cout << "WARNING DIGI channel is unknown!" << std::endl;
       
       if (digiChannel[i]==MCP_0_DIGITIZER_CHANNEL &&  digiGroup[i]==0 )
	   h_mcp_0_waveDump->Fill(digiSampleIndex[i],digiSampleValue[i]);

       if (digiChannel[i] == MCP_1_DIGITIZER_CHANNEL )
       	 h_mcp_1_waveDump->Fill(digiSampleIndex[i],digiSampleValue[i]);

       if (digiChannel[i] == MCP_2_DIGITIZER_CHANNEL )
       	 h_mcp_2_waveDump->Fill(digiSampleIndex[i],digiSampleValue[i]);
	 
       if (digiChannel[i] == MCP_3_DIGITIZER_CHANNEL )
       	 h_mcp_3_waveDump->Fill(digiSampleIndex[i],digiSampleValue[i]);

       if (digiChannel[i] == MCP_4_DIGITIZER_CHANNEL )
       	 h_mcp_4_waveDump->Fill(digiSampleIndex[i],digiSampleValue[i]);

       if (digiChannel[i] == SCINT_FRONT_DIGITIZER_CHANNEL )
       	 h_scintFront_waveDump->Fill(digiSampleIndex[i],digiSampleValue[i]);

       if (digiChannel[i] == SCINT_BACK_DIGITIZER_CHANNEL )
       	 h_scintBack_waveDump->Fill(digiSampleIndex[i],digiSampleValue[i]);

       if (digiChannel[i] == TR0_DIGITIZER_CHANNEL )
       	 h_tr0_waveDump->Fill(digiSampleIndex[i],digiSampleValue[i]);
     }

   //Fill position plots
   if (hodoXEnergy>0)
     h_hodoXPos->Fill(hodoXPos/hodoXEnergy);
   if (hodoYEnergy>0)
     h_hodoYPos->Fill(hodoYPos/hodoYEnergy);
   
   //Fill time averages
   hodoXEnergy_TimeAverage.addMeasure(itime,hodoXEnergy);
   hodoYEnergy_TimeAverage.addMeasure(itime,hodoYEnergy);
   hodoXPos_TimeAverage.addMeasure(itime,hodoXPos);
   hodoYPos_TimeAverage.addMeasure(itime,hodoYPos);
   scintFrontEnergy_TimeAverage.addMeasure(itime,scintFrontEnergy);
      scintBackEnergy_TimeAverage.addMeasure(itime,scintBackEnergy);
      
      
   }
   

   for (unsigned int i(0); i<HODOX_CHANNELS;++i)
     {
       float eventsAbobePedestal=h_hodoXCorSpectrum[i]->Integral(10,50);
       //       float rms=h_hodoXRawSpectrum[i]->GetRMS();
       h_hodoXEnergyProfile->SetBinContent(i+1,eventsAbobePedestal);
       h_hodoXEnergyProfile->SetBinError(i+1,sqrt(eventsAbobePedestal));
     }
   TF1* fitFuncHodo=new TF1("fitFuncHodo","pol0+gaus(1)",0.,8.);
   float avgY = 0.; 
   for(unsigned int i(0); i<HODOX_CHANNELS; i++) 
        avgY+= h_hodoXEnergyProfile->GetBinContent(i+1);
   avgY /= HODOX_CHANNELS;
   fitFuncHodo->SetParameter(1,avgY);
   fitFuncHodo->SetParameter(2,h_hodoXEnergyProfile->GetMean());
   fitFuncHodo->SetParameter(3,h_hodoXEnergyProfile->GetRMS());
   std::cout << " Fitting h_hodoXEnergyProfile" << std::endl;
   h_hodoXEnergyProfile->Fit(fitFuncHodo,"RL+","",0.,8.);

   for (unsigned int i(0); i<HODOY_CHANNELS;++i)
     {
       float eventsAbobePedestal=h_hodoYCorSpectrum[i]->Integral(10,50);
       //       float rms=h_hodoYRawSpectrum[i]->GetRMS();
       h_hodoYEnergyProfile->SetBinContent(i+1,eventsAbobePedestal);
       h_hodoYEnergyProfile->SetBinError(i+1,sqrt(eventsAbobePedestal));
     }
   avgY = 0.; 
   for(unsigned int i(0); i<HODOY_CHANNELS; i++) 
        avgY+= h_hodoYEnergyProfile->GetBinContent(i+1);
   avgY /= HODOY_CHANNELS;
   fitFuncHodo->SetParameter(1,avgY);
   fitFuncHodo->SetParameter(2,h_hodoYEnergyProfile->GetMean());
   fitFuncHodo->SetParameter(3,h_hodoYEnergyProfile->GetRMS());
   std::cout << " Fitting h_hodoYEnergyProfile" << std::endl;
   h_hodoYEnergyProfile->Fit(fitFuncHodo,"RL+","",0.,8.);

   //Calculate & fill time profile histograms
   for (unsigned int i(0);i<TIME_MAX_INTERVALS;++i)
     if (nTriggers[i]>0)
       {
	 h_nTriggersTimeProfile->SetBinContent(i+1,nTriggers[i]);
	 h_nTriggersTimeProfile->SetBinError(i+1,TMath::Sqrt(nTriggers[i]));
       }

   rate_TimeAverage.calculateAverages();
   fillTimeProfile(rate_TimeAverage,h_rateTimeProfile);
   hodoXEnergy_TimeAverage.calculateAverages();
   fillTimeProfile(hodoXEnergy_TimeAverage,h_hodoXEnergyTimeProfile);
   hodoXPos_TimeAverage.calculateAverages();
   fillTimeProfile(hodoXPos_TimeAverage,h_hodoXPosTimeProfile);
   hodoYEnergy_TimeAverage.calculateAverages();
   fillTimeProfile(hodoYEnergy_TimeAverage,h_hodoYEnergyTimeProfile);
   hodoYPos_TimeAverage.calculateAverages();
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
