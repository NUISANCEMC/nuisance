// Copyright 2018 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#pragma once

#include "samples/IDataComparison.hxx"

#include "event/FullEvent.hxx"

#include "input/InputManager.hxx"

#include "persistency/ROOTOutput.hxx"

#include "utility/FileSystemUtility.hxx"
#include "utility/HistogramUtility.hxx"
#include "utility/KinematicUtility.hxx"
#include "utility/ROOTUtility.hxx"
#include "utility/StatsUtility.hxx"

#include <array>
#include <functional>
#include <limits>
#include <memory>
#include <vector>

template <size_t nd, typename NumericT = double,
          typename HT = typename nuis::utility::HType_Helper<nd, void>::type>
class SimpleDataComparison : public IDataComparison {

  NEW_NUIS_EXCEPT(invalid_SimpleDataComparison_initialization);
  NEW_NUIS_EXCEPT(SimpleDataComparison_already_finalized);

public:
  static size_t const NDim = nd;
  std::string fWrite_directory;

protected:
  using HistType = HT;
  using TH_Help = typename nuis::utility::TH_Helper<HistType>;
  nuis::input::InputManager::Input_id_t fIH_id;

  std::string fName;

  size_t NMaxSample_override;
  int fIsShapeOnly;
  int fIsFluxUnfolded;

  std::vector<bool> fSignalCache;
  std::vector<std::array<NumericT, NDim>> fProjectionCache;
  bool fUseCache;
  bool fModeHists;

  std::string fDataInputDescriptor;
  std::unique_ptr<HistType> fData;
  std::string fMaskInputDescriptor;
  std::unique_ptr<HistType> fMask;
  std::string fCovarianceInputDescriptor;
  std::unique_ptr<TH2> fCovariance;
  std::unique_ptr<HistType> fPrediction;
  std::map<nuis::event::Channel_t, std::unique_ptr<HistType>> fPrediction_modes;
  std::unique_ptr<HistType> fPrediction_xsec;
  std::unique_ptr<HistType> fPrediction_shape;
  std::unique_ptr<HistType> fPrediction_comparison;
  bool fComparisonFinalized;

  std::string fJournalReference;
  std::string fDOI;
  std::string fYear;
  std::string fTargetMaterial;
  std::string fFluxDescription;
  std::string fSignalDescription;

  nuis::utility::KinematicRange energy_cut;

  std::function<bool(nuis::event::FullEvent const &)> IsSigFunc;

  // If assigned by subclass will be called on for all events, bool signifies
  // whether the event was selected.
  std::function<void(nuis::event::FullEvent const &, bool, double)>
      ProcessExtraFunc;

public:
  std::function<std::array<NumericT, NDim>(nuis::event::FullEvent const &)>
      CompProjFunc;

