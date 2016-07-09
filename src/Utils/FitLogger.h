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

#ifndef FITLOGGER_HPP
#define FITLOGGER_HPP

// We still need a forward declaration of 'ostream' in order to
// swallow templated manipulators such as 'endl'.
#include <iosfwd>
#include <iostream>
#include <fstream>
#include <sstream>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

/*!                                                                                                                                                                                                   
 *  \addtogroup FitBase                                                                                                                                                                               
 *  @{                                                                                                                                                                                              
 */

namespace FitPar{
  
  unsigned int log_verb = 5; //!< Current VERBOSITY
  unsigned int err_verb = 3; //!< Current ERROR VERBOSITY
  bool use_colors = true;    //!< Use BASH Terminal Colors Flag
  bool super_rainbow_mode = false; //!< For when fitting gets boring.
  unsigned int super_rainbow_mode_colour = 0; 
}


std::ostream* logStream(&std::cout);
std::ostream* errStream(&std::cerr);
std::ofstream nullStream;

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
/// -1 DEB - Will print only debugging info wherever a LOG(DEB) statement was made   
enum log_levels { DEB=-1, QUIET, FIT, MIN, SAM, REC, SIG, EVT };

/// Fitter ERROR VERBOSITY Enumerations
///
/// 0 QUIET - No Error Output
/// 1 FTL - Show errors only if fatal
/// 2 WRN - Show Warning messages
enum err_levels { ERRQUIET=0, FTL, WRN };

bool LOG_LEVEL(int level);

//! Set LOG VERBOSITY from a string
void LOG_VERB(std::string verb);

//! Set ERROR VERBOSITY from a string
void ERR_VERB(std::string verb);

/// Logging Function. Use as a string stream.  e.g. LOG(SAM) << "This sample is dope." << std::endl;
std::ostream& LOG(int level);

//! Error Function. Use as a string stream.  e.g. ERR(FTL) << "The fit is completely buggered." << std::endl;          
std::ostream& ERR(int level);

/*! @} */
#endif // FILELOGGER_HPP

