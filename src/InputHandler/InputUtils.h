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

#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <string>
#include "TFile.h"
#include "InputTypes.h"

namespace InputUtils {

/// Extract Input Type from start of filename
InputType ParseInputType(std::string const &inp);

/// Check for comma separated joint input files
bool IsJointInput(std::string const &inputs);

/// Replace Input Event Diretory tags with Full Names from Config
std::string ExpandInputDirectories(std::string const &inputs);

/// Open ROOT file and guess what the file type is from the tree names
InputType GuessInputTypeFromFile(TFile *inpF);

/// Guess file input type and form an updated filename with it
std::string PrependGuessedInputTypeToName(std::string const &inpFName);

/// Split file inputs by commas and remove brackets before putting into a vector
std::vector<std::string> ParseInputFileList(std::string const& inpFile);

}

#endif
