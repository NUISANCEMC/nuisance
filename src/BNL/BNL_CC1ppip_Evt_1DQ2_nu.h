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

#ifndef BNL_CC1PPIP_EVT_1DQ2_NU_H_SEEN
#define BNL_CC1PPIP_EVT_1DQ2_NU_H_SEEN

#include "Measurement1D.h"
// T. Kitagaki et al. "Charged-current exclusive pion production in neutrino-deuterium interactions"
// Physical Review D, Volume 34, Number 9, 1 Nov 1986
// Figure 3
//
// K. Furuno et al. NuInt02 proceedings, (supposedly published in Nucl. Phys. B but I never found it), Retreieved from KEK preprints.
// KEK Preprint 2003-48, RCNS-03-01, September 2003
// Figure 8
//
// Also see 
// N. J. Baker et al. "Study of the isospin structure of single-pion production in charged-current neutrino interactions"
// Physical Review D, Volume 23, Number 11, 1 June 1981

class BNL_CC1ppip_Evt_1DQ2_nu : public Measurement1D {
public:
  BNL_CC1ppip_Evt_1DQ2_nu(nuiskey samplekey);
  virtual ~BNL_CC1ppip_Evt_1DQ2_nu() {};
  
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

 private:
  double HadCut; // Hadronic mass cut, specified by user (W < 1.4 or no W cut)
};
  
#endif
