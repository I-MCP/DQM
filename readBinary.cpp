#include <fstream>
#include <iostream>
#include <vector>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <TFile.h>
#include <TTree.h>

#define MAX_ADC_CHANNELS 200
#define MAX_TDC_CHANNELS 200

struct treeStructData
{
  // unsigned int runNumber; to be added soon
  unsigned int evtNumber;
  unsigned int evtTimeDist;
  unsigned int evtTimeStart;
  unsigned int evtTime;

  unsigned int boardTriggerBit;

  unsigned int triggerWord;

  unsigned int nAdcChannels;
  unsigned int adcBoard[MAX_ADC_CHANNELS];
  unsigned int adcChannel[MAX_ADC_CHANNELS];
  unsigned int adcData[MAX_ADC_CHANNELS];

  unsigned int nTdcChannels;
  unsigned int tdcBoard[MAX_TDC_CHANNELS];
  unsigned int tdcChannel[MAX_TDC_CHANNELS];
  unsigned int tdcData[MAX_TDC_CHANNELS];
};

struct adcData
{
  unsigned int board;
  unsigned int channel;
  unsigned int adcReadout;
};


struct tdcData
{
  unsigned int board;
  unsigned int channel;
  unsigned int tdcReadout;
};

struct myDAQEventData
{
  unsigned int evtNumber;
  unsigned int boardTriggerBit;
  std::vector<bool> triggerWord;
  std::vector<adcData> adcValues; 
  std::vector<tdcData> tdcValues; 
  unsigned int evtTimeDist;
  unsigned int evtTimeStart;
  unsigned int evtTime;
};


void cannotOpenFile(char * file)
{
  fprintf (stderr,
	   "File %s cannot be opened\n",
	   file);
  exit(-1);

}

void fillTreeData(myDAQEventData& data,treeStructData& treeData)
{
  treeData.evtNumber=data.evtNumber;
  treeData.evtTime=data.evtTime;
  treeData.evtTimeStart=data.evtTimeStart;
  treeData.evtTimeDist=data.evtTimeDist;

  treeData.boardTriggerBit=data.boardTriggerBit;

  treeData.triggerWord=0;
  for (unsigned int i=0;i<fmin(32,data.triggerWord.size());++i)
    treeData.triggerWord+=data.triggerWord[i]>>i;

  treeData.nAdcChannels=data.adcValues.size();
  for (unsigned int i=0;i<fmin(MAX_ADC_CHANNELS,data.adcValues.size());++i)
    {
      treeData.adcBoard[i]=data.adcValues[i].board;
      treeData.adcChannel[i]=data.adcValues[i].channel;
      treeData.adcData[i]=data.adcValues[i].adcReadout;
    }

  treeData.nTdcChannels=data.tdcValues.size();
  for (unsigned int i=0;i<fmin(MAX_TDC_CHANNELS,data.tdcValues.size());++i)
    {
      treeData.tdcBoard[i]=data.tdcValues[i].board;
      treeData.tdcChannel[i]=data.tdcValues[i].channel;
      treeData.tdcData[i]=data.tdcValues[i].tdcReadout;
    }

}


void createOutBranches(TTree* tree,treeStructData& treeData)
{
  //Instantiate the tree branches
  tree->Branch("evtNumber",&treeData.evtNumber,"evtNumber/i");
  tree->Branch("evtTimeDist",&treeData.evtTimeDist,"evtTimeDist/i");
  tree->Branch("evtTimeStart",&treeData.evtTimeStart,"evtTimeStart/i");
  tree->Branch("evtTime",&treeData.evtTime,"evtTime/i");

  tree->Branch("boardTriggerBit",&treeData.boardTriggerBit,"boardTriggerBit/i");

  tree->Branch("triggerWord",&treeData.triggerWord,"triggerWord/i");

  tree->Branch("nAdcChannels",&treeData.nAdcChannels,"nAdcChannels/i");
  tree->Branch("adcBoard",treeData.adcBoard,"adcBoard[nAdcChannels]/i");
  tree->Branch("adcChannel",treeData.adcChannel,"adcChannel[nAdcChannels]/i");
  tree->Branch("adcData",treeData.adcData,"adcData[nAdcChannels]/i");

  tree->Branch("nTdcChannels",&treeData.nTdcChannels,"nTdcChannels/i");
  tree->Branch("tdcBoard",treeData.tdcBoard,"tdcBoard[nTdcChannels]/i");
  tree->Branch("tdcChannel",treeData.tdcChannel,"tdcChannel[nTdcChannels]/i");
  tree->Branch("tdcData",treeData.tdcData,"tdcData[nTdcChannels]/i");

}

