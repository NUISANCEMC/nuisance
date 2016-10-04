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
#ifndef T2K_CC0PI_2DPCOS_NU_H_SEEN
#define T2K_CC0PI_2DPCOS_NU_H_SEEN

#include "Measurement2D.h"

class T2K_CC0pi_XSec_2DPcos_nu : public Measurement2D {
public:

  /// Basic Constructor.
  /// /brief Parses two different measurements.
  ///
  /// T2K_CC0pi_XSec_2DPcos_nu    -> T2K CC0PI Analysis 2
  /// T2K_CC0pi_XSec_2DPcos_nu_I  -> T2K CC0PI Analysis 1
  /// T2K_CC0pi_XSec_2DPcos_nu_II -> T2K CC0PI Analysis 2
  T2K_CC0pi_XSec_2DPcos_nu(std::string name, std::string inputfile, FitWeight *rw, std::string type);

  /// Virtual Destructor
  ~T2K_CC0pi_XSec_2DPcos_nu() {};

  /// Return NDOF
  /// /brief Have to override default because binning is awful
  int GetNDOF();

  /// Numu CC0PI Signal Definition
  ///
  /// /item 
  bool isSignal(FitEvent *nvect);

  /// Read histograms in a special way because format is different.
  /// Read from FitPar::GetDataBase()+"/T2K/CC0pi/T2K_CC0PI_2DPmuCosmu_Data.root"
  void SetHistograms();

  /// Bin Tmu CosThetaMu
  void FillEventVariables(FitEvent* customEvent);

  /// Have to do a weird event scaling for analysis 1
  void ConvertEventRates();

 private:

  bool forwardgoing;
  bool only_allowed_particles;
  bool numu_event;
  double numu_energy;
  int particle_pdg;
  double pmu, CosThetaMu;
  int fAnalysis;

  bool fIsSystCov, fIsStatCov, fIsNormCov;
  
};
  
#endif
