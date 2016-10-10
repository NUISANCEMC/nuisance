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
  void ScaleEvents();
  void Write(std::string drawOpts);

private:
  bool isNew;
  int nPions;

  std::vector<int> piIndex;
  std::vector<double> TpiVect;

  TH1I *hnPions;
  TH1D *onePions;
  TH1D *twoPions;
  TH1D *threePions;
  TH1D *morePions;

};

#endif
