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

#ifndef MINERvA_CC0pi_XSec_1DFill_nue_H_SEEN
#define MINERvA_CC0pi_XSec_1DFill_nue_H_SEEN
#include"Measurement1D.h"

//********************************************************************  
class MINERvA_CC0pi_XSec_1DFill_nue : public Measurement1D {
//********************************************************************  

public:

  MINERvA_CC0pi_XSec_1DFill_nue(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile, bool neutrino_mode, int fillvar );
  virtual ~MINERvA_CC0pi_XSec_1DFill_nue() {};

  // Functions for handling each neut event
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

 private:

  double Enu_rec;
  double Ee, Q2QEe, Thetae;
  bool anty_flag;
  bool nue_flag;
  bool bad_particle;

  bool Fill1DEe, Fill1DQ2e, Fill1DThetae;
};
  
#endif
