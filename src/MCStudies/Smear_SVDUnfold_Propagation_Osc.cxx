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

#include "HistogramInputHandler.h"

void Smear_SVDUnfold_Propagation_Osc::AddNDInputs(nuiskey &samplekey) {
  NDSample nds;

  // Plot Setup -------------------------------------------------------
  // Check that we have the relevant near detector histograms specified.

  if (!NDSamples.size()) {  // If this is the first ND sample, take from the
                            // sample input
    InputHandlerBase *InputBase = GetInput();
    if (InputBase->GetType() != kHISTO) {
      THROW(
          "Smear_SVDUnfold_Propagation_Osc expects a Histogram input that "
          "contains the ND observed spectrum.");
    }
    HistoInputHandler *HInput = dynamic_cast<HistoInputHandler *>(InputBase);
    if (!HInput) {
      THROW(
          "Smear_SVDUnfold_Propagation_Osc expects a Histogram input that "
          "contains the ND observed spectrum.");
    }
    if (HInput->NHistograms() != 2) {
      THROW(
          "Input expected to contain 2 histograms. "
          "HISTO:input.root[NDObs_TH1D,NDSmear_TH2D]");
    }
    nds.NDDataHist = dynamic_cast<TH1D *>(HInput->GetHistogram(0));
    nds.NDToSpectrumSmearingMatrix =
        dynamic_cast<TH2D *>(HInput->GetHistogram(1));

    if (!nds.NDDataHist) {
      THROW("Expected a valid TH1D input for the ND observed spectrum.");
    }

    if (!nds.NDToSpectrumSmearingMatrix) {
      THROW("Expected a valid TH2D input for the ND observed smearing.");
    }
  } else {
    std::vector<TH1 *> NDObsInputs =
        PlotUtils::GetTH1sFromRootFile(samplekey.GetS("ObsInput"));
    if (NDObsInputs.size() < 2) {
      THROW(
          "Near detector sample must contain the observed ERec spectrum and "
          "the "
          "ND ETrue/ERec smearing matrix. e.g. "
          "ObsInput=\"input.root[NDObs_species,NDSmearing_species]\"");
    }

    nds.NDDataHist = dynamic_cast<TH1D *>(NDObsInputs[0]);
    if (!nds.NDDataHist) {
      ERROR(FTL,
            "First histogram from ObsInput attribute was not a TH1D containing "
            "the near detector observed ERec spectrum ("
                << samplekey.GetS("ObsInput") << ").");
      THROW(
          "Near detector sample must contain the observed ERec spectrum and "
          "the "
          "ND ETrue/ERec smearing matrix. e.g. "
          "ObsInput=\"input.root[FDObs_species,FDSmearing_species]\"");
    }

    nds.NDToSpectrumSmearingMatrix = dynamic_cast<TH2D *>(NDObsInputs[1]);
    if (!nds.NDToSpectrumSmearingMatrix) {
      ERROR(
          FTL,
          "Second histogram from ObsInput attribute was not a TH2D containing "
          "the near detector ETrue/ERec smearing matrix ("
              << samplekey.GetS("ObsInput") << ").");
      THROW(
          "Near detector sample must contain the observed ERec spectrum and "
          "the "
          "ND ETrue/ERec smearing matrix. e.g. "
          "ObsInput=\"input.root[FDObs_species,FDSmearing_species]\"");
    }
  }

  nds.NDDataHist->Scale(ScalePOT);

  if (UseRateErrors) {
    for (Int_t bi_it = 1; bi_it < nds.NDDataHist->GetXaxis()->GetNbins() + 1;
         ++bi_it) {
      nds.NDDataHist->SetBinError(bi_it,
                                  sqrt(nds.NDDataHist->GetBinContent(bi_it)));
    }
  }

  nds.TruncateStart = 0;
  if (samplekey.Has("TruncateStart")) {
    nds.TruncateStart = samplekey.GetI("TruncateStart");
  }

  nds.TruncateUpTo = 0;
  if (samplekey.Has("TruncateUpTo")) {
    nds.TruncateUpTo = samplekey.GetI("TruncateUpTo");
    QLOG(SAM, "\tAllowed to truncate unfolding matrix by up to "
                  << nds.TruncateUpTo
                  << " singular values to limit negative ENu spectra.");
  }
  nds.NuPDG = 14;
  if (samplekey.Has("NuPDG")) {
    nds.NuPDG = samplekey.GetI("NuPDG");
  }

  NDSamples.push_back(nds);
}

