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

#include "MiniBooNE_CCQE_XSec_1DQ2_antinu.h"
#include <csignal>

MiniBooNE_CCQE_XSec_1DQ2_antinu::MiniBooNE_CCQE_XSec_1DQ2_antinu(
    nuiskey samplekey) {

  // Sample overview
  std::string descrip =
      "MiniBooNE CCQE/CC0pi sample. \n"
      "Target: CH2.08 \n"
      "Flux: CCQE  = Forward Horn Current numu \n"
      "      CC0pi = Forward Horn Current numu+numub \n"
      "Signal: CCQE  = True CCQE + True 2p2h (Mode == 1 or 2) \n"
      "        CC0pi = Events with 1 mu+/mu-, N nucleons, 0 other";

  // 1. Initalise sample Settings ---------------------------------------
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{QE} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 3.0);
  fSettings.SetSuggestedFlux(FitPar::GetDataBase() +
                             "/MiniBooNE/ccqe/mb_ccqe_flux.root");

  // Define input data information
  fSettings.FoundFill("name", "CCQELike", fCCQElike, true);
  fSettings.FoundFill("name", "CTarg", fUseCorrectedCTarget, true);

  if (fCCQElike && fUseCorrectedCTarget) {
    QTHROW("Sample: MiniBooNE_CCQE_XSec_1DQ2_antinu cannot run in both "
           "QELike and C-Target mode. You're welcome to add the data set.");
  }

  if (fCCQElike) {

    // CCQELike plot information
    fSettings.SetTitle("MiniBooNE #nu_#mu CCQE on CH");
    fSettings.SetDataInput(FitPar::GetDataBase() +
                           "/MiniBooNE/anti-ccqe/asqq_con.txt");
    fSettings.SetCovarInput(FitPar::GetDataBase() +
                            "/MiniBooNE/anti-ccqe/asqq_diagcovar");
    fSettings.SetDefault("ccqelikebkg_input",
                         FitPar::GetDataBase() +
                             "/MiniBooNE/anti-ccqe/asqq_bkg_ccqe.txt");
    fSettings.SetDefault("ccpimbkg_input",
                         FitPar::GetDataBase() +
                             "/MiniBooNE/anti-ccqe/asqq_bkg_ccpim.txt");
    fSettings.SetHasExtraHistograms(true);
    fSettings.DefineAllowedSpecies("numu,numub");
    fSettings.DefineAllowedTargets("C,H");

  } else if (!fUseCorrectedCTarget) {
    // CCQE Plot Information
    fSettings.SetTitle("MiniBooNE #nu_#mu CC0#pi on CH");
    fSettings.SetDataInput(FitPar::GetDataBase() +
                           "/MiniBooNE/anti-ccqe/asqq_con.txt");
    fSettings.SetCovarInput(FitPar::GetDataBase() +
                            "/MiniBooNE/anti-ccqe/asqq_diagcovar");
    fSettings.DefineAllowedSpecies("numu");
    fSettings.DefineAllowedTargets("C,H");

  } else {

    // CCQE Corrected Target Plot Information
    fSettings.SetTitle("MiniBooNE #nu_#mu CC0#pi on C");
    fSettings.SetDataInput(FitPar::GetDataBase() +
                           "/MiniBooNE/anti-ccqe/asqq_con_ctarget.txt");
    fSettings.SetCovarInput(FitPar::GetDataBase() +
                            "/MiniBooNE/anti-ccqe/asqq_diagcovar");
    fSettings.DefineAllowedSpecies("numu");
    fSettings.DefineAllowedTargets("C");
  }

  FinaliseSampleSettings();

  // 2. Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  // Multiply by 14.08/6.0 to get per neutron
  double NNucPerNTarg = fUseCorrectedCTarget ? 12.0 / 6.0 : 14.08 / 8.0;
  fScaleFactor =
      ((GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) *
       NNucPerNTarg / TotalIntegratedFlux());

  // 3. Plot Setup -------------------------------------------------------
  SetDataFromTextFile(fSettings.GetDataInput());
  SetCovarFromDiagonal();

  ///
  /// If CCQELike is used an additional the CCQELike BKG is used and a PDG
  /// Histogram is saved
  if (fCCQElike) {

    // CCQELike Data
    fDataHist_CCQELIKE =
        PlotUtils::GetTH1DFromFile(fSettings.GetS("ccqelikebkg_input"),
                                   fSettings.GetName() + "_CCQELIKE_data");
    fDataHist_CCQELIKE->SetNameTitle(
        (fSettings.Name() + "_CCQELIKE_BKG").c_str(),
        ("MiniBooNE #nu_#mu CCQE-Like Backgrounds" + fSettings.PlotTitles())
            .c_str());
    fDataHist->Add(fDataHist_CCQELIKE);
    SetAutoProcessTH1(fDataHist_CCQELIKE, kCMD_Write);

    // CCQELike MC
    fMCHist_CCQELIKE = new NuNuBarTrueModeStack(
        fSettings.Name() + "_CCQELIKE_MC",
        "CCQE-like MC" + fSettings.PlotTitles(), fDataHist_CCQELIKE);
    SetAutoProcessTH1(fMCHist_CCQELIKE);

    // Data CCRES
    fDataHist_CCPIM =
        PlotUtils::GetTH1DFromFile(fSettings.GetS("ccpimbkg_input"),
                                   fSettings.GetName() + "_CCPIM_BKG_data");
    fDataHist_CCPIM->SetNameTitle(
        (fSettings.Name() + "_CCPIM_data").c_str(),
        ("MiniBooNE #nu_#mu CCQE-Like Backgrounds" + fSettings.PlotTitles())
            .c_str());
    SetAutoProcessTH1(fDataHist_CCQELIKE, kCMD_Write);

    // MC CCRES
    fMCHist_CCPIM = new NuNuBarTrueModeStack(
        fSettings.Name() + "_CCPIM_BKG_MC",
        "CCQE-like BKG CC-RES" + fSettings.PlotTitles(), fDataHist_CCPIM);
    SetAutoProcessTH1(fMCHist_CCPIM);

    // Make NON CCPIM
    fDataHist_NONCCPIM = (TH1D *)fDataHist_CCQELIKE->Clone();
    fDataHist_NONCCPIM->SetNameTitle((fName + "_data_NONCCPIM").c_str(),
                                     (fName + "_data_NONCCPIM").c_str());
    fDataHist_NONCCPIM->Add(fDataHist_CCPIM, -1.0);
    SetAutoProcessTH1(fDataHist_NONCCPIM, kCMD_Write);

    fMCHist_NONCCPIM = new NuNuBarTrueModeStack(
        fSettings.Name() + "_NONCCPIM_BKG",
        "CCQE-like BKG CC-NonRES" + fSettings.PlotTitles(), fDataHist_NONCCPIM);
    SetAutoProcessTH1(fMCHist_NONCCPIM);
  }

  FinaliseMeasurement();
};

