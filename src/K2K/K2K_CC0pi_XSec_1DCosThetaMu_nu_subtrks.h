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

#ifndef K2K_CC0pi_1DCosThetaMu_ALLSAMPLES_H_SEEN
#define K2K_CC0pi_1DCosThetaMu_ALLSAMPLES_H_SEEN
#include "Measurement1D.h"

//********************************************************************     
class K2K_CC0pi_XSec_1DCosThetaMu_nu_subtrks : public Measurement1D {
//********************************************************************     
public:

  K2K_CC0pi_XSec_1DCosThetaMu_nu_subtrks(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~K2K_CC0pi_XSec_1DCosThetaMu_nu_subtrks() {};
  
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);
  void ScaleEvents();

 private:

  bool bad_particle;
  int ncharged;

  TH1D* ccqelike;

  enum k2K_sample_enum
  {
    fK2K_1track=0,
    fK2K_2trackQE,
    fK2K_2trackNONQE,
  };
  
  unsigned int k2k_sample_type;

  unsigned int ncharged_cut;
  double deltaphi_cut;
  double CosThetaMu;
  bool matchnorm;
};
  
#endif
