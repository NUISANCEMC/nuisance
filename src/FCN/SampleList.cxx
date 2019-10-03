#include "SampleList.h"

#ifndef __NO_ANL__
#include "ANL_CCQE_Evt_1DQ2_nu.h"
#include "ANL_CCQE_XSec_1DEnu_nu.h"

// ANL CC1ppip
#include "ANL_CC1ppip_Evt_1DQ2_nu.h"
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
#include "ANL_NC1ppim_Evt_1DcosmuStar_nu.h"
#include "ANL_NC1ppim_XSec_1DEnu_nu.h"
// ANL CC2pi 1pim1pip (mm, even more exotic!)
#include "ANL_CC2pi_1pim1pip_Evt_1Dpmu_nu.h"
#include "ANL_CC2pi_1pim1pip_Evt_1Dppim_nu.h"
#include "ANL_CC2pi_1pim1pip_Evt_1Dppip_nu.h"
#include "ANL_CC2pi_1pim1pip_Evt_1Dpprot_nu.h"
#include "ANL_CC2pi_1pim1pip_XSec_1DEnu_nu.h"
// ANL CC2pi 1pip1pip (mm, even more exotic!)
#include "ANL_CC2pi_1pip1pip_Evt_1Dpmu_nu.h"
#include "ANL_CC2pi_1pip1pip_Evt_1Dpneut_nu.h"
#include "ANL_CC2pi_1pip1pip_Evt_1DppipHigh_nu.h"
#include "ANL_CC2pi_1pip1pip_Evt_1DppipLow_nu.h"
#include "ANL_CC2pi_1pip1pip_XSec_1DEnu_nu.h"
// ANL CC2pi 1pip1pi0 (mm, even more exotic!)
#include "ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu.h"
#include "ANL_CC2pi_1pip1pi0_Evt_1Dppi0_nu.h"
#include "ANL_CC2pi_1pip1pi0_Evt_1Dppip_nu.h"
#include "ANL_CC2pi_1pip1pi0_Evt_1Dpprot_nu.h"
#include "ANL_CC2pi_1pip1pi0_XSec_1DEnu_nu.h"
#endif

#ifndef __NO_ArgoNeuT__
// ArgoNeuT CC1Pi
#include "ArgoNeuT_CC1Pi_XSec_1Dpmu_antinu.h"
#include "ArgoNeuT_CC1Pi_XSec_1Dpmu_nu.h"
#include "ArgoNeuT_CC1Pi_XSec_1Dthetamu_antinu.h"
#include "ArgoNeuT_CC1Pi_XSec_1Dthetamu_nu.h"
#include "ArgoNeuT_CC1Pi_XSec_1Dthetamupi_antinu.h"
#include "ArgoNeuT_CC1Pi_XSec_1Dthetamupi_nu.h"
#include "ArgoNeuT_CC1Pi_XSec_1Dthetapi_antinu.h"
#include "ArgoNeuT_CC1Pi_XSec_1Dthetapi_nu.h"
// ArgoNeuT CC-inclusive
#include "ArgoNeuT_CCInc_XSec_1Dpmu_antinu.h"
#include "ArgoNeuT_CCInc_XSec_1Dpmu_nu.h"
#include "ArgoNeuT_CCInc_XSec_1Dthetamu_antinu.h"
#include "ArgoNeuT_CCInc_XSec_1Dthetamu_nu.h"
#endif

#ifndef __NO_BNL__
// BNL CCQE
#include "BNL_CCQE_Evt_1DQ2_nu.h"
#include "BNL_CCQE_XSec_1DEnu_nu.h"
// BNL CC1ppip
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
#endif

#ifndef __NO_FNAL__
// FNAL CCQE
#include "FNAL_CCQE_Evt_1DQ2_nu.h"
// FNAL CC1ppip
#include "FNAL_CC1ppip_Evt_1DQ2_nu.h"
#include "FNAL_CC1ppip_XSec_1DEnu_nu.h"
#include "FNAL_CC1ppip_XSec_1DQ2_nu.h"
// FNAL CC1ppim
#include "FNAL_CC1ppim_XSec_1DEnu_antinu.h"
#endif

#ifndef __NO_BEBC__
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
#endif

#ifndef __NO_GGM__
// GGM CC1ppip
#include "GGM_CC1ppip_Evt_1DQ2_nu.h"
#include "GGM_CC1ppip_XSec_1DEnu_nu.h"
#endif

#ifndef __NO_MiniBooNE__
// MiniBooNE CCQE
#include "MiniBooNE_CCQE_XSec_1DQ2_antinu.h"
#include "MiniBooNE_CCQE_XSec_1DQ2_nu.h"
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
#include "MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu.h"
#include "MiniBooNE_NC1pi0_XSec_1Dcospi0_nu.h"
#include "MiniBooNE_NC1pi0_XSec_1Dppi0_antinu.h"
#include "MiniBooNE_NC1pi0_XSec_1Dppi0_nu.h"

// MiniBooNE NC1pi0
//#include "MiniBooNE_NCpi0_XSec_1Dppi0_nu.h"

// MiniBooNE NCEL
#include "MiniBooNE_NCEL_XSec_Treco_nu.h"
#endif

#ifndef __NO_MicroBooNE__
#include "MicroBooNE_CCInc_XSec_2DPcos_nu.h"
#endif

#ifndef __NO_MINERvA__
// MINERvA CCQE
#include "MINERvA_CCQE_XSec_1DQ2_antinu.h"
#include "MINERvA_CCQE_XSec_1DQ2_joint.h"
#include "MINERvA_CCQE_XSec_1DQ2_nu.h"

// MINERvA CC0pi
#include "MINERvA_CC0pi_XSec_1DEe_nue.h"
#include "MINERvA_CC0pi_XSec_1DQ2_nu_proton.h"
#include "MINERvA_CC0pi_XSec_1DQ2_nue.h"
#include "MINERvA_CC0pi_XSec_1DThetae_nue.h"

// 2018 MINERvA CC0pi STV
#include "MINERvA_CC0pinp_STV_XSec_1D_nu.h"

// 2018 MINERvA CC0pi 2D
#include "MINERvA_CC0pi_XSec_1D_2018_nu.h"
#include "MINERvA_CC0pi_XSec_2D_nu.h"

// 2018 MINERvA CC0pi 2D antinu
#include "MINERvA_CC0pi_XSec_2D_antinu.h"

// MINERvA CC1pi+
#include "MINERvA_CC1pip_XSec_1DTpi_20deg_nu.h"
#include "MINERvA_CC1pip_XSec_1DTpi_nu.h"
#include "MINERvA_CC1pip_XSec_1Dth_20deg_nu.h"
#include "MINERvA_CC1pip_XSec_1Dth_nu.h"
// 2017 data update
#include "MINERvA_CC1pip_XSec_1D_2017Update.h"

// MINERvA CCNpi+
#include "MINERvA_CCNpip_XSec_1DEnu_nu.h"
#include "MINERvA_CCNpip_XSec_1DQ2_nu.h"
#include "MINERvA_CCNpip_XSec_1DTpi_nu.h"
#include "MINERvA_CCNpip_XSec_1Dpmu_nu.h"
#include "MINERvA_CCNpip_XSec_1Dth_nu.h"
#include "MINERvA_CCNpip_XSec_1Dthmu_nu.h"

// MINERvA CC1pi0
#include "MINERvA_CC1pi0_XSec_1DEnu_antinu.h"
#include "MINERvA_CC1pi0_XSec_1DQ2_antinu.h"
#include "MINERvA_CC1pi0_XSec_1DTpi0_antinu.h"
#include "MINERvA_CC1pi0_XSec_1Dpmu_antinu.h"
#include "MINERvA_CC1pi0_XSec_1Dppi0_antinu.h"
#include "MINERvA_CC1pi0_XSec_1Dth_antinu.h"
#include "MINERvA_CC1pi0_XSec_1Dthmu_antinu.h"

// MINERvA CC1pi0 neutrino
#include "MINERvA_CC1pi0_XSec_1D_nu.h"

// MINERvA CCINC
#include "MINERvA_CCinc_XSec_1DEnu_ratio.h"
#include "MINERvA_CCinc_XSec_1Dx_ratio.h"
#include "MINERvA_CCinc_XSec_2DEavq3_nu.h"

// MINERvA CCDIS
#include "MINERvA_CCDIS_XSec_1DEnu_ratio.h"
#include "MINERvA_CCDIS_XSec_1Dx_ratio.h"

// MINERvA CCCOH pion
#include "MINERvA_CCCOHPI_XSec_1DEnu_antinu.h"
#include "MINERvA_CCCOHPI_XSec_1DEpi_antinu.h"
#include "MINERvA_CCCOHPI_XSec_1DQ2_antinu.h"

