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
#ifndef FITEVENT2_H_SEEN
#define FITEVENT2_H_SEEN
/*!
 *  \addtogroup InputHandler
 *  @{
 */

#include <algorithm>
#include <iterator>
#include <vector>
#include "FitParticle.h"
#include "TLorentzVector.h"
#include "TSpline.h"

#include "BaseFitEvt.h"
#include "FitLogger.h"
#include "TArrayD.h"
#include "TTree.h"
#include "TChain.h"
#include "TargetUtils.h"

#include "PhysConst.h"

/// Common container for event particles
class FitEvent : public BaseFitEvt {
public:

  FitEvent();
  ~FitEvent() {};

  void FreeFitParticles();
  void ClearFitParticles();
  void ResetEvent(void);
  void ResetParticleList(void);
  void HardReset();
  void OrderStack();
  void SetBranchAddress(TChain* tn);
  void AddBranchesToTree(TTree* tn);
  void Print();
  void DeallocateParticleStack();
  void AllocateParticleStack(int stacksize);
  void ExpandParticleStack(int stacksize);
  void AddGeneratorInfo(GeneratorInfoBase* gen);


  // ---- HELPER/ACCESS FUNCTIONS ---- //
  /// Return True Interaction ID
  inline int GetMode    (void) const { return Mode;    };
  /// Return Target Atomic Number
  inline int GetTargetA (void) const { return fTargetA; };
  /// Return Target Nuclear Charge
  inline int GetTargetZ (void) const { return fTargetZ; };
  /// Get Event Total Cross-section
  inline int GetTotCrs  (void) const { return fTotCrs;  };

  /// Is Event Charged Current?
  inline bool IsCC() const { if (abs(this->probe_pdg) == 11) return false; return (abs(Mode) <= 30); };
  /// Is Event Neutral Current?
  inline bool IsNC() const { if (abs(this->probe_pdg) == 11) return true; return (abs(Mode) > 30);  };
  // Is Event resonant?
  inline bool IsResonant() const { if (Mode != 11 && Mode != 12 && Mode != 13 && Mode != 31 && Mode != 32 && Mode != 33 && Mode != 34) return false; return true; };

  /// Return Particle 4-momentum for given index in particle stack
  TLorentzVector GetParticleP4    (int index) const;
  /// Return Particle 3-momentum for given index in particle stack
  TVector3       GetParticleP3    (int index) const;
  /// Return Particle absolute momentum for given index in particle stack
  double         GetParticleMom   (int index) const;
  /// Return Particle absolute momentum-squared for given index in particle stack
  double         GetParticleMom2  (int index) const;
  /// Return Particle energy for given index in particle stack
  double         GetParticleE     (int index) const;
  /// Return Particle State for given index in particle stack
  int            GetParticleState (int index) const;
  /// Return Particle PDG for given index in particle stack
  int            GetParticlePDG   (int index) const;


  /// Allows the removal of KE up to total KE.
  inline void RemoveKE(int index, double KE){

    FitParticle *fp = GetParticle(index);

    double mass = fp->M();
    double oKE = fp->KE();
    double nE = mass + (oKE - KE);
    if(nE < mass){ // Can't take more KE than it has
      nE = mass;
    }
    double n3Mom = sqrt(nE*nE - mass*mass);
    TVector3 np3 = fp->P3().Unit()*n3Mom;

    fParticleMom[index][0] = np3[0];
    fParticleMom[index][1] = np3[1];
    fParticleMom[index][2] = np3[2];
    fParticleMom[index][3] = nE;

  }

  /// Allows the removal of KE up to total KE.
  inline void GiveKE(int index, double KE){
    RemoveKE(index,-KE);
  }
  /// Return Particle for given index in particle stack
  FitParticle* GetParticle(int const index);
  /// Get Total Number of Particles in stack
  inline uint  NParticles (void) const { return fNParticles; };

  /// Check if event contains a particle given a pdg and state.
  /// If no state is passed all states are considered.
  bool HasParticle (int const pdg = 0, int const state = -1) const ;

  template <size_t N>
  inline bool HasParticle(int const (&pdgs)[N], int const state = -1) const {
    for (size_t i = 0; i < N; i++) {
      if (HasParticle( pdgs[i], state )) {
        return false;
      }
    }
    return false;
  }

  /// Get total number of particles given a pdg and state.
  /// If no state is passed all states are considered.
  int  NumParticle (int const pdg = 0, int const state = -1) const;

  template <size_t N>
  inline int NumParticle(int const (&pdgs)[N], int const state = -1) const {
    int ncount = 0;
    for (size_t i = 0; i < N; i++) {
      ncount += NumParticle( pdgs[i], state );
    }
    return ncount;
  }

  /// Return a vector of particle indices that can be used with 'GetParticle'
  /// functions given a particle pdg and state.
  /// If no state is passed all states are considered.
  std::vector<int> GetAllParticleIndices (int const pdg = -1, int const state = -1) const;

  template <size_t N>
  inline std::vector<int> GetAllParticleIndices(int const (&pdgs)[N], const int state = -1) const {
    std::vector<int> plist;
    for (size_t i = 0; i < N; i++) {
      std::vector<int> plisttemp = GetAllParticleIndices(pdgs[i], state);
      plist.insert( plist.end(), plisttemp.begin(), plisttemp.end() );
    }
    return plist;
  }

