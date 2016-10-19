#include "MINERvA_CCNpip_XSec_1DEnu_nu.h"

// The constructor
MINERvA_CCNpip_XSec_1DEnu_nu::MINERvA_CCNpip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile) {

  fName = "MINERvA_CCNpip_XSec_1DEnu_nu_2016";
  fPlotTitles = "; E_{#nu} (GeV); d#sigma(E_{#nu}) (cm^{2}/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  fIsDiag = false;
  fAllowedTypes += "NEW";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);


  //this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016/ccnpip_enu.txt");
  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016/nu-ccNpi+-xsec-enu.csv");

  // MINERvA has the error quoted as a percentage of the cross-section
  // Need to make this into an absolute error before we go from correlation matrix -> covariance matrix since it depends on the error in the ith bin
  for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
    fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*(fDataHist->GetBinError(i+1)/100.));
  }

  // We're given a correlation matrix, so need to convert it to a covariance matrix
  //this->SetCovarMatrixFromCorrText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016/ccnpip_enu_corr.txt", fDataHist->GetNbinsX());
  this->SetCovarMatrixFromCorrText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016/nu-ccNpi+-correlation-enu.csv", fDataHist->GetNbinsX());

  this->SetupDefaultHist();

  fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents);
};

void MINERvA_CCNpip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

    TLorentzVector Pnu = (event->PartInfo(0))->fP;
    TLorentzVector Ppip;
    TLorentzVector Pmu;

    // Loop over the particle stack
    for (unsigned int j = 2; j < event->Npart(); ++j) {
      if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue;
      int PID = (event->PartInfo(j))->fPID;
      if (PID == 211 && event->PartInfo(j)->fP.E() > Ppip.E()) {
        Ppip = event->PartInfo(j)->fP;
      } else if (PID == 13) {
        Pmu = (event->PartInfo(j))->fP;
      }
    }

    double hadMass = FitUtils::Wrec(Pnu, Pmu);
    double Enu;

    if (hadMass > 100 && hadMass < 1800) {
      Enu = Pnu.E()/1000.;
    } else {
      Enu = -999;
  }

  fXVar = Enu;

  return;
};

//******************************************************************** 
bool MINERvA_CCNpip_XSec_1DEnu_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  int dummy;
  return SignalDef::isCCNpip_MINERvA(event, dummy, EnuMin, EnuMax, false);
}
