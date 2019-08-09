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

#pragma once

#include "utility/TerminalUtility.hxx"

#include <regex>
#include <string>
#include <vector>

namespace nuis {
namespace utility {

std::string EnsureTrailingSlash(std::string str);
std::string parseCode(std::regex_constants::error_type etype);
std::string DeGlobPattern(std::string const &pattern);

std::string indent_apply_width(std::string, size_t indent = 0,
                               size_t width = GetWindowWidth());

std::vector<std::string> split(std::string const &str,
                               std::string const &delim);
std::vector<std::string> split(std::string const &str,
                               std::vector<std::string> const &delims);

std::string str_replace(std::string const &inp, std::string const &from,
                        std::string const &to);
} // namespace utility
} // namespace nuis
