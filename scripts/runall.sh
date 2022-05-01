#!/usr/bin/env bash

set -e

mkdir -p nuis_comps
cd nuis_comps

mkdir -p T2K/nu_mode/numu
cd T2K/nu_mode/numu

for TARGET in CH C O H2O; do
    if [ ! -e "T2K_ND.NEUT.${TARGET}.root" ] && [ "${FORCE}" != "ON" ]; then
        nuis gen NEUT -t ${TARGET} -m nu -p numu -n 250000 -e T2K_ND -o T2K_ND.NEUT.${TARGET}.root
    fi
done

if ! [ -e T2K_ND.NEUT.CH.comp.root ] && [ "${FORCE}" != "ON" ]; then
    nuis comp T2K_ND.NEUT.CH.root -f -t NEUT -o T2K_ND.NEUT.CH.comp.root\
        -s T2K_CC0pi_XSec_2DPcos_nu_I \
        -s T2K_CC0pi_XSec_2DPcos_nu_II \
        -s T2K_CCinc_XSec_2DPcos_nu_nonuniform \
        -s T2K_NuMu_CC0pi_CH_XSec_2DPcos \
        -s T2K_CC1pip_CH_XSec_1Dppi_nu \
        -s T2K_CC1pip_CH_XSec_1Dthpi_nu \
        -s T2K_CC1pip_CH_XSec_1Dthmupi_nu \
        -s T2K_CC1pip_CH_XSec_1DQ2_nu \
        -s T2K_CC1pip_CH_XSec_1DAdlerPhi_nu \
        -s T2K_CC1pip_CH_XSec_1DCosThAdler_nu \
        -s T2K_CC0pinp_STV_XSec_1Ddpt_nu \
        -s T2K_CC0pinp_STV_XSec_1Ddphit_nu \
        -s T2K_CC0pinp_STV_XSec_1Ddat_nu \
        -s T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np \
        -s T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p \
        -s T2K_CC0piWithProtons_XSec_2018_multidif_0p \
        -s T2K_CC0piWithProtons_XSec_2018_multidif_1p \
        -s T2K_CC0pinp_ifk_XSec_3Dinfp_nu \
        -s T2K_CC0pinp_ifk_XSec_3Dinfa_nu \
        -s T2K_CC0pinp_ifk_XSec_3Dinfip_nu \
        --config UseSVDInverse=1
fi

nuis plot -i T2K_ND.NEUT.CH.comp.root \
    -s T2K_CC0pi_XSec_2DPcos_nu_I:T2K_CC0pi_XSec_2DPcos_nu_I_MC:T2K_CC0pi_XSec_2DPcos_nu_I_data_1D \
    -o T2K_CC0pi_XSec_2DPcos_nu_I.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_CC0pi_XSec_2DPcos_nu_II \
    --split2D Y --bins-nodisplay X 2  \
    --zaxis-title 'd#sigma/dp_{#mu}cos(#theta_{#mu}) cm^{2} c/GeV' \
    -o T2K_CC0pi_XSec_2DPcos_nu_II.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_CCinc_XSec_2DPcos_nu_nonuniform \
    --slice-list T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice0_MC:T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice0_data \
                 T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice1_MC:T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice1_data \
                 T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice2_MC:T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice2_data \
                 T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice3_MC:T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice3_data \
                 T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice4_MC:T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice4_data \
                 T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice5_MC:T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice5_data \
                 T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice6_MC:T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice6_data \
                 T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice7_MC:T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice7_data \
                 T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice8_MC:T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice8_data \
                 T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice9_MC:T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice9_data \
                 T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice10_MC:T2K_CCinc_XSec_2DPcos_nu_nonuniform_Slice10_data \
    --bins-nodisplay X 0:1 --variable-yscale \
    -o T2K_CCinc_XSec_2DPcos_nu_nonuniform.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_NuMu_CC0pi_CH_XSec_2DPcos -o T2K_NuMu_CC0pi_CH_XSec_2DPcos.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_CC1pip_CH_XSec_1Dppi_nu \
    --bins-nodisplay X 1 -o T2K_CC1pip_CH_XSec_1Dppi_nu.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_CC1pip_CH_XSec_1Dthpi_nu -o T2K_CC1pip_CH_XSec_1Dthpi_nu.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_CC1pip_CH_XSec_1Dthmupi_nu -o T2K_CC1pip_CH_XSec_1Dthmupi_nu.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_CC1pip_CH_XSec_1DQ2_nu -o T2K_CC1pip_CH_XSec_1DQ2_nu.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_CC1pip_CH_XSec_1DAdlerPhi_nu -o T2K_CC1pip_CH_XSec_1DAdlerPhi_nu.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_CC1pip_CH_XSec_1DCosThAdler_nu -o T2K_CC1pip_CH_XSec_1DCosThAdler_nu.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_CC0pinp_STV_XSec_1Ddpt_nu -o T2K_CC0pinp_STV_XSec_1Ddpt_nu.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_CC0pinp_STV_XSec_1Ddphit_nu -o T2K_CC0pinp_STV_XSec_1Ddphit_nu.pdf

nuis plot -i T2K_ND.NEUT.CH.comp.root -s T2K_CC0pinp_STV_XSec_1Ddat_nu -o T2K_CC0pinp_STV_XSec_1Ddat_nu.pdf


