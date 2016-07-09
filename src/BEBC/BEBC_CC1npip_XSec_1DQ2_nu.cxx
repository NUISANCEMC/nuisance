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

#include "BEBC_CC1npip_XSec_1DQ2_nu.h"

// The constructor
BEBC_CC1npip_XSec_1DQ2_nu::BEBC_CC1npip_XSec_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  measurementName = "BEBC_CC1npip_XSec_1DQ2_nu";
  plotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); d#sigma/dQ^{2} (cm^{2}/GeV^{2}/neutron)";
  EnuMin = 5;
  EnuMax = 200;
  isDiag = true;
  normError = 0.20;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/BEBC/Dfill/BEBC_Dfill_CC1pi+_on_n_W14_edit.txt");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  hadMassHist = new TH1D((measurementName+"_Wrec").c_str(),(measurementName+"_Wrec").c_str(), 100, 1000, 2000);
  hadMassHist->SetTitle((measurementName+"; W_{rec} (GeV/c^{2}); Area norm. # of events").c_str());

  this->scaleFactor = (this->eventHist->Integral("width")*1E-38)/((nevents+0.)*this->TotalIntegratedFlux("width"))*16./8.;
};


void BEBC_CC1npip_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {
  
  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Pn;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 2112) {
      Pn = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }

  hadMass = FitUtils::MpPi(Pn, Ppip);
  double q2CCpip;
  
  // BEBC has a 1.1GeV < M(pi, p) < 1.4 GeV cut imposed (also no cut measurement but not useful for delta tuning)
  if (hadMass < 1400 && hadMass > 1100) {
    q2CCpip = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip);
  } else {
    q2CCpip = -1.0;
  }

  this->X_VAR = q2CCpip;

  return;
};

bool BEBC_CC1npip_XSec_1DQ2_nu::isSignal(FitEvent *event) {

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false; 

  int pipCnt = 0;
  int lepCnt = 0;
  int neutronCnt = 0;

  for (int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 13) 
      lepCnt++;
    else if (PID == 211) 
      pipCnt++;
    else if (PID == 2112) 
      neutronCnt++;
    else 
      return false; // require only three prong events! (allow photons?)
  }

  // don't think there's away of implementing spectator proton cuts in NEUT?
  // 100 MeV or larger protons

  if (pipCnt != 1) return false;
  if (lepCnt != 1) return false;
  if (neutronCnt != 1) return false;

  // This is corrected for
  // this channel requires theta_(neutron, nu) > 10 deg
  // theta_(mu, pi) < 150 deg
  // p_neutron / p_neutrino < 0.9
  /*
  double th_neutron_neutrino = FitUtils::th(Pn, Pnu)*180/M_PI;
  double th_mu_pi = FitUtils::th(Pmu, Ppip)*180/M_PI;
  double p_neutron = FitUtils::p(Pn);
  double p_neutrino = FitUtils::p(Pnu);

  if (th_neutron_neutrino < 10 || th_mu_pi > 150 || p_neutron/p_neutrino > 0.9) return false;
  */

  return true;
}

void BEBC_CC1npip_XSec_1DQ2_nu::FillHistograms() {

  Measurement1D::FillHistograms();

  hadMassHist->Fill(hadMass);

  return;
}

void BEBC_CC1npip_XSec_1DQ2_nu::Write(std::string drawOpt) {
  
  Measurement1D::Write(drawOpt);
  hadMassHist->Scale(1/hadMassHist->Integral());
  hadMassHist->Write();

  return;
}
