// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "T2K_SignalDef.h"
#include "FitUtils.h"

namespace SignalDef {

// T2K H2O signal definition
// https://doi.org/10.1103/PhysRevD.97.012001
bool isCC1pip_T2K_PRD97_012001(FitEvent *event, double EnuMin, double EnuMax) {

  if (!isCC1pi(event, 14, 211, EnuMin, EnuMax))
    return false;

  TLorentzVector Pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;

  double p_mu = FitUtils::p(Pmu) * 1000;
  double p_pi = FitUtils::p(Ppip) * 1000;
  double cos_th_mu = cos(FitUtils::th(Pnu, Pmu));
  double cos_th_pi = cos(FitUtils::th(Pnu, Ppip));

  if (p_mu <= 200 || p_pi <= 200 || cos_th_mu <= 0.3 || cos_th_pi <= 0.3) {
    return false;
  }

  return true;
};

// ******************************************************
// T2K CC1pi+ CH analysis (Raquel's thesis)
// Has different phase space cuts depending on if using Michel tag or not
//
// Essentially consists of two samples: one sample which has Michel e (which we
// can't get pion direction from); this covers backwards angles quite well.
// Measurements including this sample does not have include pion kinematics cuts
//                                      one sample which has PID in FGD and TPC
//                                      and no Michel e. These are mostly
//                                      forward-going so require a pion
//                                      kinematics cut
//
//  Essentially, cuts are:
//                          1 negative muon
//                          1 positive pion
//                          Any number of nucleons
//                          No other particles in the final state
//
// https://arxiv.org/abs/1909.03936
bool isCC1pip_T2K_arxiv1909_03936(FitEvent *event, double EnuMin, double EnuMax,
                                  int pscuts) {
  // ******************************************************

  if (!isCC1pi(event, 14, 211, EnuMin, EnuMax)) {
    return false;
  }

  TLorentzVector Pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double cos_th_mu = cos(FitUtils::th(Pnu, Pmu));

  if (pscuts == kMuonFwd) {
    return (cos_th_mu > 0);
  }

  double p_mu = FitUtils::p(Pmu) * 1000;

  if (pscuts & kMuonHighEff) {
    if ((cos_th_mu <= 0.2) || (p_mu <= 200)) {
      return false;
    }
  }

  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  double cos_th_pi = cos(FitUtils::th(Pnu, Ppip));
  double p_pi = FitUtils::p(Ppip) * 1000;


  if ((pscuts & kPionFwd) && (cos_th_pi <= 0)) {
    return false;
  }

  if ((pscuts & kPionVFwd) && (cos_th_pi <= 0.2)) {
    return false;
  }

  if ((pscuts & kPionHighMom) && (p_pi <= 200)) {
    return false;
  }

  return true;
};

bool isT2K_CC0pi(FitEvent *event, double EnuMin, double EnuMax, int ana) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax))
    return false;

  TLorentzVector Pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));
  double p_mu = Pmu.Vect().Mag();

  // If we're doing a restricted phase space, Analysis II asks for:
  // Cos(theta_mu) > 0.0 and p_mu > 200 MeV
  if (ana == kAnalysis_II) {
    if ((CosThetaMu < 0.0) || (p_mu < 200)) {
      return false;
    }
  }

  return true;
}

bool isT2K_CC0pi1p(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax))
    return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0)
    return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp = event->GetHMFSParticle(2212)->fP;

  // Proton phase space
  if (pp.Vect().Mag() < 500) {
    return false;
  }

  // Need exactly one proton with 500 MeV or more momentum
  std::vector<FitParticle *> protons = event->GetAllFSProton();
  int nProtonsAboveThresh = 0;
  for (size_t i = 0; i < protons.size(); i++) {
    if (protons[i]->p() > 500)
      nProtonsAboveThresh++;
  }
  if (nProtonsAboveThresh != 1)
    return false;

  return true;
}

bool isT2K_CC0piNp(FitEvent *event, double EnuMin, double EnuMax) {
  // In this case, we specifically mean N>2, as defined in the T2K CC0pi 2018 paper

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax))
    return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0)
    return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp = event->GetHMFSParticle(2212)->fP;

  // Proton phase space
  if (pp.Vect().Mag() < 500) {
    return false;
  }

  // Need exactly one proton with 500 MeV or more momentum
  std::vector<FitParticle *> protons = event->GetAllFSProton();
  int nProtonsAboveThresh = 0;
  for (size_t i = 0; i < protons.size(); i++) {
    if (protons[i]->p() > 500)
      nProtonsAboveThresh++;
  }
  if (nProtonsAboveThresh <= 1)
    return false;

  return true;
}

bool isT2K_CC0pi0p(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax))
    return false;

  // Return true if no FS proton
  if (event->NumFSParticle(2212) == 0)
    return true;

  // Otherwise, check momentum of highest-momentum proton, and require it is below threshold
  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp = event->GetHMFSParticle(2212)->fP;

  // Proton phase space
  if (pp.Vect().Mag() < 500) {
    return true;
  }

  // If there are FS protons and the highest-momentum one is above threshold, return false
  return false;
}

