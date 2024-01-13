#include "T2K_NC1pip_XSec_lin2D.h"

T2K_NC1pip_XSec_lin2D::T2K_NC1pip_XSec_lin2D(nuiskey samplekey) {

  fSettings = LoadSampleSettings(samplekey);
  std::string name = fSettings.GetS("name");
  std::string descrip = name +". \n"
    "Target: CH \n"
    "Flux: T2K FHC all flavours \n"
    "Signal: NC-1pi^{+} \n";
  
  fSettings.SetTitle("T2K FHC #nu_{x}-NC-1#pi^{+}");
  fSettings.DefineAllowedSpecies("numu, numub, nue, nueb");
  fSettings.SetDataInput(FitPar::GetDataBase() + "/T2K/NC1piplus/T2K_FHC_NC1pip_data.txt");
  fSettings.SetCovarInput(FitPar::GetDataBase() + "/T2K/NC1piplus/T2K_FHC_NC1pip_covar.txt");

  // Setup common settings
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("");
  fSettings.SetYTitle("#frac{d^{2}#sigma}{dpdcos#theta} (cm^{2}/nucleon/GeV)");
  fSettings.SetEnuRange(0.0, 30.0);
  fSettings.DefineAllowedTargets("C,H");
  FinaliseSampleSettings();

  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromTextFile(fSettings.GetCovarInput());
  ScaleCovar(1E76);

  // Funky labels
  fDataHist->GetXaxis()->SetBinLabel(1, "#splitline{c[0.5,0.7]}{p[0.2,0.6]}");
  fDataHist->GetXaxis()->SetBinLabel(2, "#splitline{c[0.7,0.8]}{p[0.2,0.6]}");
  fDataHist->GetXaxis()->SetBinLabel(3, "#splitline{c[0.5,0.8]}{p[0.6,1.0]}");
  fDataHist->GetXaxis()->SetBinLabel(4, "#splitline{c[0.8,0.9]}{p[0.2,0.4]}");
  fDataHist->GetXaxis()->SetBinLabel(5, "#splitline{c[0.8,0.9]}{p[0.4,0.6]}");
  fDataHist->GetXaxis()->SetBinLabel(6, "#splitline{c[0.8,0.9]}{p[0.6,1.0]}");
  fDataHist->GetXaxis()->SetBinLabel(7, "#splitline{c[0.9,1.0]}{p[0.2,0.4]}");
  fDataHist->GetXaxis()->SetBinLabel(8, "#splitline{c[0.9,1.0]}{p[0.4,0.6]}");
  fDataHist->GetXaxis()->SetBinLabel(9, "#splitline{c[0.9,1.0]}{p[0.6,1.0]}");

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * 1E-38 / fNEvents /
                 TotalIntegratedFlux();
  
  // Don't scale by width
  fIsNoWidth = true;

  FinaliseMeasurement();

  // Don't try to save a fine histogram
  fSaveFine = false;
};


bool T2K_NC1pip_XSec_lin2D::isSignal(FitEvent *event) {

  // Check that this is NC1piplus (also checks no other mesons)
  if (!SignalDef::isNC1pi(event, 14, 211) &&
      !SignalDef::isNC1pi(event, 12, 211) &&
      !SignalDef::isNC1pi(event, -14, 211) &&
      !SignalDef::isNC1pi(event, -12, 211)) return false;

  // Veto protons > ppr = 200 MeV
  if (event->NumFSParticle(2212)){
    double ppr  = FitUtils::p(event->GetHMFSParticle(2212)->fP);
    if (ppr > 0.2) return false;
  }

  return true;
};

void T2K_NC1pip_XSec_lin2D::FillEventVariables(FitEvent *event) {
  if (event->NumFSParticle(211) == 0) return;

  TLorentzVector Pnu = event->GetBeamNeutrinoP4();
  TLorentzVector Ppi = event->GetHMFSParticle(211)->fP;

  p_pi = FitUtils::p(Ppi);
  costh_pi = cos(FitUtils::th(Pnu, Ppi));
};


// An ugly function to return the correct bin
int GetBin(double p, double costh){
  
  if (p < 0.2 || p > 1.0) return -1;
  if (costh < 0.5) return -1;

  if (costh > 0.9){
    if (p > 0.6) return 8;
    if (p > 0.4) return 7;
    if (p > 0.2) return 6;
  }
  if (costh > 0.8){
    if (p > 0.6) return 5;
    if (p > 0.4) return 4;
    if (p > 0.2) return 3;
  }

  if (p > 0.6) return 2;
  if (costh > 0.7) return 1;
  return 0;
}


// Because the binning is weird, have to take over this method
void T2K_NC1pip_XSec_lin2D::FillHistograms() {

  if (!Signal) return;

  // As this is a linearised, irregular, 2D binning, this causes most of the hassle
  int bin = GetBin(p_pi, costh_pi);

  NUIS_LOG(DEB, "Fill MCHist: " << p_pi << "," << costh_pi << "(bin = " << bin << "), " << Weight);

  fMCHist->AddBinContent(bin+1, Weight);
  fMCStat->AddBinContent(bin+1, 1.0);
  if (fMCHist_Modes)
    fMCHist_Modes->Fill(Mode, bin+0.5, Weight);
  
  return;
};
