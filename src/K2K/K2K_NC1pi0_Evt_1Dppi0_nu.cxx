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

#include "K2K_NC1pi0_Evt_1Dppi0_nu.h"

// The constructor
K2K_NC1pi0_Evt_1Dppi0_nu::K2K_NC1pi0_Evt_1Dppi0_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  measurementName = "K2K_NC1pi0_Evt_1Dppi0_nu";
  plotTitles = "; p_{#pi^{0}} (MeV/c); Number of events";
  EnuMin = 0.;
  EnuMax = 5.;
  isDiag = true;
  normError = 0.15;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/K2K/ncpi0/ppi0.csv");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  this->scaleFactor = 1; // No need to care about scaling factors for shape measurements
};

void K2K_NC1pi0_Evt_1Dppi0_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Ppi0;

  for (int j = 2; j < event->Npart(); ++j){
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
    }
  }

  double ppi0 = FitUtils::p(Ppi0)*1000.;

  this->X_VAR = ppi0;

  return;
};

bool K2K_NC1pi0_Evt_1Dppi0_nu::isSignal(FitEvent *event) {
  return SignalDef::isNC1pi0_MiniBooNE(event, EnuMin, EnuMax); // Check this signal definition
  // Fairly sure K2K and MiniBooNE had same definition...
}
