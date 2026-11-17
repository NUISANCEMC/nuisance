// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

#ifndef MINERVA_NUKECC0PI_XSEC_1D_NU_H_SEEN
#define MINERVA_NUKECC0PI_XSEC_1D_NU_H_SEEN

#include "Measurement1D.h"

enum measurement_1d {
  target1d_ch,
  target1d_carbon,
  target1d_h2o,
  target1d_fe,
  target1d_pb,
  target1d_ch_c_flux,
  target1d_ch_h2o_flux,
  target1d_ch_fe_flux,
  target1d_ch_pb_flux
};

enum kinematic_1d {
  kin1d_dpt,
  kin1d_dpt_fine,
  kin1d_dptx,
  kin1d_dpty,
  kin1d_pl,
  kin1d_pn,
  kin1d_alpha,
  kin1d_phi,
  kin1d_muon_p,
  kin1d_muon_pt,
  kin1d_muon_theta,
  kin1d_proton_p,
  kin1d_proton_pt,
  kin1d_proton_theta
};

//********************************************************************
class MINERvA_NukeCC0pi_XSec_1D_nu : public Measurement1D {
//********************************************************************

 public:

  // Constructor
  MINERvA_NukeCC0pi_XSec_1D_nu(nuiskey samplekey, measurement_1d measurement,
                                kinematic_1d kinematic);

  // Destructor
  virtual ~MINERvA_NukeCC0pi_XSec_1D_nu() {};

  // Required functions
  bool isSignal(FitEvent *nvect);
  void FillEventVariables(FitEvent *event);

 protected:
  // Set up settings based on distribution
  void _SetupDataSettings(measurement_1d measurement, kinematic_1d kinematic);