#include "MINERvA_CCCOHPI_XSec_1DEpi_nu.h"
#include "MINERvA_CCCOHPI_XSec_1DQ2_nu.h"
#include "MINERvA_CCCOHPI_XSec_1Dth_nu.h"

#include "MINERvA_CCCOHPI_XSec_joint.h"

#include "MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu.h"
#include "MINERvA_CC0pi_XSec_1DQ2_Tgt_nu.h"
#endif

#ifndef __NO_T2K__
// T2K CC0pi 2016
#include "T2K_CC0pi_XSec_2DPcos_nu_I.h"
#include "T2K_CC0pi_XSec_2DPcos_nu_II.h"

// T2K CC-inclusive with full acceptance 2018
#include "T2K_CCinc_XSec_2DPcos_nu_nonuniform.h"

// T2K STV CC0pi 2018
#include "T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform.h"
#include "T2K_CC0pinp_STV_XSec_1Ddat_nu.h"
#include "T2K_CC0pinp_STV_XSec_1Ddphit_nu.h"
#include "T2K_CC0pinp_STV_XSec_1Ddpt_nu.h"
#include "T2K_CC0pinp_ifk_XSec_3Dinfa_nu.h"
#include "T2K_CC0pinp_ifk_XSec_3Dinfip_nu.h"
#include "T2K_CC0pinp_ifk_XSec_3Dinfp_nu.h"

// T2K CC1pi+ on CH
#include "T2K_CC1pip_CH_XSec_1DAdlerPhi_nu.h"
#include "T2K_CC1pip_CH_XSec_1DCosThAdler_nu.h"
#include "T2K_CC1pip_CH_XSec_1DQ2_nu.h"
#include "T2K_CC1pip_CH_XSec_1Dppi_nu.h"
#include "T2K_CC1pip_CH_XSec_1Dthmupi_nu.h"
#include "T2K_CC1pip_CH_XSec_1Dthpi_nu.h"
#include "T2K_CC1pip_CH_XSec_2Dpmucosmu_nu.h"
//#include "T2K_CC1pip_CH_XSec_1Dthq3pi_nu.h"
//#include "T2K_CC1pip_CH_XSec_1DWrec_nu.h"
//#include "T2K_CC1pip_CH_XSec_1Dq3_nu.h"

// T2K CC1pi+ on H2O
#include "T2K_CC1pip_H2O_XSec_1DEnuDelta_nu.h"
#include "T2K_CC1pip_H2O_XSec_1DEnuMB_nu.h"
#include "T2K_CC1pip_H2O_XSec_1Dcosmu_nu.h"
#include "T2K_CC1pip_H2O_XSec_1Dcosmupi_nu.h"
#include "T2K_CC1pip_H2O_XSec_1Dcospi_nu.h"
#include "T2K_CC1pip_H2O_XSec_1Dpmu_nu.h"
#include "T2K_CC1pip_H2O_XSec_1Dppi_nu.h"

#endif

#ifndef __NO_SciBooNE__

// SciBooNE COH studies
#include "SciBooNE_CCCOH_1TRK_1DQ2_nu.h"
#include "SciBooNE_CCCOH_1TRK_1Dpmu_nu.h"
#include "SciBooNE_CCCOH_1TRK_1Dthetamu_nu.h"
#include "SciBooNE_CCCOH_MuPiNoVA_1DQ2_nu.h"
#include "SciBooNE_CCCOH_MuPiNoVA_1Dpmu_nu.h"
#include "SciBooNE_CCCOH_MuPiNoVA_1Dthetamu_nu.h"
#include "SciBooNE_CCCOH_MuPiNoVA_1Dthetapi_nu.h"
#include "SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu.h"
#include "SciBooNE_CCCOH_MuPiVA_1DQ2_nu.h"
#include "SciBooNE_CCCOH_MuPiVA_1Dpmu_nu.h"
#include "SciBooNE_CCCOH_MuPiVA_1Dthetamu_nu.h"
#include "SciBooNE_CCCOH_MuPr_1DQ2_nu.h"
#include "SciBooNE_CCCOH_MuPr_1Dpmu_nu.h"
#include "SciBooNE_CCCOH_MuPr_1Dthetamu_nu.h"
#include "SciBooNE_CCCOH_STOPFINAL_1DQ2_nu.h"
#include "SciBooNE_CCCOH_STOP_NTrks_nu.h"
#endif

#ifndef __NO_K2K__
// K2K NC1pi0
#include "K2K_NC1pi0_Evt_1Dppi0_nu.h"
#endif

// MC Studies
#include "ExpMultDist_CCQE_XSec_1DVar_FakeStudy.h"
#include "ExpMultDist_CCQE_XSec_2DVar_FakeStudy.h"
#include "MCStudy_CCQEHistograms.h"

#include "GenericFlux_Tester.h"
#include "GenericFlux_Vectors.h"

#include "ElectronFlux_FlatTree.h"
#include "ElectronScattering_DurhamData.h"
#include "MCStudy_KaonPreSelection.h"
#include "MCStudy_MuonValidation.h"

#include "OfficialNIWGPlots.h"
#include "T2K2017_FakeData.h"

#include "Simple_Osc.h"
#include "Smear_SVDUnfold_Propagation_Osc.h"

#include "FitWeight.h"

#include "NuisConfig.h"
#include "NuisKey.h"

#ifdef __USE_DYNSAMPLES__

#include "TRegexp.h"

#include <dirent.h>

// linux
#include <dlfcn.h>

