#ifndef T2K_NUECCINC_XSEC_1DTHE_H_SEEN
#define T2K_NUECCINC_XSEC_1DTHE_H_SEEN

#include "Measurement1D.h"

class T2K_nueCCinc_XSec_1Dthe : public Measurement1D {
public:

  T2K_nueCCinc_XSec_1Dthe(nuiskey samplekey);

  ~T2K_nueCCinc_XSec_1Dthe() {};

  bool isSignal(FitEvent *event);

  void FillEventVariables(FitEvent* event);

 private:
  int nuPDG;
  int lepPDG;
};
  
#endif
