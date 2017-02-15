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

#ifndef FITEVENT2_H_SEEN
#define FITEVENT2_H_SEEN
/*!
 *  \addtogroup FitBase
 *  @{
 */

#include <algorithm>
#include <iterator>
#include <vector>
#include "FitParticle.h"
#include "TLorentzVector.h"
#include "TSpline.h"

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
#include "BaseFitEvt.h"
#include "FitLogger.h"
#include "GeneratorUtils.h"
#include "TArrayD.h"

// New Fit Event Structure.
// Fit Event should contain a pointer to the BaseFitEvent.
// Each subclass should have a FillNUISANCEEvent function.
// 


//! Converts NEUT/NuWro/GENIE events to a comman format straight from the tree
class FitEvent : public BaseFitEvt {
 public:
  // Config Options for this class
  static const bool kRemoveFSIParticles = true;
  static const bool kRemoveUndefParticles = true;

  FitEvent() {
    for (UInt_t i = 0; i < kMaxParticles; i++) {
      fParticleList[i] = NULL;
    }
  };
  ~FitEvent(){};

  //! Run event convertor, calls relevent event generator kinematic functions.
  void CalcKinematics(void);

  //! Reset the event variables
  void ResetEvent(void);
  void ResetParticleList(void);

/* Event Convertors */
#ifdef __NEUT_ENABLED__
  //! Constructor assigns event address to NeutVect memory.
  FitEvent(NeutVect* event) { 
    fNeutVect = event;
    fType = kNEUT;
  };
#endif

#ifdef __NUWRO_ENABLED__
  //! Constructor assigns event address to NuWro event class memory.
  FitEvent(event* tempEvent) { 
    fType = kNUWRO;
    fNuwroEvent = tempEvent;
  };
#endif

#ifdef __GENIE_ENABLED__
  //! Constructor assigns event address to GENIE event class memory.
  FitEvent(NtpMCEventRecord* tempevent) { 
    fType = kGENIE;
    genie_event = tempevent;
};

#endif

#ifdef __GiBUU_ENABLED__
FitEvent(GiBUUStdHepReader* tempevent){
    fType = kGiBUU;
    GiRead = tempevent;
}
#endif

#ifdef __NUANCE_ENABLED__
  //! Constructor assigns event address to a Nuance reader
  void SetEventAddress(NuanceEvent** tempevent);

  //! Convert Nuance event class to common format
  void NuanceKinematics(void);
#endif

  /* Standard Event Functions */

  void OrderStack();

  //! Return Any FitParticle from event
  FitParticle* PartInfo(UInt_t i);
  inline UInt_t Npart(void) const { return NPart(); };
  inline UInt_t NPart(void) const { return fNParticles; };

  int Mode;  // Public access needed

  inline int GetBeamPartPos(void) const { return 0; };
  int GetNeutrinoInPos(void) const;
  int GetLeptonOutPos(void) const;

  FitParticle* GetBeamPart(void);
  FitParticle* GetNeutrinoIn(void);
  FitParticle* GetLeptonOut(void);

  // Access functions
  inline bool IsCC() { return (abs(fMode) <= 30); };
  inline bool IsNC() { return (abs(fMode) > 30); };

  // Utility functions to return number of a certain class of particles
  int NumFSLeptons();
  int NumFSMesons();

  bool HasParticle(int pdg = 0, int state = -1);
  int NumParticle(int pdg = 0, int state = -1);
  int NumParticle(std::vector<int> pdg, int state = -1);

  inline bool HasISParticle(int pdg) {
    return HasParticle(pdg, kInitialState);
  };
  inline int NumISParticle(int pdg = 0) {
    return NumParticle(pdg, kInitialState);
  };
  // inline int NumISParticle(std::vector<int> pdg) {
  //   return NumParticle(pdg, kInitialState);
  // };
  inline bool HasISNuElectron(void) { return HasISParticle(12); };
  inline bool HasISNuMuon(void) { return HasISParticle(14); };
  inline bool HasISNuTau(void) { return HasISParticle(16); };
  inline bool HasISElectron(void) { return HasISParticle(11); };
  inline bool HasISMuon(void) { return HasISParticle(13); };
  inline bool HasISTau(void) { return HasISParticle(15); };
  inline bool HasISProton(void) { return HasISParticle(2212); };
  inline bool HasISNeutron(void) { return HasISParticle(2112); };
  inline bool HasISPiZero(void) { return HasISParticle(111); };
  inline bool HasISPiPlus(void) { return HasISParticle(211); };
  inline bool HasISPiMinus(void) { return HasISParticle(-211); };
  inline bool HasISPhoton(void) { return HasISParticle(22); };

