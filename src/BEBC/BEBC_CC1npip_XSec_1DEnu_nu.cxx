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

#include "BEBC_CC1npip_XSec_1DEnu_nu.h"

// The constructor
BEBC_CC1npip_XSec_1DEnu_nu::BEBC_CC1npip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  fName = "BEBC_CC1npip_XSec_1DEnu_nu";
  fPlotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/neutron)";
  EnuMin = 5.;
  EnuMax = 200.;
  fIsDiag = true; // refers to covariance matrix; this measurement has none so only use errors, not covariance
  fNormError = 0.20; // normalisation error on ANL BNL flux
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/BEBC/theses/BEBC_theses_CC1pip_on_n_W14.txt");
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)*(16./8.);
  //this->fScaleFactor = double(1.0E-38)/double(fNEvents)*(16./8.);
};


void BEBC_CC1npip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pn;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (UInt_t j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 2112) {
      Pn = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }

  double hadMass = FitUtils::MpPi(Pn, Ppip);
  double Enu;

  if (hadMass < 1400) {
    Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip);
  } else {
    Enu = -1.0;
  }

  fXVar = Enu;

  return;
};

bool BEBC_CC1npip_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2112, EnuMin, EnuMax);
}

