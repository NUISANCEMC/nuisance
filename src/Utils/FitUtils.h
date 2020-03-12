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

#include <ctime>
#include <iostream>
#include <math.h>
#include <numeric>
#include <stdlib.h>
#include <unistd.h>

#include "FitEvent.h"
#include "TGraph.h"
#include "TH2Poly.h"
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

#include "FitLogger.h"

/*!
 *  \addtogroup Utils
 *  @{
 */

/// Functions needed by individual samples for calculating kinematic quantities.
namespace FitUtils {

/// Return a vector of all values saved in map
double *GetArrayFromMap(std::vector<std::string> invals,
                        std::map<std::string, double> inmap);

/// Returns kinetic energy of particle
double T(TLorentzVector part);

/// Returns momentum of particle
double p(TLorentzVector part);
double p(FitParticle *part);

/// Returns angle between particles (_NOT_ cosine!)
double th(TLorentzVector part, TLorentzVector part2);
double th(FitParticle *part1, FitParticle *part2);

/// Hadronic mass reconstruction
double Wrec(TLorentzVector pnu, TLorentzVector pmu);

/// Hadronic mass true from initial state particles and muon; useful if the full
/// FSI vectors aren't not saved and we for some reasons need W_true
double Wtrue(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector pnuc);

double SumKE_PartVect(std::vector<FitParticle *> const fps);
double SumTE_PartVect(std::vector<FitParticle *> const fps);

/// Return E Hadronic for all FS Particles in Hadronic System
double GetErecoil_TRUE(FitEvent *event);

/// Return E Hadronic for all Charged FS Particles in Hadronic System
double GetErecoil_CHARGED(FitEvent *event);
double Eavailable(FitEvent *event);

/*
  CCQE MiniBooNE/MINERvA
*/
/// Function to calculate the reconstructed Q^{2}_{QE}
double Q2QErec(TLorentzVector pmu, double costh, double binding,
               bool neutrino = true);

/// Function returns the reconstructed E_{nu} values
double EnuQErec(TLorentzVector pmu, double costh, double binding,
                bool neutrino = true);

/// Function returns the reconstructed E_{nu} values
double EnuQErec(TLorentzVector pmu, TLorentzVector pnu, double binding,
                bool neutrino = true);

//! Function to calculate the reconstructed Q^{2}_{QE}
double Q2QErec(double pl, double costh, double binding, bool neutrino = true);

//! Function to calculate the reconstructed Q^{2}_{QE}
double Q2QErec(TLorentzVector Pmu, TLorentzVector Pnu, double binding,
               bool neutrino = true);

//! Function returns the reconstructed E_{nu} values
double EnuQErec(double pl, double costh, double binding, bool neutrino = true);

/*
  CCQE1p MINERvA
*/
/// Reconstruct Q2QE given just the maximum energy proton.
double ProtonQ2QErec(double pE, double binding);

/*
  E Recoil MINERvA
*/
double GetErecoil_MINERvA_LowRecoil(FitEvent *event);

/*
  CC1pi0 MiniBooNE
*/
/// Reconstruct Enu from CCpi0 vectors and binding energy
double EnuCC1pi0rec(TLorentzVector pnu, TLorentzVector pmu,
                    TLorentzVector ppi0 = TLorentzVector(0, 0, 0, 0));

/// Reconstruct Q2 from CCpi0 vectors and binding energy
double Q2CC1pi0rec(TLorentzVector pnu, TLorentzVector pmu,
                   TLorentzVector ppi0 = TLorentzVector(0, 0, 0, 0));

/*
  CC1pi+ MiniBooNE
*/

/// returns reconstructed Enu a la MiniBooNE CCpi+
/// returns reconstructed Enu a la MiniBooNE CCpi+
// Also for when not having pion info (so when we have a Michel tag in T2K)
double EnuCC1piprec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppip,
                    bool pionInfo = true);

/// returns reconstructed Enu assumming resonance interaction where intermediate
/// resonance was a Delta
double EnuCC1piprecDelta(TLorentzVector pnu, TLorentzVector pmu);

/// returns reconstructed in a variety of flavours
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

const double kDefaultSTV_ProtonMinCut = 0;
const double kDefaultSTV_ProtonMaxCut = 100E3;
const double kDefaultSTV_PProtonCosThetaCut = -1;

/// Gets delta p T as defined in Phys.Rev. C94 (2016) no.1, 015503
double
Get_STV_dpt_protonps(FitEvent *event,
                     double ProtonMinCut = kDefaultSTV_ProtonMinCut,
                     double ProtonMaxCut = kDefaultSTV_ProtonMaxCut,
                     double ProtonCosThetaCut = kDefaultSTV_PProtonCosThetaCut,
                     int ISPDG = 14, bool Is0pi = true);
