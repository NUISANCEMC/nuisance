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

namespace Logger {

// Logger Variables
int log_verb = 4;
bool use_colors = true;

bool showtrace  = true;

std::ostream* __LOG_outstream(&std::cout);
std::ofstream __LOG_nullstream;

// Error Variables
int err_verb = 0;
std::ostream* __ERR_outstream(&std::cerr);

// Extra Variables
bool external_verb = false;

bool super_rainbow_mode = true;  //!< For when fitting gets boring.
unsigned int super_rainbow_mode_colour = 0;

std::streambuf* default_cout = std::cout.rdbuf();
std::streambuf* default_cerr = std::cerr.rdbuf();
std::ofstream redirect_stream("/dev/null");
int silentfd = open("/dev/null", O_WRONLY);
int savedstdoutfd = dup(fileno(stdout));
int savedstderrfd = dup(fileno(stderr));

int nloggercalls = 0;
int timelastlog = 0;
}

// -------- Logging Functions --------- //

bool LOGGING(int level) {
  // std::cout << "LOGGING : " << __FILENAME__ << " " << __FUNCTION__ <<
  // std::endl;
  return (Logger::log_verb >=
          (int)__GETLOG_LEVEL(level, __FILENAME__, __FUNCTION__));
};

int __GETLOG_LEVEL(int level, const char* filename, const char* funct) {
#ifdef __DEBUG__
  int logfile = FitPar::Config().GetParI("logging." + std::string(filename));
  if (logfile >= DEB and logfile <= EVT) {
    level = logfile;
  }

  int logfunc = FitPar::Config().GetParI("logging." + std::string(funct));
  if (logfunc >= DEB and logfunc <= EVT) {
    level = logfunc;
  }
#endif

  return level;
};

std::ostream& __OUTLOG(int level, const char* filename, const char* funct,
                       int line) {
  if (Logger::log_verb < (int)level &&
      Logger::log_verb != (int)DEB) {
    return (Logger::__LOG_nullstream);

  } else {
    if (Logger::use_colors) {
      switch (level) {
        case FIT:
          std::cout << BOLDGREEN;
          break;
        case MIN:
          std::cout << BOLDBLUE;
          break;
        case SAM:
          std::cout << MAGENTA;
          break;
        case REC:
          std::cout << BLUE;
          break;
        case SIG:
          std::cout << GREEN;
          break;
        case DEB:
          std::cout << CYAN;
          break;
        default:
          break;
      }
    }

    switch (level) {
      case FIT:
        std::cout << "[LOG Fitter]";
        break;
      case MIN:
        std::cout << "[LOG Minmzr]";
        break;
      case SAM:
        std::cout << "[LOG Sample]";
        break;
      case REC:
        std::cout << "[LOG Reconf]";
        break;
      case SIG:
        std::cout << "[LOG Signal]";
        break;
      case EVT:
        std::cout << "[LOG Event ]";
        break;
      case DEB:
        std::cout << "[LOG DEBUG ]";
        break;
      default:
        std::cout << "[LOG INFO  ]";
        break;
    }

    // Apply indent
    if (true) {
      switch (level) {
        case FIT:
          std::cout << ": ";
          break;
        case MIN:
          std::cout << ":- ";
          break;
        case SAM:
          std::cout << ":-- ";
          break;
        case REC:
          std::cout << ":--- ";
          break;
        case SIG:
          std::cout << ":---- ";
          break;
        case EVT:
          std::cout << ":----- ";
          break;
        case DEB:
          std::cout << ":------ ";
          break;
        default:
          std::cout << " ";
          break;
      }
    }

    if (Logger::use_colors) std::cout << RESET;

    if (Logger::showtrace) {
      std::cout << " : " << filename << "::" << funct << "[l. " << line
                << "] : ";
    }

    return *(Logger::__LOG_outstream);
  }
}

void SETVERBOSITY(int level) { Logger::log_verb = level; }
void SETERRVERBOSITY(int level) { Logger::err_verb = level; }