# if ! [ -e T2K_ND.NEUT.C.comp.root ] && [ ${FORCE} != "ON" ]; then
#     nuis comp T2K_ND.NEUT.C.root -f -t NEUT -o T2K_ND.NEUT.C.comp.root\
#         -s T2K_NuMu_CC0pi_C_XSec_2DPcos \
#         -s T2K_CCCOH_C12_XSec_1DEnu_nu \
#         --config UseSVDInverse=1
# fi

# nuis plot -i T2K_ND.NEUT.C.comp.root -s T2K_NuMu_CC0pi_C_XSec_2DPcos -o T2K_NuMu_CC0pi_C_XSec_2DPcos.pdf

# nuis plot -i T2K_ND.NEUT.C.comp.root -s T2K_CCCOH_C12_XSec_1DEnu_nu -o T2K_CCCOH_C12_XSec_1DEnu_nu.pdf

# if ! [ -e T2K_ND.NEUT.H2O.comp.root ] && [ ${FORCE} != "ON" ]; then
#     nuis comp T2K_ND.NEUT.H2O.root -f -t NEUT -o T2K_ND.NEUT.H2O.comp.root\
#         -s T2K_CC0pi_XSec_H2O_2DPcos_anu \
#         -s T2K_CC1pip_H2O_XSec_1DEnuDelta_nu \
#         -s T2K_CC1pip_H2O_XSec_1DEnuMB_nu \
#         -s T2K_CC1pip_H2O_XSec_1Dcosmu_nu \
#         -s T2K_CC1pip_H2O_XSec_1Dcosmupi_nu \
#         -s T2K_CC1pip_H2O_XSec_1Dcospi_nu \
#         -s T2K_CC1pip_H2O_XSec_1Dpmu_nu \
#         -s T2K_CC1pip_H2O_XSec_1Dppi_nu \
#         --config UseSVDInverse=1
# fi

# nuis plot -i T2K_ND.NEUT.H2O.comp.root -s T2K_CC0pi_XSec_H2O_2DPcos_anu -o T2K_CC0pi_XSec_H2O_2DPcos_anu.pdf

# nuis plot -i T2K_ND.NEUT.H2O.comp.root -s T2K_CC1pip_H2O_XSec_1DEnuDelta_nu -o T2K_CC1pip_H2O_XSec_1DEnuDelta_nu.pdf

# nuis plot -i T2K_ND.NEUT.H2O.comp.root -s T2K_CC1pip_H2O_XSec_1DEnuMB_nu -o T2K_CC1pip_H2O_XSec_1DEnuMB_nu.pdf

# nuis plot -i T2K_ND.NEUT.H2O.comp.root -s T2K_CC1pip_H2O_XSec_1Dcosmu_nu -o T2K_CC1pip_H2O_XSec_1Dcosmu_nu.pdf

# nuis plot -i T2K_ND.NEUT.H2O.comp.root -s T2K_CC1pip_H2O_XSec_1Dcosmupi_nu -o T2K_CC1pip_H2O_XSec_1Dcosmupi_nu.pdf

# nuis plot -i T2K_ND.NEUT.H2O.comp.root -s T2K_CC1pip_H2O_XSec_1Dcospi_nu -o T2K_CC1pip_H2O_XSec_1Dcospi_nu.pdf

# nuis plot -i T2K_ND.NEUT.H2O.comp.root -s T2K_CC1pip_H2O_XSec_1Dpmu_nu -o T2K_CC1pip_H2O_XSec_1Dpmu_nu.pdf

# nuis plot -i T2K_ND.NEUT.H2O.comp.root -s T2K_CC1pip_H2O_XSec_1Dppi_nu -o T2K_CC1pip_H2O_XSec_1Dppi_nu.pdf

# if ! [ -e T2K_ND.NEUT.H2O.comp.root ] && [ ${FORCE} != "ON" ]; then
#     nuis comp T2K_ND.NEUT.H2O.root -f -t NEUT -o T2K_ND.NEUT.H2O.comp.root\
#         -s T2K_NuMu_CC0pi_O_XSec_2DPcos \
#         --config UseSVDInverse=1
# fi

# nuis plot -i T2K_ND.NEUT.O.comp.root -s T2K_NuMu_CC0pi_O_XSec_2DPcos -o T2K_NuMu_CC0pi_O_XSec_2DPcos.pdf

# if ! [ -e T2K_ND.NEUT.OC.comp.root ] && [ ${FORCE} != "ON" ]; then
#     nuis comp T2K_ND.NEUT.O.root T2K_ND.NEUT.C.root -f -t NEUT -o T2K_ND.NEUT.OC.comp.root\
#         -s T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint \
#         --config UseSVDInverse=1
# fi

# nuis plot -i T2K_ND.NEUT.OC.comp.root -s T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint -o T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint.pdf

# T2K_AntiNuMu_CC0pi_CH_XSec_2DPcos
# T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos_joint

# T2K_nueCCinc_XSec_1Dpe_FHC
# T2K_nueCCinc_XSec_1Dpe_RHC
# T2K_nuebarCCinc_XSec_1Dpe_RHC
# T2K_nueCCinc_XSec_1Dthe_FHC
# T2K_nueCCinc_XSec_1Dthe_RHC
# T2K_nuebarCCinc_XSec_1Dthe_RHC
# T2K_nueCCinc_XSec_1Dpe_joint
# T2K_nueCCinc_XSec_1Dthe_joint
# T2K_nueCCinc_XSec_joint

cd -

# mkdir -p T2K/nu_mode/numu/H2O
# cd T2K/nu_mode/numu/H2O

# nuis gen NEUT -t H2O -m nu -p numu -n 1000 -e T2K_ND -o T2K_ND.H2O.1000.root

# cd -