/// Gets delta phi T as defined in Phys.Rev. C94 (2016) no.1, 015503
double Get_STV_dphit_protonps(
    FitEvent *event, double ProtonMinCut = kDefaultSTV_ProtonMinCut,
    double ProtonMaxCut = kDefaultSTV_ProtonMaxCut,
    double ProtonCosThetaCut = kDefaultSTV_PProtonCosThetaCut, int ISPDG = 14,
    bool Is0pi = true);
/// Gets delta alpha T as defined in Phys.Rev. C94 (2016) no.1, 015503
double Get_STV_dalphat_protonps(
    FitEvent *event, double ProtonMinCut = kDefaultSTV_ProtonMinCut,
    double ProtonMaxCut = kDefaultSTV_ProtonMaxCut,
    double ProtonCosThetaCut = kDefaultSTV_PProtonCosThetaCut, int ISPDG = 14,
    bool Is0pi = true);

inline double Get_STV_dpt(FitEvent *event, int ISPDG, bool Is0pi) {
  return Get_STV_dpt_protonps(event, kDefaultSTV_ProtonMinCut,
                              kDefaultSTV_ProtonMaxCut,
                              kDefaultSTV_PProtonCosThetaCut, ISPDG, Is0pi);
}
inline double Get_STV_dphit(FitEvent *event, int ISPDG, bool Is0pi) {
  return Get_STV_dphit_protonps(event, kDefaultSTV_ProtonMinCut,
                                kDefaultSTV_ProtonMaxCut,
                                kDefaultSTV_PProtonCosThetaCut, ISPDG, Is0pi);
}
inline double Get_STV_dalphat(FitEvent *event, int ISPDG, bool Is0pi) {
  return Get_STV_dalphat_protonps(event, kDefaultSTV_ProtonMinCut,
                                  kDefaultSTV_ProtonMaxCut,
                                  kDefaultSTV_PProtonCosThetaCut, ISPDG, Is0pi);
}

// As defined in PhysRevC.95.065501
// Using prescription from arXiv 1805.05486
double Get_pn_reco_C_protonps(
    FitEvent *event, double ProtonMinCut = kDefaultSTV_ProtonMinCut,
    double ProtonMaxCut = kDefaultSTV_ProtonMaxCut,
    double ProtonCosThetaCut = kDefaultSTV_PProtonCosThetaCut, int ISPDG = 14,
    bool Is0pi = true);

double Get_pn_reco_Ar_protonps(
    FitEvent *event, double ProtonMinCut = kDefaultSTV_ProtonMinCut,
    double ProtonMaxCut = kDefaultSTV_ProtonMaxCut,
    double ProtonCosThetaCut = kDefaultSTV_PProtonCosThetaCut, int ISPDG = 14,
    bool Is0pi = true);

inline double Get_pn_reco_C(FitEvent *event, int ISPDG, bool Is0pi) {
  return Get_pn_reco_C_protonps(event, kDefaultSTV_ProtonMinCut,
                                  kDefaultSTV_ProtonMaxCut,
                                  kDefaultSTV_PProtonCosThetaCut, ISPDG, Is0pi);
}

inline double Get_pn_reco_Ar(FitEvent *event, int ISPDG, bool Is0pi) {
  return Get_pn_reco_Ar_protonps(event, kDefaultSTV_ProtonMinCut,
                                 kDefaultSTV_ProtonMaxCut,
                                 kDefaultSTV_PProtonCosThetaCut, ISPDG, Is0pi);
}

// Helper for STV; get a proton from an event within some cut range (different
// for T2K and MINERvA)
TLorentzVector GetProtonInRange(FitEvent *event, double ProtonMinCut,
                                double ProtonMaxCut, double ProtonCosThetaCut);

// For T2K inferred kinematics analyis - variables defined as on page 7 of T2K
// TN287v11 (and now arXiv 1802.05078)
double ppInfK(TLorentzVector pmu, double costh, double binding, bool neutrino);
TVector3 tppInfK(TLorentzVector pmu, double costh, double binding,
                 bool neutrino);
double cthpInfK(TLorentzVector pmu, double costh, double binding,
                bool neutrino);

double CosThAdler(TLorentzVector Pnu, TLorentzVector Pmu, TLorentzVector Ppi,
                  TLorentzVector Pprot);
double PhiAdler(TLorentzVector Pnu, TLorentzVector Pmu, TLorentzVector Ppi,
                TLorentzVector Pprot);
} // namespace FitUtils

/*! @} */
#endif
