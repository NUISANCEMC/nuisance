// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "MINERvA_CC1pi0_XSec_1Dppi0_antinu.h"

// The constructor
MINERvA_CC1pi0_XSec_1Dppi0_antinu::MINERvA_CC1pi0_XSec_1Dppi0_antinu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  measurementName = "MINERvA_CC1pi0_XSec_1Dppi0_nubar";
  plotTitles = "; p_{#pi^{0}} (GeV/c); d#sigma/dp_{#pi^{0}} (cm^{2}/(GeV/c)/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  isDiag = true;
  normError = 0.15;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pi0/ccpi0_ppi0.csv");
// NO COVARIANCE FOR THIS MEASUREMENT!
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  // Scale up data to temporarily account for flux discrepancy at MINERvA
  // Please update this when new data becomes available
  for (int i = 0; i < dataHist->GetNbinsX() + 1; i++) {
    dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1.11);
  }

  this->scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

void MINERvA_CC1pi0_XSec_1Dppi0_antinu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;
  TLorentzVector Ppi0;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
    } else if (PID == -13) {
      Pmu = event->PartInfo(j)->fP;
    }
  }

  double ppi0 = FitUtils::p(Ppi0);

  this->X_VAR = ppi0;

  return;
};

//******************************************************************** 
bool MINERvA_CC1pi0_XSec_1Dppi0_antinu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pi0Bar_MINERvA(event, EnuMin, EnuMax);
}
