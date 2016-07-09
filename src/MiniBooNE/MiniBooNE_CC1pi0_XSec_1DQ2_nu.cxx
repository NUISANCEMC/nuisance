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

#include "MiniBooNE_CC1pi0_XSec_1DQ2_nu.h"

// The constructor
MiniBooNE_CC1pi0_XSec_1DQ2_nu::MiniBooNE_CC1pi0_XSec_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
  
  measurementName = "MiniBooNE_CC1pi0_XSec_1DQ2_nu";
  plotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); d#sigma/dQ_{CC#pi}^{2} (cm^{2}/GeV^{2})";
  EnuMin = 0.5;
  EnuMax = 2.;
  isDiag = true;
  normError = 0.107;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MiniBooNE/CC1pi0/dxsecdq2_edit.txt");
  //this->SetCovarMatrix(std::string(std::getenv("NIWG_DATA"))+"/MiniBooNE/cc1pi0/dxsecdq2_covar.txt", this->data_points-1);
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  // Calculates a flux averaged cross-section from (Evt("width")/Flux("width")) * 14.08/6.0
  this->scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)*(14.08)/TotalIntegratedFlux("width");
};

void MiniBooNE_CC1pi0_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Ppi0;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j){
    if (!event->PartInfo(j)->fIsAlive || event->PartInfo(j)->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = event->PartInfo(j)->fP;
    }
  }
  
  double q2CCpi0 = FitUtils::Q2CC1pi0rec(Pnu, Pmu, Ppi0);

  this->X_VAR = q2CCpi0;

  return;
};

//******************************************************************** 
bool MiniBooNE_CC1pi0_XSec_1DQ2_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pi0_MiniBooNE(event, EnuMin, EnuMax);
}
