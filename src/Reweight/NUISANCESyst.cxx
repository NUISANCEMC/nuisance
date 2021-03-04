#include "NUISANCESyst.h"

int Reweight::ConvertNUISANCEDial(std::string type) {

  for (int i = kUnkownNUISANCEDial + 1; i < kNUISANCEDial_LAST; i++) {
    if (!type.compare(ConvNUISANCEDial(i).c_str())) {
      return i;
    }
  }
  return kUnkownNUISANCEDial;}
;

std::string Reweight::ConvNUISANCEDial(int type) {

  switch (type) {
    case kGaussianCorr_CCQE_norm:             { return "GaussianCorr_CCQE_norm";  }
    case kGaussianCorr_CCQE_tilt:             { return "GaussianCorr_CCQE_tilt";   }
    case kGaussianCorr_CCQE_Pq0:              { return "GaussianCorr_CCQE_Pq0";   }
    case kGaussianCorr_CCQE_Wq0:              { return "GaussianCorr_CCQE_Wq0";   }
    case kGaussianCorr_CCQE_Pq3:              { return "GaussianCorr_CCQE_Pq3";   }
    case kGaussianCorr_CCQE_Wq3:              { return "GaussianCorr_CCQE_Wq3";   }

    case kGaussianCorr_2p2h_norm:             { return "GaussianCorr_2p2h_norm";  }
    case kGaussianCorr_2p2h_tilt:             { return "GaussianCorr_2p2h_tilt";   }
    case kGaussianCorr_2p2h_Pq0:              { return "GaussianCorr_2p2h_Pq0";   }
    case kGaussianCorr_2p2h_Wq0:              { return "GaussianCorr_2p2h_Wq0";   }
    case kGaussianCorr_2p2h_Pq3:              { return "GaussianCorr_2p2h_Pq3";   }
    case kGaussianCorr_2p2h_Wq3:              { return "GaussianCorr_2p2h_Wq3";   }

    case kGaussianCorr_2p2h_PPandNN_norm:     { return "GaussianCorr_2p2h_PPandNN_norm";  }
    case kGaussianCorr_2p2h_PPandNN_tilt:     { return "GaussianCorr_2p2h_PPandNN_tilt";   }
    case kGaussianCorr_2p2h_PPandNN_Pq0:      { return "GaussianCorr_2p2h_PPandNN_Pq0";   }
    case kGaussianCorr_2p2h_PPandNN_Wq0:      { return "GaussianCorr_2p2h_PPandNN_Wq0";   }
    case kGaussianCorr_2p2h_PPandNN_Pq3:      { return "GaussianCorr_2p2h_PPandNN_Pq3";   }
    case kGaussianCorr_2p2h_PPandNN_Wq3:      { return "GaussianCorr_2p2h_PPandNN_Wq3";   }

    case kGaussianCorr_2p2h_NP_norm:          { return "GaussianCorr_2p2h_NP_norm";  }
    case kGaussianCorr_2p2h_NP_tilt:          { return "GaussianCorr_2p2h_NP_tilt";   }
    case kGaussianCorr_2p2h_NP_Pq0:           { return "GaussianCorr_2p2h_NP_Pq0";   }
    case kGaussianCorr_2p2h_NP_Wq0:           { return "GaussianCorr_2p2h_NP_Wq0";   }
    case kGaussianCorr_2p2h_NP_Pq3:           { return "GaussianCorr_2p2h_NP_Pq3";   }
    case kGaussianCorr_2p2h_NP_Wq3:           { return "GaussianCorr_2p2h_NP_Wq3";   }

    case kGaussianCorr_CC1pi_norm:            { return "GaussianCorr_CC1pi_norm";  }
    case kGaussianCorr_CC1pi_tilt:            { return "GaussianCorr_CC1pi_tilt";   }
    case kGaussianCorr_CC1pi_Pq0:             { return "GaussianCorr_CC1pi_Pq0";   }
    case kGaussianCorr_CC1pi_Wq0:             { return "GaussianCorr_CC1pi_Wq0";   }
    case kGaussianCorr_CC1pi_Pq3:             { return "GaussianCorr_CC1pi_Pq3";   }
    case kGaussianCorr_CC1pi_Wq3:             { return "GaussianCorr_CC1pi_Wq3";   }
    case kGaussianCorr_AllowSuppression:      { return "GaussianCorr_AllowSuppression";   }

    case kBeRPA_A:             { return "BeRPA_A";   }
    case kBeRPA_B:             { return "BeRPA_B";   }
    case kBeRPA_D:             { return "BeRPA_D";   }
    case kBeRPA_E:             { return "BeRPA_E";   }
    case kBeRPA_U:             { return "BeRPA_U";   }

    case kModeNorm_NormRES:                   { return "NormRES"; }

    case kMINERvARW_NormCCQE:                { return "MINERvARW_NormCCQE"; }
    case kMINERvARW_NormCCMEC:                { return "MINERvARW_NormCCMEC"; }
    case kMINERvARW_NormCCRES:                { return "MINERvARW_NormCCRES"; }

    case kMINERvARW_RikRPA_ApplyRPA:            { return "MINERvARW_RikRPA_ApplyRPA"; }
    case kMINERvARW_RikRPA_LowQ2:               { return "MINERvARW_RikRPA_LowQ2"; }
    case kMINERvARW_RikRPA_HighQ2:              { return "MINERvARW_RikRPA_HighQ2"; }

    case kMINERvARW_RikRESRPA_ApplyRPA:            { return "MINERvARW_RikRESRPA_ApplyRPA"; }
    case kMINERvARW_RikRESRPA_LowQ2:               { return "MINERvARW_RikRESRPA_LowQ2"; }
    case kMINERvARW_RikRESRPA_HighQ2:              { return "MINERvARW_RikRESRPA_HighQ2"; }
      
    // SF modif 

    // Constant RW per shell  

    case kSFRW_pShellNorm_C:              { return "SFRW_pShellNorm_C"; }
    case kSFRW_sShellNorm_C:              { return "SFRW_sShellNorm_C"; }
    case kSFRW_p12ShellNorm_O:            { return "SFRW_p12ShellNorm_O"; }
    case kSFRW_p32ShellNorm_O:            { return "SFRW_p32ShellNorm_O"; }
    case kSFRW_sShellNorm_O:              { return "SFRW_sShellNorm_O"; }

    // end Constant RW per shell

    // Gaussian RW per shell
        
    case kGaussian_pShell_C_norm:           { return "Gaussian_pShell_C_norm"; }
    case kGaussian_pShell_C_p:              { return "Gaussian_pShell_C_p"; }
    case kGaussian_pShell_C_w:              { return "Gaussian_pShell_C_w"; }
            
    case kGaussian_sShell_C_norm:           { return "Gaussian_sShell_C_norm"; }
    case kGaussian_sShell_C_p:              { return "Gaussian_sShell_C_p"; }
    case kGaussian_sShell_C_w:              { return "Gaussian_sShell_C_w"; }
        
        
    case kGaussian_p12Shell_O_norm:         { return "Gaussian_p12Shell_O_norm"; }
    case kGaussian_p12Shell_O_p:            { return "Gaussian_p12Shell_O_p"; }
    case kGaussian_p12Shell_O_w:            { return "Gaussian_p12Shell_O_w"; }
        
    case kGaussian_p32Shell_O_norm:         { return "Gaussian_p32Shell_O_norm"; }
    case kGaussian_p32Shell_O_p:            { return "Gaussian_p32Shell_O_p"; }
    case kGaussian_p32Shell_O_w:            { return "Gaussian_p32Shell_O_w"; }
        
    case kGaussian_sShell_O_norm:         { return "Gaussian_sShell_O_norm"; }
    case kGaussian_sShell_O_p:            { return "Gaussian_sShell_O_p"; }
    case kGaussian_sShell_O_w:            { return "Gaussian_sShell_O_w"; }

    case kSRC_strength:            { return "SRC_strength"; } // RW SRC part

    case kCCQE_norm: {return "CCQE_norm";} // Overall CCQE normalization
    
      // end Gaussian RW per shell

      // Pmiss RW
    case kPmissRW_pC:              { return "PmissRW_pC"; }
    case kPmissRW_sC:              { return "PmissRW_sC"; }
    
    case kPmissRW_p12O:            { return "PmissRW_p12O"; }
    case kPmissRW_p32O:            { return "PmissRW_p32O"; }
    case kPmissRW_sO:              { return "PmissRW_sO"; }
    // end Pmiss RW


    // end SF modif

    case kPB_q0q3:                   { return "PB_q0q3RW"; }
    
    
    // FSI RW modif

    case kFSIRW_noFSI:             { return "FSIRW_noFSI"; }
    case kFSIRW_elasticFSI:        { return "FSIRW_elasticFSI"; }
    case kFSIRW_inelasticFSI:      { return "FSIRW_inelasticFSI"; }
    case kFSIRW_pionProdFSI:       { return "FSIRW_pionProdFSI"; }
    case kFSIRW_pionAbsFSI:        { return "FSIRW_pionAbsFSI"; }

    // Binary FSI
    case kBinaryFSIRW_noFSI:          { return "BinaryFSIRW_noFSI"; }
    case kBinaryFSIRW_withFSI:        { return "BinaryFSIRW_withFSI"; }
    // end Binary FSI

    case kFSIRWPiAbs: {return "FSIRWPiAbs"; }

   // end FSI RW modif

      // 2p2h normalization
    case kRW2p2h_norm:             { return "RW2p2h_norm"; } 

    case kSBLOsc_Distance: { return "SBLOsc_Distance"; }
    case kSBLOsc_MassSplitting: { return "SBLOsc_MassSplitting"; }
    case kSBLOsc_Sin2Theta: { return "SBLOsc_Sin2Theta"; }

    default: return "unknown_nuisance_dial";
  }

};
