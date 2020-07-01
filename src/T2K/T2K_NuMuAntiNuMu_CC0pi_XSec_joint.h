#ifndef T2K_NUMUANTINUMU_CC0PI_XSEC_JOINT_H_SEEN
#define T2K_NUMUANTINUMU_CC0PI_XSEC_JOINT_H_SEEN

#include "JointMeas1D.h"
#include "Measurement1D.h"
#include "T2K_nueCCinc_XSec_1Dpe.h"

class T2K_nueCCinc_XSec_joint : public JointMeas1D {
public:

  T2K_nueCCinc_XSec_joint(nuiskey samplekey);
  virtual ~T2K_nueCCinc_XSec_joint() {};

  void MakePlots();

  // Makes a data hist from all the smaller ones
  void CombineDataHists();

  // Interprets the fractional covariance provided
  void SetFractCovarFromTextFile(std::string covfile);

  // No need for these
  void ScaleEvents(){return;};
  bool isSignal(){return false;};
  void FillEventVariables(){return;};

 private:
  // The separate measurements that go into this
  Measurement1D* FHC_nue;
  Measurement1D* RHC_nue;
  Measurement1D* RHC_nuebar;
};

#endif