void Smear_SVDUnfold_Propagation_Osc::SetupNDInputs() {
  for (size_t nd_it = 0; nd_it < NDSamples.size(); ++nd_it) {
    NDSample &nds = NDSamples[nd_it];

    TMatrixD NDToSpectrumResponseMatrix_l = SmearceptanceUtils::GetMatrix(
        SmearceptanceUtils::SVDGetInverse(nds.NDToSpectrumSmearingMatrix));

    nds.NDToSpectrumResponseMatrix.ResizeTo(NDToSpectrumResponseMatrix_l);
    nds.NDToSpectrumResponseMatrix = NDToSpectrumResponseMatrix_l;

    if (nds.TruncateStart != 0) {
      TMatrixD NDToSpectrumResponseMatrix_l =
          SmearceptanceUtils::GetMatrix(SmearceptanceUtils::SVDGetInverse(
              nds.NDToSpectrumSmearingMatrix, nds.TruncateStart));

      nds.NDToSpectrumResponseMatrix.ResizeTo(NDToSpectrumResponseMatrix_l);
      nds.NDToSpectrumResponseMatrix = NDToSpectrumResponseMatrix_l;
    }

    if (nds.TruncateStart >= nds.TruncateUpTo) {
      nds.TruncateUpTo = nds.TruncateStart + 1;
    }

    UnfoldToNDETrueSpectrum(nd_it);
  }
}

void Smear_SVDUnfold_Propagation_Osc::ReadExtraConfig(nuiskey &samplekey) {
  UseRateErrors = false;
  if (samplekey.Has("SetErrorsFromRate")) {
    UseRateErrors = samplekey.GetI("SetErrorsFromRate");
  }

  NDetectorInfo.first = 0xdeadbeef;
  if (samplekey.Has("DetectorVolume") && samplekey.Has("DetectorDensity")) {
    NDetectorInfo.first = samplekey.GetD("DetectorVolume");
    NDetectorInfo.second = samplekey.GetD("DetectorDensity");

    double TargetMass_kg = NDetectorInfo.first * NDetectorInfo.second;

    QLOG(SAM, "\tND sample detector mass : ");
    QLOG(SAM, "\t\tTarget volume : " << NDetectorInfo.first);
    QLOG(SAM, "\t\tTarget density : " << NDetectorInfo.second);
    QLOG(SAM, "\t\tTarget mass : " << TargetMass_kg << " kg");
  }

  ScalePOT = 1;
  if (samplekey.Has("ScalePOT")) {
    ScalePOT = samplekey.GetD("ScalePOT");
  }
}