  /// Return a vector of FitParticles given a particle pdg and state.
  /// This is memory intensive and slow than GetAllParticleIndices,
  /// but is slightly easier to use.
  std::vector<FitParticle*> GetAllParticle (int const pdg = -1, int const state = -1);

  template <size_t N>
  inline std::vector<FitParticle*> GetAllParticle(int const (&pdgs)[N], int const state = -1) {
    std::vector<FitParticle*> plist;
    for (size_t i = 0; i < N; i++) {
      std::vector<FitParticle*> plisttemp = GetAllParticle(pdgs[i], state);
      plist.insert( plist.end(), plisttemp.begin(), plisttemp.end() );
    }
    return plist;
  }

  inline std::vector<int> GetAllNuElectronIndices (void) { return GetAllParticleIndices(12);   };
  inline std::vector<int> GetAllNuMuonIndices     (void) { return GetAllParticleIndices(14);   };
  inline std::vector<int> GetAllNuTauIndices      (void) { return GetAllParticleIndices(16);   };
  inline std::vector<int> GetAllElectronIndices   (void) { return GetAllParticleIndices(11);   };
  inline std::vector<int> GetAllMuonIndices       (void) { return GetAllParticleIndices(13);   };
  inline std::vector<int> GetAllTauIndices        (void) { return GetAllParticleIndices(15);   };
  inline std::vector<int> GetAllProtonIndices     (void) { return GetAllParticleIndices(2212); };
  inline std::vector<int> GetAllNeutronIndices    (void) { return GetAllParticleIndices(2112); };
  inline std::vector<int> GetAllPiZeroIndices     (void) { return GetAllParticleIndices(111);  };
  inline std::vector<int> GetAllPiPlusIndices     (void) { return GetAllParticleIndices(211);  };
  inline std::vector<int> GetAllPiMinusIndices    (void) { return GetAllParticleIndices(-211); };
  inline std::vector<int> GetAllPhotonIndices     (void) { return GetAllParticleIndices(22);   };

  inline std::vector<FitParticle*> GetAllNuElectron (void) { return GetAllParticle(12);   };
  inline std::vector<FitParticle*> GetAllNuMuon     (void) { return GetAllParticle(14);   };
  inline std::vector<FitParticle*> GetAllNuTau      (void) { return GetAllParticle(16);   };
  inline std::vector<FitParticle*> GetAllElectron   (void) { return GetAllParticle(11);   };
  inline std::vector<FitParticle*> GetAllMuon       (void) { return GetAllParticle(13);   };
  inline std::vector<FitParticle*> GetAllTau        (void) { return GetAllParticle(15);   };
  inline std::vector<FitParticle*> GetAllProton     (void) { return GetAllParticle(2212); };
  inline std::vector<FitParticle*> GetAllNeutron    (void) { return GetAllParticle(2112); };
  inline std::vector<FitParticle*> GetAllPiZero     (void) { return GetAllParticle(111);  };
  inline std::vector<FitParticle*> GetAllPiPlus     (void) { return GetAllParticle(211);  };
  inline std::vector<FitParticle*> GetAllPiMinus    (void) { return GetAllParticle(-211); };
  inline std::vector<FitParticle*> GetAllPhoton     (void) { return GetAllParticle(22);   };

  // --- Highest Momentum Search Functions --- //
  /// Returns the Index of the highest momentum particle given a pdg and state.
  /// If no state is given all states are considered, but that will just return the
  /// momentum of the beam in most cases so is not advised.
  int  GetHMParticleIndex (int const pdg = 0, int const state = -1) const;

  template <size_t N>
  inline int GetHMParticleIndex (int const (&pdgs)[N], int const state = -1) const {

    double mom = -999.9;
    int rtnindex = -1;

    for (size_t i = 0; i < N; ++i) {
      // Use ParticleMom as doesn't require Particle Mem alloc
      int pindex = GetHMParticleIndex(pdgs[i], state);
      if (pindex != -1){
	double momnew = GetParticleMom2(pindex);
	if (momnew > mom) {
	  rtnindex = pindex;
	  mom = momnew;
	}
      }
    }
    return rtnindex;
  };

  // --- Second highest Momentum Search Functions --- //
  /// Returns the Index of the second highest momentum particle given a pdg and state.
  /// If no state is given all states are considered, but that will just return the
  /// momentum of the beam in most cases so is not advised.
  int  GetSHMParticleIndex (int const pdg = 0, int const state = -1) const;

  template <size_t N>
  inline int GetSHMParticleIndex (int const (&pdgs)[N], int const state = -1) const {

    double leadmom = -999.9;
    int leadrtnindex = -1;

    double recoilmom = -999.9;
    int recoilrtnindex = -1;

    for (size_t i = 0; i < N; ++i) {
      // Use ParticleMom as doesn't require Particle Mem alloc
      int pindex = GetSHMParticleIndex(pdgs[i], state);
      if (pindex != -1){
	double leadmomnew = GetParticleMom2(pindex);
	if (leadmomnew > leadmom) {

	  recoilrtnindex = leadrtnindex;
	  recoilmom = leadmomnew;

	  leadrtnindex = pindex;
	  leadmom = leadmomnew;

	}

	if (leadmomnew < leadmom && leadmomnew > recoilmom) {

	  recoilrtnindex = i;
	  recoilmom = leadmomnew;

	}


      }
    }

    if (recoilrtnindex == -1) { recoilrtnindex = leadrtnindex; }
    return recoilrtnindex;
  };

