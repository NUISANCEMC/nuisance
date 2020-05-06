// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
 *    This file is part of NUISANCE.
 *
 *    NUISANCE is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    NUISANCE is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#include "MINERvAUtils.h"

#include "FitUtils.h"

namespace MINERvAPar {
double MINERvADensity = FitPar::Config().GetParD("MINERvADensity");
double MINERvARecoDist = FitPar::Config().GetParD("MINERvARecoDist");
double PenetratingMuonE = FitPar::Config().GetParD("PenetratingMuonEnergy");
double NumRangeSteps = FitPar::Config().GetParI("NumRangeSteps");
} // namespace MINERvAPar

double MINERvAUtils::StoppedEfficiency(TH2D *effHist, FitParticle *nu,
                                       FitParticle *muon) {

  double eff = 0.;

  if (!effHist)
    return eff;
  eff = effHist->GetBinContent(effHist->FindBin(
      FitUtils::p(muon), FitUtils::th(nu, muon) / TMath::Pi() * 180.));

  return eff;
}

double MINERvAUtils::PenetratedEfficiency(FitParticle *nu, FitParticle *muon) {

  double eff = 0.;

  if (FitUtils::th(nu, muon) / TMath::Pi() * 180. > 50)
    eff = 0.;
  if (FitUtils::p(muon) < 1.4)
    eff = 0.;

  return eff;
}

double MINERvAUtils::BetheBlochCH(double E, double mass) {

  double beta2 = 1 - mass * mass / E / E;
  double gamma = 1. / sqrt(1 - beta2);
  double mass_ratio = PhysConst::mass_electron * 1000. / mass;
  // Argh, have to remember to convert to MeV or you'll hate yourself!
  double I2 = 68.7e-6 * 68.7e-6; // mean excitation potential I

  double w_max = 2 * PhysConst::mass_electron * 1000. * beta2 * gamma * gamma;
  w_max /= 1 + 2 * gamma * mass_ratio + mass_ratio * mass_ratio;

  // Values taken from the PDG for K = 0.307075 MeV mol-1 cm2, mean ionization
  // energy I = 68.7 eV (Polystyrene) <Z/A> = 0.53768
  // (pdg.lbl.gov/AtomicNuclearProperties)
  double log_term = log(2 * PhysConst::mass_electron * 1000. * beta2 * gamma *
                        gamma * w_max / I2);
  double dedx = 0.307075 * 0.53768 / beta2 * (0.5 * log_term - beta2);

  return dedx;
}

// This function returns an estimate of the range of the particle in
// scintillator. It uses crude integration and Bethe-Bloch to approximate the
// range.
double MINERvAUtils::RangeInScintillator(FitParticle *particle, int nsteps) {

  // The particle energy
  double E = particle->fP.E();
  double M = particle->fP.M();
  double Ek = E - M;

  double step_size = Ek / float(nsteps + 1);
  double range = 0;

  // Add an offset to make the integral a touch more accurate
  Ek -= step_size / 2.;
  for (int i = 0; i < nsteps; ++i) {

    double dEdx = MINERvAUtils::BetheBlochCH(Ek + M, M);

    Ek -= step_size;
    // dEdx is -ve
    range -= step_size / dEdx;
  }

  // Account for density of polystyrene
  range /= MINERvAPar::MINERvADensity;

  // Range estimate is in cm
  return fabs(range);
}

double
MINERvAUtils::GetEDepositOutsideRangeInScintillator(FitParticle *particle,
                                                    double rangelimit) {

  // The particle energy
  double E = particle->fP.E();
  double M = particle->fP.M();
  double Ek = E - M;

  int nsteps = MINERvAPar::NumRangeSteps;
  double step_size = Ek / float(nsteps + 1);
  double range = 0;
  double Ekinside = 0.0;
  double Ekstart = Ek;

  // Add an offset to make the integral a touch more accurate
  Ek -= step_size / 2.;
  for (int i = 0; i < nsteps; ++i) {

    double dEdx = MINERvAUtils::BetheBlochCH(Ek + M, M);

    Ek -= step_size;
    // dEdx is -ve
    range -= step_size / dEdx;
    if (fabs(range) / MINERvAPar::MINERvADensity < rangelimit) {
      Ekinside = Ek;
    }
  }

  // Range estimate is in cm
  return Ekstart - Ekinside;
}