  inline bool HasFSParticle(int pdg) { return HasParticle(pdg, kFinalState); };
  inline int NumFSParticle(int pdg = 0) {
    return NumParticle(pdg, kFinalState);
  };
  template <size_t N>
  inline int NumFSParticle(int const (&pdgs)[N]) {
    int rtn = 0;
    for (size_t i = 0; i < N; ++i) {
      rtn += NumParticle(pdgs[i], kFinalState);
    }
    return rtn;
  };
  template <size_t N>
  inline int NumParticle(int const (&pdgs)[N]) {
    int rtn = 0;
    for (size_t i = 0; i < N; ++i) {
      rtn += NumParticle(pdgs[i]);
    }
    return rtn;
  };

  // inline int NumFSParticle(std::vector<int> pdg) {
  //   return NumParticle(pdg, kFinalState);
  // };
  inline bool HasFSNuElectron(void) { return HasFSParticle(12); };
  inline bool HasFSNuMuon(void) { return HasFSParticle(14); };
  inline bool HasFSNuTau(void) { return HasFSParticle(16); };
  inline bool HasFSElectron(void) { return HasFSParticle(11); };
  inline bool HasFSMuon(void) { return HasFSParticle(13); };
  inline bool HasFSTau(void) { return HasFSParticle(15); };
  inline bool HasFSProton(void) { return HasFSParticle(2212); };
  inline bool HasFSNeutron(void) { return HasFSParticle(2112); };
  inline bool HasFSPiZero(void) { return HasFSParticle(111); };
  inline bool HasFSPiPlus(void) { return HasFSParticle(211); };
  inline bool HasFSPiMinus(void) { return HasFSParticle(-211); };
  inline bool HasFSPhoton(void) { return HasFSParticle(22); };

  FitParticle* GetHMParticle(int pdg = 0, int state = -1);
  FitParticle* GetHMParticle(std::vector<int> pdg, int state = -1);

  template <size_t N>
  inline FitParticle* GetHMParticle(int const (&pdgs)[N]) {
    FitParticle* rtn = NULL;
    for (size_t i = 0; i < N; ++i) {
      FitParticle* p = GetHMParticle(pdgs[i]);
      // If this one is good, and we don't have one.
      if (p && !rtn) {
        rtn = p;
        continue;
      }
      // if this one is good and it has more 3mom than the other one.
      // (Mag2 doesn't need sqrt so slightly faster for same comparison.)
      if (p && (p->fP.Vect().Mag2() > rtn->fP.Vect().Mag2())) {
        rtn = p;
      }
    }
    return rtn;
  };

  inline FitParticle* GetHMISParticle(int pdg) {
    return GetHMParticle(pdg, kInitialState);
  };
  template <size_t N>
  inline FitParticle* GetHMISParticle(int const (&pdgs)[N]) {
    FitParticle* rtn = NULL;
    for (size_t i = 0; i < N; ++i) {
      FitParticle* p = GetHMParticle(pdgs[i], kInitialState);
      // If this one is good, and we don't have one.
      if (p && !rtn) {
        rtn = p;
        continue;
      }
      // if this one is good and it has more 3mom than the other one.
      // (Mag2 doesn't need sqrt so slightly faster for same comparison.)
      if (p && (p->fP.Vect().Mag2() > rtn->fP.Vect().Mag2())) {
        rtn = p;
      }
    }
    return rtn;
  };
  // inline FitParticle* GetHMISParticle(std::vector<int> pdg) {
  //   return GetHMParticle(pdg, kInitialState);
  // };
  inline FitParticle* GetHMISNuElectron(void) { return GetHMISParticle(12); };
  inline FitParticle* GetHMISNuMuon(void) { return GetHMISParticle(14); };
  inline FitParticle* GetHMISNuTau(void) { return GetHMISParticle(16); };
  inline FitParticle* GetHMISElectron(void) { return GetHMISParticle(11); };
  inline FitParticle* GetHMISMuon(void) { return GetHMISParticle(13); };
  inline FitParticle* GetHMISTau(void) { return GetHMISParticle(15); };
  inline FitParticle* GetHMISProton(void) { return GetHMISParticle(2212); };
  inline FitParticle* GetHMISNeutron(void) { return GetHMISParticle(2112); };
  inline FitParticle* GetHMISPiZero(void) { return GetHMISParticle(111); };
  inline FitParticle* GetHMISPiPlus(void) { return GetHMISParticle(211); };
  inline FitParticle* GetHMISPiMinus(void) { return GetHMISParticle(-211); };
  inline FitParticle* GetHMISPhoton(void) { return GetHMISParticle(22); };

