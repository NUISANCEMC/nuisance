#ifndef T2K_NUMUANTINUMU_CC0PI_XSEC_JOINT_H_SEEN
#define T2K_NUMUANTINUMU_CC0PI_XSEC_JOINT_H_SEEN

#include "JointMeas1D.h"
#include "Measurement1D.h"
#include "T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos.h"

class T2K_NuMuAntiNuMu_CC0pi_XSec_joint : public JointMeas1D {
public:

  T2K_NuMuAntiNuMu_CC0pi_XSec_joint(nuiskey samplekey);
  virtual ~T2K_NuMuAntiNuMu_CC0pi_XSec_joint() {};

  void MakePlots();

  // Makes a data hist from all the smaller ones
  void CombineDataHists();
  
 private:
  // The separate measurements that go into this
  T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos* NuMuCC0pi;
  T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos* AntiNuMuCC0pi;

  bool isSignal(){return false;};
  void SetHistograms(){return;};
  void FillEventVariables(){return;};
  void FillHistograms(){return;};
  void ConvertEventRates(){return;};
  void ScaleEvents(){return;};

};

#endif
