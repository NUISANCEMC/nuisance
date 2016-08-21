#include "ArgoNeuT_CCInc_XSec_1Dpmu_antinu.h"

//********************************************************************
ArgoNeuT_CCInc_XSec_1Dpmu_antinu::ArgoNeuT_CCInc_XSec_1Dpmu_antinu(
    std::string inputfile, FitWeight *rw, std::string type,
    std::string fakeDataFile)
//********************************************************************
{
  measurementName = "ArgoNeuT_CCInc_XSec_1Dpmu_antinu";
  default_types = "FIX/DIAG/CHI2";
  plotTitles = "; p_{#mu} (GeV); d#sigma/dp_{#mu} (cm^{2} Ar^{-1} GeV^{-1})";
  EnuMin = 0;
  EnuMax = 50;
  isDiag = true;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  SetDataValues(std::string(std::getenv("EXT_FIT")) +
                "/data/ArgoNeuT/CCInc_dsig_dmumom_nubar.dat");

  SetupDefaultHist();

  scaleFactor = eventHist->Integral("width") * double(1E-38) / double(nevents) *
                (40.0 /*Data is /Ar */) / TotalIntegratedFlux("width");
};

void ArgoNeuT_CCInc_XSec_1Dpmu_antinu::FillEventVariables(FitEvent *event) {
  X_VAR = FitUtils::GetHMPDG_4Mom(-13, event).Vect().Mag();
  return;
};

//********************************************************************
bool ArgoNeuT_CCInc_XSec_1Dpmu_antinu::isSignal(FitEvent *event)
//********************************************************************
{
  return SignalDef::isCCInc_ArgoNeuT(event, true);
}

