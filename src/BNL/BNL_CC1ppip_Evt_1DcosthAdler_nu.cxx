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

#include "BNL_CC1ppip_Evt_1DcosthAdler_nu.h"

// The constructor
BNL_CC1ppip_Evt_1DcosthAdler_nu::BNL_CC1ppip_Evt_1DcosthAdler_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  fName = "BNL_CC1ppip_Evt_1DcosthAdler_nu";
  fPlotTitles = "; cos#theta_{Adler}; Number of events";
  EnuMin = 0;
  EnuMax = 6.0;
  fIsDiag = true;
  fIsRawEvents = true;
  fAllowedTypes += "EVT";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/BNL/CC1pip_on_p/BNL_CC1ppip_W14_cosThAdler.csv");
  this->SetupDefaultHist();

  // set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = this->fEventHist->Integral("width")/(fNEvents+0.)*16./8.;
};


void BNL_CC1ppip_Evt_1DcosthAdler_nu::FillEventVariables(FitEvent *event) {
  
  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;
  
  // Get the hadronic mass
  double hadMass = FitUtils::MpPi(Pp, Ppip);
  // Need to boost pion and muon into resonance rest-frame to get phi (e.g. see F. Sanchez arxiv 1511.00501v2)
  //
  // Get the resonance 4-vector
  TLorentzVector Pres = Ppip + Pp;
  Ppip.Boost(-Pres.BoostVector());
  Pmu.Boost(-Pres.BoostVector());
  Pnu.Boost(Pres.BoostVector());
  // Define the vectors
  TVector3 PpipVect = Ppip.Vect();
  TVector3 PnuVect = Pnu.Vect();
  TVector3 PmuVect = Pmu.Vect();
  // Define the z-direction; should be same as Pres
  TVector3 zVect = (PnuVect-PmuVect);
  zVect *= 1/double(zVect.Mag());

  // Then finally construct phi as the angle between pion projection and x axis
  double cosThAdler = -999;
  
  // BNL has a M(pi, p) < 1.4 GeV cut imposed
  if (hadMass < 1400)
    cosThAdler = cos(PpipVect.Angle(zVect));

  fXVar = cosThAdler;

  return;
};

bool BNL_CC1ppip_Evt_1DcosthAdler_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212,EnuMin,EnuMax);
}

/*
void BNL_CC1ppip_Evt_1DcosthAdler_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void BNL_CC1ppip_Evt_1DcosthAdler_nu::ScaleEvents() {
  
  PlotUtils::FluxUnfoldedScaling(fMCHist, fFluxHist);
  PlotUtils::FluxUnfoldedScaling(fMCFine, fFluxHist);

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fScaleFactor);

  return;
}
*/
