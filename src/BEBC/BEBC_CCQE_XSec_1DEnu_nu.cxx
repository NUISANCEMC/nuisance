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

#include "BEBC_CCQE_XSec_1DEnu_nu.h"


//********************************************************************
BEBC_CCQE_XSec_1DEnu_nu::BEBC_CCQE_XSec_1DEnu_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "BEBC_CCQE_XSec_1DEnu_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("#sigma(E_{#nu}) (cm^{2}/nucleon)");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG/Q2CORR/MASK");
  fSettings.SetEnuRange(0.0, 200.0);
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("BEBC_CCQE_XSec_1DEnu_nu");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "BEBC/CCQE/BEBC_CCQE_1DEnu.csv");
   
  // is Q2 Correction applied
  applyQ2correction = fSettings.Found("type", "Q2CORR");
  if (applyQ2correction) {
    fSettings.SetS("q2correction_file",  FitPar::GetDataBase() + "/data/ANL/ANL_CCQE_Data_PRL31_844.root");
    fSettings.SetS("q2correction_hist", "ANL_XSec_1DEnu_Correction");
  }

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =  GetEventHistogram()->Integral("width")*double(1E-38)/(double(fNEvents));

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  // Data is in units of 1E-38
  fDataHist->Scale(1E-38);
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

}

//********************************************************************
void BEBC_CCQE_XSec_1DEnu_nu::FillEventVariables(FitEvent * event) {
//********************************************************************

  if (event->NumFSParticle(13) == 0)
    return;

  // Fill histogram with reconstructed Q2 Distribution
  fXVar = -999.9;
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  fXVar = Pnu.E()/1E3;

  GetQ2Box()->fQ2 = -(Pnu-Pmu).Mag2();
  return;
};

//********************************************************************
bool BEBC_CCQE_XSec_1DEnu_nu::isSignal(FitEvent * event) {
//********************************************************************

  if (!SignalDef::isCCQE(event, 14, EnuMin, EnuMax)) return false;

  // Q2 cut
  if (GetQ2Box()->fQ2 <= 0) return false;

  return true;
};

//********************************************************************
void BEBC_CCQE_XSec_1DEnu_nu::FillHistograms() {
//********************************************************************

  if (applyQ2correction) {
    fMCHist_NoCorr->Fill( GetQ2Box()->fQ2, Weight);

    if (GetQ2Box()->fQ2 < CorrectionHist->GetXaxis()->GetXmax() &&
        GetQ2Box()->fQ2 > CorrectionHist->GetXaxis()->GetXmin())
      Weight *= CorrectionHist->Interpolate(GetQ2Box()->fQ2);
  }

  Measurement1D::FillHistograms();

}

