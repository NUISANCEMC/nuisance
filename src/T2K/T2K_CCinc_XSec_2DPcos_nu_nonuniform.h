#ifndef T2K_CCINC_2DPCOS_NU_NONUNIFORM_H_SEEN
#define T2K_CCINC_2DPCOS_NU_NONUNIFORM_H_SEEN

#include "Measurement1D.h"

class T2K_CCinc_XSec_2DPcos_nu_nonuniform : public Measurement1D {
public:

  /// Basic Constructor.
  T2K_CCinc_XSec_2DPcos_nu_nonuniform(nuiskey samplekey);

  /// Virtual Destructor
  ~T2K_CCinc_XSec_2DPcos_nu_nonuniform() {};

  /// Numu CC0PI Signal Definition
  bool isSignal(FitEvent *nvect);

  /// Bin Tmu CosThetaMu
  void FillEventVariables(FitEvent* customEvent);

  // Fill Histograms
  void FillHistograms();

  /// Have to do a weird event scaling for analysis 1
  void ConvertEventRates();

 private:

  //double pmu, CosThetaMu;

  //TFile* fInputFile;

  std::vector<TH1D*> fMCHist_Slices;
  std::vector<TH1D*> fDataHist_Slices;

  void FillMCSlice(double x, double y, double w);

  // The number of slices
  int nSlices;

  // Setup the histograms
  void SetHistograms();

  bool UnfoldWithGENIE;
  
};
  
#endif
