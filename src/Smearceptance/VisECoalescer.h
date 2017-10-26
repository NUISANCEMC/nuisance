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

#ifndef VISECOALESCER_HXX_SEEN
#define VISECOALESCER_HXX_SEEN

#include "ISmearcepter.h"

#include <map>

///Stacks all VisE deposits from particles of the same PDG
class VisECoalescer : public ISmearcepter {
 private:
  void SpecifcSetup(nuiskey &) {}

 public:
  RecoInfo *Smearcept(FitEvent *) {
    THROW("VisECoalescer cannot act as an accepter");
  }

  /// Helper method for using this class as a component in a more complex
  /// smearer
  void SmearRecoInfo(RecoInfo *ri) {
    std::map<Int_t, double> TotalSpeciesVisE;

    for (size_t ve_it = 0; ve_it < ri->RecVisibleEnergy.size(); ++ve_it) {
      if (!TotalSpeciesVisE.count(ri->TrueContribPDGs[ve_it])) {
        TotalSpeciesVisE[ri->TrueContribPDGs[ve_it]] = 0;
      }
      TotalSpeciesVisE[ri->TrueContribPDGs[ve_it]] +=
          ri->RecVisibleEnergy[ve_it];
    }

    ri->RecVisibleEnergy.clear();
    ri->TrueContribPDGs.clear();

    for (std::map<Int_t, double>::iterator ve_it = TotalSpeciesVisE.begin();
         ve_it != TotalSpeciesVisE.end(); ++ve_it) {
      ri->TrueContribPDGs.push_back(ve_it->first);
      ri->RecVisibleEnergy.push_back(ve_it->second);
    }
  }
};

#endif
