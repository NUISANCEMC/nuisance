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

#ifndef SAMPLES_IEventProcessor_HXX_SEEN
#define SAMPLES_IEventProcessor_HXX_SEEN

#include "plugins/traits.hxx"

#include "exception/exception.hxx"

#include "config/GlobalConfiguration.hxx"

#include "fhiclcpp/ParameterSet.h"

#include "TH1.h"

#include <iomanip>
#include <limits>
#include <string>

namespace nuis {
namespace event {
class FullEvent;
class MinimalEvent;
} // namespace event
} // namespace nuis

#define IEventProcessor_DEBUG(v)                                               \
  if (verb > 2) {                                                              \
    std::cout << "[DEBUG]: " << v << std::endl;                                \
  }
#define IEventProcessor_INFO(v)                                                \
  if (verb > 1) {                                                              \
    std::cout << "[INFO]: " << v << std::endl;                                 \
  }
#define IEventProcessor_WARN(v)                                                \
  if (verb > 0) {                                                              \
    std::cout << "[WARN] " << __FILENAME__ << ":" << __LINE__ << " : " << v    \
              << std::endl;                                                    \
  }

class IEventProcessor {
protected:
  NEW_NUIS_EXCEPT(unknown_IEventProcessor_verbosity);

  enum sample_verbosity { kSilent = 0, kWarn = 1, kReticent = 2, kVerbose = 3 };
  sample_verbosity verb;

  void SetSampleVerbosity(std::string v) {
    v = nuis::config::GetDocument().get<std::string>(
        "global.sample.verbosity_override", v);

    if (v == "Silent") {
      verb = kSilent;
    } else if (v == "Warn") {
      verb = kWarn;
    } else if (v == "Reticent") {
      verb = kReticent;
    } else if (v == "Verbose") {
      verb = kVerbose;
    } else {
      throw unknown_IEventProcessor_verbosity()
          << "[ERROR]: Failed to parse IEventProcessor verbosity setting from: "
          << std::quoted(v);
    }
  }

public:
  NEW_NUIS_EXCEPT(uninitialized_IEventProcessor);
  NEW_NUIS_EXCEPT(unimplemented_IEventProcessor_optional_method);

  IEventProcessor() {
    SetSampleVerbosity(nuis::config::GetDocument().get<std::string>(
        "global.sample.verbosity_default", "Silent"));

    TH1::SetDefaultSumw2();
  }

  virtual void Initialize(fhicl::ParameterSet const &) = 0;

  // Interface for processing a single, external event
  //
  // IEventProcessors are not required to implement processing events from
  // 'outside'.
  virtual void ProcessEvent(nuis::event::FullEvent const &, double weight = 1) {
    throw unimplemented_IEventProcessor_optional_method()
        << "[ERROR]: IEventProcessor " << Name()
        << " does not implement ProcessEvent.";
  }

  // Interface for processing a pre-selected single, external event
  //
  // IEventProcessors are not required to implement processing events from
  // 'outside'.
  virtual void ProcessSignalEvent(nuis::event::FullEvent const &,
                                  double weight = 1) {
    throw unimplemented_IEventProcessor_optional_method()
        << "[ERROR]: IEventProcessor " << Name()
        << " does not implement ProcessSignalEvent.";
  }

  virtual void
  ProcessSample(size_t nmax = std::numeric_limits<size_t>::max()) = 0;

  virtual void Write() = 0;

  virtual std::string Name() = 0;

  virtual ~IEventProcessor() {}
};

DECLARE_PLUGIN_INTERFACE(IEventProcessor);

#endif
