#ifndef T2K_CC1PIP_CH_XSEC_2DPMUCOSMU_NU_H_SEEN
#define T2K_CC1PIP_CH_XSEC_2DPMUCOSMU_NU_H_SEEN 

#include "Measurement1D.h"

class T2K_CC1pip_CH_XSec_2Dpmucosmu_nu : public Measurement1D {
public:
  T2K_CC1pip_CH_XSec_2Dpmucosmu_nu(nuiskey samplekey);
  virtual ~T2K_CC1pip_CH_XSec_2Dpmucosmu_nu() {};

  // Functions to deal with the input data and covariance
  //void SetDataValues(std::string fileLocation);
  //void SetCovarMatrix(std::string covarFile);

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  void FillHistograms();

  // Piggy-back off this one to fill the MC hist in slices
  void ConvertEventRates();

  private:
    std::vector<TH1D*> fMCHist_Slices;
    std::vector<TH1D*> fDataHist_Slices;
    void FillMCSlice(double x, double y, double w);
    void SetHistograms();
};

#endif
