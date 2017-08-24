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

// Initialise the static members for the parameters class
FitParameters* FitParameters::m_fitparameterInstance = NULL;

FitParameters& FitParameters::GetParams(void) {
  if (!m_fitparameterInstance) {
    m_fitparameterInstance = new FitParameters;
  }

  return *m_fitparameterInstance;
};

FitParameters::~FitParameters() { parameterMap_all.clear(); };

FitParameters::FitParameters() {
  this->iteration = 0;

  std::string NUISANCE_dir = GeneralUtils::GetTopLevelDir();
  this->ReadParamFile(NUISANCE_dir + "/parameters/fitter.config.dat");
};

void FitParameters::SetParamFile(std::string fileName) {
  parFileName = fileName;
  return;
}

// Parameter File Parser
void FitParameters::ReadParamFile(std::string fileName) {
  std::string line;
  std::ifstream card(fileName.c_str(), std::ifstream::in);

  while (std::getline(card >> std::ws, line, '\n')) {
    std::vector<std::string> inputlist = GeneralUtils::ParseToStr(line, " ");

    // Check the line length
    if (inputlist.size() < 3) continue;

    // Check whether this is a comment
    if (inputlist[0].c_str()[0] == '#') continue;

    // Check whether this is a relevant line
    if (inputlist[0].compare("config") != 0) continue;

    std::string parName = inputlist[1];
    std::string parEntry = inputlist[2];
    if (parameterMap_all.find(parName) == parameterMap_all.end())
      parameterMap_all.insert(
          std::map<std::string, std::string>::value_type(parName, parEntry));
    else
      parameterMap_all[parName] = parEntry;
  }
  card.close();
  return;
}

// Used to override a parameter by the command line
void FitParameters::ForceParam(std::string parOption) {
  unsigned first = parOption.find("=");
  std::string parName = parOption.substr(0, first);
  std::string parEntry = parOption.substr(first + 1, parOption.size());

  QLOG(REC,"Read in Parameter Override : " << parName << " = " << parEntry);

  parameterMap_all.insert(
      std::map<std::string, std::string>::value_type(parName, parEntry));
  if (parameterMap_all.find(parName) == parameterMap_all.end())
    parameterMap_all.insert(
        std::map<std::string, std::string>::value_type(parName, parEntry));
  else
    parameterMap_all[parName] = parEntry;

  return;
};

void FitParameters::SetParB(std::string parName, bool val) {
  if (parameterMap_bool.find(parName) != parameterMap_bool.end()) {
    parameterMap_bool[parName] = val;
  } else {
    parameterMap_bool.insert(
        std::map<std::string, bool>::value_type(parName, val));
  }

  return;
}

void FitParameters::SetParD(std::string parName, double val) {
  // Check if it is saved in int map
  if (parameterMap_double.find(parName) != parameterMap_double.end()) {
    parameterMap_double[parName] = val;
  } else {
    parameterMap_double.insert(
        std::map<std::string, double>::value_type(parName, val));
  }
  return;
}

void FitParameters::SetParI(std::string parName, int val) {
  // Check if it is saved in int map
  if (parameterMap_int.find(parName) != parameterMap_int.end()) {
    parameterMap_int[parName] = val;
  } else {
    parameterMap_int.insert(
        std::map<std::string, int>::value_type(parName, val));
  }
  return;
}

// Parameter fetch commands
int FitParameters::GetParI(std::string parName) {
  return Config::Get().ConfI(parName);
};

// Parameter fetch commands
bool FitParameters::GetParB(std::string parName) {
  return Config::Get().ConfB(parName);
};

double FitParameters::GetParD(std::string parName) {
  return Config::Get().ConfD(parName);
};

std::string FitParameters::GetParS(std::string parName) {
  return Config::Get().ConfS(parName);
};

std::string FitParameters::GetParDIR(std::string parName) {
  std::string outstr = this->GetParS(parName);

  // Make replacements in the string
  const int nfiletypes = 2;
  const std::string filetypes[nfiletypes] = {"@data", "@nuisance"};
  std::string filerepl[nfiletypes] = {FitPar::GetDataBase(),
                                      FitPar::GetDataBase() + "/../"};

  for (int i = 0; i < nfiletypes; i++) {
    std::string findstring = filetypes[i];
    std::string replstring = filerepl[i];
    if (outstr.find(findstring) != std::string::npos) {
      outstr.replace(outstr.find(findstring), findstring.size(), filerepl[i]);
      break;
    }
  }

  return outstr;
};

std::string FitParameters::GetAllParametersArg() {
  std::map<std::string, std::string>::iterator mystr = parameterMap_all.begin();
  std::string longlist = "";

  for (; mystr != parameterMap_all.end(); mystr++) {
    longlist += "-q " + mystr->first + "=" + mystr->second + " ";
  }

  return longlist;
}

void FitParameters::MakeParameterCard(std::string filename) {
  std::ofstream parcard;
  parcard.open((filename).c_str(), std::ios::out);
  std::map<std::string, std::string>::iterator mystr = parameterMap_all.begin();

  for (; mystr != parameterMap_all.end(); mystr++) {
    std::string name = (mystr->first);
    parcard << "config " << name << " ";

    if (parameterMap_int.find(name) != parameterMap_int.end())
      parcard << parameterMap_int.at(name);
    else if (parameterMap_double.find(name) != parameterMap_double.end())
      parcard << parameterMap_double.at(name);
    else if (parameterMap_string.find(name) != parameterMap_string.end())
      parcard << parameterMap_string.at(name);
    else if (parameterMap_all.find(name) != parameterMap_all.end())
      parcard << parameterMap_all.at(name);
    parcard << "\n";
  }

  parcard.close();

  return;
}

void FitParameters::Write() {
  // Loop through parameters
  /*  TTree* tr = new TTree("fit_header","fit_header");
  tr->Branch("par_name",  &parNames);
  tr->Branch("par_value", &parValues);
  tr->Branch("card_input", &cardLines);
  tr->Fill();

  tr->Write();
  */
  return;
}

// Global Access Namespace
//! Namespace to allow singletons to be accessed easily
namespace FitPar {

//! Returns FitParameters singleton. Usually used for parameters,
//! e.g. FitPar::Config().GetParI("input.maxevents")
FitParameters& Config() { return FitParameters::GetParams(); };

std::string GetDataBase() { return GeneralUtils::GetTopLevelDir() + "/data/"; };
}