double MINERvAUtils::GetEDepositInsideRangeInScintillator(FitParticle *particle,
                                                          double rangelimit) {
  // The particle energy
  double E = particle->fP.E();
  double M = particle->fP.M();
  double Ek = E - M;

  return Ek - GetEDepositOutsideRangeInScintillator(particle, rangelimit);
}

// Function to calculate the distance the particle travels in scintillator
bool MINERvAUtils::PassesDistanceCut(FitParticle *beam, FitParticle *particle) {

  double dist =
      MINERvAUtils::RangeInScintillator(particle, MINERvAPar::NumRangeSteps);
  double zdist = dist * cos(FitUtils::th(beam, particle));

  if (abs(zdist) < MINERvAPar::MINERvARecoDist)
    return false;
  return true;
}

// Function to return the MainTrk
int MINERvAUtils::GetMainTrack(FitEvent *event, TH2D *effHist,
                               FitParticle *&mainTrk, double &weight,
                               bool penetrated) {

  FitParticle *nu = event->GetNeutrinoIn();
  double highestMom = 0;
  int index = 0;
  mainTrk = NULL;

  // Loop over particles
  for (uint j = 2; j < event->Npart(); ++j) {

    // Final state only!
    if (!(event->PartInfo(j))->fIsAlive)
      continue;
    if (event->PartInfo(j)->fNEUTStatusCode != 0)
      continue;

    int PID = event->PartInfo(j)->fPID;

    // Only consider pions, muons for now
    if (abs(PID) != 211 && abs(PID) != 13)
      continue;

    // Ignore if higher momentum tracks available
    if (FitUtils::p(event->PartInfo(j)) < highestMom)
      continue;

    // Okay, now this is highest momentum
    highestMom = FitUtils::p(event->PartInfo(j));
    weight *= MINERvAUtils::StoppedEfficiency(effHist, nu, event->PartInfo(j));
    index = j;
    mainTrk = event->PartInfo(j);
  } // end loop over particle stack

  return index;
}

void MINERvAUtils::GetOtherTrackInfo(FitEvent *event, int mainIndex,
                                     int &nProtons, int &nPiMus, int &nVertex,
                                     FitParticle *&secondTrk) {

  // Reset everything
  nPiMus = 0;
  nProtons = 0;
  nVertex = 0;
  secondTrk = NULL;

  double highestMom = 0.;

  // Loop over particles
  for (uint j = 2; j < event->Npart(); ++j) {

    // Don't re-count the main track
    if (j == (uint)mainIndex)
      continue;

    // Final state only!
    if (!(event->PartInfo(j))->fIsAlive)
      continue;
    if (event->PartInfo(j)->fNEUTStatusCode != 0)
      continue;

    int PID = event->PartInfo(j)->fPID;

    // Only consider pions, muons, protons
    if (abs(PID) != 211 && PID != 2212 && abs(PID) != 13)
      continue;

    // Must be reconstructed as a track in SciBooNE
    if (MINERvAUtils::PassesDistanceCut(event->PartInfo(0),
                                        event->PartInfo(j))) {

      // Keep track of the second highest momentum track
      if (FitUtils::p(event->PartInfo(j)) > highestMom) {
        highestMom = FitUtils::p(event->PartInfo(j));
        secondTrk = event->PartInfo(j);
      }

      if (PID == 2212)
        nProtons += 1;
      else
        nPiMus += 1;
    } else
      nVertex += 1;

  } // end loop over particle stack

  return;
}

