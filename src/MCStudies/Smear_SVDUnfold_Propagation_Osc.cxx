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

#include "Smear_SVDUnfold_Propagation_Osc.h"

#include "SmearceptanceUtils.h"

#include "OscWeightEngine.h"

//********************************************************************
Smear_SVDUnfold_Propagation_Osc::Smear_SVDUnfold_Propagation_Osc(
    nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip =
      "Simple measurement class for doing fake data oscillation studies.\n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);

  fSettings.SetTitle("Osc Studies");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("XXX");
  fSettings.SetYTitle("Number of events");
  fSettings.SetEnuRange(0.0, 50);
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.DefineAllowedTargets("*");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") / (fNEvents + 0.);

  // Plot Setup -------------------------------------------------------
  // Check that we have the relevant histograms specified.
  if (!samplekey.Has("NDDataHist") || !samplekey.Has("FDDataHist") ||
      !samplekey.Has("NDToSpectrumSmearingMatrix")) {
    THROW(
        "Expected to attributes named: NDDataHist, FDDataHist, "
        "NDToSpectrumSmearingMatrix on the Smear_SVDUnfold_Propagation_Osc "
        "sample "
        "tag.");
  }

  NDDataHist = NULL;
  FDDataHist = NULL;
  NDToSpectrumSmearingMatrix = NULL;
  fMCHist = NULL;
  fDataHist = NULL;

  std::vector<std::string> NDHistDescriptor =
      GeneralUtils::ParseToStr(samplekey.GetS("NDDataHist"), ",");
  if (NDHistDescriptor.size() == 2) {
    NDDataHist = PlotUtils::GetTH1DFromRootFile(NDHistDescriptor[0],
                                                NDHistDescriptor[1]);
  }

  if (!NDDataHist) {
    THROW("Attempted to load NDDataHist from the descriptor: \""
          << samplekey.GetS("NDDataHist")
          << "\", but failed. Does it look correct? \"<ROOTFILE>,<HISTNAME>\"");
  }

  std::vector<std::string> FDHistDescriptor =
      GeneralUtils::ParseToStr(samplekey.GetS("FDDataHist"), ",");
  if (FDHistDescriptor.size() == 2) {
    FDDataHist = PlotUtils::GetTH1DFromRootFile(FDHistDescriptor[0],
                                                FDHistDescriptor[1]);
  }

  if (!FDDataHist) {
    THROW("Attempted to load FDDataHist from the descriptor: \""
          << samplekey.GetS("FDDataHist")
          << "\", but failed. Does it look correct? \"<ROOTFILE>,<HISTNAME>\"");
  }

  std::vector<std::string> NDToEvSmearingDescriptor = GeneralUtils::ParseToStr(
      samplekey.GetS("NDToSpectrumSmearingMatrix"), ",");
  if (NDToEvSmearingDescriptor.size() == 2) {
    NDToSpectrumSmearingMatrix = PlotUtils::GetTH2DFromRootFile(
        NDToEvSmearingDescriptor[0], NDToEvSmearingDescriptor[1]);
  }

  if (!NDToSpectrumSmearingMatrix) {
    THROW("Attempted to load NDToSpectrumSmearingMatrix from the descriptor: \""
          << samplekey.GetS("NDToSpectrumSmearingMatrix")
          << "\", but failed. Does it look correct? \"<ROOTFILE>,<HISTNAME>\"");
  }

  TMatrixD NDToSpectrumResponseMatrix_l = SmearceptanceUtils::GetMatrix(
      SmearceptanceUtils::SVDGetInverse(NDToSpectrumSmearingMatrix));

  NDToSpectrumResponseMatrix.ResizeTo(NDToSpectrumResponseMatrix_l);
  NDToSpectrumResponseMatrix = NDToSpectrumResponseMatrix_l;

  TMatrixD SpectrumToFDResponseMatrix_l =
      SmearceptanceUtils::GetMatrix(NDToSpectrumSmearingMatrix);

  SpectrumToFDResponseMatrix.ResizeTo(SpectrumToFDResponseMatrix_l);
  SpectrumToFDResponseMatrix = SpectrumToFDResponseMatrix_l;

  fDataHist = FDDataHist;

  fDataHist->SetNameTitle((fSettings.GetName() + "_data").c_str(),
                          (fSettings.GetFullTitles()).c_str());
  SetCovarFromDiagonal();

  TruncateUpTo = 0;
  if (samplekey.Has("TruncateUpTo")) {
    TruncateUpTo = samplekey.GetI("TruncateUpTo");
    QLOG(SAM, "Allowed to truncate unfolding matrix by up to "
                  << TruncateUpTo
                  << " singular values to limit negative ENu spectra.");
  }

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

void Smear_SVDUnfold_Propagation_Osc::FillEventVariables(FitEvent *event){};

bool Smear_SVDUnfold_Propagation_Osc::isSignal(FitEvent *event) {
  return false;
}

void Smear_SVDUnfold_Propagation_Osc::ConvertEventRates(void) {
  TH1D *OscHist = NDToSpectrumSmearingMatrix->ProjectionY();
  OscHist->Clear();

  TRandom3 rnjesus;

  NDDataHist->Write("ERec_ND", TObject::kOverwrite);

  bool HasNegValue = false;
  int truncations = 0;
  do {
    if (truncations > TruncateUpTo) {
      THROW("Unfolded enu spectrum had negative values even after "
            << truncations << " SVD singular value truncations.");
    }

    // Unfold ND ERec -> Enu spectrum
    // ------------------------------
    SmearceptanceUtils::PushTH1ThroughMatrixWithErrors(
        NDDataHist, OscHist, NDToSpectrumResponseMatrix, 1000, false);

    std::stringstream ss("");
    ss << "Unfolded_ENuTrue_ND_trunc" << truncations;
    OscHist->Write(ss.str().c_str(), TObject::kOverwrite);

    HasNegValue = false;

    for (Int_t bi_it = 1; bi_it < OscHist->GetXaxis()->GetNbins() + 1;
         ++bi_it) {
      if (OscHist->GetBinContent(bi_it) < 0) {
        HasNegValue = true;
        break;
      }
    }

    if (HasNegValue) {
      TMatrixD NDToSpectrumResponseMatrix_l =
          SmearceptanceUtils::GetMatrix(SmearceptanceUtils::SVDGetInverse(
              NDToSpectrumSmearingMatrix, truncations));

      NDToSpectrumResponseMatrix.ResizeTo(NDToSpectrumResponseMatrix_l);
      NDToSpectrumResponseMatrix = NDToSpectrumResponseMatrix_l;
    }

    truncations++;
  } while (HasNegValue);

  // Apply FD/ND weights
  // ------------------------------
  OscHist->Write("ND_FD_Corrected_ENuTrue", TObject::kOverwrite);

  // Apply Oscillations
  // ------------------------------
  FitWeight *fw = FitBase::GetRW();
  OscWeightEngine *oscWE =
      dynamic_cast<OscWeightEngine *>(fw->GetRWEngine(kOSCILLATION));

  if (!oscWE) {
    THROW(
        "Couldn't load oscillation weight engine for sample: "
        "Smear_SVDUnfold_Propagation_Osc.");
  }

  for (Int_t bi_it = 1; bi_it < OscHist->GetXaxis()->GetNbins() + 1; ++bi_it) {
    double oscWeight =
        oscWE->CalcWeight(OscHist->GetXaxis()->GetBinCenter(bi_it), 14);
    OscHist->SetBinContent(bi_it, OscHist->GetBinContent(bi_it) * oscWeight);
  }
  OscHist->Write("ENuTrue_FD", TObject::kOverwrite);

  TGraph POsc;

  POsc.Set(1E4 - 1);

  double min = OscHist->GetXaxis()->GetBinLowEdge(1);
  double step =
      (OscHist->GetXaxis()->GetBinUpEdge(OscHist->GetXaxis()->GetNbins()) -
       OscHist->GetXaxis()->GetBinLowEdge(1)) /
      double(1E4);

  for (size_t i = 1; i < 1E4; ++i) {
    double enu = min + i * step;
    double ow = oscWE->CalcWeight(enu, 14);
    if (ow != ow) {
      std::cout << "Bad osc weight for ENu: " << enu << std::endl;
    }
    POsc.SetPoint(i - 1, enu, ow);
  }

  POsc.Write("POsc", TObject::kOverwrite);

  return;

  // Forward fold Spectrum -> ERec FD
  // ------------------------------
  if (fMCHist) {
    fMCHist->SetDirectory(NULL);
    delete fMCHist;
  }

  fMCHist = static_cast<TH1D *>(FDDataHist->Clone());

  SmearceptanceUtils::PushTH1ThroughMatrixWithErrors(
      OscHist, fMCHist, SpectrumToFDResponseMatrix, 1000, false);

  fMCHist->Write("ERec_FD_pred", TObject::kOverwrite);
  FDDataHist->Write("ERec_FD_obs", TObject::kOverwrite);
}