DynamicSampleFactory::DynamicSampleFactory() : NSamples(0), NManifests(0) {
  LoadPlugins();
  NUIS_LOG(FIT, "Loaded " << NSamples << " from " << NManifests
                          << " shared object libraries.");
}
DynamicSampleFactory *DynamicSampleFactory::glblDSF = NULL;
DynamicSampleFactory::PluginManifest::~PluginManifest() {
  for (size_t i_it = 0; i_it < Instances.size(); ++i_it) {
    (*(DSF_DestroySample))(Instances[i_it]);
  }
}
std::string EnsureTrailingSlash(std::string const &inp) {
  if (!inp.length()) {
    return "/";
  }
  if (inp[inp.length() - 1] == '/') {
    return inp;
  }
  return inp + "/";
}
void DynamicSampleFactory::LoadPlugins() {
  std::vector<std::string> SearchDirectories;

  if (Config::HasPar("dynamic_sample.path")) {
    SearchDirectories =
        GeneralUtils::ParseToStr(Config::GetParS("dynamic_sample.path"), ":");
  }

  char const *envPath = getenv("NUISANCE_DS_PATH");
  if (envPath) {
    std::vector<std::string> envPaths = GeneralUtils::ParseToStr(envPath, ":");
    for (size_t ep_it = 0; ep_it < envPaths.size(); ++ep_it) {
      SearchDirectories.push_back(envPaths[ep_it]);
    }
  }

  if (!SearchDirectories.size()) {
    char const *pwdPath = getenv("PWD");
    if (pwdPath) {
      SearchDirectories.push_back(pwdPath);
    }
  }

  for (size_t sp_it = 0; sp_it < SearchDirectories.size(); ++sp_it) {
    std::string dirpath = EnsureTrailingSlash(SearchDirectories[sp_it]);

    NUIS_LOG(FIT, "Searching for dynamic sample manifests in: " << dirpath);

    Ssiz_t len = 0;
    DIR *dir;
    struct dirent *ent;
    dir = opendir(dirpath.c_str());
    if (dir != NULL) {
      TRegexp matchExp("*.so", true);
      while ((ent = readdir(dir)) != NULL) {
        if (matchExp.Index(TString(ent->d_name), &len) != Ssiz_t(-1)) {
          NUIS_LOG(FIT, "\tFound shared object: "
                            << ent->d_name
                            << " checking for relevant methods...");

          void *dlobj =
              dlopen((dirpath + ent->d_name).c_str(), RTLD_NOW | RTLD_GLOBAL);
          char const *dlerr_cstr = dlerror();
          std::string dlerr;
          if (dlerr_cstr) {
            dlerr = dlerr_cstr;
          }

          if (dlerr.length()) {
            NUIS_ERR(WRN, "\tDL Load Error: " << dlerr);
            continue;
          }

          PluginManifest plgManif;
          plgManif.dllib = dlobj;
          plgManif.soloc = (dirpath + ent->d_name);

          plgManif.DSF_NSamples =
              reinterpret_cast<DSF_NSamples_ptr>(dlsym(dlobj, "DSF_NSamples"));

          dlerr = "";
          dlerr_cstr = dlerror();
          if (dlerr_cstr) {
            dlerr = dlerr_cstr;
          }

          if (dlerr.length()) {
            NUIS_ERR(WRN, "\tFailed to load symbol \"DSF_NSamples\" from "
                              << (dirpath + ent->d_name) << ": " << dlerr);
            dlclose(dlobj);
            continue;
          }

          plgManif.DSF_GetSampleName = reinterpret_cast<DSF_GetSampleName_ptr>(
              dlsym(dlobj, "DSF_GetSampleName"));

          dlerr = "";
          dlerr_cstr = dlerror();
          if (dlerr_cstr) {
            dlerr = dlerr_cstr;
          }

          if (dlerr.length()) {
            NUIS_ERR(WRN, "\tFailed to load symbol \"DSF_GetSampleName\" from "
                              << (dirpath + ent->d_name) << ": " << dlerr);
            dlclose(dlobj);
            continue;
          }

          plgManif.DSF_GetSample = reinterpret_cast<DSF_GetSample_ptr>(
              dlsym(dlobj, "DSF_GetSample"));

          dlerr = "";
          dlerr_cstr = dlerror();
          if (dlerr_cstr) {
            dlerr = dlerr_cstr;
          }

          if (dlerr.length()) {
            NUIS_ERR(WRN, "\tFailed to load symbol \"DSF_GetSample\" from "
                              << (dirpath + ent->d_name) << ": " << dlerr);
            dlclose(dlobj);
            continue;
          }

          plgManif.DSF_DestroySample = reinterpret_cast<DSF_DestroySample_ptr>(
              dlsym(dlobj, "DSF_DestroySample"));

          dlerr = "";
          dlerr_cstr = dlerror();
          if (dlerr_cstr) {
            dlerr = dlerr_cstr;
          }

          if (dlerr.length()) {
            NUIS_ERR(WRN, "Failed to load symbol \"DSF_DestroySample\" from "
                              << (dirpath + ent->d_name) << ": " << dlerr);
            dlclose(dlobj);
            continue;
          }

          plgManif.NSamples = (*(plgManif.DSF_NSamples))();
          NUIS_LOG(FIT, "\tSuccessfully loaded dynamic sample manifest: "
                            << plgManif.soloc << ". Contains "
                            << plgManif.NSamples << " samples.");

          for (size_t smp_it = 0; smp_it < plgManif.NSamples; ++smp_it) {
            char const *smp_name = (*(plgManif.DSF_GetSampleName))(smp_it);
            if (!smp_name) {
              NUIS_ABORT("Could not load sample "
                         << smp_it << " / " << plgManif.NSamples << " from "
                         << plgManif.soloc);
            }

            if (Samples.count(smp_name)) {
              NUIS_ERR(WRN, "Already loaded a sample named: \""
                                << smp_name
                                << "\". cannot load duplciates. This "
                                   "sample will be skipped.");
              continue;
            }

            plgManif.SamplesProvided.push_back(smp_name);
            Samples[smp_name] = std::make_pair(plgManif.soloc, smp_it);
            NUIS_LOG(FIT, "\t\t" << smp_name);
          }

          if (plgManif.SamplesProvided.size()) {
            Manifests[plgManif.soloc] = plgManif;

            NSamples += plgManif.SamplesProvided.size();
            NManifests++;
          } else {
            dlclose(dlobj);
          }
        }
      }
      closedir(dir);
    } else {
      NUIS_ERR(WRN, "Tried to open non-existant directory.");
    }
  }
}
DynamicSampleFactory &DynamicSampleFactory::Get() {
  if (!glblDSF) {
    glblDSF = new DynamicSampleFactory();
  }
  return *glblDSF;
}
void DynamicSampleFactory::Print() {
  std::map<std::string, std::vector<std::string> > ManifestSamples;

  for (std::map<std::string, std::pair<std::string, int> >::iterator smp_it =
           Samples.begin();
       smp_it != Samples.end(); ++smp_it) {
    if (!ManifestSamples.count(smp_it->second.first)) {
      ManifestSamples[smp_it->second.first] = std::vector<std::string>();
    }
    ManifestSamples[smp_it->second.first].push_back(smp_it->first);
  }

  NUIS_LOG(FIT, "Dynamic sample manifest: ");
  for (std::map<std::string, std::vector<std::string> >::iterator m_it =
           ManifestSamples.begin();
       m_it != ManifestSamples.end(); ++m_it) {
    NUIS_LOG(FIT, "\tLibrary " << m_it->first << " contains: ");
    for (size_t s_it = 0; s_it < m_it->second.size(); ++s_it) {
      NUIS_LOG(FIT, "\t\t" << m_it->second[s_it]);
    }
  }
}
bool DynamicSampleFactory::HasSample(std::string const &name) {
  return Samples.count(name);
}
bool DynamicSampleFactory::HasSample(nuiskey &samplekey) {
  return HasSample(samplekey.GetS("name"));
}
MeasurementBase *DynamicSampleFactory::CreateSample(nuiskey &samplekey) {
  if (!HasSample(samplekey)) {
    NUIS_ERR(WRN, "Asked to load unknown sample: \"" << samplekey.GetS("name")
                                                     << "\".");
    return NULL;
  }

  std::pair<std::string, int> sample = Samples[samplekey.GetS("name")];
  NUIS_LOG(SAM,
           "\tLoading sample " << sample.second << " from " << sample.first);

  return (*(Manifests[sample.first].DSF_GetSample))(sample.second, &samplekey);
}

DynamicSampleFactory::~DynamicSampleFactory() { Manifests.clear(); }

#endif

