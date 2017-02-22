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

//********************************************************************
/// @brief MiniBooNE CCQE antinumu 1DQ2 Measurement on CH2 (Ref: - )
///
//********************************************************************
MiniBooNE_CCQE_XSec_1DQ2_antinu::MiniBooNE_CCQE_XSec_1DQ2_antinu(nuiskey samplekey) {
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
  fSettings.SetSuggestedFlux( FitPar::GetDataBase() + "/MiniBooNE/ccqe/mb_ccqe_flux.root");

  // Define input data information
  fSettings.FoundFill("name", "CCQELike", fCCQElike, true);
  fSettings.FoundFill("name", "CTarg", fUseCorrectedCTarget, true);

  if (fCCQElike && fUseCorrectedCTarget) {
    ERR(FTL) << "Sample: MiniBooNE_CCQE_XSec_1DQ2_antinu cannot run in both "
             "QELike and C-Target mode. You're welcome to add the data set."
             << std::endl;
    throw;
  }

  if (fCCQElike) {
    // CCQELike plot information
    fSettings.SetTitle("MiniBooNE #nu_#mu CCQE on CH");
    fSettings.SetDataInput(  FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_like.txt" );
    fSettings.SetCovarInput( FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_diagcovar" );
    fSettings.SetDefault( "ccqelikebkg_input", FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_bkg_ccqe.txt" );
    fSettings.SetDefault( "ccpimbkg_input", FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_bkg_ccpim.txt" );
    fSettings.SetHasExtraHistograms(true);
    fSettings.DefineAllowedSpecies("numu,numub");
    fSettings.DefineAllowedTargets("C,H");
  } else if (!fUseCorrectedCTarget) {
    // CCQE Plot Information
    fSettings.SetTitle("MiniBooNE #nu_#mu CC0#pi on CH");
    fSettings.SetDataInput(  FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_con.txt" );
    fSettings.SetCovarInput( FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_diagcovar" );
    fSettings.DefineAllowedSpecies("numu");
    fSettings.DefineAllowedTargets("C,H");
  } else {
    // CCQE Corrected Target Plot Information
    fSettings.SetTitle("MiniBooNE #nu_#mu CC0#pi on C");
    fSettings.SetDataInput(  FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_con_ctarget.txt" );
    fSettings.SetCovarInput( FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_diagcovar" );
    fSettings.DefineAllowedSpecies("numu");
    fSettings.DefineAllowedTargets("C,H");
  }

  FinaliseSampleSettings();

  // 2. Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  // Multiply by 14.08/6.0 to get per neutron
  double NNucPerNTarg = fUseCorrectedCTarget ? 12.0 / 6.0 : 14.08 / 8.0;
  fScaleFactor = ((GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) *
                  NNucPerNTarg / TotalIntegratedFlux());

  // 3. Plot Setup -------------------------------------------------------
  std::cout << "Loading Data" << std::endl;
  fDataHist  = PlotUtils::GetTH1DFromFile( fSettings.GetDataInput(), fSettings.GetName() );
  fDataHist->SetTitle( (fSettings.Title() + fSettings.PlotTitles()).c_str() );


  ///
  /// If CCQELike is used an additional the CCQELike BKG is used and a PDG
  /// Histogram is saved
  std::cout << "Loading CCQELike" << std::endl;
  if (fCCQElike) {
    // CCQELike
    fDataHist_CCQELIKE = PlotUtils::GetTH1DFromFile( fSettings.GetS("ccqelikebkg_input"),
                         fSettings.GetName() + "_data" );
    SetAutoProcessTH1(fDataHist_CCQELIKE, kCMD_Write);

    fDataHist_CCQELIKE->SetNameTitle( (fSettings.Name() + "_CCQELIKE_BKG").c_str(),
                                      ("MiniBooNE #nu_#mu CCQE-Like Backgrounds" + fSettings.PlotTitles()).c_str() );

    fMCHist_CCQELIKE = new NuNuBarTrueModeStack( fSettings.Name() + "_CCQELIKE_BKG_MC",
        "CCQE-like BKG MC" + fSettings.PlotTitles(),
        fDataHist_CCQELIKE );
    SetAutoProcessTH1(fMCHist_CCQELIKE, kCMD_Reset, kCMD_Scale, kCMD_Norm, kCMD_Write);


    // CCPIM
    fDataHist_CCPIM = PlotUtils::GetTH1DFromFile( fSettings.GetS("ccpimbkg_input"),
                      fSettings.GetName() + "_data" );
    fDataHist_CCPIM->SetNameTitle( (fSettings.Name() + "_CCPIM_BKG").c_str(),
                                   ("MiniBooNE #nu_#mu CCQE-Like Backgrounds" + fSettings.PlotTitles()).c_str() );
    PlotUtils::CreateNeutModeArray((TH1D*)this->fDataHist, (TH1**)this->fMCHist_CCPIM);
    PlotUtils::ResetNeutModeArray((TH1**)this->fMCHist_CCPIM);

    // Make NON CCPIM
    fDataHist_NONCCPIM = (TH1D *)fDataHist_CCQELIKE->Clone();
    fDataHist_NONCCPIM->SetNameTitle((fName + "_data_NONCCPIM").c_str(),
                                     (fName + "_data_NONCCPIM").c_str());
    for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
      fDataHist_NONCCPIM->SetBinContent(i + 1, fDataHist_CCQELIKE->GetBinContent(i + 1)
                                        - fDataHist_CCPIM->GetBinContent(i + 1));
    }
    PlotUtils::CreateNeutModeArray((TH1D*)this->fDataHist, (TH1**)this->fMCHist_NONCCPIM);
    PlotUtils::ResetNeutModeArray((TH1**)this->fMCHist_NONCCPIM);


    // Create a new Stack


  }
  FinaliseMeasurement();

};

//********************************************************************
/// @details Extract q2qe(fXVar) from the event
void MiniBooNE_CCQE_XSec_1DQ2_antinu::FillEventVariables(FitEvent * event) {
  //********************************************************************

  if (event->NumFSParticle(PhysConst::pdg_muons) == 0) return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  // The highest momentum mu+/mu-. The isSignal definition should make sure we
  // only
  // accept events we want, so no need to do an additional check here.
  TLorentzVector Pmu = event->GetHMFSParticle(PhysConst::pdg_muons)->fP;

  // Set X Variables
  fXVar = FitUtils::Q2QErec(Pmu, cos(Pnu.Vect().Angle(Pmu.Vect())), 30., false);

  return;
};

//********************************************************************
bool MiniBooNE_CCQE_XSec_1DQ2_antinu::isSignal(FitEvent * event) {
  //********************************************************************

  // If CC0pi, include both charges
  if (fCCQElike) {
    if (SignalDef::isCC0pi(event, 14, EnuMin, EnuMax) ||
        SignalDef::isCC0pi(event, -14, EnuMin, EnuMax)) {
      // P. Stowell Removed this bad check for only pdg=14 types
      //      if(event->GetNeutrinoIn()->fPID == -14){
      //        return false;
      //      }
      return true;
    }
  } else {
    if (SignalDef::isCCQELike(event, -14, EnuMin, EnuMax)) return true;
  }

  return false;
};


void MiniBooNE_CCQE_XSec_1DQ2_antinu::FillExtraHistograms(MeasurementVariableBox* vars, double weight) {

  // No Extra Hists if not ccqelike
  if (!fCCQElike) return;

  // Use MiniBooNE box
  MiniBooNE_CCQELike_Box* mbbox = static_cast<MiniBooNE_CCQELike_Box*>(vars);

  if ((vars->fMode != 1 and vars->fMode != 2) and
      (vars->fSignal)) {

    if (fabs(vars->fMode) == 11 or fabs(vars->fMode == 13)) {
      PlotUtils::FillNeutModeArray(fMCHist_CCPIM, vars->fMode,
                                   vars->fX, weight);

    } else {
      PlotUtils::FillNeutModeArray(fMCHist_NONCCPIM, vars->fMode,
                                   vars->fX, weight);
    }

    fMCHist_CCQELIKE->Fill(mbbox->fPDGnu, vars->fMode, vars->fX, weight);
  }
}

 MeasurementVariableBox* MiniBooNE_CCQE_XSec_1DQ2_antinu::CreateBox() {
  if (fCCQElike){ return new MiniBooNE_CCQELike_Box(); }
  else { return new MeasurementVariableBox(); }
 };

