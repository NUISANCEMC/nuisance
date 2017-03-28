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

#include "BNL_CCQE_XSec_1DEnu_nu.h"

//********************************************************************
BNL_CCQE_XSec_1DEnu_nu::BNL_CCQE_XSec_1DEnu_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "BNL CCQ2 XSec 1DEnu nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("#sigma(#E_{#nu}) (cm^{2}/neutron)");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG/Q2CORR/MASK");
  fSettings.SetEnuRange(0.0, 6.0);
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("BNL #nu_mu CCQE");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "BNL/BNL_Data_PRD23_2499.root;BNL_1DEnu_Data");

  // is Q2 Correction applied
  applyQ2correction = fSettings.Found("type", "Q2CORR");
  if (applyQ2correction) {
    fSettings.SetS("q2correction_file",  FitPar::GetDataBase() + "/ANL/ANL_CCQE_Data_PRL31_844.root");
    fSettings.SetS("q2correction_hist", "ANL_XSec_1DQ2_Correction");
  }

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor for shape
  fScaleFactor = (GetEventHistogram()->Integral("width") * (2.0 / 1.0) * 1E-38 / (fNEvents + 0.));

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile( fSettings.GetDataInput() );
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
void BNL_CCQE_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

  if (event->NumFSParticle(13) == 0)
    return;

  // Get Q2
  double q2qe = 0.0;
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  q2qe = FitUtils::Q2QErec(Pmu, cos(ThetaMu), 0., true);
  Enu_rec = FitUtils::EnuQErec(Pmu, cos(ThetaMu), 0., true);

  fXVar = Enu_rec;
  GetQ2Box()->fQ2 = q2qe;

  return;
};


//********************************************************************
bool BNL_CCQE_XSec_1DEnu_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCCQE(event, 14, EnuMin, EnuMax);
};


//********************************************************************
void BNL_CCQE_XSec_1DEnu_nu::FillHistograms() {
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
void BNL_CCQE_XSec_1DEnu_nu::ScaleEvents() {
//********************************************************************

  Measurement1D::ScaleEvents();

  // Flux unfold our extra histogram
  PlotUtils::FluxUnfoldedScaling(fMCHist_NoCorr, GetFluxHistogram(),
                                 GetEventHistogram(), fScaleFactor,
                                 fNEvents);

}




