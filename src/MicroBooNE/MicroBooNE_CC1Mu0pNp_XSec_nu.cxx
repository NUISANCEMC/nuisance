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

#include "MicroBooNE_CC1Mu0pNp_XSec_nu.h"
#include "MicroBooNE_SignalDef.h"

#include "TH1D.h"
#include "TH2D.h"
#include "WireCellHelper.h"

//********************************************************************
template <distribution_t D, distribution_t... Ds>
MicroBooNE_CC1Mu0pNp_XSec_nu<D, Ds...>::MicroBooNE_CC1Mu0pNp_XSec_nu(
    nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MicroBooNE_CC1Mu0pNp_XSec_nu sample. \n"
                        "Target: Ar \n"
                        "Flux: BNB FHC numu \n"
                        "Signal: CC 1Mu0pNp WireCell \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Bin Number");
  fSettings.SetYTitle("d#sigma (cm^{2}/^{40}Ar)");

  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV",
                            "FIX/FULL");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("Ar");

  // Plot information
  fSettings.SetTitle("MicroBooNE_CC1Mu0pNp_XSec_nu");
  fSettings.DefineAllowedSpecies("numu");

  std::string sample_name = fSettings.GetName();
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width") / (fNEvents + 0.)) *
                  1E-38 / (TotalIntegratedFlux())) * 40;

  // Setup Histograms
  CC1Mu0pNpHelper<D, Ds...> ana_helper;
  ana_helper.load_measurement();

  // we access bin numbers and widths for each measurement based on input physics values
  fTable = ana_helper.get_lookuptable();
  // we don't use the template arguments and fetch it from here instead
  // to handle the kAll case
  fDists = fTable.get_dists();

  // the data histogram
  fDataHist = (TH1D *)ana_helper.get_data();
  fDataHist->SetName(Form("%s_data", sample_name.c_str()));
  ScaleData(1E-36);

  // the measurement covariance
  fFullCovar = ana_helper.get_cov_m();
  // convert from 10^-72 -> 10^-76 which nuisance expects
  (*fFullCovar) *= 1E4;

  // set the errors to the ones from covariance matrix
  // don't think this is actually needed but atleast suppresses some warnings
  for(int i = 0; i < fDataHist->GetNbinsX(); i++)
    fDataHist->SetBinError(i+1, sqrt((*fFullCovar)(i, i))*1E-38);

  // the additional Wiener-SVD Ac smearing matrix
  fSmearingMatrix = ana_helper.get_ac_m();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

  fSaveFine = false;

}

//********************************************************************
template <distribution_t D, distribution_t... Ds>
bool MicroBooNE_CC1Mu0pNp_XSec_nu<D, Ds...>::isSignal(FitEvent *nvect)
{
  return (SignalDef::isCCINC(nvect, 14, EnuMin, EnuMax)) && (nvect->NumFSMuon() == 1);
}

//********************************************************************
template <distribution_t D, distribution_t... Ds>
void MicroBooNE_CC1Mu0pNp_XSec_nu<D, Ds...>::FillEventVariables(FitEvent *customEvent)
{

  if (!isSignal(customEvent)) { // double the work, but it lets us use the below
                          // functions without error checking
    fXVar = -999;
    return;
  }
  // real physics observables go here
  const double MeV2GeV = 0.001;

  // muon
  FitParticle* Muon = customEvent->GetHMFSParticle(13);
  double EMu = Muon->E()*MeV2GeV;
  double CosThetaMu = Muon->P3()[2]/Muon->p();

  // energy related
  double ENu = customEvent->Enu()*MeV2GeV;
  double TransferEnergy = ENu - EMu;
  double AvailEnergy = 0.;

  // proton
  int NProton = 0;
  double ProtonKE = 0.;
  double ProtonCosTheta = -5.;

  std::vector<int> AllFSIndices = customEvent->GetAllFSParticleIndices(0);
  // start loop
  for(int i = 0; i < AllFSIndices.size(); i++){
    int pdg = customEvent->GetParticlePDG(AllFSIndices.at(i));
    // ignore muon and neutrons
    if(abs(pdg) == 13 || pdg == 2112) continue;

    double KE = customEvent->GetParticle(AllFSIndices.at(i))->KE()*MeV2GeV;
    double E  = customEvent->GetParticle(AllFSIndices.at(i))->E()*MeV2GeV;
    double M  = customEvent->GetParticle(AllFSIndices.at(i))->M()*MeV2GeV;
    double Pz = customEvent->GetParticle(AllFSIndices.at(i))->P3()[2]*MeV2GeV;
    double P  = customEvent->GetParticle(AllFSIndices.at(i))->p()*MeV2GeV;

    // Ben saw some weirdness with GiBUU, so lets replicate his checks
    if(E < M) continue;
    // sum up protons
    if(pdg == 2212){
      // get the leading proton observables
      if(KE > ProtonKE){
        ProtonKE = KE;
        if(KE >= 0.035) ProtonCosTheta = Pz/P;
      }
      // 35 MeV threhold
      if(KE >= 0.035) {
        NProton++;
        AvailEnergy += KE;
      }
    }
    // sum up pions
    else if(abs(pdg) == 211){
      // 10 MeV threshold
      if(KE >= 0.01) AvailEnergy += KE;
    }
    else AvailEnergy += KE;
  }

  int curr_bin = 0;
  // loop over our blocks
  for(auto it = fDists.begin(); it != fDists.end(); ++it){
    distribution_t dist = *it;
    int nblockbins = fTable.get_nbins(dist);
    int localbin = -1;
    switch(dist){
      case k0pNpEMu:
        localbin = fTable.find_bin(dist, EMu, ProtonKE);
        break;
      case k0pNpCosThetaMu:
        localbin = fTable.find_bin(dist, CosThetaMu, ProtonKE);
        break;
      case k0pNpEnu:
        localbin = fTable.find_bin(dist, ENu, ProtonKE);
        break;
      case k0pNpTransferEnergy:
        localbin = fTable.find_bin(dist, TransferEnergy, ProtonKE);
        break;
      case k0pNpAvailEnergy:
        localbin = fTable.find_bin(dist, AvailEnergy, ProtonKE);
        break;
      case kProtonKE:
        localbin = fTable.find_bin(dist, ProtonKE);
        break;
      case kProtonCosTheta:
        localbin = fTable.find_bin(dist, ProtonCosTheta);
        break;
      case kProtonMult:
        localbin = fTable.find_bin(dist, NProton);
        break;
      case k0pNpEMuCosThetaMu:
        localbin = fTable.find_bin(dist, EMu, CosThetaMu, ProtonKE);
        break;
      case kNpProtonKECosTheta:
        localbin = fTable.find_bin(dist, ProtonKE, ProtonCosTheta);
        break;
      case kXpEMu:
        localbin = fTable.find_bin(dist, EMu);
        break;
      case kXpCosThetaMu:
        localbin = fTable.find_bin(dist, CosThetaMu);
        break;
      case kXpEMuCosThetaMu:
        localbin = fTable.find_bin(dist, EMu, CosThetaMu);
        break;
      case kXpAvailEnergyCosThetaMuEMu:
        localbin = fTable.find_bin(dist, EMu, CosThetaMu, AvailEnergy);
        break;
    }
    // don't do anything else here
    // we fill in our own FillHistograms directly
    // since an event can belong to multiple bins
    fXVars[dist] = (localbin < 0) ? -999 : localbin + curr_bin;
    curr_bin += nblockbins;
  }
}

