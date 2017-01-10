#include "ArgoNeuT_CCInc_XSec_1Dthetamu_nu.h"

//********************************************************************
ArgoNeuT_CCInc_XSec_1Dthetamu_nu::ArgoNeuT_CCInc_XSec_1Dthetamu_nu(
    std::string inputfile, FitWeight *rw, std::string type,
    std::string fakeDataFile)
//********************************************************************
{
  fName = "ArgoNeuT_CCInc_XSec_1Dthetamu_nu";
  fDefaultTypes = "FIX/DIAG/CHI2";
  fPlotTitles =
      "; theta_{#mu} (degrees); d#sigma/d#theta_{#mu} (cm^{2} Ar^{-1} "
      "degrees^{-1})";
  EnuMin = 0;
  EnuMax = 50;
  fIsDiag = true;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  SetDataValues(GeneralUtils::GetTopLevelDir() +
                "/data/ArgoNeuT/CCInc_dsig_dthetamu_nu.dat");

  fDataHist->Scale(1E-38);
  fDataTrue->Scale(1E-38);

  SetupDefaultHist();

  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) *
                (40.0 /*Data is /Ar */) / TotalIntegratedFlux("width");
};

void ArgoNeuT_CCInc_XSec_1Dthetamu_nu::FillEventVariables(FitEvent *event) {
  FitParticle* pmu = event->GetHMFSParticle(13);
  if (pmu) fXVar = 180.*pmu->fP.Vect().Theta()/TMath::Pi();
  return;
};

//********************************************************************
bool ArgoNeuT_CCInc_XSec_1Dthetamu_nu::isSignal(FitEvent *event)
//********************************************************************
{
  return SignalDef::isCCINC(event, 14, EnuMin, EnuMax);
}
