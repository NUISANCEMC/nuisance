#include "ArgoNeuT_CCInc_XSec_1Dthetamu_antinu.h"


//********************************************************************
ArgoNeuT_CCInc_XSec_1Dthetamu_antinu::ArgoNeuT_CCInc_XSec_1Dthetamu_antinu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ArgoNeuT_CCInc_XSec_1Dthetamu_antinu sample. \n" \
                        "Target: Ar40 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("theta_{#mu} (degrees)");
  fSettings.SetYTitle("d#sigma/d#theta_{#mu} (cm^{2} Ar^{-1} degrees^{-1})");
  fSettings.SetAllowedTypes("FIX/DIAG", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 50.0);
  fSettings.DefineAllowedTargets("Ar");

  // CCQELike plot information
  fSettings.SetTitle("ArgoNeut CC-INC antinumu 1D#theta_{#mu}");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/ArgoNeuT/CCInc_dsig_dthetamu_nubar.dat" );
  fSettings.DefineAllowedSpecies("numub");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) *
                 (40.0 /*Data is /Ar */) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  ScaleData(1E-38);
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void ArgoNeuT_CCInc_XSec_1Dthetamu_antinu::FillEventVariables(FitEvent *event) {
  FitParticle* pmu = event->GetHMFSParticle(-13);
  if (pmu) fXVar = 180.*pmu->fP.Vect().Theta()/TMath::Pi();
  return;
};

//********************************************************************
bool ArgoNeuT_CCInc_XSec_1Dthetamu_antinu::isSignal(FitEvent *event)
//********************************************************************
{
  return SignalDef::isCCINC(event, -14, EnuMin, EnuMax);
}
