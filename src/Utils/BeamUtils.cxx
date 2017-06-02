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
