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
MiniBooNE_CCQE_XSec_1DQ2_antinu::MiniBooNE_CCQE_XSec_1DQ2_antinu(
    std::string name, std::string inputfile, FitWeight *rw, std::string type,
    std::string fakeDataFile) {
  //********************************************************************

  // Measurement Details
  fName = name;
  fPlotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  EnuMin = 0.;
  EnuMax = 3.;
  fNormError = 0.130;
  fDefaultTypes = "FIX/DIAG";
  fAllowedTypes = "FIX,FREE,SHAPE/DIAG";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Plots
  fPlotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  fCCQElike = name.find("CCQELike") != std::string::npos;

  if(fCCQElike){
    fMeasurementSpeciesType = kNumuWithWrongSignMeasurement;
  }

  fUseCorrectedCTarget = name.find("CTarg") != std::string::npos;

  if (fCCQElike && fUseCorrectedCTarget) {
    ERR(FTL) << "Sample: MiniBooNE_CCQE_XSec_1DQ2_antinu cannot run in both "
                "QELike and C-Target mode. You're welcome to add the data set."
             << std::endl;
    throw;
  }

  if(fUseCorrectedCTarget){
    SetDataValues(FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_con_ctarget.txt");
  } else {
    SetDataValues(FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_con.txt");
  }

  SetupDefaultHist();

  if (!fIsDiag)
    SetCovarMatrix(FitPar::GetDataBase() +
                   "/MiniBooNE/anti-ccqe/MiniBooNE_1DQ2_antinu.root");
  else {
    LOG(SAM) << "Making diagonal covar" << endl;
    fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
    covar = StatUtils::GetInvert(fFullCovar);
  }

  ///
  /// If CCQELike is used an additional the CCQELike BKG is used and a PDG
  /// Histogram is saved
  if (fCCQElike) {
    fDataHist_CCQELIKE = PlotUtils::GetTH1DFromFile(
        FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_bkg_ccqe.txt",
        (fName + "_data_CCQELIKE"), fPlotTitles);

    fDataHist_CCPIM = PlotUtils::GetTH1DFromFile(
        FitPar::GetDataBase() + "/MiniBooNE/anti-ccqe/asqq_bkg_ccpim.txt",
        (fName + "_data_CCPIM"), fPlotTitles);

    // Make NON CCPIM
    fDataHist_NONCCPIM = (TH1D *)fDataHist_CCQELIKE->Clone();
    fDataHist_NONCCPIM->SetNameTitle((fName + "_data_NONCCPIM").c_str(),
                                     (fName + "_data_NONCCPIM").c_str());

    // Perform fDataHist Sums
    for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
      fDataHist_NONCCPIM->SetBinContent(
          i + 1, fDataHist_CCQELIKE->GetBinContent(i + 1) -
                     fDataHist_CCPIM->GetBinContent(i + 1));
      fDataHist->SetBinContent(i + 1,
                               fDataHist->GetBinContent(i + 1) +
                                   fDataHist_CCQELIKE->GetBinContent(i + 1));
    }

    PlotUtils::CreateNeutModeArray((TH1D *)fMCHist, (TH1 **)fMCHist_CCQELIKE);
    PlotUtils::ResetNeutModeArray((TH1 **)fMCHist_CCQELIKE);

    PlotUtils::CreateNeutModeArray((TH1D *)fMCHist, (TH1 **)fMCHist_NONCCPIM);
    PlotUtils::ResetNeutModeArray((TH1 **)fMCHist_NONCCPIM);

    PlotUtils::CreateNeutModeArray((TH1D *)fMCHist, (TH1 **)fMCHist_CCPIM);
    PlotUtils::ResetNeutModeArray((TH1 **)fMCHist_CCPIM);
  }

  // ScaleFactor
  double NNucPerNTarg = fUseCorrectedCTarget ? 12.0/6.0 : 14.08/8.0;
  fScaleFactor = ((fEventHist->Integral("width") * 1E-38 / (fNEvents + 0.)) *
                  NNucPerNTarg / TotalIntegratedFlux());
};

//********************************************************************
/// @details Extract q2qe(fXVar) from the event
void MiniBooNE_CCQE_XSec_1DQ2_antinu::FillEventVariables(FitEvent *event) {
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
bool MiniBooNE_CCQE_XSec_1DQ2_antinu::isSignal(FitEvent *event) {
  //********************************************************************

  // If CC0pi, include both charges
  if (fCCQElike) {
    if (SignalDef::isCC0pi(event, 14, EnuMin, EnuMax) ||
        SignalDef::isCC0pi(event, -14, EnuMin, EnuMax)){
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

//********************************************************************
/// @details Fills a ccqe-like and ccpim background plot if required
void MiniBooNE_CCQE_XSec_1DQ2_antinu::FillHistograms() {
  //********************************************************************

  Measurement1D::FillHistograms();

  if (Mode != -1 && Mode != -2 and fCCQElike and Signal) {
    if (fabs(Mode) == 11 or fabs(Mode) == 13)
      PlotUtils::FillNeutModeArray(fMCHist_CCPIM, Mode, fXVar, Weight);
    else
      PlotUtils::FillNeutModeArray(fMCHist_NONCCPIM, Mode, fXVar, Weight);

    PlotUtils::FillNeutModeArray(fMCHist_CCQELIKE, Mode, fXVar, Weight);
  }
}

//********************************************************************
/// @details Extra write command to save the CCQELike/CCPIM PDG if required
void MiniBooNE_CCQE_XSec_1DQ2_antinu::Write(std::string drawOpt) {
  //********************************************************************

  Measurement1D::Write(drawOpt);

  if (fCCQElike) {
    fDataHist_CCQELIKE->Write();

    THStack combo_fMCHist_CCQELIKE = PlotUtils::GetNeutModeStack(
        (fName + "_MC_CCQELIKE").c_str(), (TH1 **)fMCHist_CCQELIKE, 0);
    combo_fMCHist_CCQELIKE.Write();

    fDataHist_CCPIM->Write();

    THStack combo_fMCHist_CCPIM = PlotUtils::GetNeutModeStack(
        (fName + "_MC_CCPIM").c_str(), (TH1 **)fMCHist_CCPIM, 0);
    combo_fMCHist_CCPIM.Write();

    fDataHist_NONCCPIM->Write();

    THStack combo_fMCHist_NONCCPIM = PlotUtils::GetNeutModeStack(
        (fName + "_MC_NONCCPIM").c_str(), (TH1 **)fMCHist_NONCCPIM, 0);
    combo_fMCHist_NONCCPIM.Write();
  }
}

//********************************************************************
/// @details Extra scale command for CCQELIKE/CCPIM PDG Hist
void MiniBooNE_CCQE_XSec_1DQ2_antinu::ScaleEvents() {
  //********************************************************************

  Measurement1D::ScaleEvents();

  if (fCCQElike) {
    PlotUtils::ScaleNeutModeArray((TH1 **)fMCHist_CCQELIKE, fScaleFactor,
                                  "width");
    PlotUtils::ScaleNeutModeArray((TH1 **)fMCHist_CCPIM, fScaleFactor, "width");
    PlotUtils::ScaleNeutModeArray((TH1 **)fMCHist_NONCCPIM, fScaleFactor,
                                  "width");
  }
}

//********************************************************************
/// @details Apply norm scaling to CCQELIKE/CCPIM PDG Hist
void MiniBooNE_CCQE_XSec_1DQ2_antinu::ApplyNormScale(double norm) {
  //********************************************************************

  Measurement1D::ApplyNormScale(norm);

  if (fCCQElike) {
    PlotUtils::ScaleNeutModeArray((TH1 **)fMCHist_CCQELIKE, 1.0 / norm, "");
    PlotUtils::ScaleNeutModeArray((TH1 **)fMCHist_CCPIM, 1.0 / norm, "");
    PlotUtils::ScaleNeutModeArray((TH1 **)fMCHist_NONCCPIM, 1.0 / norm, "");
  }
}

//********************************************************************
///// @details Extra scale command for CCQELIKE PDG Hist
void MiniBooNE_CCQE_XSec_1DQ2_antinu::ResetAll() {
  //********************************************************************
  Measurement1D::ResetAll();
  if (fCCQElike) {
    PlotUtils::ResetNeutModeArray((TH1 **)fMCHist_CCQELIKE);
    PlotUtils::ResetNeutModeArray((TH1 **)fMCHist_CCPIM);
    PlotUtils::ResetNeutModeArray((TH1 **)fMCHist_NONCCPIM);
  }
}
