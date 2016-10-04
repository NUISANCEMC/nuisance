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

  measurementName = "ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu";
  plotTitles = "; p_{#mu} (GeV); Number of events (area norm.)";
  EnuMin = 0.;
  EnuMax = 6.0;
  isDiag = true; // refers to covariance matrix; this measurement has none so only use errors, not covariance
  isRawEvents = true;
  normError = 0.20; // normalisation error on ANL BNL flux
  fDefaultTypes="EVT/SHAPE/DIAG";
  fAllowedTypes="EVT/SHAPE/DIAG";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // there's also _unweight rather than weight data file
  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/ANL/CC2pi/1pip1pi0/CC2pi_1pip1pi0_pMu_weight.csv");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  for (int i = 0; i < dataHist->GetNbinsX()+1; ++i) {
    dataHist->SetBinError(i+1, sqrt(dataHist->GetBinContent(i+1)));
  }

  this->scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)*(16./8.);
};



void ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pmu;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (UInt_t j =  2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0 && (event->PartInfo(j)->fStatus != 2)) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 13) {
      Pmu = event->PartInfo(j)->fP;
    }
  }

  double pmu = FitUtils::p(Pmu);

  this->X_VAR = pmu;

  return;
}

// Signal asks for 1pi-, 1pi+, 1mu-, 1p
bool ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu::isSignal(FitEvent *event) {

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false; 

  int pipCnt = 0;
  int pi0Cnt = 0;
  int lepCnt = 0;
  int protonCnt = 0;

  for (UInt_t j =  2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 13) {
      lepCnt++;
    } else if (PID == 211) {
      pipCnt++;
    } else if (PID == 111) {
      pi0Cnt++;
    } else if (PID == 2212) {
      protonCnt++;
    } else {
      return false;
    }
  }

  if (pipCnt != 1) return false;
  if (pi0Cnt != 1) return false;
  if (lepCnt != 1) return false;
  if (protonCnt != 1) return false;

  return true;
}


/*
void ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu::ScaleEvents() {
  
  PlotUtils::FluxUnfoldedScaling(mcHist, fluxHist);
  PlotUtils::FluxUnfoldedScaling(mcFine, fluxHist);

  mcHist->Scale(scaleFactor);
  mcFine->Scale(scaleFactor);

  return;
}
*/