  /// Returns the highest momentum particle given a pdg and state.
  /// If no state is given all states are considered, but that will just return the
  /// momentum of the beam in most cases so is not advised.
  inline FitParticle* GetHMParticle(int const pdg = 0, int const state = -1) {
    return GetParticle( GetHMParticleIndex(pdg, state) );
  }

  template <size_t N>
  inline FitParticle* GetHMParticle(int const (&pdgs)[N], int const state) {
    return GetParticle(GetHMParticleIndex(pdgs, state));
  };

  /// Returns the second highest momentum particle given a pdg and state.
  inline FitParticle* GetSHMParticle(int const pdg = 0, int const state = -1) {
    return GetParticle( GetSHMParticleIndex(pdg, state) );
  }

  template <size_t N>
  inline FitParticle* GetSHMParticle(int const (&pdgs)[N], int const state) {
    return GetParticle(GetSHMParticleIndex(pdgs, state));
  };


  // ---- Initial State Helpers --- //
  /// Checks the event has a particle of a given pdg in the initial state.
  inline bool HasISParticle(int const pdg) const {
    return HasParticle(pdg, kInitialState);
  };
  template <size_t N>
  inline bool HasISParticle(int const (&pdgs)[N]) const {
    return HasParticle(pdgs, kInitialState);
  };

  /// Returns the number of particles with a given pdg in the initial state.
  inline int NumISParticle(int const pdg = 0) const {
    return NumParticle(pdg, kInitialState);
  };
  template <size_t N>
  inline int NumISParticle(int const (&pdgs)[N]) const {
    return NumParticle(pdgs, kInitialState);
  };

  /// Returns a list of indices for all particles with a given pdg
  /// in the initial state. These can be used with the 'GetParticle' functions.
  inline std::vector<int> GetAllISParticleIndices(int const pdg = -1) const {
    return GetAllParticleIndices(pdg, kInitialState);
  };
  template <size_t N>
  inline std::vector<int> GetAllISParticleIndices(int const (&pdgs)[N]) const {
    return GetAllParticleIndices(pdgs, kInitialState);
  };

  /// Returns a list of particles with a given pdg in the initial state.
  /// This function is more memory intensive and slower than the Indices function.
  inline std::vector<FitParticle*> GetAllISParticle(int const pdg = -1) {
    return GetAllParticle(pdg, kInitialState);
  };
  template <size_t N>
  inline std::vector<FitParticle*> GetAllISParticle(int const (&pdgs)[N]) {
    return GetAllParticle(pdgs, kInitialState);
  };

  /// Returns the highest momentum particle with a given pdg in the initial state.
  inline FitParticle* GetHMISParticle(int const pdg) {
    return GetHMParticle(pdg, kInitialState);
  };
  template <size_t N>
  inline FitParticle* GetHMISParticle(int const (&pdgs)[N]) {
    return GetHMParticle(pdgs, kInitialState);
  };

  /// Returns the highest momentum particle index with a given pdg in the initial state.
  inline int GetHMISParticleIndex(int const pdg) const {
    return GetHMParticleIndex(pdg, kInitialState);
  };
  template <size_t N>
  inline int GetHMISParticleIndex(int const (&pdgs)[N]) const {
    return GetHMParticleIndex(pdgs, kInitialState);
  };

  inline bool HasISNuElectron   (void) const { return HasISParticle(12);   };
  inline bool HasISNuMuon       (void) const { return HasISParticle(14);   };
  inline bool HasISNuTau        (void) const { return HasISParticle(16);   };
  inline bool HasISElectron     (void) const { return HasISParticle(11);   };
  inline bool HasISMuon         (void) const { return HasISParticle(13);   };
  inline bool HasISTau          (void) const { return HasISParticle(15);   };
  inline bool HasISProton       (void) const { return HasISParticle(2212); };
  inline bool HasISNeutron      (void) const { return HasISParticle(2112); };
  inline bool HasISPiZero       (void) const { return HasISParticle(111);  };
  inline bool HasISPiPlus       (void) const { return HasISParticle(211);  };
  inline bool HasISPiMinus      (void) const { return HasISParticle(-211); };
  inline bool HasISPhoton       (void) const { return HasISParticle(22);   };
  inline bool HasISLeptons      (void) const { return HasISParticle(PhysConst::pdg_leptons);       };
  inline bool HasISPions        (void) const { return HasISParticle(PhysConst::pdg_pions);         };
  inline bool HasISChargePions  (void) const { return HasISParticle(PhysConst::pdg_charged_pions); };

