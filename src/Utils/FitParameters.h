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

#ifndef FITPARAMETERS_H_SEEN
#define FITPARAMETERS_H_SEEN

#include <math.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <sstream>
#include <string>
#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "TGraph.h"

using namespace std;

/*!
 *  \addtogroup FitBase
 *  @{
 */

//! Singleton class to allow easy reading and modification of configuration
//! parameters
class FitParameters {
 public:
  static FitParameters& GetParams(void);

  virtual ~FitParameters();

  FitParameters();

  void SetParamFile(std::string fileName);
  // Parameter File Parser
  void ReadParamFile(std::string fileName);

  // Used to override a parameter by the command line
  void ForceParam(std::string parOption);

  void SetParB(std::string parName, bool val);

  void SetParD(std::string parName, double val);

  void SetParI(std::string parName, int val);

  // Parameter fetch commands
  int GetParI(std::string parName);

  // Parameter fetch commands
  bool GetParB(std::string parName);

  double GetParD(std::string parName);

  std::string GetParS(std::string parName);

  // Variables
  std::string parFileName;
  std::map<std::string, std::string> parameterMap_all;

  int variables;
  bool nuwro_enabled;

  std::string GetAllParametersArg();

  void MakeParameterCard(std::string filename);
  
  void Write();

  TFile* out;  // pointer to output file
  std::string outputname;
  std::map<std::string, int> parameterMap_int;
  std::map<std::string, double> parameterMap_double;
  std::map<std::string, std::string> parameterMap_string;
  std::map<std::string, bool> parameterMap_bool;

  std::vector<std::string> parNames;
  std::vector<std::string> parValues;
  std::vector<std::string> cardLines;
  int iteration;

 protected:
  static FitParameters* m_fitparameterInstance;
};


// Global Access Namespace
//! Namespace to allow singletons to be accessed easily
namespace FitPar {

//! Returns FitParameters singleton. Usually used for parameters,
//! e.g. FitPar::Config().GetParI("MAXEVENTS")
FitParameters& Config();


//! Return Fitter DataBase
std::string GetDataBase();

//! Return NIWG Data Repo
std::string GetNIWGData();
}

/*! @} */
#endif