// NOTE: need to adapt this to allow for penetrating events...
// Simpler, but gives the same results as in Hirade-san's thesis
double MINERvAUtils::CalcThetaPr(FitEvent *event, FitParticle *main,
                                 FitParticle *second, bool penetrated) {

  FitParticle *nu = event->GetNeutrinoIn();

  if (!main || !nu || !second)
    return -999;

  // Construct the vector p_pr = (-p_mux, -p_muy, Enurec - pmucosthetamu)
  // where p_mux, p_muy are the projections of the candidate muon momentum onto
  // the x and y dimension respectively
  double pmu = main->fP.Vect().Mag();
  double pmu_x = main->fP.Vect().X();
  double pmu_y = main->fP.Vect().Y();

  if (penetrated) {
    pmu = 1400.;
    double ratio = 1.4 / main->fP.Vect().Mag();
    TVector3 mod_mu = main->fP.Vect() * ratio;
    pmu_x = mod_mu.X();
    pmu_y = mod_mu.Y();
  }

  double Enuqe =
      FitUtils::EnuQErec(pmu / 1000., cos(FitUtils::th(nu, main)), 27., true) *
      1000.;
  double p_pr_z = Enuqe - pmu * cos(FitUtils::th(nu, main));

  TVector3 p_pr = TVector3(-pmu_x, -pmu_y, p_pr_z);
  double thetapr = p_pr.Angle(second->fP.Vect()) / TMath::Pi() * 180.;

  return thetapr;
}

double MINERvAUtils::CalcThetaPi(FitEvent *event, FitParticle *second) {

  FitParticle *nu = event->GetNeutrinoIn();

  if (!second || !nu)
    return -999;

  double thetapi = FitUtils::th(nu, second) / TMath::Pi() * 180.;
  return thetapi;
}

/// Functions to deal with the SB mode stacks
MINERvAUtils::ModeStack::ModeStack(std::string name, std::string title,
                                   TH1 *hist) {
  fName = name;
  fTitle = title;

  AddMode(0, "Other", "Other", kGreen + 2, 2, 3244);
  AddMode(1, "CCDIS", "#nu_{#mu} CC DIS", kRed, 2, 3304);
  AddMode(2, "CCRES", "#nu_{#mu} CC Resonant", kGray + 2, 2, 1001);
  AddMode(3, "CCQE", "#nu_{#mu} CC QE", kMagenta, 2, 1001);
  AddMode(4, "CC2P2H", "#nu_{#mu} CC 2p2h", kAzure + 1, 2, 1001);

  StackBase::SetupStack(hist);
};

int MINERvAUtils::ModeStack::ConvertModeToIndex(int mode) {
  switch (abs(mode)) {
  case 1:
    return 3;
  case 2:
    return 4;
  case 11:
  case 12:
  case 13:
    return 2;
  case 26:
    return 1;
  default:
    return 0;
  }
};

void MINERvAUtils::ModeStack::Fill(int mode, double x, double y, double z,
                                   double weight) {
  StackBase::FillStack(MINERvAUtils::ModeStack::ConvertModeToIndex(mode), x, y,
                       z, weight);
};

void MINERvAUtils::ModeStack::Fill(FitEvent *evt, double x, double y, double z,
                                   double weight) {
  StackBase::FillStack(MINERvAUtils::ModeStack::ConvertModeToIndex(evt->Mode),
                       x, y, z, weight);
};

void MINERvAUtils::ModeStack::Fill(BaseFitEvt *evt, double x, double y,
                                   double z, double weight) {
  StackBase::FillStack(MINERvAUtils::ModeStack::ConvertModeToIndex(evt->Mode),
                       x, y, z, weight);
};

double MINERvAUtils::Get_STV_dpt_MINERvAPS(FitEvent *event,
                                           double ProtonMinCut_MeV,
                                           double ProtonMaxCut_MeV,
                                           double ProtonCosThetaCut, int ISPDG,
                                           bool Is0pi) {
  // Check that the neutrino exists
  if (event->NumISParticle(ISPDG) == 0) {
    return -9999;
  }
  // Return 0 if the proton or muon are missing
  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1)) == 0) {
    return -9999;
  }

  // Now get the TVector3s for each particle
  TVector3 const &NuP = event->GetHMISParticle(ISPDG)->fP.Vect();
  TVector3 const &LeptonP =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->fP.Vect();
  // Find the highest momentum proton in the event between ProtonMinCut_MeV and
  // ProtonMaxCut_MeV MeV with cos(theta_p) > ProtonCosThetaCut
  TLorentzVector Pprot = MINERvAUtils::GetProtonInRange(
      event, ProtonMinCut_MeV, ProtonMaxCut_MeV, ProtonCosThetaCut);

  // Get highest momentum proton in allowed proton range
  TVector3 HadronP = Pprot.Vect();

  // If we don't have a CC0pi signal definition we also add in pion momentum
  if (!Is0pi) {
    if (event->NumFSParticle(PhysConst::pdg_pions) == 0) {
      return -9999;
    }
    // Count up pion momentum
    TLorentzVector pp = event->GetHMFSParticle(PhysConst::pdg_pions)->fP;
    HadronP += pp.Vect();
  }
  return FitUtils::GetDeltaPT(LeptonP, HadronP, NuP).Mag();
}