int main(int argc, char *argv[])
{
  char *filename = NULL;
  char *outfile = NULL;
  int c;
  bool adc792=false;

  while ((c = getopt (argc, argv, "f:o:")) != -1)
    switch (c)
      {
      case 'f':
	filename = optarg;
	break;
      case 'o':
	outfile = optarg;
	break;

      case '?':
	if (optopt == 'f' || optopt == 'o')
	  fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	else if (isprint (optopt))
	  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	else
	  fprintf (stderr,
		   "Unknown option character `\\x%x'.\n",
		   optopt);
	return 1;
      default:
	exit (-1);
      }
     
  std::ifstream myFile (filename, std::ios::in | std::ios::binary);
  
  if (!myFile.is_open())
    cannotOpenFile(filename);
  
  std::streampos size;
  myFile.seekg(0,std::ios::end);
  size=myFile.tellg();

  fprintf(stdout,
	  "File %s opened with size %d\n",
	  filename,(int)size
	  );

  myFile.seekg(0,std::ios::beg);

  TFile *out = TFile::Open(outfile,"RECREATE");  
  if (!out->IsOpen())
    cannotOpenFile(outfile);

  TTree* tree=new TTree("eventRawData","VME event RAW Data");
  treeStructData thisTreeEvent;
  
  createOutBranches(tree,thisTreeEvent);
  
  unsigned int evtSize;
  unsigned int adc265Channels=0;
  unsigned int adc792Channels=0;
  unsigned int eventHeaderSize;
  myDAQEventData thisEvent;
  
  unsigned int totEvents=0;
  while (!myFile.eof())
    {
      myFile.read ((char*)&evtSize, sizeof(evtSize));
      if (myFile.eof())
	continue;
      myFile.read ((char*)&thisEvent.evtNumber, sizeof(thisEvent.evtNumber));
      if (thisEvent.evtNumber % 1000 == 0)
	{
	  std::cout << "Event Number " << thisEvent.evtNumber << std::endl;
	}
      myFile.read ((char*)&thisEvent.boardTriggerBit, sizeof(thisEvent.boardTriggerBit));
      bool has_ADC265=(thisEvent.boardTriggerBit >> 3) & 0x1;
      bool has_ADC792=(thisEvent.boardTriggerBit >> 4) & 0x1;
      thisEvent.triggerWord.clear();
      thisEvent.adcValues.clear();
      thisEvent.tdcValues.clear();
      if (has_ADC265)
	myFile.read ((char*)&adc265Channels, sizeof(adc265Channels));
      if (has_ADC792)
	myFile.read ((char*)&adc792Channels, sizeof(adc792Channels));
      myFile.read ((char*)&eventHeaderSize, sizeof(eventHeaderSize));


      for (int i=0;i<adc265Channels;++i)
	{
	      unsigned int adcRawData;
	      myFile.read ((char*)&adcRawData, sizeof(adcRawData));
	      //FOR ADC265 DATA
	      adcData thisData;
	      thisData.board=0; 
	      //These bit masks are valid for the ADC V265
	      thisData.channel = (adcRawData & 0xe000)>>13;
	      thisData.adcReadout = adcRawData & 0xfff;
	      thisEvent.adcValues.push_back(thisData);
	      
	}
      
      for (int i=0;i<adc792Channels;++i)
	{
	  unsigned int adcRawData;
	  myFile.read ((char*)&adcRawData, sizeof(adcRawData));
	  
	  short dt_type = adcRawData>>24 & 0x7; //dt_type 0 is datum,2 is BOE,4 is EOE
	  if (dt_type==0)
	    {
	      //FOR ADC792 DATA
	      adcData thisData;
	      
	      thisData.board=1; 
	      thisData.channel = adcRawData>>16 & 0x1F;
	      thisData.adcReadout = adcRawData & 0xFFF;
	      if ((adcRawData>>12) & 0x1) //overflow
		thisData.adcReadout=4095;
	      
	      thisEvent.adcValues.push_back(thisData);
	    }
	  if (dt_type==4)
	    {
	      unsigned int adcEvt=adcRawData & 0xFFFFFF;
	      if (adcEvt+1 != thisEvent.evtNumber)
		std::cout << "HEY MISMATCH IN EVT NUMBER ADCEVT " << adcEvt+1 << " EVT " << thisEvent.evtNumber << std::endl;
	    }
	  //printf("WORD %d: %X %d\n",i,adcRawData,dt_type);
	}


      //      std::cout << "This event has " << thisEvent.adcValues.size() << " ADC channels " << std::endl;
      myFile.read ((char*)&thisEvent.evtTimeDist, sizeof(thisEvent.evtTimeDist));
      myFile.read ((char*)&thisEvent.evtTimeStart, sizeof(thisEvent.evtTimeStart));
      myFile.read ((char*)&thisEvent.evtTime, sizeof(thisEvent.evtTime));

      fillTreeData(thisEvent,thisTreeEvent);
      tree->Fill();
      ++totEvents;
    }

  std::cout << totEvents << " events were read in total" << std::endl;

  tree->Write();
  out->Close();
  std::cout << outfile << " is closed" << std::endl;

  myFile.close();
  std::cout << filename << " is closed" << std::endl;
}
