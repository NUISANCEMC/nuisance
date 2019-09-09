#include "T2K_CCinc_XSec_2DPcos_nu_nonuniform.h"
#include "T2K_SignalDef.h"

// ***********************************
// Implemented by Alfonso Garcia, Barcelona (now NIKHEF)
//                Clarence Wret, Rochester
// (Alfonso was the T2K analyser)
// ***********************************

//********************************************************************
T2K_CCinc_XSec_2DPcos_nu_nonuniform::T2K_CCinc_XSec_2DPcos_nu_nonuniform(
    nuiskey samplekey) {
  //********************************************************************

  fAllowedTypes += "/GENIE/NEUT";

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CCinc_XSec_2DPcos_nu_nonuniform sample. \n"
                        "Target: CH \n"
                        "Flux: T2K FHC numu  \n"
                        "Signal: CC-inclusive \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("");
  // fSettings.SetXTitle("p_{#mu} (GeV)");
  // fSettings.SetYTitle("cos#theta_{#mu}");
  fSettings.SetYTitle("#frac{d^{2}#sigma}{dp_{#mu}dcos#theta_{#mu}} "
                      "[#frac{cm^{2}}{nucleon/GeV/c}]");
  fSettings.SetEnuRange(0.0, 30.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("T2K CC-inclusive p_{#mu} cos#theta_{#mu}");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * 1E-38 / fNEvents /
                 TotalIntegratedFlux();

  // Default to using the NEUT unfolded data
  UnfoldWithGENIE = false;
  // Check option
  if (fSettings.Found("type", "GENIE"))
    UnfoldWithGENIE = true;

  // Tell user what's happening
  if (UnfoldWithGENIE) {
    NUIS_LOG(SAM, fName << " is using GENIE unfolded data. Want NEUT? Specify "
                       "type=\"NEUT\" in your config file");
  } else {
    NUIS_LOG(SAM, fName << " is using NEUT unfolded data. Want GENIE? Specify "
                       "type=\"GENIE\" in your config file");
  }
  // Setup Histograms
  SetHistograms();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

// Signal is simply a CC inclusive without any angular/momentum cuts
bool T2K_CCinc_XSec_2DPcos_nu_nonuniform::isSignal(FitEvent *event) {
  return SignalDef::isCCINC(event, 14, EnuMin, EnuMax);
};

void T2K_CCinc_XSec_2DPcos_nu_nonuniform::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double pmu = Pmu.Vect().Mag() / 1000.;
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));

  fXVar = pmu;
  fYVar = CosThetaMu;
};

// Fill up the MCSlice
void T2K_CCinc_XSec_2DPcos_nu_nonuniform::FillHistograms() {
  if (Signal)
    FillMCSlice(fXVar, fYVar, Weight);
}

// Modification is needed after the full reconfigure to move bins around
void T2K_CCinc_XSec_2DPcos_nu_nonuniform::ConvertEventRates() {

  // Do standard conversion.
  Measurement1D::ConvertEventRates();

  // First scale MC slices also by their width in Y
  fMCHist_Slices[0]->Scale(1.0 / 0.25);
  fMCHist_Slices[1]->Scale(1.0 / 0.50);
  fMCHist_Slices[2]->Scale(1.0 / 0.20);
  fMCHist_Slices[3]->Scale(1.0 / 0.15);
  fMCHist_Slices[4]->Scale(1.0 / 0.11);
  fMCHist_Slices[5]->Scale(1.0 / 0.09);
  fMCHist_Slices[6]->Scale(1.0 / 0.07);
  fMCHist_Slices[7]->Scale(1.0 / 0.05);
  fMCHist_Slices[8]->Scale(1.0 / 0.04);
  fMCHist_Slices[9]->Scale(1.0 / 0.025);
  fMCHist_Slices[10]->Scale(1.0 / 0.015);

  // Now Convert into 1D list
  fMCHist->Reset();
  int bincount = 0;
  for (int i = 0; i < nSlices; i++) {
    for (int j = 0; j < fMCHist_Slices[i]->GetNbinsX(); j++) {
      fMCHist->SetBinContent(bincount + 1,
                             fMCHist_Slices[i]->GetBinContent(j + 1));
      fMCHist->SetBinError(bincount + 1, fMCHist_Slices[i]->GetBinError(j + 1));
      bincount++;
    }
  }
};

void T2K_CCinc_XSec_2DPcos_nu_nonuniform::FillMCSlice(double x, double y,
                                                      double w) {
  if (y >= -1.0 && y < -0.25)
    fMCHist_Slices[0]->Fill(x, w);
  else if (y >= -0.25 && y < 0.25)
    fMCHist_Slices[1]->Fill(x, w);
  else if (y >= 0.25 && y < 0.45)
    fMCHist_Slices[2]->Fill(x, w);
  else if (y >= 0.45 && y < 0.6)
    fMCHist_Slices[3]->Fill(x, w);
  else if (y >= 0.6 && y < 0.71)
    fMCHist_Slices[4]->Fill(x, w);
  else if (y >= 0.71 && y < 0.80)
    fMCHist_Slices[5]->Fill(x, w);
  else if (y >= 0.80 && y < 0.87)
    fMCHist_Slices[6]->Fill(x, w);
  else if (y >= 0.87 && y < 0.92)
    fMCHist_Slices[7]->Fill(x, w);
  else if (y >= 0.92 && y < 0.96)
    fMCHist_Slices[8]->Fill(x, w);
  else if (y >= 0.96 && y <= 0.985)
    fMCHist_Slices[9]->Fill(x, w);
  else if (y >= 0.985 && y <= 1.0)
    fMCHist_Slices[10]->Fill(x, w);
};

