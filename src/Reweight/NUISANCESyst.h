#ifndef NUISANCESyst_H
#define NUISANCESyst_H
#include "GeneralUtils.h"

namespace Reweight {

  enum NUISANCESyst {

    kUnkownNUISANCEDial = 0,

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

    kSBLOsc_Distance,
    kSBLOsc_MassSplitting,
    kSBLOsc_Sin2Theta,
    
    // SF modif
    
    // Constant RW per shell
    
    kSFRW_pShellNorm_C,
    kSFRW_sShellNorm_C,
    kSFRW_p12ShellNorm_O,
    kSFRW_p32ShellNorm_O,
    kSFRW_sShellNorm_O,
    
    // end Constant RW per shell

    // Gaussian RW per shell

    kGaussian_pShell_C_norm,
    kGaussian_pShell_C_p,
    kGaussian_pShell_C_w,
            
    kGaussian_sShell_C_norm,
    kGaussian_sShell_C_p,
    kGaussian_sShell_C_w,
        
    kGaussian_p12Shell_O_norm,
    kGaussian_p12Shell_O_p,
    kGaussian_p12Shell_O_w,
        
    kGaussian_p32Shell_O_norm,
    kGaussian_p32Shell_O_p,
    kGaussian_p32Shell_O_w,
        
    kGaussian_sShell_O_norm,
    kGaussian_sShell_O_p,
    kGaussian_sShell_O_w,

    kSRC_strength, // RW SRC part

    // end Gaussian RW per shell

    // Pmiss RW

    kPmissRW_pC,
    kPmissRW_sC,
    kPmissRW_p12O,
    kPmissRW_p32O,
    kPmissRW_sO,
    
    // end Pmiss RW


    // end SF modif

    // FSI RW modif
    kFSIRW_noFSI,
    kFSIRW_elasticFSI,
    kFSIRW_inelasticFSI,
    kFSIRW_pionProdFSI,
    kFSIRW_pionAbsFSI,
    // end FSI RW modif
    
    kRW2p2h_norm, // 2p2h normalization
    
    kNUISANCEDial_LAST
  };

  int ConvertNUISANCEDial(std::string type);
  std::string ConvNUISANCEDial(int type);
};
#endif
