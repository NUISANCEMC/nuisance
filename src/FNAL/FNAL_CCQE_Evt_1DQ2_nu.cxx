// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "FNAL_CCQE_Evt_1DQ2_nu.h"

// From Physical Review D, Volume 28, Number 3, 1 August 1983
// Title: High-energy quasielastic nu_mu n -> mu^- p scattering in deuterium
//********************************************************************
FNAL_CCQE_Evt_1DQ2_nu::FNAL_CCQE_Evt_1DQ2_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "FNAL_CCQE_Evt_1DQ2_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2} (GeV^{2})");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG/Q2CORR/MASK");
  fSettings.SetEnuRange(0.0, 600.0);
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("FNAL #nu_{#mu} CCQE");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "FNAL/CCQE/FNAL_CCQE_Data_PRD29_436.root;FNAL_CCQE_Data_1DQ2");

  // is Q2 Correction applied
  applyQ2correction = fSettings.Found("type", "Q2CORR");
  if (applyQ2correction) {
    fSettings.SetS("q2correction_file",  FitPar::GetDataBase() + "ANL/ANL_CCQE_Data_PRL31_844.root");
    fSettings.SetS("q2correction_hist", "ANL_XSec_1DQ2_Correction");
  }

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor for shape
  fScaleFactor = 1.0;

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile( fSettings.GetDataInput() );
  SetPoissonErrors();
  SetCovarFromDiagonal();

  // Correction Histogram
  if (applyQ2correction) {

    // Correction Hist
    CorrectionHist = PlotUtils::GetTH1DFromFile( fSettings.GetS("q2correction_file"),
                     fSettings.GetS("q2correction_hist") );
    SetAutoProcessTH1(CorrectionHist, kCMD_Write);


    // Make uncorrected MC hist
    fMCHist_NoCorr = (TH1D*) fDataHist->Clone();
    fMCHist_NoCorr->Reset();
    fMCHist_NoCorr->SetNameTitle( (fName + "_NOCORR").c_str(),
                                  (fName + "_NOCORR").c_str());
    SetAutoProcessTH1(fMCHist_NoCorr);
  }

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

  // Override hack
  fIsRawEvents = false;
}

//********************************************************************
void FNAL_CCQE_Evt_1DQ2_nu::FillEventVariables(FitEvent * event) {
//********************************************************************

  if (event->NumFSParticle(13) == 0)
    return;

  // Fill histogram with reconstructed Q2 Distribution
  fXVar = -999.9;
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  fXVar = FitUtils::Q2QErec(Pmu, cos(ThetaMu), 0., true);

  GetQ2Box()->fQ2 = fXVar;
  return;
};

//********************************************************************
bool FNAL_CCQE_Evt_1DQ2_nu::isSignal(FitEvent * event) {
//********************************************************************

  if (!SignalDef::isCCQE(event, 14, EnuMin, EnuMax)) return false;

  // Q2 cut
  if (GetQ2Box()->fQ2 <= 0) return false;

  return true;
};

//********************************************************************
void FNAL_CCQE_Evt_1DQ2_nu::FillHistograms() {
//********************************************************************

  if (applyQ2correction) {
    fMCHist_NoCorr->Fill( GetQ2Box()->fQ2, Weight);

    if (GetQ2Box()->fQ2 < CorrectionHist->GetXaxis()->GetXmax() &&
        GetQ2Box()->fQ2 > CorrectionHist->GetXaxis()->GetXmin())
      Weight *= CorrectionHist->Interpolate(GetQ2Box()->fQ2);
  }

  Measurement1D::FillHistograms();

}

//********************************************************************
void FNAL_CCQE_Evt_1DQ2_nu::ScaleEvents() {
//********************************************************************

  Measurement1D::ScaleEvents();

  // Flux unfold our extra histogram
  if (applyQ2correction) {
    if (fMCHist_NoCorr->Integral()) {
      fMCHist_NoCorr->Scale(fDataHist->Integral() / fMCHist_NoCorr->Integral());
    }
  }
}
