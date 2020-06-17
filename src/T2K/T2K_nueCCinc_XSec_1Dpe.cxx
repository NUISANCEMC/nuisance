#include "T2K_nueCCinc_XSec_1Dpe.h"
#include "T2K_SignalDef.h"

//********************************************************************
T2K_nueCCinc_XSec_1Dpe::T2K_nueCCinc_XSec_1Dpe(nuiskey samplekey) {
//********************************************************************

  fSettings = LoadSampleSettings(samplekey);
  std::string name = fSettings.GetS("name");
  std::string descrip = "";
  // This has to deal with nue FHC, and nue/nuebar RHC
  if (!name.compare("T2K_nueCCinc_XSec_1Dpe_FHC")){
    descrip = name +". \n"
      "Target: CH \n"
      "Flux: T2K FHC nue  \n"
      "Signal: CC-inclusive \n";

    fSettings.SetTitle("T2K FHC #nu_{e}-CC-inclusive p_{e}");
    fSettings.DefineAllowedSpecies("nue");
    fSettings.SetDataInput(FitPar::GetDataBase() + "/T2K/CCinc/nue_2019/FHC_nue_pe.txt");
    nuPDG  = 12;
    lepPDG = 11;
  } else if (!name.compare("T2K_nueCCinc_XSec_1Dpe_RHC")){
    descrip = name +". \n"
      "Target: CH \n"
      "Flux: T2K RHC nue  \n"
      "Signal: CC-inclusive \n";

    fSettings.SetTitle("T2K RHC #nu_{e}-CC-inclusive p_{e}");
    fSettings.DefineAllowedSpecies("nue");
    fSettings.SetDataInput(FitPar::GetDataBase() + "/T2K/CCinc/nue_2019/RHC_nue_pe.txt");
    nuPDG  = 12;
    lepPDG = 11;
  } else if (!name.compare("T2K_nuebarCCinc_XSec_1Dpe_RHC")){
    descrip = name +". \n"
      "Target: CH \n"
      "Flux: T2K RHC nuebar  \n"
      "Signal: CC-inclusive \n";

    fSettings.SetTitle("T2K RHC #bar{#nu}_{e}-CC-inclusive p_{e}");
    fSettings.DefineAllowedSpecies("nueb");
    fSettings.SetDataInput(FitPar::GetDataBase() + "/T2K/CCinc/nue_2019/RHC_nuebar_pe.txt");
    nuPDG  = -12;
    lepPDG = -11;
  }

  // Setup common settings
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{e} (GeV)");
  fSettings.SetYTitle("#frac{d#sigma}{dp_{e}} (cm^{2}/nucleon)");
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
bool T2K_nueCCinc_XSec_1Dpe::isSignal(FitEvent *event) {
  if (!SignalDef::isCCINC(event, nuPDG, EnuMin, EnuMax)) return false;

  // Phase space restriction
  TLorentzVector Pnu = event->GetHMISParticle(nuPDG)->fP;
  TLorentzVector Pe  = event->GetHMFSParticle(lepPDG)->fP;
  static double max_angle = 45*TMath::Pi()/180.;
  if (FitUtils::th(Pnu, Pe) > max_angle) return false;
  return true;
};

void T2K_nueCCinc_XSec_1Dpe::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(lepPDG) == 0) return;
  TLorentzVector Pe = event->GetHMFSParticle(lepPDG)->fP;
  fXVar = Pe.Vect().Mag() / 1000.;
};
