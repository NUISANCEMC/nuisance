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

#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <iostream>
#include <fstream>  
#include <unistd.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

/*!
 *  \addtogroup Utils
 *  @{
 */

//! Functions which deal with basic string and file handling. They should have no dependence on the other NUISANCE files! 
namespace GeneralUtils{

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

  //! A utility function to return a std::vector from an array
  template< typename T, size_t N >
    std::vector<T> makeVector( const T (&data)[N] )
  {
    return std::vector<T>(data, data+N);
  }

}

/*! @} */
#endif
