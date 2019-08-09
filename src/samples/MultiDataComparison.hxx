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

/// Wraps multiple projections of the same signal selection
class MultiDataComparison : public IDataComparison {

  NEW_NUIS_EXCEPT(invalid_MultiDataComparison_initialization);
  NEW_NUIS_EXCEPT(MultiDataComparison_already_finalized);

protected:
  std::string fName;

  nuis::input::InputManager::Input_id_t fIH_id;
  std::string fWrite_directory;
  size_t NMaxSample_override;

  std::string fJournalReference;
  std::string fDOI;
  std::string fYear;
  std::string fTargetMaterial;
  std::string fFluxDescription;
  std::string fSignalDescription;

  std::function<bool(nuis::event::FullEvent const &)> IsSigFunc;

  std::vector<std::pair<std::string, std::unique_ptr<IDataComparison>>>
      Comparisons;

  nuis::utility::KinematicRange energy_cut;

public:
  MultiDataComparison(std::string name) {
    fName = std::move(name);
    fIH_id = std::numeric_limits<nuis::input::InputManager::Input_id_t>::max();
    fWrite_directory = "";
    NMaxSample_override = std::numeric_limits<size_t>::max();

    fJournalReference = "";
    fDOI = "";
    fYear = "";
    fTargetMaterial = "";
    fFluxDescription = "";
    fSignalDescription = "";

    energy_cut = nuis::utility::KinematicRange{
        std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
  }

  std::string Name() { return fName; }

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

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    fInstanceConfig = instance_sample_configuration;

    SetSampleVerbosity(fInstanceConfig.get<std::string>(
        "verbosity", nuis::config::GetDocument().get<std::string>(
                         "global.sample.verbosity_default", "Reticent")));

    ReadGlobalConfigDefaults();

    if (fInstanceConfig.has_key("verbosity")) {
      SetSampleVerbosity(fInstanceConfig.get<std::string>("verbosity"));
    }

    NMaxSample_override =
        fInstanceConfig.get<size_t>("nmax", std::numeric_limits<size_t>::max());

    if (!fWrite_directory.size()) {
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

    size_t NSig = 0;
    while (ev_idx < NEvsToProcess) {
      bool is_sig = false;

      nuis::event::FullEvent const &fev = IH.GetFullEvent(ev_idx);

      if (NToShout && !(ev_idx % NToShout)) {
        IEventProcessor_INFO("\tProcessed " << ev_idx << "/" << NEvsToProcess
                                            << " events.");
      }

      if (IsSigFunc(fev)) {
        NSig++;
        for (auto &comp : Comparisons) {
          comp.second->ProcessSignalEvent(fev, IH.GetEventWeight(ev_idx) *
                                                   nmax_scaling);
        }
      }

      ev_idx++;
    }
    IEventProcessor_INFO("\t" << NSig << "/" << NEvsToProcess
                              << " events passed selection.");
  }
  void Write() {}

  double GetGOF() {
    double totGOF = 0;
    for (auto const &comp : Comparisons) {
      double sGOF = comp.second->GetGOF();
      if (sGOF != std::numeric_limits<double>::max()) {
        totGOF += sGOF;
      } else {
        std::cout << "[WARN]: Projection \"" << comp.first << "\" of sample "
                  << Name() << " produced bad GOF." << std::endl;
      }
    }
    return totGOF;
  }
  double GetNDOGuess() {
    size_t totNDOGuess = 0;
    for (auto const &comp : Comparisons) {
      totNDOGuess += comp.second->GetNDOGuess();
    }
    return totNDOGuess;
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
