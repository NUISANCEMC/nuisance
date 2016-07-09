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

#include "MiniBooNE_CC1pip_XSec_1DEnu_nu.h"

// The constructor
//********************************************************************   
MiniBooNE_CC1pip_XSec_1DEnu_nu::MiniBooNE_CC1pip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  measurementName = "MiniBooNE_CC1pip_XSec_1DEnu_nu";
  plotTitles = "; E_{#nu} (MeV); #sigma(E_{#nu}) (cm^{2}/CH_{2})";
  EnuMin = 0.5;
  EnuMax = 2.;
  isDiag = true;
  isEnu1D = true;
  normError = 0.107;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MiniBooNE/CC1pip/ccpipXSec_enu.txt");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar = StatUtils::GetInvert(fullcovar);
  //StatUtils::ForceNormIntoCovar(this->covar, this->dataHist, this->normError);

  this->scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)*(14.08);
};


//********************************************************************   
void MiniBooNE_CC1pip_XSec_1DEnu_nu::FillEventVariables(FitEvent* event) {
//********************************************************************   
  
  // Set up the 4-vectors
  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;
  TLorentzVector Ppip;
  
  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }

  // No W cut on MiniBooNE data from publication
  // WARNING: DRAGONS LAY HERE! Mike Wilking's thesis might not have this. Beware that the publication says W < 1.35 GeV, but this is "efficiency corrected", i.e. FILLED WITH MONTE-CARLO!!!!!!!! AAAAAH
  //double hadMass = FitUtils::Wrec(Pnu, Pmu, Ppip);

  double Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip);
  //  if (isRat) Enu = Enu/1000.; //CCpi+/CCQE ratio paper puts in GeV, CCpi+ paper in MeV

  this->X_VAR = Enu;

  return;
};

bool MiniBooNE_CC1pip_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pip_MiniBooNE(event, EnuMin, EnuMax);
}
