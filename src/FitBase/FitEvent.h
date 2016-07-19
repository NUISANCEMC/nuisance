#ifndef FITEVENT_H_SEEN
#define FITEVENT_H_SEEN

#include "FitBuild.h"
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

/*!      
 *  \addtogroup FitBase      
 *  @{   
 */

/*! FitEvent Class
  Used to convert NEUT/NuWro/GENIE events into a common format that can be read by the fitter.
 */

//! Converts NEUT/NuWro/GENIE events to a comman format straight from the tree
class FitEvent : public BaseFitEvt {
  
 public:

  //! Default Consstructors. Everything is set to NULL
  FitEvent(){

  };

  //! Default destructor
  ~FitEvent(){
  };
  
  // Generator specific functions
  // Each event type needs a way to set the event address from the tree
  // Then Neut Kinematics sets up the overall information for the event (Mode, NParticles)

  /*
    NEUT
  */

#ifdef __NEUT_ENABLED__
  
  //! Constructor assigns event address to NeutVect memory.
  FitEvent(NeutVect* event){ this->SetEventAddress(&event); };
  
  //! Set event address to NeutVect memory
  void SetEventAddress(NeutVect** tempevent);
  
  //! Convert NeutVect to common format
  void NeutKinematics();
#endif
  

  /*
    NUWRO
  */
  
#ifdef __NUWRO_ENABLED__

  //! Constructor assigns event address to NuWro event class memory. 
  FitEvent(event* tempEvent){ this->SetEventAddress(&tempEvent); };

  //! Set event address to NuWro event class memory
  void SetEventAddress(event** tempevent);

  //! Convert NuWro event class to common format
  void NuwroKinematics();

#endif


  /*
    GENIE
  */
  
#ifdef __GENIE_ENABLED__

  //! Constructor assigns event address to GENIE event class memory.    
  FitEvent(NtpMCEventRecord* tempevent){this->SetEventAddress(&tempevent);};

  //! Set event address to GENIE event record memory
  //! Gets GHepRecord from NTuple record.
  void SetEventAddress(NtpMCEventRecord** tempevent);

  //! Convert GENIE event class to common format
  void GENIEKinematics(); 
#endif


  /*
    GENERAL Fit Event Functions
  */

  //! Run event convertor, calls relevent event generator kinematic functions.
  void CalcKinematics(); 

  //! Reset the event to NULL
  void ResetEvent();     
  
  // Access Functions
  //! Return Any FitParticle from event   
  FitParticle* PartInfo(UInt_t i);
  
  //! Return total particle number             
  UInt_t Npart(){return this->fNParticles;};

  //! Return final state particle count.
  UInt_t NFinalpart(){return this->fNFinalParticles;};  

  // Header Variables
  // protected: // To Make things easier everything is accessible. Not a great standard.
  UInt_t fEventNo; //!< Event No in MC

  UInt_t fNParticles; //!< Total Number of Particles
  UInt_t fNFSIParticles; //!< Total Number of Particles involved in FSI
  UInt_t fNIncomingParticles; //!< Total Number of Starting particles
  UInt_t fNFinalParticles; //!< Total Number of Final Particles
  UInt_t fCurrPartIndex; //!< Current index of particle in iteration
  
  FitParticle* fit_particle; //!< Pointer to the currently created fit_particle
  std::vector<FitParticle> all_particles; //!< vector of all fit particles

  Double_t TotCrs; //!< Total Cross-section (Gives per event in NEUT, Total Integrated in NuWro)
  Double_t PFSurf; //!< Fermi Surface Momentum
  Double_t PFMax;  //!< Max Fermi Momentum
  UInt_t   TargetA; //!< Target Atomic Number
  UInt_t   TargetZ; //!< Target Nucleus Charge
  UInt_t   TargetH; //!< Target Free Protons
  UInt_t   Ibound;  //!< Is target bound
  
  UInt_t Nparticles; //!< Number of particles
  UInt_t Nprimary; //!< Number of primary particles
  
  Double_t weight; //!< event weight
  Double_t FlightDistance; //!< flight distance of neutrino, used for oscillation analysis

  // True Generator events: Just Pointers that can be set.
  

  // ACCESS FUNCTIONS
  double Enu(){ return this->PartInfo(0)->fP.E(); };
  double Tnu(){ return this->PartInfo(0)->fP.E(); };
  double Pnu(){ return this->PartInfo(0)->fP.E(); };
  int PDGnu(){  return this->PartInfo(0)->fPID; };

  int Ilep(){
    for (UInt_t i = 2; i < this->Npart(); i++){
      if (this->PartInfo(i)->fPID == this->PDGnu() - int(this->Mode < 30))
	return i;
    }
    return 0;
  };

  double q0(){ return (this->PartInfo(0)->fP - this->PartInfo(this->Ilep())->fP).E(); };
  double q3(){ return (this->PartInfo(0)->fP - this->PartInfo(this->Ilep())->fP).Vect().Mag(); };

  /* double Elep(); */
  /* double Tlep(); */
  /* double Plep(); */
  /* double PDGlep(); */
  
  /* double Coslep(); */
  /* double Thetalep(); */
  
  /* int Npions(); */
  /* int Npiplus(); */
  

  ClassDef(FitEvent, 1);
};











/*! @} */
#endif

