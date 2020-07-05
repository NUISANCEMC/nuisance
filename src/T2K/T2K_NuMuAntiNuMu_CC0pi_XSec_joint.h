#ifndef T2K_NUMUANTINUMU_CC0PI_XSEC_JOINT_H_SEEN
#define T2K_NUMUANTINUMU_CC0PI_XSEC_JOINT_H_SEEN

#include "JointMeas1D.h"
#include "Measurement1D.h"
#include "MeasurementVariableBox2D.h"
#include "T2K_NuMuAntiNuMu_CC0pi_CH_XSec_2DPcos.h"

class T2K_NuMuAntiNuMu_CC0pi_XSec_joint : public JointMeas1D {
public:

  T2K_NuMuAntiNuMu_CC0pi_XSec_joint(nuiskey samplekey);
  virtual ~T2K_NuMuAntiNuMu_CC0pi_XSec_joint() {};

  void CombineMCHists();

  // Makes a data hist from all the smaller ones
  void CombineDataHists();

 private:
  // The separate measurements that go into this
  Measurement1D* NuMuCC0pi;
  Measurement1D* AntiNuMuCC0pi;

};

#endif
