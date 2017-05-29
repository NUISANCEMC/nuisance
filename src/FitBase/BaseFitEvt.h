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

#include "SplineReader.h"
#include "InputTypes.h"
#include "GeneratorInfoBase.h"

/*!
 *  \addtogroup FitBase
 *  @{
 */

/// Base Event Class used to store just the generator event pointers 
class BaseFitEvt {
 public:

  // Base Constructors
  BaseFitEvt(void);
  ~BaseFitEvt();
  BaseFitEvt(const BaseFitEvt* obj);

  /// Reset weight to 1.0
  void ResetWeight();
  double GetWeight();

  inline void SetType(int type){fType = type;};
  
  // Global Event Variables/Weights
  int Mode;  ///< True interaction mode
  double E;  ///< True probe energy
  
  // Weighting Info
  double Weight;        ///< Total Weight For Event
  double InputWeight;   ///< Input Starting Weight (used for GiBUU)
  double RWWeight;      ///< Saved RW from FitWeight
  double CustomWeight;  ///< Extra custom weight that samples can set
  double SavedRWWeight; ///< Saved RW value for FitEvents

  // Spline Info Coefficients and Readers
  float* fSplineCoeff; ///< ND Array of Spline Coefficients
  SplineReader* fSplineRead; ///< Spline Interpretter


  // Generator level pointers.
  GeneratorInfoBase* fGenInfo;
  UInt_t fType;

#ifdef __NEUT_ENABLED__
  void SetNeutVect(NeutVect* v);
  NeutVect* fNeutVect;  ///< Pointer to Neut Vector
#endif

#ifdef __NUWRO_ENABLED__
  void SetNuwroEvent(event* e);
  event* fNuwroEvent;  ///< Pointer to Nuwro event
#endif

#ifdef __GENIE_ENABLED__
  void SetGenieEvent(NtpMCEventRecord* ntpl);
  NtpMCEventRecord* genie_event;  ///< Pointer to NTuple Genie Event
  GHepRecord* genie_record;  ///< Pointer to actually accessible Genie Record
#endif

#ifdef __NUANCE_ENABLED__
  void SetNuanceEvent(NuanceEvent* e);
  NuanceEvent* nuance_event; ///< Pointer to Nuance reader
#endif

#ifdef __GiBUU_ENABLED__
  void SetGiBUUReader(GiBUUStdHepReader* g);
  GiBUUStdHepReader* GiRead; ///< Pointer to GiBUU reader
#endif

  void SetInputFitEvent();
  void SetInputFitSpline();
  void SetInputHepMC();

};

#endif
