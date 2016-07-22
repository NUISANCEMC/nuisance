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

#ifndef GENERATOR_UTILS_H
#define GENERATOR_UTILS_H

#ifdef __NEUT_ENABLED__
#include "nefillverC.h"
#include "necardC.h"
#include "neutmodelC.h"
#include "neutparamsC.h"
#include "neworkC.h"
#include "fsihistC.h"
//Comment out as NEUT does not have the necessary proton FSI information yet
#include "nucleonfsihistC.h"
#include "neutcrsC.h"
#include "neutvect.h"
#include "neutpart.h"
#include "neutfsipart.h"
#include "neutfsivert.h"
#include "neutnucfsivert.h"
#include "neutnucfsistep.h"
#include "neutrootTreeSingleton.h"
#include "NModeDefn.h"

#include "NSyst.h"
#include "NFortFns.h" // Contains all the NEUT common blocks
#endif

#ifdef __NIWG_ENABLED__
#include "NIWGEvent.h"
#include "NIWGSyst.h"
#endif

#ifdef __NUWRO_ENABLED__
#include "event1.h"
#endif

#include "NuanceEvent.h"

namespace GeneratorUtils {

#ifdef __NEUT_ENABLED__
  void FillNeutCommons(NeutVect* nvect);
#endif

#ifdef __NIWG_ENABLED__
  niwg::rew::NIWGEvent* GetNIWGEvent(NeutVect* nvect);
#endif

#ifdef __NUWRO_ENABLED__
  int ConvertNuwroMode (event * e);
#endif

#ifdef __NUANCE_ENABLED__
  int ConvertNuanceMode(NuanceEvent * evt);
#endif
};
#endif
