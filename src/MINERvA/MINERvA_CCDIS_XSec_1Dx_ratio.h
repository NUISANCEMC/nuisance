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

#ifndef MINERVA_CCDIS_XSec_1Dx_ratio_H_SEEN
#define MINERVA_CCDIS_XSec_1Dx_ratio_H_SEEN

// Fit Includes
#include "MeasurementBase.h"
#include "JointMeas1D.h"
#include "MINERvA_CCDIS_XSec_1Dx_nu.h"

class MINERvA_CCDIS_XSec_1Dx_ratio : public JointMeas1D {
public:

  MINERvA_CCDIS_XSec_1Dx_ratio(nuiskey samplekey);
  virtual ~MINERvA_CCDIS_XSec_1Dx_ratio() {};

  void MakePlots();

  // This is a dummy function as it is not required for the ratio (and does bad bad things)
  void ScaleEvents(){return;};

 private:

  // This is a dummy, the signal is defined separately for each sample!
  bool isSignal(){return false;};  

  // Need to have the distributions for the numerator and denominator stored separately
  MINERvA_CCDIS_XSec_1Dx_nu * NUM;
  MINERvA_CCDIS_XSec_1Dx_nu * DEN;
  Int_t nBins;
  std::string target;

};
  
#endif
