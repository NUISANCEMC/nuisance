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

#include "FitLogger.h"
#include <fcntl.h>
#include <unistd.h>

namespace FitPar{
  unsigned int log_verb = 0; //!< Current VERBOSITY
  unsigned int err_verb = 0; //!< Current ERROR VERBOSITY
  bool use_colors = true; //!< Use BASH Terminal Colors Flag
  bool super_rainbow_mode = true; //!< For when fitting gets boring.
  unsigned int super_rainbow_mode_colour = 0;

  // For redirecting various print outs
  std::streambuf *default_cout = std::cout.rdbuf();
  std::streambuf *default_cerr = std::cerr.rdbuf();
  std::ofstream redirect_stream("/dev/null");
  int silentfd = open("/dev/null",O_WRONLY);
  int savedstdoutfd = dup(fileno(stdout));
  int savedstderrfd = dup(fileno(stderr));
}

std::ostream* logStream(&std::cout);
std::ostream* errStream(&std::cerr);
std::ofstream nullStream;

//******************************************
void LOG_VERB(std::string verb){
//******************************************

  if      (!verb.compare("DEB")) FitPar::log_verb=-1;
  else if (!verb.compare("QUIET")) FitPar::log_verb=0;
  else if (!verb.compare("FIT"))  FitPar::log_verb=1;
  else if (!verb.compare("MIN"))   FitPar::log_verb=2;
  else if (!verb.compare("SAM"))   FitPar::log_verb=3;
  else if (!verb.compare("REC"))   FitPar::log_verb=4;
  else if (!verb.compare("SIG"))   FitPar::log_verb=5;
  else if (!verb.compare("EVT"))   FitPar::log_verb=6;
  else FitPar::log_verb = GeneralUtils::StrToInt(verb);
  
  return;
}

//******************************************
void ERR_VERB(std::string verb){
//******************************************


  if    (!verb.compare("ERRQUIET")) FitPar::err_verb=0;
  else if (!verb.compare("FTL")) FitPar::err_verb=1;
  else if (!verb.compare("WRN")) FitPar::err_verb=2;
  else FitPar::log_verb = GeneralUtils::StrToInt(verb);

  return;
}

//******************************************
bool LOG_LEVEL(int level){
//******************************************

  if (FitPar::log_verb == (unsigned int) DEB){
    return true;
  }

  if (FitPar::log_verb <  (unsigned int) level){
    return false;
  }

  return true;
}

//******************************************
std::ostream& LOG(int level)
//******************************************
{

  if (FitPar::log_verb < (unsigned int)level && 
      FitPar::log_verb != (unsigned int)DEB){
    return nullStream;
  } else {

    if (FitPar::super_rainbow_mode and FitPar::use_colors){
      switch(FitPar::super_rainbow_mode_colour){
      case 1:  std::cout<<RED;
      case 2:  std::cout<<GREEN;
      case 3:  std::cout<<YELLOW;
      case 4:  std::cout<<BLUE;
      case 5:  std::cout<<MAGENTA;
      case 6:  std::cout<<CYAN;
      default: FitPar::super_rainbow_mode_colour = 0;
      }

      FitPar::super_rainbow_mode_colour++;
    }

    if (FitPar::use_colors){
      switch(level){
      case FIT: std::cout << BOLDGREEN; break;
      case MIN: std::cout << BOLDBLUE;  break;
      case SAM: std::cout << MAGENTA;   break;
      case REC: std::cout << BLUE;      break;
      case SIG: std::cout << GREEN;     break;
      case DEB: std::cout << CYAN;      break;
      default: break;
      }
    }

    switch(level){
    case FIT: std::cout << "[LOG Fitter]: "; break;
    case MIN: std::cout << "[LOG Minmzr]: "; break;
    case SAM: std::cout << "[LOG Sample]: - "; break;
    case REC: std::cout << "[LOG Reconf]: -- "; break;
    case SIG: std::cout << "[LOG Signal]: --- "; break;
    case EVT: std::cout << "[LOG Event ]: ---- "; break;
    case DEB: std::cout << "[LOG DEBUG ]: "; break;
    default: std::cout << "Log : "; break;
    }

    if (FitPar::use_colors or FitPar::super_rainbow_mode) std::cout << RESET;

    return *logStream;
  }
}
//******************************************
std::ostream& ERR(int level)
//******************************************
{

  if (FitPar::use_colors) std::cerr << RED;
  
  switch(level){
  case FTL: std::cerr << "[ERR FATAL ]: "; break;
  case WRN: std::cerr << "[ERR WARN  ] : "; break;
  }

  if (FitPar::use_colors) std::cerr << RESET;
  
  return *errStream;
}


void StopTalking(){
  // Only redirect if we're not debugging
  if (FitPar::log_verb == (unsigned int)DEB) return;
  std::cout.rdbuf(FitPar::redirect_stream.rdbuf());
  std::cerr.rdbuf(FitPar::redirect_stream.rdbuf());
  shhpythiaitokay_();
  fflush(stdout); 
  fflush(stderr);
  dup2(FitPar::silentfd, fileno(stdout));
  dup2(FitPar::silentfd, fileno(stderr));
}

void StartTalking(){
  std::cout.rdbuf(FitPar::default_cout);
  std::cerr.rdbuf(FitPar::default_cerr);
  canihazpythia_();
  fflush(stdout); 
  fflush(stderr);
  dup2(FitPar::savedstdoutfd, fileno(stdout));
  dup2(FitPar::savedstderrfd, fileno(stderr));
}

