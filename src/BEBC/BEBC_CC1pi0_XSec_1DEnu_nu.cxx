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

#include "BEBC_CC1pi0_XSec_1DEnu_nu.h"

// The constructor
BEBC_CC1pi0_XSec_1DEnu_nu::BEBC_CC1pi0_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  measurementName = "BEBC_CC1pi0_XSec_1DEnu_nu";
  plotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/neutron)";
  EnuMin = 5.;
  EnuMax = 200.;
  isDiag = true; // refers to covariance matrix; this measurement has none so only use errors, not covariance
  normError = 0.20; // normalisation error on ANL BNL flux
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/BEBC/theses/BEBC_theses_CC1pi0_W14.txt");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  this->scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)*(16./8.);
  //this->scaleFactor = double(1.0E-38)/double(nevents)*(16./8.);
};

void BEBC_CC1pi0_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pp;
  TLorentzVector Ppi0;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
    } else if (PID == 2212) {
      Pp = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  double hadMass = FitUtils::MpPi(Pp, Ppi0);
  double Enu;

  // weirdly, the Enu distribution does not have a W < 1.4GeV cut
  if (hadMass < 1400) {
    //std::cout << "hadMass = " << hadMass << "      " << "Enu = " << Enu << std::endl;
    //std::cout << "Enu vec = " << Pnu.E() << std::endl;
    Enu = FitUtils::EnuCC1pi0rec(Pnu, Pmu, Ppi0);
  } else {
    Enu = -1.0;
  }

  this->X_VAR = Enu;

  return;
};


bool BEBC_CC1pi0_XSec_1DEnu_nu::isSignal(FitEvent *event) {

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false; 

  int pi0Cnt = 0;
  int lepCnt = 0;
  int protonCnt = 0;

  for (int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 13) {
      lepCnt++;
    } else if (PID == 111) {
      pi0Cnt++;
    } else if (PID == 2212 /*&& event->Ibound == 0*/) {
      protonCnt++;
    } else {
      return false; // require only three prong events! (allow photons?)
    }
  }

  // don't think there's away of implementing spectator proton cuts in NEUT?
  // 100 MeV or larger protons

  if (pi0Cnt != 1) return false;
  if (lepCnt != 1) return false;
  if (protonCnt != 1) return false;

  return true;
}