  inline FitParticle* GetHMFSParticle(int pdg) {
    return GetHMParticle(pdg, kFinalState);
  };
  template <size_t N>
  inline FitParticle* GetHMFSParticle(int const (&pdgs)[N]) {
    FitParticle* rtn = NULL;
    for (size_t i = 0; i < N; ++i) {
      FitParticle* p = GetHMParticle(pdgs[i], kFinalState);
      // If this one is good, and we don't have one.
      if (p && !rtn) {
        rtn = p;
        continue;
      }
      // if this one is good and it has more 3mom than the other one.
      // (Mag2 doesn't need sqrt so slightly faster for same comparison.)
      if (p && (p->fP.Vect().Mag2() > rtn->fP.Vect().Mag2())) {
        rtn = p;
      }
    }
    return rtn;
  };
  // inline FitParticle* GetHMFSParticle(std::vector<int> pdg) {
  //   return GetHMParticle(pdg, kFinalState);
  // };
  inline FitParticle* GetHMFSNuElectron(void) { return GetHMFSParticle(12); };
  inline FitParticle* GetHMFSNuMuon(void) { return GetHMFSParticle(14); };
  inline FitParticle* GetHMFSNuTau(void) { return GetHMFSParticle(16); };
  inline FitParticle* GetHMFSElectron(void) { return GetHMFSParticle(11); };
  inline FitParticle* GetHMFSMuon(void) { return GetHMFSParticle(13); };
  inline FitParticle* GetHMFSTau(void) { return GetHMFSParticle(15); };
  inline FitParticle* GetHMFSProton(void) { return GetHMFSParticle(2212); };
  inline FitParticle* GetHMFSNeutron(void) { return GetHMFSParticle(2112); };
  inline FitParticle* GetHMFSPiZero(void) { return GetHMFSParticle(111); };
  inline FitParticle* GetHMFSPiPlus(void) { return GetHMFSParticle(211); };
  inline FitParticle* GetHMFSPiMinus(void) { return GetHMFSParticle(-211); };
  inline FitParticle* GetHMFSPhoton(void) { return GetHMFSParticle(22); };

  inline int GetMode(void) { return fMode; };
  inline double Enu(void) { return PartInfo(0)->fP.E(); };
  inline int PDGnu(void) { return PartInfo(0)->fPID; };

  inline int GetTargetA(void) { return fTargetA; };
  inline int GetTargetZ(void) { return fTargetZ; };
  inline int GetTotCrs(void) { return fTotCrs; };

  double weight;  // need for silly reason

  /* Read/Write FitEvent Functions */
  void SetBranchAddress(TChain* tn);
  void AddBranchesToTree(TTree* tn);

  void Print();

  // Event Information
  int fMode;
  UInt_t fEventNo;
  double fTotCrs;
  int fTargetA;
  int fTargetZ;
  int fTargetH;
  bool fBound;
  int fDistance;

  // Reduced Particle Stack
  const static UInt_t kMaxParticles = 400;
  int fNParticles;
  //  double  fParticlePos[kMaxParticles][4]; // not needed at the moment
  double fParticleMom[kMaxParticles][4];
  UInt_t fParticleState[kMaxParticles];
  int fParticlePDG[kMaxParticles];
  FitParticle* fParticleList[kMaxParticles];

  double* fNEUT_ParticleStatusCode;
  double* fNEUT_ParticleAliveCode;


};

/*! @} */
#endif
