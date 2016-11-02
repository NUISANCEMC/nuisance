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

#include "MiniBooNE_CC1pip_XSec_2DTuEnu_nu.h"

// The constructor
MiniBooNE_CC1pip_XSec_2DTuEnu_nu::MiniBooNE_CC1pip_XSec_2DTuEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
  
  fName = "MiniBooNE_CC1pip_XSec_2DTuEnu_nu";
  fPlotTitles = "; E_{#nu} (MeV); T_{#mu} (MeV); d#sigma(E_{#nu})/dT_{#mu} (cm^{2}/MeV)";
  EnuMin = 0.5;
  EnuMax = 2.0;
  fIsDiag = true;
  fNormError = 0.107;
  Measurement2D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MiniBooNE/CC1pip/ccpipXSecs.root", std::string("MUKEVENUXSec"));//data comes in .root file, yes!
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  // Calculates a flux averaged cross-section from (Evt("width")/Flux("width")) * 14.08/6.0
  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)*(14.08);
};


/*
void MiniBooNE_CC1pip_XSec_2DTuEnu_nu::SetDataValues(std::string fileLocation) {
  LOG(DEB) << "Reading: " << this->fName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  fDataHist = (TH2D*)(dataFile->Get("MUKEVENUXSec")->Clone());

  fDataHist->SetDirectory(0); //should disassociate fDataHist with dataFile

  dataFile->Close();
  delete dataFile;
};
*/

void MiniBooNE_CC1pip_XSec_2DTuEnu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip)*1000.;
  double Tmu = FitUtils::T(Pmu)*1000.;

  fXVar = Enu;
  fYVar = Tmu;

  return;
};

//******************************************************************** 
bool MiniBooNE_CC1pip_XSec_2DTuEnu_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pi(event, 14, 211, EnuMin, EnuMax);
}
