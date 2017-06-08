#include "FNAL_CC1ppim_XSec_1DEnu_antinu.h"


//********************************************************************
FNAL_CC1ppim_XSec_1DEnu_antinu::FNAL_CC1ppim_XSec_1DEnu_antinu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "FNAL_CC1ppim_XSec_1DEnu_antinu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("#sigma(E_{#nu}) (cm^{2}/proton)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG", "FIX/DIAG");
  fSettings.SetEnuRange(5.0, 70.0);
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("FNAL_CC1ppim_XSec_1DEnu_antinu");
  fSettings.DefineAllowedSpecies("numub");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "FNAL/CC1ppim_on_p/FNAL_CC1ppim_on_p_Enu.csv");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor for shape
  fScaleFactor =  GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents)*(2./1.);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

}

void FNAL_CC1ppim_XSec_1DEnu_antinu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(-211) == 0 || 
      event->NumFSParticle(-13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(-211)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double Enu     = -1.0;

  if (hadMass < 1400) Enu = Pnu.E()/1.E3;
  fXVar = Enu;

  return;
}


bool FNAL_CC1ppim_XSec_1DEnu_antinu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, -14, -211, 2212, EnuMin, EnuMax);
}
