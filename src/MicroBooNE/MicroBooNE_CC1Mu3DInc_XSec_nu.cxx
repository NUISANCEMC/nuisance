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

#include "MicroBooNE_CC1Mu3DInc_XSec_nu.h"
#include "MicroBooNE_SignalDef.h"

#include "TH1D.h"
#include "TH2D.h"

//********************************************************************
MicroBooNE_CC1Mu3DInc_XSec_nu::MicroBooNE_CC1Mu3DInc_XSec_nu(
  nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MicroBooNE_CC1Mu3DInc_XSec_nu sample. \n"
                        "Target: Ar \n"
                        "Flux: BNB FHC numu \n"
                        "Signal: CC 1Mu3DInc WireCell \n";

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
  fSettings.SetTitle("MicroBooNE_CC1Mu3DInc_XSec_nu");
  fSettings.DefineAllowedSpecies("numu");

  std::string sample_name = fSettings.GetName();
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width") / (fNEvents + 0.)) *
                  1E-38 / (TotalIntegratedFlux())) * 40;

  // Setup Histograms
  CCInc3DHelper ana_helper;
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
bool MicroBooNE_CC1Mu3DInc_XSec_nu::isSignal(FitEvent *nvect)
{
  return SignalDef::isCCINC(nvect, 14, EnuMin, EnuMax);
}

//********************************************************************
void MicroBooNE_CC1Mu3DInc_XSec_nu::FillEventVariables(FitEvent *customEvent)
{

  if (!isSignal(customEvent)) { // double the work, but it lets us use the below
                          // functions without error checking
    fXVar = -999;
    return;
  }
  // real physics observables go here
  const double MeV2GeV = 0.001;

  FitParticle* Muon = customEvent->GetHMFSParticle(13);
  double PMu = Muon->p()*MeV2GeV;
  double CosThetaMu = Muon->P3()[2]/Muon->p();

  double ENu = customEvent->Enu()*MeV2GeV;
  fXVar = fTable.find_bin(kEnuCosThetaMuEMu, ENu, CosThetaMu, PMu);
}

//********************************************************************
void MicroBooNE_CC1Mu3DInc_XSec_nu::ConvertEventRates() {

  int n = fMCHist->GetNbinsX();

  // standard conversion
  Measurement1D::ConvertEventRates();

  // convert to differential xsec by scaling it wrt the bin widths
  for(int i = 0; i < n; i++){
    double bin_width = fTable.get_width(kEnuCosThetaMuEMu, i);
    // for xsec vs neutrino energy, we divide by the fraction of flux
    // producing the events in that energy range
    double scaling = fTable.apply(kEnuCosThetaMuEMu, i,
                                  GetFluxFraction, GetFluxHistogram());

    fMCHist->SetBinContent(i + 1, fMCHist->GetBinContent(i + 1)/(bin_width*scaling));
    fMCHist->SetBinError  (i + 1, fMCHist->GetBinError(i + 1)/(bin_width*scaling));
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
