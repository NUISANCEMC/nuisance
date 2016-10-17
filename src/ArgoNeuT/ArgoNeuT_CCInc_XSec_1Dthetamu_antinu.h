#ifndef ArgoNeuT_CCInc_XSec_1Dthetamu_antinu_H_SEEN
#define ArgoNeuT_CCInc_XSec_1Dthetamu_antinu_H_SEEN

#include "Measurement1D.h"

class ArgoNeuT_CCInc_XSec_1Dthetamu_antinu : public Measurement1D {
public:
  ArgoNeuT_CCInc_XSec_1Dthetamu_antinu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~ArgoNeuT_CCInc_XSec_1Dthetamu_antinu() {};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  private:
};

#endif
