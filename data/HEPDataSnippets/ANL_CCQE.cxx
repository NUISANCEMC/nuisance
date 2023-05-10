#include <iostream>
#include <string>

#include <FitEvent.h>

#include "TLorentzVector.h"

extern "C" {
bool ANL_CCQE_project_Q2(FitEvent *event) {
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  return FitUtils::Q2QErec(Pmu, cos(ThetaMu), 0., true);
}

bool ANL_CCQE_filter(FitEvent *event) {

  // This is not what we want!
  // Need to provide IsCCQE function in environment which knows how to check for
  // a given input
  if (abs(event->Mode) != 1) {
    return false;
  }

  double Enu = event->GetNeutrinoIn()->fP.E();

  double const EnuMin = 0;
  double const EnuMax = 6000;

  if ((Enu < EnuMin) || (Enu > EnuMax)) {
    return false;
  }

  // Q2 cut
  if (ANL_CCQE_project_Q2(event) <= 0) {
    return false;
  }

  return true;
}
}