// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
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
  // Check if NUWRO is installed in the config
  this->nuwro_enabled = (!std::string(std::getenv("NIWG_DATA")).empty());
  this->iteration = 0;

  std::string ext_fit_dir = std::string(std::getenv("EXT_FIT"));
  this->ReadParamFile( ext_fit_dir + "/parameters/fitter.requirements.dat" );
};

void FitParameters::SetParamFile(std::string fileName) {
  parFileName = fileName;
  return;
}

// Parameter File Parser
void FitParameters::ReadParamFile(std::string fileName) {
  std::string line;
  std::ifstream card(fileName.c_str(), ifstream::in);

  while (std::getline(card, line, '\n')) {
    std::istringstream stream(line);

    std::string token;
    std::string parName;
    std::string parEntry;
    int val = 0;
    // check the type
    while (std::getline(stream, token, ' ')) {
      // Strip any leading whitespace from the stream
      stream >> std::ws;

      // Ignore comments
      if (token.c_str()[0] == '#') continue;

      std::istringstream stoken(token);

      if (val == 0) {
        if (token.compare("config") != 0) {
          break;
        }
      } else if (val == 1) {
        parName = token;
      } else if (val == 2) {
        parEntry = token;
        if (parameterMap_all.find(parName) == parameterMap_all.end())
          parameterMap_all.insert(
              map<std::string, std::string>::value_type(parName, parEntry));
        else
          parameterMap_all[parName] = parEntry;

      } else {
        break;
      }
      val++;
    }
  }
  card.close();
  return;
}

// Used to override a parameter by the command line
void FitParameters::ForceParam(std::string parOption) {
  unsigned first = parOption.find("=");
  std::string parName = parOption.substr(0, first);
  std::string parEntry = parOption.substr(first + 1, parOption.size());

  std::cout << "Read in Parameter Override : " << parName << " = " << parEntry
            << std::endl;

  parameterMap_all.insert(
      map<std::string, std::string>::value_type(parName, parEntry));
  if (parameterMap_all.find(parName) == parameterMap_all.end())
    parameterMap_all.insert(
        map<std::string, std::string>::value_type(parName, parEntry));
  else
    parameterMap_all[parName] = parEntry;

  return;
};

void FitParameters::SetParB(std::string parName, bool val) {
  if (parameterMap_bool.find(parName) != parameterMap_bool.end()) {
    parameterMap_bool[parName] = val;
  } else {
    parameterMap_bool.insert(map<std::string, bool>::value_type(parName, val));
  }

  return;
}

void FitParameters::SetParD(std::string parName, double val) {
  // Check if it is saved in int map
  if (parameterMap_double.find(parName) != parameterMap_double.end()) {
    parameterMap_double[parName] = val;
  } else {
    parameterMap_double.insert(
        map<std::string, double>::value_type(parName, val));
  }
  return;
}

void FitParameters::SetParI(std::string parName, int val) {
  // Check if it is saved in int map
  if (parameterMap_int.find(parName) != parameterMap_int.end()) {
    parameterMap_int[parName] = val;
  } else {
    parameterMap_int.insert(map<std::string, int>::value_type(parName, val));
  }
  return;
}

// Parameter fetch commands
int FitParameters::GetParI(std::string parName) {
  if (parName == "VERBOSITY") {
    if (parameterMap_all.find(parName) != parameterMap_all.end()) {
      int tempVal = 1;
      std::string verb = parameterMap_all.at(parName);

      if (!verb.compare("DEB"))
        tempVal = -1;
      else if (!verb.compare("QUIET"))
        tempVal = 0;
      else if (!verb.compare("FIT"))
        tempVal = 1;
      else if (!verb.compare("MIN"))
        tempVal = 2;
      else if (!verb.compare("SAM"))
        tempVal = 3;
      else if (!verb.compare("REC"))
        tempVal = 4;
      else if (!verb.compare("SIG"))
        tempVal = 5;
      else if (!verb.compare("EVT"))
        tempVal = 6;
      else {
        std::istringstream stoken(parameterMap_all[parName]);
        stoken >> tempVal;
      }

      // Convert.
      parameterMap_int.insert(
          map<std::string, int>::value_type(parName, tempVal));

    } else {
      return 1;  // If no parameter set for verbosity assume FIT
    }
  }

  // Check if it is saved in int map
  if (parameterMap_int.find(parName) != parameterMap_int.end()) {
    return parameterMap_int[parName];

    // Check if it is in the entire map
  } else if (parameterMap_all.find(parName) != parameterMap_all.end()) {
    int tempVal = 0;
    std::istringstream stoken(parameterMap_all[parName]);
    stoken >> tempVal;

    parameterMap_int.insert(
        map<std::string, int>::value_type(parName, tempVal));
    return tempVal;

  } else {
    //  std::cout<<"Error: Parameter - "<<parName<<" - not found in requirements
    //  file."<<std::endl;
    return -999;
  }
};

// Parameter fetch commands
bool FitParameters::GetParB(std::string parName) {
  // Check if it is saved in bool map
  if (parameterMap_bool.find(parName) != parameterMap_bool.end()) {
    return parameterMap_bool[parName];

    // Check if it is in the entire map
  } else if (parameterMap_all.find(parName) != parameterMap_all.end()) {
    bool tempVal = 0;
    std::istringstream stoken(parameterMap_all[parName]);
    stoken >> tempVal;

    parameterMap_bool.insert(
        map<std::string, bool>::value_type(parName, tempVal));
    return tempVal;

  } else {
    parameterMap_bool.insert(
        map<std::string, bool>::value_type(parName, false));
    return false;
  }
};

double FitParameters::GetParD(std::string parName) {
  // Check if it is saved in int map
  if (parameterMap_double.find(parName) != parameterMap_double.end()) {
    return parameterMap_double[parName];

    // Check if it is in the entire map
  } else if (parameterMap_all.find(parName) != parameterMap_all.end()) {
    double tempVal = 0.0;
    std::istringstream stoken(parameterMap_all[parName]);
    stoken >> tempVal;

    parameterMap_double.insert(
        map<std::string, double>::value_type(parName, tempVal));
    return tempVal;

  } else {
    //      std::cout<<"Error: Parameter - "<<parName<<" - not found in
    //      requirements file."<<std::endl;
    return -999.9;
  }
};

std::string FitParameters::GetParS(std::string parName) {
  // Check if it is saved in int map
  if (parameterMap_all.find(parName) != parameterMap_all.end()) {
    return parameterMap_all[parName];

  } else {
    std::cout << "Error: Parameter - " << parName
              << " - not found in requirements file." << std::endl;
    return "NULL PARAMETER";
  }
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
  ofstream parcard;
  parcard.open((filename).c_str(), ios::out);
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

void FitParameters::Write(){

  // Loop through parameters                                                                                                                                                                                                              
  TTree* tr = new TTree("fit_header","fit_header");
  tr->Branch("par_name",  &parNames);
  tr->Branch("par_value", &parValues);
  tr->Branch("card_input", &cardLines);
  tr->Fill();
  
  tr->Write();
  return;
}

// Global Access Namespace
//! Namespace to allow singletons to be accessed easily
namespace FitPar {

//! Returns FitParameters singleton. Usually used for parameters,
//! e.g. FitPar::Config().GetParI("MAXEVENTS")
FitParameters& Config() { return FitParameters::GetParams(); };

  std::string GetDataBase(){ return std::string(std::getenv("EXT_FIT")) + "/data/"; };
  std::string GetNIWGData(){ return std::string(std::getenv("NIWG_DATA")); };

}
