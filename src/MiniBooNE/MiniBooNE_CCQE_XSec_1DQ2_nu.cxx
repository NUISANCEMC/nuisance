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

  // 1. Initalise sample Settings ---------------------------------------
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetXTitle("Q^{2}_{QE} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 3.0);
  fSettings.DefineAllowedTargets("C,H");

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
  fScaleFactor *= (14.08 / 6.0);

  // 3. Plot Setup -------------------------------------------------------
  // fDataHist  = PlotUtils::TH1DFromText( fSettings.GetDataInput() );
  // fFullCovar = DataUtils::CovarFromText( fSettings.GetCovarInput() );

  /// If CCQELike is used the CCQELike BKG is saved.
  if (ccqelike) {
    // fDataHist_CCQELIKE = PlotUtils::TH1DFromText( fSettings.GetS("ccqelikebkg_input") );
    fDataHist_CCQELIKE->SetNameTitle( (fSettings.Name() + "_CCQELIKE_BKG").c_str(),
                                      ("MiniBooNE #nu_#mu CCQE-Like Backgrounds" + fSettings.PlotTitles()).c_str() );

    PlotUtils::CreateNeutModeArray((TH1D*)this->fMCHist, (TH1**)this->fMCHist_CCQELIKE);
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

//********************************************************************
/// @details Fills a ccqe-like background plot if required
void MiniBooNE_CCQE_XSec_1DQ2_nu::FillHistograms() {
//********************************************************************

  Measurement1D::FillHistograms();

  if (Mode != 1 and Mode != 2 and ccqelike and Signal) {
    PlotUtils::FillNeutModeArray(fMCHist_CCQELIKE, Mode, fXVar, Weight);
  }
}


//********************************************************************
/// @details Extra write command to save the CCQELike PDG if required
void MiniBooNE_CCQE_XSec_1DQ2_nu::Write(std::string drawOpt) {
//********************************************************************
  // Add renaming option to Measurement1D
  Measurement1D::Write(drawOpt);

  if (ccqelike) {
    fDataHist_CCQELIKE->Write();

    THStack combo_fMCHist_CCQELIKE = PlotUtils::GetNeutModeStack((this->fName + "_MC_CCQELIKE").c_str(), (TH1**)this->fMCHist_CCQELIKE, 0);
    combo_fMCHist_CCQELIKE.Write();
  }

}


//********************************************************************
/// @details Extra scale command for CCQELIKE PDG Hist
void MiniBooNE_CCQE_XSec_1DQ2_nu::ResetAll() {
//********************************************************************

  if (ccqelike)
    PlotUtils::ResetNeutModeArray((TH1**)fMCHist_CCQELIKE);

}

//********************************************************************
/// @details Extra scale command for CCQELIKE PDG Hist
void MiniBooNE_CCQE_XSec_1DQ2_nu::ScaleEvents() {
//********************************************************************

  Measurement1D::ScaleEvents();
  if (ccqelike)
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_CCQELIKE, fScaleFactor, "width");

}


//********************************************************************
/// @details Apply norm scaling to CCQELIKE PDG Hist
void MiniBooNE_CCQE_XSec_1DQ2_nu::ApplyNormScale(double norm) {
//********************************************************************
  Measurement1D::ApplyNormScale(norm);
  if (ccqelike)
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_CCQELIKE, 1.0 / norm, "");
}
