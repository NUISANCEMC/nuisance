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
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include "FitLogger.h"

/*!
 *  \addtogroup Utils
 *  @{
 */

/// Functions which deal with basic string and file handling. They should have
/// no dependence on the other NUISANCE files!
namespace GeneralUtils {

/*!
  String handling and file parsing functions
*/

/// Parse a string into a vector of doubles given a delimiter "del"
std::vector<double> ParseToDbl(std::string str, const char* del);

/// Parse a string into a vector of ints given a delimiter "del"
std::vector<int> ParseToInt(std::string str, const char* del);

/// Parse a string into a vector of strings given a delimiter "del"
std::vector<std::string> ParseToStr(std::string str, const char* del);

/// Parse text file into a vector of strings
std::vector<std::string> ParseFileToStr(std::string str, const char* del);

/// Convert a string to a double
double StrToDbl(std::string str);

/// Convert a string to an int
int StrToInt(std::string str);

/// Convert a string to an bool
bool StrToBool(std::string str);

/// Convert a bool to string
std::string BoolToStr(bool val);

/// Convert Int to string
std::string IntToStr(int val);

/// Convert Double to String
std::string DblToStr(double val);

/// Return the top level environmental variable for the fitter
std::string GetTopLevelDir();

// /// A utility function to return a std::vector from an array
// template <typename T, size_t N>
// std::vector<T> makeVector(const T (&data)[N]) {
//   return std::vector<T>(data, data + N);
// }
std::vector<std::string> LoadCharToVectStr(int argc, char* argv[]);


template <typename T, size_t N>
size_t GetArraySize(const T (&data)[N]) {
  return N;
}
template <typename T>
size_t GetHammingWeight(T const& d) {
  T c = d;
  size_t w = 0;
  while (bool(c)) {
    w += c & 1;
    c = (c >> 1);
  }
  return w;
}

template <typename T>
size_t GetFirstOnBit(T const& d) {
  T c = d;
  size_t fob = 0;
  while (bool(c)) {
    if (c & 1) {
      return fob;
    } else {
      c = (c >> 1);
    }
    fob++;
  }
  return fob;
}

template <typename T>
size_t IsSmallNum(T const& d) {
  if (std::numeric_limits<T>::is_integer) {
    return (d == 0);
  }
  return (((d > 0) && (d < std::numeric_limits<T>::epsilon())) ||
          ((d < 0) && (d > -std::numeric_limits<T>::epsilon())));
}

std::string ReplaceAll(std::string const &inp, std::string const &from,
                    std::string const &to);

}

/*! @} */
#endif
