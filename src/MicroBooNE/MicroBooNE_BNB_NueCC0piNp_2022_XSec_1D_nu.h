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

// MicroBooNE BNB nue CC0pi https://doi.org/10.1103/PhysRevD.106.L051102
// Only considers Np meausurements ; E_e, cos th_e, and cos th_p

#ifndef MICROBOONE_BNB_NUECC0PINP_2022_1D_NU_H_SEEN
#define MICROBOONE_BNB_NUECC0PINP_2022_1D_NU_H_SEEN

#include "Measurement1D.h"

#include <iostream>
#include <fstream>

class TH2D;

class MicroBooNE_BNB_NueCC0piNp_2022_XSec_1D_nu : public Measurement1D {
public:
  /// Basic Constructor.
  MicroBooNE_BNB_NueCC0piNp_2022_XSec_1D_nu(nuiskey samplekey);

  /// Virtual Destructor
  ~MicroBooNE_BNB_NueCC0piNp_2022_XSec_1D_nu() {};

  /// Apply signal definition
  bool isSignal(FitEvent* nvect);

  /// Fill kinematic distributions
  void FillEventVariables(FitEvent* customEvent);

  // Convert to xsec units 
  void ConvertEventRates();

private:
  enum Distribution { kElecCosTheta, kElecEnergy, kProtonCosTheta }; 
  Distribution fDist;

};

#endif
