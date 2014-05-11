void findTurnOn(char * filename)
{
  TFile *f=TFile::Open(filename);
  TTree* eventRawData=f->Get("eventRawData");
  TH1F* h=new TH1F("h","h",100,0,6000);
  eventRawData->Project("h","((adcData[12]-125)*1.19+(adcData[14]-107)*0.83+(adcData[16]-114)*1.06+(adcData[18]-122)*0.98)","(adcData[8]<1800 && adcData[8]>500.)","");
  // h->Draw();
  float integral=h->Integral(6,100);
  float mean=h->GetMean();
  float mean_err=h->GetMeanError();
  std::cout << integral/ eventRawData->GetEntries() << "\t" << mean << "\t\t" << mean_err << std::endl;
// .q
// TFile *_file0 = TFile::Open("run_BTF_352_20140503-150638_beam.root")
// (float)eventRawData.Draw("((adcData[12]-125)*1.19+(adcData[14]-107)*0.83+(adcData[16]-114)*1.06+(adcData[18]-122)*0.98)>>hh(100,0,6000)","(adcData[8]<1800 && adcData[8]>500.)","")/eventRawData.GetEntries()

}
