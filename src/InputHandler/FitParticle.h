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
#ifndef FITPARTICLE_H_SEEN
#define FITPARTICLE_H_SEEN
/*!
 *  \addtogroup InputHandler
 *  @{
 */

#include "TLorentzVector.h"

#include <iostream>

/// Partle state flags for its position in the event
enum particle_state{
  kUndefinedState = 5,
  kInitialState   = 0,
  kFSIState       = 1,
  kFinalState     = 2,
  kNuclearInitial = 3,
  kNuclearRemnant = 4,
};

/// Condensed FitParticle class which acts a common format between the generators
class FitParticle {
  public:

  /// Create particle of given pdg from momentum variables and state
  FitParticle(double x, double y, double z, double t, int pdg, Int_t state);

  /// Create empty particle (zero momentum)
  FitParticle(){};
  ~FitParticle(){};

  /// Used to change values after creation
  void SetValues(double x, double y, double z, double t, int pdg, Int_t state);

  /// Return Status Code according to particle_state enum
  inline int  Status (void) const { return fStatus; };

  /// Get Particle PDF
  inline int  PDG    (void) const { return fPID;    };

  /// Check if particle makes it to final state
  inline bool IsFinalState   (void) const { return (fStatus == kFinalState);   };

  /// Was particle involved in intermediate state
  inline bool IsFSIState     (void) const { return (fStatus == kFSIState);     };

  /// Was particle incoming
  inline bool IsInitialState (void) const { return (fStatus == kInitialState); };

  /// Get Mass
  inline double M  (void){ return fP.Mag(); };

  /// Get Kinetic Energy
  inline double KE (void){ return fP.E() - fP.Mag(); };

  /// Get Total Energy
  inline double E  (void){ return fP.E(); };

  /// Get 4 Momentum
  inline TLorentzVector P4(void) {return fP;};

  /// Get 3 Momentum
  inline TVector3       P3(void) {return fP.Vect();};

  /// Get 3 momentum magnitude
  inline double         p(void) { return fP.Vect().Mag(); };

  /// Get 3 momentum magnitude squared
  inline double         p2(void) { return fP.Vect().Mag2(); };

  /// Data Members
  TLorentzVector fP;   ///< Particle 4 Momentum
  int fPID;            ///< Particle PDG Code
  int fIsAlive;        ///< Whether the particle is alive at the end of the event (Yes 1, No 0, Other? -1)
  int fNEUTStatusCode; ///< Particle Status (Incoming 1, FSI 2, Outgoing 0, Other 3)
  double fMass;        ///< Particle Mass
  int fStatus;         ///< State corresponding to particle_state enum
  bool fIsPrimary;     ///< Primary target
};

inline std::ostream& operator<<(std::ostream& os, FitParticle const& p){

  return os << " Particle[pdgc:" << p.fPID
	    << ", stat:"<<p.fStatus
	    << ", 4mom:("<< p.fP.X() << "," << p.fP.Y() << "," << p.fP.Z() << "," << p.fP.T() << ")]";

}



/*! @} */
#endif
