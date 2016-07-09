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

#include "ANL_CC1ppip_XSec_1DEnu_nu.h"

// The constructor
ANL_CC1ppip_XSec_1DEnu_nu::ANL_CC1ppip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  measurementName = "ANL_CC1ppip_XSec_1DEnu_nu";
  plotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/nucleon)";
  EnuMin = 0.;
  EnuMax = 6.0;
  isDiag = true; // refers to covariance matrix; this measurement has none so only use errors, not covariance
  normError = 0.20; // normalisation error on ANL BNL flux
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/ANL/CC1pip_on_p/anl82corr-numu-p-to-mu-p-piplus-lowW_edges.txt");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  this->scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)*(16./8.);
};



void ANL_CC1ppip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Pmu;
  TLorentzVector Ppip;
  TLorentzVector Pp;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j) {
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 2212)  {
      Pp = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double Enu; // declare here to save memory (beware of conflict with private in Measurement1D)
      
  // ANL has a M(pi, p) < 1.4 GeV cut imposed
  if (hadMass < 1400) { // hadronic mass cut
    Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip);
  } else {
    Enu = -1.0;
  }

  this->X_VAR = Enu;

  return;
}


bool ANL_CC1ppip_XSec_1DEnu_nu::isSignal(FitEvent *event) {

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
    } else if (PID == 2212 /* && event->Ibound == 0*/) { // TEMPORARY!!!!!!! ONLY TESTING A THING
      protonCnt++;
    } else {
      return false; // require only three prong events! (allow photons?)
    }
  }

  if (pipCnt != 1) return false;
  if (lepCnt != 1) return false;
  if (protonCnt != 1) return false;

  return true;
}


/*
void ANL_CC1ppip_XSec_1DEnu_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void ANL_CC1ppip_XSec_1DEnu_nu::ScaleEvents() {
  
  PlotUtils::FluxUnfoldedScaling(mcHist, fluxHist);
  PlotUtils::FluxUnfoldedScaling(mcFine, fluxHist);

  mcHist->Scale(scaleFactor);
  mcFine->Scale(scaleFactor);

  return;
}
*/
