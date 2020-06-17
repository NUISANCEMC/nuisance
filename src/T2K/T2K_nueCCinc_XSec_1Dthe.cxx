#include "T2K_nueCCinc_XSec_1Dthe.h"
#include "T2K_SignalDef.h"

//********************************************************************
T2K_nueCCinc_XSec_1Dthe::T2K_nueCCinc_XSec_1Dthe(nuiskey samplekey) {
//********************************************************************

  fSettings = LoadSampleSettings(samplekey);
  std::string name = fSettings.GetS("name");
  std::string descrip = "";
  // This has to deal with nue FHC, and nue/nuebar RHC
  if (!name.compare("T2K_nueCCinc_XSec_1Dthe_FHC")){
    descrip = name +". \n"
      "Target: CH \n"
      "Flux: T2K FHC nue  \n"
      "Signal: CC-inclusive \n";

    fSettings.SetTitle("T2K FHC #nu_{e}-CC-inclusive cos#theta_{e}");
    fSettings.DefineAllowedSpecies("nue");
    fSettings.SetDataInput(FitPar::GetDataBase() + "/T2K/CCinc/nue_2019/FHC_nue_thetae.txt");
    nuPDG  = 12;
    lepPDG = 11;
  } else if (!name.compare("T2K_nueCCinc_XSec_1Dthe_RHC")){
    descrip = name +". \n"
      "Target: CH \n"
      "Flux: T2K RHC nue  \n"
      "Signal: CC-inclusive \n";

    fSettings.SetTitle("T2K RHC #nu_{e}-CC-inclusive cos#theta_{e}");
    fSettings.DefineAllowedSpecies("nue");
    fSettings.SetDataInput(FitPar::GetDataBase() + "/T2K/CCinc/nue_2019/RHC_nue_thetae.txt");
    nuPDG  = 12;
    lepPDG = 11;
  } else if (!name.compare("T2K_nuebarCCinc_XSec_1Dthe_RHC")){
    descrip = name +". \n"
      "Target: CH \n"
      "Flux: T2K RHC nuebar  \n"
      "Signal: CC-inclusive \n";

    fSettings.SetTitle("T2K RHC #bar{#nu}_{e}-CC-inclusive cos#theta_{e}");
    fSettings.DefineAllowedSpecies("nueb");
    fSettings.SetDataInput(FitPar::GetDataBase() + "/T2K/CCinc/nue_2019/RHC_nuebar_thetae.txt");
    nuPDG  = -12;
    lepPDG = -11;
  }

  // Setup common settings
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("cos#theta_{e} (GeV)");
  fSettings.SetYTitle("#frac{d#sigma}{dcos#theta_{e}} (cm^{2}/nucleon)");
  fSettings.SetEnuRange(0.0, 30.0);
  fSettings.DefineAllowedTargets("C,H");
  FinaliseSampleSettings();

  SetDataFromTextFile( fSettings.GetDataInput() );

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * 1E-38 / fNEvents /
                 TotalIntegratedFlux();
  
  // Don't scale by width
  fIsNoWidth = true;

  FinaliseMeasurement();
};

// Signal is simply a CC inclusive without any angular/momentum cuts
bool T2K_nueCCinc_XSec_1Dthe::isSignal(FitEvent *event) {
  if (!SignalDef::isCCINC(event, nuPDG, EnuMin, EnuMax)) return false;

  // Phase space restriction
  TLorentzVector Pe  = event->GetHMFSParticle(lepPDG)->fP;
  if (Pe.Vect().Mag() < 300) return false;
  return true;
};

void T2K_nueCCinc_XSec_1Dthe::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(lepPDG) == 0) return;
  TLorentzVector Pnu = event->GetHMISParticle(nuPDG)->fP;
  TLorentzVector Pe = event->GetHMFSParticle(lepPDG)->fP;
  fXVar = cos(FitUtils::th(Pnu, Pe));
};
