// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#include "GenericVectorsInputHandler.h"
#include "InputUtils.h"

GenericVectorsInputHandler::GenericVectorsInputHandler(std::string const &handle,
                                           std::string const &rawinputs) {
  NUIS_LOG(SAM, "Creating GenericVectorsInputHandler : " << handle);

  // Run a joint input handling
  fName = handle;
  fFitEventTree = new TChain("FlatTree_VARS");
  fCacheSize = FitPar::Config().GetParI("CacheSize");

  std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
  for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
    // Open File for histogram access
    TFile *inp_file = new TFile(inputs[inp_it].c_str(), "READ");
    if (!inp_file or inp_file->IsZombie()) {
      NUIS_ABORT("FitEvent File IsZombie() at " << inputs[inp_it]);
    }

    // Get Flux/Event hist
    TH1D *fluxhist = (TH1D *)inp_file->Get("FlatTree_FLUX");
    TH1D *eventhist = (TH1D *)inp_file->Get("FlatTree_EVT");
    if (!fluxhist or !eventhist) {
      NUIS_ABORT("FitEvent FILE doesn't contain flux/xsec info");
    }

    // Get N Events
    TTree *eventtree = (TTree *)inp_file->Get("FlatTree_VARS");
    if (!eventtree) {
      NUIS_ABORT("nuisance_events not located in GENIE file! " << inputs[inp_it]);
    }
    int nevents = eventtree->GetEntries();

    // Register input to form flux/event rate hists
    RegisterJointInput(inputs[inp_it], nevents, fluxhist, eventhist);

    // Add to TChain
    fFitEventTree->Add(inputs[inp_it].c_str());
  }

  // Registor all our file inputs
  SetupJointInputs();

  // Assign to tree
  fEventType = kINPUTFITEVENT;

  // Create Fit Event
  fNUISANCEEvent = new FitEvent();
  fNUISANCEEvent->HardReset();

  fFitEventTree->SetBranchAddress("Mode", &FlatTreeMode);
  fFitEventTree->SetBranchAddress("tgta", &FlatTreeTargetA);
  fFitEventTree->SetBranchAddress("tgtz", &FlatTreeTargetZ);
  fFitEventTree->SetBranchAddress("tgt", &FlatTreeTargetPDG);


  // Save outgoing particle vectors
  fFitEventTree->SetBranchAddress("nfsp", &nfsp);
  fFitEventTree->SetBranchAddress("px", px_fsp);
  fFitEventTree->SetBranchAddress("py", py_fsp);
  fFitEventTree->SetBranchAddress("pz", pz_fsp);
  fFitEventTree->SetBranchAddress("E", E_fsp);
  fFitEventTree->SetBranchAddress("pdg", pdg_fsp);
  // fFitEventTree->SetBranchAddress("pdg_rank", pdg_fsp);

  // Save init particle vectors
  fFitEventTree->SetBranchAddress("ninitp", &ninitp);
  fFitEventTree->SetBranchAddress("px_init", px_init);
  fFitEventTree->SetBranchAddress("py_init", py_init);
  fFitEventTree->SetBranchAddress("pz_init", pz_init);
  fFitEventTree->SetBranchAddress("E_init", E_init);
  fFitEventTree->SetBranchAddress("pdg_init", pdg_init);

  // Save pre-FSI vectors
  fFitEventTree->SetBranchAddress("nvertp", &nvertp);
  fFitEventTree->SetBranchAddress("px_vert", px_vert);
  fFitEventTree->SetBranchAddress("py_vert", py_vert);
  fFitEventTree->SetBranchAddress("pz_vert", pz_vert);
  fFitEventTree->SetBranchAddress("E_vert", E_vert);
  fFitEventTree->SetBranchAddress("pdg_vert", pdg_vert);

  fFitEventTree->SetBranchAddress("InputWeight", &FlatTreeInputWeight);
  fFitEventTree->SetBranchAddress("RWWeight", &FlatTreeRWWeight);
  fFitEventTree->SetBranchAddress("Weight", &FlatTreeWeight);


  // fFitEventTree->Show(0);
  fNUISANCEEvent = GetNuisanceEvent(0);
  // std::cout << "NParticles = " << fNUISANCEEvent->Npart() << std::endl;
  // std::cout << "Event Info " << fNUISANCEEvent->PartInfo(0)->fPID << std::endl;
}

GenericVectorsInputHandler::~GenericVectorsInputHandler() {
  if (fFitEventTree)
    delete fFitEventTree;
}

void GenericVectorsInputHandler::CreateCache() {
  //    fFitEventTree->SetCacheEntryRange(0, fNEvents);
  //    fFitEventTree->AddBranchToCache("*", 1);
  //    fFitEventTree->SetCacheSize(fCacheSize);
}

