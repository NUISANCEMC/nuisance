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

#ifndef MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu_H_SEEN
#define MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu_H_SEEN

// Fit Includes
#include "MeasurementBase.h"
#include "JointMeas1D.h"
#include "MINERvA_CC0pi_XSec_1DQ2_Tgt_nu.h"

///\brief MINERvA CC0pi1p Target Ratio Analysis : Q2 Distribution                
///
///\n [arXiv:1705.03791]                          
///\n Input: Target events generated with all interaction modes with flux given in Ref.
///   handles multiple targets : C, CH, Fe, Pb  
class MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu : public JointMeas1D {
public:

  MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu(nuiskey samplekey);
  virtual ~MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu() {};

  void MakePlots();

  // This is a dummy function as it is not required for the ratio (and does bad bad things)
  void ScaleEvents(){return;};;

 private:

  // This is a dummy, the signal is defined separately for each sample!
  bool isSignal(){return false;};  

  // Need to have the distributions for the numerator and denominator stored separately
  MINERvA_CC0pi_XSec_1DQ2_Tgt_nu * NUM;
  MINERvA_CC0pi_XSec_1DQ2_Tgt_nu * DEN;

  Int_t nBins;
  std::string target;

};
  
#endif
