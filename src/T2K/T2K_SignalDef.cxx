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

#include "FitUtils.h"
#include "T2K_SignalDef.h"

namespace SignalDef {

// T2K H2O signal definition
bool isCC1pip_T2K_H2O(FitEvent *event, double EnuMin,
                                 double EnuMax) {

  if (!isCC1pi(event, 14, 211, EnuMin, EnuMax)) return false;

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
// MOVE T2K
bool isCC1pip_T2K_CH(FitEvent *event, double EnuMin, double EnuMax,
                                bool MichelElectron) {
  // ******************************************************

  if (!isCC1pi(event, 14, 211, EnuMin, EnuMax)) return false;

  TLorentzVector Pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;

  // If this event passes the criteria on particle counting, enforce the T2K
  // ND280 phase space constraints
  // Will be different if Michel tag sample is included or not
  // Essentially, if there's a Michel tag we don't cut on the pion variables

  double p_mu = FitUtils::p(Pmu) * 1000;
  double p_pi = FitUtils::p(Ppip) * 1000;
  double cos_th_mu = cos(FitUtils::th(Pnu, Pmu));
  double cos_th_pi = cos(FitUtils::th(Pnu, Ppip));

  // If we're using Michel e- requirement we only care about the muon restricted
  // phase space and use full pion phase space
  if (MichelElectron) {
    // Make the cuts on the muon variables
    if (p_mu <= 200 || cos_th_mu <= 0.2) {
      return false;
    } else {
      return true;
    }

    // If we aren't using Michel e- (i.e. we use directional information from
    // pion) we need to impose the phase space cuts on the muon AND the pion)
  } else {
    // Make the cuts on muon and pion variables
    if (p_mu <= 200 || p_pi <= 200 || cos_th_mu <= 0.2 || cos_th_pi <= 0.2) {
      return false;
    } else {
      return true;
    }
  }

  // Default to false; should never fire
  return false;
};

bool isT2K_CC0pi(FitEvent *event, double EnuMin, double EnuMax,
                            bool forwardgoing) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax)) return false;

  TLorentzVector Pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));
  double p_mu = Pmu.Vect().Mag();

  // If we're doing a restricted phase space, Analysis II asks for:
  // Cos(theta_mu) > 0.0 and p_mu > 200 MeV
  if (forwardgoing) {
    if (CosThetaMu < 0.0 || p_mu < 200) return false;
  }

  return true;
}

bool isT2K_CC0pi1p(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax)) return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0) return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp  = event->GetHMFSParticle(2212)->fP;


  // Proton phase space
  if (pp.Vect().Mag() < 500) {
    return false;
  }

  //Need exactly one proton with 500 MeV or more momentum
  std::vector<FitParticle*> protons = event->GetAllFSProton();
  int nProtonsAboveThresh=0;
  for(int i=0; i<protons.size(); i++){
    if(protons[i]->p()>500) nProtonsAboveThresh++;
  }
  if(nProtonsAboveThresh!=1) return false;

  return true;
}


//CC0pi antinu in the P0D - TN328
bool isT2K_CC0piAnuP0D(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a anumu CC0pi event
  if (!isCC0pi(event, -14, EnuMin, EnuMax)) return false;


  TLorentzVector pnu = event->GetHMISParticle(-14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(-13)->fP;
  double Pmu = pmu.Vect().Mag();
  double CosThetaMu = cos(pnu.Vect().Angle(pmu.Vect()));
  // Muon phase space
  if (Pmu < 400 || Pmu > 3410) return false;
  if (Pmu < 530 && CosThetaMu<0.85) return false;
  if (Pmu < 670 && CosThetaMu<0.88) return false;
  if (Pmu < 800 && CosThetaMu<0.9) return false;
  if (Pmu < 1000 && CosThetaMu<0.91) return false;
  if (Pmu < 1380 && CosThetaMu<0.92) return false;
  if (Pmu < 2010 && CosThetaMu<0.95) return false;

 
  return true;
}

bool isT2K_CC0piNp(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax)) return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0) return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp  = event->GetHMFSParticle(2212)->fP;

  // Proton phase space
  if (pp.Vect().Mag() < 500) {
    return false;
  }

  //Need exactly one proton with 500 MeV or more momentum
  std::vector<FitParticle*> protons = event->GetAllFSProton();
  int nProtonsAboveThresh=0;
  for(int i=0; i<protons.size(); i++){
    if(protons[i]->p()>500) nProtonsAboveThresh++;
  }
  if(nProtonsAboveThresh<2) return false;

  return true;
}

bool isT2K_CC0pi0p(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax)) return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0) return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp  = event->GetHMFSParticle(2212)->fP;


  // Proton phase space
  if (pp.Vect().Mag() > 500) {
    return false;
  }

  return true;
}


bool isT2K_CC0pi_STV(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax)) return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0) return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp  = event->GetHMFSParticle(2212)->fP;

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
  if (!isCC0pi(event, 14, EnuMin, EnuMax)) return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0) return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp  = event->GetHMFSParticle(2212)->fP;

  // Proton phase space
  // Pprot > 450 MeV, cos(theta_proton) > 0.4
  if ((pp.Vect().Mag() < 450) || (cos(pnu.Vect().Angle(pp.Vect())) < 0.4)) {
    return false;
  }

  return true;
}

bool isT2K_CC0pi_1bin(FitEvent *event, double EnuMin, double EnuMax) {

  // Require a numu CC0pi event
  if (!isCC0pi(event, 14, EnuMin, EnuMax)) return false;

  // Require at least one FS proton
  if (event->NumFSParticle(2212) == 0) return false;

  TLorentzVector pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector pp  = event->GetHMFSParticle(2212)->fP;

  // Muon phase space
  //if ((pmu.Vect().Mag() < 250) || cos(pnu.Vect().Angle(pmu.Vect())) < -0.6) {
  //  return false;
  //}

  // Proton phase space
  if ((pp.Vect().Mag() < 450) || (cos(pnu.Vect().Angle(pp.Vect())) < 0.4)) {
    return false;
  }

  return true;
}

}
