#include "MINERvA_CCNpip_XSec_1DTpi_20deg_nu.h"

// The constructor
MINERvA_CCNpip_XSec_1DTpi_20deg_nu::MINERvA_CCNpip_XSec_1DTpi_20deg_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "MINERvA_CCNpip_XSec_1DTpi_20deg_nu";
  fPlotTitles = "; T_{#pi} (MeV); d#sigma/dT_{#pi} (cm^{2}/MeV/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  fIsDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  if (fIsShape) {
    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg_shape.txt");
    this->SetCovarMatrixFromCorrText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg_shape_cov.txt", fDataHist->GetNbinsX());
  } else {
    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg.txt");
    this->SetCovarMatrixFromCorrText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg_cov.txt", fDataHist->GetNbinsX());
  }

  this->SetupDefaultHist();

  // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
  // Please change when MINERvA releases new data!
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1.11);
  }

  fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

void MINERvA_CCNpip_XSec_1DTpi_20deg_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211 && event->PartInfo(j)->fP.E() > Ppip.E()) {
        Ppip = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double Tpi;

  if (hadMass > 100 && hadMass < 1800) {
    Tpi = FitUtils::T(Ppip)*1000.;
  } else {
    Tpi = -999;
  }

  fXVar = Tpi;

  return;
};

//******************************************************************** 
bool MINERvA_CCNpip_XSec_1DTpi_20deg_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  // Last false refers to that this is NOT the restricted MINERvA phase space, in which only forward-going muons are accepted
  return SignalDef::isCCNpip_MINERvA(event, nPions, EnuMin, EnuMax, true);
}
