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
#include "ANL_NC1npip_Evt_1Dppi_nu.h"

/** 
  * M. Derrick et al., "Study of single-pion production by weak neutral currents in low-energy \nu d interactions", Physical Review D, Volume 23, Number 3, 569, 1 February 1981
*/

ANL_NC1npip_Evt_1Dppi_nu::ANL_NC1npip_Evt_1Dppi_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  fName = "ANL_CC1npip_Evt_1Dppi_nu";
  fPlotTitles = "; p_{#pi} (MeV); Number of events";
  fDefaultTypes="EVT/SHAPE/DIAG";
  fAllowedTypes="EVT/SHAPE/DIAG";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // There are two different measurements here; _weight and _unweight
  // See publication for information
  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/ANL/NC1npip/ANL_ppi_NC1npip_weight.csv");
  //this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/ANL/NC1npip/ANL_ppi_NC1npip_weight.csv");
  this->SetupDefaultHist();

  // set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = this->fEventHist->Integral("width")/((fNEvents+0.)*fFluxHist->Integral("width"))*(16./8.);
};

void ANL_NC1npip_Evt_1Dppi_nu::FillEventVariables(FitEvent *event) {
  
  TLorentzVector Pn;
  TLorentzVector Ppip;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (UInt_t j =  2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211)
      Ppip = event->PartInfo(j)->fP;
    else if (PID == 2112)
      Pn = event->PartInfo(j)->fP;
  }

  double hadMass = FitUtils::MpPi(Pn, Ppip);
  double ppip;
  
  // ANL has a M(pi, p) < 1.4 GeV cut imposed
  if (hadMass < 1400) {
    ppip = FitUtils::p(Ppip)*1000.;
  } else {
    ppip = -1.0;
  }

  fXVar = ppip;

  return;
};

bool ANL_NC1npip_Evt_1Dppi_nu::isSignal(FitEvent *event) {

  // Incoming particle should be a neutrino
  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false; 

  // Outgoing particle should be a neutrino
  if (((event->PartInfo(2))->fPID != 14) && ((event->PartInfo(3))->fPID != 14)) return false; 

  int pipCnt = 0;
  int neutronCnt = 0;

  for (UInt_t j =  2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fNEUTStatusCode != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
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
  if (neutronCnt != 1) return false;

  return true;
}

