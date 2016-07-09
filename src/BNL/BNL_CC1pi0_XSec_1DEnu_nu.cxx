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

#include "BNL_CC1pi0_XSec_1DEnu_nu.h"

// The constructor
BNL_CC1pi0_XSec_1DEnu_nu::BNL_CC1pi0_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  measurementName = "BNL_CC1pi0_XSec_1DEnu_nu";
  plotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/neutron)";
  EnuMin = 0.;
  EnuMax = 6.0;
  isDiag = true;
  normError = 0.15;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);
  // THIS IS DEFINITELY DODGY WITH THE "CORRECTION"; EnuMax for BNL definitely stops at 3GeV because of flux uncertainties, although correction goes to 6!

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/BNL/CC1pi0_on_n/BNL_CC1pi0_on_n_1986.txt");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar = StatUtils::GetInvert(fullcovar);

  this->scaleFactor = (this->eventHist->Integral("width")*1E-38)/(nevents+0.)*16./8.;
};


void BNL_CC1pi0_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

    TLorentzVector Pnu = (event->PartInfo(0))->fP;
    TLorentzVector Pp;
    TLorentzVector Pmu;
    TLorentzVector Ppi0;

    // Loop over the particle stack
    for (int j = 2; j < event->Npart(); ++j){
      if (!(event->PartInfo(j))->fIsAlive || (event->PartInfo(j))->fStatus != 0) continue;
      int PID = (event->PartInfo(j))->fPID;
      if (PID == 111) {
        Ppi0 = event->PartInfo(j)->fP;
      } else if (PID == 13) {
        Pmu = (event->PartInfo(j))->fP;
      } else if (PID == 2212) {
        Pp = event->PartInfo(j)->fP;
      }
    }

    double hadMass = FitUtils::MpPi(Pp, Ppi0);

    //BNL doesn't have a W cut for CC1pi0 sadly (I'm super happy if you can find it!)
    double Enu = FitUtils::EnuCC1pi0rec(Pnu, Pmu, Ppi0);

    this->X_VAR = Enu;

    return;
  }


bool BNL_CC1pi0_XSec_1DEnu_nu::isSignal(FitEvent *event) {

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false;

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int pi0Cnt = 0;
  int lepCnt = 0;
  int protonCnt = 0;

  // Look for final state particles
  for (int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) || (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
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

  // don't think there's away of implementing spectator proton cuts in NEUT?
  // 100 MeV or larger protons

  if (pi0Cnt != 1)
    return false;
  if (lepCnt != 1)
    return false;
  if (protonCnt != 1)
    return false;

  return true;
}


/*
void BNL_CC1pi0_XSec_1DEnu_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void BNL_CC1pi0_XSec_1DEnu_nu::ScaleEvents() {

  PlotUtils::FluxUnfoldedScaling(mcHist, fluxHist);
  PlotUtils::FluxUnfoldedScaling(mcFine, fluxHist);

  mcHist->Scale(scaleFactor);
  mcFine->Scale(scaleFactor);

  return;
}
*/
