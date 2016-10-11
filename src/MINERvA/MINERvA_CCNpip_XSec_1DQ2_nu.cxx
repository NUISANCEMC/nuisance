#include "MINERvA_CCNpip_XSec_1DQ2_nu.h"

// The constructor
MINERvA_CCNpip_XSec_1DQ2_nu::MINERvA_CCNpip_XSec_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  measurementName = "MINERvA_CCNpip_XSec_1DQ2_nu_2016";
  plotTitles = "; Q^{2} (GeV^{2}); d#sigma/dQ^{2} (cm^{2}/GeV^{2}/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  isDiag = false;
  allowed_types += "NEW";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);


  //this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_q2.txt");
  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016/nu-ccNpi+-xsec-q2.csv");

  // MINERvA has the error quoted as a percentage of the cross-section
  // Need to make this into an absolute error before we go from correlation matrix -> covariance matrix since it depends on the error in the ith bin
  for (int i = 0; i < dataHist->GetNbinsX()+1; i++) {
    dataHist->SetBinError(i+1, dataHist->GetBinContent(i+1)*(dataHist->GetBinError(i+1)/100.));
  }

  //this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_q2_corr.txt", dataHist->GetNbinsX());
  // This is a correlation matrix
  this->SetCovarMatrixFromCorrText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016/nu-ccNpi+-correlation-q2.csv", dataHist->GetNbinsX());

  this->SetupDefaultHist();

  scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

void MINERvA_CCNpip_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j) {
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211 && event->PartInfo(j)->fP.E() > Ppip.E()) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double q2 = -999;

  // Include up to some given hadronic mass cut
  if (hadMass > 100 && hadMass < 1800) {
    q2 = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip);
  }

  this->X_VAR = q2;

  return;
};

//******************************************************************** 
bool MINERvA_CCNpip_XSec_1DQ2_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  // Last false refers to that this is NOT the restricted MINERvA phase space, in which only forward-going muons are accepted
  int dummy;
  return SignalDef::isCCNpip_MINERvA(event, dummy, EnuMin, EnuMax, false);
}
