#ifndef T2K_CCCOH_C12_XSEC_1DENU_NU_H_SEEN
#define T2K_CCCOH_C12_XSEC_1DENU_NU_H_SEEN 

#include "Measurement1D.h"

class T2K_CCCOH_C12_XSec_1DEnu_nu : public Measurement1D {
public:
  T2K_CCCOH_C12_XSec_1DEnu_nu(nuiskey samplekey);
  virtual ~T2K_CCCOH_C12_XSec_1DEnu_nu() {};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

};

#endif
