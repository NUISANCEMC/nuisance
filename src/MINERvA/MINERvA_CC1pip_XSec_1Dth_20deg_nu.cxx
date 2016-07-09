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

#include "MINERvA_CC1pip_XSec_1Dth_20deg_nu.h"

// The constructor
MINERvA_CC1pip_XSec_1Dth_20deg_nu::MINERvA_CC1pip_XSec_1Dth_20deg_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  measurementName = "MINERvA_CC1pip_XSec_1Dth_20deg_nu";
  plotTitles = "; #theta_{#pi} (degrees); d#sigma/d#theta_{#pi} (cm^{2}/degrees/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  isDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  if (isShape) {
    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pip/ccpip_theta_20_shape.csv");
    this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pip/ccpip_theta_20_cov_shape.csv", dataHist->GetNbinsX());
  } else {
    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pip/ccpip_theta_20.csv");
    this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CC1pip/ccpip_theta_20_cov.csv", dataHist->GetNbinsX());
  }
  this->SetupDefaultHist();

  // Scaling for MINERvA corrected flux; roughly a normalisation change of 11%
  // PLEASE CHANGE THIS WHEN NEW MINERvA MEASUREMENT IS RELEASED
  for (int i = 0; i < dataHist->GetNbinsX() + 1; i++) {
    dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1.11);
  }

  hadMassHist = new TH1D((measurementName+"_hadMass").c_str(), (measurementName+"_hadMass").c_str(), 100, 1000, 2000);

  scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

void MINERvA_CC1pip_XSec_1Dth_20deg_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  for (int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  hadMass = FitUtils::Wrec(Pnu, Pmu, Ppip);
  hadMassHist->Fill(hadMass);
  double th;

  // MINERvA CC1pi+ imposes a 0.1 < W < 1.4 GeV hadronic mass constraint
  if (hadMass > 100 && hadMass < 1400) {
    th = (180./M_PI)*FitUtils::th(Pnu, Ppip);
  } else {
    th = -999;
  }

  this->X_VAR = th;

  return;
};

//******************************************************************** 
bool MINERvA_CC1pip_XSec_1Dth_20deg_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  // Last true refers to that this is the restricted MINERvA phase space, in which only forward-going muons are accepted
  return SignalDef::isCC1pip_MINERvA(event, EnuMin, EnuMax, true);
}

void MINERvA_CC1pip_XSec_1Dth_20deg_nu::Write(std::string drawOpt) {
  Measurement1D::Write(drawOpt);
  hadMassHist->Write();

  return;
}
