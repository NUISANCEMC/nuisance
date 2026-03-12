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
  fSettings.SetXTitle("p_{#mu}-cos#theta_{#mu}");
  fSettings.SetYTitle("d^{2}#sigma/dp_{#mu}dcos#theta_{#mu} (cm^{2}/GeV/nucleon)");
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
  SetHistograms();
  covar = StatUtils::GetInvert(fFullCovar, true);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
  SetShapeCovar();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
  fSaveFine = false;
};

void T2K_CC1pip_CH_XSec_2Dpmucosmu_nu::SetHistograms() {

  TFile *data = new TFile(fSettings.GetDataInput().c_str(), "open");
  std::string dataname = "p_mu_theta_mu";

  // Number of slices we have
  const int nslices = 4;
  int nbins = 0;
  for (int i = 0; i < nslices; ++i) {
    TH1D *slice = (TH1D *)data->Get(Form("%s_%i", dataname.c_str(), i));
    slice = (TH1D *)slice->Clone((fName + Form("_data_Slice%i", i)).c_str());
    slice->Scale(1E-38);
    slice->GetXaxis()->SetTitle("p_{#mu} (GeV/c)");
    slice->GetYaxis()->SetTitle(fSettings.GetYTitle().c_str());
    fDataHist_Slices.push_back(slice);

    fMCHist_Slices.push_back(
        (TH1D *)slice->Clone((fName + Form("_MC_Slice%i", i)).c_str()));
    
    SetAutoProcessTH1(fDataHist_Slices[i], kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i]);

    fMCHist_Slices[i]->Reset();
    fMCHist_Slices[i]->SetLineColor(kRed);

    // Skip the highest momentum bin because it's rubbish
    nbins += slice->GetXaxis()->GetNbins() - 1;

    // Have our fMCHist_Slices now, make the mode histograms and finely binned histograms
    fMCHist_Slices_Mode.push_back(new TrueModeStack(Form("%s_Slice%i_MODES", fSettings.GetName().c_str(), int(i)), ("True Channels"), fMCHist_Slices[i]));

    // Make the fine and mode histograms
    std::string titles = fSettings.GetFullTitles();
    fMCHist_Slices_Fine.push_back(new TH1D(
          Form("%s_MC_Slice%i_FINE", fName.c_str(), i), 
          Form("%s_MC_Slice%i_FINE%s", fName.c_str(), i, titles.c_str()), 
          100, 
          fMCHist_Slices[i]->GetXaxis()->GetBinLowEdge(1), 
          fMCHist_Slices[i]->GetXaxis()->GetBinLowEdge(fMCHist_Slices[i]->GetXaxis()->GetNbins()+1)));

    fMCHist_Slices_Fine_Mode.push_back(new TrueModeStack(Form("%s_Slice%i_MODES_FINE", fSettings.GetName().c_str(), int(i)), ("True Channels"), fMCHist_Slices_Fine[i]));

    SetAutoProcessTH1(fMCHist_Slices_Fine[i]);
    SetAutoProcessTH1(fMCHist_Slices_Mode[i], kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices_Fine_Mode[i], kCMD_Write);

    fMCHist_Slices_Fine[i]->GetXaxis()->SetTitle("p_{#mu} (GeV/c)");
    //fMCHist_Slices_Mode[i]->GetXaxis()->SetTitle("p_{#mu} (GeV/c)");
    //fMCHist_Slices_Fine_Mode[i]->GetXaxis()->SetTitle("p_{#mu} (GeV/c)");

    // Auto-process each individual mode histogram, only scale (get written above)
    for (size_t j = 0; j < fMCHist_Slices_Mode[i]->fAllLabels.size(); ++j) {
      SetAutoProcessTH1(fMCHist_Slices_Mode[i]->fAllHists[j], kCMD_Scale, kCMD_Norm, kCMD_Reset);
      SetAutoProcessTH1(fMCHist_Slices_Fine_Mode[i]->fAllHists[j], kCMD_Scale, kCMD_Norm, kCMD_Reset);
    }

  }

  fDataHist = new TH1D(dataname.c_str(), dataname.c_str(), nbins, 0, nbins);
  fDataHist->SetNameTitle((fName + "_data").c_str(), (fName + "_data").c_str());
  int bincount = 1;
  for (int i = 0; i < nslices; ++i) {
    // Skip the highest momentum bin because it's rubbish
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
  // Skip the highest momentum bin because it's rubbish
  fFullCovar = new TMatrixDSym(ncovbins - 4);
  if (fFullCovar->GetNrows() != fDataHist->GetXaxis()->GetNbins()*fDataHist->GetYaxis()->GetNbins()) {
    NUIS_ERR(FTL, "Number of bins in covariance matrix does not match data");
  }

  // Number of costhetamu slices is nslices
  // Number of pmu slices is
  int count1 = 0;
  // Skip the highest momentum bin because it's rubbish
  for (int i = 0; i < ncovbins - 4; ++i) {
    int count2 = 0;
    // Skip the highest momentum bin because it's rubbish
    for (int j = 0; j < ncovbins - 4; ++j) {
      // 1E79 matched to diagonal error
      (*fFullCovar)(count1, count2) = (*temp)(i, j);
      count2++;
    }
    count1++;
  }

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
    fMCHist_Slices_Fine[i]->GetSumw2();
    for (size_t j = 0; j < fMCHist_Slices_Mode[i]->fAllLabels.size(); ++j) {
      fMCHist_Slices_Mode[i]->fAllHists[j]->GetSumw2();
      fMCHist_Slices_Fine_Mode[i]->fAllHists[j]->GetSumw2();
    }
  }

  // Do standard conversion.
  Measurement1D::ConvertEventRates();

  // First scale MC slices also by their width in Y and Z
  for (int i = 0; i < nslices; ++i) {
    double scaling = 1;
    if      (i == 0) scaling = 1.0/0.80;
    else if (i == 1) scaling = 1.0/0.05;
    else if (i == 2) scaling = 1.0/0.05;
    else if (i == 3) scaling = 1.0/0.10;

    fMCHist_Slices[i]->Scale(scaling);
    fMCHist_Slices_Fine[i]->Scale(scaling);
    fMCHist_Slices_Mode[i]->Scale(scaling);
    fMCHist_Slices_Fine_Mode[i]->Scale(scaling);
  }

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
  int bin = -1;
  // Hard code the bin edges in here
  if (cosmu < 0.8) {
    bin = 0;
  } else if (cosmu > 0.8 && cosmu < 0.85) {
    bin = 1;
  } else if (cosmu > 0.85 && cosmu < 0.90) {
    bin = 2;
  } else if (cosmu > 0.90 && cosmu < 1.00) {
    bin = 3;
  }

  if (bin == -1) {
    std::cerr << "Bin -1 for event " << pmu << " " << cosmu << std::endl;
    throw;
  }

  fMCHist_Slices[bin]->Fill(pmu, weight);
  fMCHist_Slices_Fine[bin]->Fill(pmu, weight);
  fMCHist_Slices_Mode[bin]->Fill(Mode, pmu, weight);
  fMCHist_Slices_Fine_Mode[bin]->Fill(Mode, pmu, weight);

};

//********************************************************************
bool T2K_CC1pip_CH_XSec_2Dpmucosmu_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC1pip_T2K_arxiv1909_03936(event, EnuMin, EnuMax,
      SignalDef::kMuonFwd);
};
