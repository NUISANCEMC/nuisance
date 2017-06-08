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
#ifndef BEAM_UTILS_H
#define BEAM_UTILS_H

#include <vector>
#include "FitLogger.h"
#include "GeneralUtils.h"

/*!
 *  \addtogroup Utils
 *  @{
 */

/// Namespace for any global util functions related to beam simulation
namespace BeamUtils{

  /// @brief Converts comma seperated string of beam ids into PDG vector
  /// 
  /// e.g. 'numu,nue,numub,e,16' = <14,12,-14,11,16>
  std::vector<int> ParseSpeciesToIntVect(std::string spc);
}
/*! @} */
#endif
