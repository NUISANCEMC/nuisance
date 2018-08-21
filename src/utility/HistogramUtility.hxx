// Copyright 2018 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#ifndef UTILITY_HISTOGRAMUTILITY_HXX_SEEN
#define UTILITY_HISTOGRAMUTILITY_HXX_SEEN

#include "utility/ROOTUtility.hxx"

#include "exception/exception.hxx"

#include "string_parsers/from_string.hxx"

#include <memory>
#include <string>
#include <vector>
#include <iomanip>

namespace nuis {
namespace utility {

NEW_NUIS_EXCEPT(unimplemented_GetHistogram_method);
NEW_NUIS_EXCEPT(invalid_histogram_descriptor);

template <typename HT>
std::unique_ptr<HT> GetHistogram(std::string const &input_descriptor) {
  std::vector<std::string> split_descriptor =
      fhicl::string_parsers::ParseToVect<std::string>(input_descriptor, ";",
                                                      true, true);

  if (split_descriptor.size() == 1) { // assume text
    throw unimplemented_GetHistogram_method();
  } else if (split_descriptor.size() == 2) {
    return GetHistogramFromROOTFile<HT>(split_descriptor[0],
                                        split_descriptor[1]);
  } else {
    throw invalid_histogram_descriptor()
        << "[ERROR]: Failed to parse histogram descriptor: "
        << std::quoted(input_descriptor)
        << " as an input histogram (Text/ROOT).";
  }
}

} // namespace utility
} // namespace nuis
#endif
