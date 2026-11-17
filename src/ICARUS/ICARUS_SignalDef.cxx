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

#include "FitUtils.h"
#include "SignalDef.h"

#include "ICARUS_SignalDef.h"
#include "ICARUSUtils.h"

namespace SignalDef {

helper_ICARUS_NuMI_CCQELike_numu isICARUS_NuMI_CCQELike_numu(FitEvent *event, double emin, double emax) {

  helper_ICARUS_NuMI_CCQELike_numu out;
  out.IsSignal = false;
  out.IsAntiNu = false;
  out.IsMuonPLT0p8 = false;
  out.MuonCos = -999.;
  out.MuonProtonCos = -999.;
  out.deltaPT = -999.;
  out.deltaalphaT = -999.;

  FitParticle* nu_cand = event->GetHMISParticle(14);
  if(!nu_cand) nu_cand = event->GetHMISParticle(-14);

  TLorentzVector pnu = nu_cand->fP;
  if( (pnu.E()/1000. < emin) || (pnu.E()/1000. >= emax) ) return out;

  unsigned int nMu_1muNp0pi(0), nP_1muNp0pi(0), nPi_1muNp0pi(0);
  unsigned int nPhoton_1muNp0pi(0), nMesons_1muNp0pi(0), nBaryonsAndPi0_1muNp0pi(0);
  double maxMomentumP_1muNp0pi = -999.;
  bool passProtonPCut_1muNp0pi = false;

  std::vector<FitParticle *> protons;

  // Start Particle Loop
  UInt_t npart = event->Npart();
  for (UInt_t i = 0; i < npart; i++) {
    // Skip particles that weren't in the final state
    bool part_alive = event->PartInfo(i)->fIsAlive and
                      event->PartInfo(i)->Status() == kFinalState;
    if (!part_alive)
      continue;

    // PDG Particle
    int pdgc = event->PartInfo(i)->fPID;
    TLorentzVector part_4mom = event->PartInfo(i)->fP;

    // ICARUS 1muNp0pi

    double momentum = part_4mom.Vect().Mag()/1000.;

    bool PassMuonPCut = (momentum > 0.226);
    if ( abs(pdgc) == 13 ) {
      if (PassMuonPCut) nMu_1muNp0pi+=1;
    }

    if ( abs(pdgc) == 2212 ) {
      protons.push_back(event->PartInfo(i));
      nP_1muNp0pi+=1;
      if ( momentum > maxMomentumP_1muNp0pi ) {
        maxMomentumP_1muNp0pi = momentum;
        passProtonPCut_1muNp0pi = (momentum > 0.4 && momentum < 1.);
      }
    }

    if ( abs(pdgc) == 111 || abs(pdgc) == 211 ) nPi_1muNp0pi+=1;
    // CHECK A SIMILAR DEFINITION AS MINERVA FOR EXTRA REJECTION OF UNWANTED THINGS IN SIGNAL DEFN.
    if ( abs(pdgc) == 22 && part_4mom.E()/1000. > 0.01 ) nPhoton_1muNp0pi+=1;
    else if ( abs(pdgc) == 211 || abs(pdgc) == 321 || abs(pdgc) == 323 ||
              pdgc == 111 || pdgc == 130 || pdgc == 310 || pdgc == 311 ||
              pdgc == 313 || abs(pdgc) == 221 || abs(pdgc) == 331 ) nMesons_1muNp0pi+=1;
    else if ( pdgc == 3112 || pdgc == 3122 || pdgc == 3212 || pdgc == 3222 ||
              pdgc == 4112 || pdgc == 4122 || pdgc == 4212 || pdgc == 4222 ||
              pdgc == 411 || pdgc == 421 || pdgc == 111 ) nBaryonsAndPi0_1muNp0pi+=1;


  }

  bool ICARUS_1muNp0pi_IsSignal = nMu_1muNp0pi==1 &&
                             nP_1muNp0pi>0 && passProtonPCut_1muNp0pi &&
                             nPi_1muNp0pi==0 &&
                             nPhoton_1muNp0pi==0 &&
                             nMesons_1muNp0pi==0 &&
                             nBaryonsAndPi0_1muNp0pi==0;

  if(!ICARUS_1muNp0pi_IsSignal) return out;

  bool IsAntiNu = event->GetNeutrinoIn()->fPID<0;

  TLorentzVector Pmu = event->GetHMFSParticle(IsAntiNu ? -13 : +13)->fP;
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  // - Sort protons in descending order of KE
  std::sort(protons.begin(), protons.end(),
            [](FitParticle* a, FitParticle* b) {
                return a->KE() > b->KE();
            });

  out.IsSignal = true;
  out.IsAntiNu = IsAntiNu;
  out.IsMuonPLT0p8 = (Pmu.Vect().Mag()/1000.)<0.8;
  out.MuonCos = cos( Pmu.Vect().Angle( Pnu.Vect() ) );
  out.MuonProtonCos = cos( Pmu.Vect().Angle( protons[0]->fP.Vect() ) );
  out.deltaPT = FitUtils::CalcTKI_deltaPT(Pmu.Vect(), protons[0]->fP.Vect(), Pnu.Vect())/1000.;
  out.deltaalphaT =FitUtils::CalcTKI_deltaalphaT(Pmu.Vect(), protons[0]->fP.Vect(), Pnu.Vect());

  return out;

}

} // namespace SignalDef