void T2K_CCinc_XSec_2DPcos_nu_nonuniform::SetHistograms() {

  // Read in 1D Data Histograms
  TFile *fInputFile =
      new TFile((FitPar::GetDataBase() +
                 "T2K/CCinc/nd280data-numu-cc-inc-xs-on-c-2018/histograms.root")
                    .c_str(),
                "OPEN");

  // Number of theta slices in the release
  nSlices = 11;

  // Data release includes unfolding with NEUT or GENIE as prior
  // Choose whichever the user specifies
  std::string basename;
  if (UnfoldWithGENIE)
    basename = "hist_xsec_data_prior_neut_cthbin";
  else
    basename = "hist_xsec_data_prior_neut_cthbin";

  // Read in 2D Data Slices and Make MC Slices
  // Count the number of bins we have in total so we can match covariance matrix
  int bincount = 0;

  for (int i = 0; i < nSlices; i++) {
    // Get Data Histogram
    // fDataHist_Slices.push_back((TH1D*)fInputFile->Get(Form("dataslice_%i",i))->Clone());
    fDataHist_Slices.push_back(
        (TH1D *)fInputFile->Get(Form("%s%i", basename.c_str(), i))
            ->Clone(
                Form("T2K_CCinc_XSec_2DPcos_nu_nonuniform_slice%i_data", i)));
    fDataHist_Slices[i]->SetDirectory(0);
    fDataHist_Slices[i]->Scale(1E-39);
    fDataHist_Slices[i]->GetYaxis()->SetTitle(fSettings.GetS("ytitle").c_str());

    // Count up the bins
    bincount += fDataHist_Slices.back()->GetNbinsX();

    // Make MC Clones
    fMCHist_Slices.push_back((TH1D *)fDataHist_Slices[i]->Clone(
        Form("T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice%i_MC", i)));
    fMCHist_Slices[i]->Reset();
    fMCHist_Slices[i]->SetDirectory(0);
    fMCHist_Slices[i]->SetLineColor(kRed);
    fMCHist_Slices[i]->GetYaxis()->SetTitle(fSettings.GetS("ytitle").c_str());

    SetAutoProcessTH1(fDataHist_Slices[i], kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i]);
  }

  fDataHist =
      new TH1D((fSettings.GetName() + "_data").c_str(),
               (fSettings.GetFullTitles()).c_str(), bincount, 0, bincount);
  fDataHist->SetDirectory(0);

  int counter = 0;
  for (int i = 0; i < nSlices; ++i) {
    // Set a nice title
    std::string costitle = fDataHist_Slices[i]->GetTitle();
    costitle = costitle.substr(costitle.find("-> ") + 3, costitle.size());
    std::string found = costitle.substr(0, costitle.find(" < "));
    std::string comp = costitle.substr(costitle.find(found) + found.size() + 3,
                                       costitle.size());
    comp = comp.substr(comp.find(" < ") + 3, comp.size());
    costitle = "cos#theta_{#mu}=" + found + "-" + comp;

    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++) {
      fDataHist->SetBinContent(counter + 1,
                               fDataHist_Slices[i]->GetBinContent(j + 1));
      fDataHist->SetBinError(counter + 1,
                             fDataHist_Slices[i]->GetBinError(j + 1));
      // Set a nice axis
      if (j == 0)
        fDataHist->GetXaxis()->SetBinLabel(
            counter + 1, Form("%s, p_{#mu}=%.1f-%.1f", costitle.c_str(),
                              fDataHist_Slices[i]->GetBinLowEdge(j + 1),
                              fDataHist_Slices[i]->GetBinLowEdge(j + 2)));
      else
        fDataHist->GetXaxis()->SetBinLabel(
            counter + 1,
            Form("p_{#mu}=%.1f-%.1f", fDataHist_Slices[i]->GetBinLowEdge(j + 1),
                 fDataHist_Slices[i]->GetBinLowEdge(j + 2)));
      counter++;
    }
  }

  // The correlation matrix
  // Major in angular bins, minor in momentum bins: runs theta1, pmu1, pmu2,
  // theta2, pmu1, pmu2, theta3, pmu1, pmu2 etc The correlation matrix
  TH2D *tempcov = NULL;
  if (UnfoldWithGENIE)
    tempcov = (TH2D *)fInputFile->Get("covariance_matrix_genie");
  else
    tempcov = (TH2D *)fInputFile->Get("covariance_matrix_neut");
  fFullCovar = new TMatrixDSym(bincount);
  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    for (int j = 0; j < fDataHist->GetNbinsX(); j++) {
      (*fFullCovar)(i, j) = tempcov->GetBinContent(i + 1, j + 1);
    }
  }
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
  (*fFullCovar) *= 1E38 * 1E38;
  (*covar) *= 1E-38 * 1E-38;

  fInputFile->Close();
};
