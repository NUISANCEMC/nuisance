#ifndef MINERVA_CCNPIP_XSEC_1DTPI_NU_H_SEEN
#define MINERVA_CCNPIP_XSEC_1DTPI_NU_H_SEEN

#include "Measurement1D.h"

class MINERvA_CCNpip_XSec_1DTpi_nu : public Measurement1D {
public:
  MINERvA_CCNpip_XSec_1DTpi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~MINERvA_CCNpip_XSec_1DTpi_nu() {};

  void FillEventVariables(FitEvent *event);
  void FillHistograms();
  bool isSignal(FitEvent *event);

private:
  bool isNew;
  int nPions;
  std::vector<int> piIndex;
  std::vector<double> TpiVect;
};

#endif
