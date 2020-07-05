#ifndef T2K_NUMUANTINUMU_CC0PI_XSEC_JOINT_H_SEEN
#define T2K_NUMUANTINUMU_CC0PI_XSEC_JOINT_H_SEEN

#include "JointMeas1D.h"
#include "T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos.h"

class T2K_NuMuAntiNuMu_CC0pi_XSec_joint : public JointMeas1D {
public:

  T2K_NuMuAntiNuMu_CC0pi_XSec_joint(nuiskey samplekey);
  virtual ~T2K_NuMuAntiNuMu_CC0pi_XSec_joint() {};

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
  T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos* NuMuCC0pi;
  T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos* AntiNuMuCC0pi;
  TFile* fInputFile;

};

#endif
