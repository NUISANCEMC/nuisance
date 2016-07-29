#ifndef MINERVA_CCNPIP_XSEC_1DTH_NU_H_SEEN
#define MINERVA_CCNPIP_XSEC_1DTH_NU_H_SEEN

#include "Measurement1D.h"

class MINERvA_CCNpip_XSec_1Dth_nu : public Measurement1D {
public:
  MINERvA_CCNpip_XSec_1Dth_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~MINERvA_CCNpip_XSec_1Dth_nu() {};

  void FillEventVariables(FitEvent *event);
  void FillHistograms();
  bool isSignal(FitEvent *event);
  void Write(std::string drawOpt);

private:
  bool isNew;
  int nPions;
  TH1I *hnPions;
  std::vector<unsigned int> piIndex;
  std::vector<double> thVect;
};

#endif
