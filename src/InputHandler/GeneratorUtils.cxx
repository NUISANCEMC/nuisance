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

#include <iostream>

#include "GeneratorUtils.h"
#include "FitLogger.h"

#ifdef NIWG_ENABLED
#ifdef NEUT_ENABLED

// niwg::rew::NIWGEvent * GeneratorUtils::GetNIWGEventLocal(NeutVect* nvect)
// {
//   niwg::rew::NIWGEvent * fDummyNIWGEvent = NULL;

//   fDummyNIWGEvent = new niwg::rew::NIWGEvent();
//   fDummyNIWGEvent->detid = 1;   // MiniBooNE (apply CCQE LowE variations)
//   fDummyNIWGEvent->neutmode = nvect->Mode;
//   fDummyNIWGEvent->targetA = nvect->TargetA;
//   fDummyNIWGEvent->recenu_ccqe_sk = -1;
//   if (nvect->Ibound==0) fDummyNIWGEvent->targetA = 1;//RT: identifies as H, rather than O16

//   // Fill initial particle stack
//   for (int ip=0; ip<nvect->Npart(); ++ip) {

//     niwg::rew::NIWGPartStack fDummyPartStack;

//     fDummyPartStack.p = (nvect->PartInfo(ip)->fP)*0.001;  // Convert to GeV

//     fDummyPartStack.pdg = nvect->PartInfo(ip)->fPID;
//     fDummyPartStack.chase = nvect->PartInfo(ip)->fIsAlive;
//     fDummyPartStack.parent = nvect->ParentIdx(ip)-1;       // WARNING: this needs to be tested with a NeutRoot file

//     fDummyNIWGEvent->part_stack.push_back(fDummyPartStack);
//   }
//   fDummyNIWGEvent->CalcKinematics();

//   return fDummyNIWGEvent;
// }
#endif 
#endif



