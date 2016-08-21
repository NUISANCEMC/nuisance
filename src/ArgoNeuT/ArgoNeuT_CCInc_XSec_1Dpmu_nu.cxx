#include "ArgoNeuT_CCInc_XSec_1Dpmu_nu.h"

//********************************************************************
ArgoNeuT_CCInc_XSec_1Dpmu_nu::ArgoNeuT_CCInc_XSec_1Dpmu_nu(
    std::string inputfile, FitWeight *rw, std::string type,
    std::string fakeDataFile)
//********************************************************************
{
  measurementName = "ArgoNeuT_CCInc_XSec_1Dpmu_nu";
  default_types = "FIX/DIAG/CHI2";
  plotTitles = "; p_{#mu} (GeV); d#sigma/dp_{#mu} (cm^{2} Ar^{-1} GeV^{-1})";
  EnuMin = 0;
  EnuMax = 50;
  isDiag = true;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  SetDataValues(std::string(std::getenv("EXT_FIT")) +
                "/data/ArgoNeuT/CCInc_dsig_dmumom_nu.dat");

  dataHist->Scale(1E-38);
  dataTrue->Scale(1E-38);

  SetupDefaultHist();

  scaleFactor = eventHist->Integral("width") * double(1E-38) / double(nevents) *
                (40.0 /*Data is /Ar */) / TotalIntegratedFlux("width");

  std::cout << "SF: " << eventHist->Integral("width") << " " << nevents << " " << TotalIntegratedFlux("width") << std::endl;
};

void ArgoNeuT_CCInc_XSec_1Dpmu_nu::FillEventVariables(FitEvent *event) {
  X_VAR = FitUtils::GetHMPDG_4Mom(13, event).Vect().Mag()/1000.0;
  return;
};

//********************************************************************
bool ArgoNeuT_CCInc_XSec_1Dpmu_nu::isSignal(FitEvent *event)
//********************************************************************
{
  return SignalDef::isCCInc_ArgoNeuT(event);
}

