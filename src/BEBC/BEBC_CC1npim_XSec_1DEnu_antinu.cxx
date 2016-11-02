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

#include "BEBC_CC1npim_XSec_1DEnu_antinu.h"

// The constructor
BEBC_CC1npim_XSec_1DEnu_antinu::BEBC_CC1npim_XSec_1DEnu_antinu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  fName = "BEBC_CC1npim_XSec_1DEnu_antinu";
  fPlotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/neutron)";
  EnuMin = 5.;
  EnuMax = 200.;
  fIsDiag = true;
  fNormError = 0.20;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/BEBC/theses/BEBC_theses_ANU_CC1pi-_nFin_W14.txt");
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)*(16./8.);
};


void BEBC_CC1npim_XSec_1DEnu_antinu::FillEventVariables(FitEvent *event) {
    
  if (event->NumFSParticle(2112) == 0 ||
      event->NumFSParticle(-211) == 0 ||
      event->NumFSParticle(-13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pn   = event->GetHMFSParticle(2112)->fP;
  TLorentzVector Ppim = event->GetHMFSParticle(-211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(-13)->fP;

  double hadMass = FitUtils::MpPi(Pn, Ppim);
  double Enu     = -1.0;

  if (hadMass < 1400) Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppim);
  fXVar = Enu;

  return;
};




bool BEBC_CC1npim_XSec_1DEnu_antinu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, -14, -211, 2112, EnuMin, EnuMax);
}