void GenericVectorsInputHandler::RemoveCache() {
  // fFitEventTree->SetCacheEntryRange(0, fNEvents);
  //    fFitEventTree->AddBranchToCache("*", 0);
  //    fFitEventTree->SetCacheSize(0);
}

FitEvent *GenericVectorsInputHandler::GetNuisanceEvent(const UInt_t entry,
                                                 const bool lightweight) {

  (void)lightweight;
  // Return NULL if out of bounds
  if (entry >= (UInt_t)fNEvents)
    return NULL;

  // Reset all variables before tree read
  fNUISANCEEvent->ResetEvent();

  // Read NUISANCE Tree
  fFitEventTree->GetEntry(entry);

  fNUISANCEEvent->Mode = FlatTreeMode;
  fNUISANCEEvent->fEventNo = entry;
  fNUISANCEEvent->fTargetA = FlatTreeTargetA;
  fNUISANCEEvent->fTargetZ = FlatTreeTargetZ;
  fNUISANCEEvent->fTargetPDG = FlatTreeTargetPDG;


  // Only allow free protons
  fNUISANCEEvent->fTargetH = FlatTreeTargetA == 1;
  fNUISANCEEvent->fBound   = FlatTreeTargetA != 1;

  // Fill Stack
  fNUISANCEEvent->fNParticles = 0;

  for (int i = 0; i < ninitp; i++) {
    size_t curpart = fNUISANCEEvent->fNParticles;
    fNUISANCEEvent->fParticleState[curpart] = kInitialState;

    // Mom
    fNUISANCEEvent->fParticleMom[curpart][0] = px_init[i]*1000.0;
    fNUISANCEEvent->fParticleMom[curpart][1] = py_init[i]*1000.0;
    fNUISANCEEvent->fParticleMom[curpart][2] = pz_init[i]*1000.0;
    fNUISANCEEvent->fParticleMom[curpart][3] = E_init[i]*1000.0;

    // PDG
    fNUISANCEEvent->fParticlePDG[curpart] = pdg_init[i];

    // Add to N particle count
    fNUISANCEEvent->fNParticles++;
  }

  // Don't add the vert stack as its combined init/final
  for (int i = 0; i < nvertp; i++) {
    size_t curpart = fNUISANCEEvent->fNParticles;

    // have to check not already in init or fsp
    bool duplicate = false;
    for (int j = 0; j < nfsp; j++) {
      if (E_fsp[j] == E_vert[i] && pdg_fsp[j] == pdg_vert[i]) {
        duplicate = true;
        break;
      }
    }

    for (int j = 0; j < ninitp; j++) {
      if (E_init[j] == E_vert[i] && pdg_init[j] == pdg_vert[i]) {
        duplicate = true;
        break;
      }
    }

    if (duplicate) continue;

    fNUISANCEEvent->fParticleState[curpart] = kFSIState;

    // Mom
    fNUISANCEEvent->fParticleMom[curpart][0] = px_vert[i]*1000.0;
    fNUISANCEEvent->fParticleMom[curpart][1] = py_vert[i]*1000.0;
    fNUISANCEEvent->fParticleMom[curpart][2] = pz_vert[i]*1000.0;
    fNUISANCEEvent->fParticleMom[curpart][3] = E_vert[i]*1000.0;

    // PDG
    fNUISANCEEvent->fParticlePDG[curpart] = pdg_vert[i];

    // Add to N particle count
    fNUISANCEEvent->fNParticles++;
  }


  for (int i = 0; i < nfsp; i++) {
    size_t curpart = fNUISANCEEvent->fNParticles;
    fNUISANCEEvent->fParticleState[curpart] = kFinalState;

    // Mom
    fNUISANCEEvent->fParticleMom[curpart][0] = px_fsp[i]*1000.0;
    fNUISANCEEvent->fParticleMom[curpart][1] = py_fsp[i]*1000.0;
    fNUISANCEEvent->fParticleMom[curpart][2] = pz_fsp[i]*1000.0;
    fNUISANCEEvent->fParticleMom[curpart][3] = E_fsp[i]*1000.0;

    // PDG
    fNUISANCEEvent->fParticlePDG[curpart] = pdg_fsp[i];

    // Add to N particle count
    fNUISANCEEvent->fNParticles++;
  }

  fNUISANCEEvent->OrderStack();

  // Setup Input scaling for joint inputs
  fNUISANCEEvent->InputWeight = GetInputWeight(entry);

  return fNUISANCEEvent;
}

double GenericVectorsInputHandler::GetInputWeight(int entry) {
  double w = (InputHandlerBase::GetInputWeight(entry) *
    FlatTreeInputWeight * FlatTreeRWWeight);
  return w * fNUISANCEEvent->SavedRWWeight;
}

void GenericVectorsInputHandler::Print() {}
