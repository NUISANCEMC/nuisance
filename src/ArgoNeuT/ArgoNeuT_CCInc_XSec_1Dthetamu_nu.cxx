#include "ArgoNeuT_CCInc_XSec_1Dthetamu_nu.h"

//********************************************************************
ArgoNeuT_CCInc_XSec_1Dthetamu_nu::ArgoNeuT_CCInc_XSec_1Dthetamu_nu(
    std::string inputfile, FitWeight *rw, std::string type,
    std::string fakeDataFile)
//********************************************************************
{
  measurementName = "ArgoNeuT_CCInc_XSec_1Dthetamu_nu";
  default_types = "FIX/DIAG/CHI2";
  plotTitles =
      "; theta_{#mu} (degrees); d#sigma/d#theta_{#mu} (cm^{2} Ar^{-1} "
      "degrees^{-1})";
  EnuMin = 0;
  EnuMax = 50;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  SetDataValues(std::string(std::getenv("EXT_FIT")) +
                "/data/ArgoNeuT/CCInc_dsig_dthetamu_nu.dat");

  SetupDefaultHist();

  scaleFactor = eventHist->Integral("width") * double(1E-38) / double(nevents) *
                (40.0 /*Data is /Ar */) / TotalIntegratedFlux("width");
};

void ArgoNeuT_CCInc_XSec_1Dthetamu_nu::FillEventVariables(FitEvent *event) {
  X_VAR =
      (FitUtils::GetHMPDG_4Mom(13, event).first.Vect().Theta() / TMath::Pi()) *
      180.;
  return;
};

//********************************************************************
bool ArgoNeuT_CCInc_XSec_1Dthetamu_nu::isSignal(FitEvent *event)
//********************************************************************
{
  return SignalDef::isCCInc_ArgoNeuT(event, true);
}
