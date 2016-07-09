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

#ifndef MINIBOONE_CCQE_XSEC_2DTcos_ANTINU_H_SEEN
#define MINIBOONE_CCQE_XSEC_2DTcos_ANTINU_H_SEEN

#include "Measurement2D.h"

//******************************************************************** 
class MiniBooNE_CCQE_XSec_2DTcos_antinu : public Measurement2D {
//******************************************************************** 

public:

  MiniBooNE_CCQE_XSec_2DTcos_antinu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~MiniBooNE_CCQE_XSec_2DTcos_antinu() {};
  
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

 private:
  double Ekmu, costheta, q2qe;
  bool bad_particle, ccqelike;
};
  
#endif
