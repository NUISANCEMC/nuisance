// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#include "BeamUtils.h"

std::vector<int> BeamUtils::ParseSpeciesToIntVect(std::string spc) {

	// Split by comma
	std::vector<std::string> splitspc = GeneralUtils::ParseToStr(spc, ",");
	std::vector<int> convspc;

	// Run through hard coded list
	for (size_t i = 0; i < splitspc.size(); i++) {

		std::string type = splitspc[i];

		if      (!type.compare("electron")) convspc.push_back(11);
		else if (!type.compare("positron")) convspc.push_back(-11);
		else if (!type.compare("muon"))     convspc.push_back(13);
		else if (!type.compare("antimuon")) convspc.push_back(-13);

		else if (!type.compare("nue"))      convspc.push_back(12);
		else if (!type.compare("numu"))     convspc.push_back(14);
		else if (!type.compare("antinue"))  convspc.push_back(12);
		else if (!type.compare("antinumu")) convspc.push_back(14);
		else {
			convspc.push_back( GeneralUtils::StrToInt(type) );
		}
	}

	return convspc;
}


///____________________________________________________________________________
void BeamUtils::ListFluxIDs() {

	// Keep in sync with ConvertTargetIDs
	LOG(FIT) << "Possible Flux IDs: \n"
	         << "\n MINERvA_fhc_numu  : " << BeamUtils::ConvertFluxIDs("MINERvA_fhc_numu")
	         << "\n MINERvA_fhc_numunumubar  : " << BeamUtils::ConvertFluxIDs("MINERvA_fhc_numunumubar")
	         << "\n MINERvA_fhc_nue  : " << BeamUtils::ConvertFluxIDs("MINERvA_fhc_nue")
	         << "\n MINERvA_fhc_nuenuebar  : " << BeamUtils::ConvertFluxIDs("MINERvA_fhc_nuenuebar")
	         << "\n MINERvA_fhc_all  : " << BeamUtils::ConvertFluxIDs("MINERvA_fhc_all")

	         << "\n MINERvA_rhc_numubar  : " << BeamUtils::ConvertFluxIDs("MINERvA_rhc_numubar")
	         << "\n MINERvA_rhc_numubarnumu  : " << BeamUtils::ConvertFluxIDs("MINERvA_rhc_numubarnumu")
	         << "\n MINERvA_rhc_nuebar  : " << BeamUtils::ConvertFluxIDs("MINERvA_rhc_nuebar")
	         << "\n MINERvA_rhc_nuebarnue  : " << BeamUtils::ConvertFluxIDs("MINERvA_rhc_nuebarnue")
	         << "\n MINERvA_rhc_all  : " << BeamUtils::ConvertFluxIDs("MINERvA_rhc_all")

	         << "\n MiniBooNE_fhc_numu    : " << BeamUtils::ConvertFluxIDs("MiniBooNE_fhc_numu")
	         << "\n MiniBooNE_fhc_numubar : " << BeamUtils::ConvertFluxIDs("MiniBooNE_fhc_numubar")
	         << "\n MiniBooNE_fhc_numunumubar : " << BeamUtils::ConvertFluxIDs("MiniBooNE_fhc_numunumubar")
	         << "\n MiniBooNE_fhc_all     : " << BeamUtils::ConvertFluxIDs("MiniBooNE_fhc_all")

	         << "\n MiniBooNE_rhc_numubar : " << BeamUtils::ConvertFluxIDs("MiniBooNE_rhc_numubar")
	         << "\n MiniBooNE_rhc_numu    : " << BeamUtils::ConvertFluxIDs("MiniBooNE_rhc_numu")
	         << "\n MiniBooNE_rhc_numubarnumu : " << BeamUtils::ConvertFluxIDs("MiniBooNE_rhc_numubarnumu")
	         << "\n MiniBooNE_rhc_all     : " << BeamUtils::ConvertFluxIDs("MiniBooNE_rhc_all")

	         << "\n ArgoNeut_fhc_numu  : " << BeamUtils::ConvertFluxIDs("ArgoNeut_fhc_numu")
	         << "\n ArgoNeut_rhc_numubar : " << BeamUtils::ConvertFluxIDs("ArgoNeut_rhc_numubar")

	         << "\n K2K_fhc_numu : " << BeamUtils::ConvertFluxIDs("K2K_fhc_numu")

	         << "\n ND280_fhc_numu : " << BeamUtils::ConvertFluxIDs("ND280_fhc_numu")
	         << "\n ND280_fhc_numubar : " << BeamUtils::ConvertFluxIDs("ND280_fhc_numubar")
	         << "\n ND280_fhc_numunumubar : " << BeamUtils::ConvertFluxIDs("ND280_fhc_numunumubar")
	         << "\n ND280_fhc_nue : " << BeamUtils::ConvertFluxIDs("ND280_fhc_nue")
	         << "\n ND280_fhc_nuebar : " << BeamUtils::ConvertFluxIDs("ND280_fhc_nuebar")
	         << "\n ND280_fhc_nuenuebar : " << BeamUtils::ConvertFluxIDs("ND280_fhc_nuenuebar")
	         << "\n ND280_fhc_all : " << BeamUtils::ConvertFluxIDs("ND280_fhc_all")

	         << "\n ND280_rhc_numubar : " << BeamUtils::ConvertFluxIDs("ND280_rhc_numubar")
	         << "\n ND280_rhc_numu : " << BeamUtils::ConvertFluxIDs("ND280_rhc_numu")
	         << "\n ND280_rhc_numubarnumu : " << BeamUtils::ConvertFluxIDs("ND280_rhc_numubarnumu")
	         << "\n ND280_rhc_nuebar : " << BeamUtils::ConvertFluxIDs("ND280_rhc_nuebar")
	         << "\n ND280_rhc_nue : " << BeamUtils::ConvertFluxIDs("ND280_rhc_nue")
	         << "\n ND280_rhc_nuebarnue : " << BeamUtils::ConvertFluxIDs("ND280_rhc_nuebarnue")
	         << "\n ND280_rhc_all : " << BeamUtils::ConvertFluxIDs("ND280_rhc_all")

	         << "\n SK_fhc_numu : " << BeamUtils::ConvertFluxIDs("SK_fhc_numu")
	         << "\n SK_fhc_numubar : " << BeamUtils::ConvertFluxIDs("SK_fhc_numubar")
	         << "\n SK_fhc_numunumubar : " << BeamUtils::ConvertFluxIDs("SK_fhc_numunumubar")
	         << "\n SK_fhc_nue : " << BeamUtils::ConvertFluxIDs("SK_fhc_nue")
	         << "\n SK_fhc_nuebar : " << BeamUtils::ConvertFluxIDs("SK_fhc_nuebar")
	         << "\n SK_fhc_nuenuebar : " << BeamUtils::ConvertFluxIDs("SK_fhc_nuenuebar")
	         << "\n SK_fhc_all : " << BeamUtils::ConvertFluxIDs("SK_fhc_all")

	         << "\n SK_rhc_numubar : " << BeamUtils::ConvertFluxIDs("SK_rhc_numubar")
	         << "\n SK_rhc_numu : " << BeamUtils::ConvertFluxIDs("SK_rhc_numu")
	         << "\n SK_rhc_numubarnumu : " << BeamUtils::ConvertFluxIDs("SK_rhc_numubarnumu")
	         << "\n SK_rhc_nuebar : " << BeamUtils::ConvertFluxIDs("SK_rhc_nuebar")
	         << "\n SK_rhc_nue : " << BeamUtils::ConvertFluxIDs("SK_rhc_nue")
	         << "\n SK_rhc_nuebarnue : " << BeamUtils::ConvertFluxIDs("SK_rhc_nuebarnue")
	         << "\n SK_rhc_all : " << BeamUtils::ConvertFluxIDs("SK_rhc_all")

	         << "\n ANL_fhc_numu : " << BeamUtils::ConvertFluxIDs("ANL_fhc_numu")
	         << "\n BNL_fhc_numu : " << BeamUtils::ConvertFluxIDs("BNL_fhc_numu")
	         << "\n BNL_fhc_numu_ALT1986 : " << BeamUtils::ConvertFluxIDs("BNL_fhc_numu_ALT1986")
	         << "\n BNL_fhc_numu_ALT1981 : " << BeamUtils::ConvertFluxIDs("BNL_fhc_numu_ALT1981")
	         << "\n BEBC_fhc_numu : " << BeamUtils::ConvertFluxIDs("BEBC_fhc_numu")
	         << "\n FNAL_fhc_numu : " << BeamUtils::ConvertFluxIDs("FNAL_fhc_numu")
	         << "\n FNAL_rhc_numub : " << BeamUtils::ConvertFluxIDs("FNAL_rhc_numub")
	         << "\n GGM_fhc_numu : " << BeamUtils::ConvertFluxIDs("GGM_fhc_numu")

	         << std::endl;

}


