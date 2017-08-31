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

#include "Smearcepterton.h"

#include "ThresholdAccepter.h"
#include "EfficiencyApplicator.h"
#include "GaussianSmearer.h"
#include "TrackedMomentumMatrixSmearer.h"
#include "MetaSimpleSmearcepter.h"

#include <vector>

Smearcepterton* Smearcepterton::_inst = NULL;
Smearcepterton& Smearcepterton::Get() {
  if (!_inst) {
    _inst = new Smearcepterton();
  }
  return *_inst;
}

Smearcepterton::Smearcepterton() { InitialiserSmearcepters(); }

void Smearcepterton::InitialiserSmearcepters() {
  // hard coded list of tag name -> smearcepter factories, add here to add your
  // own.
  std::map<std::string, SmearceptionFactory_fcn> factories;

  factories["ThresholdAccepter"] = &BuildSmearcepter<ThresholdAccepter>;
  factories["EfficiencyApplicator"] = &BuildSmearcepter<EfficiencyApplicator>;
  factories["GaussianSmearer"] = &BuildSmearcepter<GaussianSmearer>;
  factories["TrackedMomentumMatrixSmearer"] = &BuildSmearcepter<TrackedMomentumMatrixSmearer>;
  factories["VisECoalescer"] = &BuildSmearcepter<VisECoalescer>;
  factories["MetaSimpleSmearcepter"] = &BuildSmearcepter<MetaSimpleSmearcepter>;


  std::vector<nuiskey> smearcepterBlocks = Config::QueryKeys("smearcepters");

  for (size_t smearB_it = 0; smearB_it < smearcepterBlocks.size();
       ++smearB_it) {
    std::vector<nuiskey> smearcepters =
        smearcepterBlocks[smearB_it].GetListOfChildNodes();
    for (size_t smear_it = 0; smear_it < smearcepters.size(); ++smear_it) {
      std::string const& smearType = smearcepters[smear_it].GetElementName();

      if (!factories.count(smearType)) {
        ERROR(WRN, "No known smearer accepts elements named: \"" << smearType
                                                                 << "\"");
        continue;
      }

      ISmearcepter* smearer = factories[smearType](smearcepters[smear_it]);

      Smearcepters[smearer->GetName()] = smearer;

      QLOG(FIT, "Configured smearer named: " << smearer->GetName()
                                             << " of type: "
                                             << smearer->GetElementName());
    }
  }
}