  inline int NumISNuElectron   (void) const { return NumISParticle(12);   };
  inline int NumISNuMuon       (void) const { return NumISParticle(14);   };
  inline int NumISNuTau        (void) const { return NumISParticle(16);   };
  inline int NumISElectron     (void) const { return NumISParticle(11);   };
  inline int NumISMuon         (void) const { return NumISParticle(13);   };
  inline int NumISTau          (void) const { return NumISParticle(15);   };
  inline int NumISProton       (void) const { return NumISParticle(2212); };
  inline int NumISNeutron      (void) const { return NumISParticle(2112); };
  inline int NumISPiZero       (void) const { return NumISParticle(111);  };
  inline int NumISPiPlus       (void) const { return NumISParticle(211);  };
  inline int NumISPiMinus      (void) const { return NumISParticle(-211); };
  inline int NumISPhoton       (void) const { return NumISParticle(22);   };
  inline int NumISLeptons      (void) const { return NumISParticle(PhysConst::pdg_leptons);       };
  inline int NumISPions        (void) const { return NumISParticle(PhysConst::pdg_pions);         };
  inline int NumISChargePions  (void) const { return NumISParticle(PhysConst::pdg_charged_pions); };

  inline std::vector<int> GetAllISNuElectronIndices (void) const { return GetAllISParticleIndices(12);   };
  inline std::vector<int> GetAllISNuMuonIndices     (void) const { return GetAllISParticleIndices(14);   };
  inline std::vector<int> GetAllISNuTauIndices      (void) const { return GetAllISParticleIndices(16);   };
  inline std::vector<int> GetAllISElectronIndices   (void) const { return GetAllISParticleIndices(11);   };
  inline std::vector<int> GetAllISMuonIndices       (void) const { return GetAllISParticleIndices(13);   };
  inline std::vector<int> GetAllISTauIndices        (void) const { return GetAllISParticleIndices(15);   };
  inline std::vector<int> GetAllISProtonIndices     (void) const { return GetAllISParticleIndices(2212); };
  inline std::vector<int> GetAllISNeutronIndices    (void) const { return GetAllISParticleIndices(2112); };
  inline std::vector<int> GetAllISPiZeroIndices     (void) const { return GetAllISParticleIndices(111);  };
  inline std::vector<int> GetAllISPiPlusIndices     (void) const { return GetAllISParticleIndices(211);  };
  inline std::vector<int> GetAllISPiMinusIndices    (void) const { return GetAllISParticleIndices(-211); };
  inline std::vector<int> GetAllISPhotonIndices     (void) const { return GetAllISParticleIndices(22);   };
  inline std::vector<int> GetAllISLeptonsIndices    (void) const { return GetAllISParticleIndices(PhysConst::pdg_leptons);       };
  inline std::vector<int> GetAllISPionsIndices      (void) const { return GetAllISParticleIndices(PhysConst::pdg_pions);         };
  inline std::vector<int> GetAllISChargePionsIndices(void) const { return GetAllISParticleIndices(PhysConst::pdg_charged_pions); };

  inline std::vector<FitParticle*> GetAllISNuElectron (void) { return GetAllISParticle(12);   };
  inline std::vector<FitParticle*> GetAllISNuMuon     (void) { return GetAllISParticle(14);   };
  inline std::vector<FitParticle*> GetAllISNuTau      (void) { return GetAllISParticle(16);   };
  inline std::vector<FitParticle*> GetAllISElectron   (void) { return GetAllISParticle(11);   };
  inline std::vector<FitParticle*> GetAllISMuon       (void) { return GetAllISParticle(13);   };
  inline std::vector<FitParticle*> GetAllISTau        (void) { return GetAllISParticle(15);   };
  inline std::vector<FitParticle*> GetAllISProton     (void) { return GetAllISParticle(2212); };
  inline std::vector<FitParticle*> GetAllISNeutron    (void) { return GetAllISParticle(2112); };
  inline std::vector<FitParticle*> GetAllISPiZero     (void) { return GetAllISParticle(111);  };
  inline std::vector<FitParticle*> GetAllISPiPlus     (void) { return GetAllISParticle(211);  };
  inline std::vector<FitParticle*> GetAllISPiMinus    (void) { return GetAllISParticle(-211); };
  inline std::vector<FitParticle*> GetAllISPhoton     (void) { return GetAllISParticle(22);   };
  inline std::vector<FitParticle*> GetAllISLeptons    (void) { return GetAllISParticle(PhysConst::pdg_leptons);       };
  inline std::vector<FitParticle*> GetAllISPions      (void) { return GetAllISParticle(PhysConst::pdg_pions);         };
  inline std::vector<FitParticle*> GetAllISChargePions(void) { return GetAllISParticle(PhysConst::pdg_charged_pions); };

