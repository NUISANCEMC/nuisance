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

#include "MiniBooNE_CCQE_XSec_1DQ2_nu.h"

//********************************************************************
MiniBooNE_CCQE_XSec_1DQ2_nu::MiniBooNE_CCQE_XSec_1DQ2_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview
  std::string descrip = "MiniBooNE CCQE/CC0pi sample. \n" \
                        "Target: CH2.08 \n" \
                        "Flux: CCQE  = Forward Horn Current numu \n" \
                        "      CC0pi = Forward Horn Current numu+numub \n" \
                        "Signal: CCQE  = True CCQE + True 2p2h (Mode == 1 or 2) \n" \
                        "        CC0pi = Events with 1 mu+/mu-, N nucleons, 0 other";

  // 1. Initalise sample Settings ---------------------------------------
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{QE} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 3.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.SetSuggestedFlux( FitPar::GetDataBase() + "/MiniBooNE/ccqe/mb_ccqe_flux.root");

  // Define input data information
  fSettings.FoundFill("name", "CCQELike", ccqelike, true);
  if (ccqelike) {
    // CCQELike plot information
    fSettings.SetTitle("MiniBooNE #nu_#mu CCQE");
    fSettings.SetDataInput(  FitPar::GetDataBase() + "/MiniBooNE/ccqe/asqq_like.txt" );
    fSettings.SetCovarInput( FitPar::GetDataBase() + "/MiniBooNE/ccqe/asqq_diagcovar" );
    fSettings.SetDefault( "ccqelikebkg_input", FitPar::GetDataBase() + "/MiniBooNE/ccqe/asqq_bkg.txt" );
    fSettings.SetHasExtraHistograms(true);
    fSettings.DefineAllowedSpecies("numu,numub");
  } else {
    // CCQE Plot Information
    fSettings.SetTitle("MiniBooNE #nu_#mu CC0#pi");
    fSettings.SetDataInput(  FitPar::GetDataBase() + "/MiniBooNE/ccqe/asqq_con.txt" );
    fSettings.SetCovarInput( FitPar::GetDataBase() + "/MiniBooNE/ccqe/asqq_diagcovar" );
    fSettings.DefineAllowedSpecies("numu");
  }

  FinaliseSampleSettings();

  // 2. Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  // Multiply by 14.08/6.0 to get per neutron
  fScaleFactor = ((GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) *
                  (14.08 / 6.0) / TotalIntegratedFlux());

  // 3. Plot Setup -------------------------------------------------------
  fDataHist  = PlotUtils::GetTH1DFromFile( fSettings.GetDataInput(), fSettings.GetName() );
  fDataHist->SetTitle( (fSettings.Title() + fSettings.PlotTitles()).c_str() );

  // fFullCovar = DataUtils::CovarFromText( fSettings.GetCovarInput() );

  /// If CCQELike is used the CCQELike BKG is saved.
  if (ccqelike) {
    fDataHist_CCQELIKE = PlotUtils::GetTH1DFromFile( fSettings.GetS("ccqelikebkg_input"),
                         fSettings.GetName() + "_CCQELIKEBKG_data" );
    SetAutoProcessTH1(fDataHist_CCQELIKE, kCMD_Write);

    fDataHist_CCQELIKE->SetNameTitle( (fSettings.Name() + "_CCQELIKE_BKG_data").c_str(),
                                      ("MiniBooNE #nu_#mu CCQE-Like Backgrounds" + fSettings.PlotTitles()).c_str() );

    fMCHist_CCQELIKETOTAL = (TH1D*) fDataHist_CCQELIKE->Clone();
    fMCHist_CCQELIKETOTAL->SetNameTitle( (fSettings.Name() + "_CCQELIKE_BKG_MC_TOTAL").c_str(),
                                         ("MiniBooNE #nu_#mu CCQE-Like Backgrounds MC" + fSettings.PlotTitles()).c_str() );
    SetAutoProcessTH1(fMCHist_CCQELIKETOTAL, kCMD_Reset, kCMD_Scale, kCMD_Norm, kCMD_Write);


    PlotUtils::CreateNeutModeArray((TH1D*)this->fDataHist, (TH1**)this->fMCHist_CCQELIKE);
    PlotUtils::ResetNeutModeArray((TH1**)this->fMCHist_CCQELIKE);
  }

  // 4. Final Check for all requirements, necessary to setup all extra plots.
  // If any are NULL it sets up DecompCovar, InvCovar, ModeHist, FineHist, StatHist, MaskHist
  // MapHist.
  FinaliseMeasurement();
};

//********************************************************************
/// @details Extract q2qe(fXVar) from the event
void  MiniBooNE_CCQE_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

  if (event->NumFSParticle(PhysConst::pdg_muons) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  // The highest momentum mu+/mu-. The isSignal definition should make sure we only
  // accept events we want, so no need to do an additional check here.
  TLorentzVector Pmu = event->GetHMFSParticle(PhysConst::pdg_muons)->fP;

  q2qe = FitUtils::Q2QErec(Pmu, cos(Pnu.Vect().Angle(Pmu.Vect())), 34., false);

  // Set X Variables
  fXVar = q2qe;

  return;
};

//********************************************************************
bool MiniBooNE_CCQE_XSec_1DQ2_nu::isSignal(FitEvent *event) {
//********************************************************************

  // If CC0pi, include both charges
  if (ccqelike) {
    if (SignalDef::isCC0pi(event, 14, EnuMin, EnuMax) ||
        SignalDef::isCC0pi(event, -14, EnuMin, EnuMax))
      return true;
  } else {
    if (SignalDef::isCCQELike(event, 14, EnuMin, EnuMax))
      return true;
  }

  return false;

};


void MiniBooNE_CCQE_XSec_1DQ2_nu::FillExtraHistograms(MeasurementVariableBox* vars, double weight) {

  // No Extra Hists if not ccqelike
  if (!ccqelike) return;

  if ((vars->fMode != 1 and vars->fMode != 2) and
      (vars->fSignal)) {
    PlotUtils::FillNeutModeArray(fMCHist_CCQELIKE, vars->fMode,
                                 vars->fX, weight);

  fMCHist_CCQELIKETOTAL->Fill(vars->fX, weight);
  }

  return;
}