void Smear_SVDUnfold_Propagation_Osc::AddFDTarget(nuiskey &nk) {
  FDSample fds;

  fds.FitRegion_Min = 0xdeadbeef;
  if (nk.Has("FitRegion_Min")) {
    fds.FitRegion_Min = nk.GetD("FitRegion_Min");
    QLOG(SAM, "FD Sample [" << FDSamples.size() << "] imposes FitRegion E_nu > "
                            << fds.FitRegion_Min);
  }
  if ((FitRegion_Min == 0xdeadbeef) || FitRegion_Min > fds.FitRegion_Min) {
    FitRegion_Min = fds.FitRegion_Min;
  }

  nk.Print();
  fds.FitRegion_Max = 0xdeadbeef;
  if (nk.Has("FitRegion_Max")) {
    fds.FitRegion_Max = nk.GetD("FitRegion_Max");
    QLOG(SAM, "FD Sample [" << FDSamples.size() << "] imposes FitRegion E_nu < "
                            << fds.FitRegion_Max);
  }
  if ((FitRegion_Max == 0xdeadbeef) || FitRegion_Max < fds.FitRegion_Max) {
    FitRegion_Max = fds.FitRegion_Max;
  }

  fds.OscillateToPDG = 0;
  if (nk.Has("OscillateToPDG")) {
    fds.OscillateToPDG = nk.GetD("OscillateToPDG");
  }

  std::vector<nuiskey> FDNDRatioElements = nk.GetListOfChildNodes("FDNDRatio");
  for (size_t fdnd_it = 0; fdnd_it < FDNDRatioElements.size(); ++fdnd_it) {
    nuiskey &fnr = FDNDRatioElements[fdnd_it];
    if (fnr.Has("FromPDG") && fnr.Has("DivergenceFactor")) {
      fds.FDNDRatios[fnr.GetI("FromPDG")] = fnr.GetD("DivergenceFactor");
      QLOG(SAM, "FDND DivergenceFactor for far detector sample index: "
                    << FDSamples.size() << " for PDG: " << fnr.GetI("FromPDG")
                    << " -> " << fds.OscillateToPDG << " = "
                    << fnr.GetD("DivergenceFactor"));
    } else {
      THROW(
          "Far detector sample contained FDNDRatio element, but couldn't find "
          "both FromPDG and Factor attributes.");
    }
  }

  fds.FDNDMassRatio = 1;
  if (NDetectorInfo.first != 0xdeadbeef) {
    if ((!nk.Has("DetectorVolume")) || (!nk.Has("DetectorDensity"))) {
      THROW(
          "Near detector sample has specified volume but FD doesn't. This is "
          "needed to scale the predicted event rate by the mass ratio.");
    }
    fds.DetectorInfo.first = nk.GetD("DetectorVolume");
    fds.DetectorInfo.second = nk.GetD("DetectorDensity");
    double TargetMass_kg = fds.DetectorInfo.first * fds.DetectorInfo.second;

    fds.FDNDMassRatio =
        TargetMass_kg / (NDetectorInfo.first * NDetectorInfo.second);

    QLOG(SAM, "\tFD[" << FDSamples.size() << "] Event rate prediction : ");
    QLOG(SAM, "\t\tTarget volume : " << fds.DetectorInfo.first);
    QLOG(SAM, "\t\tTarget density : " << fds.DetectorInfo.second);
    QLOG(SAM, "\t\tFD/ND mass : " << fds.FDNDMassRatio);
  }

  if (!nk.Has("ObsInput")) {
    THROW("Far detector sample must specify at least ObsInput.");
  }

  std::vector<TH1 *> FDObsInputs =
      PlotUtils::GetTH1sFromRootFile(nk.GetS("ObsInput"));
  if (FDObsInputs.size() < 2) {
    THROW(
        "Far detector sample must contain the observed ERec spectrum and the "
        "FD ETrue/ERec smearing matrix. "
        "ObsInput=\"input.root[FDObs_species,FDSmearing_species]\"");
  }

  fds.FDDataHist = NULL;
  for (size_t hist_it = 0; hist_it < FDObsInputs.size() - 1; ++hist_it) {
    if (!dynamic_cast<TH1D *>(FDObsInputs[hist_it])) {
      ERROR(FTL, "Input spectrum index "
                     << hist_it
                     << " from ObsInput attribute was not a TH1D containing "
                        "a far detector observed ERec spectrum ("
                     << nk.GetS("ObsInput") << ").");
      THROW(
          "Far detector sample must contain the observed ERec spectrum and the "
          "FD ETrue/ERec smearing matrix. "
          "ObsInput=\"input.root[FDObs_species,(FDObs_species2),FDSmearing_"
          "species]\"");
    }
    FDObsInputs[hist_it]->Scale(ScalePOT);
    if (!fds.FDDataHist) {
      fds.FDDataHist = dynamic_cast<TH1D *>(FDObsInputs[hist_it]);
    } else {
      fds.FDDataHist->Add(dynamic_cast<TH1D *>(FDObsInputs[hist_it]));
    }
    QLOG(SAM, "Added " << (FDObsInputs.size() - 1)
                       << " far detector component spectra to form Observed "
                          "spectra for sample index "
                       << FDSamples.size() << ".");
  }

  fds.SpectrumToFDSmearingMatrix_TH2 = dynamic_cast<TH2D *>(FDObsInputs.back());
  if (!fds.SpectrumToFDSmearingMatrix_TH2) {
    ERROR(FTL,
          "last histogram from ObsInput attribute was not a TH2D containing "
          "the far detector ETrue/ERec smearing matrix ("
              << nk.GetS("ObsInput") << ").");
    THROW(
        "Far detector sample must contain the observed ERec spectrum and the "
        "FD ETrue/ERec smearing matrix. "
        "ObsInput=\"input.root[FDObs_species,FDSmearing_species]\"");
  }

  TMatrixD SpectrumToFDSmearingMatrix_l =
      SmearceptanceUtils::GetMatrix(fds.SpectrumToFDSmearingMatrix_TH2);

  fds.SpectrumToFDSmearingMatrix.ResizeTo(SpectrumToFDSmearingMatrix_l);
  fds.SpectrumToFDSmearingMatrix = SpectrumToFDSmearingMatrix_l;

  FDSamples.push_back(fds);
}

