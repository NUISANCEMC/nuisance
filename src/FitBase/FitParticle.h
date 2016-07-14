#ifndef FITPARTICLE_H_SEEN
#define FITPARTICLE_H_SEEN
#include "TLorentzVector.h"

#ifdef __NEUT_ENABLED__
#include "neutpart.h"
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

#include "TObject.h"

/*!                                                                                                                                                                                                   
 *  \addtogroup FitBase                                                                                                                                                                               
 *  @{                                                                                                                                                                                                
 */

//! Condensed FitParticle class which acts a common format between the generators
class FitParticle : public TObject {

  public:

  //! Virtual Destructor
  ~FitParticle(){ };

  //! Default Constructor
  FitParticle(){
    fPID = -1;    
  };

#ifdef __NEUT_ENABLED__
  //! NEUT Constructor
  FitParticle(NeutPart* part);
#endif
  
  
#ifdef __NUWRO_ENABLED__
  //! NUWRO Constructor
  FitParticle(particle* nuwro_particle, Int_t state);
#endif

  
#ifdef __GENIE_ENABLED__
  //! GENIE Constructor
  FitParticle(genie::GHepParticle* genie_particle);
#endif

  //! NULL Constructor for when the generator screws up.
  FitParticle(UInt_t* i);

  TLorentzVector fP; //!< Particle 4 Momentum
  int fPID; //!< Particle PDG Code
  int fIsAlive; //!< Whether the particle is alive at the end of the event (Yes 1, No 0, Other? -1)
  int fStatus; //!< Particle Status (Incoming 1, FSI 2, Outgoing 0, Other 3)
  double fMass; //!< Particle Mass

  ClassDef(FitParticle,1);
  
};

/*! @} */
#endif
