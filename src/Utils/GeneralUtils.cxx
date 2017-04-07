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


std::string GeneralUtils::BoolToStr(bool val) {
  std::ostringstream ss;
  ss << val;
  return ss.str();
}

std::string GeneralUtils::IntToStr(int val) {
  std::ostringstream ss;
  ss << val;
  return ss.str();
};

std::string GeneralUtils::DblToStr(double val) {
  std::ostringstream ss;
  ss << val;
  return ss.str();

};


std::vector<std::string> GeneralUtils::LoadCharToVectStr(int argc, char* argv[]){
  std::vector<std::string> vect;
  for (int i = 1; i < argc; i++){
    vect.push_back( std::string(argv[i]) );
  }
  return vect;
}




std::vector<std::string> GeneralUtils::ParseToStr(std::string str, const char* del) {

  std::istringstream stream(str);
  std::string temp_string;
  std::vector<std::string> vals;

  while (std::getline(stream >> std::ws, temp_string, *del)) {
    if (temp_string.empty()) continue;
    vals.push_back(temp_string);

  }

  return vals;

}
std::vector<double> GeneralUtils::ParseToDbl(std::string str, const char* del) {

  std::istringstream stream(str);
  std::string temp_string;
  std::vector<double> vals;

  while (std::getline(stream >> std::ws, temp_string, *del)) {
    if (temp_string.empty()) continue;
    std::istringstream stream(temp_string);
    double entry;
    stream >> entry;

    vals.push_back(entry);

  }

  return vals;
}

std::vector<int> GeneralUtils::ParseToInt(std::string str, const char* del) {

  std::istringstream stream(str);
  std::string temp_string;
  std::vector<int> vals;

  while (std::getline(stream >> std::ws, temp_string, *del)) {
    if (temp_string.empty()) continue;
    std::istringstream stream(temp_string);
    int entry;
    stream >> entry;

    vals.push_back(entry);

  }

  return vals;
}

// To stop rooku's skin from crawling :p
double GeneralUtils::StrToDbl(std::string str) {

  std::istringstream stream(str);
  double val;
  stream >> val;

  return val;
}

int GeneralUtils::StrToInt(std::string str) {

  std::istringstream stream(str);
  int val;
  stream >> val;

  return val;
}

bool GeneralUtils::StrToBool(std::string str) {

  // convert result to lower case
  // for (int i = 0; i < str.size(); i++) str[i] = tolower(str[i]);

  // Test for true/false
  if      (!str.compare("false")) return false;
  else if (!str.compare("true") ) return true;
  if (str.empty()) return false;
  // Push into bool
  std::istringstream stream(str);
  bool val;
  stream >> val;

  return val;
}


std::vector<std::string> GeneralUtils::ParseFileToStr(std::string str, const char* del) {

  std::vector<std::string> linevect;
  std::string line;

  std::ifstream read;
  read.open(str.c_str());

  if (!read.is_open()) {
    ERR(FTL) << "Cannot open file " << str << " in ParseFileToStr" << std::endl;
    throw;
  }

  while ( std::getline(read >> std::ws, line, *del) ) {
    linevect.push_back(line);
  }

  read.close();

  return linevect;
}

std::string GeneralUtils::GetTopLevelDir() {

  static bool first = true;
  static std::string topLevelVarVal;

  if (first) {
    char * const var = getenv("EXT_FIT");
    if (!var) {
      ERR(FTL) << "Cannot find top level directory! Set the EXT_FIT environmental variable" << std::endl;
      exit(-1);
    }
    topLevelVarVal = std::string(var);
    first = false;
  }

  return topLevelVarVal;
}

