#include "T2K_CC1pip_CH_XSec_2Dpmucosmu_nu.h"
#include "T2K_SignalDef.h"

//********************************************************************
T2K_CC1pip_CH_XSec_2Dpmucosmu_nu::T2K_CC1pip_CH_XSec_2Dpmucosmu_nu(
    nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC1pip_CH_XSec_nu sample. \n"
                        "Target: CH \n"
                        "Flux: T2K FHC numu \n"
                        "Signal: CC1pi+, costheta_mu > 0"
                        "https://arxiv.org/abs/1909.03936";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle(" ");
  fSettings.SetYTitle(
      "d^{2}#sigma/dp_{#mu}dcos#theta_{#mu} (cm^{2}/(GeV/c)/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("T2K_CC1pip_CH_XSec_2Dpmucosmu_nu");
  fSettings.SetDataInput(GeneralUtils::GetTopLevelDir() +
                         "/data/T2K/CC1pip/CH/PmuThetamu.root");
  fSettings.SetCovarInput(GeneralUtils::GetTopLevelDir() +
                          "/data/T2K/CC1pip/CH/PmuThetamu.root");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38) /
                 double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  // SetDataValues(  fSettings.GetDataInput() );
  // SetCovarMatrix( fSettings.GetCovarInput() );
  SetHistograms();
  // fFullCovar = StatUtils::GetCovarFromRootFile(fSettings.GetCovarInput(),
  //"Covariance_pmu_thetamu");
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
  SetShapeCovar();
  /*
  for (int i = 0; i < covar->GetNrows(); ++i) {
    for (int j = 0; j < covar->GetNrows(); ++j) {
      if (i == j) std::cout << i << " " << j << " = " << 1/sqrt((*covar)(i,j))
  << std::endl;
    }
  }
  throw;
  */

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

void T2K_CC1pip_CH_XSec_2Dpmucosmu_nu::SetHistograms() {

  TFile *data = new TFile(fSettings.GetDataInput().c_str(), "open");
  // std::string dataname = fSettings.Get
  std::string dataname = "p_mu_theta_mu";

  // Number of slices we have
  const int nslices = 4;
  int nbins = 0;
  for (int i = 0; i < nslices; ++i) {
    TH1D *slice = (TH1D *)data->Get(Form("%s_%i", dataname.c_str(), i));
    slice = (TH1D *)slice->Clone((fName + Form("_data_slice%i", i)).c_str());
    slice->Scale(1E-38);
    slice->GetXaxis()->SetTitle(fSettings.GetS("xtitle").c_str());
    slice->GetYaxis()->SetTitle(fSettings.GetS("ytitle").c_str());
    fDataHist_Slices.push_back(slice);
    fMCHist_Slices.push_back(
        (TH1D *)slice->Clone((fName + Form("_mc_slice%i", i)).c_str()));
    SetAutoProcessTH1(fDataHist_Slices[i], kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i]);
    fMCHist_Slices[i]->Reset();
    fMCHist_Slices[i]->SetLineColor(kRed);
    // nbins += slice->GetXaxis()->GetNbins();
    nbins += slice->GetXaxis()->GetNbins() - 1;
  }

  fDataHist = new TH1D(dataname.c_str(), dataname.c_str(), nbins, 0, nbins);
  fDataHist->SetNameTitle((fName + "_data").c_str(), (fName + "_data").c_str());
  int bincount = 1;
  for (int i = 0; i < nslices; ++i) {
    for (int j = 0; j < fDataHist_Slices[i]->GetXaxis()->GetNbins() - 1; ++j) {
      fDataHist->SetBinContent(bincount,
                               fDataHist_Slices[i]->GetBinContent(j + 1));
      fDataHist->SetBinError(bincount, fDataHist_Slices[i]->GetBinError(j + 1));
      TString title;
      if (j == 0) {
        title = "cos#theta_{#mu}=";
        if (i == 0) {
          title += "0-0.8, ";
        } else if (i == 1) {
          title += "0.8-0.85, ";
        } else if (i == 2) {
          title += "0.85-0.90, ";
        } else if (i == 3) {
          title += "0.90-1.00, ";
        }
      }
      title +=
          Form("p_{#mu}=%.2f-%.2f", fDataHist_Slices[i]->GetBinLowEdge(j + 1),
               fDataHist_Slices[i]->GetBinLowEdge(j + 2));
      fDataHist->GetXaxis()->SetBinLabel(bincount, title);
      bincount++;
    }
  }
  fDataHist->GetXaxis()->SetTitle(fSettings.GetS("xtitle").c_str());
  fDataHist->GetYaxis()->SetTitle(fSettings.GetS("ytitle").c_str());

  // Get the covariance
  TMatrixDSym *temp = StatUtils::GetCovarFromRootFile(fSettings.GetCovarInput(),
                                                      "Covariance_pmu_thetamu");
  int ncovbins = temp->GetNrows();
  fFullCovar = new TMatrixDSym(ncovbins - 4);
  if (ncovbins != fDataHist->GetXaxis()->GetNbins()) {
    NUIS_ERR(FTL, "Number of bins in covariance matrix does not match data");
  }

  // Number of costhetamu slices is nslices
  // Number of pmu slices is
  int count1 = 0;
  for (int i = 0; i < ncovbins - 4; ++i) {
    int count2 = 0;
    for (int j = 0; j < ncovbins - 4; ++j) {
      // 1E79 matched to diagonal error
      (*fFullCovar)(count1, count2) = (*temp)(i, j);
      count2++;
    }
    count1++;
  }

  // Now reorganise the rows

  delete temp;
};

