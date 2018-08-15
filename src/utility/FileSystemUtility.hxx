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

#ifndef UTILITY_FILESYSTEMUTILITY_HXX_SEEN
#define UTILITY_FILESYSTEMUTILITY_HXX_SEEN

#include "exception/exception.hxx"

#include <dirent.h>

#include <regex>
#include <string>
#include <vector>

namespace nuis {
namespace utility {

NEW_NUIS_EXCEPT(unexpected_empty_string);

inline std::string EnsureTrailingSlash(std::string str) {
  if (!str.size()) {
    throw unexpected_empty_string();
  }
  if (str.back() != '/') {
    return str + '/';
  }
  return str;
}

inline std::vector<std::string> GetMatchingFiles(std::string directory,
                                                 std::string const &pattern) {
  directory = EnsureTrailingSlash(directory);
  std::regex rpattern(pattern);

  std::vector<std::string> matches;

  DIR *dir;
  struct dirent *ent;
  dir = opendir(directory.c_str());
  if (dir != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (std::regex_match(ent->d_name, rpattern)) {
        matches.push_back(ent->d_name);
      }
    }
  }
  return matches;
}
} // namespace utility
} // namespace nuis
#endif