double MINERvAUtils::Get_STV_dphit_MINERvAPS(FitEvent *event,
                                             double ProtonMinCut_MeV,
                                             double ProtonMaxCut_MeV,
                                             double ProtonCosThetaCut,
                                             int ISPDG, bool Is0pi) {
  // Check that the neutrino exists
  if (event->NumISParticle(ISPDG) == 0) {
    return -9999;
  }
  // Return 0 if the proton or muon are missing
  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1)) == 0) {
    return -9999;
  }

  // Now get the TVector3s for each particle
  TVector3 const &NuP = event->GetHMISParticle(ISPDG)->fP.Vect();
  TVector3 const &LeptonP =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->fP.Vect();

  // Find the highest momentum proton in the event between ProtonMinCut_MeV and
  // ProtonMaxCut_MeV MeV with cos(theta_p) > ProtonCosThetaCut
  TLorentzVector Pprot = MINERvAUtils::GetProtonInRange(
      event, ProtonMinCut_MeV, ProtonMaxCut_MeV, ProtonCosThetaCut);
  TVector3 HadronP = Pprot.Vect();
  if (!Is0pi) {
    if (event->NumFSParticle(PhysConst::pdg_pions) == 0) {
      return -9999;
    }
    TLorentzVector pp = event->GetHMFSParticle(PhysConst::pdg_pions)->fP;
    HadronP += pp.Vect();
  }
  return FitUtils::GetDeltaPhiT(LeptonP, HadronP, NuP);
}

double MINERvAUtils::Get_STV_dalphat_MINERvAPS(FitEvent *event,
                                               double ProtonMinCut_MeV,
                                               double ProtonMaxCut_MeV,
                                               double ProtonCosThetaCut,
                                               int ISPDG, bool Is0pi) {
  // Check that the neutrino exists
  if (event->NumISParticle(ISPDG) == 0) {
    return -9999;
  }
  // Return 0 if the proton or muon are missing
  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1)) == 0) {
    return -9999;
  }

  // Now get the TVector3s for each particle
  TVector3 const &NuP = event->GetHMISParticle(ISPDG)->fP.Vect();
  TVector3 const &LeptonP =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->fP.Vect();

  // Find the highest momentum proton in the event between ProtonMinCut_MeV and
  // ProtonMaxCut_MeV MeV with cos(theta_p) > ProtonCosThetaCut
  TLorentzVector Pprot = MINERvAUtils::GetProtonInRange(
      event, ProtonMinCut_MeV, ProtonMaxCut_MeV, ProtonCosThetaCut);
  TVector3 HadronP = Pprot.Vect();
  if (!Is0pi) {
    if (event->NumFSParticle(PhysConst::pdg_pions) == 0) {
      return -9999;
    }
    TLorentzVector pp = event->GetHMFSParticle(PhysConst::pdg_pions)->fP;
    HadronP += pp.Vect();
  }
  return FitUtils::GetDeltaAlphaT(LeptonP, HadronP, NuP);
}

