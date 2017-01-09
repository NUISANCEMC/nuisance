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

#include "BNL_CC1ppip_Evt_1Dphi_nu.h"

// The constructor
BNL_CC1ppip_Evt_1Dphi_nu::BNL_CC1ppip_Evt_1Dphi_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {

  fName = "BNL_CC1ppip_Evt_1Dphi_nu";
  fPlotTitles = "; #phi_{Adler}; Number of events";
  EnuMin = 0;
  EnuMax = 6.0;
  fIsDiag = true;
  fIsRawEvents = true;
  fAllowedTypes += "EVT";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/BNL/CC1pip_on_p/BNL_CC1ppip_W14_phiAdler.csv");
  this->SetupDefaultHist();

  // set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  TRandom3 rand;

  this->fScaleFactor = GetEventHistogram()->Integral("width")/(fNEvents+0.)*16./8.;
};


void BNL_CC1ppip_Evt_1Dphi_nu::FillEventVariables(FitEvent *event) {

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
  // Boost the pion 4-vector into the resonance 4-vector rest-frame
  Ppip.Boost(Pres.BoostVector());
  Pmu.Boost(-Pres.BoostVector());
  Pp.Boost(-Pres.BoostVector());

  // Get the vectors from the 4-vector
  TVector3 PmuVect = Pmu.Vect();
  TVector3 PnuVect = Pnu.Vect();
  TVector3 PresVect = Pres.Vect();
  TVector3 PpipVect = Ppip.Vect();

  // Define y direction as being z (resonance direction) x pmu*
  TVector3 zVect = (PnuVect-PmuVect);
  zVect *= 1/double(zVect.Mag());
  TVector3 yVect = zVect.Cross(PmuVect);
  // Normalise yVector
  yVect *= 1/double(yVect.Mag());
  // define x direction as being y X z
  TVector3 xVect = yVect.Cross(zVect);
  // Normalise zVector
  xVect *= 1/double(xVect.Mag());

  // Project pion onto z axis
  TVector3 PpipVectZ = zVect * PpipVect.Dot(zVect);
  // Then subtract this vector off the pion vector
  TVector3 PpipVectPlane = PpipVect - PpipVectZ;

  // Then finally construct phi as the angle between pion projection and x axis
  double phi = -999;

  // BNL has a M(pi, p) < 1.4 GeV cut imposed
  if (hadMass < 1400) {
    if (PpipVectPlane.Y() > 0) {
      phi = (180./M_PI)*PpipVectPlane.Angle(xVect);
    } else if (PpipVectPlane.Y() < 0) {
      phi = (180./M_PI)*(2*M_PI-PpipVectPlane.Angle(xVect));
    } else if (PpipVectPlane.Y() == 0) {
      double randNo = rand.Rndm();
      if (randNo > 0.5) {
        phi = (180./M_PI)*PpipVectPlane.Angle(xVect);
      } else {
        phi = (180./M_PI)*(2*M_PI-PpipVectPlane.Angle(xVect));
      }
    }
  }

  fXVar = phi;

  return;
};

bool BNL_CC1ppip_Evt_1Dphi_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212,EnuMin,EnuMax);
}

/*
void BNL_CC1ppip_Evt_1Dphi_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void BNL_CC1ppip_Evt_1Dphi_nu::ScaleEvents() {

  PlotUtils::FluxUnfoldedScaling(fMCHist, GetFluxHistogram());
  PlotUtils::FluxUnfoldedScaling(fMCFine, GetFluxHistogram());

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fScaleFactor);

  return;
}
*/