void Smear_SVDUnfold_Propagation_Osc::FinaliseFDSamples() {
  std::stringstream ss("");

  for (size_t fds_it = 0; fds_it < FDSamples.size(); ++fds_it) {
    FDSample &fds = FDSamples[fds_it];
    // Set up FD histograms.
    // ==============================

    for (size_t nd_it = 0; nd_it < NDSamples.size(); ++nd_it) {
      NDSample &nds = NDSamples[nd_it];

      TH1D *sampleHist =
          static_cast<TH1D *>(nds.ND_Unfolded_Spectrum_Hist->Clone());
      sampleHist->Reset();
      ss.str("");
      ss << "FD_Propagated_Spectrum_Hist_" << fds_it << "_NDSample_" << nd_it;
      sampleHist->SetName(ss.str().c_str());

      fds.FD_Propagated_Spectrum_Hist_NDSamples.push_back(sampleHist);
    }

    fds.FD_Propagated_Spectrum_Hist = static_cast<TH1D *>(
        fds.FD_Propagated_Spectrum_Hist_NDSamples[0]->Clone());
    fds.FD_Propagated_Spectrum_Hist->Reset();
    ss.str("");
    ss << "FD_Propagated_Spectrum_Hist_" << fds_it;

    fds.FD_Propagated_Spectrum_Hist->SetName(ss.str().c_str());

    for (size_t nd_it = 0; nd_it < NDSamples.size(); ++nd_it) {
      //NDSample &nds = NDSamples[nd_it];

      TH1D *sampleHist =
          static_cast<TH1D *>(fds.FD_Propagated_Spectrum_Hist->Clone());
      sampleHist->Reset();
      ss.str("");
      ss << "NDFD_Corrected_Spectrum_Hist_" << fds_it << "_NDSample_" << nd_it;
      sampleHist->SetName(ss.str().c_str());

      fds.NDFD_Corrected_Spectrum_Hist_NDSamples.push_back(sampleHist);
    }

    fds.NDFD_Corrected_Spectrum_Hist =
        static_cast<TH1D *>(fds.FD_Propagated_Spectrum_Hist->Clone());
    fds.NDFD_Corrected_Spectrum_Hist->Reset();
    ss.str("");
    ss << "NDFD_Corrected_Spectrum_Hist_" << fds_it;

    fds.NDFD_Corrected_Spectrum_Hist->SetName(ss.str().c_str());

    fds.FD_Smeared_Spectrum_Hist =
        new TH1D(*fds.SpectrumToFDSmearingMatrix_TH2->ProjectionX());
    fds.FD_Smeared_Spectrum_Hist->SetDirectory(NULL);
    fds.FD_Smeared_Spectrum_Hist->Reset();
    ss.str("");
    ss << "FD_Smeared_Spectrum_Hist_" << fds_it;

    fds.FD_Smeared_Spectrum_Hist->SetName(ss.str().c_str());

    for (size_t nd_it = 0; nd_it < NDSamples.size(); ++nd_it) {
      NDSample &nds = NDSamples[nd_it];

      if (!fds.FDNDRatios.count(nds.NuPDG)) {
        ERROR(WRN, "Have an ND sample that provides PDG:"
                       << nds.NuPDG
                       << " neutrinos but far detector sample index " << fds_it
                       << " doesn't have an NDFD ratio for this sample. "
                          "Setting to 0 (contribution from sample ignored.)");
        fds.FDNDRatios[nds.NuPDG] = 0;
      }
    }
  }
}

Int_t Smear_SVDUnfold_Propagation_Osc::GetFDSampleNAnalysisBins(size_t fds_it) {
  if (fds_it >= FDSamples.size()) {
    THROW("Requested FD sample index " << fds_it << " but only initialised "
                                       << FDSamples.size());
  }
  FDSample &fds = FDSamples[fds_it];
  Int_t NAnalysisBins = 0;
  for (Int_t bi_it = 1; bi_it < fds.FDDataHist->GetXaxis()->GetNbins() + 1;
       ++bi_it) {
    if ((fds.FitRegion_Min != 0xdeadbeef) &&
        (fds.FDDataHist->GetXaxis()->GetBinUpEdge(bi_it) <=
         fds.FitRegion_Min)) {
      continue;
    }
    if ((fds.FitRegion_Max != 0xdeadbeef) &&
        (fds.FDDataHist->GetXaxis()->GetBinLowEdge(bi_it) >
         fds.FitRegion_Max)) {
      continue;
    }
    NAnalysisBins++;
  }
  return NAnalysisBins;
}

void Smear_SVDUnfold_Propagation_Osc::SetupChi2Hists() {
  fDataHist =
      new TH1D("SmearSVDUnfold", "", NFDAnalysisBins, 0, NFDAnalysisBins);
  fDataHist->SetNameTitle((fSettings.GetName() + "_data").c_str(),
                          (fSettings.GetFullTitles()).c_str());

  Int_t CurrAnalysisBin = 1;
  for (size_t fds_it = 0; fds_it < FDSamples.size(); ++fds_it) {
    FDSample &fds = FDSamples[fds_it];
    for (Int_t bi_it = 1; bi_it < fds.FDDataHist->GetXaxis()->GetNbins() + 1;
         ++bi_it) {
      if ((fds.FitRegion_Min != 0xdeadbeef) &&
          (fds.FDDataHist->GetXaxis()->GetBinUpEdge(bi_it) <=
           fds.FitRegion_Min)) {
        continue;
      }
      if ((fds.FitRegion_Max != 0xdeadbeef) &&
          (fds.FDDataHist->GetXaxis()->GetBinLowEdge(bi_it) >
           fds.FitRegion_Max)) {
        continue;
      }
      fDataHist->SetBinContent(CurrAnalysisBin,
                               fds.FDDataHist->GetBinContent(bi_it));
      if (UseRateErrors) {
        fDataHist->SetBinError(CurrAnalysisBin,
                               sqrt(fds.FDDataHist->GetBinContent(bi_it)));
      } else {
        fDataHist->SetBinError(CurrAnalysisBin,
                               fds.FDDataHist->GetBinError(bi_it));
      }
      CurrAnalysisBin++;
    }
  }

  fMCHist = static_cast<TH1D *>(fDataHist->Clone());
  fMCHist->SetNameTitle((fSettings.GetName() + "_MC").c_str(),
                        (fSettings.GetFullTitles()).c_str());
  fMCHist->Reset();
}

