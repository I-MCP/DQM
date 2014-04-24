#include <cstdlib>
#include <fstream>
#include <iostream>

#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TCanvas.h"



float getSinglePeakPosition( const std::string& outdir, int hv, const std::string& suffix, int iChannel);


int main() {


  // first get pedestal:
  TFile* file_ped = TFile::Open("../run_precalib_BGO_pedestal_noSource.root");
  TTree* tree_ped = (TTree*)file_ped->Get("eventRawData");

  TH1D* h1_pedestal = new TH1D("ped", "", 200, 100., 200.);
  tree_ped->Project("ped", "adcData[0]");

  float pedestal = h1_pedestal->GetMean();

  int hv = 1700;
  std::string suffix = "front";

  std::string outdir = Form("bgoCalibration_hv%d_%s", hv, suffix.c_str() );
  std::string mkdir_command = "mkdir -p " + outdir;
  system(mkdir_command.c_str());

  std::string resultsFileName = Form("%s/results.txt", outdir.c_str());
  ofstream ofs(resultsFileName.c_str());

  ofs << "ped " << pedestal << std::endl;

  int nChannels=8;

  for( unsigned i=1; i<nChannels+1; ++i ) 
    ofs << i << " " << getSinglePeakPosition(outdir, hv, suffix, i) - pedestal << std::endl;

  ofs.close();

  std::cout << std::endl << "-> Saved results in file: " << resultsFileName << std::endl; 

  return 0;

}



float getSinglePeakPosition( const std::string& outdir, int hv, const std::string& suffix, int iChannel) {

  TFile* file = TFile::Open(Form("../run_precalib_BGO_ch%d_HV%d_%s.root", iChannel, hv, suffix.c_str()));

  TTree* tree = (TTree*)file->Get("eventRawData");

  TH1D* h1_spectrum_full  = new TH1D("spectrum_full",  "", 1000, 0., 1000.);
  TH1D* h1_spectrum_trunc = new TH1D("spectrum_trunc", "", 70, 300., 1000.);

  h1_spectrum_full->SetXTitle( "adcData[0]" );
  
  tree->Project("spectrum_full",  "adcData[0]");
  tree->Project("spectrum_trunc", "adcData[0]");

  // initialize position to point of maximum of truncated histo:
  float mean = h1_spectrum_trunc->GetBinCenter( h1_spectrum_trunc->GetMaximumBin() );
  float rms  = h1_spectrum_trunc->GetRMS()/2.; // just a guess (maybe 50 is better?)
  float integral  = h1_spectrum_trunc->Integral();

  float nSigma_lo = 1.;
  float nSigma_hi = 2.;

  float xMin = mean-nSigma_lo*rms;
  float xMax = mean+nSigma_hi*rms;

  TF1* f1_gaus = new TF1("gaus0", "gaus", xMin, xMax);
  f1_gaus->SetParameter(0, integral);
  f1_gaus->SetParameter(1, mean);
  f1_gaus->SetParameter(2, rms);

  f1_gaus->SetLineColor(kRed);

  int niter = 4;
  for( unsigned iter=0; iter<niter; ++iter ) {
    std::string option = (iter==niter-1) ? "R" : "RNQ";
    h1_spectrum_full->Fit( f1_gaus, option.c_str() );
    float mean_new = f1_gaus->GetParameter(1);
    float rms_new = f1_gaus->GetParameter(2);
    f1_gaus->SetRange( mean_new-nSigma_lo*rms_new, mean_new+nSigma_hi*rms_new );
  }



  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  h1_spectrum_full->Draw();
    
  c1->SaveAs( Form("%s/ped_ch%d.eps", outdir.c_str(), iChannel) );
  c1->SaveAs( Form("%s/ped_ch%d.png", outdir.c_str(), iChannel) );

  delete c1;


  return f1_gaus->GetParameter(1);

}
  
