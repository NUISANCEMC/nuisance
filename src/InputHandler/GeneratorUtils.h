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
#include "NIWGEvent.h"
#include "NIWGSyst.h"
#endif

#ifdef __NEUT_ENABLED__
#include "NEUTInputHandler.h"
#endif

namespace GeneratorUtils {
#if defined(__NIWG_ENABLED__) && defined(__NEUT_ENABLED__)
  niwg::rew::NIWGEvent* GetNIWGEvent(NeutVect* nvect);
#endif

};
/*! @} */
#endif
