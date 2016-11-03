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

#include "MINERvA_SignalDef.h"

#include "MINERvA_CC1pi0_XSec_1Dppi0_antinu.h"

// The constructor
MINERvA_CC1pi0_XSec_1Dppi0_antinu::MINERvA_CC1pi0_XSec_1Dppi0_antinu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

    fName = "MINERvA_CC1pi0_XSec_1Dppi0_nubar";
   fPlotTitles = "; p_{#pi^{0}} (GeV/c); d#sigma/dp_{#pi^{0}} (cm^{2}/(GeV/c)/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
     fIsDiag = true;
    fNormError = 0.15;
    // No hadronic mass cut on old publication
    hadMassCut = 99999;
  fAllowedTypes += "NEW";

  // THIS DATASET IS THE DODGY ONE 

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

    this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2015/ccpi0_ppi0.csv");

    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    // Please change when MINERvA releases new data!
    for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1.11);
    }

    fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
    covar     = StatUtils::GetInvert(fFullCovar);

  this->SetupDefaultHist();

  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};

void MINERvA_CC1pi0_XSec_1Dppi0_antinu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(111) == 0 ||
      event->NumFSParticle(-13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(-13)->fP;

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double ppi0 = -999;

  if (hadMass > 100 && hadMass < hadMassCut) {
    // 2016 does pion kinetic energy in GeV
    // 2015 does pion momentum in GeV
    ppi0 = FitUtils::p(Ppi0);
  }

  fXVar = ppi0;

  return;
};

//********************************************************************
bool MINERvA_CC1pi0_XSec_1Dppi0_antinu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pi(event, -14, 111, EnuMin, EnuMax);
}
