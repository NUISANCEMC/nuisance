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

#include "BEBC_CC1pi0_XSec_1DQ2_nu.h"

// The constructor
BEBC_CC1pi0_XSec_1DQ2_nu::BEBC_CC1pi0_XSec_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  measurementName = "BEBC_CC1pi0_XSec_1DQ2_nu";
  plotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); d#sigma/dQ^{2} (cm^{2}/GeV^{2}/neutron)";
  EnuMin = 5.;
  EnuMax = 200.;
  isDiag = true;
  normError = 0.20;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/BEBC/Dfill/BEBC_Dfill_CC1pi0_on_n_W14_edit.txt");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  hadMassHist = new TH1D((measurementName+"_Wrec").c_str(),(measurementName+"_Wrec").c_str(), 100, 1000, 2000);
  hadMassHist->SetTitle((measurementName+"; W_{rec} (GeV/c^{2}); Area norm. # of events").c_str());

  this->scaleFactor = (this->eventHist->Integral("width")*1E-38)/((nevents+0.)*this->TotalIntegratedFlux("width"))*16./8.;
};


void BEBC_CC1pi0_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Pp;
  TLorentzVector Ppi0;
  TLorentzVector Pmu;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
    } else if (PID == 2212) {
      Pp = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }

  hadMass = FitUtils::MpPi(Pp, Ppi0);
  double q2CCpi0;
  
  // BEBC has a M(pi, p) < 1.4 GeV cut imposed
  if (hadMass < 1400 && hadMass > 1100) {
    q2CCpi0 = FitUtils::Q2CC1pi0rec(Pnu, Pmu, Ppi0);
  } else {
    q2CCpi0 = -1.0;
  }

  this->X_VAR = q2CCpi0;

  return;
};

bool BEBC_CC1pi0_XSec_1DQ2_nu::isSignal(FitEvent *event) {

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
    } else if (PID == 2212) {
      protonCnt++;
    } else {
      return false; // require only three prong events! (allow photons?)
    }
  }

  if (pi0Cnt != 1) return false;
  if (lepCnt != 1) return false;
  if (protonCnt != 1) return false;

  return true;
}

void BEBC_CC1pi0_XSec_1DQ2_nu::FillHistograms() {

  Measurement1D::FillHistograms();

  hadMassHist->Fill(hadMass);

  return;
}

void BEBC_CC1pi0_XSec_1DQ2_nu::Write(std::string drawOpt) {
  
  Measurement1D::Write(drawOpt);
  hadMassHist->Scale(1/hadMassHist->Integral());
  hadMassHist->Write();

  return;
}
