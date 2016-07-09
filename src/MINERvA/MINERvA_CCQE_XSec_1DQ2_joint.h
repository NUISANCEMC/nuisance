// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef MINERVA_1DQ2_joint_H_SEEN
#define MINERVA_1DQ2_joint_H_SEEN


// Fit Includes
#include "MeasurementBase.h"
#include "JointMeas1D.h"
#include "MINERvA_CCQE_XSec_1DQ2_nu.h"
#include "MINERvA_CCQE_XSec_1DQ2_antinu.h"

class MINERvA_CCQE_XSec_1DQ2_joint : public JointMeas1D {
public:

  MINERvA_CCQE_XSec_1DQ2_joint(std::string name, std::string inputfiles, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~MINERvA_CCQE_XSec_1DQ2_joint() {};
  void MakePlots();

 private:

  // This is a dummy, the signal is defined separately for each sample!
  bool isSignal(){return false;};  
  
  bool isFluxFix, fullphasespace;

  // Need to have the neutrino and anti-neutrino objects here
  MINERvA_CCQE_XSec_1DQ2_nu * MIN_nu;
  MINERvA_CCQE_XSec_1DQ2_antinu * MIN_anu;
  Int_t nBins;

};
  
#endif
