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

#include "samples/IEventProcessor.hxx"

#include "event/FullEvent.hxx"

#include "input/InputManager.hxx"

#include <functional>

class SimpleMCStudy : public IEventProcessor {
protected:
  nuis::input::InputManager::Input_id_t fIH_id;
  std::string write_directory;
  size_t NMaxSample_override;

  std::function<void(nuis::event::FullEvent const &, double)>
      ProcessEventFunction;

public:
  SimpleMCStudy() {
    fIH_id = std::numeric_limits<nuis::input::InputManager::Input_id_t>::max();
    write_directory = "";
    NMaxSample_override = std::numeric_limits<size_t>::max();

    ProcessEventFunction =
        std::function<void(nuis::event::FullEvent const &, double)>();
  }

  fhicl::ParameterSet fGlobalConfig;
  fhicl::ParameterSet fInstanceConfig;

  void ReadGlobalConfigDefaults() {
    fGlobalConfig = nuis::config::GetDocument().get<fhicl::ParameterSet>(
        std::string("global.sample_configuration.") + Name(),
        fhicl::ParameterSet());
  }

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    fInstanceConfig = instance_sample_configuration;

    SetSampleVerbosity(fInstanceConfig.get<std::string>(
        "verbosity", nuis::config::GetDocument().get<std::string>(
                         "global.sample.verbosity_default", "Reticent")));

    ReadGlobalConfigDefaults();

    NMaxSample_override =
        fInstanceConfig.get<size_t>("nmax", std::numeric_limits<size_t>::max());

    write_directory =
        fInstanceConfig.get<std::string>("write_directory", Name());

    fIH_id =
        nuis::input::InputManager::Get().EnsureInputLoaded(fInstanceConfig);
  }

  void ProcessSample(size_t nmax = std::numeric_limits<size_t>::max()) {
    if (fIH_id ==
        std::numeric_limits<nuis::input::InputManager::Input_id_t>::max()) {
      throw uninitialized_IEventProcessor();
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

    while (ev_idx < NEvsToProcess) {
      if (NToShout && !(ev_idx % NToShout)) {
        IEventProcessor_INFO("\t\t Done " << ev_idx << "/" << NEvsToProcess
                                          << " events.");
      }
      nuis::event::FullEvent const &fev = IH.GetFullEvent(ev_idx);
      ProcessEventFunction(fev, IH.GetEventWeight(ev_idx) * nmax_scaling);
      ev_idx++;
    }

    IEventProcessor_INFO("Sample " << std::quoted(Name()) << " processed "
                                   << NEvsToProcess << " events.");
  }
};
