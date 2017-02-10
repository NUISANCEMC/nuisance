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

#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <string>
#include "TFile.h"

namespace InputUtils {

enum InputType {
  kNEUT_Input = 0,
  kNUWRO_Input,
  kGENIE_Input,
  kGiBUU_Input,
  kNUANCE_Input,
  kEVSPLN_Input,
  kEMPTY_Input,
  kFEVENT_Input,
  kJOINT_Input,  // Kept for backwards compatibility
  kInvalid_Input,
  kHIST_Input,   // Not sure if this are currently used.
  kBNSPLN_Input  // Not sure if this are currently used.
};

inline std::string EnsureTrailSlash(std::string str) {
  if (str[str.length() - 1] != '/') {
    str += '/';
  }
  return str;
}

inline std::string RemoveDoubleSlash(std::string str) {
  size_t torpl = str.find("//");
  while (torpl != std::string::npos) {
    str.replace(torpl, 2, "/");
    torpl = str.find("//");
  }
  return str;
}

InputType ParseInputType(std::string const &inp);
bool IsJointInput(std::string const &inputs);
std::string ExpandInputDirectories(std::string const &inputs);

InputType GuessInputTypeFromFile(TFile *inpF);
std::string PrependGuessedInputTypeToName(std::string const &inpFName);
}

inline std::ostream &operator<<(std::ostream &os, InputUtils::InputType it) {
  switch (it) {
    case InputUtils::kNEUT_Input: {
      return os << "kNEUT_Input";
    }
    case InputUtils::kNUWRO_Input: {
      return os << "kNUWRO_Input";
    }
    case InputUtils::kGENIE_Input: {
      return os << "kGENIE_Input";
    }
    case InputUtils::kGiBUU_Input: {
      return os << "kGiBUU_Input";
    }
    case InputUtils::kNUANCE_Input: {
      return os << "kNUANCE_Input";
    }
    case InputUtils::kEVSPLN_Input: {
      return os << "kEVSPLN_Input";
    }
    case InputUtils::kEMPTY_Input: {
      return os << "kEMPTY_Input";
    }
    case InputUtils::kFEVENT_Input: {
      return os << "kFEVENT_Input";
    }
    case InputUtils::kJOINT_Input: {
      return os << "kJOINT_Input";
    }
    case InputUtils::kInvalid_Input:
    case InputUtils::kHIST_Input:
    case InputUtils::kBNSPLN_Input:
    default: { return os << "kInvalid_Input"; }
  }
}

#endif
