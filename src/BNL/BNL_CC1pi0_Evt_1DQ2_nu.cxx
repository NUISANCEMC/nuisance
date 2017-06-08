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

#include "BNL_CC1pi0_Evt_1DQ2_nu.h"

// The constructor
BNL_CC1pi0_Evt_1DQ2_nu::BNL_CC1pi0_Evt_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {

  fName = "BNL_CC1pi0_Evt_1DQ2_nu";
  fPlotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); Number of events";
  EnuMin = 0;
  EnuMax = 6.0;
  fIsDiag = true;
  fIsRawEvents = true;
  fAllowedTypes += "EVT";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  //SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/BNL/CC1pi0_on_n/BNL_CC1pi0_on_n_noEvents_q2_noWcut_firstQ2rem.txt");
  SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/BNL/CC1pi0_on_n/BNL_CC1pi0_on_n_noEvents_q2_noWcut_HighQ2Gone.txt");
  SetupDefaultHist();

  // set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  fScaleFactor = GetEventHistogram()->Integral("width")/(fNEvents+0.);
};


void BNL_CC1pi0_Evt_1DQ2_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(13) == 0) return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  // No W cut on BNL CC1pi0
  // Want true Q2
  double q2CCpi0 = -1*(Pnu-Pmu).Mag2()/1.E6;
  fXVar = q2CCpi0;

  return;
};



bool BNL_CC1pi0_Evt_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 111, 2212, EnuMin, EnuMax);
}

