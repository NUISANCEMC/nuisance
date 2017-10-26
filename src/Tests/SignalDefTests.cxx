#include <cassert>
#include <sstream>

#include "TLorentzVector.h"
#include "TRandom3.h"

#include "FitEvent.h"
#include "SignalDef.h"

struct ConstructibleFitEvent : public FitEvent {
  ConstructibleFitEvent() : FitEvent() { fNParticles = 0; }
  void AddPart(double Mom[4], size_t State, int PDG) {
    fParticleMom[fNParticles][0] = Mom[0];
    fParticleMom[fNParticles][1] = Mom[1];
    fParticleMom[fNParticles][2] = Mom[2];
    fParticleMom[fNParticles][3] = Mom[3];
    fParticleState[fNParticles] = State;
    fParticlePDG[fNParticles] = PDG;
    fNParticles++;
  }
  void SetMode(int mode) { Mode = mode; }
  std::string ToString() {
    std::stringstream ss("");
    ss << "Mode: " << Mode << std::endl;
    ss << "Particles: " << fNParticles << std::endl;
    ss << " -> Particle Stack " << std::endl;
    for (int i = 0; i < fNParticles; i++) {
      ss << " -> -> " << i << ". " << fParticlePDG[i] << " "
         << fParticleState[i] << " "
         << "  Mom(" << fParticleMom[i][0] << ", " << fParticleMom[i][1] << ", "
         << fParticleMom[i][2] << ", " << fParticleMom[i][3] << ")."
         << std::endl;
    }
    return ss.str();
  }
};

template <size_t N, size_t M>
ConstructibleFitEvent MakePDGStackEvent(int (&ISpdgs)[N], int (&FSpdgs)[M],
                                        int Mode = 1) {
  ConstructibleFitEvent fe;
  double MomHolder[4] = {0, 0, 1E3, 1E3};
  for (size_t p_it = 0; p_it < N; ++p_it) {
    fe.AddPart(MomHolder, kInitialState, ISpdgs[p_it]);
  }

  TRandom3 rnd;
  TLorentzVector rnd4M;
  TVector3 rn3M;
  for (size_t p_it = 0; p_it < M; ++p_it) {
    /// Could do better and actually get the correct masses...
    rn3M.SetMagThetaPhi(fabs(rnd.Gaus(200, 50)), rnd.Uniform(M_PI),
                        2 * rnd.Uniform(M_PI));
    rnd4M.SetVectM(rn3M, 105);
    MomHolder[0] = rnd4M[0];
    MomHolder[1] = rnd4M[1];
    MomHolder[2] = rnd4M[2];
    MomHolder[3] = rnd4M[3];
    fe.AddPart(MomHolder, kFinalState, FSpdgs[p_it]);
  }
  fe.SetMode(Mode);
  fe.OrderStack();
  return fe;
}

