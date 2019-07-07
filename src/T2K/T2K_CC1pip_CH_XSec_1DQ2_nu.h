#ifndef T2K_CC1PIP_CH_XSEC_1DQ2_NU_H_SEEN
#define T2K_CC1PIP_CH_XSEC_1DQ2_NU_H_SEEN 

#include "Measurement1D.h"

//enum T2K_CC1pip_type {kMB = 0, keMB, kDelta};

class T2K_CC1pip_CH_XSec_1DQ2_nu : public Measurement1D {
public:
  T2K_CC1pip_CH_XSec_1DQ2_nu(nuiskey samplekey);
  virtual ~T2K_CC1pip_CH_XSec_1DQ2_nu() {};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

};

#endif
