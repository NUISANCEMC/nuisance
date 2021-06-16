// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "FitLogger.h"

/// namespace to contain all physical constants used by NUISANCE
namespace PhysConst {
const double mass_proton = 0.93827203;   // Proton mass in GeV
const double mass_neutron = 0.93956536;  // Neutron mass in GeV
const double mass_nucleon = (mass_proton + mass_neutron) / 2.;
const double mass_proton_kg = 1.6727E-27;   // Proton mass in kg
const double mass_neutron_kg = 1.6750E-27;  // Neutron mass in kg
const double mass_nucleon_kg = (mass_proton_kg + mass_neutron_kg) / 2.;
const double mass_delta = 1.232;              // Delta mass in GeV
const double mass_muon = 0.10565837;          // Muon mass in GeV
const double mass_electron = 0.000510998928;  // Electron mass in GeV
const double mass_cpi = 0.13957;              // charged pion mass in GeV
const double mass_pi0 = 0.13498;              // neutral pion mass in GeV
const double mass_cK = 0.493677;              // charged kaon mass in GeV
const double mass_K0 = 0.497611;              // neutral kaon mass in GeV
const double mass_411 = 1.86961;              // D+ meson mass in GeV                                                                                                       
const double mass_421 = 1.86484;              // D0 meson mass in GeV
const double mass_431 = 1.96830;              // D+_s meson mass in GeV
const double mass_3112 = 1.197449;            // Sigma- baryon mass in GeV
const double mass_3122 = 1.115683;            // Lambda0 baryon mass in GeV
const double mass_3212 = 1.192642;            // Sigma0 baryon mass in GeV
const double mass_3222 = 1.18937;             // Sigma+ baryon mass in GeV
const double mass_3312 = 1.32171;             // Xi- baryon mass in GeV
const double mass_3322 = 1.31486;             // Xi0 baryon mass in GeV
// const double mass_ = ;                // mass in GeV                                                                                                                    
const double mass_4112 = 2.45374;             // Sigma0_c baryon mass in GeV
const double mass_4122 = 2.28626;             // Lambda_c^+ baryon mass in GeV
const double mass_4222 = 2.45398;             // Sigma++_c baryon mass in GeV


inline double GetMass(int pdg) {
  switch (abs(pdg)) {
    case 11:
      return mass_electron;
    case 13:
      return mass_muon;
    case 111:
      return mass_pi0;
    case 211:
      return mass_cpi;
    case 321:
      return mass_cK;
    case 311:
    case 310:
    case 130:
      return mass_K0;
    case 2112:
      return mass_neutron;
    case 2212:
      return mass_proton;
    case 411:
      return mass_411;
    case 421:
      return mass_421;
    case 431:
      return mass_431;
    case 3112:
      return mass_3112;
    case 3122:
      return mass_3122;
    case 3212:
      return mass_3212;
    case 3222:
      return mass_3222;
    case 3312:
      return mass_3312;
    case 3322:
      return mass_3322;
    case 4112:
      return mass_4112;
    case 4122:
      return mass_4122;
    case 4222:
      return mass_4222;
    default: {
      NUIS_ERR(WRN, "Attempted to get mass for PDG: "
                     << pdg << ", but it is not catered for. Please add it to "
                               "src/Utils/PhysConst.h");
      return -1;
    }
  }
}

const double mass_MeV = 1000;  // MeV/GeV

const int pdg_neutrinos[] = {12, -12, 14, -14 /*, 16, -16*/};
const int pdg_muons[] = {13, -13};
const int pdg_leptons[] = {11, -11, 13, -13, 15, -15};
const int pdg_all_leptons[] = {11, -11, 13, -13, 15, -15,
                               12, -12, 14, -14, 16, -16};

const int pdg_pions[] = {211, -211, 111};
const int pdg_charged_pions[] = {211, -211};
const int pdg_strangemesons[] = {
    130,     310,     311,     321,     9000311, 9000321, 10311,
    10321,   100311,  100321,  9010311, 9010321, 9020311, 9020321,
    313,     323,     10313,   10323,   20313,   20323,   100313,
    100323,  9000313, 9000323, 30313,   30323,   315,     325,
    9000315, 9000325, 10315,   10325,   20315,   20325,   9010315,
    9010325, 9020315, 9020325, 317,     327,     9010317, 9010327};

const int pdg_nucleons[] = {2112, 2212};

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
