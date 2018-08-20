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

#ifndef SAMPLES_IDATACOMPARISON_HXX_SEEN
#define SAMPLES_IDATACOMPARISON_HXX_SEEN

#include "samples/ISample.hxx"

#include "fhiclcpp/ParameterSet.h"

#include <iomanip>
#include <sstream>

class IDataComparison : public ISample {
public:
  virtual double GetGOF() = 0;

  virtual std::string GetJournalReference() {
    std::stringstream ss("");
    ss << "Unknown Journal Ref. for IDataComparison: " << std::quoted(Name());
    return ss.str();
  }

  virtual std::string GetTargetMaterial() {
    std::stringstream ss("");
    ss << "Unknown Target material for IDataComparison: " << std::quoted(Name());
    return ss.str();
  }

  virtual std::string GetFluxDescription() {
    std::stringstream ss("");
    ss << "Unknown Flux description for IDataComparison: " << std::quoted(Name());
    return ss.str();
  }

  virtual std::string GetSignalDescription() {
    std::stringstream ss("");
    ss << "Unknown Signal description for IDataComparison: " << std::quoted(
        Name());
    return ss.str();
  }

  virtual std::string GetDocumentation() {
    std::stringstream ss("");
    ss << "No documentation provided for IDataComparison: " << std::quoted(Name());
    return ss.str();
  }

  virtual fhicl::ParameterSet GetExampleConfiguration() {
    return fhicl::ParameterSet();
  }
};

DECLARE_PLUGIN_INTERFACE(IDataComparison);

#endif
