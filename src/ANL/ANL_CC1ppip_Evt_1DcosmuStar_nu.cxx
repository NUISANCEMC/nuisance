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

#include "ANL_CC1ppip_Evt_1DcosmuStar_nu.h"

// The constructor
ANL_CC1ppip_Evt_1DcosmuStar_nu::ANL_CC1ppip_Evt_1DcosmuStar_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  measurementName = "ANL_CC1ppip_Evt_1DcosmuStar_nu";
  plotTitles = "; cos(#theta*); Number of events";
  EnuMin = 0;
  EnuMax = 6.0;
  isDiag = true;
  isRawEvents = true;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/ANL/CC1pip_on_p/ANL_CC1ppip_cosmuStar.csv");
  this->SetupDefaultHist();

  // set Poisson errors on dataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < dataHist->GetNbinsX() + 1; i++) {
    dataHist->SetBinError(i+1, sqrt(dataHist->GetBinContent(i+1)));
  }

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar = StatUtils::GetInvert(fullcovar);

  this->scaleFactor = this->eventHist->Integral("width")/(nevents+0.)*16./8.;
};


void ANL_CC1ppip_Evt_1DcosmuStar_nu::FillEventVariables(FitEvent *event) {
  
  // set up the 4-vectors from NEUT
  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Pin = event->PartInfo(1)->fP;
  TLorentzVector Pp;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (int j = 0; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 2212) {
      Pp = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double cosmuStar;

  // Now need to boost into center-of-mass frame
  TLorentzVector CMS = Pnu + Pin;
  // Boost the muon backwards
  Pmu.Boost(-CMS.BoostVector());
  // Boost the neutrino forwards
  Pnu.Boost(CMS.BoostVector());
  
  // ANL has a M(pi, p) < 1.4 GeV cut imposed
  if (hadMass < 1400) {
    // Find angle in CMS frame
    cosmuStar = cos(FitUtils::th(Pmu, Pnu));
  } else {
    cosmuStar = -999;
  }

  this->X_VAR = cosmuStar;

  return;
};

bool ANL_CC1ppip_Evt_1DcosmuStar_nu::isSignal(FitEvent *event) {

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false; 

  int pipCnt = 0;
  int lepCnt = 0;
  int protonCnt = 0;

  for (int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 13) {
      lepCnt++;
    } else if (PID == 211) {
      pipCnt++;
    } else if (PID == 2212) {
      protonCnt++;
    } else {
      return false; // require only three prong events! (allow photons?)
    }
  }

  // don't think there's away of implementing spectator proton cuts in NEUT?
  // 100 MeV or larger protons

  if (pipCnt != 1) return false;
  if (lepCnt != 1) return false;
  if (protonCnt != 1) return false;

  return true;
}
