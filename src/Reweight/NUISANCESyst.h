#ifndef NUISANCESyst_H
#define NUISANCESyst_H
#include "GeneralUtils.h"

namespace Reweight {

  enum NUISANCESyst {

    kUnknownNUISANCEDial = 0,

    kGaussianCorr_CCQE_norm,
    kGaussianCorr_CCQE_tilt,
    kGaussianCorr_CCQE_Pq0,
    kGaussianCorr_CCQE_Wq0,
    kGaussianCorr_CCQE_Pq3,
    kGaussianCorr_CCQE_Wq3,

    kGaussianCorr_2p2h_norm,
    kGaussianCorr_2p2h_tilt,
    kGaussianCorr_2p2h_Pq0,
    kGaussianCorr_2p2h_Wq0,
    kGaussianCorr_2p2h_Pq3,
    kGaussianCorr_2p2h_Wq3,

    kGaussianCorr_2p2h_PPandNN_norm,
    kGaussianCorr_2p2h_PPandNN_tilt,
    kGaussianCorr_2p2h_PPandNN_Pq0,
    kGaussianCorr_2p2h_PPandNN_Wq0,
    kGaussianCorr_2p2h_PPandNN_Pq3,
    kGaussianCorr_2p2h_PPandNN_Wq3,

    kGaussianCorr_2p2h_NP_norm,
    kGaussianCorr_2p2h_NP_tilt,
    kGaussianCorr_2p2h_NP_Pq0,
    kGaussianCorr_2p2h_NP_Wq0,
    kGaussianCorr_2p2h_NP_Pq3,
    kGaussianCorr_2p2h_NP_Wq3,

    kGaussianCorr_CC1pi_norm,
    kGaussianCorr_CC1pi_tilt,
    kGaussianCorr_CC1pi_Pq0,
    kGaussianCorr_CC1pi_Wq0,
    kGaussianCorr_CC1pi_Pq3,
    kGaussianCorr_CC1pi_Wq3,

    kGaussianCorr_CCRES_norm,
    kGaussianCorr_CCRES_tilt,
    kGaussianCorr_CCRES_Pq0,
    kGaussianCorr_CCRES_Wq0,
    kGaussianCorr_CCRES_Pq3,
    kGaussianCorr_CCRES_Wq3,

    kGaussianCorr_AllowSuppression,

    kBeRPA_A,
    kBeRPA_B,
    kBeRPA_D,
    kBeRPA_E,
    kBeRPA_U,

    kModeNorm_NormRES,

    kMINERvARW_NormCCQE,
    kMINERvARW_NormCCMEC,
    kMINERvARW_NormCCRES,
    //kMINERvARW_NormCCNonRES1pi,

    kMINERvARW_RikRPA_ApplyRPA,
    kMINERvARW_RikRPA_LowQ2,
    kMINERvARW_RikRPA_HighQ2,

    kMINERvARW_RikRESRPA_ApplyRPA,
    kMINERvARW_RikRESRPA_LowQ2,
    kMINERvARW_RikRESRPA_HighQ2,

    kMINERvARW_MINOSRPA_Apply,
    kMINERvARW_MINOSRPA_A,
    kMINERvARW_MINOSRPA_B,

    kMINERvARW_LagrangeRPA_Apply,
    kMINERvARW_LagrangeRPA_R1,
    kMINERvARW_LagrangeRPA_R2,

    kMINERvARW_NormCOH,
    kMINERvARW_CutCOH,
    kMINERvARW_ApplyCOH,

    kMINERvARW_ApplyWTune,
    kMINERvARW_NormWTune,
    kMINERvARW_MeanWTune,
    kMINERvARW_SigmaWTune,

    kSBLOsc_Distance,
    kSBLOsc_MassSplitting,
    kSBLOsc_Sin2Theta,

    kNUISANCEDial_LAST
  };

  int ConvertNUISANCEDial(std::string type);
  std::string ConvNUISANCEDial(int type);
};
#endif
