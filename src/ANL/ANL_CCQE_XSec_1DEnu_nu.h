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

#ifndef ANL_CCQE_XSEC_1DENU_NU_H_SEEN
#define ANL_CCQE_XSEC_1DENU_NU_H_SEEN
#include "Measurement1D.h"

//******************************************************************** 
class ANL_CCQE_XSec_1DEnu_nu : public Measurement1D {
//******************************************************************** 

public:

  ANL_CCQE_XSec_1DEnu_nu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~ANL_CCQE_XSec_1DEnu_nu() {};
  
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *nvect);
  void ScaleEvents();
  void FillHistograms();

 private:
  double q2qe;
  bool applyQ2correction;
  TH1D* CorrectionHist;

};
  
#endif
