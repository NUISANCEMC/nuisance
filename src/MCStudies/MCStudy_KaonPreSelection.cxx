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

#include "MCStudy_KaonPreSelection.h"

#include "T2K_SignalDef.h"
#include "MINERvA_SignalDef.h"

//********************************************************************
/// @brief Class to perform MC Studies on a custom measurement
MCStudy_KaonPreSelection::MCStudy_KaonPreSelection(std::string name, std::string inputfile,
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
  this->fDataHist = new TH1D(("empty_data"), ("empty-data"), 1, 0, 1);
  this->SetupDefaultHist();
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  // 1. The generator is organised in SetupMeasurement so it gives the
  // cross-section in "per nucleon" units.
  //    So some extra scaling for a specific measurement may be required. For
  //    Example to get a "per neutron" measurement on carbon
  //    which we do here, we have to multiple by the number of nucleons 12 and
  //    divide by the number of neutrons 6.
  this->fScaleFactor = (this->fEventHist->Integral("width") * 1E-38 / (fNEvents + 0.)) /
    this->TotalIntegratedFlux();

  // Create a new TTree and add Nuisance Events Branches
  FitPar::Config().out->cd();
  fEventTree = new TTree("nuisance_events","nuisance_events");
  GetInput()->GetEventPointer()->AddBranchesToTree(fEventTree);
  
  // Add some extra variables they might need?
  // Enu, PDGnu, PDGLep, CosLep, ELep, TLep?
  
  // Add Event Scaling Information
  // This scale factor is used to get the predicted event rate for this sample given                                                                                                                                        
  // the input flux. Use this when merging different output event ttrees             
  fEventTree->Branch("EventScaleFactor", &fEventScaleFactor, "EventScaleFactor/D");
  fEventScaleFactor = fEventHist->Integral("width") * 1E-38 / (fNEvents + 0.);

  // NOTES:
  // To get normalised predictions weight event event by 'EventScaleFactor' to get the
  // predicted event rate for that sample given the flux used. Then to get cross-sections
  // divide by the integrated flux from all samples. 
  // e.g. To get the numu+numubar prediction, add the event rate predictions from both
  // samples together, then divide by the integral of the 'nuisance_flux' histograms in each 
  // sample.

  // Every particle in the nuisance event is saved into the TTree. The list of particle
  // status codes are given in src/FitBase/FitParticle.h. The neutrino is usually the first
  // particle in the list.
  // If selecting final state kaons, select only kaons with state=2.

  /*
    enum particle_state{
    kUndefinedState = 5,
    kInitialState   = 0,
    kFSIState       = 1,
    kFinalState     = 2,
    kNuclearInitial = 3,
    kNuclearRemnant = 4
    };
  */

  // The structure of the particle lists are a dimensional array for each particle mom, then a 1D array
  // for the PDG and state. Mode gives the true NEUT interaction channel code.
  /*
    tn->Branch("Mode", &fMode, "Mode/I");
    tn->Branch("EventNo", &fEventNo, "EventNo/i");
    tn->Branch("TotCrs", &fTotCrs, "TotCrs/D");
    tn->Branch("TargetA", &fTargetA, "TargetA/I");
    tn->Branch("TargetH", &fTargetH, "TargetH/I");
    tn->Branch("Bound", &fBound, "Bound/O");
    
    tn->Branch("InputWeight", &InputWeight, "InputWeight/D");
    
    tn->Branch("NParticles", &fNParticles, "NParticles/I");
    tn->Branch("ParticleState", fParticleState, "ParticleState[NParticles]/i");
    tn->Branch("ParticlePDG", fParticlePDG, "ParticlePDG[NParticles]/I");
    tn->Branch("ParticleMom", fParticleMom, "ParticleMom[NParticles][4]/D");
  */
  
  return;
}

//********************************************************************
void MCStudy_KaonPreSelection::FillEventVariables(FitEvent *event) {
//********************************************************************

  // Fill Event Variables for Muon/Neutrino?
  
  // Fill the TTree only for signal
  if (isSignal(event)){
    fEventTree->Fill();
  }

  return;
};

//********************************************************************
void MCStudy_KaonPreSelection::Write(std::string drawOpt) {
//********************************************************************

  // Save the event ttree
  fEventTree->Write();

  // Save Flux and Event Histograms too
  GetInput()->GetFluxHistogram()->Write("nuisance_eventhist");
  GetInput()->GetEventHistogram()->Write("nuisance_fluxhist");

  return;
}


//********************************************************************
/// Select only events with final state Kaons
bool MCStudy_KaonPreSelection::isSignal(FitEvent *event) {
//********************************************************************

  // Apply a Kaon Pre-selection
  // Search for Strange Mesons (included full list from MC PDG)

  /*
  PhystConst::pdg_strangemesons = {130,310,311,321,
				   9000311,9000321,
				   10311,10321,100311,100321,
				   9010311,9010321,9020311,9020321,
				   313,323,
				   10313,10323,
				   20313,20323,
				   100313,100323,
				   9000313,9000323,
				   30313,30323,
				   315,325,
				   9000315,9000325,
				   10315,10325,
				   20315,20325,
				   9010315,9010325,9020315,9020325,
				   317,327,
				   9010317,9010327};
  PhysConst::pdg_antistrangemesons = {above * -1.0};					      
  */

  int nstrangemesons = event->NumFSParticle(PhysConst::pdg_strangemesons);
  nstrangemesons += event->NumFSParticle(PhysConst::pdg_antistrangemesons);
  if (nstrangemesons < 1) return false;

  // Do we want any other signal?
  return true;
};

// -------------------------------------------------------------------
// Purely MC Plot
// Following functions are just overrides to handle this
// -------------------------------------------------------------------
//********************************************************************
void MCStudy_KaonPreSelection::ScaleEvents() {
  //********************************************************************
  // Saving everything to a TTree so no scaling required
  return;
}

//********************************************************************
void MCStudy_KaonPreSelection::ApplyNormScale(float norm) {
//********************************************************************

  // Saving everything to a TTree so no scaling required
  this->fCurrentNorm = norm;
  return;
}

//********************************************************************
void MCStudy_KaonPreSelection::FillHistograms() {
//********************************************************************
  // No Histograms need filling........
  return;
}

//********************************************************************
void MCStudy_KaonPreSelection::ResetAll() {
  //********************************************************************
  fEventTree->Reset();
  return;
}

//********************************************************************
float MCStudy_KaonPreSelection::GetChi2() {
  //********************************************************************
  // No Likelihood to test, purely MC
  return 0.0;
}