  inline FitParticle* GetHMISNuElectron (void) { return GetHMISParticle(12);   };
  inline FitParticle* GetHMISNuMuon     (void) { return GetHMISParticle(14);   };
  inline FitParticle* GetHMISNuTau      (void) { return GetHMISParticle(16);   };
  inline FitParticle* GetHMISElectron   (void) { return GetHMISParticle(11);   };
  inline FitParticle* GetHMISMuon       (void) { return GetHMISParticle(13);   };
  inline FitParticle* GetHMISTau        (void) { return GetHMISParticle(15);   };
  inline FitParticle* GetHMISAnyLeptons (void) { return GetHMISParticle(PhysConst::pdg_all_leptons);   };
  inline FitParticle* GetHMISProton     (void) { return GetHMISParticle(2212); };
  inline FitParticle* GetHMISNeutron    (void) { return GetHMISParticle(2112); };
  inline FitParticle* GetHMISPiZero     (void) { return GetHMISParticle(111);  };
  inline FitParticle* GetHMISPiPlus     (void) { return GetHMISParticle(211);  };
  inline FitParticle* GetHMISPiMinus    (void) { return GetHMISParticle(-211); };
  inline FitParticle* GetHMISPhoton     (void) { return GetHMISParticle(22);   };
  inline FitParticle* GetHMISLepton    (void) { return GetHMISParticle(PhysConst::pdg_leptons);       };
  inline FitParticle* GetHMISPions      (void) { return GetHMISParticle(PhysConst::pdg_pions);         };
  inline FitParticle* GetHMISChargePions(void) { return GetHMISParticle(PhysConst::pdg_charged_pions); };

  inline int GetHMISNuElectronIndex (void) { return GetHMISParticleIndex(12);   };
  inline int GetHMISNuMuonIndex     (void) { return GetHMISParticleIndex(14);   };
  inline int GetHMISNuTauIndex      (void) { return GetHMISParticleIndex(16);   };
  inline int GetHMISElectronIndex   (void) { return GetHMISParticleIndex(11);   };
  inline int GetHMISMuonIndex       (void) { return GetHMISParticleIndex(13);   };
  inline int GetHMISTauIndex        (void) { return GetHMISParticleIndex(15);   };
  inline int GetHMISProtonIndex     (void) { return GetHMISParticleIndex(2212); };
  inline int GetHMISNeutronIndex    (void) { return GetHMISParticleIndex(2112); };
  inline int GetHMISPiZeroIndex     (void) { return GetHMISParticleIndex(111);  };
  inline int GetHMISPiPlusIndex     (void) { return GetHMISParticleIndex(211);  };
  inline int GetHMISPiMinusIndex    (void) { return GetHMISParticleIndex(-211); };
  inline int GetHMISPhotonIndex     (void) { return GetHMISParticleIndex(22);   };
  inline int GetHMISLeptonsIndex    (void) { return GetHMISParticleIndex(PhysConst::pdg_leptons);       };
  inline int GetHMISPionsIndex      (void) { return GetHMISParticleIndex(PhysConst::pdg_pions);         };
  inline int GetHMISChargePionsIndex(void) { return GetHMISParticleIndex(PhysConst::pdg_charged_pions); };

  // ---- Final State Helpers --- //
  inline bool HasFSParticle(int const pdg) const {
    return HasParticle(pdg, kFinalState);
  };
  template <size_t N>
  inline bool HasFSParticle(int const (&pdgs)[N]) const {
    return HasParticle(pdgs, kFinalState);
  };

  inline int NumFSParticle(int const pdg = 0) const {
    return NumParticle(pdg, kFinalState);
  };
  template <size_t N>
  inline int NumFSParticle(int const (&pdgs)[N]) const {
    return NumParticle(pdgs, kFinalState);
  };

  inline std::vector<int> GetAllFSParticleIndices(int const pdg = -1) const {
    return GetAllParticleIndices(pdg, kFinalState);
  };
  template <size_t N>
  inline std::vector<int> GetAllFSParticleIndices(int const (&pdgs)[N]) const {
    return GetAllParticleIndices(pdgs, kFinalState);
  };

  inline std::vector<FitParticle*> GetAllFSParticle(int const pdg = -1) {
    return GetAllParticle(pdg, kFinalState);
  };
  template <size_t N>
  inline std::vector<FitParticle*> GetAllFSParticle(int const (&pdgs)[N]) {
    return GetAllParticle(pdgs, kFinalState);
  };

  inline FitParticle* GetHMFSParticle(int const pdg) {
    return GetHMParticle(pdg, kFinalState);
  };

  inline FitParticle* GetSHMFSParticle(int const pdg) {
    return GetSHMParticle(pdg, kFinalState);
  };

  template <size_t N>
  inline FitParticle* GetHMFSParticle(int const (&pdgs)[N]) {
    return GetHMParticle(pdgs, kFinalState);
  };

  template <size_t N>
  inline FitParticle* GetSHMFSParticle(int const (&pdgs)[N]) {
    return GetSHMParticle(pdgs, kFinalState);
  };

  inline int GetHMFSParticleIndex(int const pdg) const {
    return GetHMParticleIndex(pdg, kFinalState);
  };
  template <size_t N>
  inline int GetHMFSParticleIndex(int const (&pdgs)[N]) const {
    return GetHMParticleIndex(pdgs, kFinalState);
  };

