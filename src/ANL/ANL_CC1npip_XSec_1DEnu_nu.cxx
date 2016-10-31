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

#include "ANL_CC1npip_XSec_1DEnu_nu.h"

// The constructor
ANL_CC1npip_XSec_1DEnu_nu::ANL_CC1npip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  // Measurement Details
  fName = "ANL_CC1npip_XSec_1DEnu_nu";
  fPlotTitles = "; E_{#nu} (GeV^{2}); #sigma (cm^{2}/nucleon)";
  EnuMin = 0.;
  EnuMax = 1.5;
  fIsDiag = true;
  fNormError = 0.20;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile); 
  
  // Setup Plots
  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/ANL/CC1pip_on_n/anl82corr-numu-n-to-mu-n-piplus-lowW_edges.txt");
  this->SetupDefaultHist();
  
  // Setup Covariance
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);
  
  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)*(16./8.); // NEUT (16./8. from /nucleus -> /nucleon scaling for nucleon interactions)
};

void ANL_CC1npip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;
  TLorentzVector Pn;

  // Loop over the particle stack
  for (UInt_t j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue;
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
  double Enu;
    
  if (hadMass < 1400) {
    Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip);
  } else {
    Enu = -1.0;
  }

  fXVar = Enu;

  return;
};

bool ANL_CC1npip_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2112, EnuMin, EnuMax);
}

/*
void ANL_CC1npip_XSec_1DEnu_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void ANL_CC1npip_XSec_1DEnu_nu::ScaleEvents() {
  
  PlotUtils::FluxUnfoldedScaling(fMCHist, fFluxHist);
  PlotUtils::FluxUnfoldedScaling(fMCFine, fFluxHist);

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fScaleFactor);

  return;
}
*/

