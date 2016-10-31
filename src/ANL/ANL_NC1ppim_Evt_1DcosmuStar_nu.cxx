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
#include "ANL_NC1ppim_Evt_1DcosmuStar_nu.h"

/** 
  * M. Derrick et al., "Study of the reaction \nu n \rightarrow \nu p \pi^-", Physics Letters, Volume 92B, Number 3,4, 363, 19 May 1980
*/

ANL_NC1ppim_Evt_1DcosmuStar_nu::ANL_NC1ppim_Evt_1DcosmuStar_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  fName = "ANL_NC1ppim_Evt_1DcosmuStar_nu";
  fPlotTitles = "; cos*_{#mu}; Number of events";
  EnuMin = 0.3;
  EnuMax = 1.5;
  fIsDiag = true;
  fIsRawEvents = true;
  fIsEnu1D = false;
  fDefaultTypes="EVT/SHAPE/DIAG";
  fAllowedTypes="EVT/SHAPE/DIAG";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/ANL/NC1ppim/ANL_NC1ppim_cosMuStar.csv");
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  // Set Poisson errors on data points (number of events weighted)
  for (int i = 0; i < fDataHist->GetNbinsX()+1; ++i) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  this->fScaleFactor = this->fEventHist->Integral("width")/((fNEvents+0.)*fFluxHist->Integral("width"))*(16./8.);
};

void ANL_NC1ppim_Evt_1DcosmuStar_nu::FillEventVariables(FitEvent *event) {
  
  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Pin = event->PartInfo(1)->fP;
  TLorentzVector Pp;
  TLorentzVector Ppim;
  TLorentzVector PnuOut;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (UInt_t j =  2; j < event->Npart(); ++j){

    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0 && (event->PartInfo(j)->fNEUTStatusCode != 2)) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;

    if (PID == -211) {
      Ppim = event->PartInfo(j)->fP;
    } else if (PID == 2212) {
      Pp = event->PartInfo(j)->fP;
    } else if (PID == 14) {
      PnuOut = event->PartInfo(j)->fP;
    }
  }

  // Boost into centre of mass frame
  TLorentzVector CMS = Pnu + Pin;
  // Boost outgoing neutrino backwards CMS
  PnuOut.Boost(-CMS.BoostVector());
  // Boost incoming neutrino forwards by CMS
  Pnu.Boost(CMS.BoostVector());

  double cosmuStar = cos(FitUtils::th(PnuOut, Pnu));

  this->fXVar = cosmuStar;

  return;
};

bool ANL_NC1ppim_Evt_1DcosmuStar_nu::isSignal(FitEvent *event) {
  return SignalDef::isNC1pi3Prong(event, 14, -211, 2212, EnuMin, EnuMax);
}

