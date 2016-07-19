#ifndef _SAMPLE_LIST_H_
#define _SAMPLE_LIST_H_

/*!
 *  \addtogroup FCN
 *  @{
 */

#include "ANL_CCQE_Evt_1DQ2_nu.h"
#include "ANL_CCQE_XSec_1DEnu_nu.h"

// ANL CC1ppip
#include "ANL_CC1ppip_Evt_1DQ2_nu.h"
#include "ANL_CC1ppip_Evt_1DcosmuStar_nu.h"
#include "ANL_CC1ppip_Evt_1DcosmuStar_nu.h"
#include "ANL_CC1ppip_Evt_1DcosthAdler_nu.h"
#include "ANL_CC1ppip_Evt_1Dphi_nu.h"
#include "ANL_CC1ppip_Evt_1Dppi_nu.h"
#include "ANL_CC1ppip_Evt_1Dthpr_nu.h"
#include "ANL_CC1ppip_XSec_1DEnu_nu.h"
#include "ANL_CC1ppip_XSec_1DQ2_nu.h"
// ANL CC1npip
#include "ANL_CC1npip_Evt_1DQ2_nu.h"
#include "ANL_CC1npip_Evt_1DcosmuStar_nu.h"
#include "ANL_CC1npip_Evt_1Dppi_nu.h"
#include "ANL_CC1npip_XSec_1DEnu_nu.h"
// ANL CC1pi0
#include "ANL_CC1pi0_Evt_1DQ2_nu.h"
#include "ANL_CC1pi0_Evt_1DcosmuStar_nu.h"
#include "ANL_CC1pi0_XSec_1DEnu_nu.h"
// ANL NC1npip (mm, exotic!)
#include "ANL_NC1npip_Evt_1Dppi_nu.h"

// BNL CCQE
#include "BNL_CCQE_Evt_1DQ2_nu.h"
#include "BNL_CCQE_XSec_1DEnu_nu.h"
// BNL CC1ppip
#include "BNL_CC1ppip_Evt_1DQ2_nu.h"
#include "BNL_CC1ppip_Evt_1DQ2_nu.h"
#include "BNL_CC1ppip_Evt_1DcosthAdler_nu.h"
#include "BNL_CC1ppip_Evt_1Dphi_nu.h"
#include "BNL_CC1ppip_XSec_1DEnu_nu.h"
// BNL CC1npip
#include "BNL_CC1npip_Evt_1DQ2_nu.h"
#include "BNL_CC1npip_XSec_1DEnu_nu.h"
// BNL CC1pi0
#include "BNL_CC1pi0_Evt_1DQ2_nu.h"
#include "BNL_CC1pi0_XSec_1DEnu_nu.h"

// FNAL CCQE
#include "FNAL_CCQE_Evt_1DQ2_nu.h"
// FNAL CC1ppip
#include "FNAL_CC1ppip_Evt_1DQ2_nu.h"
#include "FNAL_CC1ppip_XSec_1DEnu_nu.h"
#include "FNAL_CC1ppip_XSec_1DQ2_nu.h"
// FNAL CC1ppim
//#include "FNAL_CC1ppim_XSec_1DEnu_antinu.h"

// BEBC CCQE
#include "BEBC_CCQE_XSec_1DQ2_nu.h"
// BEBC CC1ppip
#include "BEBC_CC1ppip_XSec_1DEnu_nu.h"
#include "BEBC_CC1ppip_XSec_1DQ2_nu.h"
// BEBC CC1npip
#include "BEBC_CC1npip_XSec_1DEnu_nu.h"
#include "BEBC_CC1npip_XSec_1DQ2_nu.h"
// BEBC CC1pi0
#include "BEBC_CC1pi0_XSec_1DEnu_nu.h"
#include "BEBC_CC1pi0_XSec_1DQ2_nu.h"
// BEBC CC1npim
#include "BEBC_CC1npim_XSec_1DEnu_antinu.h"
#include "BEBC_CC1npim_XSec_1DQ2_antinu.h"
// BEBC CC1ppim
#include "BEBC_CC1ppim_XSec_1DEnu_antinu.h"
#include "BEBC_CC1ppim_XSec_1DQ2_antinu.h"

// GGM CC1ppip
#include "GGM_CC1ppip_Evt_1DQ2_nu.h"
#include "GGM_CC1ppip_XSec_1DEnu_nu.h"

// MiniBooNE CCQE
#include "MiniBooNE_CCQE_XSec_1DQ2_nu.h"
#include "MiniBooNE_CCQE_XSec_2DTcos_antinu.h"
#include "MiniBooNE_CCQE_XSec_2DTcos_antinu.h"
#include "MiniBooNE_CCQE_XSec_2DTcos_nu.h"

