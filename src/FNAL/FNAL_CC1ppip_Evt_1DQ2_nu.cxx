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

#include "FNAL_CC1ppip_Evt_1DQ2_nu.h"

// The constructor
FNAL_CC1ppip_Evt_1DQ2_nu::FNAL_CC1ppip_Evt_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  fName = "FNAL_CC1ppip_Evt_1DQ2_nu";
  fPlotTitles = "; Q^{2}_{CC1#pi} (GeV^{2}); Number of events";
  EnuMin = 10;
  EnuMax = 100;
  fIsDiag = true;
  fIsRawEvents = true;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/FNAL/CC1pip_on_p/FNAL_cc1ppip_nEvent_Q2_w14_bin_edit.txt");
  this->SetupDefaultHist();

  // set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }


  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = this->fEventHist->Integral("width")/(fNEvents+0.)*16./8.;
};


void FNAL_CC1ppip_Evt_1DQ2_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Pp;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (UInt_t j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 2212) {
      Pp = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double q2CCpip;
  
  // FNAL has a M(pi, p) < 1.4 GeV cut imposed only on this channel
  if (hadMass < 1400) {
    q2CCpip = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip);
  } else {
    q2CCpip = -1.0;
  }

  fXVar = q2CCpip;

  return;
};

bool FNAL_CC1ppip_Evt_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}
