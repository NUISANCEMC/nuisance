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

#ifndef MINERvA_CC0pi_XSec_1DThetae_nue_H_SEEN
#define MINERvA_CC0pi_XSec_1DThetae_nue_H_SEEN

#include "MINERvA_CC0pi_XSec_1DFill_nue.h"
#include "JointMeas1D.h"

//********************************************************************  
class MINERvA_CC0pi_XSec_1DThetae_nue : public JointMeas1D {
//********************************************************************  

public:

  MINERvA_CC0pi_XSec_1DThetae_nue(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~MINERvA_CC0pi_XSec_1DThetae_nue(){};

 private:

  double Enu_rec, q2qe,ThetaMu;
  double Ee;

  MINERvA_CC0pi_XSec_1DFill_nue* nue_sample;
  MINERvA_CC0pi_XSec_1DFill_nue* nuebar_sample;
  
};
  
#endif
