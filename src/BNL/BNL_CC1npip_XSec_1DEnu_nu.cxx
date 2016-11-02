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

#include "BNL_CC1npip_XSec_1DEnu_nu.h"

// The constructor
BNL_CC1npip_XSec_1DEnu_nu::BNL_CC1npip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "BNL_CC1npip_XSec_1DEnu_nu";
  fPlotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/neutron)";
  EnuMin = 0.;
  EnuMax = 3.0;
  fIsDiag = true;
  fNormError = 0.15;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/BNL/CC1pip_on_n/BNL_CC1pip_on_n_1986.txt");
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = (this->fEventHist->Integral("width")*1E-38)/((fNEvents+0.))*16./8.;
};


void BNL_CC1npip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pn   = event->GetHMFSParticle(2112)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  //double hadMass = FitUtils::MpPi(Pn, Ppip);
  // No W cut for BNL CC1pi+ on neutron (I'm happy if you can find it!!!)
  double Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip);

  fXVar = Enu;

  return;
};


bool BNL_CC1npip_XSec_1DEnu_nu::isSignal(FitEvent *event) {

  // BNL has somewhat tricky signal definition for the selection:
  // P_visible = visible 4-momentum, so the three tracks total momentum
  // P_visible > 150 MeV/c2
  // Theta_Pvis_nu (angle between visible momentum and neutrino direction) < 50 degrees
  // At least one negative track leaves chamber without interacting or stops consistent with muon
  // THESE ARE NOT IMPLEMENTED BUT SHOULD BE KNOWN BY ANYONE WHO FITS THIS DATA! (see Kitagaki et al. 2556)
  //
  // The only actual restriction seems to be the final state particles. There's no mention of E_nu restrictions either (events span to 10GeV in fig 2, Kitagaki)
  // There's a mention of uncertainity in the neutrino flux for Enu > 6.0 GeV
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2112, EnuMin, EnuMax);
}


/*
void BNL_CC1npip_XSec_1DEnu_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void BNL_CC1npip_XSec_1DEnu_nu::ScaleEvents() {

  PlotUtils::FluxUnfoldedScaling(fMCHist, fFluxHist);
  PlotUtils::FluxUnfoldedScaling(fMCFine, fFluxHist);

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fScaleFactor);

  return;
}
*/