  measurement_1d fMeasurement;
  kinematic_1d fKinematic;
  std::string covar_name;
};
class MINERvA_NukeCC0pi_CH_XSec_1D_Dpt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_Dpt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_dpt) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_Dpt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_Dpt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_dpt) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_Dpt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_Dpt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_dpt) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_Dpt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_Dpt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_dpt) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_Dpt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_Dpt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_dpt) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Dpt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Dpt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_dpt) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Dpt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Dpt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_dpt) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Dpt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Dpt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_dpt) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Dpt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Dpt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_dpt) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_DptFine_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_DptFine_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_dpt_fine) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_DptFine_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_DptFine_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_dpt_fine) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_DptFine_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_DptFine_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_dpt_fine) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_DptFine_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_DptFine_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_dpt_fine) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_DptFine_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_DptFine_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_dpt_fine) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_DptFine_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_DptFine_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_dpt_fine) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_DptFine_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_DptFine_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_dpt_fine) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_DptFine_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_DptFine_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_dpt_fine) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_DptFine_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_DptFine_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_dpt_fine) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_Dptx_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_Dptx_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_dptx) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_Dptx_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_Dptx_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_dptx) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_Dptx_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_Dptx_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_dptx) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_Dptx_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_Dptx_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_dptx) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_Dptx_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_Dptx_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_dptx) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Dptx_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Dptx_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_dptx) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Dptx_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Dptx_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_dptx) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Dptx_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Dptx_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_dptx) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Dptx_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Dptx_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_dptx) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_Dpty_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_Dpty_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_dpty) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_Dpty_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_Dpty_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_dpty) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_Dpty_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_Dpty_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_dpty) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_Dpty_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_Dpty_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_dpty) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_Dpty_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_Dpty_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_dpty) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Dpty_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Dpty_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_dpty) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Dpty_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Dpty_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_dpty) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Dpty_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Dpty_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_dpty) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Dpty_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Dpty_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_dpty) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_Pl_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_Pl_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_pl) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_Pl_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_Pl_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_pl) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_Pl_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_Pl_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_pl) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_Pl_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_Pl_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_pl) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_Pl_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_Pl_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_pl) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Pl_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Pl_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_pl) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Pl_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Pl_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_pl) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Pl_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Pl_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_pl) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Pl_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Pl_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_pl) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_Pn_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_Pn_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_pn) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_Pn_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_Pn_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_pn) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_Pn_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_Pn_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_pn) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_Pn_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_Pn_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_pn) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_Pn_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_Pn_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_pn) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Pn_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Pn_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_pn) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Pn_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Pn_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_pn) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Pn_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Pn_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_pn) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Pn_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Pn_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_pn) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_Alpha_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_Alpha_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_alpha) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_Alpha_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_Alpha_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_alpha) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_Alpha_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_Alpha_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_alpha) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_Alpha_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_Alpha_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_alpha) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_Alpha_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_Alpha_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_alpha) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Alpha_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Alpha_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_alpha) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Alpha_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Alpha_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_alpha) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Alpha_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Alpha_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_alpha) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Alpha_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Alpha_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_alpha) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_Phi_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_Phi_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_phi) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_Phi_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_Phi_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_phi) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_Phi_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_Phi_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_phi) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_Phi_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_Phi_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_phi) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_Phi_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_Phi_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_phi) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Phi_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_Phi_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_phi) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Phi_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_Phi_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_phi) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Phi_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_Phi_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_phi) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Phi_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_Phi_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_phi) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_MuonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_MuonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_muon_p) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_MuonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_MuonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_muon_p) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_MuonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_MuonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_muon_p) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_MuonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_MuonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_muon_p) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_MuonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_MuonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_muon_p) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_MuonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_MuonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_muon_p) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_MuonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_MuonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_muon_p) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_MuonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_MuonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_muon_p) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_MuonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_MuonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_muon_p) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_MuonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_MuonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_muon_pt) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_MuonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_MuonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_muon_pt) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_MuonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_MuonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_muon_pt) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_MuonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_MuonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_muon_pt) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_MuonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_MuonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_muon_pt) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_MuonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_MuonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_muon_pt) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_MuonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_MuonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_muon_pt) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_MuonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_MuonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_muon_pt) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_MuonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_MuonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_muon_pt) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_MuonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_MuonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_muon_theta) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_MuonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_MuonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_muon_theta) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_MuonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_MuonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_muon_theta) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_MuonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_MuonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_muon_theta) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_MuonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_MuonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_muon_theta) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_MuonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_MuonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_muon_theta) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_MuonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_MuonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_muon_theta) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_MuonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_MuonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_muon_theta) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_MuonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_MuonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_muon_theta) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_ProtonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_ProtonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_proton_p) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_ProtonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_ProtonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_proton_p) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_ProtonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_ProtonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_proton_p) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_ProtonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_ProtonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_proton_p) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_ProtonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_ProtonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_proton_p) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_ProtonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_ProtonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_proton_p) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_ProtonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_ProtonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_proton_p) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_ProtonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_ProtonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_proton_p) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_ProtonP_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_ProtonP_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_proton_p) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_ProtonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_ProtonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_proton_pt) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_ProtonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_ProtonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_proton_pt) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_ProtonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_ProtonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_proton_pt) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_ProtonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_ProtonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_proton_pt) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_ProtonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_ProtonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_proton_pt) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_ProtonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_ProtonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_proton_pt) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_ProtonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_ProtonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_proton_pt) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_ProtonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_ProtonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_proton_pt) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_ProtonPt_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_ProtonPt_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_proton_pt) {}
};

class MINERvA_NukeCC0pi_CH_XSec_1D_ProtonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_XSec_1D_ProtonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch, kin1d_proton_theta) {}
};

class MINERvA_NukeCC0pi_C_XSec_1D_ProtonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_C_XSec_1D_ProtonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_carbon, kin1d_proton_theta) {}
};

class MINERvA_NukeCC0pi_H2O_XSec_1D_ProtonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_H2O_XSec_1D_ProtonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_h2o, kin1d_proton_theta) {}
};

class MINERvA_NukeCC0pi_Fe_XSec_1D_ProtonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Fe_XSec_1D_ProtonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_fe, kin1d_proton_theta) {}
};

class MINERvA_NukeCC0pi_Pb_XSec_1D_ProtonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_Pb_XSec_1D_ProtonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_pb, kin1d_proton_theta) {}
};

class MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_ProtonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_ProtonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_c_flux, kin1d_proton_theta) {}
};

class MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_ProtonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_ProtonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_h2o_flux, kin1d_proton_theta) {}
};

class MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_ProtonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_ProtonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_fe_flux, kin1d_proton_theta) {}
};

class MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_ProtonTheta_nu : public MINERvA_NukeCC0pi_XSec_1D_nu {
  public:
    MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_ProtonTheta_nu(nuiskey samplekey)
        : MINERvA_NukeCC0pi_XSec_1D_nu(samplekey, target1d_ch_pb_flux, kin1d_proton_theta) {}
};

#endif