  inline bool HasFSNuElectron   (void) const { return HasFSParticle(12);   };
  inline bool HasFSNuMuon       (void) const { return HasFSParticle(14);   };
  inline bool HasFSNuTau        (void) const { return HasFSParticle(16);   };
  inline bool HasFSElectron     (void) const { return HasFSParticle(11);   };
  inline bool HasFSMuon         (void) const { return HasFSParticle(13);   };
  inline bool HasFSTau          (void) const { return HasFSParticle(15);   };
  inline bool HasFSProton       (void) const { return HasFSParticle(2212); };
  inline bool HasFSNeutron      (void) const { return HasFSParticle(2112); };
  inline bool HasFSPiZero       (void) const { return HasFSParticle(111);  };
  inline bool HasFSPiPlus       (void) const { return HasFSParticle(211);  };
  inline bool HasFSPiMinus      (void) const { return HasFSParticle(-211); };
  inline bool HasFSPhoton       (void) const { return HasFSParticle(22);   };
  inline bool HasFSLeptons      (void) const { return HasFSParticle(PhysConst::pdg_leptons);       };
  inline bool HasFSPions        (void) const { return HasFSParticle(PhysConst::pdg_pions);         };
  inline bool HasFSChargePions  (void) const { return HasFSParticle(PhysConst::pdg_charged_pions); };
  inline bool HasFSNucleons     (void) const { return HasFSParticle(PhysConst::pdg_nucleons); };

  inline int NumFSNuElectron   (void) const { return NumFSParticle(12);   };
  inline int NumFSNuMuon       (void) const { return NumFSParticle(14);   };
  inline int NumFSNuTau        (void) const { return NumFSParticle(16);   };
  inline int NumFSElectron     (void) const { return NumFSParticle(11);   };
  inline int NumFSMuon         (void) const { return NumFSParticle(13);   };
  inline int NumFSTau          (void) const { return NumFSParticle(15);   };
  inline int NumFSProton       (void) const { return NumFSParticle(2212); };
  inline int NumFSNeutron      (void) const { return NumFSParticle(2112); };
  inline int NumFSPiZero       (void) const { return NumFSParticle(111);  };
  inline int NumFSPiPlus       (void) const { return NumFSParticle(211);  };
  inline int NumFSPiMinus      (void) const { return NumFSParticle(-211); };
  inline int NumFSPhoton       (void) const { return NumFSParticle(22);   };
  int NumFSLeptons      (void) const; // { return NumFSParticle(PhysConst::pdg_leptons);       };
  inline int NumFSPions        (void) const { return NumFSParticle(PhysConst::pdg_pions);         };
  inline int NumFSChargePions  (void) const { return NumFSParticle(PhysConst::pdg_charged_pions); };
  inline int NumFSNucleons  (void) const { return NumFSParticle(PhysConst::pdg_nucleons); };

  inline std::vector<int> GetAllFSNuElectronIndices (void) const { return GetAllFSParticleIndices(12);   };
  inline std::vector<int> GetAllFSNuMuonIndices     (void) const { return GetAllFSParticleIndices(14);   };
  inline std::vector<int> GetAllFSNuTauIndices      (void) const { return GetAllFSParticleIndices(16);   };
  inline std::vector<int> GetAllFSElectronIndices   (void) const { return GetAllFSParticleIndices(11);   };
  inline std::vector<int> GetAllFSMuonIndices       (void) const { return GetAllFSParticleIndices(13);   };
  inline std::vector<int> GetAllFSTauIndices        (void) const { return GetAllFSParticleIndices(15);   };
  inline std::vector<int> GetAllFSProtonIndices     (void) const { return GetAllFSParticleIndices(2212); };
  inline std::vector<int> GetAllFSNeutronIndices    (void) const { return GetAllFSParticleIndices(2112); };
  inline std::vector<int> GetAllFSPiZeroIndices     (void) const { return GetAllFSParticleIndices(111);  };
  inline std::vector<int> GetAllFSPiPlusIndices     (void) const { return GetAllFSParticleIndices(211);  };
  inline std::vector<int> GetAllFSPiMinusIndices    (void) const { return GetAllFSParticleIndices(-211); };
  inline std::vector<int> GetAllFSPhotonIndices     (void) const { return GetAllFSParticleIndices(22);   };
  inline std::vector<int> GetAllFSLeptonsIndices    (void) const { return GetAllFSParticleIndices(PhysConst::pdg_leptons);       };
  inline std::vector<int> GetAllFSPionsIndices      (void) const { return GetAllFSParticleIndices(PhysConst::pdg_pions);         };
  inline std::vector<int> GetAllFSChargePionsIndices(void) const { return GetAllFSParticleIndices(PhysConst::pdg_charged_pions); };
  inline std::vector<int> GetAllFSNucleonIndices(void) const { return GetAllFSParticleIndices(PhysConst::pdg_nucleons); };

  inline std::vector<FitParticle*> GetAllFSNuElectron (void) { return GetAllFSParticle(12);   };
  inline std::vector<FitParticle*> GetAllFSNuMuon     (void) { return GetAllFSParticle(14);   };
  inline std::vector<FitParticle*> GetAllFSNuTau      (void) { return GetAllFSParticle(16);   };
  inline std::vector<FitParticle*> GetAllFSElectron   (void) { return GetAllFSParticle(11);   };
  inline std::vector<FitParticle*> GetAllFSMuon       (void) { return GetAllFSParticle(13);   };
  inline std::vector<FitParticle*> GetAllFSTau        (void) { return GetAllFSParticle(15);   };
  inline std::vector<FitParticle*> GetAllFSProton     (void) { return GetAllFSParticle(2212); };
  inline std::vector<FitParticle*> GetAllFSNeutron    (void) { return GetAllFSParticle(2112); };
  inline std::vector<FitParticle*> GetAllFSPiZero     (void) { return GetAllFSParticle(111);  };
  inline std::vector<FitParticle*> GetAllFSPiPlus     (void) { return GetAllFSParticle(211);  };
  inline std::vector<FitParticle*> GetAllFSPiMinus    (void) { return GetAllFSParticle(-211); };
  inline std::vector<FitParticle*> GetAllFSPhoton     (void) { return GetAllFSParticle(22);   };
  inline std::vector<FitParticle*> GetAllFSLeptons     (void) { return GetAllFSParticle(PhysConst::pdg_leptons);       };
  inline std::vector<FitParticle*> GetAllFSPions       (void) { return GetAllFSParticle(PhysConst::pdg_pions);         };
  inline std::vector<FitParticle*> GetAllFSChargePions (void) { return GetAllFSParticle(PhysConst::pdg_charged_pions); };
  inline std::vector<FitParticle*> GetAllFSNucleons    (void) { return GetAllFSParticle(PhysConst::pdg_nucleons); };

