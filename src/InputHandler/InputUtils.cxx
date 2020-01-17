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

#include "GeneralUtils.h"

#include "InputHandler.h"
#include "InputUtils.h"

namespace InputUtils {

std::vector<std::string> ParseInputFileList(std::string const &inpFile) {
  std::vector<std::string> inputs = GeneralUtils::ParseToStr(inpFile, ",");
  if (inputs.front()[0] == '(') {
    inputs.front() = inputs.front().substr(1);
  }
  if (inputs.back()[inputs.back().size() - 1] == ')') {
    inputs.back() = inputs.back().substr(0, inputs.back().size() - 1);
  }
  return inputs;
}

InputType ParseInputType(std::string const &inp) {
  // The hard-coded list of supported input generators
  const static std::string filetypes[] = {
      "NEUT",  "NUWRO",  "GENIE", "GiBUU",       "NUANCE", "EVSPLN",
      "EMPTY", "FEVENT", "JOINT", "SIGMAQ0HIST", "HISTO",  "HEPMCNUEVT"};

  size_t nInputTypes = GeneralUtils::GetArraySize(filetypes);

  for (size_t i = 0; i < nInputTypes; i++) {
    if (inp == filetypes[i]) {
      return InputType(i);
    }
  }

  return kInvalid_Input;
}

bool IsJointInput(std::string const &inputs) {
  bool isJoint = (inputs[0] == '(');
  if (isJoint && (inputs[inputs.length() - 1] != ')')) {
    NUIS_ABORT(
        "Inputs specifier: \""
        << inputs
        << "\" looks like a composite input specifier -- "
           "(filea.root,fileb.root), however, it did not end in a \')\', "
           "it ended in a \'"
        << inputs[inputs.length() - 1] << "\'");
  }
  return isJoint;
}

std::string ExpandInputDirectories(std::string const &inputs) {
  // Parse the "environement" flags in the fitter config
  // Can specify NEUT_DIR = "" and others in parameters/fitter.config.dat
  const static std::string filedir[] = {"NEUT_DIR",   "NUWRO_DIR",
                                        "GENIE_DIR",  "NUANCE_DIR",
                                        "EVSPLN_DIR", "GIBUU_DIR"};
  size_t nfiledir = GeneralUtils::GetArraySize(filedir);
  std::string expandedInputs = inputs;

  for (size_t i = 0; i < nfiledir; i++) {
    std::string tempdir = "@" + filedir[i];
    bool didRpl;
    do {
      size_t torpl = expandedInputs.find(tempdir);
      if (torpl != std::string::npos) {
        std::string event_folder = FitPar::Config().GetParS(filedir[i]);
        expandedInputs.replace(torpl, tempdir.size(), event_folder);
        didRpl = true;
      } else {
        didRpl = false;
      }
    } while (didRpl);
  }

  bool didRpl;
  do {
    size_t torpl = expandedInputs.find("//");
    if (torpl != std::string::npos) {
      expandedInputs.replace(torpl, 2, "/");
      didRpl = true;
    } else {
      didRpl = false;
    }
  } while (didRpl);

  return expandedInputs;
}

InputType GuessInputTypeFromFile(TFile *inpF) {
  const std::string NEUT_TreeName = "neuttree";
  const std::string NuWro_TreeName = "treeout";
  const std::string GENIE_TreeName = "gtree";
  const std::string GiBUU_TreeName = "giRooTracker";
  if (!inpF) {
    return kInvalid_Input;
  }
  TTree *NEUT_Input = dynamic_cast<TTree *>(inpF->Get(NEUT_TreeName.c_str()));
  if (NEUT_Input) {
    return kNEUT_Input;
  }
  TTree *NUWRO_Input = dynamic_cast<TTree *>(inpF->Get(NuWro_TreeName.c_str()));
  if (NUWRO_Input) {
    return kNUWRO_Input;
  }
  TTree *GENIE_Input = dynamic_cast<TTree *>(inpF->Get(GENIE_TreeName.c_str()));
  if (GENIE_Input) {
    return kGENIE_Input;
  }
  TTree *GiBUU_Input = dynamic_cast<TTree *>(inpF->Get(GiBUU_TreeName.c_str()));
  if (GiBUU_Input) {
    return kGiBUU_Input;
  }

  return kInvalid_Input;
}

std::string PrependGuessedInputTypeToName(std::string const &inpFName) {

  // If it already has a name.
  if (inpFName.find(":") != std::string::npos) {
    return inpFName;
  }

  TFile *inpF = TFile::Open(inpFName.c_str(), "READ");
  if (!inpF || !inpF->IsOpen()) {
    NUIS_ABORT("Couldn't open \"" << inpFName << "\" for reading.");
  }
  InputType iType = GuessInputTypeFromFile(inpF);
  if (iType == kInvalid_Input) {
    NUIS_ABORT("Couldn't determine input type from file: " << inpFName << ".");
  }
  inpF->Close();
  delete inpF;

  switch (iType) {
  case kNEUT_Input: {
    return "NEUT:" + inpFName;
  }
  case kNUWRO_Input: {
    return "NUWRO:" + inpFName;
  }
  case kGENIE_Input: {
    return "GENIE:" + inpFName;
  }
  case kGiBUU_Input: {
    return "GiBUU:" + inpFName;
  }
  default: {
    NUIS_ABORT("Input type from file: " << inpFName << " was invalid.");
    throw;
  }
  }
}
} // namespace InputUtils
