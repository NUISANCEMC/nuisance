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

#ifndef FITUTILS_H_SEEN
#define FITUTILS_H_SEEN

// C Includes
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <numeric>

// ROOT includes
#include <TChain.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <THStack.h>
#include <TKey.h>
#include <TLegend.h>
#include <TList.h>
#include <TLorentzVector.h>
#include <TObjArray.h>
#include <TROOT.h>
#include <TRandom3.h>
#include <TTree.h>
#include "FitEvent.h"
#include "TGraph.h"
#include "TH2Poly.h"

// Fit  includes
#include "FitParameters.h"

/*!
 *  \addtogroup Utils
 *  @{
 */

//! Functions needed by individual samples for calculating kinematic quantities.
namespace FitUtils {

/*
  MISC
*/

//! Function to produce a histogram indicating the fraction of events which have
//! q3 < qCut
TH2D *CalculateQ3Cut(std::string inFile, TH2D *data, double qCut, int nuPDG,
                     double eMin, double eMax);

//! Return a vector of all values saved in map
double *GetArrayFromMap(std::vector<std::string> invals,
                        std::map<std::string, double> inmap);

/*
  MISC Event
*/

//! Returns kinetic energy of particle
double T(TLorentzVector part);

//! Returns momentum of particle
double p(TLorentzVector part);

//! Returns angle between particles (_NOT_ cosine!)
double th(TLorentzVector part, TLorentzVector part2);

//! Hadronic mass reconstruction
double Wrec(TLorentzVector pnu, TLorentzVector pmu);

//! Hadronic mass true from initial state particles and muon; useful if the full
//! FSI vectors aren't not saved and we for some reasons need W_true
double Wtrue(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector pnuc);

/*
  E Recoil
*/
double GetErecoil_TRUE(FitEvent *event);
double GetErecoil_CHARGED(FitEvent *event);

/*
  CCQE MiniBooNE/MINERvA
*/
//! Function to calculate the reconstructed Q^{2}_{QE}
double Q2QErec(TLorentzVector pmu, double costh, double binding,
               bool neutrino = true);

//! Function returns the reconstructed E_{nu} values
double EnuQErec(TLorentzVector pmu, double costh, double binding,
                bool neutrino = true);

/*
  CCQE1p MINERvA
*/
//! Reconstruct Q2QE given just the maximum energy proton.
double ProtonQ2QErec(double pE, double binding);

/*
  E Recoil MINERvA
*/
double GetErecoil_MINERvA_LowRecoil(FitEvent *event);

/*
  CC1pi0 MiniBooNE
*/
//! Reconstruct Enu from CCpi0 vectors and binding energy
double EnuCC1pi0rec(TLorentzVector pnu, TLorentzVector pmu,
                    TLorentzVector ppi0 = TLorentzVector(0, 0, 0, 0));

//! Reconstruct Q2 from CCpi0 vectors and binding energy
double Q2CC1pi0rec(TLorentzVector pnu, TLorentzVector pmu,
                   TLorentzVector ppi0 = TLorentzVector(0, 0, 0, 0));

/*
  CC1pi+ MiniBooNE
*/

//! returns reconstructed Enu a la MiniBooNE CCpi+
//! returns reconstructed Enu a la MiniBooNE CCpi+
// Also for when not having pion info (so when we have a Michel tag in T2K)
double EnuCC1piprec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppip,
                    bool pionInfo = true);

//! returns reconstructed Enu assumming resonance interaction where intermediate
//! resonance was a Delta
double EnuCC1piprecDelta(TLorentzVector pnu, TLorentzVector pmu);

//! returns reconstructed in a variety of flavours
double Q2CC1piprec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppip,
                   int enuType = 0, bool pionInfo = true);

/*
  T2K CC1pi+ on CH
*/
double thq3pi_CC1pip_T2K(TLorentzVector pnu, TLorentzVector pmu,
                         TLorentzVector ppi);
double q3_CC1pip_T2K(TLorentzVector pnu, TLorentzVector pmu,
                     TLorentzVector ppi);
double WrecCC1pip_T2K_MB(TLorentzVector pnu, TLorentzVector pmu,
                         TLorentzVector ppip);
double EnuCC1piprec_T2K_eMB(TLorentzVector pnu, TLorentzVector pmu,
                            TLorentzVector ppi);

/*
  nucleon single pion
*/
double MpPi(TLorentzVector pp, TLorentzVector ppi);

template <size_t N>
std::pair<TLorentzVector, int> GetHMPDG_4Mom(int const (&pdg)[N],
                                             FitEvent *event,
                                             bool InitalState = false) {
  TLorentzVector HM(0, 0, 0, 0);
  int OPDG = 0;
  UInt_t const &np = event->NPart();
  for (size_t p_it = 0; p_it < np; ++p_it) {
    FitParticle const &part = *event->PartInfo(p_it);
    if ((InitalState && (part.Status() != kInitialState)) ||
        (!InitalState && (part.Status() != kFinalState))) {
      continue;
    }

    for (size_t pdg_it = 0; pdg_it < N; ++pdg_it) {
      if ((part.fPID == pdg[pdg_it]) &&
          (part.fP.Vect().Mag2() > HM.Vect().Mag2())) {
        OPDG = part.fPID;
        HM = part.fP;
      }
    }
  }
  return std::make_pair(HM, OPDG);
}

/// Gets the 4 mom of the highest momentum alive particle with a PDG
/// contained equal to pdg.
std::pair<TLorentzVector, int> GetHMPDG_4Mom(int pdg, FitEvent *event);
/// Gets the 4 mom of the highest momentum final state charged lepton.
std::pair<TLorentzVector, int> GetHMFSCLepton_4Mom(FitEvent *event);
/// Gets the 4 mom of the highest momentum final state muon.
std::pair<TLorentzVector, int> GetHMFSMuon_4Mom(FitEvent *event);
/// Gets the 4 mom of the highest momentum final state anti-muon.
std::pair<TLorentzVector, int> GetHMFSAntiMuon_4Mom(FitEvent *event);
/// Gets the 4 mom of the highest momentum initial state neutral lepton.
std::pair<TLorentzVector, int> GetHMISNLepton_4Mom(FitEvent *event);
/// Gets the 4 mom of the highest momentum initial state muon neutrino.
std::pair<TLorentzVector, int> GetHMISMuonNeutrino_4Mom(FitEvent *event);
/// Gets the 4 mom of the highest momentum initial state anti-muon neutrino.
std::pair<TLorentzVector, int> GetHMISAntiMuonNeutrino_4Mom(FitEvent *event);
/// Gets the 4 mom of the highest momentum final state proton.
std::pair<TLorentzVector, int> GetHMFSProton_4Mom(FitEvent *event);
/// Gets the 4 mom of the highest momentum final state charged pion.
std::pair<TLorentzVector, int> GetHMFSCPion_4Mom(FitEvent *event);
/// Gets the 4 mom of the highest momentum final state pion.
std::pair<TLorentzVector, int> GetHMFSPion_4Mom(FitEvent *event);

/// Gets delta p T as defined in Phys.Rev. C94 (2016) no.1, 015503
double Get_STV_dpt(FitEvent *event, bool Is0pi);
/// Gets delta phi T as defined in Phys.Rev. C94 (2016) no.1, 015503
double Get_STV_dphit(FitEvent *event, bool Is0pi);
/// Gets delta alpha T as defined in Phys.Rev. C94 (2016) no.1, 015503
double Get_STV_dalphat(FitEvent *event, bool Is0pi);
}

/*! @} */
#endif