  inline FitParticle* GetHMFSNuElectron (void) { return GetHMFSParticle(12);   };
  inline FitParticle* GetHMFSNuMuon     (void) { return GetHMFSParticle(14);   };
  inline FitParticle* GetHMFSNuTau      (void) { return GetHMFSParticle(16);   };
  inline FitParticle* GetHMFSElectron   (void) { return GetHMFSParticle(11);   };
  inline FitParticle* GetHMFSMuon       (void) { return GetHMFSParticle(13);   };
  inline FitParticle* GetHMFSTau        (void) { return GetHMFSParticle(15);   };
  inline FitParticle* GetHMFSAnyLeptons (void) { return GetHMFSParticle(PhysConst::pdg_all_leptons);   };
  inline FitParticle* GetHMFSProton     (void) { return GetHMFSParticle(2212); };
  inline FitParticle* GetHMFSNeutron    (void) { return GetHMFSParticle(2112); };
  inline FitParticle* GetHMFSPiZero     (void) { return GetHMFSParticle(111);  };
  inline FitParticle* GetHMFSPiPlus     (void) { return GetHMFSParticle(211);  };
  inline FitParticle* GetHMFSPiMinus    (void) { return GetHMFSParticle(-211); };
  inline FitParticle* GetHMFSPhoton     (void) { return GetHMFSParticle(22);   };

  inline FitParticle* GetHMFSLeptons    (void) { return GetHMFSParticle(PhysConst::pdg_leptons);       };
  inline FitParticle* GetHMFSAnyLepton  (void) { return GetHMFSParticle(PhysConst::pdg_all_leptons);   };

  inline FitParticle* GetHMFSPions      (void) { return GetHMFSParticle(PhysConst::pdg_pions);         };
  inline FitParticle* GetHMFSChargePions(void) { return GetHMFSParticle(PhysConst::pdg_charged_pions); };
  inline FitParticle* GetHMFSNucleons(void) { return GetHMFSParticle(PhysConst::pdg_nucleons); };

  inline int GetHMFSNuElectronIndex (void) const { return GetHMFSParticleIndex(12);   };
  inline int GetHMFSNuMuonIndex     (void) const { return GetHMFSParticleIndex(14);   };
  inline int GetHMFSNuTauIndex      (void) const { return GetHMFSParticleIndex(16);   };
  inline int GetHMFSElectronIndex   (void) const { return GetHMFSParticleIndex(11);   };
  inline int GetHMFSMuonIndex       (void) const { return GetHMFSParticleIndex(13);   };
  inline int GetHMFSTauIndex        (void) const { return GetHMFSParticleIndex(15);   };
  inline int GetHMFSProtonIndex     (void) const { return GetHMFSParticleIndex(2212); };
  inline int GetHMFSNeutronIndex    (void) const { return GetHMFSParticleIndex(2112); };
  inline int GetHMFSPiZeroIndex     (void) const { return GetHMFSParticleIndex(111);  };
  inline int GetHMFSPiPlusIndex     (void) const { return GetHMFSParticleIndex(211);  };
  inline int GetHMFSPiMinusIndex    (void) const { return GetHMFSParticleIndex(-211); };
  inline int GetHMFSPhotonIndex     (void) const { return GetHMFSParticleIndex(22);   };

  inline int GetHMFSLeptonsIndex    (void) const { return GetHMFSParticleIndex(PhysConst::pdg_leptons);       };
  inline int GetHMFSAnyLeptonIndex  (void) const { return GetHMFSParticleIndex(PhysConst::pdg_all_leptons);       };

  inline int GetHMFSPionsIndex      (void) const { return GetHMFSParticleIndex(PhysConst::pdg_pions);         };
  inline int GetHMFSChargePionsIndex(void) const { return GetHMFSParticleIndex(PhysConst::pdg_charged_pions); };
  inline int GetHMFSChargeNucleonIndex(void) const { return GetHMFSParticleIndex(PhysConst::pdg_nucleons); };

