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
#ifndef FITLOGGER_HPP
#define FITLOGGER_HPP
/*!
 *  \addtogroup FitBase
 *  @{
 */

#include <fstream>
#include <iosfwd>
#include <iostream>
#include <sstream>
#include "FitParameters.h"
#include "Initialiser.h"
#include "TRandom3.h"

#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

namespace Logger {
extern int log_verb;  //!< Current VERBOSITY
extern int err_verb;  //!< Current ERROR VERBOSITY
extern bool external_verb;
extern bool use_colors;          //!< Use BASH Terminal Colors Flag
extern bool super_rainbow_mode;  //!< For when fitting gets boring.
extern unsigned int super_rainbow_mode_colour;

extern bool showtrace;  // Quick Tracing for debugging
extern int nloggercalls;
extern int timelastlog;
extern std::streambuf*
    default_cout;  //!< Where the STDOUT stream is currently directed
extern std::streambuf*
    default_cerr;  //!< Where the STDERR stream is currently directed
extern std::ofstream
    redirect_stream;  //!< Where should unwanted messages be thrown
}

/// Returns full path to file currently in
#define __FILENAME__ \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// ------ LOGGER FUNCTIONS ------------ //
namespace Logger {
/// NULL Output Stream
extern std::ofstream __LOG_nullstream;

/// Logging Stream
extern std::ostream* __LOG_outstream;
}

/// Fitter VERBOSITY Enumerations
/// These go through the different depths of the fitter.
///
/// 0 QUIET - Little output.
/// 1 FIT - Top Level Minimizer Status
/// 2 MIN - Output from the FCN Minimizer Functions
/// 3 SAM - Output from each of the samples during setup etc
/// 4 REC - Output during each reconfigure. Percentage progress etc.
/// 5 SIG - Output during every signal event that is found.
/// 6 EVT - Output during every event.
/// -1 DEB - Will print only debugging info wherever a LOG(DEB) statement was
/// made
enum __LOG_levels { DEB = -1, QUIET, FIT, MIN, SAM, REC, SIG, EVT };

/// Returns log level for a given file/function
int __GETLOG_LEVEL(int level, const char* filename, const char* funct);

/// Actually runs the logger
std::ostream& __OUTLOG(int level, const char* filename, const char* funct,
                       int line);

/// Global Logging Definitions
#define QLOG(level, stream)                                               \
  {                                                                       \
    if (Logger::log_verb >=                                               \
        __GETLOG_LEVEL(level, __FILENAME__, __FUNCTION__)) {              \
      __OUTLOG(level, __FILENAME__, __FUNCTION__, __LINE__) << stream     \
                                                            << std::endl; \
    }                                                                     \
  };

#define BREAK(level)                                                      \
  {                                                                       \
    \ if (Logger::log_verb >=                                             \
          __GETLOG_LEVEL(level, __FILENAME__, __FUNCTION__)) {            \
      __OUTLOG(level, __FILENAME__, __FUNCTION__, __LINE__) << std::endl; \
    }                                                                     \
  };

/// Return whether logging level is valid
bool LOGGING(int level);

/// Set Global Verbosity
void SETVERBOSITY(int level);

/// Set Global Verbosity from String
void SETVERBOSITY(std::string verb);

/// Set Trace Option
void SETTRACE(bool val);

// ----------- ERROR FUNCTIONS ---------- //

/// Error Stream
extern std::ostream* __ERR_outstream;

/// Fitter ERROR VERBOSITY Enumerations
///
/// 0 QUIET - No Error Output
/// 1 FTL - Show errors only if fatal
/// 2 WRN - Show Warning messages
enum __ERR_levels { ERRQUIET = 0, FTL, WRN };

/// Actually runs the error messager
std::ostream& __OUTERR(int level, const char* filename, const char* funct,
                       int line);

/// Error Logging Function
#define ERROR(level, stream)                                            \
  {                                                                     \
    __OUTERR(level, __FILENAME__, __FUNCTION__, __LINE__) << stream     \
                                                          << std::endl; \
  };

// ----------- ERROR HANDLING ------------- //
/// Exit the program with given error message stream
#define THROW(stream)                                                   \
  {                                                                     \
    __OUTERR(FTL, __FILENAME__, __FUNCTION__, __LINE__) << stream       \
                                                        << std::endl;   \
    __OUTERR(FTL, __FILENAME__, __FUNCTION__, __LINE__)                 \
        << "Attempting to save output file." << std::endl;              \
    if (FitPar::Config().out && FitPar::Config().out->IsOpen()) {       \
      FitPar::Config().out->Write();                                    \
      FitPar::Config().out->Close();                                    \
      __OUTERR(FTL, __FILENAME__, __FUNCTION__, __LINE__) << "Done."    \
                                                          << std::endl; \
    } else {                                                            \
      __OUTERR(FTL, __FILENAME__, __FUNCTION__, __LINE__)               \
          << "No output file set." << std::endl;                        \
    }                                                                   \
    __OUTERR(FTL, __FILENAME__, __FUNCTION__, __LINE__) << "Exiting!"   \
                                                        << std::endl;   \
    std::abort();                                                       \
  }

// ----------- External Logging ----------- //
void SETEXTERNALVERBOSITY(int level);

void StopTalking();
void StartTalking();

extern "C" {
void shhnuisancepythiaitokay_(void);
void canihaznuisancepythia_(void);
}

// ---------- LEGACY FUNCTIONS -------------- //

bool LOG_LEVEL(int level);

//! Set LOG VERBOSITY from a string
void LOG_VERB(std::string verb);
inline void LOG_VERB(int verb) { Logger::log_verb = verb; };

void SET_TRACE(bool val);

//! Set ERROR VERBOSITY from a string
void ERR_VERB(std::string verb);
inline void ERR_VERB(int verb) { Logger::err_verb = verb; };

/// Logging Function. Use as a string stream.  e.g. LOG(SAM) << "This sample is
/// dope." << std::endl;
std::ostream& _LOG(int level, const char* filename, const char* funct,
                   int line);
#define LOG(level) _LOG(level, __FILENAME__, __FUNCTION__, __LINE__)

//! Error Function. Use as a string stream.  e.g. ERR(FTL) << "The fit is
//! completely buggered." << std::endl;
std::ostream& _ERR(int level, const char* filename, const char* funct,
                   int line);
#define ERR(level) _ERR(level, __FILENAME__, __FUNCTION__, __LINE__)

/*! @} */
#endif
