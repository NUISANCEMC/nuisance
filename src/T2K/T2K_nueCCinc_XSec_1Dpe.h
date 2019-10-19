#ifndef T2K_NUECCINC_XSEC_1DPE_H_SEEN
#define T2K_NUECCINC_XSEC_1DPE_H_SEEN

#include "Measurement1D.h"

class T2K_nueCCinc_XSec_1Dpe : public Measurement1D {
public:

  T2K_nueCCinc_XSec_1Dpe(nuiskey samplekey);

  ~T2K_nueCCinc_XSec_1Dpe() {};

  bool isSignal(FitEvent *event);

  void FillEventVariables(FitEvent* event);

 private:
  int nuPDG;
  int lepPDG;
};
  
#endif
