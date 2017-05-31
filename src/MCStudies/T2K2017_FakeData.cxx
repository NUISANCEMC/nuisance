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
  fScaleFactor = GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.) / TotalIntegratedFlux();

  // Setup some MC Histograms
  const int nbin_x = 14;
  const double binedge_x[15] = {0., 300., 400., 500., 600., 700., 800., 900., 1000., 1250., 1500., 2000., 3000., 5000., 30000.};
  const int nbin_y = 10;
  const double binedge_y[11] = { -1, 0.6, 0.7, 0.8, 0.85, 0.9, 0.92, 0.94, 0.96, 0.98, 1.};

  fMCHist_FGD1NuMuCC0Pi = new TH2D((fName + "_FGD1NuMuCC0Pi_MC").c_str(),
                                   (fName + "_FGD1NuMuCC0Pi_MC").c_str(),
                                   nbin_x, binedge_x, nbin_y, binedge_y);

  fMCHistPDG_FGD1NuMuCC0Pi = new TrueModeStack((fName + "_FGD1NuMuCC0Pi_MC_PDG").c_str(),
      (fName + "_FGD1NuMuCC0Pi_MC_PDG").c_str(),
      fMCHist_FGD1NuMuCC0Pi);

  fMCFine_FGD1NuMuCC0Pi = new TH2D((fName + "_FGD1NuMuCC0Pi_MC_FINE").c_str(),
                                   (fName + "_FGD1NuMuCC0Pi_MC_FINE").c_str(),
                                   40, 0., 1000., 40, -1., 1.);

  fMCFinePDG_FGD1NuMuCC0Pi = new TrueModeStack((fName + "_FGD1NuMuCC0Pi_MC_FINE_PDG").c_str(),
      (fName + "_FGD1NuMuCC0Pi_MC_FINE_PDG").c_str(),
      fMCFine_FGD1NuMuCC0Pi);

  SetAutoProcessTH1(fMCHist_FGD1NuMuCC0Pi);
  SetAutoProcessTH1(fMCHistPDG_FGD1NuMuCC0Pi);
  SetAutoProcessTH1(fMCFine_FGD1NuMuCC0Pi);
  SetAutoProcessTH1(fMCFinePDG_FGD1NuMuCC0Pi);

  // Setup TTree to Save -------------------------------------------
  SetEmptyData();

  if (samplekey.Has("saveflattree")) {
    fSaveEventTree = samplekey.GetI("saveflattree") > 0;
  } else {
    fSaveEventTree = false;
  }


  if (fSaveEventTree) {
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
  }

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

  FitParticle* muon = NULL;
  if (nu->fPID == 14) {
    muon = event->GetHMFSParticle(13);
  } else if (nu->fPID == -14) {
    muon = event->GetHMFSParticle(-13);
  }
  fTMu = FitUtils::T(muon->fP) * 1.E3;
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
  if (fSaveEventTree) {
    fEventTree->Write();
  }

  fMCHist_FGD1NuMuCC0Pi->Write();
  fMCHistPDG_FGD1NuMuCC0Pi->Write();

  fMCFine_FGD1NuMuCC0Pi->Write();
  fMCFinePDG_FGD1NuMuCC0Pi->Write();


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
void T2K2017_FakeData::FillHistograms() {
//********************************************************************

  fMCHist_FGD1NuMuCC0Pi->Fill(fTMu, fCosMu, Weight);
  fMCHistPDG_FGD1NuMuCC0Pi->Fill(Mode, fTMu, fCosMu, Weight);

  fMCFine_FGD1NuMuCC0Pi->Fill(fTMu, fCosMu, Weight);
  fMCFinePDG_FGD1NuMuCC0Pi->Fill(Mode, fTMu, fCosMu, Weight);

  return;
}

//********************************************************************
void T2K2017_FakeData::ResetAll() {
  //********************************************************************
  Measurement1D::ResetAll();

  if (fSaveEventTree) {
    fEventTree->Reset();
  }
  return;
}

//********************************************************************
float T2K2017_FakeData::GetChi2() {
  //********************************************************************
  // No Likelihood to test, purely MC
  return 0.0;
}
