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

#ifndef MINERVA_CCinc_XSec_1DEnu_ratio_H_SEEN
#define MINERVA_CCinc_XSec_1DEnu_ratio_H_SEEN

// Fit Includes
#include "MeasurementBase.h"
#include "JointMeas1D.h"
#include "MINERvA_CCinc_XSec_1DEnu_nu.h"

class MINERvA_CCinc_XSec_1DEnu_ratio : public JointMeas1D {
public:

  MINERvA_CCinc_XSec_1DEnu_ratio(std::string name, std::string inputfiles, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~MINERvA_CCinc_XSec_1DEnu_ratio() {};

  void MakePlots();

  // This is a dummy function as it is not required for the ratio (and does bad bad things)
  void ScaleEvents(){return;};;

  void SetCovarMatrixFromText(std::string covarFile, int dim);

  // Custom write function because the ratio has much more limited information available than normal.
  void Write(std::string drawOpt);

 private:

  // This is a dummy, the signal is defined separately for each sample!
  bool isSignal(){return false;};  

  // Need to have the distributions for the numerator and denominator stored separately
  MINERvA_CCinc_XSec_1DEnu_nu * NUM;
  MINERvA_CCinc_XSec_1DEnu_nu * DEN;
  Int_t nBins;
  std::string target;

};
  
#endif