//********************************************************************
template <distribution_t D, distribution_t... Ds>
void MicroBooNE_CC1Mu0pNp_XSec_nu<D, Ds...>::FillHistograms() {
  // loop over our blocks and fill for each one of them
  for(auto it = fDists.begin(); it != fDists.end(); ++it){
    distribution_t dist = *it;
    fXVar = fXVars[dist];
    Measurement1D::FillHistograms();
  }
}

//********************************************************************
template <distribution_t D, distribution_t... Ds>
void MicroBooNE_CC1Mu0pNp_XSec_nu<D, Ds...>::ConvertEventRates() {

  // standard conversion
  Measurement1D::ConvertEventRates();
  int curr_bin = 0;
  // loop over our blocks
  for(auto it = fDists.begin(); it != fDists.end(); ++it){
    distribution_t dist = *it;
    int nblockbins = fTable.get_nbins(dist);
    // loop through the block
    for(int i = 0; i < nblockbins; i++){
      // convert to differential xsec by scaling it wrt the bin widths
      double bin_width = fTable.get_width(dist, i);
      // for xsec vs neutrino energy, we divide by the fraction of flux
      // producing the events in that energy range
      double scaling = 1.;
      if(dist == k0pNpEnu)
        scaling = fTable.apply(dist, i,
                               GetFluxFraction, GetFluxHistogram());

      fMCHist->SetBinContent(curr_bin + i + 1, fMCHist->GetBinContent(curr_bin + i + 1)/(bin_width*scaling));
      fMCHist->SetBinError  (curr_bin + i + 1, fMCHist->GetBinError(curr_bin + i + 1)/(bin_width*scaling));
    }
    curr_bin += nblockbins;
  }

  // now apply Wiener-SVD Ac smearing
  int n = fMCHist->GetNbinsX();
  TVectorD v(n), e(n);
  for (int i = 0; i < n; i++) {
    v(i) = fMCHist->GetBinContent(i + 1);
    e(i) = std::pow(fMCHist->GetBinError(i + 1), 2);
  }

  TVectorD vs = (*fSmearingMatrix) * v;
  TVectorD es = (*fSmearingMatrix) * e;

  for (int i = 0; i < n; i++) {
    fMCHist->SetBinContent(i + 1, vs(i));
    fMCHist->SetBinError(i + 1, std::sqrt(es(i)));
  }
}

template class MicroBooNE_CC1Mu0pNp_XSec_nu<k0pNpEMu>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<k0pNpCosThetaMu>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<k0pNpEnu>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<k0pNpTransferEnergy>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<k0pNpAvailEnergy>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<kProtonKE>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<kProtonCosTheta>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<kProtonMult>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<k0pNpEMuCosThetaMu>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<kNpProtonKECosTheta>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<kXpEMu>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<kXpCosThetaMu>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<kXpEMuCosThetaMu>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<kXpAvailEnergyCosThetaMuEMu>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<kAll>;
template class MicroBooNE_CC1Mu0pNp_XSec_nu<k0pNpEMu, k0pNpCosThetaMu, k0pNpEnu, k0pNpTransferEnergy, k0pNpAvailEnergy, kProtonKE, kProtonCosTheta, kProtonMult>;
