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

struct LetterBackronym {
  LetterBackronym(size_t n, std::string const &b, float p = 1.0,
                  std::string const &t = "") {
    NUsed = n;
    Backkie = b;
    ProbAccept = p;
    TagLine = t;
  };
  size_t NUsed;
  float ProbAccept;
  std::string Backkie;
  std::string TagLine;
};

__attribute__((constructor)) void nuisance_init(void) {
  std::vector<std::vector<LetterBackronym> > Letters;

  for (size_t i = 0; i < 8; ++i) {
    Letters.push_back(std::vector<LetterBackronym>());
  }

  Letters[0].push_back(LetterBackronym(2, "Neutrino"));
  Letters[0].push_back(LetterBackronym(3, "NUIsance", 0.2));

  Letters[2].push_back(LetterBackronym(1, "Interaction"));

  Letters[3].push_back(LetterBackronym(1, "Systematics"));
  Letters[3].push_back(LetterBackronym(
      1, "Synthesiser", 0.2, "Playing on the comparisons you want to see"));

  Letters[4].push_back(LetterBackronym(2, "ANalyser"));
  Letters[4].push_back(LetterBackronym(1, "Aggregating", 0.5));
  Letters[4].push_back(LetterBackronym(3, "from A-Neutrino sCattering", 1,
                                       "You can always find a frame"));

  Letters[5].push_back(
      LetterBackronym(1, "New", 1, "The freshest comparisons"));

  Letters[6].push_back(LetterBackronym(1, "by Comparing"));
  Letters[6].push_back(LetterBackronym(1, "Constraints from"));

  Letters[7].push_back(LetterBackronym(1, "Experiments"));

  std::vector<std::string> TagLines;
  TagLines.push_back("Fit and compare.");

  std::stringstream back("");

  TRandom3 tr;
  tr.SetSeed();

  for (size_t i = 0; i < 8;) {
    LetterBackronym const &let = Letters[i][tr.Integer(Letters[i].size())];
    if (tr.Uniform() > let.ProbAccept) {
      continue;
    }
    back << let.Backkie << " ";
    i += let.NUsed;
    if (let.TagLine.length()) {
      TagLines.push_back(let.TagLine);
    }
  }

  std::string Name = "Nuisance";
  std::string TagL = TagLines[tr.Integer(TagLines.size())];

  std::vector<std::pair<std::string, std::pair<std::string, std::string> > >
      OneBlob;

  OneBlob.push_back(
      std::make_pair("NUISANCE", std::make_pair("", "FiXing your Neutrinos")));

  if (tr.Uniform() < 0.01) {
    std::pair<std::string, std::pair<std::string, std::string> > const &blob =
        OneBlob[tr.Integer(OneBlob.size())];
    Name = blob.first;
    back.str("");
    back << blob.second.first;
    TagL = blob.second.second;
  }

  std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
               "%%%%%%%%%%%%%%%"
               "%%"
            << std::endl
            << "%%  Welcome to " << Name << ": \033[5m" << back.str()
            << "\033[0m-- " << TagL << std::endl
            << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
               "%%%%%%%%%%%%%%%"
               "%%"
            << std::endl;
}


namespace FitPar{
  unsigned int log_verb = 4; //!< Current VERBOSITY
  unsigned int err_verb = 0; //!< Current ERROR VERBOSITY
  bool use_colors = true; //!< Use BASH Terminal Colors Flag
  bool super_rainbow_mode = true; //!< For when fitting gets boring.
  unsigned int super_rainbow_mode_colour = 0;
   bool showtrace = false;
  // For redirecting various print outs
  std::streambuf *default_cout = std::cout.rdbuf();
  std::streambuf *default_cerr = std::cerr.rdbuf();
  std::ofstream redirect_stream("/dev/null");
  int silentfd = open("/dev/null",O_WRONLY);
  int savedstdoutfd = dup(fileno(stdout));
  int savedstderrfd = dup(fileno(stderr));

  int nloggercalls = 0;
  int timelastlog = 0;

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
  
  std::cout << "Set logging verbosity to : " << FitPar::log_verb << std::endl;
  return;
}

//******************************************
void ERR_VERB(std::string verb){
//******************************************
  std::cout << "Setting ERROR VERB" << std::endl;

  if    (!verb.compare("ERRQUIET")) FitPar::err_verb=0;
  else if (!verb.compare("FTL")) FitPar::err_verb=1;
  else if (!verb.compare("WRN")) FitPar::err_verb=2;
  else FitPar::err_verb = GeneralUtils::StrToInt(verb);

  std::cout << "Set error verbosity to : " << FitPar::err_verb << std::endl;
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

void SET_TRACE(bool val){
  FitPar::showtrace = val;
}


// std::ostream& LOG(FIT){
  // std::cout << "[ NUISANCE ]: ";
  // return *logStream;
// }

//******************************************
std::ostream& _LOG(int level, const char* filename, const char* func, int line)
//******************************************
{

  if (FitPar::nloggercalls > 1000){
    std::cout << "[WARNING] : Large number of logger calls being piped to LOGGER." << std::endl;
    std::cout << "This is super inefficient. : " << FitPar::nloggercalls << std::endl;
    std::cout << "Occuring at " << filename << "::" << func << "[l. " << line << "]" << std::endl;
    FitPar::nloggercalls = -1;
  }

  if (abs(time(NULL) - FitPar::timelastlog) > 10){
    FitPar::timelastlog = time(NULL);
    FitPar::nloggercalls = 0;
  }

  if (FitPar::nloggercalls != -1){
    FitPar::nloggercalls++;
  }

  

  if (FitPar::log_verb < (unsigned int)level && 
      FitPar::log_verb != (unsigned int)DEB){
    return nullStream;
  } else {

    if (FitPar::showtrace){
      std::cout << filename << "::" << func << "[l. " << line << "] : ";
    }

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
  //  return;
  // Only redirect if we're not debugging
  if (FitPar::log_verb == (unsigned int)DEB) return;
  std::cout.rdbuf(FitPar::redirect_stream.rdbuf());
  std::cerr.rdbuf(FitPar::redirect_stream.rdbuf());
  shhnuisancepythiaitokay_();
  fflush(stdout); 
  fflush(stderr);
  dup2(FitPar::silentfd, fileno(stdout));
  dup2(FitPar::silentfd, fileno(stderr));
}

void StartTalking(){
  std::cout.rdbuf(FitPar::default_cout);
  std::cerr.rdbuf(FitPar::default_cerr);
  canihaznuisancepythia_();
  fflush(stdout); 
  fflush(stderr);
  dup2(FitPar::savedstdoutfd, fileno(stdout));
  dup2(FitPar::savedstderrfd, fileno(stderr));
}

