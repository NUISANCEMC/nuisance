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

#ifndef T2K_CC0PI_2DPCOS_NU_H_SEEN
#define T2K_CC0PI_2DPCOS_NU_H_SEEN

// Fit Includes
#include "Measurement2D.h"

class T2K_CC0pi_XSec_2DPcos_nu : public Measurement2D {
public:

  T2K_CC0pi_XSec_2DPcos_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~T2K_CC0pi_XSec_2DPcos_nu() {};
  int GetNDOF(){return 67;};  
  bool isSignal(FitEvent *nvect);
  void SetHistograms(std::string infile);
  void FillEventVariables(FitEvent* customEvent);

  void Reconfigure(double norm, bool fullconfig);

 private:

  bool forwardgoing;
  bool only_allowed_particles;
  bool numu_event;
  double numu_energy;
  int particle_pdg;
  double pmu, CosThetaMu;
};
  
#endif