//! Functions to make it easier for samples to be created and handled.
namespace SampleUtils {

//! Create a given sample given its name, file, type, fakdata(fkdt) file and the
//! current rw engine and push it back into the list fChain.
MeasurementBase *CreateSample(std::string name, std::string file,
                              std::string type, std::string fkdt,
                              FitWeight *rw) {
  nuiskey samplekey = Config::CreateKey("sample");
  samplekey.Set("name", name);
  samplekey.Set("input", file);
  samplekey.Set("type", type);

  return CreateSample(samplekey);
}

MeasurementBase *CreateSample(nuiskey samplekey) {
#ifdef __USE_DYNSAMPLES__
  if (DynamicSampleFactory::Get().HasSample(samplekey)) {
    NUIS_LOG(SAM, "Instantiating dynamic sample...");

    MeasurementBase *ds = DynamicSampleFactory::Get().CreateSample(samplekey);
    if (ds) {
      NUIS_LOG(SAM, "Done.");
      return ds;
    }
    NUIS_ABORT("Failed to instantiate dynamic sample.");
  }
#endif

  FitWeight *rw = FitBase::GetRW();
  std::string name = samplekey.GetS("name");
  std::string file = samplekey.GetS("input");
  std::string type = samplekey.GetS("type");
  std::string fkdt = "";

  /*
     ANL CCQE Samples
  */

#ifndef __NO_ANL__
  if (!name.compare("ANL_CCQE_XSec_1DEnu_nu") ||
      !name.compare("ANL_CCQE_XSec_1DEnu_nu_PRD26") ||
      !name.compare("ANL_CCQE_XSec_1DEnu_nu_PRL31") ||
      !name.compare("ANL_CCQE_XSec_1DEnu_nu_PRD16")) {
    return (new ANL_CCQE_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("ANL_CCQE_Evt_1DQ2_nu") ||
             !name.compare("ANL_CCQE_Evt_1DQ2_nu_PRL31") ||
             !name.compare("ANL_CCQE_Evt_1DQ2_nu_PRD26") ||
             !name.compare("ANL_CCQE_Evt_1DQ2_nu_PRD16")) {
    return (new ANL_CCQE_Evt_1DQ2_nu(samplekey));
    /*
      ANL CC1ppip samples
    */
  } else if (!name.compare("ANL_CC1ppip_XSec_1DEnu_nu") ||
             !name.compare("ANL_CC1ppip_XSec_1DEnu_nu_W14Cut") ||
             !name.compare("ANL_CC1ppip_XSec_1DEnu_nu_Uncorr") ||
             !name.compare("ANL_CC1ppip_XSec_1DEnu_nu_W14Cut_Uncorr") ||
             !name.compare("ANL_CC1ppip_XSec_1DEnu_nu_W16Cut_Uncorr")) {
    return (new ANL_CC1ppip_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("ANL_CC1ppip_XSec_1DQ2_nu")) {
    return (new ANL_CC1ppip_XSec_1DQ2_nu(samplekey));
  } else if (!name.compare("ANL_CC1ppip_Evt_1DQ2_nu") ||
             !name.compare("ANL_CC1ppip_Evt_1DQ2_nu_W14Cut")) {
    return (new ANL_CC1ppip_Evt_1DQ2_nu(samplekey));
  } else if (!name.compare("ANL_CC1ppip_Evt_1Dppi_nu")) {
    return (new ANL_CC1ppip_Evt_1Dppi_nu(samplekey));
  } else if (!name.compare("ANL_CC1ppip_Evt_1Dthpr_nu")) {
    return (new ANL_CC1ppip_Evt_1Dthpr_nu(samplekey));
  } else if (!name.compare("ANL_CC1ppip_Evt_1DcosmuStar_nu")) {
    return (new ANL_CC1ppip_Evt_1DcosmuStar_nu(samplekey));
  } else if (!name.compare("ANL_CC1ppip_Evt_1DcosthAdler_nu")) {
    return (new ANL_CC1ppip_Evt_1DcosthAdler_nu(samplekey));
  } else if (!name.compare("ANL_CC1ppip_Evt_1Dphi_nu")) {
    return (new ANL_CC1ppip_Evt_1Dphi_nu(samplekey));
    /*
      ANL CC1npip sample
    */
  } else if (!name.compare("ANL_CC1npip_XSec_1DEnu_nu") ||
             !name.compare("ANL_CC1npip_XSec_1DEnu_nu_W14Cut") ||
             !name.compare("ANL_CC1npip_XSec_1DEnu_nu_Uncorr") ||
             !name.compare("ANL_CC1npip_XSec_1DEnu_nu_W14Cut_Uncorr") ||
             !name.compare("ANL_CC1npip_XSec_1DEnu_nu_W16Cut_Uncorr")) {
    return (new ANL_CC1npip_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("ANL_CC1npip_Evt_1DQ2_nu") ||
             !name.compare("ANL_CC1npip_Evt_1DQ2_nu_W14Cut")) {
    return (new ANL_CC1npip_Evt_1DQ2_nu(samplekey));
  } else if (!name.compare("ANL_CC1npip_Evt_1Dppi_nu")) {
    return (new ANL_CC1npip_Evt_1Dppi_nu(samplekey));
  } else if (!name.compare("ANL_CC1npip_Evt_1DcosmuStar_nu")) {
    return (new ANL_CC1npip_Evt_1DcosmuStar_nu(samplekey));
    /*
      ANL CC1pi0 sample
    */
  } else if (!name.compare("ANL_CC1pi0_XSec_1DEnu_nu") ||
             !name.compare("ANL_CC1pi0_XSec_1DEnu_nu_W14Cut") ||
             !name.compare("ANL_CC1pi0_XSec_1DEnu_nu_Uncorr") ||
             !name.compare("ANL_CC1pi0_XSec_1DEnu_nu_W14Cut_Uncorr") ||
             !name.compare("ANL_CC1pi0_XSec_1DEnu_nu_W16Cut_Uncorr")) {
    return (new ANL_CC1pi0_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("ANL_CC1pi0_Evt_1DQ2_nu") ||
             !name.compare("ANL_CC1pi0_Evt_1DQ2_nu_W14Cut")) {
    return (new ANL_CC1pi0_Evt_1DQ2_nu(samplekey));
  } else if (!name.compare("ANL_CC1pi0_Evt_1DcosmuStar_nu")) {
    return (new ANL_CC1pi0_Evt_1DcosmuStar_nu(samplekey));
    /*
      ANL NC1npip sample
    */
  } else if (!name.compare("ANL_NC1npip_Evt_1Dppi_nu")) {
    return (new ANL_NC1npip_Evt_1Dppi_nu(samplekey));
    /*
      ANL NC1ppim sample
    */
  } else if (!name.compare("ANL_NC1ppim_XSec_1DEnu_nu")) {
    return (new ANL_NC1ppim_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("ANL_NC1ppim_Evt_1DcosmuStar_nu")) {
    return (new ANL_NC1ppim_Evt_1DcosmuStar_nu(samplekey));
    /*
      ANL CC2pi sample
    */
  } else if (!name.compare("ANL_CC2pi_1pim1pip_XSec_1DEnu_nu")) {
    return (new ANL_CC2pi_1pim1pip_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pim1pip_Evt_1Dpmu_nu")) {
    return (new ANL_CC2pi_1pim1pip_Evt_1Dpmu_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pim1pip_Evt_1Dppip_nu")) {
    return (new ANL_CC2pi_1pim1pip_Evt_1Dppip_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pim1pip_Evt_1Dppim_nu")) {
    return (new ANL_CC2pi_1pim1pip_Evt_1Dppim_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pim1pip_Evt_1Dpprot_nu")) {
    return (new ANL_CC2pi_1pim1pip_Evt_1Dpprot_nu(samplekey));

  } else if (!name.compare("ANL_CC2pi_1pip1pip_XSec_1DEnu_nu")) {
    return (new ANL_CC2pi_1pip1pip_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pip1pip_Evt_1Dpmu_nu")) {
    return (new ANL_CC2pi_1pip1pip_Evt_1Dpmu_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pip1pip_Evt_1Dpneut_nu")) {
    return (new ANL_CC2pi_1pip1pip_Evt_1Dpneut_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pip1pip_Evt_1DppipHigh_nu")) {
    return (new ANL_CC2pi_1pip1pip_Evt_1DppipHigh_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pip1pip_Evt_1DppipLow_nu")) {
    return (new ANL_CC2pi_1pip1pip_Evt_1DppipLow_nu(samplekey));

  } else if (!name.compare("ANL_CC2pi_1pip1pi0_XSec_1DEnu_nu")) {
    return (new ANL_CC2pi_1pip1pi0_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu")) {
    return (new ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pip1pi0_Evt_1Dppip_nu")) {
    return (new ANL_CC2pi_1pip1pi0_Evt_1Dppip_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pip1pi0_Evt_1Dppi0_nu")) {
    return (new ANL_CC2pi_1pip1pi0_Evt_1Dppi0_nu(samplekey));
  } else if (!name.compare("ANL_CC2pi_1pip1pi0_Evt_1Dpprot_nu")) {
    return (new ANL_CC2pi_1pip1pi0_Evt_1Dpprot_nu(samplekey));

    /*
      ArgoNeut Samples
    */
  } else
#endif
#ifndef __NO_ArgoNeuT__
      if (!name.compare("ArgoNeuT_CCInc_XSec_1Dpmu_antinu")) {
    return (new ArgoNeuT_CCInc_XSec_1Dpmu_antinu(samplekey));
  } else if (!name.compare("ArgoNeuT_CCInc_XSec_1Dpmu_nu")) {
    return (new ArgoNeuT_CCInc_XSec_1Dpmu_nu(samplekey));
  } else if (!name.compare("ArgoNeuT_CCInc_XSec_1Dthetamu_antinu")) {
    return (new ArgoNeuT_CCInc_XSec_1Dthetamu_antinu(samplekey));
  } else if (!name.compare("ArgoNeuT_CCInc_XSec_1Dthetamu_nu")) {
    return (new ArgoNeuT_CCInc_XSec_1Dthetamu_nu(samplekey));
  } else if (!name.compare("ArgoNeuT_CC1Pi_XSec_1Dpmu_nu")) {
    return (new ArgoNeuT_CC1Pi_XSec_1Dpmu_nu(samplekey));
  } else if (!name.compare("ArgoNeuT_CC1Pi_XSec_1Dthetamu_nu")) {
    return (new ArgoNeuT_CC1Pi_XSec_1Dthetamu_nu(samplekey));
  } else if (!name.compare("ArgoNeuT_CC1Pi_XSec_1Dthetapi_nu")) {
    return (new ArgoNeuT_CC1Pi_XSec_1Dthetapi_nu(samplekey));
  } else if (!name.compare("ArgoNeuT_CC1Pi_XSec_1Dthetamupi_nu")) {
    return (new ArgoNeuT_CC1Pi_XSec_1Dthetamupi_nu(samplekey));
  } else if (!name.compare("ArgoNeuT_CC1Pi_XSec_1Dpmu_antinu")) {
    return (new ArgoNeuT_CC1Pi_XSec_1Dpmu_antinu(samplekey));
  } else if (!name.compare("ArgoNeuT_CC1Pi_XSec_1Dthetamu_antinu")) {
    return (new ArgoNeuT_CC1Pi_XSec_1Dthetamu_antinu(samplekey));
  } else if (!name.compare("ArgoNeuT_CC1Pi_XSec_1Dthetapi_antinu")) {
    return (new ArgoNeuT_CC1Pi_XSec_1Dthetapi_antinu(samplekey));
  } else if (!name.compare("ArgoNeuT_CC1Pi_XSec_1Dthetamupi_antinu")) {
    return (new ArgoNeuT_CC1Pi_XSec_1Dthetamupi_antinu(samplekey));

    /*
      BNL Samples
    */
  } else
#endif
#ifndef __NO_BNL__
      if (!name.compare("BNL_CCQE_XSec_1DEnu_nu")) {
    return (new BNL_CCQE_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("BNL_CCQE_Evt_1DQ2_nu")) {
    return (new BNL_CCQE_Evt_1DQ2_nu(samplekey));

    /*
      BNL CC1ppip samples
    */
  } else if (!name.compare("BNL_CC1ppip_XSec_1DEnu_nu") ||
             !name.compare("BNL_CC1ppip_XSec_1DEnu_nu_Uncorr") ||
             !name.compare("BNL_CC1ppip_XSec_1DEnu_nu_W14Cut") ||
             !name.compare("BNL_CC1ppip_XSec_1DEnu_nu_W14Cut_Uncorr")) {
    return (new BNL_CC1ppip_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("BNL_CC1ppip_Evt_1DQ2_nu") ||
             !name.compare("BNL_CC1ppip_Evt_1DQ2_nu_W14Cut")) {
    return (new BNL_CC1ppip_Evt_1DQ2_nu(samplekey));
  } else if (!name.compare("BNL_CC1ppip_Evt_1DcosthAdler_nu")) {
    return (new BNL_CC1ppip_Evt_1DcosthAdler_nu(samplekey));
  } else if (!name.compare("BNL_CC1ppip_Evt_1Dphi_nu")) {
    return (new BNL_CC1ppip_Evt_1Dphi_nu(samplekey));

    /*
      BNL CC1npip samples
    */
  } else if (!name.compare("BNL_CC1npip_XSec_1DEnu_nu") ||
             !name.compare("BNL_CC1npip_XSec_1DEnu_nu_Uncorr")) {
    return (new BNL_CC1npip_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("BNL_CC1npip_Evt_1DQ2_nu")) {
    return (new BNL_CC1npip_Evt_1DQ2_nu(samplekey));
    /*
      BNL CC1pi0 samples
    */
  } else if (!name.compare("BNL_CC1pi0_XSec_1DEnu_nu")) {
    return (new BNL_CC1pi0_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("BNL_CC1pi0_Evt_1DQ2_nu")) {
    return (new BNL_CC1pi0_Evt_1DQ2_nu(samplekey));

    /*
      FNAL Samples
    */
  } else
#endif
#ifndef __NO_FNAL__
      if (!name.compare("FNAL_CCQE_Evt_1DQ2_nu")) {
    return (new FNAL_CCQE_Evt_1DQ2_nu(samplekey));
    /*
      FNAL CC1ppip
    */
  } else if (!name.compare("FNAL_CC1ppip_XSec_1DEnu_nu")) {
    return (new FNAL_CC1ppip_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("FNAL_CC1ppip_XSec_1DQ2_nu")) {
    return (new FNAL_CC1ppip_XSec_1DQ2_nu(samplekey));
  } else if (!name.compare("FNAL_CC1ppip_Evt_1DQ2_nu")) {
    return (new FNAL_CC1ppip_Evt_1DQ2_nu(samplekey));
    /*
      FNAL CC1ppim
    */
  } else if (!name.compare("FNAL_CC1ppim_XSec_1DEnu_antinu")) {
    return (new FNAL_CC1ppim_XSec_1DEnu_antinu(samplekey));

    /*
      BEBC Samples
    */
  } else
#endif
#ifndef __NO_BEBC__
      if (!name.compare("BEBC_CCQE_XSec_1DQ2_nu")) {
    return (new BEBC_CCQE_XSec_1DQ2_nu(samplekey));
    /*
      BEBC CC1ppip samples
    */
  } else if (!name.compare("BEBC_CC1ppip_XSec_1DEnu_nu")) {
    return (new BEBC_CC1ppip_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("BEBC_CC1ppip_XSec_1DQ2_nu")) {
    return (new BEBC_CC1ppip_XSec_1DQ2_nu(samplekey));
    /*
      BEBC CC1npip samples
    */
  } else if (!name.compare("BEBC_CC1npip_XSec_1DEnu_nu")) {
    return (new BEBC_CC1npip_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("BEBC_CC1npip_XSec_1DQ2_nu")) {
    return (new BEBC_CC1npip_XSec_1DQ2_nu(samplekey));
    /*
      BEBC CC1pi0 samples
    */
  } else if (!name.compare("BEBC_CC1pi0_XSec_1DEnu_nu")) {
    return (new BEBC_CC1pi0_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("BEBC_CC1pi0_XSec_1DQ2_nu")) {
    return (new BEBC_CC1pi0_XSec_1DQ2_nu(samplekey));
    /*
      BEBC CC1npim samples
    */
  } else if (!name.compare("BEBC_CC1npim_XSec_1DEnu_antinu")) {
    return (new BEBC_CC1npim_XSec_1DEnu_antinu(samplekey));
  } else if (!name.compare("BEBC_CC1npim_XSec_1DQ2_antinu")) {
    return (new BEBC_CC1npim_XSec_1DQ2_antinu(samplekey));
    /*
      BEBC CC1ppim samples
    */
  } else if (!name.compare("BEBC_CC1ppim_XSec_1DEnu_antinu")) {
    return (new BEBC_CC1ppim_XSec_1DEnu_antinu(samplekey));
  } else if (!name.compare("BEBC_CC1ppim_XSec_1DQ2_antinu")) {
    return (new BEBC_CC1ppim_XSec_1DQ2_antinu(samplekey));

    /*
      GGM CC1ppip samples
    */
  } else
#endif
#ifndef __NO_GGM__
      if (!name.compare("GGM_CC1ppip_XSec_1DEnu_nu")) {
    return (new GGM_CC1ppip_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("GGM_CC1ppip_Evt_1DQ2_nu")) {
    return (new GGM_CC1ppip_Evt_1DQ2_nu(samplekey));

    /*
      MiniBooNE Samples
    */
    /*
      CCQE
    */
  } else
#endif
#ifndef __NO_MiniBooNE__
      if (!name.compare("MiniBooNE_CCQE_XSec_1DQ2_nu") ||
          !name.compare("MiniBooNE_CCQELike_XSec_1DQ2_nu")) {
    return (new MiniBooNE_CCQE_XSec_1DQ2_nu(samplekey));
  } else if (!name.compare("MiniBooNE_CCQE_XSec_1DQ2_antinu") ||
             !name.compare("MiniBooNE_CCQELike_XSec_1DQ2_antinu") ||
             !name.compare("MiniBooNE_CCQE_CTarg_XSec_1DQ2_antinu")) {
    return (new MiniBooNE_CCQE_XSec_1DQ2_antinu(samplekey));

  } else if (!name.compare("MiniBooNE_CCQE_XSec_2DTcos_nu") ||
             !name.compare("MiniBooNE_CCQELike_XSec_2DTcos_nu")) {
    return (new MiniBooNE_CCQE_XSec_2DTcos_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CCQE_XSec_2DTcos_antinu") ||
             !name.compare("MiniBooNE_CCQELike_XSec_2DTcos_antinu")) {
    return (new MiniBooNE_CCQE_XSec_2DTcos_antinu(samplekey));

    /*
      MiniBooNE CC1pi+
    */
    // 1D
  } else if (!name.compare("MiniBooNE_CC1pip_XSec_1DEnu_nu")) {
    return (new MiniBooNE_CC1pip_XSec_1DEnu_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_1DQ2_nu")) {
    return (new MiniBooNE_CC1pip_XSec_1DQ2_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_1DTpi_nu")) {
    return (new MiniBooNE_CC1pip_XSec_1DTpi_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_1DTu_nu")) {
    return (new MiniBooNE_CC1pip_XSec_1DTu_nu(samplekey));

    // 2D
  } else if (!name.compare("MiniBooNE_CC1pip_XSec_2DQ2Enu_nu")) {
    return (new MiniBooNE_CC1pip_XSec_2DQ2Enu_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_2DTpiCospi_nu")) {
    return (new MiniBooNE_CC1pip_XSec_2DTpiCospi_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_2DTpiEnu_nu")) {
    return (new MiniBooNE_CC1pip_XSec_2DTpiEnu_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_2DTuCosmu_nu")) {
    return (new MiniBooNE_CC1pip_XSec_2DTuCosmu_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_2DTuEnu_nu")) {
    return (new MiniBooNE_CC1pip_XSec_2DTuEnu_nu(samplekey));

    /*
      MiniBooNE CC1pi0
    */
  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1DEnu_nu")) {
    return (new MiniBooNE_CC1pi0_XSec_1DEnu_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1DQ2_nu")) {
    return (new MiniBooNE_CC1pi0_XSec_1DQ2_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1DTu_nu")) {
    return (new MiniBooNE_CC1pi0_XSec_1DTu_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1Dcosmu_nu")) {
    return (new MiniBooNE_CC1pi0_XSec_1Dcosmu_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1Dcospi0_nu")) {
    return (new MiniBooNE_CC1pi0_XSec_1Dcospi0_nu(samplekey));

  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1Dppi0_nu")) {
    return (new MiniBooNE_CC1pi0_XSec_1Dppi0_nu(samplekey));

  } else if (!name.compare("MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu") ||
             !name.compare("MiniBooNE_NC1pi0_XSec_1Dcospi0_rhc")) {
    return (new MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu(samplekey));

  } else if (!name.compare("MiniBooNE_NC1pi0_XSec_1Dcospi0_nu") ||
             !name.compare("MiniBooNE_NC1pi0_XSec_1Dcospi0_fhc")) {
    return (new MiniBooNE_NC1pi0_XSec_1Dcospi0_nu(samplekey));

  } else if (!name.compare("MiniBooNE_NC1pi0_XSec_1Dppi0_antinu") ||
             !name.compare("MiniBooNE_NC1pi0_XSec_1Dppi0_rhc")) {
    return (new MiniBooNE_NC1pi0_XSec_1Dppi0_antinu(samplekey));

  } else if (!name.compare("MiniBooNE_NC1pi0_XSec_1Dppi0_nu") ||
             !name.compare("MiniBooNE_NC1pi0_XSec_1Dppi0_fhc")) {
    return (new MiniBooNE_NC1pi0_XSec_1Dppi0_nu(samplekey));

    /*
      MiniBooNE NCEL
    */
  } else if (!name.compare("MiniBooNE_NCEL_XSec_Treco_nu")) {
    return (new MiniBooNE_NCEL_XSec_Treco_nu(samplekey));
  } else
#endif

#ifndef __NO_MicroBooNE__
      /*
      MicroBooNE Samples
      */

      /*
        MicroBooNE CCinclusive
      */
      if (!name.compare("MicroBooNE_CCInc_XSec_2DPcos_nu")) {
    return (new MicroBooNE_CCInc_XSec_2DPcos_nu(samplekey));
  } else
#endif

#ifndef __NO_MINERvA__
      /*
      MINERvA Samples
      */
      if (!name.compare("MINERvA_CCQE_XSec_1DQ2_nu") ||
          !name.compare("MINERvA_CCQE_XSec_1DQ2_nu_20deg") ||
          !name.compare("MINERvA_CCQE_XSec_1DQ2_nu_oldflux") ||
          !name.compare("MINERvA_CCQE_XSec_1DQ2_nu_20deg_oldflux")) {
    return (new MINERvA_CCQE_XSec_1DQ2_nu(samplekey));

  } else if (!name.compare("MINERvA_CCQE_XSec_1DQ2_antinu") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_antinu_20deg") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_antinu_oldflux") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_antinu_20deg_oldflux")) {
    return (new MINERvA_CCQE_XSec_1DQ2_antinu(samplekey));

  } else if (!name.compare("MINERvA_CCQE_XSec_1DQ2_joint_oldflux") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_joint_20deg_oldflux") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_joint") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_joint_20deg")) {
    return (new MINERvA_CCQE_XSec_1DQ2_joint(samplekey));

  } else if (!name.compare("MINERvA_CC0pi_XSec_1DEe_nue")) {
    return (new MINERvA_CC0pi_XSec_1DEe_nue(samplekey));

  } else if (!name.compare("MINERvA_CC0pi_XSec_1DQ2_nue")) {
    return (new MINERvA_CC0pi_XSec_1DQ2_nue(samplekey));

  } else if (!name.compare("MINERvA_CC0pi_XSec_1DThetae_nue")) {
    return (new MINERvA_CC0pi_XSec_1DThetae_nue(samplekey));

  } else if (!name.compare("MINERvA_CC0pinp_STV_XSec_1Dpmu_nu") ||
             !name.compare("MINERvA_CC0pinp_STV_XSec_1Dthmu_nu") ||
             !name.compare("MINERvA_CC0pinp_STV_XSec_1Dpprot_nu") ||
             !name.compare("MINERvA_CC0pinp_STV_XSec_1Dthprot_nu") ||
             !name.compare("MINERvA_CC0pinp_STV_XSec_1Dpnreco_nu") ||
             !name.compare("MINERvA_CC0pinp_STV_XSec_1Ddalphat_nu") ||
             !name.compare("MINERvA_CC0pinp_STV_XSec_1Ddpt_nu") ||
             !name.compare("MINERvA_CC0pinp_STV_XSec_1Ddphit_nu")) {
    return (new MINERvA_CC0pinp_STV_XSec_1D_nu(samplekey));

  } else if (!name.compare("MINERvA_CC0pi_XSec_1DQ2_nu_proton")) {
    return (new MINERvA_CC0pi_XSec_1DQ2_nu_proton(samplekey));

  } else if (!name.compare("MINERvA_CC0pi_XSec_1DQ2_TgtC_nu") ||
             !name.compare("MINERvA_CC0pi_XSec_1DQ2_TgtCH_nu") ||
             !name.compare("MINERvA_CC0pi_XSec_1DQ2_TgtFe_nu") ||
             !name.compare("MINERvA_CC0pi_XSec_1DQ2_TgtPb_nu")) {
    return (new MINERvA_CC0pi_XSec_1DQ2_Tgt_nu(samplekey));

  } else if (!name.compare("MINERvA_CC0pi_XSec_1DQ2_TgtRatioC_nu") ||
             !name.compare("MINERvA_CC0pi_XSec_1DQ2_TgtRatioFe_nu") ||
             !name.compare("MINERvA_CC0pi_XSec_1DQ2_TgtRatioPb_nu")) {
    return (new MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu(samplekey));

    // Dan Ruterbories measurements of late 2018
  } else if (!name.compare("MINERvA_CC0pi_XSec_2Dptpz_nu")) {
    return (new MINERvA_CC0pi_XSec_2D_nu(samplekey));

  } else if (!name.compare("MINERvA_CC0pi_XSec_1Dpt_nu") ||
             !name.compare("MINERvA_CC0pi_XSec_1Dpz_nu") ||
             !name.compare("MINERvA_CC0pi_XSec_1DQ2QE_nu") ||
             !name.compare("MINERvA_CC0pi_XSec_1DEnuQE_nu")) {
    return (new MINERvA_CC0pi_XSec_1D_2018_nu(samplekey));

    // C. Patrick's early 2018 measurements
  } else if (!name.compare("MINERvA_CC0pi_XSec_2Dptpz_antinu") ||
             !name.compare("MINERvA_CC0pi_XSec_2DQ2QEEnuQE_antinu") ||
             !name.compare("MINERvA_CC0pi_XSec_2DQ2QEEnuTrue_antinu")) {
    return (new MINERvA_CC0pi_XSec_2D_antinu(samplekey));

    /*
      CC1pi+
    */
    // DONE
  } else if (!name.compare("MINERvA_CC1pip_XSec_1DTpi_nu") ||
             !name.compare("MINERvA_CC1pip_XSec_1DTpi_nu_20deg") ||
             !name.compare("MINERvA_CC1pip_XSec_1DTpi_nu_fluxcorr") ||
             !name.compare("MINERvA_CC1pip_XSec_1DTpi_nu_20deg_fluxcorr")) {
    return (new MINERvA_CC1pip_XSec_1DTpi_nu(samplekey));

    // DONE
  } else if (!name.compare("MINERvA_CC1pip_XSec_1Dth_nu") ||
             !name.compare("MINERvA_CC1pip_XSec_1Dth_nu_20deg") ||
             !name.compare("MINERvA_CC1pip_XSec_1Dth_nu_fluxcorr") ||
             !name.compare("MINERvA_CC1pip_XSec_1Dth_nu_20deg_fluxcorr")) {
    return (new MINERvA_CC1pip_XSec_1Dth_nu(samplekey));

  } else if (!name.compare("MINERvA_CC1pip_XSec_1DTpi_nu_2017") ||
             !name.compare("MINERvA_CC1pip_XSec_1Dth_nu_2017") ||
             !name.compare("MINERvA_CC1pip_XSec_1Dpmu_nu_2017") ||
             !name.compare("MINERvA_CC1pip_XSec_1Dthmu_nu_2017") ||
             !name.compare("MINERvA_CC1pip_XSec_1DQ2_nu_2017") ||
             !name.compare("MINERvA_CC1pip_XSec_1DEnu_nu_2017")) {
    return (new MINERvA_CC1pip_XSec_1D_2017Update(samplekey));
    /*
      CCNpi+
    */
  } else if (!name.compare("MINERvA_CCNpip_XSec_1Dth_nu") ||
             !name.compare("MINERvA_CCNpip_XSec_1Dth_nu_2015") ||
             !name.compare("MINERvA_CCNpip_XSec_1Dth_nu_2016") ||
             !name.compare("MINERvA_CCNpip_XSec_1Dth_nu_2015_20deg") ||
             !name.compare("MINERvA_CCNpip_XSec_1Dth_nu_2015_fluxcorr") ||
             !name.compare("MINERvA_CCNpip_XSec_1Dth_nu_2015_20deg_fluxcorr")) {
    return (new MINERvA_CCNpip_XSec_1Dth_nu(samplekey));

  } else if (!name.compare("MINERvA_CCNpip_XSec_1DTpi_nu") ||
             !name.compare("MINERvA_CCNpip_XSec_1DTpi_nu_2015") ||
             !name.compare("MINERvA_CCNpip_XSec_1DTpi_nu_2016") ||
             !name.compare("MINERvA_CCNpip_XSec_1DTpi_nu_2015_20deg") ||
             !name.compare("MINERvA_CCNpip_XSec_1DTpi_nu_2015_fluxcorr") ||
             !name.compare(
                 "MINERvA_CCNpip_XSec_1DTpi_nu_2015_20deg_fluxcorr")) {
    return (new MINERvA_CCNpip_XSec_1DTpi_nu(samplekey));

  } else if (!name.compare("MINERvA_CCNpip_XSec_1Dthmu_nu")) {
    return (new MINERvA_CCNpip_XSec_1Dthmu_nu(samplekey));

  } else if (!name.compare("MINERvA_CCNpip_XSec_1Dpmu_nu")) {
    return (new MINERvA_CCNpip_XSec_1Dpmu_nu(samplekey));

  } else if (!name.compare("MINERvA_CCNpip_XSec_1DQ2_nu")) {
    return (new MINERvA_CCNpip_XSec_1DQ2_nu(samplekey));

  } else if (!name.compare("MINERvA_CCNpip_XSec_1DEnu_nu")) {
    return (new MINERvA_CCNpip_XSec_1DEnu_nu(samplekey));

    /*
      MINERvA CC1pi0 anti-nu
    */
    // Done
  } else if (!name.compare("MINERvA_CC1pi0_XSec_1Dth_antinu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1Dth_antinu_2015") ||
             !name.compare("MINERvA_CC1pi0_XSec_1Dth_antinu_2016") ||
             !name.compare("MINERvA_CC1pi0_XSec_1Dth_antinu_fluxcorr") ||
             !name.compare("MINERvA_CC1pi0_XSec_1Dth_antinu_2015_fluxcorr") ||
             !name.compare("MINERvA_CC1pi0_XSec_1Dth_antinu_2016_fluxcorr")) {
    return (new MINERvA_CC1pi0_XSec_1Dth_antinu(samplekey));

  } else if (!name.compare("MINERvA_CC1pi0_XSec_1Dppi0_antinu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1Dppi0_antinu_fluxcorr")) {
    return (new MINERvA_CC1pi0_XSec_1Dppi0_antinu(samplekey));

  } else if (!name.compare("MINERvA_CC1pi0_XSec_1DTpi0_antinu")) {
    return (new MINERvA_CC1pi0_XSec_1DTpi0_antinu(samplekey));

    // Done
  } else if (!name.compare("MINERvA_CC1pi0_XSec_1DQ2_antinu")) {
    return (new MINERvA_CC1pi0_XSec_1DQ2_antinu(samplekey));

    // Done
  } else if (!name.compare("MINERvA_CC1pi0_XSec_1Dthmu_antinu")) {
    return (new MINERvA_CC1pi0_XSec_1Dthmu_antinu(samplekey));

    // Done
  } else if (!name.compare("MINERvA_CC1pi0_XSec_1Dpmu_antinu")) {
    return (new MINERvA_CC1pi0_XSec_1Dpmu_antinu(samplekey));

    // Done
  } else if (!name.compare("MINERvA_CC1pi0_XSec_1DEnu_antinu")) {
    return (new MINERvA_CC1pi0_XSec_1DEnu_antinu(samplekey));

    // MINERvA CC1pi0 nu
  } else if (!name.compare("MINERvA_CC1pi0_XSec_1DTpi_nu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1Dth_nu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1Dpmu_nu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1Dthmu_nu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1DQ2_nu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1DEnu_nu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1DWexp_nu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1DPPi0Mass_nu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1DPPi0MassDelta_nu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1DCosAdler_nu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1DPhiAdler_nu")) {
    return (new MINERvA_CC1pi0_XSec_1D_nu(samplekey));

    /*
      CCINC
    */
  } else if (!name.compare("MINERvA_CCinc_XSec_2DEavq3_nu")) {
    return (new MINERvA_CCinc_XSec_2DEavq3_nu(samplekey));

  } else if (!name.compare("MINERvA_CCinc_XSec_1Dx_ratio_C12_CH") ||
             !name.compare("MINERvA_CCinc_XSec_1Dx_ratio_Fe56_CH") ||
             !name.compare("MINERvA_CCinc_XSec_1Dx_ratio_Pb208_CH")) {
    return (new MINERvA_CCinc_XSec_1Dx_ratio(samplekey));

  } else if (!name.compare("MINERvA_CCinc_XSec_1DEnu_ratio_C12_CH") ||
             !name.compare("MINERvA_CCinc_XSec_1DEnu_ratio_Fe56_CH") ||
             !name.compare("MINERvA_CCinc_XSec_1DEnu_ratio_Pb208_CH")) {
    return (new MINERvA_CCinc_XSec_1DEnu_ratio(samplekey));
    /*
      CCDIS
    */
  } else if (!name.compare("MINERvA_CCDIS_XSec_1Dx_ratio_C12_CH") ||
             !name.compare("MINERvA_CCDIS_XSec_1Dx_ratio_Fe56_CH") ||
             !name.compare("MINERvA_CCDIS_XSec_1Dx_ratio_Pb208_CH")) {
    return (new MINERvA_CCDIS_XSec_1Dx_ratio(samplekey));

  } else if (!name.compare("MINERvA_CCDIS_XSec_1DEnu_ratio_C12_CH") ||
             !name.compare("MINERvA_CCDIS_XSec_1DEnu_ratio_Fe56_CH") ||
             !name.compare("MINERvA_CCDIS_XSec_1DEnu_ratio_Pb208_CH")) {
    return (new MINERvA_CCDIS_XSec_1DEnu_ratio(samplekey));

    /*
      CC-COH
    */
  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1DEnu_nu")) {
    return (new MINERvA_CCCOHPI_XSec_1DEnu_nu(samplekey));
  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1DEpi_nu")) {
    return (new MINERvA_CCCOHPI_XSec_1DEpi_nu(samplekey));
  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1Dth_nu")) {
    return (new MINERvA_CCCOHPI_XSec_1Dth_nu(samplekey));
  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1DQ2_nu")) {
    return (new MINERvA_CCCOHPI_XSec_1DQ2_nu(samplekey));
  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1DEnu_antinu")) {
    return (new MINERvA_CCCOHPI_XSec_1DEnu_antinu(samplekey));
  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1DEpi_antinu")) {
    return (new MINERvA_CCCOHPI_XSec_1DEpi_antinu(samplekey));
  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1Dth_antinu")) {
    return (new MINERvA_CCCOHPI_XSec_1Dth_antinu(samplekey));
  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1DQ2_antinu")) {
    return (new MINERvA_CCCOHPI_XSec_1DQ2_antinu(samplekey));

  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1DEnu_joint")) {
    return (new MINERvA_CCCOHPI_XSec_joint(samplekey));
  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1DEpi_joint")) {
    return (new MINERvA_CCCOHPI_XSec_joint(samplekey));
  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1Dth_joint")) {
    return (new MINERvA_CCCOHPI_XSec_joint(samplekey));
  } else if (!name.compare("MINERvA_CCCOHPI_XSec_1DQ2_joint")) {
    return (new MINERvA_CCCOHPI_XSec_joint(samplekey));

    /*
    T2K Samples
    */

  } else
#endif
#ifndef __NO_T2K__
      if (!name.compare("T2K_CC0pi_XSec_2DPcos_nu_I")) {
    return (new T2K_CC0pi_XSec_2DPcos_nu_I(samplekey));

  } else if (!name.compare("T2K_CC0pi_XSec_2DPcos_nu_II")) {
    return (new T2K_CC0pi_XSec_2DPcos_nu_II(samplekey));

  } else if (!name.compare("T2K_CCinc_XSec_2DPcos_nu_nonuniform")) {
    return (new T2K_CCinc_XSec_2DPcos_nu_nonuniform(samplekey));

    /*
      T2K CC1pi+ CH samples
    */
    // Comment these out for now because we don't have the proper data

  } else if (!name.compare("T2K_CC1pip_CH_XSec_2Dpmucosmu_nu")) {
    return (new T2K_CC1pip_CH_XSec_2Dpmucosmu_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1Dppi_nu")) {
    return (new T2K_CC1pip_CH_XSec_1Dppi_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1Dthpi_nu")) {
    return (new T2K_CC1pip_CH_XSec_1Dthpi_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1Dthmupi_nu")) {
    return (new T2K_CC1pip_CH_XSec_1Dthmupi_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1DQ2_nu")) {
    return (new T2K_CC1pip_CH_XSec_1DQ2_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1DAdlerPhi_nu")) {
    return (new T2K_CC1pip_CH_XSec_1DAdlerPhi_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1DCosThAdler_nu")) {
    return (new T2K_CC1pip_CH_XSec_1DCosThAdler_nu(samplekey));

    // Maybe something for the future: were in Raquel's thesis
    //} else if (!name.compare("T2K_CC1pip_CH_XSec_1Dq3_nu")) {
    // return (new T2K_CC1pip_CH_XSec_1Dq3_nu(file, rw, type, fkdt));

    //} else if (!name.compare("T2K_CC1pip_CH_XSec_1Dthq3pi_nu")) {
    // return (new T2K_CC1pip_CH_XSec_1Dthq3pi_nu(file, rw, type, fkdt));

    //} else if (!name.compare("T2K_CC1pip_CH_XSec_1DWrec_nu")) {
    // return (new T2K_CC1pip_CH_XSec_1DWrec_nu(file, rw, type, fkdt));

    /*
      T2K CC1pi+ H2O samples
    */
  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1DEnuDelta_nu")) {
    return (new T2K_CC1pip_H2O_XSec_1DEnuDelta_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1DEnuMB_nu")) {
    return (new T2K_CC1pip_H2O_XSec_1DEnuMB_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1Dcosmu_nu")) {
    return (new T2K_CC1pip_H2O_XSec_1Dcosmu_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1Dcosmupi_nu")) {
    return (new T2K_CC1pip_H2O_XSec_1Dcosmupi_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1Dcospi_nu")) {
    return (new T2K_CC1pip_H2O_XSec_1Dcospi_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1Dpmu_nu")) {
    return (new T2K_CC1pip_H2O_XSec_1Dpmu_nu(samplekey));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1Dppi_nu")) {
    return (new T2K_CC1pip_H2O_XSec_1Dppi_nu(samplekey));

    /*
      T2K CC0pi + np CH samples
    */
  } else if (!name.compare("T2K_CC0pinp_STV_XSec_1Ddpt_nu")) {
    return (new T2K_CC0pinp_STV_XSec_1Ddpt_nu(samplekey));

  } else if (!name.compare("T2K_CC0pinp_STV_XSec_1Ddphit_nu")) {
    return (new T2K_CC0pinp_STV_XSec_1Ddphit_nu(samplekey));

  } else if (!name.compare("T2K_CC0pinp_STV_XSec_1Ddat_nu")) {
    return (new T2K_CC0pinp_STV_XSec_1Ddat_nu(samplekey));

  } else if (!name.compare("T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform")) {
    return (new T2K_CC0pi1p_XSec_3DPcoscos_nu_nonuniform(samplekey));

  } else if (!name.compare("T2K_CC0pinp_ifk_XSec_3Dinfp_nu")) {
    return (new T2K_CC0pinp_ifk_XSec_3Dinfp_nu(samplekey));

  } else if (!name.compare("T2K_CC0pinp_ifk_XSec_3Dinfa_nu")) {
    return (new T2K_CC0pinp_ifk_XSec_3Dinfa_nu(samplekey));

  } else if (!name.compare("T2K_CC0pinp_ifk_XSec_3Dinfip_nu")) {
    return (new T2K_CC0pinp_ifk_XSec_3Dinfip_nu(samplekey));

    // SciBooNE COH studies
  } else
#endif
#ifndef __NO_SciBooNE__
      if (!name.compare("SciBooNE_CCCOH_STOP_NTrks_nu")) {
    return (new SciBooNE_CCCOH_STOP_NTrks_nu(samplekey));

  } else if (!name.compare("SciBooNE_CCCOH_1TRK_1DQ2_nu")) {
    return (new SciBooNE_CCCOH_1TRK_1DQ2_nu(samplekey));
  } else if (!name.compare("SciBooNE_CCCOH_1TRK_1Dpmu_nu")) {
    return (new SciBooNE_CCCOH_1TRK_1Dpmu_nu(samplekey));
  } else if (!name.compare("SciBooNE_CCCOH_1TRK_1Dthetamu_nu")) {
    return (new SciBooNE_CCCOH_1TRK_1Dthetamu_nu(samplekey));

  } else if (!name.compare("SciBooNE_CCCOH_MuPr_1DQ2_nu")) {
    return (new SciBooNE_CCCOH_MuPr_1DQ2_nu(samplekey));
  } else if (!name.compare("SciBooNE_CCCOH_MuPr_1Dpmu_nu")) {
    return (new SciBooNE_CCCOH_MuPr_1Dpmu_nu(samplekey));
  } else if (!name.compare("SciBooNE_CCCOH_MuPr_1Dthetamu_nu")) {
    return (new SciBooNE_CCCOH_MuPr_1Dthetamu_nu(samplekey));

  } else if (!name.compare("SciBooNE_CCCOH_MuPiVA_1DQ2_nu")) {
    return (new SciBooNE_CCCOH_MuPiVA_1DQ2_nu(samplekey));
  } else if (!name.compare("SciBooNE_CCCOH_MuPiVA_1Dpmu_nu")) {
    return (new SciBooNE_CCCOH_MuPiVA_1Dpmu_nu(samplekey));
  } else if (!name.compare("SciBooNE_CCCOH_MuPiVA_1Dthetamu_nu")) {
    return (new SciBooNE_CCCOH_MuPiVA_1Dthetamu_nu(samplekey));

  } else if (!name.compare("SciBooNE_CCCOH_MuPiNoVA_1DQ2_nu")) {
    return (new SciBooNE_CCCOH_MuPiNoVA_1DQ2_nu(samplekey));
  } else if (!name.compare("SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu")) {
    return (new SciBooNE_CCCOH_MuPiNoVA_1Dthetapr_nu(samplekey));
  } else if (!name.compare("SciBooNE_CCCOH_MuPiNoVA_1Dthetapi_nu")) {
    return (new SciBooNE_CCCOH_MuPiNoVA_1Dthetapi_nu(samplekey));
  } else if (!name.compare("SciBooNE_CCCOH_MuPiNoVA_1Dthetamu_nu")) {
    return (new SciBooNE_CCCOH_MuPiNoVA_1Dthetamu_nu(samplekey));
  } else if (!name.compare("SciBooNE_CCCOH_MuPiNoVA_1Dpmu_nu")) {
    return (new SciBooNE_CCCOH_MuPiNoVA_1Dpmu_nu(samplekey));

  } else if (!name.compare("SciBooNE_CCCOH_STOPFINAL_1DQ2_nu")) {
    return (new SciBooNE_CCCOH_STOPFINAL_1DQ2_nu(samplekey));

    /*
    K2K Samples
    */
    /*
      NC1pi0
    */
  } else
#endif
#ifndef __NO_K2K__
      if (!name.compare("K2K_NC1pi0_Evt_1Dppi0_nu")) {
    return (new K2K_NC1pi0_Evt_1Dppi0_nu(samplekey));

    /*
    Fake Studies
    */

  } else
#endif
      if (name.find("ExpMultDist_CCQE_XSec_1D") != std::string::npos &&
          name.find("_FakeStudy") != std::string::npos) {
    return (
        new ExpMultDist_CCQE_XSec_1DVar_FakeStudy(name, file, rw, type, fkdt));

  } else if (name.find("ExpMultDist_CCQE_XSec_2D") != std::string::npos &&
             name.find("_FakeStudy") != std::string::npos) {
    return (
        new ExpMultDist_CCQE_XSec_2DVar_FakeStudy(name, file, rw, type, fkdt));

  } else if (name.find("GenericFlux_") != std::string::npos) {
    return (new GenericFlux_Tester(name, file, rw, type, fkdt));

  } else if (name.find("GenericVectors_") != std::string::npos) {
    return (new GenericFlux_Vectors(name, file, rw, type, fkdt));

  } else if (!name.compare("T2K2017_FakeData")) {
    return (new T2K2017_FakeData(samplekey));

  } else if (!name.compare("MCStudy_CCQE")) {
    return (new MCStudy_CCQEHistograms(name, file, rw, type, fkdt));

  } else if (!name.compare("ElectronFlux_FlatTree")) {
    return (new ElectronFlux_FlatTree(name, file, rw, type, fkdt));

  } else if (name.find("ElectronData_") != std::string::npos) {
    return new ElectronScattering_DurhamData(samplekey);

  } else if (name.find("MuonValidation_") != std::string::npos) {
    return (new MCStudy_MuonValidation(name, file, rw, type, fkdt));

  } else if (!name.compare("NIWGOfficialPlots")) {
    return (new OfficialNIWGPlots(samplekey));

  } else if (!name.compare("Simple_Osc")) {
    return (new Simple_Osc(samplekey));

  } else if (!name.compare("Smear_SVDUnfold_Propagation_Osc")) {
    return (new Smear_SVDUnfold_Propagation_Osc(samplekey));

  } else {
    NUIS_ABORT("Error: No such sample: " << name << std::endl);
  }

  // Return NULL if no sample loaded.
  return NULL;
}
} // namespace SampleUtils
