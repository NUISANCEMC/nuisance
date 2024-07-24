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

#include "MicroBooNE_NCpi0_XSec_nu.h"
#include "MicroBooNE_SignalDef.h"

#include "TH1D.h"
#include "TH2D.h"
#include "WireCellHelper.h"

//********************************************************************
template <distribution_t D, distribution_t... Ds>
MicroBooNE_NCpi0_XSec_nu<D, Ds...>::MicroBooNE_NCpi0_XSec_nu(
    nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MicroBooNE_NCpi0_XSec_nu sample. \n"
                        "Target: Ar \n"
                        "Flux: BNB FHC numu \n"
                        "Signal: NCpi0 WireCell \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Bin Number");
  fSettings.SetYTitle("d#sigma (cm^{2}/^{40}/Ar)");

  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV",
                            "FIX/FULL");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("Ar");

  // Plot information
  fSettings.SetTitle("MicroBooNE_NCpi0_XSec_nu");
  //hack for now, need all species
  fSettings.DefineAllowedSpecies("numu");

  std::string sample_name = fSettings.GetName();
  FinaliseSampleSettings();

  //NEED TO CHANGE
  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width") / (fNEvents + 0.)) *
                  1E-38 / (TotalIntegratedFlux())) * 40;
  //NEED TO CHANGE

  // Setup Histograms
  NCpi0Helper<D, Ds...> ana_helper;
  ana_helper.load_measurement();

  // we access bin numbers and widths for each measurement based on input physics values
  fTable = ana_helper.get_lookuptable();
  // we don't use the template arguments and fetch it from here instead
  // to handle the kAll case
  fDists = fTable.get_dists();

  // the data histogram
  fDataHist = (TH1D *)ana_helper.get_data();
  fDataHist->SetName(Form("%s_data", sample_name.c_str()));
  ScaleData(1E-39*40);

  // the measurement covariance
  fFullCovar = ana_helper.get_cov_m();
  // convert from 10^-78/nucleon/nuceon -> 10^-76/A/A which nuisance expects
  (*fFullCovar) *= 1E-2*40*40;

  // set the errors to the ones from covariance matrix
  // don't think this is actually needed but atleast suppresses some warnings
  for(int i = 0; i < fDataHist->GetNbinsX(); i++)
    fDataHist->SetBinError(i+1, sqrt((*fFullCovar)(i, i))*1E-38);

  // the additional Wiener-SVD Ac smearing matrix
  fSmearingMatrix = ana_helper.get_ac_m();

  fSaveFine = false;
  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

}


//********************************************************************
template <distribution_t D, distribution_t... Ds>
bool MicroBooNE_NCpi0_XSec_nu<D, Ds...>::isSignal(FitEvent *nvect)
{
  return SignalDef::MicroBooNE::isNCpi0(nvect);
}

//********************************************************************
template <distribution_t D, distribution_t... Ds>
void MicroBooNE_NCpi0_XSec_nu<D, Ds...>::FillEventVariables(FitEvent *customEvent)
{

  if (!isSignal(customEvent)) { // double the work, but it lets us use the below
                          // functions without error checking
    fXVar = -999;
    return;
  }
  // real physics observables go here
  const double MeV2GeV = 0.001;

  // pi0
  FitParticle* pi0 = customEvent->GetHMFSParticle(111);
  double Ppi0 = pi0->p()*MeV2GeV;
  double CosThetaPi0 = pi0->P3()[2]/pi0->p();

  //proton
  double Kp = 0;
  if(customEvent->HasFSParticle(2212)){
    FitParticle* proton = customEvent->GetHMFSParticle(2212);
    Kp = proton->KE()*MeV2GeV;
  }

  int curr_bin = 0;
  // loop over our blocks
  for(auto it = fDists.begin(); it != fDists.end(); ++it){
    distribution_t dist = *it;
    int nblockbins = fTable.get_nbins(dist);
    int localbin = -1;
    switch(dist){
      case k0pNpPpi0:
        localbin = fTable.find_bin(dist, Ppi0, Kp);
        break;
      case kXpPpi0:
        localbin = fTable.find_bin(dist, Ppi0);
        break;
      case k0pNpCosThetaPi0:
        localbin = fTable.find_bin(dist, CosThetaPi0, Kp);
        break;
      case kXpCosThetaPi0:
        localbin = fTable.find_bin(dist, CosThetaPi0);
        break;
      case kXpPpi0CosThetaPi0:
        localbin = fTable.find_bin(dist, Ppi0, CosThetaPi0);
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
void MicroBooNE_NCpi0_XSec_nu<D, Ds...>::FillHistograms() {
  // loop over our blocks and fill for each one of them
  for(auto it = fDists.begin(); it != fDists.end(); ++it){
    distribution_t dist = *it;
    fXVar = fXVars[dist];
    Measurement1D::FillHistograms();
  }
}

//********************************************************************
template <distribution_t D, distribution_t... Ds>
void MicroBooNE_NCpi0_XSec_nu<D, Ds...>::ConvertEventRates() {

  // standard conversion
  Measurement1D::ConvertEventRates();
  int curr_bin = 0;
  // loop over our blocks
  for(auto it = fDists.begin(); it != fDists.end(); ++it){
    distribution_t dist = *it;
    int nblockbins = fTable.get_nbins(dist);
    // loop through the block
    double scale = 0.975; // Approximation for now. Produces nearly identical cross section to the 4 flavor case, but using on the numus.
    for(int i = 0; i < nblockbins; i++){
      // convert to differential xsec by scaling it wrt the bin widths
      double bin_width = fTable.get_width(dist, i);
      fMCHist->SetBinContent(curr_bin + i + 1, fMCHist->GetBinContent(curr_bin + i + 1)/bin_width*scale);
      fMCHist->SetBinError  (curr_bin + i + 1, fMCHist->GetBinError(curr_bin + i + 1)/bin_width*scale);
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

template class MicroBooNE_NCpi0_XSec_nu<k0pNpPpi0>;
template class MicroBooNE_NCpi0_XSec_nu<kXpPpi0>;
template class MicroBooNE_NCpi0_XSec_nu<k0pNpCosThetaPi0>;
template class MicroBooNE_NCpi0_XSec_nu<kXpCosThetaPi0>;
template class MicroBooNE_NCpi0_XSec_nu<kXpPpi0CosThetaPi0>;
template class MicroBooNE_NCpi0_XSec_nu<kAllNCpi0>;
