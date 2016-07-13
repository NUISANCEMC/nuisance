#ifndef MINERvA_CC0pi_XSec_1DQ2_nue_H_SEEN
#define MINERvA_CC0pi_XSec_1DQ2_nue_H_SEEN
#include"Measurement1D.h"

//********************************************************************  
class MINERvA_CC0pi_XSec_1DQ2_nue : public Measurement1D {
//********************************************************************  

public:

  MINERvA_CC0pi_XSec_1DQ2_nue(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~MINERvA_CC0pi_XSec_1DQ2_nue() {};

  // Functions for handling each neut event
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

 private:

  double Enu_rec;
  double Ee, Q2QEe, Thetae;
  bool anty_flag;
  bool nue_flag;
  bool bad_particle;

};
  
#endif
