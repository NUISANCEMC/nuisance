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

#include "FitParameters.h"
#include "GeneralUtils.h"

#include "InputUtils.h"

namespace InputUtils {
//********************************************************************
InputType ParseInputType(std::string const &inp) {
  //********************************************************************

  // The hard-coded list of supported input generators
  const static std::string filetypes[] = {"NEUT",  "NUWRO",  "GENIE",
                                          "GiBUU", "NUANCE", "EVSPLN",
                                          "EMPTY", "FEVENT", "JOINT"};

  size_t nInputTypes = GeneralUtils::GetArraySize(filetypes);

  for (size_t i = 0; i < nInputTypes; i++) {
    if (inp == filetypes[i]) {
      return InputType(i);
    }
  }

  return kInvalid_Input;
}
//********************************************************************
bool IsJointInput(std::string const &inputs) {
  //********************************************************************

  bool isJoint = (inputs[0] == '(');
  if (isJoint && (inputs[inputs.length() - 1] != ')')) {
    ERR(FTL) << "Inputs specifier: \"" << inputs
             << "\" looks like a composite input specifier -- "
                "(filea.root,fileb.root), however, it did not end in a \')\', "
                "it ended in a \'"
             << inputs[inputs.length() - 1] << "\'" << std::endl;
    throw;
  }
  return isJoint;
}

//********************************************************************
std::string ExpandInputDirectories(std::string const &inputs) {
  //********************************************************************

  // Parse the "environement" flags in the fitter config
  // Can specify NEUT_DIR = "" and others in parameters/fitter.config.dat
  const static std::string filedir[] = {"NEUT_DIR",   "NUWRO_DIR",
                                        "GENIE_DIR",  "NUANCE_DIR",
                                        "EVSPLN_DIR", "GIBUU_DIR"};
  size_t nfiledir = GeneralUtils::GetArraySize(filedir);
  std::string expandedInputs = inputs;

  for (size_t i = 0; i < nfiledir; i++) {
    std::string tempdir = "@" + filedir[i];
    size_t torpl = expandedInputs.find(tempdir);
    if (torpl != std::string::npos) {
      std::string event_folder = FitPar::Config().GetParS(filedir[i]);
      expandedInputs.replace(torpl, tempdir.size(), event_folder);
      break;
    }
  }

  return expandedInputs;
}
}