//____________________________________________________________________________
std::string BeamUtils::ConvertFluxIDs(std::string id) {

	char * const var = getenv("NUISANCE");
	if (!var) {
		std::cout << "Cannot find top level directory! Set the NUISANCE environmental variable" << std::endl;
		exit(-1);
	}
	std::string topnuisancedir = std::string(var);
	std::string fluxfolder = topnuisancedir + "/data/flux/";
	std::string inputs = "";

	if (!id.compare("MINERvA_fhc_numu")) inputs = "minerva_flux.root,numu_fhc[14]";
	else if (!id.compare("MINERvA_fhc_numunumubar")) inputs = "minerva_flux.root,numu_fhc[14],numubar_fhc[-14]";
	else if (!id.compare("MINERvA_fhc_nue")) inputs = "minerva_flux.root,nue_fhc[12]";
	else if (!id.compare("MINERvA_fhc_nuenuebar")) inputs = "minerva_flux.root,nue_fhc[12],nuebar_fhc[-12]";
	else if (!id.compare("MINERvA_fhc_all")) inputs = "minerva_flux.root,numu_fhc[14],numubar_fhc[-14],nue_fhc[12],nuebar_fhc[-12]";

	else if (!id.compare("MINERvA_rhc_numubar")) inputs = "minerva_flux.root,numubar_rhc[-14]";
	else if (!id.compare("MINERvA_rhc_numubarnumu")) inputs = "minerva_flux.root,numubar_rhc[-14],numu_rhc[14]";
	else if (!id.compare("MINERvA_rhc_nuebar")) inputs = "minerva_flux.root,nuebar_rhc[-12]";
	else if (!id.compare("MINERvA_rhc_nuebarnue")) inputs = "minerva_flux.root,nuebar_rhc[-12],nue_rhc[12]";
	else if (!id.compare("MINERvA_rhc_all")) inputs = "minerva_flux.root,numu_rhc[14],numubar_rhc[-14],nue_rhc[12],nuebar_rhc[-12]";

	else if (!id.compare("MiniBooNE_fhc_numu")) inputs = "MiniBooNE_numu_flux.root,numu_mb[14]";
	else if (!id.compare("MiniBooNE_fhc_numubar")) inputs = "MiniBooNE_numu_flux.root,numub_mb[-14]";
	else if (!id.compare("MiniBooNE_fhc_numunumubar")) inputs = "MiniBooNE_numu_flux.root,numu_mb[14],numub_mb[-14]";
	else if (!id.compare("MiniBooNE_fhc_all")) inputs = "MiniBooNE_numu_flux.root,numu_mb[14],numub_mb[-14],nue_mb[12],nueb_mb[-12]";

	else if (!id.compare("MiniBooNE_rhc_numubar")) inputs = "MiniBooNE_numubar_flux.root,numub_mb[-14]";
	else if (!id.compare("MiniBooNE_rhc_numu")) inputs = "MiniBooNE_numubar_flux.root,numu_mb[14]";
	else if (!id.compare("MiniBooNE_rhc_numubarnumu")) inputs = "MiniBooNE_numubar_flux.root,numub_mb[-14],numu_mb[14]";
	else if (!id.compare("MiniBooNE_rhc_all")) inputs = "MiniBooNE_numubar_flux.root,numub_mb[-14],numu_mb[14],nueb_mb[-12],nue_mb[12]";

	else if (!id.compare("ArgoNeut_fhc_numu")) inputs = "ArgoNeut_numu_flux.root,numu[14]";
	else if (!id.compare("ArgoNeut_rhc_numub")) inputs = "ArgoNeut_numub_flux,numub[-14]";

	else if (!id.compare("K2K_fhc_numu")) inputs = "K2K_flux.root,numu_flux[14]";

	else if (!id.compare("ND280_fhc_numu")) inputs = "t2kflux_2016_plus250kA.root,enu_nd280_numu[14]";
	else if (!id.compare("ND280_fhc_numubar")) inputs = "t2kflux_2016_plus250kA.root,enu_nd280_numub[-14]";
	else if (!id.compare("ND280_fhc_numunumubar")) inputs = "t2kflux_2016_plus250kA.root,enu_nd280_numu[14],enu_nd280_numub[-14]";
	else if (!id.compare("ND280_fhc_nue")) inputs = "t2kflux_2016_plus250kA.root,enu_nd280_nue[12]";
	else if (!id.compare("ND280_fhc_nuebar")) inputs = "t2kflux_2016_plus250kA.root,enu_nd280_nueb[-12]";
	else if (!id.compare("ND280_fhc_nuenuebar")) inputs = "t2kflux_2016_plus250kA.root,enu_nd280_nue[12],enu_nd280_nueb[-12]";
	else if (!id.compare("ND280_fhc_all")) inputs = "t2kflux_2016_plus250kA.root,enu_nd280_numu[14],enu_nd280_numub[-14],enu_nd280_nue[12],enu_nd280_nueb[-12]";

	else if (!id.compare("ND280_rhc_numubar")) inputs = "t2kflux_2016_minus250kA.root,enu_nd280_numub[-14]";
	else if (!id.compare("ND280_rhc_numu")) inputs = "t2kflux_2016_minus250kA.root,enu_nd280_numu[14]";
	else if (!id.compare("ND280_rhc_numubarnumu")) inputs = "t2kflux_2016_minus250kA.root,enu_nd280_numub[-14],enu_nd280_numu[14]";
	else if (!id.compare("ND280_rhc_nuebar")) inputs = "t2kflux_2016_minus250kA.root,enu_nd280_nueb[-12]";
	else if (!id.compare("ND280_rhc_nue")) inputs = "t2kflux_2016_minus250kA.root,enu_nd280_nue[12]";
	else if (!id.compare("ND280_rhc_nuebarnue")) inputs = "t2kflux_2016_minus250kA.root,enu_nd280_nueb[-12],enu_nd280_nue[12]";
	else if (!id.compare("ND280_rhc_all")) inputs = "t2kflux_2016_minus250kA.root,enu_nd280_numub[-14],enu_nd280_numu[14],enu_nd280_nueb[-12],enu_nd280_nue[12]";

	else if (!id.compare("SK_fhc_numu")) inputs = "t2kflux_2016_plus250kA.root,enu_sk_numu[14]";
	else if (!id.compare("SK_fhc_numubar")) inputs = "t2kflux_2016_plus250kA.root,enu_sk_numub[-14]";
	else if (!id.compare("SK_fhc_numunumubar")) inputs = "t2kflux_2016_plus250kA.root,enu_sk_numu[14],enu_sk_numub[-14]";
	else if (!id.compare("SK_fhc_nue")) inputs = "t2kflux_2016_plus250kA.root,enu_sk_nue[12]";
	else if (!id.compare("SK_fhc_nuebar")) inputs = "t2kflux_2016_plus250kA.root,enu_sk_nueb[-12]";
	else if (!id.compare("SK_fhc_nuenuebar")) inputs = "t2kflux_2016_plus250kA.root,enu_sk_nue[12],enu_sk_nueb[-12]";
	else if (!id.compare("SK_fhc_all")) inputs = "t2kflux_2016_plus250kA.root,enu_sk_numu[14],enu_sk_numub[-14],enu_sk_nue[12],enu_sk_nueb[-12]";

	else if (!id.compare("SK_rhc_numubar")) inputs = "t2kflux_2016_minus250kA.root,enu_sk_numub[-14]";
	else if (!id.compare("SK_rhc_numu")) inputs = "t2kflux_2016_minus250kA.root,enu_sk_numu[14]";
	else if (!id.compare("SK_rhc_numubarnumu")) inputs = "t2kflux_2016_minus250kA.root,enu_sk_numub[-14],enu_sk_numu[14]";
	else if (!id.compare("SK_rhc_nuebar")) inputs = "t2kflux_2016_minus250kA.root,enu_sk_nueb[-12]";
	else if (!id.compare("SK_rhc_nue")) inputs = "t2kflux_2016_minus250kA.root,enu_sk_nue[12]";
	else if (!id.compare("SK_rhc_nuebarnue")) inputs = "t2kflux_2016_minus250kA.root,enu_sk_nueb[-12],enu_sk_nue[12]";
	else if (!id.compare("SK_rhc_all")) inputs = "t2kflux_2016_minus250kA.root,enu_sk_numub[-14],enu_sk_numu[14],enu_sk_nueb[-12],enu_nd280_nue[12]";

	else if (!id.compare("ANL_fhc_numu"))         inputs = "ANL_1977_2horn_rescan.root, numu_flux[14]";
	else if (!id.compare("BNL_fhc_numu"))         inputs = "BNL_NuInt02_rescan.root, numu_flux[14]";
	else if (!id.compare("BNL_fhc_numu_ALT1986")) inputs = "BNL_1986_flux - ALTERNATIVE.root, numu_flux[14]";
	else if (!id.compare("BNL_fhc_numu_ALT1981")) inputs = "BNL_CCQE_1981_rescan - ALTERNATIVE.root, numu_flux[14]";

	else if (!id.compare("BEBC_fhc_numu"))   inputs = "BEBC_Wachsmuth_numu_table.root, numu_flux[14]";
	else if (!id.compare("FNAL_fhc_numu"))   inputs = "FNAL_CCinc_1982_nu_MCadj.root, numu_flux[14]";
	else if (!id.compare("FNAL_rhc_numub"))  inputs = "FNAL_coh_1993_anu.root, numu_flux[-14]";
	else if (!id.compare("GGM_fhc_numu"))    inputs = "GGM_nu_flux_1979_rescan.root, numu_flux[14]";
	else return "";

	return fluxfolder + inputs;

};