  SimpleDataComparison(std::string name) {
    fName = std::move(name);
    fIH_id = std::numeric_limits<nuis::input::InputManager::Input_id_t>::max();
    fUseCache = false;
    fWrite_directory = "";
    NMaxSample_override = std::numeric_limits<size_t>::max();
    fDataInputDescriptor = "";
    fData = nullptr;
    fMaskInputDescriptor = "";
    fMask = nullptr;
    fCovarianceInputDescriptor = "";
    fCovariance = nullptr;
    fPrediction = nullptr;
    fPrediction_xsec = nullptr;
    fPrediction_shape = nullptr;
    fPrediction_comparison = nullptr;
    fComparisonFinalized = false;
    IsSigFunc = [](nuis::event::FullEvent const &) -> bool { return true; };
    CompProjFunc =
        [](nuis::event::FullEvent const &) -> std::array<NumericT, NDim> {
      std::array<NumericT, NDim> arr;
      for (NumericT &el : arr) {
        el = 0;
      }
      return arr;
    };

    ProcessExtraFunc =
        std::function<void(nuis::event::FullEvent const &, bool, double)>();

    fJournalReference = "";
    fDOI = "";
    fYear = "";
    fTargetMaterial = "";
    fFluxDescription = "";
    fSignalDescription = "";
    fIsShapeOnly = -1;
    fIsFluxUnfolded = -1;

    energy_cut = nuis::utility::KinematicRange{
        std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
  }

  std::string Name() { return fName; }

  void SetUseCache(bool uc = true) {
    fUseCache = uc;
    if (!uc) {
      fSignalCache.resize(0);
      fProjectionCache.resize(0);
    }
  }

  fhicl::ParameterSet fGlobalConfig;
  fhicl::ParameterSet fInstanceConfig;

  void ReadGlobalConfigDefaults() {
    fGlobalConfig = nuis::config::GetDocument().get<fhicl::ParameterSet>(
        std::string("global.sample_configuration.") + Name(),
        fhicl::ParameterSet());

    if (!fJournalReference.length()) {
      fJournalReference = fGlobalConfig.get<std::string>("journal_reference",
                                                         "Not yet published");
    }
    if (!fDOI.length()) {
      fDOI = fGlobalConfig.get<std::string>("DOI", "Unknown DOI");
    }
    if (!fYear.length()) {
      fYear = fGlobalConfig.get<std::string>("year", "Unknown year");
    }

    if (!fTargetMaterial.length()) {
      fTargetMaterial = fGlobalConfig.get<std::string>(
          "target_material", "Unknown target material");
    }
    if (!fFluxDescription.length()) {
      fFluxDescription =
          fGlobalConfig.get<std::string>("flux_description", "Unknown flux");
    }
    if (!fSignalDescription.length()) {
      fSignalDescription = fGlobalConfig.get<std::string>("signal_description",
                                                          "Unknown Signal");
    }

    if (fIsShapeOnly == -1) {
      fIsShapeOnly = fGlobalConfig.get<bool>("shape_only", false);
    }
    if (fIsFluxUnfolded == -1) {
      fIsFluxUnfolded = fGlobalConfig.get<bool>("flux_unfolded", false);
    }

    if ((energy_cut.first == std::numeric_limits<double>::max()) &&
        (fGlobalConfig.has_key("enu_range"))) {
      energy_cut = fGlobalConfig.get<std::pair<double, double>>("enu_range");
    }
  }

  virtual std::string GetJournalReference() { return fJournalReference; }
  virtual std::string GetDOI() { return fDOI; }
  virtual std::string GetYear() { return fYear; }
  virtual std::string GetTargetMaterial() { return fTargetMaterial; }
  virtual std::string GetFluxDescription() { return fFluxDescription; }
  virtual std::string GetSignalDescription() { return fSignalDescription; }

  void SetShapeOnly(bool iso) { fIsShapeOnly = iso; }
  void SetFluxUnfolded(bool ifo) { fIsFluxUnfolded = ifo; }

  void SetData(std::string const &data_descriptor) {
    fDataInputDescriptor = data_descriptor;
  }

  void SetMask(std::string const &mask_descriptor) {
    fMaskInputDescriptor = mask_descriptor;
  }

  void SetCovariance(std::string const &cov_descriptor) {
    fCovarianceInputDescriptor = cov_descriptor;
  }

  virtual void FillProjection(std::array<NumericT, NDim> const &proj,
                              NumericT event_weight) {
    TH_Help::Fill(fPrediction, proj, event_weight);
  }
  virtual void FillModeProjection(std::array<NumericT, NDim> const &proj,
                                  NumericT event_weight,
                                  nuis::event::Channel_t mode) {
    if (!fPrediction_modes.count(mode)) {
      std::stringstream ss;
      ss << "Prediction_" << mode;
      fPrediction_modes[mode] =
          nuis::utility::Clone(fPrediction, true, ss.str());
    }
    TH_Help::Fill(fPrediction_modes[mode], proj, event_weight);
  }

  virtual void ProcessSignalEvent(nuis::event::FullEvent const &fev,
                                  double weight = 1) {
    auto const &proj = CompProjFunc(fev);
    FillProjection(proj, weight);
    if (fModeHists) {
      FillModeProjection(proj, weight, fev.mode);
    }
  }

  virtual void FinalizeComparison() {
    if (fComparisonFinalized) {
      throw SimpleDataComparison_already_finalized()
          << "[ERROR]: Attempted to re-finalize a comparison for "
             "SimpleDataComparison: "
          << std::quoted(Name());
    }
    fPrediction_xsec =
        nuis::utility::Clone(fPrediction, false, "Prediction_xsec");

    TH_Help::Scale(fPrediction_xsec, 1.0, "width");
    if (fModeHists) {
      for (auto &mh : fPrediction_modes) {
        TH_Help::Scale(mh.second, 1.0, "width");
      }
    }
    // If we have a flux cut
    if (energy_cut.first != std::numeric_limits<double>::max()) {
      IInputHandler const &IH =
          nuis::input::InputManager::Get().GetInputHandler(fIH_id);
      TH_Help::Scale(fPrediction_xsec, IH.GetXSecScaleFactor(energy_cut));
    }

    fPrediction_shape =
        nuis::utility::Clone(fPrediction_xsec, false, "Prediction_shape");
    if (fData) {
      TH_Help::Scale(fPrediction_shape,
                     TH_Help::Integral(fData, "width") /
                         TH_Help::Integral(fPrediction_shape, "width"));

    } else {
      IEventProcessor_WARN(
          "When Finalizing comparison, no Data histogram available.");
    }

    if (fIsFluxUnfolded) {
      // fPrediction_comparison
    } else if (fIsShapeOnly) {
      fPrediction_comparison = nuis::utility::Clone(fPrediction_shape, false,
                                                    "Prediction_comparison");
    } else {
      fPrediction_comparison = nuis::utility::Clone(fPrediction_xsec, false,
                                                    "Prediction_comparison");
    }
    fComparisonFinalized = true;
  }

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    fInstanceConfig = instance_sample_configuration;

    SetSampleVerbosity(fInstanceConfig.get<std::string>(
        "verbosity", nuis::config::GetDocument().get<std::string>(
                         "global.sample.verbosity_default", "Reticent")));

    ReadGlobalConfigDefaults();

    if (!fData) { // If data hasn't been set externally.
      if (fInstanceConfig.has_key("fake_data")) {
        fData = nuis::utility::GetHistogram<HistType>(
            fInstanceConfig.get<std::string>("fake_data_histogram"));
      } else if (!fGlobalConfig.get<bool>("has_data", true) ||
                 !fInstanceConfig.get<bool>("has_data", true)) {
        // Explicitly not expecting data
      } else {
        if (!fDataInputDescriptor.length()) {
          if (!fGlobalConfig.has_key("data_descriptor")) {
            throw invalid_SimpleDataComparison_initialization()
                << "[ERROR]: SimpleDataComparison::Initialize for "
                   "IDataComparison: "
                << std::quoted(Name())
                << " failed as no input data was set by a call to "
                   "SimpleDataComparison::SetData and no data_descriptor for "
                   "this SimpleDataComparison could be found in the global "
                   "configuration.";
          }
          fDataInputDescriptor =
              fGlobalConfig.get<std::string>("data_descriptor");
        }
        fData = nuis::utility::GetHistogram<HistType>(fDataInputDescriptor);
      }
    }

    if (!fPrediction) {
      if (!fData) {
        throw invalid_SimpleDataComparison_initialization()
            << "[ERROR]: SimpleDataComparison::Initialize for "
               "IDataComparison: "
            << std::quoted(Name())
            << " failed. As `has_data: false` was set in the configuration  "
               "(global: "
            << (!fGlobalConfig.get<bool>("has_data", true))
            << ", instance: " << !fInstanceConfig.get<bool>("has_data", true)
            << "), the instance constructor must supply the fPrediction "
               "binning, and it wasn't.";
      }
      fPrediction = nuis::utility::Clone(fData, true, "Prediction");
    }

    if (fCovarianceInputDescriptor.length()) {
      fCovariance =
          nuis::utility::GetHistogram<TH2D>(fCovarianceInputDescriptor);
    } else if (fGlobalConfig.has_key("covariance_descriptor")) {
      fCovariance = nuis::utility::GetHistogram<TH2D>(
          fGlobalConfig.get<std::string>("covariance_descriptor"));
    }

    if (fMaskInputDescriptor.length()) {
      fMask = nuis::utility::GetHistogram<HistType>(fMaskInputDescriptor);
    } else if (fGlobalConfig.has_key("mask_descriptor")) {
      fMask = nuis::utility::GetHistogram<HistType>(
          fGlobalConfig.get<std::string>("mask_descriptor"));
    }

    if (fInstanceConfig.has_key("verbosity")) {
      SetSampleVerbosity(fInstanceConfig.get<std::string>("verbosity"));
    }

    fModeHists = fInstanceConfig.get<bool>("write_mode_hists", false);

    NMaxSample_override =
        fInstanceConfig.get<size_t>("nmax", std::numeric_limits<size_t>::max());

    if (!fWrite_directory.size()) { // Can be set by subclass
      fWrite_directory =
          fInstanceConfig.get<std::string>("write_directory", Name());
    }
  }

