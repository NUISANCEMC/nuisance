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
#include "EfficiencyApplicator.h"
#include "GaussianSmearer.h"
#include "ThresholdAccepter.h"
#include "TrackedMomentumMatrixSmearer.h"

#include "MetaSimpleSmearcepter.h"

void MetaSimpleSmearcepter::SpecifcSetup(nuiskey &nk) {
  std::map<std::string, SmearceptionFactory_fcn> factories;

  factories["ThresholdAccepter"] = &BuildSmearcepter<ThresholdAccepter>;
  factories["EfficiencyApplicator"] = &BuildSmearcepter<EfficiencyApplicator>;
  factories["GaussianSmearer"] = &BuildSmearcepter<GaussianSmearer>;
  factories["VisECoalescer"] = &BuildSmearcepter<VisECoalescer>;
  factories["TrackedMomentumMatrixSmearer"] =
      &BuildSmearcepter<TrackedMomentumMatrixSmearer>;

  std::vector<nuiskey> smearcepters = nk.GetListOfChildNodes();
  for (size_t smear_it = 0; smear_it < smearcepters.size(); ++smear_it) {
    std::string const &smearType = smearcepters[smear_it].GetElementName();

    if (smearType == "EnergyShuffler") {
      ES = new EnergyShuffler();
      ES->Setup(smearcepters[smear_it]);
      continue;
    }

    if (!factories.count(smearType)) {
      ERROR(WRN, "No known smearer accepts elements named: \"" << smearType
                                                               << "\"");
      continue;
    }

    Smearcepters.push_back(factories[smearType](smearcepters[smear_it]));

    QLOG(FIT, "MetaSimpleSmearcepter adopted child smearcepter: "
                  << Smearcepters.back()->GetName()
                  << " of type: " << Smearcepters.back()->GetElementName());
  }
  NSmearcepters = Smearcepters.size();
}
RecoInfo *MetaSimpleSmearcepter::Smearcept(FitEvent *fe) {
  if (ES) {
    ES->DoTheShuffle(fe);
  }
  RecoInfo *ri = NULL;
  for (size_t sm_it = 0; sm_it < NSmearcepters; ++sm_it) {
    if (!sm_it) {
      ri = Smearcepters[sm_it]->Smearcept(fe);
    } else {
      Smearcepters[sm_it]->SmearRecoInfo(ri);
    }
  }
  return ri;
}
