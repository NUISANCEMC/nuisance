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

#ifndef MINIBOONE_NC1PI0_XSEC_1DCOSPI0_NUBAR_H_SEEN
#define MINIBOONE_NC1PI0_XSEC_1DCOSPI0_NUBAR_H_SEEN

#include "Measurement1D.h"

class MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu : public Measurement1D {
public:

  MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu(nuiskey samplekey);
  virtual ~MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu() {};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  private:
  /* As MB provides data for the nu/antinu siganl of nu/antinu running
     mode, as well as the combined nu+antinu signal for each mode, set
     these bools in the .cxx to select which signal to use from which mode*/ 
  bool nunubar_mode;

};

#endif