void Smear_SVDUnfold_Propagation_Osc::UpdateChi2Hists() {
  Int_t CurrAnalysisBin = 1;
  for (size_t fds_it = 0; fds_it < FDSamples.size(); ++fds_it) {
    FDSample &fds = FDSamples[fds_it];
    for (Int_t bi_it = 1;
         bi_it < fds.FD_Smeared_Spectrum_Hist->GetXaxis()->GetNbins() + 1;
         ++bi_it) {
      if ((fds.FitRegion_Min != 0xdeadbeef) &&
          (fds.FD_Smeared_Spectrum_Hist->GetXaxis()->GetBinUpEdge(bi_it) <=
           fds.FitRegion_Min)) {
        continue;
      }
      if ((fds.FitRegion_Max != 0xdeadbeef) &&
          (fds.FD_Smeared_Spectrum_Hist->GetXaxis()->GetBinLowEdge(bi_it) >
           fds.FitRegion_Max)) {
        continue;
      }
      fMCHist->SetBinContent(
          CurrAnalysisBin, fds.FD_Smeared_Spectrum_Hist->GetBinContent(bi_it));

      fMCHist->SetBinError(CurrAnalysisBin,
                           fds.FD_Smeared_Spectrum_Hist->GetBinError(bi_it));
      CurrAnalysisBin++;
    }
  }
}

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

  fMCHist = NULL;
  fDataHist = NULL;

  ReadExtraConfig(samplekey);

  AddNDInputs(samplekey);
  std::vector<nuiskey> NDTargets = samplekey.GetListOfChildNodes("NDObs");
  for (size_t nd_it = 0; nd_it < NDTargets.size(); ++nd_it) {
    AddNDInputs(NDTargets[nd_it]);
  }

  std::vector<nuiskey> FDTargets = samplekey.GetListOfChildNodes("FDObs");
  NFDAnalysisBins = 0;
  if (!FDTargets.size()) {  // If no child elements, assume that everything is
                            // contained on the sample element
    AddFDTarget(samplekey);
  } else {
    for (size_t fd_it = 0; fd_it < FDTargets.size(); ++fd_it) {
      AddFDTarget(FDTargets[fd_it]);
      NFDAnalysisBins += GetFDSampleNAnalysisBins(fd_it);
    }
  }

  if ((FitRegion_Min != 0xdeadbeef) || (FitRegion_Max != 0xdeadbeef)) {
    QLOG(SAM, "When unfolding, interested region limited to:");
    if (FitRegion_Min != 0xdeadbeef) {
      QLOG(SAM, "\tE_nu > " << FitRegion_Min);
    }
    if (FitRegion_Max != 0xdeadbeef) {
      QLOG(SAM, "\tE_nu < " << FitRegion_Max);
    }
  }

  SetupNDInputs();

  FinaliseFDSamples();

  QLOG(SAM, "Set up " << FDSamples.size()
                      << " samples for oscillation analysis with "
                      << NFDAnalysisBins << " analysis bins.");

  SetupChi2Hists();

  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

void Smear_SVDUnfold_Propagation_Osc::FillEventVariables(FitEvent *event){};

bool Smear_SVDUnfold_Propagation_Osc::isSignal(FitEvent *event) {
  return false;
}

