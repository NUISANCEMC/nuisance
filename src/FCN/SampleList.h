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
// ANL NC1ppim (mm, exotic!)
#include "ANL_NC1ppim_XSec_1DEnu_nu.h"
#include "ANL_NC1ppim_Evt_1DcosmuStar_nu.h"
// ANL CC2pi 1pim1pip (mm, even more exotic!)
#include "ANL_CC2pi_1pim1pip_XSec_1DEnu_nu.h"
#include "ANL_CC2pi_1pim1pip_Evt_1Dpmu_nu.h"
#include "ANL_CC2pi_1pim1pip_Evt_1Dppip_nu.h"
#include "ANL_CC2pi_1pim1pip_Evt_1Dppim_nu.h"
#include "ANL_CC2pi_1pim1pip_Evt_1Dpprot_nu.h"
// ANL CC2pi 1pip1pip (mm, even more exotic!)
#include "ANL_CC2pi_1pip1pip_XSec_1DEnu_nu.h"
#include "ANL_CC2pi_1pip1pip_Evt_1Dpmu_nu.h"
#include "ANL_CC2pi_1pip1pip_Evt_1Dpneut_nu.h"
#include "ANL_CC2pi_1pip1pip_Evt_1DppipHigh_nu.h"
#include "ANL_CC2pi_1pip1pip_Evt_1DppipLow_nu.h"
// ANL CC2pi 1pip1pi0 (mm, even more exotic!)
#include "ANL_CC2pi_1pip1pi0_XSec_1DEnu_nu.h"
#include "ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu.h"
#include "ANL_CC2pi_1pip1pi0_Evt_1Dppip_nu.h"
#include "ANL_CC2pi_1pip1pi0_Evt_1Dppi0_nu.h"
#include "ANL_CC2pi_1pip1pi0_Evt_1Dpprot_nu.h"

// ArgoNeuT CC-inclusive
#include "ArgoNeuT_CCInc_XSec_1Dpmu_antinu.h"
#include "ArgoNeuT_CCInc_XSec_1Dpmu_nu.h"
#include "ArgoNeuT_CCInc_XSec_1Dthetamu_antinu.h"
#include "ArgoNeuT_CCInc_XSec_1Dthetamu_nu.h"

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
#include "MiniBooNE_CCQE_XSec_1DQ2_antinu.h"
#include "MiniBooNE_CCQE_XSec_2DTcos_antinu.h"
#include "MiniBooNE_CCQE_XSec_2DTcos_antinu.h"
#include "MiniBooNE_CCQE_XSec_2DTcos_nu.h"

// MiniBooNE CC1pi+ 1D
#include "MiniBooNE_CC1pip_XSec_1DEnu_nu.h"
#include "MiniBooNE_CC1pip_XSec_1DQ2_nu.h"
#include "MiniBooNE_CC1pip_XSec_1DTpi_nu.h"
#include "MiniBooNE_CC1pip_XSec_1DTu_nu.h"
// MiniBooNE CC1pi+ 2D
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

// MiniBooNE NC1pi0
//#include "MiniBooNE_NCpi0_XSec_1Dppi0_nu.h"
#include "MiniBooNE_NC1pi0_XSec_1Dppi0_nu.h"
#include "MiniBooNE_NC1pi0_XSec_1Dcospi0_nu.h"
#include "MiniBooNE_NC1pi0_XSec_1Dppi0_antinu.h"
#include "MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu.h"

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
#include "MINERvA_CCNpip_XSec_1Dth_nu.h"
#include "MINERvA_CCNpip_XSec_1DTpi_nu.h"
#include "MINERvA_CCNpip_XSec_1Dpmu_nu.h"
#include "MINERvA_CCNpip_XSec_1Dthmu_nu.h"
#include "MINERvA_CCNpip_XSec_1DQ2_nu.h"
#include "MINERvA_CCNpip_XSec_1DEnu_nu.h"

// MINERvA CC1pi0
#include "MINERvA_CC1pi0_XSec_1Dth_antinu.h"
#include "MINERvA_CC1pi0_XSec_1Dppi0_antinu.h"
#include "MINERvA_CC1pi0_XSec_1DTpi0_antinu.h"
#include "MINERvA_CC1pi0_XSec_1Dthmu_antinu.h"
#include "MINERvA_CC1pi0_XSec_1Dpmu_antinu.h"
#include "MINERvA_CC1pi0_XSec_1DQ2_antinu.h"
#include "MINERvA_CC1pi0_XSec_1DEnu_antinu.h"

// MINERvA CCINC
#include "MINERvA_CCinc_XSec_2DEavq3_nu.h"
#include "MINERvA_CCinc_XSec_1Dx_ratio.h"
#include "MINERvA_CCinc_XSec_1DEnu_ratio.h"

// T2K CC0pi
#include "T2K_CC0pi_XSec_2DPcos_nu.h"

// T2K CC1pi+ on CH
#include "T2K_CC1pip_CH_XSec_1Dpmu_nu.h"
#include "T2K_CC1pip_CH_XSec_1Dppi_nu.h"
#include "T2K_CC1pip_CH_XSec_1Dthpi_nu.h"
#include "T2K_CC1pip_CH_XSec_1Dthmupi_nu.h"
#include "T2K_CC1pip_CH_XSec_1DQ2_nu.h"
#include "T2K_CC1pip_CH_XSec_1Dq3_nu.h"
#include "T2K_CC1pip_CH_XSec_1Dthq3pi_nu.h"
#include "T2K_CC1pip_CH_XSec_1DWrec_nu.h"

// T2K CC1pi+ on H2O
#include "T2K_CC1pip_H2O_XSec_1DEnuDelta_nu.h"
#include "T2K_CC1pip_H2O_XSec_1DEnuMB_nu.h"
#include "T2K_CC1pip_H2O_XSec_1Dcosmu_nu.h"
#include "T2K_CC1pip_H2O_XSec_1Dcosmupi_nu.h"
#include "T2K_CC1pip_H2O_XSec_1Dcospi_nu.h"
#include "T2K_CC1pip_H2O_XSec_1Dpmu_nu.h"
#include "T2K_CC1pip_H2O_XSec_1Dppi_nu.h"

// T2K STV CC0pi
#include "T2K_CC0pinp_STV_XSec_1Ddpt_nu.h"

// SciBooNE COH studies
#include "SciBooNE_CCCOH_STOP_NTrks_nu.h"
#include "SciBooNE_CCCOH_1TRK_1DQ2_nu.h"
#include "SciBooNE_CCCOH_MuPr_1DQ2_nu.h"
#include "SciBooNE_CCCOH_MuPiVA_1DQ2_nu.h"
#include "SciBooNE_CCCOH_MuPiNoVA_1DQ2_nu.h"
#include "SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu.h"
#include "SciBooNE_CCCOH_MuPiNoVA_1Dthetapi_nu.h"
#include "SciBooNE_CCCOH_STOPFINAL_1DQ2_nu.h"

// K2K NC1pi0
#include "K2K_NC1pi0_Evt_1Dppi0_nu.h"

// MC Studies
#include "ExpMultDist_CCQE_XSec_1DVar_FakeStudy.h"
#include "ExpMultDist_CCQE_XSec_2DVar_FakeStudy.h"

#include "GenericFlux_Tester.h"

#include "MCStudy_KaonPreSelection.h"
#include "MCStudy_MuonValidation.h"

#include "FitWeight.h"


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
