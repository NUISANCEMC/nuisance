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

#include "GGM_CC1ppip_Evt_1DQ2_nu.h"

// The constructor
GGM_CC1ppip_Evt_1DQ2_nu::GGM_CC1ppip_Evt_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  fName = "GGM_CC1ppip_Evt_1DQ2_nu";
  // could replace with fName = std::string(__FILE__).substr(0, std::string(__FILE__).find_last_of("."))
  fPlotTitles = "; Q^{2}_{CC1#pi} (GeV^{2}); Number of events";
  EnuMin = 1;
  EnuMax = 10;
  fIsDiag = true;
  fIsRawEvents = true;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/GGM/CC1pip_on_p/GGM_CC1ppip_Q2_events_bin_edit.txt");
  this->SetupDefaultHist();

  // set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents+0.)*(16./8.);
};


void GGM_CC1ppip_Evt_1DQ2_nu::FillEventVariables(FitEvent *event) {
  
  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double q2CCpip = -1.0;

  // GGM has a M(pi, p) < 1.4 GeV cut imposed only on this channel
  if (hadMass < 1400) q2CCpip = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip);
  fXVar = q2CCpip;

  return;
}

bool GGM_CC1ppip_Evt_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}
