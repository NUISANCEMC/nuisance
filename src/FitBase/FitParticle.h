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

#ifndef FITPARTICLE_H_SEEN
#define FITPARTICLE_H_SEEN

#include "TLorentzVector.h"
#include "TObject.h"

#ifdef __NEUT_ENABLED__
#include "neutpart.h"
#include "neutfsipart.h"
#endif

#ifdef __NUWRO_ENABLED__
#include "particle.h"
#endif

#ifdef __GENIE_ENABLED__
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepRecord.h"
#include "Ntuple/NtpMCEventRecord.h"
#include "GHEP/GHepParticle.h"
#include "PDG/PDGCodes.h"
#include "PDG/PDGUtils.h"
#include "GHEP/GHepStatus.h"
#include "GHEP/GHepFlags.h"
#include "GHEP/GHepUtils.h"
#endif

#include "StdHepEvt.h"

/*!
 *  \addtogroup FitBase
 *  @{
 */

enum particle_state{
  kUndefinedState = 5,
  kInitialState   = 0,
  kFSIState       = 1,
  kFinalState     = 2,
  kNuclearInitial = 3,
  kNuclearRemnant = 4
};

//! Condensed FitParticle class which acts a common format between the generators
class FitParticle {

  public:
  FitParticle(double x, double y, double z, double t, int pdg, Int_t state);

  FitParticle(){};
  //! Virtual Destructor
  ~FitParticle(){};

 
  TLorentzVector fP; //!< Particle 4 Momentum
  int fPID; //!< Particle PDG Code
  int fIsAlive; //!< Whether the particle is alive at the end of the event (Yes 1, No 0, Other? -1)
  int fNEUTStatusCode; //!< Particle Status (Incoming 1, FSI 2, Outgoing 0, Other 3)
  double fMass; //!< Particle Mass
 

  inline int  Status (void) const { return fStatus; };
  inline int  PDG    (void) const { return fPID;    };
  inline bool IsFinalState   (void) const { return (fStatus == kFinalState);   };
  inline bool IsFSIState     (void) const { return (fStatus == kFSIState);     };
  inline bool IsInitialState (void) const { return (fStatus == kInitialState); };

 private:
  int fStatus;


};

/*! @} */
#endif
