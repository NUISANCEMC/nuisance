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


#include "TLorentzVector.h"
#include "FitParticle.h"
#include "TSpline.h"

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
#include "TArrayD.h"
#include "BaseFitEvt.h"
#include "GeneratorUtils.h"
#include "FitLogger.h"

//! Converts NEUT/NuWro/GENIE events to a comman format straight from the tree
class FitEvent : public BaseFitEvt {
 public:

  // Config Options for this class
  static const bool kRemoveFSIParticles   = true;
  static const bool kRemoveUndefParticles = true;
  
  FitEvent(){}; 
  ~FitEvent(){};

  //! Run event convertor, calls relevent event generator kinematic functions.
  void CalcKinematics (void);

  //! Reset the event variables
  void ResetEvent (void);



  
  /* Event Convertors */
#ifdef __NEUT_ENABLED__
  //! Constructor assigns event address to NeutVect memory.
  FitEvent(NeutVect* event){ this->SetEventAddress(&event); };

  //! Set event address to NeutVect memory
  void SetEventAddress(NeutVect** tempevent);

  //! Convert NeutVect to common format
  void NeutKinematics(void);
#endif

#ifdef __NUWRO_ENABLED__
  //! Constructor assigns event address to NuWro event class memory.
  FitEvent(event* tempEvent){ this->SetEventAddress(&tempEvent); };

  //! Set event address to NuWro event class memory
  void SetEventAddress(event** tempevent);

  //! Convert NuWro event class to common format
  void NuwroKinematics(void);
#endif

#ifdef __GENIE_ENABLED__
  //! Constructor assigns event address to GENIE event class memory.
  FitEvent(NtpMCEventRecord* tempevent){this->SetEventAddress(&tempevent);};

  //! Set event address to GENIE event record memory
  //! Gets GHepRecord from NTuple record.
  void SetEventAddress(NtpMCEventRecord** tempevent);

  //! Convert GENIE event class to common format
  void GENIEKinematics(void);

  //! Flag to remove nuclear components in GENIE
  static const bool kRemoveGenieNuclear = true;
#endif

#ifdef __GiBUU_ENABLED__
  //! Constructor assisgns event address to a GiBUU reader
  void SetEventAddress(GiBUUStdHepReader* tempevent);

  //! Convert GiBUUStdHep event class to common format
  void GiBUUKinematics(void);

  //! Flag to remove nuclear components in GiBUU
  static const bool kRemoveGiBUUNuclear = true;
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
  
  int    Mode; // Public access needed 

  inline int GetBeamPartPos (void) const { return 0; };
  int GetNeutrinoInPos      (void) const;
  int GetLeptonOutPos       (void) const;

  FitParticle* GetBeamPart   (void);
  FitParticle* GetNeutrinoIn (void);
  FitParticle* GetLeptonOut  (void);
  
  inline int GetMode (void) { return fMode; };
  inline double Enu  (void) { return PartInfo(0)->fP.E(); };
  inline int PDGnu   (void) { return PartInfo(0)->fPID; };

  inline int GetTargetA (void) { return fTargetA; };
  inline int GetTargetZ (void) { return fTargetZ; };
  inline int GetTotCrs  (void) { return fTotCrs;  };
  
  double weight; // need for silly reason
 
  /* Read/Write FitEvent Functions */
  void SetBranchAddress(TChain* tn); 
  void AddBranchesToTree(TTree* tn);
  
 private:

  // Event Information
  int    fMode;
  UInt_t fEventNo;
  double fTotCrs;
  int    fTargetA;
  int    fTargetZ;
  int    fTargetH;
  bool   fBound;
  int    fDistance;

  // Reduced Particle Stack
  const static UInt_t  kMaxParticles = 200;
  int     fNParticles;
  //  double  fParticlePos[kMaxParticles][4]; // not needed at the moment
  double  fParticleMom[kMaxParticles][4];
  UInt_t  fParticleState[kMaxParticles];
  int     fParticlePDG[kMaxParticles];

  // Current read particle
  int         fCurParticleIndex;
  FitParticle fCurParticle;

  
};

/*! @} */
#endif