// MiniBooNE CC1pi+
#include "MiniBooNE_CC1pip_XSec_1DEnu_nu.h"
#include "MiniBooNE_CC1pip_XSec_1DQ2_nu.h"
#include "MiniBooNE_CC1pip_XSec_1DTpi_nu.h"
#include "MiniBooNE_CC1pip_XSec_1DTu_nu.h"

#include "MiniBooNE_CC1pip_XSec_2DQ2Enu_nu.h"
#include "MiniBooNE_CC1pip_XSec_2DTpiCospi_nu.h"
#include "MiniBooNE_CC1pip_XSec_2DTpiEnu_nu.h"
#include "MiniBooNE_CC1pip_XSec_2DTuCosmu_nu.h"
#include "MiniBooNE_CC1pip_XSec_2DTuEnu_nu.h"

// MiniBooNE CC1pi0
#include "MiniBooNE_CC1pi0_XSec_1DEnu_nu.h"
#include "MiniBooNE_CC1pi0_XSec_1DQ2_nu.h"
#include "MiniBooNE_CC1pi0_XSec_1DTu_nu.h"
#include "MiniBooNE_CC1pi0_XSec_1Dcosmu_nu.h"
#include "MiniBooNE_CC1pi0_XSec_1Dcospi0_nu.h"
#include "MiniBooNE_CC1pi0_XSec_1Dppi0_nu.h"

//#include "MiniBooNE_NCpi0_XSec_1Dppi0_nu.h"

// MiniBooNE NCEL
// #include "MiniBooNE_NCEL_XSec_Treco_nu.h"

// MINERvA CCQE
#include "MINERvA_CCQE_XSec_1DQ2_antinu.h"
#include "MINERvA_CCQE_XSec_1DQ2_joint.h"
#include "MINERvA_CCQE_XSec_1DQ2_nu.h"
// MINERvA CC0pi
#include "MINERvA_CC0pi_XSec_1DEe_nue.h"
#include "MINERvA_CC0pi_XSec_1DQ2_nu_proton.h"
#include "MINERvA_CC0pi_XSec_1DQ2_nue.h"
#include "MINERvA_CC0pi_XSec_1DThetae_nue.h"
// MINERvA CC1pi+
#include "MINERvA_CC1pip_XSec_1DTpi_20deg_nu.h"
#include "MINERvA_CC1pip_XSec_1DTpi_nu.h"
#include "MINERvA_CC1pip_XSec_1Dth_20deg_nu.h"
#include "MINERvA_CC1pip_XSec_1Dth_nu.h"
// MINERvA CCNpi+
#include "MINERvA_CCNpip_XSec_1DTpi_20deg_nu.h"
#include "MINERvA_CCNpip_XSec_1DTpi_nu.h"
#include "MINERvA_CCNpip_XSec_1Dth_20deg_nu.h"
#include "MINERvA_CCNpip_XSec_1Dth_nu.h"
// MINERvA CC1pi0
#include "MINERvA_CC1pi0_XSec_1Dppi0_antinu.h"
#include "MINERvA_CC1pi0_XSec_1Dth_antinu.h"
// MINERvA CCINC
#include "MINERvA_CCinc_XSec_2DEavq3_nu.h"
#include "MINERvA_CCinc_XSec_1Dx_ratio.h"
#include "MINERvA_CCinc_XSec_1DEnu_ratio.h"

// T2K CC0pi
#include "T2K_CC0pi_XSec_2DPcos_nu.h"

// K2K CC0pi
#include "K2K_CC0pi_XSec_1DDelPhi_nu_Ntrks.h"
#include "K2K_CC0pi_XSec_1DPmu_nu_Ntrks.h"
#include "K2K_CC0pi_XSec_1DQ2_nu_subtrks.h"
#include "K2K_CC0pi_XSec_1DCosThetaMu_nu_subtrks.h"
#include "K2K_CC0pi_XSec_1DThetaMu_nu_Ntrks.h"
// K2K NC1pi0
#include "K2K_NC1pi0_Evt_1Dppi0_nu.h"

#include "ExpMultDist_CCQE_XSec_1DVar_FakeStudy.h"
#include "ExpMultDist_CCQE_XSec_2DVar_FakeStudy.h"

#include "GenericFlux_Tester.h"

#include "FitWeight.h"
#include "parameter_pulls.h"

//! Functions to make it easier for samples to be created and handled.
namespace SampleUtils {

//! Create a given sample given its name, file, type, fakdata(fkdt) file and the
//! current rw engine and push it back into the list fChain.
bool LoadSample(std::list<MeasurementBase*>* fChain, std::string name,
                std::string file, std::string type, std::string fkdt,
                FitWeight* rw);
}

/*! @} */
#endif
