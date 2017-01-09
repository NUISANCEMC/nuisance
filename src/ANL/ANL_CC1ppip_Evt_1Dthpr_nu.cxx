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
 * Derrick et al. Phys Rev D, Vol 23, Number 3, 1 Feb 1981, p 569-575
*/
#include "ANL_CC1ppip_Evt_1Dthpr_nu.h"

// The constructor
ANL_CC1ppip_Evt_1Dthpr_nu::ANL_CC1ppip_Evt_1Dthpr_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {

  fName = "ANL_CC1ppip_Evt_1Dthpr_nu";
  fPlotTitles = "; cos #theta_{p}; Number of events";
  EnuMin = 0;
  EnuMax = 1.5; // Different EnuMax for cos(thpr), see Derrick et al, Phys Rev D V23 N3, p572 fig 3
  fIsDiag = true;
  fIsRawEvents = true;
  fDefaultTypes="EVT/SHAPE/DIAG";
  fAllowedTypes="EVT/SHAPE/DIAG";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // ANL ppi has Enu < 1.5 GeV, W < 1.4 GeV
  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/ANL/CC1pip_on_p/ANL_CC1pip_on_p_noEvents_thProt.csv");
  this->SetupDefaultHist();

  // set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = GetEventHistogram()->Integral("width")/((fNEvents+0.))*(16./8.);
};

void ANL_CC1ppip_Evt_1Dthpr_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double costhpr = -999;

  // This measurement has M(Npi) = W < 1.4GeV
  if (hadMass < 1400) costhpr = cos(FitUtils::th(Pnu,Pp));

  fXVar = costhpr;

  return;
};


bool ANL_CC1ppip_Evt_1Dthpr_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}

