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

#include "ANL_CC1pi0_Evt_1DQ2_nu.h"

// The constructor
ANL_CC1pi0_Evt_1DQ2_nu::ANL_CC1pi0_Evt_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  fName = "ANL_CC1pi0_Evt_1DQ2_nu";
  fPlotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); Number of events";
  EnuMin = 0;
  EnuMax = 1.5;
  fIsDiag = true; // refers to covariance matrix; this measurement has none so only use errors, not covariance
  fIsRawEvents = true;
  fDefaultTypes="EVT/SHAPE/DIAG";
  fAllowedTypes="EVT/SHAPE/DIAG";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/ANL/CC1pi0_on_n/ANL_CC1pi0_on_n_noEvents_Q2_14GeV_bin_firstQ2gone.txt");
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


void ANL_CC1pi0_Evt_1DQ2_nu::FillEventVariables(FitEvent *event) {

  // set up the 4-vectors from NEUT
  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Pp;
  TLorentzVector Ppi0;
  TLorentzVector Pmu;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (UInt_t j =  2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
    } else if (PID == 2212) {
      Pp = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }

  double hadMass = FitUtils::MpPi(Pp, Ppi0);
  double q2CCpi0;
  
  // ANL has a M(pi, p) < 1.4 GeV cut imposed
  if (hadMass < 1400) {
    q2CCpi0 = FitUtils::Q2CC1pi0rec(Pnu, Pmu, Ppi0);
  } else {
    q2CCpi0 = -1.0;
  }

  fXVar = q2CCpi0;

  return;
};

bool ANL_CC1pi0_Evt_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 111, 2212, EnuMin, EnuMax);
}

/*
void ANL_CC1pi0_Evt_1DQ2_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void ANL_CC1pi0_Evt_1DQ2_nu::ScaleEvents() {
  
 // PlotUtils::FluxUnfoldedScaling(fMCHist, fFluxHist);
  //PlotUtils::FluxUnfoldedScaling(fMCFine, fFluxHist);

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fScaleFactor);

  return;
}
*/
