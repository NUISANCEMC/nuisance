// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "MicroBooNE_CC1MuNp_XSec_1D_nu.h"
#include "MicroBooNE_SignalDef.h"
#include "TH2D.h"

//********************************************************************
MicroBooNE_CC1MuNp_XSec_1D_nu::MicroBooNE_CC1MuNp_XSec_1D_nu(nuiskey samplekey) {
//********************************************************************
  fSettings = LoadSampleSettings(samplekey);
  std::string name = fSettings.GetS("name");
  std::string objSuffix;

  if (!name.compare("MicroBooNE_CC1MuNp_XSec_1DPmu_nu")) {
    fDist = kPmu;
    objSuffix = "mumom";
    fSettings.SetXTitle("P_{#mu}^{reco} (GeV)");
    fSettings.SetYTitle("d#sigma/dP_{#mu}^{reco} (cm^{2}/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1MuNp_XSec_1Dcosmu_nu")) {
    fDist = kCosMu;
    objSuffix = "muangle";
    fSettings.SetXTitle("cos#theta_{#mu}^{reco}");
    fSettings.SetYTitle("d#sigma/dcos#theta_{#mu}^{reco} (cm^{2}/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1MuNp_XSec_1DPp_nu")) {
    fDist = kPp;
    objSuffix = "pmom";
    fSettings.SetXTitle("P_{p}^{reco} (GeV)");
    fSettings.SetYTitle("d#sigma/dP_{p}^{reco} (cm^{2}/GeV/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1MuNp_XSec_1Dcosp_nu")) {
    fDist = kCosP;
    objSuffix = "pangle";
    fSettings.SetXTitle("cos#theta_{p}^{reco}");
    fSettings.SetYTitle("d#sigma/dcos#theta_{p}^{reco} (cm^{2}/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1MuNp_XSec_1Dthetamup_nu")) {
    fDist = kThetaMuP;
    objSuffix = "thetamup";
    fSettings.SetXTitle("#theta_{#mup}^{reco}");
    fSettings.SetYTitle("d#sigma/d#theta_{#mup}^{reco} (cm^{2}/^{40}Ar)");
  }
  else {
    assert(false);
  }

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n" \
                        "Target: Ar\n" \
                        "Flux: BNB FHC numu\n" \
                        "Signal: CC1MuNp\n";

  fSettings.SetDescription(descrip);
  fSettings.SetTitle(name);
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 6.8);
  fSettings.DefineAllowedTargets("Ar");
  fSettings.DefineAllowedSpecies("numu");
  FinaliseSampleSettings();

  // Load data ---------------------------------------------------------
  std::string inputFile = FitPar::GetDataBase() + "/MicroBooNE/CC1MuNp/CCNp_data_MC_cov_dataRelease.root";
  SetDataFromRootFile(inputFile, "DataXsec_" + objSuffix);
  ScaleData(1E-38);

  // ScaleFactor for DiffXSec/cm2/Nucleus
  fScaleFactor = GetEventHistogram()->Integral("width") / fNEvents * 1E-38 / TotalIntegratedFlux();

  SetCovarFromRootFile(inputFile, "CovarianceMatrix_" + objSuffix);

  TFile* inputRootFile = TFile::Open(inputFile.c_str());
  assert(inputRootFile && inputRootFile->IsOpen());
  TH2D* hsmear = (TH2D*) inputRootFile->Get(("SmearingMatrix_" + objSuffix).c_str());
  assert(hsmear);
  fSmearingMatrix = (TH2D*) hsmear->Clone((name + "_smearingMatrix").c_str());
  fSmearingMatrix->SetDirectory(0);
  inputRootFile->Close();
  assert(fSmearingMatrix);

  // Normalize columns
  TH1D* hpx = fSmearingMatrix->ProjectionX("_smearing_py");
  for (int i=1; i<fSmearingMatrix->GetNbinsX()+1; i++) {
    for (int j=1; j<fSmearingMatrix->GetNbinsY()+1; j++) {
      double v = fSmearingMatrix->GetBinContent(i, j) / hpx->GetBinContent(i);
      fSmearingMatrix->SetBinContent(i, j, v);
    }
  }
  delete hpx;

  // Final setup ------------------------------------------------------
  FinaliseMeasurement();
};


bool MicroBooNE_CC1MuNp_XSec_1D_nu::isSignal(FitEvent* event) {
  return SignalDef::MicroBooNE::isCC1MuNp(event, EnuMin, EnuMax);
};


void MicroBooNE_CC1MuNp_XSec_1D_nu::FillEventVariables(FitEvent* event) {
  if (fDist == kPmu) {
    if (event->NumFSParticle(13) == 0) return;
    fXVar = event->GetHMFSParticle(13)->fP.Vect().Mag() / 1000;
  }
  else if (fDist == kCosMu) {
    if (event->NumFSParticle(13) == 0) return;
    fXVar = event->GetHMFSParticle(13)->fP.Vect().CosTheta();
  }
  else if (fDist == kPp) {
    if (event->NumFSParticle(2212) == 0) return;
    fXVar = event->GetHMFSParticle(2212)->fP.Vect().Mag() / 1000;
  }
  else if (fDist == kCosP) {
    if (event->NumFSParticle(2212) == 0) return;
    fXVar = event->GetHMFSParticle(2212)->fP.Vect().CosTheta();
  }
  else if (fDist == kThetaMuP) {
    if (event->NumFSParticle(13) == 0) return;
    if (event->NumFSParticle(2212) == 0) return;
    TVector3 vpmu = event->GetHMFSParticle(13)->fP.Vect();
    TVector3 vppl = event->GetHMFSParticle(2212)->fP.Vect();
    fXVar = vpmu.Angle(vppl);
  }
}


void MicroBooNE_CC1MuNp_XSec_1D_nu::ConvertEventRates() {
  // Do standard conversion
  Measurement1D::ConvertEventRates();

  // Apply MC truth -> reco smearing
  TH1D* truth = (TH1D*) fMCHist->Clone(TString(fMCHist->GetName()) + "_truth");
  assert(fSmearingMatrix->GetNbinsX() == fSmearingMatrix->GetNbinsY());
  for (int ireco=1; ireco<fSmearingMatrix->GetNbinsY()+1; ireco++) {
    double total = 0;
    for (int itrue=1; itrue<fSmearingMatrix->GetNbinsX()+1; itrue++) {
      total += truth->GetBinContent(itrue) * truth->GetBinWidth(itrue) * fSmearingMatrix->GetBinContent(itrue, ireco);
    }
    fMCHist->SetBinContent(ireco, total / truth->GetBinWidth(ireco));
  }
}

