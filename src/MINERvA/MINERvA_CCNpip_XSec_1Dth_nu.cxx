#include "MINERvA_CCNpip_XSec_1Dth_nu.h"

// The constructor
MINERvA_CCNpip_XSec_1Dth_nu::MINERvA_CCNpip_XSec_1Dth_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  measurementName = "MINERvA_CCNpip_XSec_1Dth_nu";
  plotTitles = "; #theta_{#pi} (degrees); d#sigma/d#theta_{#pi} (cm^{2}/degrees/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  isDiag = false;
  allowed_types += "NEW";

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Reserve length 3 for the number of pions
  piIndex.reserve(3);
  thVect.reserve(3);

  if (type.find("NEW") != std::string::npos) {
    measurementName += "_2016";
    isNew = true;

    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_thpi.txt");

    // MINERvA mucked up the scaling in the data-release where everything was bin-width normalised to the first bin, not the nth bin
    double binOneWidth = dataHist->GetBinWidth(1);
    for (int i = 0; i < dataHist->GetNbinsX()+1; i++) {
      double binNWidth = dataHist->GetBinWidth(i+1);
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1E-40);
      dataHist->SetBinError(i+1, dataHist->GetBinContent(i+1)*dataHist->GetBinError(i+1)/100.);
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*binOneWidth/binNWidth);
      dataHist->SetBinError(i+1, dataHist->GetBinError(i+1)*binOneWidth/binNWidth);
    }
    // This is a correlation matrix! but it's all fixed in SetCovarMatrixFromText
    this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_thpi_corr.txt", dataHist->GetNbinsX());

  } else {
    isNew = false;

    if (isShape) {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th_shape.txt");
      this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th_shape_cov.txt", dataHist->GetNbinsX());
    } else {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th.txt");
      this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th_cov.txt", dataHist->GetNbinsX());
    }

    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    // Please change when MINERvA releases new data!
    for (int i = 0; i < dataHist->GetNbinsX() + 1; i++) {
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1.11);
    }
  }

  this->SetupDefaultHist();

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

  this->X_VAR = th;

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
      this->mcHist->Fill(thVect.at(k), Weight);
      this->mcFine->Fill(thVect.at(k), Weight);
      this->mcStat->Fill(thVect.at(k), 1.0);

      PlotUtils::FillNeutModeArray(mcHist_PDG, Mode, thVect.at(k), Weight);
    }

  }

  return;
}

//******************************************************************** 
void MINERvA_CCNpip_XSec_1Dth_nu::Write(std::string drawOpts) {
//******************************************************************** 
  Measurement1D::Write(drawOpts);

  return;
}
