#ifndef T2K_CC1PIP_XSEC_1DENUDELTA_NU_H_SEEN
#define T2K_CC1PIP_XSEC_1DENUDELTA_NU_H_SEEN 

#include "Measurement1D.h"

class T2K_CC1pip_H2O_XSec_1DEnuDelta_nu : public Measurement1D {
public:
  T2K_CC1pip_H2O_XSec_1DEnuDelta_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~T2K_CC1pip_H2O_XSec_1DEnuDelta_nu() {};

  // Functions to deal with the input data and covariance
  void SetDataValues(std::string fileLocation);
  void SetCovarMatrix(std::string covarFile);

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  private:
};

#endif
