#ifndef T2K_CC1PIP_CH_XSEC_1DPMU_NU_H_SEEN
#define T2K_CC1PIP_CH_XSEC_1DPMU_NU_H_SEEN 

#include "Measurement1D.h"

class T2K_CC1pip_CH_XSec_1Dpmu_nu : public Measurement1D {
public:
  T2K_CC1pip_CH_XSec_1Dpmu_nu(nuiskey samplekey);
  virtual ~T2K_CC1pip_CH_XSec_1Dpmu_nu() {};

  // Functions to deal with the input data and covariance
  void SetDataValues(std::string fileLocation);
  void SetCovarMatrix(std::string covarFile);

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  private:
};

#endif
