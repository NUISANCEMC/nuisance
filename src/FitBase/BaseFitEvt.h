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

#ifndef FITEVENTBASE_H_SEEN
#define FITEVENTBASE_H_SEEN

#include "TLorentzVector.h"
#include "TSpline.h"
#include "TArrayD.h"

#include "FitParticle.h"

#ifdef __NEUT_ENABLED__
#include "neutvect.h"
#include "neutpart.h"
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

/*!
 *  \addtogroup FitBase
 *  @{
 */

//! Global Enum to define generator type being read with FitEvent
// Have to define kNORM as if its a generator for the time being.
enum generator_event_type { kUNKNOWN=999, kNEUT=0, kNIWG=1, kNUWRO=2, kT2K=3, kCUSTOM=4, kGENIE=5, kEVTSPLINE=6, kNUANCE=7, kGiBUU=8, kNORM=9};

//! Base Event Class used to store just the generator event pointers and flat variables
class BaseFitEvt {
 public:

  BaseFitEvt();
  ~BaseFitEvt();
  BaseFitEvt(const BaseFitEvt* obj);

  double X_VAR;
  double Y_VAR;
  double Z_VAR;
  int    Mode;
  double E;
  double Weight;
  double InputWeight;
  double RWWeight;
  double CustomWeight;
  bool   Signal;
  UInt_t Index;
  UInt_t BinIndex;
  UInt_t fType;

  TArrayD*  dial_coeff; // Depedendent on dials (needs header file provided)

  // NEUT : Default
#ifdef __NEUT_ENABLED__
  NeutVect* neut_event; //!< Pointer to Neut Vector
#endif

  // NUWRO
#ifdef __NUWRO_ENABLED__
  event* nuwro_event; //!< Pointer to Nuwro event
#endif

  // GENIE
#ifdef __GENIE_ENABLED__
  NtpMCEventRecord* genie_event; //!< Pointer to NTuple Genie Event
  GHepRecord* genie_record; //!< Pointer to actually accessible Genie Record
#endif

  // NUANCE
#ifdef __NUANCE_ENABLED__
  NuanceEvent* nuance_event;
#endif

  // GiBUU
#ifdef __GiBUU_ENABLED__
  GiBUUStdHepReader* GiRead;
#endif

  double GetWeight(){ return InputWeight * RWWeight * CustomWeight; };
};
#endif




