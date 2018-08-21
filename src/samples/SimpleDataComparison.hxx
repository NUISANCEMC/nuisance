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

#ifndef SAMPLES_SIMPLEDATACOMPARISON_HXX_SEEN
#define SAMPLES_SIMPLEDATACOMPARISON_HXX_SEEN

#include "samples/IDataComparison.hxx"

#include "event/FullEvent.hxx"

#include "input/InputManager.hxx"

#include "persistency/ROOTOutput.hxx"

#include "utility/FileSystemUtility.hxx"
#include "utility/HistogramUtility.hxx"
#include "utility/ROOTUtility.hxx"

#include <array>
#include <functional>
#include <limits>
#include <memory>
#include <vector>

template <size_t> struct TH_dim_helper {};
template <> struct TH_dim_helper<1> { typedef TH1D type; };
template <> struct TH_dim_helper<2> { typedef TH2D type; };

template <size_t NDim> class SimpleDataComparison : public IDataComparison {

  NEW_NUIS_EXCEPT(invalid_SimpleDataComparison_initialization);

protected:
  nuis::input::InputManager::Input_id_t fIH_id;
  std::string write_directory;
  size_t NMaxSample_override;
  int fIsShapeOnly;
  int fIsFluxUnfolded;

  std::vector<bool> fSignalCache;
  std::vector<std::array<double, NDim>> fProjectionCache;

  std::string fDataInputDescriptor;
  std::unique_ptr<typename TH_dim_helper<NDim>::type> fData;
  std::string fCovarianceInputDescriptor;
  std::unique_ptr<TH2D> fCovariance;
  std::unique_ptr<typename TH_dim_helper<NDim>::type> fPrediction;
  std::unique_ptr<typename TH_dim_helper<NDim>::type> fPrediction_xsec;
  std::unique_ptr<typename TH_dim_helper<NDim>::type> fPrediction_shape;
  std::unique_ptr<typename TH_dim_helper<NDim>::type> fPrediction_comparison;
  bool fComparisonFinalized;

  std::string fJournalReference;
  std::string fTargetMaterial;
  std::string fFluxDescription;
  std::string fSignalDescription;

  std::pair<double, double> EnuRange;

  std::function<bool(nuis::event::FullEvent const &)> IsSigFunc;
  std::function<std::array<double, NDim>(nuis::event::FullEvent const &)>
      CompProjFunc;

public:
  SimpleDataComparison() {
    fIH_id = std::numeric_limits<nuis::input::InputManager::Input_id_t>::max();
    write_directory = "";
    NMaxSample_override = std::numeric_limits<size_t>::max();
    fDataInputDescriptor = "";
    fData = nullptr;
    fCovarianceInputDescriptor = "";
    fCovariance = nullptr;
    fPrediction = nullptr;
    fPrediction_xsec = nullptr;
    fPrediction_shape = nullptr;
    fPrediction_comparison = nullptr;
    fComparisonFinalized = false;
    IsSigFunc = [](nuis::event::FullEvent const &) -> bool { return true; };
    CompProjFunc =
        [](nuis::event::FullEvent const &) -> std::array<double, NDim> {
      std::array<double, NDim> arr;
      for (double &el : arr) {
        el = 0;
      }
      return arr;
    };

    fJournalReference = "";
    fTargetMaterial = "";
    fFluxDescription = "";
    fSignalDescription = "";
    fIsShapeOnly = -1;
    fIsFluxUnfolded = -1;
    EnuRange = std::pair<double, double>{std::numeric_limits<double>::max(),
                                         std::numeric_limits<double>::max()};
  }

  void ReadGlobalConfigDefaults() {
    fhicl::ParameterSet const &global_sample_configuration =
        nuis::config::GetDocument().get<fhicl::ParameterSet>(
            std::string("global.sample_configuration.") + Name(),
            fhicl::ParameterSet());

    if (!fJournalReference.length()) {
      fJournalReference = global_sample_configuration.get<std::string>(
          "journal_reference", fJournalReference);
    }
    if (!fTargetMaterial.length()) {
      fTargetMaterial = global_sample_configuration.get<std::string>(
          "target_material", fTargetMaterial);
    }
    if (!fFluxDescription.length()) {
      fFluxDescription = global_sample_configuration.get<std::string>(
          "flux_description", fFluxDescription);
    }
    if (!fSignalDescription.length()) {
      fSignalDescription = global_sample_configuration.get<std::string>(
          "signal_description", fSignalDescription);
    }

    if (fIsShapeOnly == -1) {
      fIsShapeOnly = global_sample_configuration.get<bool>("shape_only", false);
    }
    if (fIsFluxUnfolded == -1) {
      fIsFluxUnfolded =
          global_sample_configuration.get<bool>("flux_unfolded", false);
    }

    if ((EnuRange.first == std::numeric_limits<double>::max()) &&
        (global_sample_configuration.has_key("enu_range"))) {
      EnuRange = global_sample_configuration.get<std::pair<double, double>>(
          "enu_range");
    }
  }

  virtual std::string GetJournalReference() { return fJournalReference; }
  virtual std::string GetTargetMaterial() { return fTargetMaterial; }
  virtual std::string GetFluxDescription() { return fFluxDescription; }
  virtual std::string GetSignalDescription() { return fSignalDescription; }

  void SetShapeOnly(bool iso) { fIsShapeOnly = iso; }
  void SetFluxUnfolded(bool ifo) { fIsFluxUnfolded = ifo; }

  void SetData(std::string const &data_descriptor) {
    fDataInputDescriptor = data_descriptor;
  }

  void SetCovariance(std::string const &cov_descriptor) {
    fCovarianceInputDescriptor = cov_descriptor;
  }

  virtual void FillProjection(std::array<double, NDim> const &proj,
                              double event_weight) {
    nuis::utility::Fill(fPrediction.get(), proj, event_weight);
  }

  virtual void FinalizeComparison() {
    fPrediction_xsec = nuis::utility::Clone(fPrediction);
    fPrediction_xsec->Scale(1.0, "width");
    fPrediction_shape = nuis::utility::Clone(fPrediction_xsec);
    if (fData) {
      fPrediction_shape->Scale(fData->Integral() /
                               fPrediction_shape->Integral());
    } else {
      ISAMPLE_WARN("When Finalizing comparison, no Data histogram available.");
    }

    if (fIsFluxUnfolded) {
      // fPrediction_comparison
    } else if (fIsShapeOnly) {
      fPrediction_comparison = nuis::utility::Clone(fPrediction_shape);
    } else {
      fPrediction_comparison = nuis::utility::Clone(fPrediction_xsec);
    }
    fComparisonFinalized = true;
  }

  void Initialize(fhicl::ParameterSet const &ps) {

    if (ps.has_key("verbosity")) {
      SetSampleVerbosity(ps.get<std::string>("verbosity"));
    } else {
      SetSampleVerbosity("Reticent");
    }

    ReadGlobalConfigDefaults();

    fhicl::ParameterSet const &global_sample_configuration =
        nuis::config::GetDocument().get<fhicl::ParameterSet>(
            std::string("global.sample_configuration.") + Name(),
            fhicl::ParameterSet());

    if (ps.has_key("fake_data")) {
      fData = nuis::utility::GetHistogram<typename TH_dim_helper<NDim>::type>(
          ps.get<std::string>("fake_data_histogram"));
    } else {
      if (!fDataInputDescriptor.length()) {
        if (!global_sample_configuration.has_key("data_descriptor")) {
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
            global_sample_configuration.get<std::string>("data_descriptor");
      }
      fData = nuis::utility::GetHistogram<typename TH_dim_helper<NDim>::type>(
          fDataInputDescriptor);
    }

    fPrediction = nuis::utility::Clone(fData, true);

    if (fCovarianceInputDescriptor.length()) {
      fCovariance =
          nuis::utility::GetHistogram<TH2D>(fCovarianceInputDescriptor);
    } else if (global_sample_configuration.has_key("covariance_descriptor")) {
      fCovariance = nuis::utility::GetHistogram<TH2D>(
          global_sample_configuration.get<std::string>(
              "covariance_descriptor"));
    }

    if (ps.has_key("verbosity")) {
      SetSampleVerbosity(ps.get<std::string>("verbosity"));
    }

    NMaxSample_override =
        ps.get<size_t>("nmax", std::numeric_limits<size_t>::max());

    write_directory = ps.get<std::string>("write_directory", "");

    fIH_id = nuis::input::InputManager::Get().EnsureInputLoaded(ps);
  }
  void ProcessSample(size_t nmax = std::numeric_limits<size_t>::max()) {
    if (fIH_id ==
        std::numeric_limits<nuis::input::InputManager::Input_id_t>::max()) {
      throw uninitialized_ISample();
    }
    IInputHandler const &IH =
        nuis::input::InputManager::Get().GetInputHandler(fIH_id);

    nmax = std::min(NMaxSample_override, nmax);

    size_t NEvsToProcess = std::min(nmax, IH.GetNEvents());

    double nmax_scaling = double(IH.GetNEvents()) / double(NEvsToProcess);

    size_t NToShout = NEvsToProcess / 10;
    ISAMPLE_INFO("Sample " << std::quoted(Name()) << " processing "
                           << NEvsToProcess << " events.");

    IInputHandler::ev_index_t ev_idx = 0;
    size_t NSigEvents = 0;

    bool DetermineSignalEvents = !fSignalCache.size();

    nuis::utility::Clear(fPrediction.get());
    fComparisonFinalized = false;

    while (ev_idx < NEvsToProcess) {
      if (DetermineSignalEvents) {
        nuis::event::FullEvent const &fev = IH.GetFullEvent(ev_idx);
        bool is_sig = IsSigFunc(fev);
        fSignalCache.push_back(is_sig);
        if (is_sig) {
          fProjectionCache.push_back(CompProjFunc(fev));
        }
      }

      if (NToShout && !(ev_idx % NToShout)) {
        ISAMPLE_INFO("\tProcessed " << ev_idx << "/" << NEvsToProcess
                                    << " events.");
      }

      if (fSignalCache[ev_idx]) {
        FillProjection(fProjectionCache[ev_idx],
                       IH.GetEventWeight(ev_idx) * nmax_scaling);
      }

      ev_idx++;
    }
    ISAMPLE_INFO("\t" << fProjectionCache.size() << "/" << NEvsToProcess
                      << " events passed selection.");
  }
  void Write() {
    if (!fComparisonFinalized) {
      FinalizeComparison();
    }

    nuis::persistency::WriteToOutputFile<typename TH_dim_helper<NDim>::type>(
        fPrediction_comparison.get(), "Prediction", write_directory);
    nuis::persistency::WriteToOutputFile<typename TH_dim_helper<NDim>::type>(
        fPrediction_xsec.get(), "Prediction_xsec", write_directory);

    if (fData) {
      nuis::persistency::WriteToOutputFile<typename TH_dim_helper<NDim>::type>(
          fData.get(), "Data", write_directory);
      nuis::persistency::WriteToOutputFile<typename TH_dim_helper<NDim>::type>(
          fPrediction_shape.get(), "Prediction_shape", write_directory);
    }
  }

  double GetGOF() { return 1; }
  double GetNDOGuess() {
    if (fData) {
      return nuis::utility::TH_traits<
          typename TH_dim_helper<NDim>::type>::NbinsIncludeFlow(fData.get());
    }
    return 0;
  }

  fhicl::ParameterSet GetExampleConfiguration() {
    fhicl::ParameterSet exps;
    exps.put<std::string>("name", Name());
    exps.put<std::string>("input_type", "Generator");
    exps.put<std::string>("file", "Events.root");
    exps.put<std::string>("write_directory", Name());
    exps.put<std::string>("fake_data_histogram", "/path/to/file.root;h_name");

    return exps;
  }
};

typedef SimpleDataComparison<1> SimpleDataComparison_1D;
typedef SimpleDataComparison<2> SimpleDataComparison_2D;

#endif
