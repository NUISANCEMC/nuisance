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
#ifndef GENERATOR_UTILS_H
#define GENERATOR_UTILS_H
/*!
 *  \addtogroup InputHandler
 *  @{
 */

#ifdef __NIWG_ENABLED__
#ifdef __NEUT_ENABLED__
#include "NIWGEvent.h"
#include "NIWGSyst.h"
#include "InputHandler.h"
#include "TargetUtils.h"

#include "TTreePerfStats.h"
#include "PlotUtils.h"


/*
#include "neutpart.h"
#include "neutvect.h"
#include "nefillverC.h"
#include "necardC.h"
#include "neutmodelC.h"
#include "neutparamsC.h"
#include "neworkC.h"
#include "fsihistC.h"
#include "neutcrsC.h"
#include "neutvect.h"
#include "neutpart.h"
#include "neutfsipart.h"
#include "neutfsivert.h"
#include "neutrootTreeSingleton.h"
#include "NModeDefn.h"
#include "NSyst.h"
#include "NFortFns.h" // Contains all the NEUT common blocks                                                                                                                                                                                
#ifdef __NEUT_NUCFSI_ENABLED__
#include "nucleonfsihistC.h"
#include "neutnucfsivert.h"
#include "neutnucfsistep.h"
#endif
*/
#include "neutvect.h"

#endif
#endif

// namespace GeneratorUtils {
// #ifdef __NIWG_ENABLED__
// #ifdef __NEUT_ENABLED__
//   niwg::rew::NIWGEvent* GetNIWGEventLocal(NeutVect* nvect);
// #endif
// #endif
// };
/*! @} */
#endif
