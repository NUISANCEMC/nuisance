#ifndef T2K_CC1PIP_H2O_XSEC_1DCOSPI_NU_H_SEEN
#define T2K_CC1PIP_H2O_XSEC_1DCOSPI_NU_H_SEEN

#include "Measurement1D.h"
#include "T2K_SignalDef.h"

class T2K_CC1pip_H2O_XSec_1Dcospi_nu : public Measurement1D {
public:
  T2K_CC1pip_H2O_XSec_1Dcospi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~T2K_CC1pip_H2O_XSec_1Dcospi_nu() {};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  private:
};

#endif
