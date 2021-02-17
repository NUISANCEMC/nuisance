// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#ifndef TARGET_UTILS_H
#define TARGET_UTILS_H

#include "FitLogger.h"
#include "GeneralUtils.h"

/// Namespace for utils parsing target pdgs
namespace TargetUtils {

	/// Convert comma separated string of targets to vector of PDG codes
	///
	/// e.g. "C,H,10000100010" = "100000600120,10000100010,100000100010"
	std::vector<int> ParseTargetsToIntVect(std::string targets);

	/// Convert a target string to pdg code
	int GetTargetPDGFromString(std::string target);

	/// Get Nuclear PDG code from Z and A
	int GetTargetPDGFromZA(int Z, int A);

	/// Get TargetZ code from the full PDG
	int GetTargetZFromPDG(int PDG);

	/// Get TargetA code from the full PDG
	int GetTargetAFromPDG(int PDG);

	/// Convert target ids to target that can be used by gen_nuisance
	std::string          ConvertTargetIDs        (std::string);

	/// Lists all possible target IDS, should be kept in sync with convert function
	void            ListTargetIDs(void);


}


#endif
