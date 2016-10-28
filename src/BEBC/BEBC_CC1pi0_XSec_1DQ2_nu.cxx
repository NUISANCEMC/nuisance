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

#include "BEBC_CC1pi0_XSec_1DQ2_nu.h"

// The constructor
BEBC_CC1pi0_XSec_1DQ2_nu::BEBC_CC1pi0_XSec_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  fName = "BEBC_CC1pi0_XSec_1DQ2_nu";
  fPlotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); d#sigma/dQ^{2} (cm^{2}/GeV^{2}/neutron)";
  EnuMin = 5.;
  EnuMax = 200.;
  fIsDiag = true;
  fNormError = 0.20;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/BEBC/Dfill/BEBC_Dfill_CC1pi0_on_n_W14_edit.txt");
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  hadMassHist = new TH1D((fName+"_Wrec").c_str(),(fName+"_Wrec").c_str(), 100, 1000, 2000);
  hadMassHist->SetTitle((fName+"; W_{rec} (GeV/c^{2}); Area norm. # of events").c_str());

  this->fScaleFactor = (this->fEventHist->Integral("width")*1E-38)/((fNEvents+0.)*this->TotalIntegratedFlux("width"))*16./8.;
};


void BEBC_CC1pi0_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Pp;
  TLorentzVector Ppi0;
  TLorentzVector Pmu;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (UInt_t j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue; //move on if NOT ALIVE and NOT NORMAL
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

  fXVar = q2CCpi0;

  return;
};

bool BEBC_CC1pi0_XSec_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 111, 2212, EnuMin, EnuMax);
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
