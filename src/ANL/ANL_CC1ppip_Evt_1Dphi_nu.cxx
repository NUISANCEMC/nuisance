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

/**
 * Radecky et al. Phys Rev D, 3rd series, Vol 25, No 5, 1 March 1982, p 1161-1173
*/
#include "ANL_CC1ppip_Evt_1Dphi_nu.h"



//********************************************************************
ANL_CC1ppip_Evt_1Dphi_nu::ANL_CC1ppip_Evt_1Dphi_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "ANL CC1npip Event Rate 1DcosmuStar nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle(" #phi_{Adler}");
  fSettings.SetYTitle("Number of events");
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.SetEnuRange(0.0, 6.0);
  fSettings.DefineAllowedTargets("D,H");

  // CCQELike plot information
  fSettings.SetTitle("ANL #nu_mu CC1n#pi^{+}");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/data/ANL/CC1pip_on_p/ANL_CC1pip_on_p_noEvents_phiAdler_1982.csv" );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width")/(fNEvents+0.)*2./1.;

  // Plot Setup -------------------------------------------------------
  SetDataValues( fSettings.GetDataInput() );
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }
    TRandom3 rand;
  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


void ANL_CC1ppip_Evt_1Dphi_nu::FillEventVariables(FitEvent *event) {

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
  // Boost the outgoing and incoming particles into the resonance frame
  Pnu.Boost(Pres.BoostVector());
  Pmu.Boost(-Pres.BoostVector());
  Ppip.Boost(-Pres.BoostVector());

  // Get the vectors from the 4-vector
  TVector3 PmuVect = Pmu.Vect();
  TVector3 PnuVect = Pnu.Vect();
  TVector3 PresVect = Pres.Vect();
  TVector3 PpipVect = Ppip.Vect();

  // Define the z-direction
  TVector3 zVect = (PnuVect-PmuVect);
  zVect *= 1/double(zVect.Mag());
  // Define y direction as being z (resonance direction) x pmu*
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

  // ANL has a M(pi, p) < 1.4 GeV cut imposed
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

bool ANL_CC1ppip_Evt_1Dphi_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}

/*
void ANL_CC1ppip_Evt_1Dphi_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void ANL_CC1ppip_Evt_1Dphi_nu::ScaleEvents() {

  PlotUtils::FluxUnfoldedScaling(fMCHist, GetFluxHistogram());
  PlotUtils::FluxUnfoldedScaling(fMCFine, GetFluxHistogram());

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fScaleFactor);

  return;
}
*/
