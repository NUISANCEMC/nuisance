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

#ifndef SAMPLES_ISAMPLE_HXX_SEEN
#define SAMPLES_ISAMPLE_HXX_SEEN

#include "plugins/traits.hxx"

#include "exception/exception.hxx"

#include "config/GlobalConfiguration.hxx"

#include "fhiclcpp/ParameterSet.h"

#include <iomanip>
#include <limits>
#include <string>

namespace nuis {
namespace event {
class FullEvent;
class MinimalEvent;
} // namespace event
} // namespace nuis

#define ISAMPLE_DEBUG(v)                                                       \
  if (verb > 2) {                                                              \
    std::cout << "[DEBUG]: " << v << std::endl;                                \
  }
#define ISAMPLE_INFO(v)                                                        \
  if (verb > 1) {                                                              \
    std::cout << "[INFO]: " << v << std::endl;                                 \
  }
#define ISAMPLE_WARN(v)                                                        \
  if (verb > 0) {                                                              \
    std::cout << "[WARN] " << __FILENAME__ << ":" << __LINE__ << " : " << v    \
              << std::endl;                                                    \
  }

class ISample {
protected:
  NEW_NUIS_EXCEPT(unknown_ISample_verbosity);

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
      throw unknown_ISample_verbosity()
          << "[ERROR]: Failed to parse ISample verbosity setting from: "
          << std::quoted(v);
    }
  }

public:
  NEW_NUIS_EXCEPT(uninitialized_ISample);
  NEW_NUIS_EXCEPT(unimplemented_ISample_optional_method);

  ISample() {
    SetSampleVerbosity(nuis::config::GetDocument().get<std::string>(
        "global.sample.verbosity_default", "Silent"));
  }

  virtual void Initialize(fhicl::ParameterSet const &) = 0;

  // Interface for processing a single, external event
  //
  // ISamples are not required to implement processing events from 'outside'.
  virtual void ProcessEvent(nuis::event::FullEvent const &) {
    throw unimplemented_ISample_optional_method()
        << "[ERROR]: ISample " << Name() << " does not implement ProcessEvent.";
  }

  virtual void
  ProcessSample(size_t nmax = std::numeric_limits<size_t>::max()) = 0;

  virtual void Write() = 0;

  virtual std::string Name() = 0;

  virtual ~ISample() {}
};

DECLARE_PLUGIN_INTERFACE(ISample);

#endif
