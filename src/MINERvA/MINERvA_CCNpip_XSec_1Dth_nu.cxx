#include "MINERvA_CCNpip_XSec_1Dth_nu.h"

// The constructor
MINERvA_CCNpip_XSec_1Dth_nu::MINERvA_CCNpip_XSec_1Dth_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "MINERvA_CCNpip_XSec_1Dth_nu";
  fPlotTitles = "; #theta_{#pi} (degrees); d#sigma/d#theta_{#pi} (cm^{2}/degrees/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  isDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Reserve length 3 for the number of pions
  piIndex.reserve(3);
  thVect.reserve(3);

  if (type.find("NEW") != std::string::npos) {
    fName += "_2016";
    isNew = true;

    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_thpi.txt");

    // MINERvA mucked up the scaling in the data-release where everything was bin-width normalised to the first bin, not the nth bin
    double binOneWidth = fDataHist->GetBinWidth(1);
    for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
      double binNWidth = fDataHist->GetBinWidth(i+1);
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1E-40);
      fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*fDataHist->GetBinError(i+1)/100.);
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*binOneWidth/binNWidth);
      fDataHist->SetBinError(i+1, fDataHist->GetBinError(i+1)*binOneWidth/binNWidth);
    }
    // This is a correlation matrix! but it's all fixed in SetCovarMatrixFromText
    this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_thpi_corr.txt", fDataHist->GetNbinsX());

  } else {
    isNew = false;

    if (isShape) {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th_shape.txt");
      this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th_shape_cov.txt", fDataHist->GetNbinsX());
    } else {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th.txt");
      this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th_cov.txt", fDataHist->GetNbinsX());
    }

    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    // Please change when MINERvA releases new data!
    for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1.11);
    }
  }

  this->SetupDefaultHist();

  hnPions = new TH1I((fName+"_hNpions").c_str(), (fName+"_hNions; Number of pions; Counts").c_str(), 11, -1, 10);

  scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

void MINERvA_CCNpip_XSec_1Dth_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;
  TLorentzVector Ppip;

  piIndex.clear();
  thVect.clear();

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j) {

    // Only include alive particles
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;

    int PID = (event->PartInfo(j))->fPID;
    // Select highest momentum (energy) charged pion
    if (abs(PID) == 211) {
      piIndex.push_back(j);
    // Find muon
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double th;

  // If hadronic mass passes signal, loop over the pions
  if (hadMass > 100 && hadMass < 1800  && piIndex.size() > 0) {

    // Loop over surviving pions and pick up their kinetic energy
    for (unsigned int k = 0; k < piIndex.size(); ++k) {
      Ppip = (event->PartInfo(piIndex.at(k)))->fP;
      th = (180./M_PI)*FitUtils::th(Pnu, Ppip);
      thVect.push_back(th);

    }
  } else {
    th = -999;
  }

  fXVar = th;

  return;
};

//******************************************************************** 
bool MINERvA_CCNpip_XSec_1Dth_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  // Last false refers to that this is NOT the restricted MINERvA phase space, in which only forward-going muons are accepted
  return SignalDef::isCCNpip_MINERvA(event, nPions, EnuMin, EnuMax, false);
}

//******************************************************************** 
// Need to override FillHistograms() here because we fill the histogram N_pion times
void MINERvA_CCNpip_XSec_1Dth_nu::FillHistograms() {
//******************************************************************** 

  if (Signal){

    // Need to loop over all the pions in the sample
    for (size_t k = 0; k < thVect.size(); ++k) {
      this->fMCHist->Fill(thVect.at(k), Weight);
      this->fMCFine->Fill(thVect.at(k), Weight);
      this->mcStat->Fill(thVect.at(k), 1.0);

      PlotUtils::FillNeutModeArray(fMCHist_PDG, Mode, thVect.at(k), Weight);
    }

    hnPions->Fill(nPions);
  }

  return;
}

//******************************************************************** 
void MINERvA_CCNpip_XSec_1Dth_nu::Write(std::string drawOpts) {
//******************************************************************** 
  Measurement1D::Write(drawOpts);
  hnPions->Write();

  return;
}