void T2K_CC1pip_CH_XSec_2Dpmucosmu_nu::FillEventVariables(FitEvent *event) {
  if (event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double pmu = FitUtils::p(Pmu);
  double costhmu = cos(FitUtils::th(Pnu, Pmu));

  fXVar = pmu;
  fYVar = costhmu;
};

void T2K_CC1pip_CH_XSec_2Dpmucosmu_nu::FillHistograms() {
  Measurement1D::FillHistograms();
  if (Signal) {
    FillMCSlice(fXVar, fYVar, Weight);
  }
};

void T2K_CC1pip_CH_XSec_2Dpmucosmu_nu::ConvertEventRates() {

  const int nslices = 4;
  for (int i = 0; i < nslices; i++) {
    fMCHist_Slices[i]->GetSumw2();
  }

  // Do standard conversion.
  Measurement1D::ConvertEventRates();

  // First scale MC slices also by their width in Y and Z
  fMCHist_Slices[0]->Scale(1.0 / 0.80);
  fMCHist_Slices[1]->Scale(1.0 / 0.05);
  fMCHist_Slices[2]->Scale(1.0 / 0.05);
  fMCHist_Slices[3]->Scale(1.0 / 0.10);

  // Now Convert into 1D list
  fMCHist->Reset();
  int bincount = 1;
  for (int i = 0; i < nslices; i++) {
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX() - 1; j++) {
      fMCHist->SetBinContent(bincount, fMCHist_Slices[i]->GetBinContent(j + 1));
      bincount++;
    }
  }
}

void T2K_CC1pip_CH_XSec_2Dpmucosmu_nu::FillMCSlice(double pmu, double cosmu,
                                                   double weight) {
  // Hard code the bin edges in here
  if (cosmu < 0.8) {
    fMCHist_Slices[0]->Fill(pmu, weight);
  } else if (cosmu > 0.8 && cosmu < 0.85) {
    fMCHist_Slices[1]->Fill(pmu, weight);
  } else if (cosmu > 0.85 && cosmu < 0.90) {
    fMCHist_Slices[2]->Fill(pmu, weight);
  } else if (cosmu > 0.90 && cosmu < 1.00) {
    fMCHist_Slices[3]->Fill(pmu, weight);
  }
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_2Dpmucosmu_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC1pip_T2K_arxiv1909_03936(event, EnuMin, EnuMax,
                                                 SignalDef::kMuonFwd);
};
