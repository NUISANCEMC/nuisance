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

//********************************************************************
template <distribution_t D>
MicroBooNE_CC1Mu0pNp_XSec_nu<D>::MicroBooNE_CC1Mu0pNp_XSec_nu(
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
  CC1Mu0pNpHelper<D> ana_helper;
  ana_helper.load_measurement();

  // we access bin numbers and widths for each measurement based on input physics values
  fTable = ana_helper.get_lookuptable();

  // the data histogram
  fDataHist = (TH1D *)ana_helper.get_data();
  fDataHist->SetName(Form("%s_data", sample_name.c_str()));
  ScaleData(1E-36);

  // the measurement covariance
  fFullCovar = ana_helper.get_cov_m();
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
template <distribution_t D>
bool MicroBooNE_CC1Mu0pNp_XSec_nu<D>::isSignal(FitEvent *nvect)
{
  return SignalDef::isCCINC(nvect, 14, EnuMin, EnuMax);
}

//********************************************************************
template <distribution_t D>
void MicroBooNE_CC1Mu0pNp_XSec_nu<D>::FillEventVariables(FitEvent *customEvent)
{

  if (!isSignal(customEvent)) { // double the work, but it lets us use the below
                          // functions without error checking
    fXVar = -999;
    return;
  }
  // real physics observables go here
  const double MeV2GeV = 0.001;

  FitParticle* Muon = customEvent->GetHMFSParticle(13);
  double EMu = Muon->E()*MeV2GeV;
  double CosThetaMu = Muon->P3()[2]/Muon->p();

  double ENu = customEvent->Enu()*MeV2GeV;
  double TransferEnergy = Enu - EMu;

  int NProton = 0;
  double AvailEnergy = 0.;
  std::vector<int> AllFSIndices = customEvent->GetAllFSParticleIndices();
  // start loop
  for(int i = 0; i < AllFSIndices.size(); i++){
    int pdg = customEvent->GetParticlePDG(AllFSIndices.at(i));
    if(abs(pdg) == 13 || pdg == 2112) continue;

    double KE = customEvent->GetParticle(AllFSIndices.at(i))->KE()*MeV2GeV;
    // sum up protons
    if(pdg == 2212){
      if(KE >= 0.035) {
        NProton++;
        AvailEnergy += KE;
      }
    }
    // sum up pions
    else if(abs(pdg) == 211 || pdg == 111){
      if(KE >= 0.01) {
        AvailEnergy += KE;
      }
    }
    else AvailEnergy += KE;
  }

  // leading proton observables
  int NFSProtons = customEvent->NumFSParticle(2212);
  double ProtonKE = 0.;
  double ProtonCosTheta = -5.;
  if(NFSProtons > 0) {
    FitParticle* LeadingProton = customEvent->GetHMFSParticle(2212);
    ProtonKE = LeadingProton->KE()*MeV2GeV;
    if(NProton > 0)
      ProtonCosTheta = LeadingProton->P3()[2]/LeadingProton->p();
  }

  switch(D){
    case k0pNpEMu:
      fXVar = fTable.find_bin<D>(EMu, ProtonKE);
      break;
    case k0pNpCosThetaMu:
      fXVar = fTable.find_bin<D>(CosThetaMu, ProtonKE);
      break;
    case k0pNpEnu:
      fXVar = fTable.find_bin<D>(ENu, ProtonKE);
      break;
    case k0pNpTransferEnergy:
      fXVar = fTable.find_bin<D>(TransferEnergy, ProtonKE);
      break;
    case k0pNpAvailEnergy:
      fXVar = fTable.find_bin<D>(AvailEnergy, ProtonKE);
      break;
    case kProtonKE:
      fXVar = fTable.find_bin<D>(ProtonKE);
      break;
    case kProtonCosTheta:
      fXVar = fTable.find_bin<D>(ProtonCosTheta);
      break;
    case kProtonMult:
      fXVar = fTable.find_bin<D>(NProton);
      break;
    case k0pNpEMuCosThetaMu:
      fXVar = fTable.find_bin<D>(EMu, CosThetaMu, ProtonKE);
      break;
    case kNpProtonKECosTheta:
      fXVar = fTable.find_bin<D>(ProtonKE, ProtonCosTheta);
      break;
    case kXpEMu:
      fXVar = fTable.find_bin<D>(EMu);
      break;
    case kXpCosThetaMu:
      fXVar = fTable.find_bin<D>(CosThetaMu);
      break;
    case kXpEMuCosThetaMu:
      fXVar = fTable.find_bin<D>(EMu, CosThetaMu);
      break;
    case kXpAvailEnergyCosThetaMuEMu:
      fXVar = fTable.find_bin<D>(EMu, CosThetaMu, AvailEnergy);
      break;
  }
}

//********************************************************************
template <distribution_t D>
void MicroBooNE_CC1Mu0pNp_XSec_nu<D>::ConvertEventRates() {

  int n = fMCHist->GetNbinsX();

  // standard conversion
  Measurement1D::ConvertEventRates();

  // convert to differential xsec by scaling it wrt the bin widths
  for(int i = 0; i < n; i++){
    double bin_width = fTable.get_width(D, i);
    fMCHist->SetBinContent(i + 1, fMCHist->GetBinContent(i + 1)/bin_width);
    fMCHist->SetBinError(i + 1, fMCHist->GetBinError(i + 1)/bin_width);
  }

  // now apply Wiener-SVD Ac smearing
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
