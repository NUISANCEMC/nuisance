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
#ifndef PHYSCONST_H_SEEN
#define PHYSCONST_H_SEEN

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iostream>
#include <numeric>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include "FitLogger.h"

/// namespace to contain all physical constants used by NUISANCE
namespace PhysConst {
const double mass_proton = 0.93827203;   // Proton mass in GeV
const double mass_neutron = 0.93956536;  // Neutron mass in GeV
const double mass_delta = 1.232;         // Delta mass in GeV
const double mass_muon = 0.10565837;     // Muon mass in GeV

 const int pdg_neutrinos[] = {12, -12, 14, -14 /*, 16, -16*/};
 const int pdg_muons[] = {13, -13};
 const int pdg_leptons[] = {11, -11, 13, -13, 15, -15};

const int pdg_pions[] = {211, -211, 111};
const int pdg_charged_pions[] = {211, -211};
const int pdg_strangemesons[] = {
    130,     310,     311,     321,     9000311, 9000321, 10311,
    10321,   100311,  100321,  9010311, 9010321, 9020311, 9020321,
    313,     323,     10313,   10323,   20313,   20323,   100313,
    100323,  9000313, 9000323, 30313,   30323,   315,     325,
    9000315, 9000325, 10315,   10325,   20315,   20325,   9010315,
    9010325, 9020315, 9020325, 317,     327,     9010317, 9010327};

// Just *-1 to cover possibility
const int pdg_kplus = 321;
const int pdg_antistrangemesons[] = {
    -130,     -310,     -311,     -321,     -9000311, -9000321, -10311,
    -10321,   -100311,  -100321,  -9010311, -9010321, -9020311, -9020321,
    -313,     -323,     -10313,   -10323,   -20313,   -20323,   -100313,
    -100323,  -9000313, -9000323, -30313,   -30323,   -315,     -325,
    -9000315, -9000325, -10315,   -10325,   -20315,   -20325,   -9010315,
    -9010325, -9020315, -9020325, -317,     -327,     -9010317, -9010327};
}

/*! @} */
#endif
