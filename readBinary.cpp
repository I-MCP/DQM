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
int main(int argc, char *argv[])
{
  char *filename = NULL;
  char *outfile = NULL;
  int c;

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

  //Instantiate the tree branches
  tree->Branch("evtNumber",&thisTreeEvent.evtNumber,"evtNumber/i");
  tree->Branch("evtTimeDist",&thisTreeEvent.evtTimeDist,"evtTimeDist/i");
  tree->Branch("evtTimeStart",&thisTreeEvent.evtTimeStart,"evtTimeStart/i");
  tree->Branch("evtTime",&thisTreeEvent.evtTime,"evtTime/i");

  tree->Branch("boardTriggerBit",&thisTreeEvent.boardTriggerBit,"boardTriggerBit/i");

  tree->Branch("triggerWord",&thisTreeEvent.triggerWord,"triggerWord/i");

  tree->Branch("nAdcChannels",&thisTreeEvent.nAdcChannels,"nAdcChannels/i");
  tree->Branch("adcBoard",thisTreeEvent.adcBoard,"adcBoard[nAdcChannels]/i");
  tree->Branch("adcChannel",thisTreeEvent.adcChannel,"adcChannel[nAdcChannels]/i");
  tree->Branch("adcData",thisTreeEvent.adcData,"adcData[nAdcChannels]/i");

  tree->Branch("nTdcChannels",&thisTreeEvent.nTdcChannels,"nTdcChannels/i");
  tree->Branch("tdcBoard",thisTreeEvent.tdcBoard,"tdcBoard[nTdcChannels]/i");
  tree->Branch("tdcChannel",thisTreeEvent.tdcChannel,"tdcChannel[nTdcChannels]/i");
  tree->Branch("tdcData",thisTreeEvent.tdcData,"tdcData[nTdcChannels]/i");
  
  unsigned int evtSize;
  unsigned int adcChannels;
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
      thisEvent.triggerWord.clear();
      thisEvent.adcValues.clear();
      thisEvent.tdcValues.clear();
      myFile.read ((char*)&adcChannels, sizeof(adcChannels));
      myFile.read ((char*)&eventHeaderSize, sizeof(eventHeaderSize));
      for (int i=0;i<adcChannels;++i)
	{
	  unsigned int adcRawData;
	  myFile.read ((char*)&adcRawData, sizeof(adcRawData));
	  adcData thisData;
	  thisData.board=0; //for the moment just 1 board
	  //These bit masks are valid for the ADC V265
	  thisData.channel = (adcRawData & 0xe000)>>13;
	  thisData.adcReadout = adcRawData & 0xfff;
	  thisEvent.adcValues.push_back(thisData);
	} 
      //      std::cout << "This event has " << thisEvent.adcValues.size() << " ADC channels " << std::endl;
      myFile.read ((char*)&thisEvent.evtTimeDist, sizeof(thisEvent.evtTimeDist));
      myFile.read ((char*)&thisEvent.evtTimeStart, sizeof(thisEvent.evtTimeStart));
      myFile.read ((char*)&thisEvent.evtTime, sizeof(thisEvent.evtTime));

      fillTreeData(thisEvent,thisTreeEvent);
      tree->Fill();
      ++totEvents;
    }

  std::cout << totEvents << " were read in total" << std::endl;

  tree->Write();
  out->Close();
  std::cout << outfile << " is closed" << std::endl;

  myFile.close();
  std::cout << filename << " is closed" << std::endl;
}