  // ---- NEUTRINO INCOMING Related Functions
  int                   GetBeamNeutrinoIndex   (void) const;
  inline TLorentzVector GetBeamNeutrinoP4      (void) const { return GetParticleP4(GetBeamNeutrinoIndex()); };
  inline TVector3       GetBeamNeutrinoP3      (void) const { return GetParticleP3(GetBeamNeutrinoIndex()); };
  inline double         GetBeamNeutrinoMom     (void) const { return GetParticleMom(GetBeamNeutrinoIndex()); };
  inline double         GetBeamNeutrinoMom2    (void) const { return GetParticleMom2(GetBeamNeutrinoIndex()); };
  inline double         GetBeamNeutrinoE       (void) const { return GetParticleE(GetBeamNeutrinoIndex()); };
  inline double         Enu                    (void) const { return GetBeamNeutrinoE(); };
  inline int            GetBeamNeutrinoPDG     (void) const { return GetParticlePDG(GetBeamNeutrinoIndex()); };
  inline int            PDGnu                  (void) const { return GetBeamNeutrinoPDG(); };
  inline int            GetNeutrinoInPos       (void) const { return GetBeamNeutrinoIndex(); };
  inline FitParticle*   GetBeamNeutrino        (void) { return GetParticle(GetBeamNeutrinoIndex()); };
  inline FitParticle*   GetNeutrinoIn          (void) { return GetParticle(GetBeamNeutrinoIndex()); };


  // ---- Electron INCOMING Related Functions
  int                   GetBeamElectronIndex   (void) const;
  inline TLorentzVector GetBeamElectronP4      (void) const { return GetParticleP4(GetBeamElectronIndex()); };
  inline TVector3       GetBeamElectronP3      (void) const { return GetParticleP3(GetBeamElectronIndex()); };
  inline double         GetBeamElectronMom     (void) const { return GetParticleMom(GetBeamElectronIndex()); };
  inline double         GetBeamElectronMom2    (void) const { return GetParticleMom2(GetBeamElectronIndex()); };
  inline double         GetBeamElectronE       (void) const { return GetParticleE(GetBeamElectronIndex()); };
  inline FitParticle*   GetBeamElectron        (void) { return GetParticle(GetBeamElectronIndex()); };

  // ---- Pion INCOMING Functions
  int                   GetBeamPionIndex   (void) const;
  inline TLorentzVector GetBeamPionP4      (void) const { return GetParticleP4(GetBeamPionIndex()); };
  inline TVector3       GetBeamPionP3      (void) const { return GetParticleP3(GetBeamPionIndex()); };
  inline double         GetBeamPionMom     (void) const { return GetParticleMom(GetBeamPionIndex()); };
  inline double         GetBeamPionMom2    (void) const { return GetParticleMom2(GetBeamPionIndex()); };
  inline double         GetBeamPionE       (void) const { return GetParticleE(GetBeamPionIndex()); };
  inline FitParticle*   GetBeamPion        (void) { return GetParticle(GetBeamPionIndex()); };

  // ---- Generic beam incoming functions
  // I'm not 100% sure why these can't replace the above (FitEvent knows the type)
  int                   GetBeamPartIndex   (void) const;
  inline TLorentzVector GetBeamPartP4      (void) const { return GetParticleP4(GetBeamPartIndex()); };
  inline TVector3       GetBeamPartP3      (void) const { return GetParticleP3(GetBeamPartIndex()); };
  inline double         GetBeamPartMom     (void) const { return GetParticleMom(GetBeamPartIndex()); };
  inline double         GetBeamPartMom2    (void) const { return GetParticleMom2(GetBeamPartIndex()); };
  inline double         GetBeamPartE       (void) const { return GetParticleE(GetBeamPartIndex()); };
  inline int            GetBeamPartPDG     (void) const { return GetParticlePDG(GetBeamPartIndex()); };
  inline int            GetPartInPos       (void) const { return GetBeamPartIndex(); };
  inline FitParticle*   GetBeamPart        (void) { return GetParticle(GetBeamPartIndex()); };

  /// Legacy Functions
  inline FitParticle* PartInfo(uint i) { return GetParticle(i); };
  inline UInt_t Npart (void) const { return NPart(); };
  inline UInt_t NPart (void) const { return fNParticles; };

  // Other Functions
  int NumFSMesons();

  // Get outgoing lepton matching PDG of neutrino
  int GetLeptonOutPDG();
  FitParticle* GetLeptonOut() {return GetHMFSParticle(GetLeptonOutPDG()); };
  // Get the outgoing lepton index
  int GetLeptonIndex() { return GetHMFSParticleIndex(GetLeptonOutPDG()); };

  double GetQ2();


  // Event Information
  UInt_t fEventNo;
  double fTotCrs;
  int fTargetA;
  int fTargetZ;
  int fTargetH;
  bool fBound;
  int fDistance;
  int fTargetPDG;

  // Reduced Particle Stack
  UInt_t kMaxParticles;
  int fNParticles;
  double** fParticleMom;
  UInt_t* fParticleState;
  int* fParticlePDG;
  FitParticle** fParticleList;
  bool *fPrimaryVertex;

  double** fOrigParticleMom;
  UInt_t* fOrigParticleState;
  int* fOrigParticlePDG;
  bool* fOrigPrimaryVertex;

  double* fNEUT_ParticleStatusCode;
  double* fNEUT_ParticleAliveCode;
  GeneratorInfoBase* fGenInfo;

  // Config Options for this class
  bool kRemoveFSIParticles;
  bool kRemoveUndefParticles;



};
/*! @} */
#endif
