#include "TLorentzVector.h"
#include "TRandom3.h"
#include "FitEvent.h"

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