void DumpUnfoldDebugInfo(Smear_SVDUnfold_Propagation_Osc::NDSample &nds,
                         size_t nd_it, size_t truncations) {
  TDirectory *ogDir = gDirectory;
  std::stringstream ss;
  ss.str("");
  ss << "DEBUG_FailedInvert_NDSample_" << nd_it;

  Config::Get().out->mkdir(ss.str().c_str());
  Config::Get().out->cd(ss.str().c_str());

  ss.str("");
  ss << "ND_Smearing_Matrix_" << nd_it;
  nds.NDToSpectrumSmearingMatrix->Write(ss.str().c_str(), TObject::kOverwrite);
  ss.str("");
  ss << "ND_Inverse_Smearing_Matrix_" << nd_it;
  SmearceptanceUtils::SVDGetInverse(nds.NDToSpectrumSmearingMatrix, 0)
      ->Write(ss.str().c_str(), TObject::kOverwrite);

  ss.str("");
  ss << "ND_Inverse_Smearing_Matrix_" << nd_it << "_Trunc_" << truncations;
  SmearceptanceUtils::SVDGetInverse(nds.NDToSpectrumSmearingMatrix, truncations)
      ->Write(ss.str().c_str(), TObject::kOverwrite);

  ss.str("");
  ss << "ND_Obs_" << nd_it;
  nds.NDDataHist->Write(ss.str().c_str(), TObject::kOverwrite);

  TMatrixD NDToSpectrumResponseMatrix_notrunc = SmearceptanceUtils::GetMatrix(
      SmearceptanceUtils::SVDGetInverse(nds.NDToSpectrumSmearingMatrix, 0));

  nds.NDToSpectrumResponseMatrix.ResizeTo(NDToSpectrumResponseMatrix_notrunc);
  nds.NDToSpectrumResponseMatrix = NDToSpectrumResponseMatrix_notrunc;

  SmearceptanceUtils::PushTH1ThroughMatrixWithErrors(
      nds.NDDataHist, nds.ND_Unfolded_Spectrum_Hist,
      nds.NDToSpectrumResponseMatrix, 1000, false);

  ss.str("");
  ss << "ND_Unfolded_" << nd_it;
  nds.ND_Unfolded_Spectrum_Hist->Write(ss.str().c_str(), TObject::kOverwrite);

  TMatrixD NDToSpectrumResponseMatrix_trunc =
      SmearceptanceUtils::GetMatrix(SmearceptanceUtils::SVDGetInverse(
          nds.NDToSpectrumSmearingMatrix, truncations));

  nds.NDToSpectrumResponseMatrix.ResizeTo(NDToSpectrumResponseMatrix_trunc);
  nds.NDToSpectrumResponseMatrix = NDToSpectrumResponseMatrix_trunc;

  SmearceptanceUtils::PushTH1ThroughMatrixWithErrors(
      nds.NDDataHist, nds.ND_Unfolded_Spectrum_Hist,
      nds.NDToSpectrumResponseMatrix, 1000, false);

  ss.str("");
  ss << "ND_Unfolded_" << nd_it << "_Trunc_" << truncations;
  nds.ND_Unfolded_Spectrum_Hist->Write(ss.str().c_str(), TObject::kOverwrite);

  if (ogDir) {
    ogDir->cd();
  } else {
    Config::Get().out->cd();
  }
}

void Smear_SVDUnfold_Propagation_Osc::UnfoldToNDETrueSpectrum(size_t nd_it) {
  if (nd_it >= NDSamples.size()) {
    THROW("Attempting to unfold ND sample index "
          << nd_it << " but only have " << NDSamples.size() << " ND samples.");
  }

  NDSample &nds = NDSamples[nd_it];

  nds.ND_Unfolded_Spectrum_Hist =
      new TH1D(*nds.NDToSpectrumSmearingMatrix->ProjectionY());
  nds.ND_Unfolded_Spectrum_Hist->SetDirectory(NULL);
  nds.ND_Unfolded_Spectrum_Hist->Clear();
  std::stringstream ss("");
  ss << "ND_Unfolded_Spectrum_Hist_" << nd_it;
  nds.ND_Unfolded_Spectrum_Hist->SetName(ss.str().c_str());

  bool HasNegValue = false;
  int truncations = nds.TruncateStart;
  do {
    if (truncations >= nds.TruncateUpTo) {
      DumpUnfoldDebugInfo(nds, nd_it, truncations);

      THROW("Unfolded enu spectrum had negative values even after "
            << truncations << " SVD singular value truncations.");
    }

    // Unfold ND ERec -> Enu spectrum
    // ------------------------------
    SmearceptanceUtils::PushTH1ThroughMatrixWithErrors(
        nds.NDDataHist, nds.ND_Unfolded_Spectrum_Hist,
        nds.NDToSpectrumResponseMatrix, 1000, false);

    HasNegValue = false;

    for (Int_t bi_it = 1;
         bi_it < nds.ND_Unfolded_Spectrum_Hist->GetXaxis()->GetNbins() + 1;
         ++bi_it) {
      if ((FitRegion_Min != 0xdeadbeef) &&
          (nds.ND_Unfolded_Spectrum_Hist->GetXaxis()->GetBinUpEdge(bi_it) <=
           FitRegion_Min)) {
        continue;
      }
      if ((FitRegion_Max != 0xdeadbeef) &&
          (nds.ND_Unfolded_Spectrum_Hist->GetXaxis()->GetBinLowEdge(bi_it) >
           FitRegion_Max)) {
        continue;
      }

      if (nds.ND_Unfolded_Spectrum_Hist->GetBinContent(bi_it) < 0) {
        HasNegValue = true;
        QLOG(SAM,
             "After "
                 << truncations << " truncations, bin " << (bi_it - 1) << " ["
                 << nds.ND_Unfolded_Spectrum_Hist->GetXaxis()->GetBinLowEdge(
                        bi_it)
                 << " -- "
                 << nds.ND_Unfolded_Spectrum_Hist->GetXaxis()->GetBinUpEdge(
                        bi_it)
                 << " ] has value: "
                 << nds.ND_Unfolded_Spectrum_Hist->GetBinContent(bi_it));
        break;
      }
    }

    if (HasNegValue) {
      TMatrixD NDToSpectrumResponseMatrix_l =
          SmearceptanceUtils::GetMatrix(SmearceptanceUtils::SVDGetInverse(
              nds.NDToSpectrumSmearingMatrix, truncations));

      nds.NDToSpectrumResponseMatrix.ResizeTo(NDToSpectrumResponseMatrix_l);
      nds.NDToSpectrumResponseMatrix = NDToSpectrumResponseMatrix_l;
    }

    truncations++;
  } while (HasNegValue);
}

