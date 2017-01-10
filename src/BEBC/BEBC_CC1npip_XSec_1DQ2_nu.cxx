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

#include "BEBC_CC1npip_XSec_1DQ2_nu.h"

// The constructor
BEBC_CC1npip_XSec_1DQ2_nu::BEBC_CC1npip_XSec_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {

  fName = "BEBC_CC1npip_XSec_1DQ2_nu";
  fPlotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); d#sigma/dQ^{2} (cm^{2}/GeV^{2}/neutron)";
  EnuMin = 5;
  EnuMax = 200;
  fIsDiag = true;
  fNormError = 0.20;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/BEBC/Dfill/BEBC_Dfill_CC1pi+_on_n_W14_edit.txt");
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  hadMassHist = new TH1D((fName+"_Wrec").c_str(),(fName+"_Wrec").c_str(), 100, 1000, 2000);
  hadMassHist->SetTitle((fName+"; W_{rec} (GeV/c^{2}); Area norm. # of events").c_str());

  this->fScaleFactor = (GetEventHistogram()->Integral("width")*1E-38)/((fNEvents+0.)*this->TotalIntegratedFlux("width"))*16./8.;
};


void BEBC_CC1npip_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2112) == 0 ||
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pn   = event->GetHMFSParticle(2112)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  hadMass = FitUtils::MpPi(Pn, Ppip);
  double q2CCpip = -1.0;

  // BEBC has a 1.1GeV < M(pi, p) < 1.4 GeV cut imposed (also no cut measurement but not useful for delta tuning)
  if (hadMass < 1400) q2CCpip = -1*(Pnu-Pmu).Mag2()/1.E6;

  fXVar = q2CCpip;

  return;
};

bool BEBC_CC1npip_XSec_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2112, EnuMin, EnuMax);
}

void BEBC_CC1npip_XSec_1DQ2_nu::FillHistograms() {

  Measurement1D::FillHistograms();

  hadMassHist->Fill(hadMass);

  return;
}

void BEBC_CC1npip_XSec_1DQ2_nu::Write(std::string drawOpt) {

  Measurement1D::Write(drawOpt);
  hadMassHist->Scale(1/hadMassHist->Integral());
  hadMassHist->Write();

  return;
}
