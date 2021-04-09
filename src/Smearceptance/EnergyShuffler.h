// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#ifndef ENERGYSHUFFLER_HXX_SEEN
#define ENERGYSHUFFLER_HXX_SEEN

#include "ISmearcepter.h"

#include <map>

// #define DEBUG_ESHUFFLER

/// Tool for shuffles KE between systems.
/// Because it needs to run before an accepter has a chance to throw away
/// particles it should not be an ISmearcepter and instead built into other
/// smearcepters.
class EnergyShuffler {

 private:
  struct ShuffleDescriptor {
    std::vector<Int_t> ToPDGs;
    double EFraction;
  };

  std::vector< std::pair<Int_t, ShuffleDescriptor> > ShufflersDescriptors;

 public:
  void Setup(nuiskey &);

  void DoTheShuffle(FitEvent *);
};

#endif
