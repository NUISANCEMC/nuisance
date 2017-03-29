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

#ifndef FITEVENTBASE_H_SEEN
#define FITEVENTBASE_H_SEEN

#include "TArrayD.h"
#include "TLorentzVector.h"
#include "TSpline.h"

#include "FitParticle.h"

#ifdef __NEUT_ENABLED__
#include "neutpart.h"
#include "neutvect.h"
#endif

#ifdef __NUWRO_ENABLED__
#include "event1.h"
#endif

#ifdef __GENIE_ENABLED__
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepRecord.h"
#include "Ntuple/NtpMCEventRecord.h"
using namespace genie;
#endif

#ifdef __NUANCE_ENABLED__
#include "NuanceEvent.h"
#endif

#include "TArrayD.h"
#include "SplineReader.h"
#include "InputTypes.h"
#include "GeneratorInfoBase.h"

/*!
 *  \addtogroup FitBase
 *  @{
 */

//! Base Event Class used to store just the generator event pointers and flat
//! variables
class BaseFitEvt {
 public:
  BaseFitEvt();
  ~BaseFitEvt();
  BaseFitEvt(const BaseFitEvt* obj);

  void ResetWeight();
  double GetWeight();

  inline void SetType(int type){fType = type;};
  
  int Mode;
  double E;
  
  double Weight;
  double InputWeight;
  double RWWeight;
  double CustomWeight;

  UInt_t fType;

// NEUT : Default
#ifdef __NEUT_ENABLED__
  NeutVect* fNeutVect;  //!< Pointer to Neut Vector
#endif

// NUWRO
#ifdef __NUWRO_ENABLED__
  event* fNuwroEvent;  //!< Pointer to Nuwro event
#endif

// GENIE
#ifdef __GENIE_ENABLED__
  NtpMCEventRecord* genie_event;  //!< Pointer to NTuple Genie Event
  GHepRecord* genie_record;  //!< Pointer to actually accessible Genie Record
#endif


// NUANCE
#ifdef __NUANCE_ENABLED__
  NuanceEvent* nuance_event;
#endif

// GiBUU
#ifdef __GiBUU_ENABLED__
  GiBUUStdHepReader* GiRead;
#endif

  GeneratorInfoBase* fGenInfo;
  float* fSplineCoeff;
  SplineReader* fSplineRead;

  double SavedRWWeight;
};

#endif
