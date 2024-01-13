#ifndef T2K_NC1PIP_XSEC_LIN2D_H_SEEN
#define T2K_NC1PIP_XSEC_LIN2D_H_SEEN

#include "Measurement1D.h"

class T2K_NC1pip_XSec_lin2D : public Measurement1D {
public:

  T2K_NC1pip_XSec_lin2D(nuiskey samplekey);

  ~T2K_NC1pip_XSec_lin2D() {};

  bool isSignal(FitEvent *event);

  void FillEventVariables(FitEvent* event);

  void FillHistograms();

 private:
  double p_pi;
  double costh_pi;
};
  
#endif
