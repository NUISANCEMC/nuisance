#include "NUISANCESyst.h"

int Reweight::ConvertNUISANCEDial(std::string type) {

	for (int i = kUnkownNUISANCEDial + 1; i < kNUISANCEDial_LAST; i++) {
		if (!type.compare(ConvNUISANCEDial(i).c_str())) {
			return i;
		}
	}
	return kUnkownNUISANCEDial;
};

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

	case kModeNorm_NormRES:                   { return "NormRES"; }

	case kMINERvARW_NormCCQE:                { return "MINERvARW_NormCCQE"; }
	case kMINERvARW_NormCCMEC:                { return "MINERvARW_NormCCMEC"; }
	case kMINERvARW_NormCCRES:                { return "MINERvARW_NormCCRES"; }

	case kMINERvARW_RikRPA_ApplyRPA:            { return "MINERvARW_RikRPA_ApplyRPA"; }
    case kMINERvARW_RikRPA_LowQ2:               { return "MINERvARW_RikRPA_LowQ2"; }
    case kMINERvARW_RikRPA_HighQ2:              { return "MINERvARW_RikRPA_HighQ2"; }

    case kSBLOsc_Distance: { return "SBLOsc_Distance"; }
    case kSBLOsc_MassSplitting: { return "SBLOsc_MassSplitting"; }
    case kSBLOsc_Sin2Theta: { return "SBLOsc_Sin2Theta"; }

	default: return "unknown_nuisance_dial";
	}

};
