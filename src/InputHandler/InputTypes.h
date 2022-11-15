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
#ifndef INPUTTYPES_SEEN_H
#define INPUTTYPES_SEEN_H

/// Global Enum to define generator type being read with FitEvent
/// Have to define kNORM as if its a generator for the time being.
enum generator_event_type {
  kUNKNOWN = 999,
  kNEUT = 0,
  kNIWG = 1,
  kNuWro = 2,
  kT2K = 3,
  kCUSTOM = 4,
  kGENIE = 5,
  kEVTSPLINE = 6,
  kNUANCE = 7,
  kGiBUU = 8,
  kNORM = 9,
  kEMPTY = 10,
  kINPUTFITEVENT = 11,
  kNEWSPLINE = 12,
  kLIKEWEIGHT = 13,
  kSPLINEPARAMETER = 14,
  kNuHepMC = 15,
  kHISTO = 16,
  kSIGMAQ0HIST = 17,
  kLast_generator_event_type
};

namespace InputUtils {

enum InputType {
  kNEUT_Input = 0,
  kNuWro_Input = 1,
  kGENIE_Input = 2,
  kGiBUU_Input,
  kNUANCE_Input,
  kNuHepMC_Input,
  kEVSPLN_Input,
  kEMPTY_Input,
  kFEVENT_Input,
  kJOINT_Input,
  kSIGMAQ0HIST_Input,
  kHISTO_Input,
  kInvalid_Input,
  kBNSPLN_Input,  // Not sure if this are currently used.
};

}

inline std::ostream& operator<<(std::ostream& os,
                                generator_event_type const& gs) {
  switch (gs) {
  case kUNKNOWN: {
    return os << "kUNKNOWN";
  }
  case kNEUT: {
    return os << "kNEUT";
  }
  case kNIWG: {
    return os << "kNIWG";
  }
  case kNuWro: {
    return os << "kNuWro";
  }
  case kT2K: {
    return os << "kT2K";
  }
  case kCUSTOM: {
    return os << "kCUSTOM";
  }
  case kGENIE: {
    return os << "kGENIE";
  }
  case kEVTSPLINE: {
    return os << "kEVTSPLINE";
  }
  case kNUANCE: {
    return os << "kNUANCE";
  }
  case kGiBUU: {
    return os << "kGiBUU";
  }
  case kNORM: {
    return os << "kNORM";
  }
  case kNuHepMC: {
    return os << "kNuHepMC";
  }
  case kSIGMAQ0HIST: {
    return os << "kSIGMAQ0HIST";
  }
  case kHISTO: {
    return os << "kHISTO";
  }
  default: { return os << "kUNKNOWN"; }
  }
}


inline std::ostream &operator<<(std::ostream &os, InputUtils::InputType it) {
  switch (it) {
  case InputUtils::kNEUT_Input: {
    return os << "kNEUT_Input";
  }
  case InputUtils::kNuWro_Input: {
    return os << "kNuWro_Input";
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
  case InputUtils::kNuHepMC_Input: {
    return os << "kNuHepMC_Input";
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
  case InputUtils::kSIGMAQ0HIST_Input: {
    return os << "kSIGMAQ0HIST_Input";
  }
  case InputUtils::kHISTO_Input: {
    return os << "kHISTO_Input";
  }
  case InputUtils::kInvalid_Input:
  case InputUtils::kBNSPLN_Input:
  default: { return os << "kInvalid_Input"; }
  }
}



#endif
