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

#ifndef GENERALUTILS_H_SEEN
#define GENERALUTILS_H_SEEN

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include "FitLogger.h"

/*!
 *  \addtogroup Utils
 *  @{
 */

//! Functions which deal with basic string and file handling. They should have
//! no dependence on the other NUISANCE files!
namespace GeneralUtils {

/*!
  String handling and file parsing functions
*/

//! Parse a string into a vector of doubles given a delimiter "del"
std::vector<double> ParseToDbl(std::string str, const char* del);

//! Parse a string into a vector of ints given a delimiter "del"
std::vector<int> ParseToInt(std::string str, const char* del);

//! Parse a string into a vector of strings given a delimiter "del"
std::vector<std::string> ParseToStr(std::string str, const char* del);

//! Parse text file into a vector of strings
std::vector<std::string> ParseFileToStr(std::string str, const char* del);

//! Convert a string to a double
double StrToDbl(std::string str);

//! Convert a string to an int
int StrToInt(std::string str);

//! Convert a string to an bool
bool StrToBool(std::string str);

//! Return the top level environmental variable for the fitter
std::string GetTopLevelDir();

// //! A utility function to return a std::vector from an array
// template <typename T, size_t N>
// std::vector<T> makeVector(const T (&data)[N]) {
//   return std::vector<T>(data, data + N);
// }
}

namespace PhysConst {
const double mass_proton = 0.93827203;   // Proton mass in GeV
const double mass_neutron = 0.93956536;  // Neutron mass in GeV
const double mass_delta = 1.232;         // Delta mass in GeV
const double mass_muon = 0.10565837;     // Muon mass in GeV
const int pdg_charged_pions[] = {211,-211};
const int pdg_pions[] = {211,-211,111};
const int pdg_strangemesons[] = {130,310,311,321,
				 9000311,9000321,
				 10311,10321,100311,100321,
				 9010311,9010321,9020311,9020321,
				 313,323,
				 10313,10323,
				 20313,20323,
				 100313,100323,
				 9000313,9000323,
				 30313,30323,
				 315,325,
				 9000315,9000325,
				 10315,10325,
				 20315,20325,
				 9010315,9010325,9020315,9020325,
				 317,327,
				 9010317,9010327};

// Just *-1 to cover possibility
const int pdg_antistrangemesons[] = {-130,-310,-311,-321,
				     -9000311,-9000321,
				     -10311,-10321,-100311,-100321,
				     -9010311,-9010321,-9020311,-9020321,
				     -313,-323,
				     -10313,-10323,
				     -20313,-20323,
				     -100313,-100323,
				     -9000313,-9000323,
				     -30313,-30323,
				     -315,-325,
				     -9000315,-9000325,
				     -10315,-10325,
				     -20315,-20325,
				     -9010315,-9010325,-9020315,-9020325,
				     -317,-327,
				     -9010317,-9010327};
 
}

/*! @} */
#endif
