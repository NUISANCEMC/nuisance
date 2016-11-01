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

#include "MINERvA_CC1pi0_XSec_1DEnu_antinu.h"

// The constructor
MINERvA_CC1pi0_XSec_1DEnu_antinu::MINERvA_CC1pi0_XSec_1DEnu_antinu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile) {

  fName = "MINERvA_CC1pi0_XSec_1DEnu_nubar_2016";
  fPlotTitles = "; E_{#nu} (GeV); d#sigma(E_{#nu}) (cm^{2}/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  fIsDiag = false;
  fAllowedTypes += "NEW";

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/anu-cc1pi0-xsec-enu.csv");

  // Error is given as percentage of cross-section
  // Need to scale the bin error properly before we do correlation -> covariance conversion
  for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
    fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*fDataHist->GetBinError(i+1)/100.);
  }

  // This is a correlation matrix, changed to covariance in SetCovarMatrixFromText
  //this->SetCovarMatrixFromText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/cc1pi0_enu_corr.txt", fDataHist->GetNbinsX());
  this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/anu-cc1pi0-correlation-enu.csv", fDataHist->GetNbinsX());
  this->SetupDefaultHist();

  fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents);
};

void MINERvA_CC1pi0_XSec_1DEnu_antinu::FillEventVariables(FitEvent *event) {

    TLorentzVector Pnu = (event->PartInfo(0))->fP;
    TLorentzVector Ppi0;
    TLorentzVector Pmu;

    // Loop over the particle stack
    for (unsigned int j = 2; j < event->Npart(); ++j) {
      if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue;
      int PID = (event->PartInfo(j))->fPID;
      if (PID == 111) {
        Ppi0 = event->PartInfo(j)->fP;
      } else if (PID == -13) {
        Pmu = (event->PartInfo(j))->fP;
      }
    }

    double hadMass = FitUtils::Wrec(Pnu, Pmu);
    double Enu;

    if (hadMass > 100 && hadMass < 1800) {
      //Enu = FitUtils::EnuCC1pi0rec(Pnu, Pmu, Ppi0);
      Enu = Pnu.E()/1000.;
    } else {
      Enu = -999;
  }

  fXVar = Enu;

  return;
};

// **************************************
// MINERvA CC1pi0 in anti-neutrino mode
// Unfortunately there's no information on the neutrino component which is
// subtracted off
//
// 2014 analysis:
//                Exactly one positive muon
//                Exactly one observed pi0
//                No pi+/pi allowed
//                No information on what is done with mesons, oops?
//                No information on what is done with nucleons, oops?
//
// 2016 analysis:
//                Exactly one positive muon
//                Exactly one observed pi0
//                No other mesons
//                No restriction on number of nucleons
//
//********************************************************************
bool MINERvA_CC1pi0_XSec_1DEnu_antinu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pi(event, -14, 111, EnuMin, EnuMax);
}
