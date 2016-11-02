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
 * D.Day et al, "Study of \nud charged-current two-pion production in the threshold region", Physical Review D, Volume 28, Number 11, 1 December 1983 \n
 * Derrick, Musgrave, Ammar, Day, Kafka and Mann, "Two- and three-pion productin by \nu\mud recations nears threshold: The implication for nucleon-decay experiments", Physical Review D, Vol 30, Number 7, 1 October 1984
*/

#include "ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu.h"

// The constructor
ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu::ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  fName = "ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu";
  fPlotTitles = "; p_{#mu} (GeV); Number of events (area norm.)";
  EnuMin = 0.;
  EnuMax = 6.0;
  fIsDiag = true; // refers to covariance matrix; this measurement has none so only use errors, not covariance
  fIsRawEvents = true;
  fNormError = 0.20; // normalisation error on ANL BNL flux
  fDefaultTypes="EVT/SHAPE/DIAG";
  fAllowedTypes="EVT/SHAPE/DIAG";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // there's also _unweight rather than weight data file
  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/ANL/CC2pi/1pip1pi0/CC2pi_1pip1pi0_pMu_weight.csv");
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  for (int i = 0; i < fDataHist->GetNbinsX()+1; ++i) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)*(16./8.);
};



void ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(-211) == 0)
    return;

  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;
  double pmu = FitUtils::p(Pmu);

  this->fXVar = pmu;

  return;
}

// Signal asks for 1pi0, 1pi+, 1mu-, 1p
bool ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu::isSignal(FitEvent *event) {
  int pdgs[] = {13, 211, 111, 2212};
  return SignalDef::isCCWithFS(event, 14, GeneralUtils::makeVector(pdgs),
                               EnuMin, EnuMax);
}


/*
void ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu::ScaleEvents() {
  
  PlotUtils::FluxUnfoldedScaling(mcHist, fFluxHist);
  PlotUtils::FluxUnfoldedScaling(mcFine, fFluxHist);

  mcHist->Scale(fScaleFactor);
  mcFine->Scale(fScaleFactor);

  return;
}
*/