void Smear_SVDUnfold_Propagation_Osc::PropagateFDSample(size_t fds_it) {
  if (fds_it >= FDSamples.size()) {
    THROW("Requested FD sample index " << fds_it << " but only initialised "
                                       << FDSamples.size());
  }
  FDSample &fds = FDSamples[fds_it];

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

  for (Int_t bi_it = 1;
       bi_it < fds.NDFD_Corrected_Spectrum_Hist->GetXaxis()->GetNbins() + 1;
       ++bi_it) {
    double content_osc = 0;
    double error_osc = 0;
    double content_fdnd = 0;
    double error_fdnd = 0;
    // Oscillate each ND sample to FD neutrino
    for (size_t nd_it = 0; nd_it < NDSamples.size(); ++nd_it) {
      NDSample &nds = NDSamples[nd_it];

      double oscWeight = oscWE->CalcWeight(
          nds.ND_Unfolded_Spectrum_Hist->GetXaxis()->GetBinCenter(bi_it),
          nds.NuPDG, fds.OscillateToPDG);

      double sample_content_osc =
          nds.ND_Unfolded_Spectrum_Hist->GetBinContent(bi_it) * oscWeight;
      double sample_error_osc =
          nds.ND_Unfolded_Spectrum_Hist->GetBinError(bi_it) * oscWeight;

      fds.FD_Propagated_Spectrum_Hist_NDSamples[nd_it]->SetBinContent(
          bi_it, sample_content_osc);
      fds.FD_Propagated_Spectrum_Hist_NDSamples[nd_it]->SetBinError(
          bi_it, sample_error_osc);

      double sample_content_fdnd =
          sample_content_osc * fds.FDNDRatios[nds.NuPDG] * fds.FDNDMassRatio;
      double sample_error_fdnd =
          sample_error_osc * fds.FDNDRatios[nds.NuPDG] * fds.FDNDMassRatio;

      fds.NDFD_Corrected_Spectrum_Hist_NDSamples[nd_it]->SetBinContent(
          bi_it, sample_content_fdnd);
      fds.NDFD_Corrected_Spectrum_Hist_NDSamples[nd_it]->SetBinError(
          bi_it, sample_error_fdnd);

      content_osc += sample_content_osc;
      error_osc += sample_error_osc * sample_error_osc;

      content_fdnd += sample_content_fdnd;
      error_fdnd += sample_error_fdnd * sample_error_fdnd;
    }

    fds.FD_Propagated_Spectrum_Hist->SetBinContent(bi_it, content_osc);
    fds.FD_Propagated_Spectrum_Hist->SetBinError(bi_it, sqrt(error_osc));

    fds.NDFD_Corrected_Spectrum_Hist->SetBinContent(bi_it, content_fdnd);
    fds.NDFD_Corrected_Spectrum_Hist->SetBinError(bi_it, sqrt(error_fdnd));
  }

  // Forward fold Spectrum -> ERec FD
  // ------------------------------
  SmearceptanceUtils::PushTH1ThroughMatrixWithErrors(
      fds.NDFD_Corrected_Spectrum_Hist, fds.FD_Smeared_Spectrum_Hist,
      fds.SpectrumToFDSmearingMatrix, 1000, true);
}

void Smear_SVDUnfold_Propagation_Osc::ConvertEventRates(void) {

  /// Get topology weights
  /// for each ND sample
  /// /// Build NDSample::NDToSpectrumSmearingMatrix from sum of topology smearing histos scaled by topology weights
  ///
  /// SetupNDInputs();
  ///
  /// for each FD sample
  /// /// Build FDSample::SpectrumToFDSmearingMatrix_TH2 as above.
  /// /// Convert to TMatrixD, FDSample::SpectrumToFDSmearingMatrix

  for (size_t fds_it = 0; fds_it < FDSamples.size(); ++fds_it) {
    PropagateFDSample(fds_it);
  }
  UpdateChi2Hists();
}

