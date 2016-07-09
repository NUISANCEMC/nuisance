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

#include "MiniBooNE_CC1pip_XSec_2DTuEnu_nu.h"

// The constructor
MiniBooNE_CC1pip_XSec_2DTuEnu_nu::MiniBooNE_CC1pip_XSec_2DTuEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
  
  measurementName = "MiniBooNE_CC1pip_XSec_2DTuEnu_nu";
  plotTitles = "; E_{#nu} (MeV); T_{#mu} (MeV); d#sigma(E_{#nu})/dT_{#mu} (cm^{2}/MeV)";
  EnuMin = 0.5;
  EnuMax = 2.0;
  isDiag = true;
  normError = 0.107;
  Measurement2D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MiniBooNE/CC1pip/ccpipXSecs.root", std::string("MUKEVENUXSec"));//data comes in .root file, yes!
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  // Calculates a flux averaged cross-section from (Evt("width")/Flux("width")) * 14.08/6.0
  this->scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)*(14.08);
};


/*
void MiniBooNE_CC1pip_XSec_2DTuEnu_nu::SetDataValues(std::string fileLocation) {
  std::cout << "Reading: " << this->measurementName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  dataHist = (TH2D*)(dataFile->Get("MUKEVENUXSec")->Clone());

  dataHist->SetDirectory(0); //should disassociate dataHist with dataFile

  dataFile->Close();
  delete dataFile;
};
*/

void MiniBooNE_CC1pip_XSec_2DTuEnu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j){
    if (!event->PartInfo(j)->fIsAlive || event->PartInfo(j)->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = event->PartInfo(j)->fP;
    }
  }

  double Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip)*1000.;
  double Tmu = FitUtils::T(Pmu)*1000.;

  this->X_VAR = Enu;
  this->Y_VAR = Tmu;

  return;
};

//******************************************************************** 
bool MiniBooNE_CC1pip_XSec_2DTuEnu_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pip_MiniBooNE(event, EnuMin, EnuMax);
}
