#ifndef T2K_NUMU_CC0PI_OC_XSEC_2DPCOS_JOINT_H_SEEN
#define T2K_NUMU_CC0PI_OC_XSEC_2DPCOS_JOINT_H_SEEN

#include "JointMeas1D.h"
#include "T2K_NuMu_CC0pi_OC_XSec_2DPcos.h"

class T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint : public JointMeas1D {
public:

  T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint(nuiskey samplekey);
  virtual ~T2K_NuMu_CC0pi_OC_XSec_2DPcos_joint() {};

  // Makes a data hist from all the smaller ones
  void CombineDataHists();

  // Signal already defined in the single measurement class  
  bool isSignal(){return false;};

  // Variables already defined in the single measurement class  
  void FillEventVariables(){return;};

  // Read covariace
  void SetCovariance();

  // Read histograms
  void SetHistograms();
  
  // Fill Histograms
  void FillHistograms();
  
  // Event scaling 
  void ConvertEventRates();
 
 private:
  T2K_NuMu_CC0pi_O_XSec_2DPcos* NuMuCC0piO;
  T2K_NuMu_CC0pi_C_XSec_2DPcos* NuMuCC0piC;
  TFile* fInputFileCov;
};

#endif
