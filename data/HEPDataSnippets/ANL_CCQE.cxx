#include "FitEvent.h"
#include "FitUtils.h"

#include "TLorentzVector.h"

#include <cmath>
#include <iostream>
#include <string>

extern "C" {
double ANL_CCQE_Project_Q2(FitEvent *event) {
  // std::cout << "ANL_CCQE_project_Q2: event(" << event
  //           << ")->GetNeutrinoIn(): " << event->GetNeutrinoIn() << std::endl;
  // std::cout << "ANL_CCQE_project_Q2: event(" << event
  //           << ")->GetHMFSParticle(13): " << event->GetHMFSParticle(13)
  //           << std::endl;

  if ((!event->GetNeutrinoIn()) || (!event->GetHMFSParticle(13))) {
    return 0;
  }

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  return FitUtils::Q2QErec(Pmu, std::cos(ThetaMu), 0., true);
}

bool ANL_CCQE_Filter(FitEvent *event) {

  // std::cout << "ANL_CCQE_filter: abs(event->Mode): " << abs(event->Mode)
  //           << std::endl;
  // This is not what we want!
  // Need to provide IsCCQE function in environment which knows how to check for
  // a given input
  if (abs(event->Mode) != 1) {
    return false;
  }

  // std::cout << "ANL_CCQE_filter: event(" << event
  //           << ")->GetNeutrinoIn(): " << event->GetNeutrinoIn() << std::endl;
  // std::cout << "ANL_CCQE_filter: event(" << event
  //           << ")->GetHMFSParticle(13): " << event->GetHMFSParticle(13)
  //           << std::endl;

  if ((!event->GetNeutrinoIn()) || (!event->GetHMFSParticle(13))) {
    return false;
  }

  double Enu = event->GetNeutrinoIn()->fP.E();

  double const EnuMin = 0;
  double const EnuMax = 6000;

  if ((Enu < EnuMin) || (Enu > EnuMax)) {
    return false;
  }

  // Q2 cut
  if (ANL_CCQE_Project_Q2(event) <= 0) {
    return false;
  }

  return true;
}
}