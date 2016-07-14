#ifndef FITEVENTBASE_H_SEEN
#define FITEVENTBASE_H_SEEN

#include "TLorentzVector.h"
#include "FitParticle.h"
#include "FitBuild.h"
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

/*!      
 *  \addtogroup FitBase      
 *  @{   
 */

//! Global Enum to define generator type being read with FitEvent
enum generator_event_type { kUNKNOWN=999, kNEUT=0, kNUWRO=2, kGENIE=5, kEVTSPLINE=6 };

//! Base Event Class used to store just the generator event pointers and flat variables
class BaseFitEvt : public TObject {
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

  double GetWeight(){ return InputWeight * RWWeight * CustomWeight; };
  
  ClassDef(BaseFitEvt, 1);
};
#endif
