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

#ifndef T2K_SIGNALDEF_H_SEEN
#define T2K_SIGNALDEF_H_SEEN

#include "SignalDef.h"

namespace SignalDef {
  bool isCC1pip_T2K_H2O(FitEvent *event, double EnuMin, double EnuMax);
  bool isCC1pip_T2K_CH(FitEvent *event, double EnuMin, double EnuMax, bool Michel);

  bool isT2K_CC0pi(FitEvent* event, double EnuMin, double EnuMax, bool forwardgoing);
  bool isT2K_CC0pi_STV(FitEvent* event, double EnuMin, double EnuMax);
}

#endif
