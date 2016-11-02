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

/** 
 * Radecky et al. Phys Rev D, 3rd series, Vol 25, No 5, 1 March 1982, p 1161-1173
*/

#include "ANL_CC1ppip_Evt_1DcosmuStar_nu.h"

// The constructor
ANL_CC1ppip_Evt_1DcosmuStar_nu::ANL_CC1ppip_Evt_1DcosmuStar_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  fName = "ANL_CC1ppip_Evt_1DcosmuStar_nu";
  fPlotTitles = "; cos(#theta*); Number of events";
  EnuMin = 0;
  EnuMax = 6.0;
  fIsDiag = true;
  fIsRawEvents = true;
  fDefaultTypes="EVT/SHAPE/DIAG";
  fAllowedTypes="EVT/SHAPE/DIAG";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/ANL/CC1pip_on_p/ANL_CC1pip_on_p_noEvents_cosmuStar_1982.csv");
  this->SetupDefaultHist();

  // set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = this->fEventHist->Integral("width")/(fNEvents+0.)*16./8.;
};


void ANL_CC1ppip_Evt_1DcosmuStar_nu::FillEventVariables(FitEvent *event) {
  
  if (event->NumISParticle(2212) == 0 ||
      event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pin  = event->GetHMISParticle(2212)->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass   = FitUtils::MpPi(Pp, Ppip);
  double cosmuStar = -999;

  // Now need to boost into center-of-mass frame
  TLorentzVector CMS = Pnu + Pin;
  // Boost the muon backwards
  Pmu.Boost(-CMS.BoostVector());
  // Boost the neutrino forwards
  Pnu.Boost(CMS.BoostVector());
  
  // ANL has a M(pi, p) < 1.4 GeV cut imposed
  // Find angle in CMS frame
  if (hadMass < 1400) cosmuStar = cos(FitUtils::th(Pmu, Pnu));

  fXVar = cosmuStar;

  return;
};

bool ANL_CC1ppip_Evt_1DcosmuStar_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}