// CC0pi antinu in the P0D https://arxiv.org/abs/1908.10249
bool isT2K_CC0piAnuP0D(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a anumu CC0pi event
  if (!isCC0pi(event, -14, EnuMin, EnuMax))
    return false;

  TLorentzVector pnu = event->GetHMISParticle(-14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(-13)->fP;
  double Pmu = pmu.Vect().Mag();
  double CosThetaMu = cos(pnu.Vect().Angle(pmu.Vect()));
  // Muon phase space
  if (Pmu < 400 || Pmu > 3410) return false;
  if (Pmu < 530 && Pmu>=400 && CosThetaMu<0.84) return false;
  if (Pmu < 670 && Pmu>=530 && CosThetaMu<0.85) return false;
  if (Pmu < 800 && Pmu>=670 && CosThetaMu<0.88) return false;
  if (Pmu < 1000 &&Pmu>=800 && CosThetaMu<0.9) return false;
  if (Pmu < 1380 && Pmu>=1000 && CosThetaMu<0.91) return false;
  if (Pmu < 2010 && Pmu>=1380 && CosThetaMu<0.92) return false;
  if (Pmu < 3410 && Pmu>=2010 && CosThetaMu<0.95) return false;

  return true;
}

bool isT2K_CC0pi_STV(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax))
    return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0)
    return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp = event->GetHMFSParticle(2212)->fP;

  // Muon phase space
  // Pmu > 250 MeV, cos(theta_mu) > -0.6 (Sweet phase space!)
  if ((pmu.Vect().Mag() < 250) || cos(pnu.Vect().Angle(pmu.Vect())) < -0.6) {
    return false;
  }

  // Proton phase space
  // Pprot > 450 MeV, cos(theta_proton) > 0.4
  if ((pp.Vect().Mag() < 450) || (pp.Vect().Mag() > 1E3) ||
      (cos(pnu.Vect().Angle(pp.Vect())) < 0.4)) {
    return false;
  }

  return true;
}

bool isT2K_CC0pi_ifk(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax))
    return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0)
    return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp = event->GetHMFSParticle(2212)->fP;

  // Proton phase space
  // Pprot > 450 MeV, cos(theta_proton) > 0.4
  if ((pp.Vect().Mag() < 450) || (cos(pnu.Vect().Angle(pp.Vect())) < 0.4)) {
    return false;
  }

  return true;
}

bool isT2K_CC0pi_1bin(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax))
    return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0)
    return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp = event->GetHMFSParticle(2212)->fP;

  // Muon phase space
  // if ((pmu.Vect().Mag() < 250) || cos(pnu.Vect().Angle(pmu.Vect())) < -0.6) {
  //  return false;
  //}

  // Proton phase space
  if ((pp.Vect().Mag() < 450) || (cos(pnu.Vect().Angle(pp.Vect())) < 0.4)) {
    return false;
  }

  return true;
}

// T2K CC1pi+1p 
bool isT2K_CC1pipNp_STV(FitEvent *event) {

  // Check that it's CC1pi, no energy cuts
  if (!isCC1pi(event, 14, 211, 0, 100)) return false;

  // Get the protons
  std::vector<FitParticle*> protons = event->GetAllFSProton();
  if (protons.size() == 0) return false;

  // Get the neutrino to do the direction
  FitParticle* Nu = event->GetNeutrinoIn();

  const double protlo = 450;
  const double prothi = 1200;
  int nprot = 0;
  int protindex = 0;

  //int npip = 0;
  //int piindex = 0;
  const double pilo = 150;
  const double pihi = 1200;

  //int nmu = 0;
  //int muindex = 0;
  const double mulo = 250;
  const double muhi = 7000;

  const double angular = 70.*M_PI/180.; // 70 degree cut
  for (int i = 0; i < protons.size(); ++i) {
    // First check protons in range
    //if (protons[i]->PDG() == 2212 && 
    if (protons[i]->fP.Vect().Mag() > protlo && 
        protons[i]->fP.Vect().Mag() < prothi &&
        protons[i]->fP.Vect().Angle(Nu->fP.Vect()) < angular) {

      nprot++;
      // If we haven't seen a proton yet, save it
      if (protindex == 0) {
        protindex = i;
        // If we have seen a proton, check if this proton has higher momentum
        // If so, save it
      } else if ( protons[i]->fP.Vect().Mag() > 
          protons[protindex]->fP.Vect().Mag()) {
        protindex = i;
      }
    }

    /*
    // Not clear here if it asks for a single pion within threshold
    // or if it asks that the highest momentum pion is within threshold
    else if (particles[i]->PDG() == 211 &&
        particles[i]->fP.Vect().Mag() > pilo && 
        particles[i]->fP.Vect().Mag() < pihi &&
        particles[i]->fP.Vect().Angle(Nu->fP.Vect()) < angular) {
      npip++;
      piindex = i;
    }

    // Then check muon
    else if (particles[i]->PDG() == 13 &&
      particles[i]->fP.Vect().Mag() > mulo &&
      particles[i]->fP.Vect().Mag() < muhi &&
      particles[i]->fP.Vect().Angle(Nu->fP.Vect()) < angular) {
      nmu++;
      muindex = i;
    }
    */
  } // end loop over protons particles

  // Need to have more than one proton in range
  if (nprot == 0) return false;

  // Check the muon
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  if (pmu.Vect().Mag() < mulo || 
      pmu.Vect().Mag() > muhi ||
      pmu.Vect().Angle(Nu->fP.Vect()) > angular) {
    return false;
  }

  // Check the charged pion
  TLorentzVector ppi = event->GetHMFSParticle(211)->fP;
  if (ppi.Vect().Mag() < pilo || 
      ppi.Vect().Mag() > pihi ||
      ppi.Vect().Angle(Nu->fP.Vect()) > angular) {
    return false;
  }

  // Have exactly one pion in range
  //if (npip != 1) return false;
  // And exactly one muon in range
  //if (nmu != 1) return false;

  return true;
}

} // namespace SignalDef
