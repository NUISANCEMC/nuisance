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

#include "ANL_CC1ppip_XSec_1DQ2_nu.h"

// The constructor
ANL_CC1ppip_XSec_1DQ2_nu::ANL_CC1ppip_XSec_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
  
  fName = "ANL_CC1ppip_XSec_1DQ2_nu";
  fPlotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); d#sigma/dQ_{CC#pi^{+}}^{2} (cm^{2}/GeV^{2}/proton)";
  EnuMin = 0;
  EnuMax = 6;
  fIsDiag = true;
  fNormError = 0.20;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/ANL/CC1pip_on_p/ANL_CC1pip_on_p_dSigdQ2_W14_1982.txt");
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = (this->fEventHist->Integral("width")*1E-38)/((fNEvents+0.)*TotalIntegratedFlux("width"))*16./8.;
};


void ANL_CC1ppip_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {
  
  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Pp;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (UInt_t j =  2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue; //move on if NOT ALIVE and NOT NORMAL
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
  double q2CCpip;

  // I use the W < 1.4GeV cut ANL data to isolate single pion
  // there is also a W < 1.6 GeV and an uncut spectrum ANL 1982
  
  if (hadMass < 1400) {
    q2CCpip = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip);
  } else {
    q2CCpip = -1.0;
  }

  fXVar = q2CCpip;

  return;
};

bool ANL_CC1ppip_XSec_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}

