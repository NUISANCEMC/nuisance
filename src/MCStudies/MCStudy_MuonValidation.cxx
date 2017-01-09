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

#include "MCStudy_MuonValidation.h"

#include "T2K_SignalDef.h"
#include "MINERvA_SignalDef.h"

//********************************************************************
/// @brief Class to perform MC Studies on a custom measurement
MCStudy_MuonValidation::MCStudy_MuonValidation(std::string name, std::string inputfile,
                                       FitWeight *rw, std::string type,
                                       std::string fakeDataFile) {
  //********************************************************************

  // Measurement Details
  fName = name;
  fEventTree = NULL;

  // Define our energy range for flux calcs
  EnuMin = 0.;
  EnuMax = 100.;  // Arbritrarily high energy limit

  // Set default fitter flags
  fIsDiag = true;
  fIsShape = false;
  fIsRawEvents = false;

  // This function will sort out the input files automatically and parse all the
  // inputs,flags,etc.
  // There may be complex cases where you have to do this by hand, but usually
  // this will do.
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);
  fEventTree = NULL;

  // Setup fDataHist as a placeholder
  this->fDataHist = new TH1D(("approximate_data"), ("kaon_data"), 5, 1.0, 6.0);

  this->SetupDefaultHist();
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  // 1. The generator is organised in SetupMeasurement so it gives the
  // cross-section in "per nucleon" units.
  //    So some extra scaling for a specific measurement may be required. For
  //    Example to get a "per neutron" measurement on carbon
  //    which we do here, we have to multiple by the number of nucleons 12 and
  //    divide by the number of neutrons 6.
  this->fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) /
    this->TotalIntegratedFlux();

  // Create a new TTree and add Nuisance Events Branches
  FitPar::Config().out->cd();
  fEventTree = new TTree((fName + "_EVENTS").c_str(),(fName + "_EVENTS").c_str());

  fEventTree->Branch("ScaleFactor", &fScaleFactor, "ScaleFactor/D");
  fEventTree->Branch("InputWeight", &LocalInputWeight, "InputWeight/D");
  fEventTree->Branch("RWWeight", &LocalRWWeight, "RWWeight/D");
  fEventTree->Branch("Mode", &Mode, "Mode/I");
  fEventTree->Branch("Enu",&Enu,"Enu/F");
  fEventTree->Branch("TLep",&TLep,"TLep/F");
  fEventTree->Branch("CosLep",&CosLep,"CosLep/F");
  fEventTree->Branch("Q2",&Q2,"Q2/F");
  fEventTree->Branch("Q2QE",&Q2QE,"Q2QE/F");
  fEventTree->Branch("EQE",&EQE,"EQE/F");
  fEventTree->Branch("q0",&q0,"q0/F");
  fEventTree->Branch("q3",&q3,"q3/F");

  // the input flux. Use this when merging different output event ttrees
  fEventTree->Branch("EventScaleFactor", &fEventScaleFactor, "EventScaleFactor/D");
  fEventScaleFactor = GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.);

  return;
}

//********************************************************************
void MCStudy_MuonValidation::FillEventVariables(FitEvent *event) {
//********************************************************************

  FitParticle* muon = NULL;
  FitParticle* nu   = event->GetNeutrinoIn();
  bool IsNuMu = event->PDGnu() > 0;

  if (IsNuMu)  muon = event->GetHMFSParticle(13);
  else muon = event->GetHMFSParticle(-13);

  // Reset Variables
  Enu    = -999.9;
  TLep   = -999.9;
  CosLep = -999.9;
  Q2     = -999.9;
  Q2QE   = -999.9;
  EQE    = -999.9;
  q0     = -999.9;
  q3     = -999.9;

  // Fill Variables
  if (muon){

    Enu = event->Enu() / 1.E3;

    TLep   = (muon->fP.E() - muon->fP.Mag()) / 1.E3;
    CosLep = cos(muon->fP.Vect().Angle( nu->fP.Vect() ));

    Q2   = fabs((muon->fP - nu->fP).Mag2() / 1.E6);

    Q2QE = FitUtils::Q2QErec(muon->fP, CosLep, 34., IsNuMu);
    EQE  = FitUtils::EnuQErec(muon->fP, CosLep, 34., IsNuMu);

    q0 = fabs((muon->fP - nu->fP).E()) / 1.E3;
    q3 = fabs((muon->fP - nu->fP).Vect().Mag()) / 1.E3;

    LocalRWWeight = event->RWWeight;
    LocalInputWeight = event->InputWeight;

  }

  // Fill Tree
  if (isSignal(event)){
    fEventTree->Fill();
  }

  return;
};

//********************************************************************
void MCStudy_MuonValidation::Write(std::string drawOpt) {
//********************************************************************

  // Save the event ttree
  fEventTree->Write();

  // Save Flux and Event Histograms too
  GetInput()->GetFluxHistogram()->Write((fName + "_FLUX").c_str());
  GetInput()->GetEventHistogram()->Write((fName + "_EVT").c_str());

  return;
}


//********************************************************************
/// Select only events with final state Muons
bool MCStudy_MuonValidation::isSignal(FitEvent *event) {
//********************************************************************

  if (!event->HasFSMuon()) return false;

  // Do we want any other signal?
  return true;
};

