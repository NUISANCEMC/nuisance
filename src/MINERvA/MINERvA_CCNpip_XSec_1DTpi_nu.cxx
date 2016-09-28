#include "MINERvA_CCNpip_XSec_1DTpi_nu.h"

// The constructor
MINERvA_CCNpip_XSec_1DTpi_nu::MINERvA_CCNpip_XSec_1DTpi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "MINERvA_CCNpip_XSec_1DTpi_nu";
  fPlotTitles = "; T_{#pi} (MeV); d#sigma/dT_{#pi} (cm^{2}/MeV/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  isDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Reserve length 3 for the number of pions
  piIndex.reserve(3);
  TpiVect.reserve(3);

  if (type.find("NEW") != std::string::npos) {
    fName += "_2016";
    isNew = true;

    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_tpi.txt");

    // MINERvA mucked up the scaling in the data-release where everything was bin-width normalised to the first bin, not the nth bin
    double binOneWidth = fDataHist->GetBinWidth(1);
    for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
      double binNWidth = fDataHist->GetBinWidth(i+1);
      // First scale to 1E-40
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1E-40);
      // Then set the error as absolute error, not the percent error (which the datafile is listed with)
      fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*fDataHist->GetBinError(i+1)/100.);
      // Scale the bin content
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*binOneWidth/binNWidth);
      fDataHist->SetBinError(i+1, fDataHist->GetBinError(i+1)*binOneWidth/binNWidth);
    }

    // This is a correlation matrix
    this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_tpi_corr.txt", fDataHist->GetNbinsX());

  } else {
    isNew = false;

    if (isShape) {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi_shape.txt");
      this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi_shape_cov.txt", fDataHist->GetNbinsX());
    } else {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi.txt");
      this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi_cov.txt", fDataHist->GetNbinsX());
    }

    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1.11);
    }
  }

  this->SetupDefaultHist();

  fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

void MINERvA_CCNpip_XSec_1DTpi_nu::FillEventVariables(FitEvent *event) {

  piIndex.clear();
  TpiVect.clear();

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j) {

    // Only include alive particles
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;

    int PID = (event->PartInfo(j))->fPID;
    // Pick up the charged pions
    if (abs(PID) == 211) {
      piIndex.push_back(j);
    // Find muon
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  // Hadronic mass cut in true
  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double Tpi;

  // If hadronic mass passes signal, loop over the pions
  if (hadMass > 100 && hadMass < 1800) {

    // Loop over surviving pions and pick up their kinetic energy
    for (size_t k = 0; k < piIndex.size(); ++k) {
      TLorentzVector Ppip = event->PartInfo(piIndex.at(k))->fP;
      Tpi = FitUtils::T(Ppip)*1000.;
      TpiVect.push_back(Tpi);
    }

  } else {
    Tpi = -999;
  }

  fXVar = Tpi;

  return;
};

//******************************************************************** 
// Need to override FillHistograms() here because we fill the histogram N_pion times
void MINERvA_CCNpip_XSec_1DTpi_nu::FillHistograms() {
//******************************************************************** 

  if (Signal){

    // Need to loop over all the pions in the sample
    for (size_t k = 0; k < TpiVect.size(); ++k) {
      this->fMCHist->Fill(TpiVect.at(k), Weight);
      this->fMCFine->Fill(TpiVect.at(k), Weight);
      this->mcStat->Fill(TpiVect.at(k), 1.0);

      PlotUtils::FillNeutModeArray(fMCHist_PDG, Mode, TpiVect.at(k), Weight);
    }
  }

}

//******************************************************************** 
bool MINERvA_CCNpip_XSec_1DTpi_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  // Last false refers to that this is NOT the restricted MINERvA phase space, in which only forward-going muons are accepted
  return SignalDef::isCCNpip_MINERvA(event, nPions, EnuMin, EnuMax, false);
}