// As defined in PhysRevC.95.065501
// Using prescription from arXiv 1805.05486
// Returns in GeV
double MINERvAUtils::Get_pn_reco_C_MINERvAPS(FitEvent *event,
                                             double ProtonMinCut_MeV,
                                             double ProtonMaxCut_MeV,
                                             double ProtonCosThetaCut,
                                             int ISPDG, bool Is0pi) {

  const double mn = PhysConst::mass_neutron; // neutron mass
  const double mp = PhysConst::mass_proton;  // proton mass

  // Check that the neutrino exists
  if (event->NumISParticle(ISPDG) == 0) {
    return -9999;
  }
  // Return 0 if the proton or muon are missing
  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1)) == 0) {
    return -9999;
  }

  // Now get the TVector3s for each particle
  TVector3 const &NuP = event->GetHMISParticle(ISPDG)->fP.Vect();
  TVector3 const &LeptonP =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->fP.Vect();

  // Find the highest momentum proton in the event between ProtonMinCut_MeV and
  // ProtonMaxCut_MeV MeV with cos(theta_p) < ProtonCosThetaCut
  TLorentzVector Pprot = MINERvAUtils::GetProtonInRange(
      event, ProtonMinCut_MeV, ProtonMaxCut_MeV, ProtonCosThetaCut);
  TVector3 HadronP = Pprot.Vect();

  double const el =
      event->GetHMFSParticle(ISPDG + ((ISPDG < 0) ? 1 : -1))->E() / 1000.;
  double const eh = Pprot.E() / 1000.;

  if (!Is0pi) {
    if (event->NumFSParticle(PhysConst::pdg_pions) == 0) {
      return -9999;
    }
    TLorentzVector pp = event->GetHMFSParticle(PhysConst::pdg_pions)->fP;
    HadronP += pp.Vect();
  }
  TVector3 dpt = FitUtils::GetDeltaPT(LeptonP, HadronP, NuP);
  double dptMag = dpt.Mag() / 1000.;

  double ma =
      6 * mn + 6 * mp - 0.09216;  // target mass (E is from PhysRevC.95.065501)
  double map = ma - mn + 0.02713; // remnant mass

  double pmul = LeptonP.Dot(NuP.Unit()) / 1000.;
  double phl = HadronP.Dot(NuP.Unit()) / 1000.;

  // double pmul = GetVectorInTPlane(LeptonP, dpt).Mag()/1000.;
  // double phl = GetVectorInTPlane(HadronP, dpt).Mag()/1000.;

  double R = ma + pmul + phl - el - eh;

  double dpl = 0.5 * R - (map * map + dptMag * dptMag) / (2 * R);
  // double dpl = ((R*R)-(dptMag*dptMag)-(map*map))/(2*R); // as in in
  // PhysRevC.95.065501 - gives same result

  double pn_reco = sqrt((dptMag * dptMag) + (dpl * dpl));

  // std::cout << "Diagnostics: " << std::endl;
  // std::cout << "mn: " << mn << std::endl;
  // std::cout << "ma: " << ma << std::endl;
  // std::cout << "map: " << map << std::endl;
  // std::cout << "pmu: " << LeptonP.Mag()/1000. << std::endl;
  // std::cout << "ph: " << HadronP.Mag()/1000. << std::endl;
  // std::cout << "pmul: " << pmul << std::endl;
  // std::cout << "phl: " << phl << std::endl;
  // std::cout << "el: " << el << std::endl;
  // std::cout << "eh: " << eh << std::endl;
  // std::cout << "R: " << R << std::endl;
  // std::cout << "dptMag: " << dptMag << std::endl;
  // std::cout << "dpl: " << dpl << std::endl;
  // std::cout << "pn_reco: " << pn_reco << std::endl;

  return pn_reco;
}

// Find the highest momentum proton in the event between ProtonMinCut_MeV and
// ProtonMaxCut_MeV MeV with cos(theta_p) > ProtonCosThetaCut
TLorentzVector MINERvAUtils::GetProtonInRange(FitEvent *event,
                                              double ProtonMinCut_MeV,
                                              double ProtonMaxCut_MeV,
                                              double ProtonCosThetaCut) {
  // Get the neutrino
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  int HMFSProton = -1;
  double HighestMomentum = 0.0;
  // Get the stack of protons
  std::vector<FitParticle *> Protons = event->GetAllFSProton();
  for (size_t i = 0; i < Protons.size(); ++i) {
    if (Protons[i]->p() > ProtonMinCut_MeV &&
        Protons[i]->p() < ProtonMaxCut_MeV &&
        cos(Protons[i]->P3().Angle(Pnu.Vect())) > ProtonCosThetaCut &&
        Protons[i]->p() > HighestMomentum) {
      HighestMomentum = Protons[i]->p();
      HMFSProton = i;
    }
  }
  if (HMFSProton == -1) {
    return TLorentzVector(0, 0, 0, 0);
  }
  // Now get the proton
  TLorentzVector Pprot = Protons[HMFSProton]->fP;
  return Pprot;
}
