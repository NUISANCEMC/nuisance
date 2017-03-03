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

#include "MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu.h"

// The constructor
MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu::MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  fName = name;
  fPlotTitles = "; cos#theta_{#pi^{0}}; d#sigma/dcos#theta_{#pi^{0}} (cm^{2}/nucleon)";
  // (CP) I don't know what these energies should be, setting EnuMin to 5 for
  // now. You made this life choice based upon Fig 29 in arxiv:0806.1449v2
  EnuMin = 0.;
  EnuMax = 5.0;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  std::string data_filename = "";
  std::string covar_filename = "";

  nunubar_mode = fName.find("combined") != std::string::npos;
  if (!nunubar_mode) {
    data_filename = "/data/MiniBooNE/NC1pi0/nubarcosthetapi0xsec_edit.txt";
    covar_filename = "/data/MiniBooNE/NC1pi0/nubarcosthetapi0xsecerrormatrix.txt";
  } else {
    data_filename = "/data/MiniBooNE/NC1pi0/combinedsignnumodecosthetapi0xsec_edit.txt";
    covar_filename = "/data/MiniBooNE/NC1pi0/combinedsignnumodecosthetapi0xsecerrormatrix.txt";
  }

  SetDataValues(GeneralUtils::GetTopLevelDir() + data_filename);

  SetCovarMatrixFromText(GeneralUtils::GetTopLevelDir() + covar_filename, 10,1.E-5);

  SetupDefaultHist();
  StatUtils::SetDataErrorFromCov(fDataHist,fFullCovar,1E-38);

  // (CP) setting my scale factor, the paper reports in cm^2/nucleon so I
  // apparently don't need to faff with any other numbers.
  fScaleFactor = (GetEventHistogram()->Integral("width")*1E-38)/double(fNEvents)/TotalIntegratedFlux("width");
};


void MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu::FillEventVariables(FitEvent *event) {

  // (CP) require pi0 in final state (this makes some assumptions about how the
  // generator treats the pi0 after it is produced in the nucleus.
  // MB required 2 photons to make a pion signal, so check for those later
  if (event->NumFSParticle(111) == 0 ) {
    return;
  }

  if (abs(event->NumFSParticle(13)) == 1 || abs(event->NumFSParticle(11)) == 1 || abs(event->NumFSParticle(15)) == 1 || abs(event->NumFSParticle(17)) == 1) return; 

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;

  double CosPi0 = cos(FitUtils::th(Pnu, Ppi0));

  fXVar = CosPi0;

  return;
};

//********************************************************************
bool MiniBooNE_NC1pi0_XSec_1Dcospi0_antinu::isSignal(FitEvent *event) {
//********************************************************************
  if (nunubar_mode){
    return (SignalDef::isNC1pi(event, 14, 111, EnuMin, EnuMax) ||
	    SignalDef::isNC1pi(event, -14, 111, EnuMin, EnuMax));
  } else {
      return SignalDef::isNC1pi(event, -14, 111, EnuMin, EnuMax);
  }
}
