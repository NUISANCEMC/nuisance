// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#ifndef MINERVA_NUKECC0PI_XSEC_2D_NU_H_SEEN
#define MINERVA_NUKECC0PI_XSEC_2D_NU_H_SEEN

#include "Measurement2D.h"

enum measurement {
  ch,
  carbon,
  h2o,
  fe,
  pb,
  ch_c_flux,
  ch_h2o_flux,
  ch_fe_flux,
  ch_pb_flux
};

//********************************************************************
class MINERvA_NukeCC0pi_XSec_2D_nu : public Measurement2D {
//********************************************************************

 public:

  // Constructor
  MINERvA_NukeCC0pi_XSec_2D_nu(nuiskey samplekey);

  // Destructor
  virtual ~MINERvA_NukeCC0pi_XSec_2D_nu() {};

  // Required functions
  bool isSignal(FitEvent *nvect);
  void FillEventVariables(FitEvent *event);

 protected:
  // Converted covariance matrix to provide global binning method in GetLikelihood
  // double GetLikelihood();

  // Set up settings based on distribution
  void _SetupDataSettings(measurement measurement);
  // Do nothing and then have the subclasses change which settings are used
  virtual void SetupDataSettings() {};

  std::string covar_name;
};

class MINERvA_NukeCC0pi_CH_XSec_2D_nu : public MINERvA_NukeCC0pi_XSec_2D_nu {
  public: using MINERvA_NukeCC0pi_XSec_2D_nu::MINERvA_NukeCC0pi_XSec_2D_nu;
  protected:
    virtual void SetupDataSettings() { _SetupDataSettings(ch); }
};

class MINERvA_NukeCC0pi_C_XSec_2D_nu : public MINERvA_NukeCC0pi_XSec_2D_nu {
  public: using MINERvA_NukeCC0pi_XSec_2D_nu::MINERvA_NukeCC0pi_XSec_2D_nu;
  protected:
    virtual void SetupDataSettings() { _SetupDataSettings(carbon); }
};

class MINERvA_NukeCC0pi_H2O_XSec_2D_nu : public MINERvA_NukeCC0pi_XSec_2D_nu {
  public: using MINERvA_NukeCC0pi_XSec_2D_nu::MINERvA_NukeCC0pi_XSec_2D_nu;
  protected:
    virtual void SetupDataSettings() { _SetupDataSettings(h2o); }
};

class MINERvA_NukeCC0pi_Fe_XSec_2D_nu : public MINERvA_NukeCC0pi_XSec_2D_nu {
  public: using MINERvA_NukeCC0pi_XSec_2D_nu::MINERvA_NukeCC0pi_XSec_2D_nu;
  protected:
    virtual void SetupDataSettings() { _SetupDataSettings(fe); }
};

class MINERvA_NukeCC0pi_Pb_XSec_2D_nu : public MINERvA_NukeCC0pi_XSec_2D_nu {
  public: using MINERvA_NukeCC0pi_XSec_2D_nu::MINERvA_NukeCC0pi_XSec_2D_nu;
  protected:
    virtual void SetupDataSettings() { _SetupDataSettings(pb); }
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_2D_nu : public MINERvA_NukeCC0pi_XSec_2D_nu {
  public: using MINERvA_NukeCC0pi_XSec_2D_nu::MINERvA_NukeCC0pi_XSec_2D_nu;
  protected:
    virtual void SetupDataSettings() { _SetupDataSettings(ch_c_flux); }
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_2D_nu : public MINERvA_NukeCC0pi_XSec_2D_nu {
  public: using MINERvA_NukeCC0pi_XSec_2D_nu::MINERvA_NukeCC0pi_XSec_2D_nu;
  protected:
    virtual void SetupDataSettings() { _SetupDataSettings(ch_h2o_flux); }
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_2D_nu : public MINERvA_NukeCC0pi_XSec_2D_nu {
  public: using MINERvA_NukeCC0pi_XSec_2D_nu::MINERvA_NukeCC0pi_XSec_2D_nu;
  protected:
    virtual void SetupDataSettings() { _SetupDataSettings(ch_fe_flux); }
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_2D_nu : public MINERvA_NukeCC0pi_XSec_2D_nu {
  public: using MINERvA_NukeCC0pi_XSec_2D_nu::MINERvA_NukeCC0pi_XSec_2D_nu;
  protected:
    virtual void SetupDataSettings() { _SetupDataSettings(ch_pb_flux); }
};

#endif
