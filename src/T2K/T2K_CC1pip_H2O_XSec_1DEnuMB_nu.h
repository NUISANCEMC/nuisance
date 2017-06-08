#ifndef T2K_CC1PIP_XSEC_1DENUMB_NU_H_SEEN
#define T2K_CC1PIP_XSEC_1DENUMB_NU_H_SEEN 

#include "Measurement1D.h"
#include "T2K_SignalDef.h"

class T2K_CC1pip_H2O_XSec_1DEnuMB_nu : public Measurement1D {
public:
  T2K_CC1pip_H2O_XSec_1DEnuMB_nu(nuiskey samplekey);
  virtual ~T2K_CC1pip_H2O_XSec_1DEnuMB_nu() {};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  private:
};

#endif