void MiniBooNE_CCQE_XSec_1DQ2_antinu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(PhysConst::pdg_muons) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  // The highest momentum mu+/mu-. The isSignal definition should make sure we
  // only
  // accept events we want, so no need to do an additional check here.
  TLorentzVector Pmu = event->GetHMFSParticle(PhysConst::pdg_muons)->fP;

  // Set X Variables
  fXVar = FitUtils::Q2QErec(Pmu, cos(Pnu.Vect().Angle(Pmu.Vect())), 30., false);
  fPDGnu = event->PDGnu();

  return;
};

bool MiniBooNE_CCQE_XSec_1DQ2_antinu::isSignal(FitEvent *event) {

  // If CC0pi, include both charges
  if (fCCQElike) {
    if (SignalDef::isCC0pi(event, 14, EnuMin, EnuMax) ||
        SignalDef::isCC0pi(event, -14, EnuMin, EnuMax)) {
      return true;
    }
  } else {
    if (SignalDef::isCCQELike(event, -14, EnuMin, EnuMax))
      return true;
  }

  return false;
};

void MiniBooNE_CCQE_XSec_1DQ2_antinu::FillExtraHistograms(
    MeasurementVariableBox *vars, double weight) {

  // No Extra Hists if not ccqelike
  if (!fCCQElike or !Signal)
    return;

  // Fill Stacks
  if (Mode != -1 and Mode != -2) {
    if (fabs(Mode) == 11 or fabs(Mode) == 12 or fabs(Mode == 13)) {
      fMCHist_CCPIM->Fill(fPDGnu, Mode, fXVar, weight);
    } else {
      fMCHist_NONCCPIM->Fill(fPDGnu, Mode, fXVar, weight);
    }
  }

  fMCHist_CCQELIKE->Fill(fPDGnu, Mode, fXVar, weight);
}
