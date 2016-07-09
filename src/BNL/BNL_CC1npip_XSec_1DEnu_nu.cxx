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

#include "BNL_CC1npip_XSec_1DEnu_nu.h"

// The constructor
BNL_CC1npip_XSec_1DEnu_nu::BNL_CC1npip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  measurementName = "BNL_CC1npip_XSec_1DEnu_nu";
  plotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/neutron)";
  EnuMin = 0.;
  EnuMax = 3.0;
  isDiag = true;
  normError = 0.15;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/BNL/CC1pip_on_n/BNL_CC1pip_on_n_1986.txt");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  this->scaleFactor = (this->eventHist->Integral("width")*1E-38)/((nevents+0.))*16./8.;
};


void BNL_CC1npip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pn;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    } else if (PID == 2112) {
      Pn = event->PartInfo(j)->fP;
    }
  }

  double hadMass = FitUtils::MpPi(Pn, Ppip);
  // No W cut for BNL CC1pi+ on neutron (I'm happy if you can find it!!!)
  double Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip);

  this->X_VAR = Enu;

  return;
};


bool BNL_CC1npip_XSec_1DEnu_nu::isSignal(FitEvent *event) {

  // BNL has somewhat tricky signal definition for the selection:
  // P_visible = visible 4-momentum, so the three tracks total momentum
  // P_visible > 150 MeV/c2
  // Theta_Pvis_nu (angle between visible momentum and neutrino direction) < 50 degrees
  // At least one negative track leaves chamber without interacting or stops consistent with muon
  // THESE ARE NOT IMPLEMENTED BUT SHOULD BE KNOWN BY ANYONE WHO FITS THIS DATA! (see Kitagaki et al. 2556)
  //
  // The only actual restriction seems to be the final state particles. There's no mention of E_nu restrictions either (events span to 10GeV in fig 2, Kitagaki)
  // There's a mention of uncertainity in the neutrino flux for Enu > 6.0 GeV

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false;

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int pipCnt = 0;
  int lepCnt = 0;
  int neutronCnt = 0;

  for (int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 13) {
      lepCnt++;
    } else if (PID == 211) {
      pipCnt++;
    } else if (PID == 2112) {
      neutronCnt++;
    } else {
      return false; // require only three prong events! (allow photons?)
    }
  }

  // don't think there's away of implementing spectator proton cuts in NEUT?
  // 100 MeV or larger protons

  if (pipCnt != 1) return false;
  if (lepCnt != 1) return false;
  if (neutronCnt != 1) return false;

  // seems to be no additional signal requirements in BNL; all are corrected for!

  return true;
}


/*
void BNL_CC1npip_XSec_1DEnu_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void BNL_CC1npip_XSec_1DEnu_nu::ScaleEvents() {

  PlotUtils::FluxUnfoldedScaling(mcHist, fluxHist);
  PlotUtils::FluxUnfoldedScaling(mcFine, fluxHist);

  mcHist->Scale(scaleFactor);
  mcFine->Scale(scaleFactor);

  return;
}
*/
