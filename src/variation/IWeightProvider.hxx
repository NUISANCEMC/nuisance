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

#ifndef VARIATION_IWEIGHTPROVIDER_HXX_SEEN
#define VARIATION_IWEIGHTPROVIDER_HXX_SEEN

#include "variation/IVariationProvider.hxx"

class IWeightProvider {
public:
  double GetEventWeight(nuis::event::MinimalEvent) = 0;

  virtual ~IWeightProvider() {}
};

DECLARE_PLUGIN_INTERFACE(IWeightProvider);

#endif