int main(int argc, char const *argv[]) {
  bool FailOnFail = (argc > 1);
  LOG_VERB(SAM);

  LOG(FIT) << "*            Running SignalDef Tests" << std::endl;
  LOG(FIT) << "***************************************************"
           << std::endl;

  int IS[] = {14};
  int FS_CC0pi_1[] = {13, 2112, 2212, 2112, 2212};
  ConstructibleFitEvent fe_CC0pi_1 = MakePDGStackEvent(IS, FS_CC0pi_1);

  int FS_CC0pi_2[] = {13, 2112, 2212, 2112, 2212};
  ConstructibleFitEvent fe_CC0pi_2 = MakePDGStackEvent(IS, FS_CC0pi_2, 2);

  int FS_CC0pi_3[] = {-13, 2112, 2212, 2112, 2212};
  ConstructibleFitEvent fe_CC0pi_3 = MakePDGStackEvent(IS, FS_CC0pi_3);

  int FS_CC0pi_4[] = {13, 2112, 2212};
  ConstructibleFitEvent fe_CC0pi_4 = MakePDGStackEvent(IS, FS_CC0pi_4, 12);

  int FS_CC1pip_1[] = {13, 2212, 2112, 211};
  ConstructibleFitEvent fe_CC1pip_1 = MakePDGStackEvent(IS, FS_CC1pip_1, 2);

  int FS_CC1pim_1[] = {13, -211, 2212, 2112};
  ConstructibleFitEvent fe_CC1pim_1 = MakePDGStackEvent(IS, FS_CC1pim_1, 11);

  int FS_CC1pi0_1[] = {13, 2212, 111, 2112};
  ConstructibleFitEvent fe_CC1pi0_1 = MakePDGStackEvent(IS, FS_CC1pi0_1, 12);

  int FS_CC1pi0_2[] = {11, 2212, 111, 2112};
  ConstructibleFitEvent fe_CC1pi0_2 = MakePDGStackEvent(IS, FS_CC1pi0_2, 12);

  int FS_CCNpi_1[] = {13, 2212, 111, 2112, 211};
  ConstructibleFitEvent fe_CCNpi_1 = MakePDGStackEvent(IS, FS_CCNpi_1, 21);

  int FS_CCNpi_2[] = {13, -211, 211, 2112, 211};
  ConstructibleFitEvent fe_CCNpi_2 = MakePDGStackEvent(IS, FS_CCNpi_2, 21);

  int FS_CCNpi_3[] = {13, 2212, 111, 211, -211};
  ConstructibleFitEvent fe_CCNpi_3 = MakePDGStackEvent(IS, FS_CCNpi_3, 26);

  int FS_CCNpi_4[] = {13, 2212, 111, 111, 111};
  ConstructibleFitEvent fe_CCNpi_4 = MakePDGStackEvent(IS, FS_CCNpi_4, 26);

  int FS_CCCOH_1[] = {13, 211};
  ConstructibleFitEvent fe_CCCOH_1 = MakePDGStackEvent(IS, FS_CCCOH_1, 16);

  int FS_NCel_1[] = {14, 2112};
  ConstructibleFitEvent fe_NCel_1 = MakePDGStackEvent(IS, FS_NCel_1, 52);

  int FS_NCel_2[] = {12, 2212};
  ConstructibleFitEvent fe_NCel_2 = MakePDGStackEvent(IS, FS_NCel_2, 51);

  int FS_NC1pi_1[] = {14, 2112, -211};
  ConstructibleFitEvent fe_NC1pi_1 = MakePDGStackEvent(IS, FS_NC1pi_1, 31);

  int FS_NCNpi_1[] = {14, 2212, 211};
  ConstructibleFitEvent fe_NCNpi_1 = MakePDGStackEvent(IS, FS_NCNpi_1, 32);

  LOG(FIT) << "*            Testing: SignalDef::isCCINC" << std::endl;

  std::map<ConstructibleFitEvent *, bool> isCCINC_PassExpectations;
  isCCINC_PassExpectations[&fe_CC0pi_1] = true;    // numu CC0pi
  isCCINC_PassExpectations[&fe_CC0pi_2] = true;    // numu CC0pi (2p2h)
  isCCINC_PassExpectations[&fe_CC0pi_3] = false;   // numub CC0pi
  isCCINC_PassExpectations[&fe_CC0pi_4] = true;    // numu CC0pi (RES)
  isCCINC_PassExpectations[&fe_CC1pip_1] = true;   // numu CC1pip (2p2h)
  isCCINC_PassExpectations[&fe_CC1pim_1] = true;   // numu CC1pim
  isCCINC_PassExpectations[&fe_CC1pi0_1] = true;   // numu CC1pi0
  isCCINC_PassExpectations[&fe_CC1pi0_2] = false;  // nue CC1pi0
  isCCINC_PassExpectations[&fe_CCNpi_1] = true;    // numu CC multi pi
  isCCINC_PassExpectations[&fe_CCNpi_2] = true;    // numu CC multi pi
  isCCINC_PassExpectations[&fe_CCNpi_3] = true;    // numu CC multi pi
  isCCINC_PassExpectations[&fe_CCNpi_4] = true;    // numu CC multi pi
  isCCINC_PassExpectations[&fe_CCCOH_1] = true;    // numu CC COH pi
  isCCINC_PassExpectations[&fe_NCel_1] = false;    // numu NCEl
  isCCINC_PassExpectations[&fe_NCel_2] = false;    // nue NCEl
  isCCINC_PassExpectations[&fe_NC1pi_1] = false;   // numu NC1pi
  isCCINC_PassExpectations[&fe_NCNpi_1] = false;   // numu NC multi pi

  size_t ctr = 0;
  for (std::map<ConstructibleFitEvent *, bool>::iterator
           fe_it = isCCINC_PassExpectations.begin();
       fe_it != isCCINC_PassExpectations.end(); ++fe_it, ++ctr) {
    bool res = SignalDef::isCCINC(fe_it->first, 14);
    if (res != fe_it->second) {
      ERR(FTL) << "Event: (" << ctr << ")\n"
               << fe_it->first->ToString() << std::endl;
      ERR(FTL) << (res ? "passed" : "failed")
               << " SignalDef::isCCINC unexpectedly." << std::endl;
    } else {
      LOG(SAM) << "Event: (" << ctr << ") " << (res ? "passed" : "failed")
               << " as expected." << std::endl;
    }
    if (FailOnFail) {
      assert(res == fe_it->second);
    }
  }

  LOG(FIT) << "*            Testing: SignalDef::isNCINC" << std::endl;

  std::map<ConstructibleFitEvent *, bool> isNCINC_PassExpectations;
  isNCINC_PassExpectations[&fe_CC0pi_1] = false;   // numu CC0pi
  isNCINC_PassExpectations[&fe_CC0pi_2] = false;   // numu CC0pi (2p2h)
  isNCINC_PassExpectations[&fe_CC0pi_3] = false;   // numub CC0pi
  isNCINC_PassExpectations[&fe_CC0pi_4] = false;   // numu CC0pi (RES)
  isNCINC_PassExpectations[&fe_CC1pip_1] = false;  // numu CC1pip (2p2h)
  isNCINC_PassExpectations[&fe_CC1pim_1] = false;  // numu CC1pim
  isNCINC_PassExpectations[&fe_CC1pi0_1] = false;  // numu CC1pi0
  isNCINC_PassExpectations[&fe_CC1pi0_2] = false;  // nue CC1pi0
  isNCINC_PassExpectations[&fe_CCNpi_1] = false;   // numu CC multi pi
  isNCINC_PassExpectations[&fe_CCNpi_2] = false;   // numu CC multi pi
  isNCINC_PassExpectations[&fe_CCNpi_3] = false;   // numu CC multi pi
  isNCINC_PassExpectations[&fe_CCNpi_4] = false;   // numu CC multi pi
  isNCINC_PassExpectations[&fe_CCCOH_1] = false;   // numu CC COH pi
  isNCINC_PassExpectations[&fe_NCel_1] = true;     // numu NCEl
  isNCINC_PassExpectations[&fe_NCel_2] = false;    // nue NCEl
  isNCINC_PassExpectations[&fe_NC1pi_1] = true;    // numu NC1pi
  isNCINC_PassExpectations[&fe_NCNpi_1] = true;    // numu NC multi pi

  ctr = 0;
  for (std::map<ConstructibleFitEvent *, bool>::iterator
           fe_it = isNCINC_PassExpectations.begin();
       fe_it != isNCINC_PassExpectations.end(); ++fe_it, ++ctr) {
    bool res = SignalDef::isNCINC(fe_it->first, 14);
    if (res != fe_it->second) {
      ERR(FTL) << "Event: (" << ctr << ")\n"
               << fe_it->first->ToString() << std::endl;
      ERR(FTL) << (res ? "passed" : "failed")
               << " SignalDef::isNCINC unexpectedly." << std::endl;
    } else {
      LOG(SAM) << "Event: (" << ctr << ") " << (res ? "passed" : "failed")
               << " as expected." << std::endl;
    }
    if (FailOnFail) {
      assert(res == fe_it->second);
    }
  }

  LOG(FIT) << "*            Testing: SignalDef::isCC0pi" << std::endl;

  std::map<ConstructibleFitEvent *, bool> isCC0pi_PassExpectations;
  isCC0pi_PassExpectations[&fe_CC0pi_1] = true;    // numu CC0pi
  isCC0pi_PassExpectations[&fe_CC0pi_2] = true;    // numu CC0pi (2p2h)
  isCC0pi_PassExpectations[&fe_CC0pi_3] = false;   // numub CC0pi
  isCC0pi_PassExpectations[&fe_CC0pi_4] = true;    // numu CC0pi (RES)
  isCC0pi_PassExpectations[&fe_CC1pip_1] = false;  // numu CC1pip (2p2h)
  isCC0pi_PassExpectations[&fe_CC1pim_1] = false;  // numu CC1pim
  isCC0pi_PassExpectations[&fe_CC1pi0_1] = false;  // numu CC1pi0
  isCC0pi_PassExpectations[&fe_CC1pi0_2] = false;  // nue CC1pi0
  isCC0pi_PassExpectations[&fe_CCNpi_1] = false;   // numu CC multi pi
  isCC0pi_PassExpectations[&fe_CCNpi_2] = false;   // numu CC multi pi
  isCC0pi_PassExpectations[&fe_CCNpi_3] = false;   // numu CC multi pi
  isCC0pi_PassExpectations[&fe_CCNpi_4] = false;   // numu CC multi pi
  isCC0pi_PassExpectations[&fe_CCCOH_1] = false;   // numu CC COH pi
  isCC0pi_PassExpectations[&fe_NCel_1] = false;    // numu NCEl
  isCC0pi_PassExpectations[&fe_NCel_2] = false;    // nue NCEl
  isCC0pi_PassExpectations[&fe_NC1pi_1] = false;   // numu NC1pi
  isCC0pi_PassExpectations[&fe_NCNpi_1] = false;   // numu NC multi pi

  ctr = 0;
  for (std::map<ConstructibleFitEvent *, bool>::iterator
           fe_it = isCC0pi_PassExpectations.begin();
       fe_it != isCC0pi_PassExpectations.end(); ++fe_it, ++ctr) {
    bool res = SignalDef::isCC0pi(fe_it->first, 14);
    if (res != fe_it->second) {
      ERR(FTL) << "Event: (" << ctr << ")\n"
               << fe_it->first->ToString() << " " << std::endl;
      ERR(FTL) << (res ? "passed" : "failed")
               << " SignalDef::isCC0pi unexpectedly." << std::endl;
    } else {
      LOG(SAM) << "Event: (" << ctr << ") " << (res ? "passed" : "failed")
               << " as expected." << std::endl;
    }
    if (FailOnFail) {
      assert(res == fe_it->second);
    }
  }

  LOG(FIT) << "*            Testing: SignalDef::isCCQELike" << std::endl;

  std::map<ConstructibleFitEvent *, bool> isCCQELike_PassExpectations;
  isCCQELike_PassExpectations[&fe_CC0pi_1] = true;    // numu CC0pi
  isCCQELike_PassExpectations[&fe_CC0pi_2] = true;    // numu CC0pi (2p2h)
  isCCQELike_PassExpectations[&fe_CC0pi_3] = false;   // numub CC0pi
  isCCQELike_PassExpectations[&fe_CC0pi_4] = false;   // numu CC0pi (RES)
  isCCQELike_PassExpectations[&fe_CC1pip_1] = true;   // numu CC1pip (2p2h)
  isCCQELike_PassExpectations[&fe_CC1pim_1] = false;  // numu CC1pim
  isCCQELike_PassExpectations[&fe_CC1pi0_1] = false;  // numu CC1pi0
  isCCQELike_PassExpectations[&fe_CC1pi0_2] = false;  // nue CC1pi0
  isCCQELike_PassExpectations[&fe_CCNpi_1] = false;   // numu CC multi pi
  isCCQELike_PassExpectations[&fe_CCNpi_2] = false;   // numu CC multi pi
  isCCQELike_PassExpectations[&fe_CCNpi_3] = false;   // numu CC multi pi
  isCCQELike_PassExpectations[&fe_CCNpi_4] = false;   // numu CC multi pi
  isCCQELike_PassExpectations[&fe_CCCOH_1] = false;   // numu CC COH pi
  isCCQELike_PassExpectations[&fe_NCel_1] = false;    // numu NCEl
  isCCQELike_PassExpectations[&fe_NCel_2] = false;    // nue NCEl
  isCCQELike_PassExpectations[&fe_NC1pi_1] = false;   // numu NC1pi
  isCCQELike_PassExpectations[&fe_NCNpi_1] = false;   // numu NC multi pi

  ctr = 0;
  for (std::map<ConstructibleFitEvent *, bool>::iterator
           fe_it = isCCQELike_PassExpectations.begin();
       fe_it != isCCQELike_PassExpectations.end(); ++fe_it, ++ctr) {
    bool res = SignalDef::isCCQELike(fe_it->first, 14);
    if (res != fe_it->second) {
      ERR(FTL) << "Event: (" << ctr << ")\n"
               << fe_it->first->ToString() << std::endl;
      ERR(FTL) << (res ? "passed" : "failed")
               << " SignalDef::isCCQELike unexpectedly." << std::endl;
    } else {
      LOG(SAM) << "Event: (" << ctr << ") " << (res ? "passed" : "failed")
               << " as expected." << std::endl;
    }
    if (FailOnFail) {
      assert(res == fe_it->second);
    }
  }

  LOG(FIT) << "*            Testing: SignalDef::isCCQE" << std::endl;

  std::map<ConstructibleFitEvent *, bool> isCCQE_PassExpectations;
  isCCQE_PassExpectations[&fe_CC0pi_1] = true;    // numu CC0pi
  isCCQE_PassExpectations[&fe_CC0pi_2] = false;   // numu CC0pi (2p2h)
  isCCQE_PassExpectations[&fe_CC0pi_3] = false;   // numub CC0pi
  isCCQE_PassExpectations[&fe_CC0pi_4] = false;   // numu CC0pi (RES)
  isCCQE_PassExpectations[&fe_CC1pip_1] = false;  // numu CC1pip (2p2h)
  isCCQE_PassExpectations[&fe_CC1pim_1] = false;  // numu CC1pim
  isCCQE_PassExpectations[&fe_CC1pi0_1] = false;  // numu CC1pi0
  isCCQE_PassExpectations[&fe_CC1pi0_2] = false;  // nue CC1pi0
  isCCQE_PassExpectations[&fe_CCNpi_1] = false;   // numu CC multi pi
  isCCQE_PassExpectations[&fe_CCNpi_2] = false;   // numu CC multi pi
  isCCQE_PassExpectations[&fe_CCNpi_3] = false;   // numu CC multi pi
  isCCQE_PassExpectations[&fe_CCNpi_4] = false;   // numu CC multi pi
  isCCQE_PassExpectations[&fe_CCCOH_1] = false;   // numu CC COH pi
  isCCQE_PassExpectations[&fe_NCel_1] = false;    // numu NCEl
  isCCQE_PassExpectations[&fe_NCel_2] = false;    // nue NCEl
  isCCQE_PassExpectations[&fe_NC1pi_1] = false;   // numu NC1pi
  isCCQE_PassExpectations[&fe_NCNpi_1] = false;   // numu NC multi pi

  ctr = 0;
  for (std::map<ConstructibleFitEvent *, bool>::iterator
           fe_it = isCCQE_PassExpectations.begin();
       fe_it != isCCQE_PassExpectations.end(); ++fe_it, ++ctr) {
    bool res = SignalDef::isCCQE(fe_it->first, 14);
    if (res != fe_it->second) {
      ERR(FTL) << "Event: (" << ctr << ")\n"
               << fe_it->first->ToString() << std::endl;
      ERR(FTL) << (res ? "passed" : "failed")
               << " SignalDef::isCCQE unexpectedly." << std::endl;
    } else {
      LOG(SAM) << "Event: (" << ctr << ") " << (res ? "passed" : "failed")
               << " as expected." << std::endl;
    }
    if (FailOnFail) {
      assert(res == fe_it->second);
    }
  }

  LOG(FIT) << "*            Testing: SignalDef::isCCCOH" << std::endl;

  std::map<ConstructibleFitEvent *, bool> isCCCOH_PassExpectations;
  isCCCOH_PassExpectations[&fe_CC0pi_1] = false;   // numu CC0pi
  isCCCOH_PassExpectations[&fe_CC0pi_2] = false;   // numu CC0pi (2p2h)
  isCCCOH_PassExpectations[&fe_CC0pi_3] = false;   // numub CC0pi
  isCCCOH_PassExpectations[&fe_CC0pi_4] = false;   // numu CC0pi (RES)
  isCCCOH_PassExpectations[&fe_CC1pip_1] = false;  // numu CC1pip (2p2h)
  isCCCOH_PassExpectations[&fe_CC1pim_1] = false;  // numu CC1pim
  isCCCOH_PassExpectations[&fe_CC1pi0_1] = false;  // numu CC1pi0
  isCCCOH_PassExpectations[&fe_CC1pi0_2] = false;  // nue CC1pi0
  isCCCOH_PassExpectations[&fe_CCNpi_1] = false;   // numu CC multi pi
  isCCCOH_PassExpectations[&fe_CCNpi_2] = false;   // numu CC multi pi
  isCCCOH_PassExpectations[&fe_CCNpi_3] = false;   // numu CC multi pi
  isCCCOH_PassExpectations[&fe_CCNpi_4] = false;   // numu CC multi pi
  isCCCOH_PassExpectations[&fe_CCCOH_1] = true;    // numu CC COH pi
  isCCCOH_PassExpectations[&fe_NCel_1] = false;    // numu NCEl
  isCCCOH_PassExpectations[&fe_NCel_2] = false;    // nue NCEl
  isCCCOH_PassExpectations[&fe_NC1pi_1] = false;   // numu NC1pi
  isCCCOH_PassExpectations[&fe_NCNpi_1] = false;   // numu NC multi pi

  ctr = 0;
  for (std::map<ConstructibleFitEvent *, bool>::iterator
           fe_it = isCCCOH_PassExpectations.begin();
       fe_it != isCCCOH_PassExpectations.end(); ++fe_it, ++ctr) {
    bool res = SignalDef::isCCCOH(fe_it->first, 14, 211);
    if (res != fe_it->second) {
      ERR(FTL) << "Event: (" << ctr << ")\n"
               << fe_it->first->ToString() << std::endl;
      ERR(FTL) << (res ? "passed" : "failed")
               << " SignalDef::isCCCOH unexpectedly." << std::endl;
    } else {
      LOG(SAM) << "Event: (" << ctr << ") " << (res ? "passed" : "failed")
               << " as expected." << std::endl;
    }
    if (FailOnFail) {
      assert(res == fe_it->second);
    }
  }

  LOG(FIT) << "*            Testing: SignalDef::isCC1pi" << std::endl;

  std::map<ConstructibleFitEvent *, bool> isCC1pi_PassExpectations;
  isCC1pi_PassExpectations[&fe_CC0pi_1] = false;   // numu CC0pi
  isCC1pi_PassExpectations[&fe_CC0pi_2] = false;   // numu CC0pi (2p2h)
  isCC1pi_PassExpectations[&fe_CC0pi_3] = false;   // numub CC0pi
  isCC1pi_PassExpectations[&fe_CC0pi_4] = false;   // numu CC0pi (RES)
  isCC1pi_PassExpectations[&fe_CC1pip_1] = true;   // numu CC1pip (2p2h)
  isCC1pi_PassExpectations[&fe_CC1pim_1] = false;  // numu CC1pim
  isCC1pi_PassExpectations[&fe_CC1pi0_1] = false;  // numu CC1pi0
  isCC1pi_PassExpectations[&fe_CC1pi0_2] = false;  // nue CC1pi0
  isCC1pi_PassExpectations[&fe_CCNpi_1] = false;   // numu CC multi pi
  isCC1pi_PassExpectations[&fe_CCNpi_2] = false;   // numu CC multi pi
  isCC1pi_PassExpectations[&fe_CCNpi_3] = false;   // numu CC multi pi
  isCC1pi_PassExpectations[&fe_CCNpi_4] = false;   // numu CC multi pi
  isCC1pi_PassExpectations[&fe_CCCOH_1] = true;    // numu CC COH pi
  isCC1pi_PassExpectations[&fe_NCel_1] = false;    // numu NCEl
  isCC1pi_PassExpectations[&fe_NCel_2] = false;    // nue NCEl
  isCC1pi_PassExpectations[&fe_NC1pi_1] = false;   // numu NC1pi
  isCC1pi_PassExpectations[&fe_NCNpi_1] = false;   // numu NC multi pi

  ctr = 0;
  for (std::map<ConstructibleFitEvent *, bool>::iterator
           fe_it = isCC1pi_PassExpectations.begin();
       fe_it != isCC1pi_PassExpectations.end(); ++fe_it, ++ctr) {
    bool res = SignalDef::isCC1pi(fe_it->first, 14, 211);
    if (res != fe_it->second) {
      ERR(FTL) << "Event: (" << ctr << ")\n"
               << fe_it->first->ToString() << std::endl;
      ERR(FTL) << (res ? "passed" : "failed")
               << " SignalDef::isCC1pi unexpectedly." << std::endl;
    } else {
      LOG(SAM) << "Event: (" << ctr << ") " << (res ? "passed" : "failed")
               << " as expected." << std::endl;
    }
    if (FailOnFail) {
      assert(res == fe_it->second);
    }
  }

  LOG(FIT) << "*            Testing: SignalDef::isNC1pi" << std::endl;

  std::map<ConstructibleFitEvent *, bool> isNC1pi_PassExpectations;
  isNC1pi_PassExpectations[&fe_CC0pi_1] = false;   // numu CC0pi
  isNC1pi_PassExpectations[&fe_CC0pi_2] = false;   // numu CC0pi (2p2h)
  isNC1pi_PassExpectations[&fe_CC0pi_3] = false;   // numub CC0pi
  isCCINC_PassExpectations[&fe_CC0pi_4] = false;   // numu CC0pi (RES)
  isNC1pi_PassExpectations[&fe_CC1pip_1] = false;  // numu CC1pip (2p2h)
  isNC1pi_PassExpectations[&fe_CC1pim_1] = false;  // numu CC1pim
  isNC1pi_PassExpectations[&fe_CC1pi0_1] = false;  // numu CC1pi0
  isNC1pi_PassExpectations[&fe_CC1pi0_2] = false;  // nue CC1pi0
  isNC1pi_PassExpectations[&fe_CCNpi_1] = false;   // numu CC multi pi
  isNC1pi_PassExpectations[&fe_CCNpi_2] = false;   // numu CC multi pi
  isNC1pi_PassExpectations[&fe_CCNpi_3] = false;   // numu CC multi pi
  isNC1pi_PassExpectations[&fe_CCNpi_4] = false;   // numu CC multi pi
  isCCINC_PassExpectations[&fe_CCCOH_1] = false;   // numu CC COH pi
  isNC1pi_PassExpectations[&fe_NCel_1] = false;    // numu NCEl
  isNC1pi_PassExpectations[&fe_NCel_2] = false;    // nue NCEl
  isCCINC_PassExpectations[&fe_NC1pi_1] = true;    // numu NC1pi
  isCCINC_PassExpectations[&fe_NCNpi_1] = false;   // numu NC multi pi

  ctr = 0;
  for (std::map<ConstructibleFitEvent *, bool>::iterator
           fe_it = isNC1pi_PassExpectations.begin();
       fe_it != isNC1pi_PassExpectations.end(); ++fe_it, ++ctr) {
    bool res = SignalDef::isNC1pi(fe_it->first, 14, -211);
    if (res != fe_it->second) {
      ERR(FTL) << "Event: (" << ctr << ")\n"
               << fe_it->first->ToString() << std::endl;
      ERR(FTL) << (res ? "passed" : "failed")
               << " SignalDef::isNC1pi unexpectedly." << std::endl;
    } else {
      LOG(SAM) << "Event: (" << ctr << ") " << (res ? "passed" : "failed")
               << " as expected." << std::endl;
    }
    if (FailOnFail) {
      assert(res == fe_it->second);
    }
  }

  // SignalDef::isCCWithFS(&fe,14);
}