  void ProcessSample(size_t nmax = std::numeric_limits<size_t>::max()) {
    if (fIH_id ==
        std::numeric_limits<nuis::input::InputManager::Input_id_t>::max()) {
      fIH_id =
          nuis::input::InputManager::Get().EnsureInputLoaded(fInstanceConfig);
    }
    IInputHandler const &IH =
        nuis::input::InputManager::Get().GetInputHandler(fIH_id);

    nmax = std::min(NMaxSample_override, nmax);

    size_t NEvsToProcess = std::min(nmax, IH.GetNEvents());

    double nmax_scaling = double(IH.GetNEvents()) / double(NEvsToProcess);

    size_t NToShout = NEvsToProcess / 10;
    IEventProcessor_INFO("Sample " << std::quoted(Name()) << " processing "
                                   << NEvsToProcess << " events.");

    IInputHandler::ev_index_t ev_idx = 0;

    bool DetermineSignalEvents = !fSignalCache.size() || !fUseCache;

    nuis::utility::Clear<HistType>(*fPrediction);
    fComparisonFinalized = false;

    size_t cache_ctr = 0;

    while (ev_idx < NEvsToProcess) {
      bool is_sig = false;
      std::array<NumericT, NDim> proj;

      if (DetermineSignalEvents) {
        nuis::event::FullEvent const &fev = IH.GetFullEvent(ev_idx);
        is_sig = IsSigFunc(fev);
        fSignalCache.push_back(is_sig);
        if (is_sig) {
          proj = CompProjFunc(fev);
          fProjectionCache.push_back(proj);
        }
        if (ProcessExtraFunc) {
          ProcessExtraFunc(fev, is_sig,
                           IH.GetEventWeight(ev_idx) * nmax_scaling);
        }
        if (fModeHists && is_sig) {
          FillModeProjection(proj, IH.GetEventWeight(ev_idx) * nmax_scaling,
                             fev.mode);
        }
      } else {
        is_sig = fSignalCache[ev_idx];
        proj = fProjectionCache[cache_ctr++];
      }

      if (NToShout && !(ev_idx % NToShout)) {
        IEventProcessor_INFO("\tProcessed " << ev_idx << "/" << NEvsToProcess
                                            << " events.");
      }

      if (is_sig) {
        FillProjection(proj, IH.GetEventWeight(ev_idx) * nmax_scaling);
      }

      ev_idx++;
    }
    IEventProcessor_INFO("\t" << fProjectionCache.size() << "/" << NEvsToProcess
                              << " events passed selection.");
  }
  void Write() {
    if (!fComparisonFinalized) {
      FinalizeComparison();
    }

    nuis::persistency::WriteToOutputFile<HistType>(
        fPrediction_comparison, "Prediction", fWrite_directory);
    if (fModeHists) {
      for (auto &mh : fPrediction_modes) {
        nuis::persistency::WriteToOutputFile<HistType>(
            mh.second, mh.second->GetName(), fWrite_directory);
      }
    }
    nuis::persistency::WriteToOutputFile<HistType>(
        fPrediction_xsec, "Prediction_xsec", fWrite_directory);

    if (fData) {
      nuis::persistency::WriteToOutputFile<HistType>(fData, "Data",
                                                     fWrite_directory);
      nuis::persistency::WriteToOutputFile<HistType>(
          fPrediction_shape, "Prediction_shape", fWrite_directory);
    }
  }

  double GetGOF() {
    if (!fComparisonFinalized) {
      FinalizeComparison();
    }
    if (fData && fPrediction_comparison) {
      return nuis::utility::GetChi2(fData, fPrediction_comparison);
    } else
      return std::numeric_limits<double>::max();
  }
  double GetNDOGuess() {
    if (fData) {
      return TH_Help::Nbins(fData);
    }
    return 0;
  }

  fhicl::ParameterSet GetExampleConfiguration() {
    fhicl::ParameterSet exps;
    exps.put<std::string>("name", Name());
    exps.put<std::string>("input_type", "NuWro/NEUT/GENIE");
    exps.put<std::string>("file", "input_events.root");
    exps.put<std::string>("write_directory", Name());
    return exps;
  }
};

typedef SimpleDataComparison<1, double, TH1D> SimpleDataComparison_1D;
typedef SimpleDataComparison<2, double, TH2D> SimpleDataComparison_2D;
typedef SimpleDataComparison<1, float, TH1F> SimpleDataComparison_1F;
typedef SimpleDataComparison<2, float, TH2F> SimpleDataComparison_2F;
typedef SimpleDataComparison<2, double, TH2Poly> SimpleDataComparison_2DPoly;