void SETVERBOSITY(std::string verb) {
  if (!verb.compare("DEB"))
    Logger::log_verb = -1;
  else if (!verb.compare("QUIET"))
    Logger::log_verb = 0;
  else if (!verb.compare("FIT"))
    Logger::log_verb = 1;
  else if (!verb.compare("MIN"))
    Logger::log_verb = 2;
  else if (!verb.compare("SAM"))
    Logger::log_verb = 3;
  else if (!verb.compare("REC"))
    Logger::log_verb = 4;
  else if (!verb.compare("SIG"))
    Logger::log_verb = 5;
  else if (!verb.compare("EVT"))
    Logger::log_verb = 6;
  else
    Logger::log_verb = std::atoi(verb.c_str());
}

//******************************************
void SETERRVERBOSITY(std::string verb) {
  //******************************************
  std::cout << "Setting ERROR VERB" << std::endl;

  if (!verb.compare("ERRQUIET"))
    Logger::err_verb = 0;
  else if (!verb.compare("FTL"))
    Logger::err_verb = 1;
  else if (!verb.compare("WRN"))
    Logger::err_verb = 2;
  // else Logger::err_verb = GeneralUtils::StrToInt(verb);

  std::cout << "Set error verbosity to : " << Logger::err_verb << std::endl;
  return;
}

/// Set Trace Option
void SETTRACE(bool val) { Logger::showtrace = val; }

// ------ ERROR FUNCTIONS ---------- //
std::ostream& __OUTERR(int level, const char* filename, const char* funct,
                       int line) {
  if (Logger::use_colors) std::cerr << RED;

  switch (level) {
    case FTL:
      std::cerr << "[ERR FATAL ]: ";
      break;
    case WRN:
      std::cerr << "[ERR WARN  ]: ";
      break;
  }

  if (Logger::use_colors) std::cerr << RESET;

  // Allows enable error debugging trace
  if (true or Logger::showtrace) {
    std::cout << filename << "::" << funct << "[l. " << line << "] : ";
  }

  return *(Logger::__ERR_outstream);
}

// ----------- External Logging ----------- //
void SETEXTERNALVERBOSITY(int level) { Logger::external_verb = (level > 0); }

void StopTalking() {
  // Check verbosity set correctly
  if (!Logger::external_verb) return;

  // Only redirect if we're not debugging
  if (Logger::log_verb == (int)DEB) return;

  std::cout.rdbuf(Logger::redirect_stream.rdbuf());
  std::cerr.rdbuf(Logger::redirect_stream.rdbuf());
  shhnuisancepythiaitokay_();
  fflush(stdout);
  fflush(stderr);
  dup2(Logger::silentfd, fileno(stdout));
  dup2(Logger::silentfd, fileno(stderr));
}

void StartTalking() {
  // Check verbosity set correctly
  if (!Logger::external_verb) return;

  std::cout.rdbuf(Logger::default_cout);
  std::cerr.rdbuf(Logger::default_cerr);
  canihaznuisancepythia_();
  fflush(stdout);
  fflush(stderr);
  dup2(Logger::savedstdoutfd, fileno(stdout));
  dup2(Logger::savedstderrfd, fileno(stderr));
}

//******************************************
bool LOG_LEVEL(int level) {
  //******************************************

  if (Logger::log_verb == (int)DEB) {
    return true;
  }

  if (Logger::log_verb < (int)level) {
    return false;
  }

  return true;
}

void SET_TRACE(bool val) { Logger::showtrace = val; }

//******************************************
std::ostream& _LOG(int level, const char* filename, const char* func, int line)
//******************************************
{
  return __OUTLOG(level, filename, func, line);
}

//******************************************
std::ostream& _ERR(int level, const char* filename, const char* func, int line)
//******************************************
{
  if (Logger::use_colors) std::cerr << RED;

  if (Logger::showtrace) {
    std::cout << filename << "::" << func << "[l. " << line << "] : ";
  }

  switch (level) {
    case FTL:
      std::cerr << "[ERR FATAL ]: ";
      break;
    case WRN:
      std::cerr << "[ERR WARN  ] : ";
      break;
  }

  if (Logger::use_colors) std::cerr << RESET;

  return *Logger::__ERR_outstream;
}