void Smear_SVDUnfold_Propagation_Osc::Write(std::string drawOpt) {
  TDirectory *ogDir = gDirectory;

  ConvertEventRates();

  FitWeight *fw = FitBase::GetRW();
  OscWeightEngine *oscWE =
      dynamic_cast<OscWeightEngine *>(fw->GetRWEngine(kOSCILLATION));

  if (!oscWE) {
    THROW(
        "Couldn't load oscillation weight engine for sample: "
        "Smear_SVDUnfold_Propagation_Osc.");
  }
  oscWE->Print();

  // Write ND samples
  //----------------
  for (size_t nd_it = 0; nd_it < NDSamples.size(); ++nd_it) {
    NDSample &nds = NDSamples[nd_it];
    std::stringstream ss("");
    ss << "Smear_SVDUnfold_Propagation_Osc_NDSample_" << nd_it;
    if (ogDir) {
      ogDir->mkdir(ss.str().c_str());
      ogDir->cd(ss.str().c_str());
    } else {
      Config::Get().out->mkdir(ss.str().c_str());
      Config::Get().out->cd(ss.str().c_str());
    }

    nds.NDToSpectrumSmearingMatrix->Write("SmearingMatrix_ND",
                                          TObject::kOverwrite);
    nds.NDDataHist->Write("Obs_ND", TObject::kOverwrite);

    nds.ND_Unfolded_Spectrum_Hist->Write(
        nds.ND_Unfolded_Spectrum_Hist->GetName(), TObject::kOverwrite);

    if (ogDir) {
      ogDir->cd();
    } else {
      Config::Get().out->cd();
    }
  }

  // Write FD samples
  //----------------
  for (size_t fds_it = 0; fds_it < FDSamples.size(); ++fds_it) {
    FDSample &fds = FDSamples[fds_it];

    std::stringstream ss("");
    ss << "Smear_SVDUnfold_Propagation_Osc_FDSample_" << fds_it;
    if (ogDir) {
      ogDir->mkdir(ss.str().c_str());
      ogDir->cd(ss.str().c_str());
    } else {
      Config::Get().out->mkdir(ss.str().c_str());
      Config::Get().out->cd(ss.str().c_str());
    }

    // Calc oscillation probability
    // ------------------------------
    FitWeight *fw = FitBase::GetRW();
    OscWeightEngine *oscWE =
        dynamic_cast<OscWeightEngine *>(fw->GetRWEngine(kOSCILLATION));

    if (!oscWE) {
      THROW(
          "Couldn't load oscillation weight engine for sample: "
          "Smear_SVDUnfold_Propagation_Osc.");
    }

    for (size_t nd_it = 0; nd_it < NDSamples.size(); ++nd_it) {
      NDSample &nds = NDSamples[nd_it];
      ss.str("");
      ss << "NDSample_" << nd_it << "_contribution";
      fds.FD_Propagated_Spectrum_Hist_NDSamples[nd_it]->Write(
          ss.str().c_str(), TObject::kOverwrite);
      ss.str("");

      ss << "NDSample_" << nd_it << "_FDNDCorrected_contribution";
      fds.NDFD_Corrected_Spectrum_Hist_NDSamples[nd_it]->Write(
          ss.str().c_str(), TObject::kOverwrite);

      ss.str("");
      ss << "NDSample_" << nd_it << "_OscillationProb";

      TGraph POsc;

      POsc.Set(1E4 - 1);

      double min = nds.ND_Unfolded_Spectrum_Hist->GetXaxis()->GetBinLowEdge(1);
      double step =
          (nds.ND_Unfolded_Spectrum_Hist->GetXaxis()->GetBinUpEdge(
               nds.ND_Unfolded_Spectrum_Hist->GetXaxis()->GetNbins()) -
           nds.ND_Unfolded_Spectrum_Hist->GetXaxis()->GetBinLowEdge(1)) /
          double(1E4);

      for (size_t i = 1; i < 1E4; ++i) {
        double enu = min + i * step;
        double ow = oscWE->CalcWeight(enu, nds.NuPDG, fds.OscillateToPDG);
        if (ow != ow) {
          std::cout << "Bad osc weight for ENu: " << enu << std::endl;
        }
        POsc.SetPoint(i - 1, enu, ow);
      }

      POsc.Write(ss.str().c_str(), TObject::kOverwrite);
    }

    fds.FDDataHist->Write("Obs_FD", TObject::kOverwrite);

    fds.FD_Propagated_Spectrum_Hist->Write(
        fds.FD_Propagated_Spectrum_Hist->GetName(), TObject::kOverwrite);

    fds.NDFD_Corrected_Spectrum_Hist->Write(
        fds.NDFD_Corrected_Spectrum_Hist->GetName(), TObject::kOverwrite);

    fds.SpectrumToFDSmearingMatrix_TH2->Write("SmearingMatrix_FD",
                                              TObject::kOverwrite);

    fds.FD_Smeared_Spectrum_Hist->Write(fds.FD_Smeared_Spectrum_Hist->GetName(),
                                        TObject::kOverwrite);

    if (ogDir) {
      ogDir->cd();
    } else {
      Config::Get().out->cd();
    }
  }

  fMCHist->Write("Pred_FD", TObject::kOverwrite);
  fDataHist->Write("Obs_FD", TObject::kOverwrite);

  Measurement1D::Write(drawOpt);
}
