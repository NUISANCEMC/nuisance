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

#include "T2K2017_FakeData.h"

//********************************************************************
/// @brief Class to perform MC Studies on a custom measurement
T2K2017_FakeData::T2K2017_FakeData(nuiskey samplekey) {
//********************************************************************

   // Sample overview ---------------------------------------------------
  std::string descrip = "T2K2017 Event Kinematics MC Study \n" \
                        "Target: CH/H2O \n" \
                        "Flux: T2K numu flux \n" \
                        "Signal: numuCC \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);

  fSettings.SetTitle("T2K MC #nu_{#mu} CC");
  fSettings.SetDescription(descrip);
  fSettings.SetEnuRange(0.0, 6.0);
  fSettings.DefineAllowedTargets("H,C,O");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  fScaleFactor = GetEventHistogram()->Integral("width")/(fNEvents+0.)/TotalIntegratedFlux();

  // Setup TTree to Save -------------------------------------------
  SetEmptyData();

  fEventTree = new TTree("EventTree", "EventTree");
  fEventTree->Branch("Enu", &fEnu, "Enu/D");

  fEventTree->Branch("Tmu", &fTMu, "Tmu/D");
  fEventTree->Branch("Cosmu", &fCosMu, "Cosmu/D");

  fEventTree->Branch("Ppip", &fPPip, "Ppip/D");
  fEventTree->Branch("Cospip", &fCosPip, "Cospip/D");
  fEventTree->Branch("Ppim", &fPPim, "Ppim/D");
  fEventTree->Branch("Cospim", &fCosPim, "Cospim/D");
  fEventTree->Branch("Ppi0", &fPPi0, "Ppi0/D");
  fEventTree->Branch("Cospi0", &fCosPi0, "Cospi0/D");

  fEventTree->Branch("Mode", &fMode, "Mode/I");
  fEventTree->Branch("NuPDG", &fNuPDG, "NuPDG/I");
  fEventTree->Branch("ScaleFactor", &fScaleFactor, "ScaleFactor/D");

    // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void T2K2017_FakeData::FillEventVariables(FitEvent *event) {
//********************************************************************

  // Reset Variables
  fEnu    = -999.9;

  fTMu    = -999.9;
  fCosMu  = -999.9;

  fPPip   = -999.9;
  fCosPip = -999.9;
  fPPim   = -999.9;
  fCosPim = -999.9;
  fPPi0   = -999.9;
  fCosPi0 = -999.9;

  fMode  = 0;
  fNuPDG = 0;

  // Get neutrino information
  if (abs(event->PDGnu()) != 14) return;
  FitParticle* nu = event->GetNeutrinoIn();
  fEnu = nu->fP.E();


  // Get Muon Information
  if ((event->NumFSParticle(13) +
       event->NumFSParticle(-13)) < 1) { return; }

  FitParticle* muon;
  if (nu->fPID == 14) {
    muon = event->GetHMFSParticle(13);
  } else if (nu->fPID == -14) {
    muon = event->GetHMFSParticle(-13);
  }
  fTMu = FitUtils::T(muon->fP);
  fCosMu = cos(muon->fP.Vect().Angle(nu->fP.Vect()));


  // Get Pion+ Information
  if (event->NumFSParticle(211) >= 1) {

    FitParticle* pionp = event->GetHMFSParticle(211);
    fPPip   = pionp->fP.Vect().Mag();
    fCosPip = cos(pionp->fP.Vect().Angle(nu->fP.Vect()));

  }

  // Get Pion- Information
  if (event->NumFSParticle(-211) >= 1) {

    FitParticle* pionm = event->GetHMFSParticle(-211);
    fPPim   = pionm->fP.Vect().Mag();
    fCosPim = cos(pionm->fP.Vect().Angle(nu->fP.Vect()));

  }

  // Get Pion0 Information
  if (event->NumFSParticle(111) >= 1) {

    FitParticle* pion0 = event->GetHMFSParticle(111);
    fPPi0   = pion0->fP.Vect().Mag();
    fCosPi0 = cos(pion0->fP.Vect().Angle(nu->fP.Vect()));

  }


  return;
};

//********************************************************************
void T2K2017_FakeData::Write(std::string drawOpt) {
//********************************************************************
  fEventTree->Write();
  return;
}

// -------------------------------------------------------------------
// Purely MC Plot
// Following functions are just overrides to handle this
// -------------------------------------------------------------------
//********************************************************************
/// Everything is classed as signal...
bool T2K2017_FakeData::isSignal(FitEvent *event) {
//********************************************************************

  if (abs(event->PDGnu()) != 14) return false;
  if ((event->NumFSParticle(13) +
       event->NumFSParticle(-13)) < 1) { return false; }

  return true;
};

//********************************************************************
void T2K2017_FakeData::ScaleEvents() {
//********************************************************************
  // Saving everything to a TTree so no scaling required
  return;
}

//********************************************************************
void T2K2017_FakeData::ApplyNormScale(float norm) {
  //********************************************************************

  // Saving everything to a TTree so no scaling required
  this->fCurrentNorm = norm;
  return;
}

//********************************************************************
void T2K2017_FakeData::FillHistograms() {
//********************************************************************
  return;
}

//********************************************************************
void T2K2017_FakeData::ResetAll() {
  //********************************************************************
  fEventTree->Reset();
  return;
}

//********************************************************************
float T2K2017_FakeData::GetChi2() {
  //********************************************************************
  // No Likelihood to test, purely MC
  return 0.0;
}
