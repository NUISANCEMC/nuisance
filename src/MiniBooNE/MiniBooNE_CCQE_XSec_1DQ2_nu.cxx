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

  // Initalise sample Settings ---------------------------------------
  fSettings = LoadSampleSettings(samplekey);
  fSettings.FoundFill("name", "CCQELike", ccqelike, true);

  // Multiple constructors for similar samples
  if (ccqelike) Setup_MiniBooNE_CCQELike_XSec_1DQ2_nu();
  else Setup_MiniBooNE_CCQE_XSec_1DQ2_nu();

  // Final Check for all requirements, necessary to setup all extra plots.
  FinaliseMeasurement();
}

//********************************************************************
void MiniBooNE_CCQE_XSec_1DQ2_nu::Setup_MiniBooNE_CCQE_XSec_1DQ2_nu() {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MiniBooNE CCQE sample. \n" \
                        "Target: CH2.08 \n" \
                        "Flux: CCQE  = Forward Horn Current numu \n" \
                        "Signal: CCQE  = True CCQE + True 2p2h (Mode == 1 or 2) \n";

                        // Setup common settings
                        fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{QE} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 3.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.SetSuggestedFlux( FitPar::GetDataBase() + "/MiniBooNE/ccqe/mb_ccqe_flux.root");

  // CCQE Plot Information
  fSettings.SetTitle("MiniBooNE #nu_#mu CC0#pi");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/MiniBooNE/ccqe/asqq_con.txt" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // 2. Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  // Multiply by 14.08/6.0 to get per neutron
  fScaleFactor = ((GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) *
                  (14.08 / 6.0) / TotalIntegratedFlux());

  // 3. Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();
  
};

//********************************************************************
void MiniBooNE_CCQE_XSec_1DQ2_nu::Setup_MiniBooNE_CCQELike_XSec_1DQ2_nu() {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MiniBooNE CC0pi sample. \n" \
                        "Target: CH2.08 \n" \
                        "Flux: MiniBooNE Forward Horn Current numu \n" \
                        "Signal: Any event with 1 muon, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{QE} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 3.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.SetSuggestedFlux( FitPar::GetDataBase() + "/MiniBooNE/ccqe/mb_ccqe_flux.root");

  // CCQELike plot information
  fSettings.SetTitle("MiniBooNE #nu_#mu CCQE");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/MiniBooNE/ccqe/asqq_con.txt" );
  fSettings.SetCovarInput( FitPar::GetDataBase() + "/MiniBooNE/ccqe/asqq_diagcovar" );
  fSettings.SetDefault( "ccqelikebkg_input", FitPar::GetDataBase() + "/MiniBooNE/ccqe/asqq_bkg.txt" );
  fSettings.SetHasExtraHistograms(true);
  fSettings.DefineAllowedSpecies("numu,numub");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  // Multiply by 14.08/6.0 to get per neutron
  fScaleFactor = ((GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) *
                  (14.08 / 6.0) / TotalIntegratedFlux());

  // Plot Setup -------------------------------------------------------
  fDataHist  = PlotUtils::GetTH1DFromFile( fSettings.GetDataInput(), fSettings.GetName() );
  fDataHist->SetTitle( (fSettings.Title() + fSettings.PlotTitles()).c_str() );


  // Make Data CCQELike BKG
  fDataHist_CCQELIKE = PlotUtils::GetTH1DFromFile( fSettings.GetS("ccqelikebkg_input"),
                       fSettings.GetName() + "_CCQELIKEBKG_data" );
  fDataHist_CCQELIKE->SetNameTitle( (fSettings.Name() + "_CCQELIKE_BKG_data").c_str(),
                                    ("MiniBooNE #nu_#mu CCQE-Like Backgrounds" + fSettings.PlotTitles()).c_str() );
  fDataHist->Add(fDataHist_CCQELIKE);
  SetAutoProcessTH1(fDataHist_CCQELIKE, kCMD_Write);

  // Make MC Clone
  fMCHist_CCQELIKE = new TrueModeStack( fSettings.Name() + "_CCQELIKE_BKG_MC",
                                        "CCQE-like BKG MC" + fSettings.PlotTitles(),
                                        fDataHist_CCQELIKE);
  SetAutoProcessTH1(fMCHist_CCQELIKE);

};


//********************************************************************
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

  if ((Mode != 1 and Mode != 2) and (Signal)) {
    fMCHist_CCQELIKE->Fill(Mode, fXVar, weight);
  }

  return;
}

