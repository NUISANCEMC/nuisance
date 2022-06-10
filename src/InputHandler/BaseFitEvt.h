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
#ifndef FITEVENTBASE_H_SEEN
#define FITEVENTBASE_H_SEEN
/*!
 *  \addtogroup InputHandler
 *  @{
 */

#ifdef NEUT_ENABLED
#include "neutpart.h"
#include "neutvect.h"
#endif

#ifdef NuWro_ENABLED
#include "event1.h"
#endif

#ifdef GENIE_ENABLED
#ifdef GENIE3_API_ENABLED
#include "Framework/EventGen/EventRecord.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/Ntuple/NtpMCEventRecord.h"
#else
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepRecord.h"
#include "Ntuple/NtpMCEventRecord.h"
#endif
using namespace genie;
#endif

#ifdef NUANCE_ENABLED
#include "NuanceEvent.h"
#endif

#include "StdHepEvt.h"
#include "SplineReader.h"
#include "InputTypes.h"
#include "GeneratorInfoBase.h"

/// Base Event Class used to store just the generator event pointers
class BaseFitEvt {
 public:

  /// Base Constructor
  BaseFitEvt(void);
  ~BaseFitEvt();

  /// Copy base fit event pointers
  BaseFitEvt(const BaseFitEvt* obj);
  BaseFitEvt(BaseFitEvt const &);
  BaseFitEvt operator=(BaseFitEvt const &);

  /// Reset weight to 1.0
  void ResetWeight();

  /// Return combined weight for this event
  double GetWeight();

  /// Manually set event type
  inline void SetType(int type){fType = type;};

  // Global Event Variables/Weights
  int Mode;  ///< True interaction mode
  double probe_E;  ///< True probe energy
  double probe_pdg;

  // Weighting Info
  double Weight;        ///< Total Weight For Event
  double InputWeight;   ///< Input Starting Weight (used for GiBUU)
  double RWWeight;      ///< Saved RW from FitWeight
  double CustomWeight;  ///< Extra custom weight that samples can set
  double SavedRWWeight; ///< Saved RW value for FitEvents

  double CustomWeightArray[6]; ///< For custom tuning using arrays, e.g. NOvA MINERvA WS

  // Spline Info Coefficients and Readers
  float* fSplineCoeff; ///< ND Array of Spline Coefficients
  SplineReader* fSplineRead; ///< Spline Interpretter

  // Generator Info
  GeneratorInfoBase* fGenInfo; ///< Generator Variable Box
  UInt_t fType; ///< Generator Event Type

#ifdef NEUT_ENABLED
  /// Setup Event Reading from NEUT Event
  void SetNeutVect(NeutVect* v);
  NeutVect* fNeutVect;  ///< Pointer to Neut Vector
#endif

#ifdef NuWro_ENABLED
  event* fNuwroEvent;  ///< Pointer to Nuwro event
#endif
  
#ifdef GENIE_ENABLED
  /// Setup Event Reading from GENIE Event
  void SetGenieEvent(NtpMCEventRecord* ntpl);
  NtpMCEventRecord* genie_event;  ///< Pointer to NTuple Genie Event
  GHepRecord* genie_record;  ///< Pointer to actually accessible Genie Record
#endif

#ifdef NUANCE_ENABLED
  /// Setup Event Reading from NUANCE Event
  void SetNuanceEvent(NuanceEvent* e);
  NuanceEvent* nuance_event; ///< Pointer to Nuance reader
#endif

#ifdef GiBUU_ENABLED
  GiBUUStdHepReader* GiRead; ///< Pointer to GiBUU reader
#endif

  /// Setup Event Type to FitEvent
  void SetInputFitEvent();

  /// Setup Event Type to FitSpline
  void SetInputFitSpline();

  /// Setup Event Type to HepMC
  void SetInputHepMC();

};
/*! @} */
#endif
