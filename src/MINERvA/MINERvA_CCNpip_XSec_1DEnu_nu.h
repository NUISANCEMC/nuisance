#ifndef MINERVA_CCNPIP_XSEC_1DENU_NU_H_SEEN
#define MINERVA_CCNPIP_XSEC_1DENU_NU_H_SEEN

#include "Measurement1D.h"

class MINERvA_CCNpip_XSec_1DEnu_nu : public Measurement1D {
public:
  MINERvA_CCNpip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~MINERvA_CCNpip_XSec_1DEnu_nu() {};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

private:
  bool isNew;
};

#endif
