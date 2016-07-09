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

#include "MINERvA_CCNpip_XSec_1Dth_20deg_nu.h"

// The constructor
MINERvA_CCNpip_XSec_1Dth_20deg_nu::MINERvA_CCNpip_XSec_1Dth_20deg_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  measurementName = "MINERvA_CCNpip_XSec_1Dth_20deg_nu";
  plotTitles = "; #theta_{#pi} (degrees); d#sigma/d#theta_{#pi} (cm^{2}/degrees/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  isDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  if (isShape) {
    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th_20deg_shape.txt");
    this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th_20deg_shape_cov.txt", dataHist->GetNbinsX());
  } else {
    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th_20deg.txt");
    this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_th_20deg_cov.txt", dataHist->GetNbinsX());
  }

  this->SetupDefaultHist();

  // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
  // Please change when MINERvA releases new data!
  for (int i = 0; i < dataHist->GetNbinsX() + 1; i++) {
    dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1.11);
  }

  scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

void MINERvA_CCNpip_XSec_1Dth_20deg_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      if (event->PartInfo(j)->fP.Vect().Mag() > Ppip.Vect().Mag()) {
        Ppip = event->PartInfo(j)->fP;
      }
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  double hadMass = FitUtils::Wrec(Pnu, Pmu, Ppip);
  double th;

  if (hadMass > 100 && hadMass < 1800) {
    th = (180./M_PI)*FitUtils::th(Pnu, Ppip);
  } else {
    th = -999;
  }

  this->X_VAR = th;

  return;
};

//******************************************************************** 
bool MINERvA_CCNpip_XSec_1Dth_20deg_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  // Last false refers to that this is NOT the restricted MINERvA phase space, in which only forward-going muons are accepted
  return SignalDef::isCCNpip_MINERvA(event, nPions, EnuMin, EnuMax, true);
}
