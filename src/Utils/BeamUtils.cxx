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

std::vector<int> BeamUtils::ParseSpeciesToIntVect(std::string spc){

  // Split by comma
  std::vector<std::string> splitspc = GeneralUtils::ParseToStr(spc,",");
  std::vector<int> convspc;
  
  // Run through hard coded list
  for (size_t i = 0; i < splitspc.size(); i++){
    
    std::string type = splitspc[i];
    
    if      (!type.compare("electron")) convspc.push_back(11);
    else if (!type.compare("positron")) convspc.push_back(-11);
    else if (!type.compare("muon"))     convspc.push_back(13);
    else if (!type.compare("antimuon")) convspc.push_back(-13);
    
    else if (!type.compare("nue"))      convspc.push_back(12);
    else if (!type.compare("numu"))     convspc.push_back(14);
    else if (!type.compare("antinue"))  convspc.push_back(12);
    else if (!type.compare("antinumu")) convspc.push_back(14);
    else{
      convspc.push_back( GeneralUtils::StrToInt(type) );
    }
  }
  
  return convspc;
}


///____________________________________________________________________________
void BeamUtils::ListFluxIDs(){

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
std::string BeamUtils::ConvertFluxIDs(std::string id){

  char * const var = getenv("NUISANCE");
  if (!var) {
    std::cout << "Cannot find top level directory! Set the NUISANCE environmental variable" << std::endl;
    exit(-1);
  }
  std::string topnuisancedir = std::string(var);
  std::string fluxfolder = topnuisancedir + "/data/flux/";
  std::string inputs = "";

  if (!id.compare("MINERvA_fhc_numu")) inputs="minerva_flux.root,numu_fhc[14]";
  else if (!id.compare("MINERvA_fhc_numunumubar")) inputs="minerva_flux.root,numu_fhc[14],numubar_fhc[-14]";
  else if (!id.compare("MINERvA_fhc_nue")) inputs="minerva_flux.root,nue_fhc[12]";
  else if (!id.compare("MINERvA_fhc_nuenuebar")) inputs="minerva_flux.root,nue_fhc[12],nuebar_fhc[-12]";
  else if (!id.compare("MINERvA_fhc_all")) inputs="minerva_flux.root,numu_fhc[14],numubar_fhc[-14],nue_fhc[12],nuebar_fhc[-12]";

  else if (!id.compare("MINERvA_rhc_numubar")) inputs="minerva_flux.root,numubar_rhc[-14]";
  else if (!id.compare("MINERvA_rhc_numubarnumu")) inputs="minerva_flux.root,numubar_rhc[-14],numu_rhc[14]";
  else if (!id.compare("MINERvA_rhc_nuebar")) inputs="minerva_flux.root,nuebar_rhc[-12]";
  else if (!id.compare("MINERvA_rhc_nuebarnue")) inputs="minerva_flux.root,nuebar_rhc[-12],nue_rhc[12]";
  else if (!id.compare("MINERvA_rhc_all")) inputs="minerva_flux.root,numu_rhc[14],numubar_rhc[-14],nue_rhc[12],nuebar_rhc[-12]";

  else if (!id.compare("ANL_fhc_numu"))         inputs="ANL_1977_2horn_rescan.root,numu_flux[14]";
  else if (!id.compare("BNL_fhc_numu"))         inputs="BNL_NuInt02_rescan.root,numu_flux[14]";
  else if (!id.compare("BNL_fhc_numu_ALT1986")) inputs="BNL_1986_flux-ALTERNATIVE.root,numu_flux[14]";
  else if (!id.compare("BNL_fhc_numu_ALT1981")) inputs="BNL_CCQE_1981_rescan-ALTERNATIVE.root,numu_flux[14]";

  else if (!id.compare("BEBC_fhc_numu"))   inputs="BEBC_Wachsmuth_numu_table.root,numu_flux[14]";
  else if (!id.compare("FNAL_fhc_numu"))   inputs="FNAL_CCinc_1982_nu_MCadj.root,numu_flux[14]";
  else if (!id.compare("FNAL_rhc_numub"))  inputs="FNAL_coh_1993_anu.root,numu_flux[-14]";
  else if (!id.compare("GGM_fhc_numu"))    inputs="GGM_nu_flux_1979_rescan.root,numu_flux[14]";
  else return "";

  return fluxfolder + inputs;

};
