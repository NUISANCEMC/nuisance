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
#include "GENIEInputHandler.h"
#include "InteractionModes.h"
#ifdef GENIE3_API_ENABLED
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/GHEP/GHepUtils.h"
#include "Framework/Conventions/Units.h"
#include "Framework/Messenger/Messenger.h"
#else
#include "GHEP/GHepParticle.h"
#include "PDG/PDGUtils.h"
#include "GHEP/GHepUtils.h"
#include "Conventions/Units.h"
#include "Messenger/Messenger.h"
#endif

#include "InputUtils.h"

GENIEGeneratorInfo::~GENIEGeneratorInfo() { DeallocateParticleStack(); }

void GENIEGeneratorInfo::AddBranchesToTree(TTree *tn) {
  tn->Branch("GenieParticlePDGs", &fGenieParticlePDGs, "GenieParticlePDGs/I");
}

void GENIEGeneratorInfo::SetBranchesFromTree(TTree *tn) {
  tn->SetBranchAddress("GenieParticlePDGs", &fGenieParticlePDGs);
}

void GENIEGeneratorInfo::AllocateParticleStack(int stacksize) {
  fGenieParticlePDGs = new int[stacksize];
}

void GENIEGeneratorInfo::DeallocateParticleStack() {
  delete fGenieParticlePDGs;
}

void GENIEGeneratorInfo::FillGeneratorInfo(NtpMCEventRecord *ntpl) {
  Reset();

  // Check for GENIE Event
  if (!ntpl)
    return;
  if (!ntpl->event)
    return;

  // Cast Event Record
  GHepRecord *ghep = static_cast<GHepRecord *>(ntpl->event);
  if (!ghep)
    return;

  // Fill Particle Stack
  GHepParticle *p = 0;
  TObjArrayIter iter(ghep);

  // Loop over all particles
  int i = 0;
  while ((p = (dynamic_cast<genie::GHepParticle *>((iter).Next())))) {
    if (!p)
      continue;

    // Get PDG
    fGenieParticlePDGs[i] = p->Pdg();
    i++;
  }
}

void GENIEGeneratorInfo::Reset() {
  for (int i = 0; i < kMaxParticles; i++) {
    fGenieParticlePDGs[i] = 0;
  }
}

bool GENIEInputHandler::IsPrimary(GHepParticle *p) {

  // If initial state nucleon, or nucleon target, it is definintely primary!
  if (p->Status() == genie::kIStInitialState ||
      p->Status() == genie::kIStNucleonTarget)  return true;

  // Reject intermediate resonant state or pre-DIS state
  if (p->Status() == genie::kIStDISPreFragmHadronicState || // DIS before fragmentation
      p->Status() == genie::kIStPreDecayResonantState || // pre decay resonance state
      p->Status() == genie::kIStIntermediateState || // intermediate state
      p->Status() == genie::kIStDecayedState || // Decayed state
      p->Status() == genie::kIStUndefined) return false; // undefined state

  // Check if the mother is the neutrino or IS nucleon
  if (p->FirstMother() < 2) return true;

  // We've now filtered off intermediate states and obvious initial states

  // Loop over this particle's mothers, grandmothers, and so on cleaning out intermediate particles
  // Set the starting particle to be our particle
  GHepParticle *mother = fGenieGHep->Particle(p->FirstMother());
  while (mother->FirstMother() > 1) {
    // It could be that the mother's status is actually a decayed state linked back to the vertex
    if (mother->Status() == genie::kIStDecayedState || // A decayed state
        mother->Status() == genie::kIStDISPreFragmHadronicState || // A DIS state before fragmentation
        mother->Status() == genie::kIStPreDecayResonantState ) {  // A pre-decay resonant state
      mother = fGenieGHep->Particle(mother->FirstMother());
    } else { // If not, move out of the loop
      break;
    }
  }

  // Then do a simple check of mother is associated with the primary
  int MotherID = mother->FirstMother();
  //if (MotherID > 2) return false;

  // Finally, this should mean that our partcile is marked for transport through the nucleus
  // Could also be interactions of free proton
  if (p->Status() == genie::kIStHadronInTheNucleus ||  // Then require the particle to be paseed to FSI
      (p->Status() == genie::kIStStableFinalState && // Can also have interaction on free proton
       fGenieGHep->Summary()->InitState().TgtPtr()->A() == 1 &&
       fGenieGHep->Summary()->InitState().TgtPtr()->Z() == 1) ) {
    return true;
  } 

  return false;
}

GENIEInputHandler::GENIEInputHandler(std::string const &handle,
    std::string const &rawinputs) {
  NUIS_LOG(SAM, "Creating GENIEInputHandler : " << handle);

  // Plz no shouting
  StopTalking();
  genie::Messenger::Instance()->SetPriorityLevel("GHepUtils", pFATAL);
  StartTalking();
  // Shout all you want

  // Run a joint input handling
  fName = handle;

  // Setup the TChain
  fGENIETree = new TChain("gtree");
  fSaveExtra = FitPar::Config().GetParB("SaveExtraGenie");
  fCacheSize = FitPar::Config().GetParI("CacheSize");
  fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");

  // Loop over all inputs and grab flux, eventhist, and nevents
  std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
  for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
    // Open File for histogram access
    TFile *inp_file = new TFile(
        InputUtils::ExpandInputDirectories(inputs[inp_it]).c_str(), "READ");
    if (!inp_file or inp_file->IsZombie()) {
      NUIS_ABORT(
          "GENIE File IsZombie() at : '"
          << inputs[inp_it] << "'" << std::endl
          << "Check that your file paths are correct and the file exists!"
          << std::endl
          << "$ ls -lh " << inputs[inp_it]);
    }

    // Get Flux/Event hist
    TH1D *fluxhist = (TH1D *)inp_file->Get("nuisance_flux");
    TH1D *eventhist = (TH1D *)inp_file->Get("nuisance_events");
    if (!fluxhist or !eventhist) {
      NUIS_ERR(FTL, "Input File Contents: " << inputs[inp_it]);
      inp_file->ls();
      NUIS_ABORT("GENIE FILE doesn't contain flux/xsec info."
          << std::endl
          << "Try running the app PrepareGENIE first on :"
          << inputs[inp_it] << std::endl
          << "$ PrepareGENIE -h");
    }

    // Get N Events
    TTree *genietree = (TTree *)inp_file->Get("gtree");
    if (!genietree) {
      NUIS_ERR(FTL, "gtree not located in GENIE file: " << inputs[inp_it]);
      NUIS_ABORT(
          "Check your inputs, they may need to be completely regenerated!");
    }

    int nevents = genietree->GetEntries();
    if (nevents <= 0) {
      NUIS_ABORT("Trying to a TTree with "
          << nevents << " to TChain from : " << inputs[inp_it]);
    }

    // Register input to form flux/event rate hists
    RegisterJointInput(inputs[inp_it], nevents, fluxhist, eventhist);

    // Add To TChain
    fGENIETree->AddFile(inputs[inp_it].c_str());
  }

  // Registor all our file inputs
  SetupJointInputs();

  // Assign to tree
  fEventType = kGENIE;
  fGenieNtpl = NULL;
  fGENIETree->SetBranchAddress("gmcrec", &fGenieNtpl);

  // Libraries should be seen but not heard...
  StopTalking();
  fGENIETree->GetEntry(0);
  StartTalking();

  // Create Fit Event
  fNUISANCEEvent = new FitEvent();
  fNUISANCEEvent->SetGenieEvent(fGenieNtpl);

  if (fSaveExtra) {
    fGenieInfo = new GENIEGeneratorInfo();
    fNUISANCEEvent->AddGeneratorInfo(fGenieInfo);
  }

  fNUISANCEEvent->HardReset();
};

GENIEInputHandler::~GENIEInputHandler() {
  // if (fGenieGHep) delete fGenieGHep;
  // if (fGenieNtpl) delete fGenieNtpl;
  // if (fGENIETree) delete fGENIETree;
  // if (fGenieInfo) delete fGenieInfo;
}

void GENIEInputHandler::CreateCache() {
  if (fCacheSize > 0) {
    // fGENIETree->SetCacheEntryRange(0, fNEvents);
    fGENIETree->AddBranchToCache("*", 1);
    fGENIETree->SetCacheSize(fCacheSize);
  }
}

void GENIEInputHandler::RemoveCache() {
  // fGENIETree->SetCacheEntryRange(0, fNEvents);
  fGENIETree->AddBranchToCache("*", 0);
  fGENIETree->SetCacheSize(0);
}

FitEvent *GENIEInputHandler::GetNuisanceEvent(const UInt_t ent,
    const bool lightweight) {
  UInt_t entry = ent + fSkip;
  if (entry >= (UInt_t)fNEvents)
    return NULL;

  // Clear the previous event (See Note 1 in ROOT TClonesArray documentation)
  if (fGenieNtpl) {
    fGenieNtpl->Clear();
  }

  // Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
  fGENIETree->GetEntry(entry);

  // Run NUISANCE Vector Filler
  if (!lightweight) {
    CalcNUISANCEKinematics();
  }

#ifdef Prob3plusplus_ENABLED
  else {
    // Check for GENIE Event
    if (!fGenieNtpl)
      return NULL;
    if (!fGenieNtpl->event)
      return NULL;

    // Cast Event Record
    fGenieGHep = static_cast<GHepRecord *>(fGenieNtpl->event);
    if (!fGenieGHep)
      return NULL;

    TObjArrayIter iter(fGenieGHep);
    genie::GHepParticle *p;
    while ((p = (dynamic_cast<genie::GHepParticle *>((iter).Next())))) {
      if (!p) {
        continue;
      }

      // Get Status
      int state = GetGENIEParticleStatus(p, fNUISANCEEvent->Mode);
      if (state != genie::kIStInitialState) {
        continue;
      }
      fNUISANCEEvent->probe_E = p->E() * 1.E3;
      fNUISANCEEvent->probe_pdg = p->Pdg();
      break;
    }
  }
#endif

  // Setup Input scaling for joint inputs
  fNUISANCEEvent->InputWeight = GetInputWeight(entry);

  return fNUISANCEEvent;
}

int GENIEInputHandler::GetGENIEParticleStatus(genie::GHepParticle *p,
    int mode) {
  /*
     kIStUndefined                  = -1,
     kIStInitialState               =  0,   / generator-level initial state /
     kIStStableFinalState           =  1,   / generator-level final state:
     particles to be tracked by detector-level MC /
     kIStIntermediateState          =  2,
     kIStDecayedState               =  3,
     kIStCorrelatedNucleon          = 10,
     kIStNucleonTarget              = 11,
     kIStDISPreFragmHadronicState   = 12,
     kIStPreDecayResonantState      = 13,
     kIStHadronInTheNucleus         = 14,   / hadrons inside the nucleus: marked
     for hadron transport modules to act on /
     kIStFinalStateNuclearRemnant   = 15,   / low energy nuclear fragments
     entering the record collectively as a 'hadronic blob' pseudo-particle /
     kIStNucleonClusterTarget       = 16,   // for composite nucleons before
     phase space decay
     */

  int state = kUndefinedState;
  switch (p->Status()) {
    case genie::kIStNucleonTarget:
    case genie::kIStInitialState:
    case genie::kIStCorrelatedNucleon:
    case genie::kIStNucleonClusterTarget:
      state = kInitialState;
      break;

    case genie::kIStStableFinalState:
      state = kFinalState;
      break;

    case genie::kIStHadronInTheNucleus:
      if (abs(mode) == 2)
        state = kInitialState;
      else
        state = kFSIState;
      break;

    case genie::kIStPreDecayResonantState:
    case genie::kIStDISPreFragmHadronicState:
    case genie::kIStIntermediateState:
      state = kFSIState;
      break;

    case genie::kIStFinalStateNuclearRemnant:
    case genie::kIStUndefined:
    case genie::kIStDecayedState:
    default:
      break;
  }

  // Flag to remove nuclear part in genie
  if (kRemoveNuclearParticles) {
    if (p->Pdg() > 1000000 && 
        pdg::IsNeutronOrProton(
          fGenieGHep->Summary()->InitState().TgtPtr()->HitNucPdg())) { // Check that it isn't coherent! Want to keep initial state if so
      if (state == kInitialState)
        state = kNuclearInitial;
      else if (state == kFinalState)
        state = kNuclearRemnant;
    }
  }

  return state;
}

int GENIEInputHandler::ConvertGENIEReactionCode(GHepRecord *gheprec) {

  // I randomly picked 53 here because NEUT doesn't have an appropriate mode...
  if (gheprec->Summary()->ProcInfo().IsNuElectronElastic()){
    if (pdg::IsNeutrino(gheprec->Summary()->InitState().ProbePdg())) return InputHandler::kNuElectronElastic;
    else return -InputHandler::kNuElectronElastic;
  }

  // And the same story for 54
  if (gheprec->Summary()->ProcInfo().IsIMDAnnihilation() || gheprec->Summary()->ProcInfo().IsInverseMuDecay()){
    if (pdg::IsNeutrino(gheprec->Summary()->InitState().ProbePdg())) return InputHandler::kInvMuonDecay;
    else return -InputHandler::kInvMuonDecay;
  }

  // Electron Scattering
  if (gheprec->Summary()->ProcInfo().IsEM()) {
    if (gheprec->Summary()->InitState().ProbePdg() == 11) {
      if (gheprec->Summary()->ProcInfo().IsQuasiElastic())
        return InputHandler::kCCQE;
      else if (gheprec->Summary()->ProcInfo().IsMEC())
        return InputHandler::kCC2p2h;
      else if (gheprec->Summary()->ProcInfo().IsResonant())
        return InputHandler::kCC1piponn;
      else if (gheprec->Summary()->ProcInfo().IsDeepInelastic())
        return InputHandler::kCCDIS;
      else {
        NUIS_ERR(WRN,
            "Unknown GENIE Electron Scattering Mode!"
            << std::endl
            << "ScatteringTypeId = "
            << gheprec->Summary()->ProcInfo().ScatteringTypeId() << " "
            << "InteractionTypeId = "
            << gheprec->Summary()->ProcInfo().InteractionTypeId()
            << std::endl
            << genie::ScatteringType::AsString(
              gheprec->Summary()->ProcInfo().ScatteringTypeId())
            << " "
            << genie::InteractionType::AsString(
              gheprec->Summary()->ProcInfo().InteractionTypeId())
            << " " << gheprec->Summary()->ProcInfo().IsMEC());
        return 0;
      }
    }

    // Weak CC
  } else if (gheprec->Summary()->ProcInfo().IsWeakCC()) {
    // CC MEC
    if (gheprec->Summary()->ProcInfo().IsMEC()) {
      if (pdg::IsNeutrino(gheprec->Summary()->InitState().ProbePdg()))
        return InputHandler::kCC2p2h;
      else if (pdg::IsAntiNeutrino(gheprec->Summary()->InitState().ProbePdg()))
        return -InputHandler::kCC2p2h;
#ifdef GENIE3_API_ENABLED
    } else if (gheprec->Summary()->ProcInfo().IsDiffractive()) {
      if (pdg::IsNeutrino(gheprec->Summary()->InitState().ProbePdg()))
        return InputHandler::kCCDiffSPP;
      else if (pdg::IsAntiNeutrino(gheprec->Summary()->InitState().ProbePdg()))
        return -InputHandler::kCCDiffSPP;
#endif
      // CC OTHER
    } else {
      return utils::ghep::NeutReactionCode(gheprec);
    }

    // Weak NC
  } else if (gheprec->Summary()->ProcInfo().IsWeakNC()) {
    // NC MEC
    if (gheprec->Summary()->ProcInfo().IsMEC()) {
      if (pdg::IsNeutrino(gheprec->Summary()->InitState().ProbePdg()))
        return InputHandler::kNC2p2h;
      else if (pdg::IsAntiNeutrino(gheprec->Summary()->InitState().ProbePdg()))
        return -InputHandler::kNC2p2h;
#ifdef GENIE3_API_ENABLED
    } else if (gheprec->Summary()->ProcInfo().IsDiffractive()) {
      if (pdg::IsNeutrino(gheprec->Summary()->InitState().ProbePdg()))
        return InputHandler::kNCDiffSPP;
      else if (pdg::IsAntiNeutrino(gheprec->Summary()->InitState().ProbePdg()))
        return -InputHandler::kNCDiffSPP;
#endif
      // NC OTHER
    } else {
      return utils::ghep::NeutReactionCode(gheprec);
    }
  }

  return 0;
}

void GENIEInputHandler::CalcNUISANCEKinematics() {
  // Reset all variables
  fNUISANCEEvent->ResetEvent();

  // Check for GENIE Event
  if (!fGenieNtpl)
    return;
  if (!fGenieNtpl->event)
    return;

  // Cast Event Record
  fGenieGHep = static_cast<GHepRecord *>(fGenieNtpl->event);
  if (!fGenieGHep)
    return;

  // Convert GENIE Reaction Code
  fNUISANCEEvent->Mode = ConvertGENIEReactionCode(fGenieGHep);

  // Get the resonance ID Code
  fNUISANCEEvent->fResCode = fGenieGHep->Summary()->ExclTagPtr()->Resonance();


  if (!fNUISANCEEvent->Mode) {
    NUIS_ERR(WRN, "Failed to determine mode for GENIE event: ");
    std::cout << *fGenieGHep << std::endl;
  }

  // Set Event Info
  fNUISANCEEvent->fEventNo = 0.0;
  fNUISANCEEvent->fTotCrs = fGenieGHep->XSec();

  // Have a bool storing if interaction happened on free or bound nucleon
  bool IsFree = false;

  // Set the TargetPDG
  if (fGenieGHep->TargetNucleus() != NULL) {
    fNUISANCEEvent->fTargetPDG = fGenieGHep->TargetNucleus()->Pdg();
    IsFree = false;
    // Sometimes GENIE scatters off free nucleons, electrons, photons
    // In which TargetNucleus is NULL and we need to find the initial state
    // particle
  } else {
    // Check the particle is an initial state particle
    // Follows GHepRecord::TargetNucleusPosition but doesn't do check on
    // pdg::IsIon
    GHepParticle *p = fGenieGHep->Particle(1);
    // Check that particle 1 actually exists
    if (!p) {
      NUIS_ABORT("Can't find particle 1 for GHepRecord");
    }
    // If not an ion but is an initial state particle
    if (!pdg::IsIon(p->Pdg()) && p->Status() == kIStInitialState) {
      IsFree = true;
      fNUISANCEEvent->fTargetPDG = p->Pdg();
      // Catch if something strange happens:
      // Here particle 1 is not an initial state particle OR
      // particle 1 is an ion OR
      // both
    } else {
      if (pdg::IsIon(p->Pdg())) {
        NUIS_ABORT(
            "Particle 1 in GHepRecord stack is an ion but isn't an initial "
            "state particle");
      } else {
        NUIS_ABORT(
            "Particle 1 in GHepRecord stack is not an ion but is an initial "
            "state particle");
      }
    }
  }
  // Set the A and Z and H from the target PDG
  // Depends on if we scattered off a free or bound nucleon
  if (!IsFree) {
    fNUISANCEEvent->fTargetA =
      TargetUtils::GetTargetAFromPDG(fNUISANCEEvent->fTargetPDG);
    fNUISANCEEvent->fTargetZ =
      TargetUtils::GetTargetZFromPDG(fNUISANCEEvent->fTargetPDG);
    fNUISANCEEvent->fTargetH = 0;
  } else {
    // If free proton scattering
    if (fNUISANCEEvent->fTargetPDG == 2212) {
      fNUISANCEEvent->fTargetA = 1;
      fNUISANCEEvent->fTargetZ = 1;
      fNUISANCEEvent->fTargetH = 1;
      // If free neutron scattering
    } else if (fNUISANCEEvent->fTargetPDG == 2112) {
      fNUISANCEEvent->fTargetA = 0;
      fNUISANCEEvent->fTargetZ = 1;
      fNUISANCEEvent->fTargetH = 0;
      // If neither
    } else {
      fNUISANCEEvent->fTargetA = 0;
      fNUISANCEEvent->fTargetZ = 0;
      fNUISANCEEvent->fTargetH = 0;
    }
  }
  fNUISANCEEvent->fBound = !IsFree;
  fNUISANCEEvent->InputWeight =
    1.0; //(1E+38 / genie::units::cm2) * fGenieGHep->XSec();

  // Get N Particle Stack
  unsigned int npart = fGenieGHep->GetEntries();
  unsigned int kmax = fNUISANCEEvent->kMaxParticles;
  if (npart > kmax) {
    NUIS_ERR(WRN, "GENIE has too many particles, expanding stack.");
    fNUISANCEEvent->ExpandParticleStack(npart);
  }

  // Fill Particle Stack
  GHepParticle *p = 0;
  TObjArrayIter iter(fGenieGHep);
  fNUISANCEEvent->fNParticles = 0;

  // Loop over all particles
  while ((p = (dynamic_cast<genie::GHepParticle *>((iter).Next())))) {
    if (!p)
      continue;

    // Get Status
    int state = GetGENIEParticleStatus(p, fNUISANCEEvent->Mode);

    // Remove Undefined
    if (kRemoveUndefParticles && state == kUndefinedState)
      continue;

    // Remove FSI
    if (kRemoveFSIParticles && state == kFSIState)
      continue;

    if (kRemoveNuclearParticles &&
        (state == kNuclearInitial || state == kNuclearRemnant))
      continue;

    // Fill Vectors
    int curpart = fNUISANCEEvent->fNParticles;
    fNUISANCEEvent->fParticleState[curpart] = state;

    // Mom
    fNUISANCEEvent->fParticleMom[curpart][0] = p->Px() * 1.E3;
    fNUISANCEEvent->fParticleMom[curpart][1] = p->Py() * 1.E3;
    fNUISANCEEvent->fParticleMom[curpart][2] = p->Pz() * 1.E3;
    fNUISANCEEvent->fParticleMom[curpart][3] = p->E() * 1.E3;

    // PDG
    fNUISANCEEvent->fParticlePDG[curpart] = p->Pdg();

    // Set if the particle was on the fundamental vertex
    fNUISANCEEvent->fPrimaryVertex[curpart] = IsPrimary(p);

    // Add to N particle count
    fNUISANCEEvent->fNParticles++;

    // Extra Check incase GENIE fails.
    if ((UInt_t)fNUISANCEEvent->fNParticles == kmax) {
      NUIS_ERR(WRN, "Number of GENIE Particles exceeds maximum!");
      NUIS_ERR(WRN, "Extend kMax, or run without including FSI particles!");
      break;
    }
  }

  // Fill Extra Stack
  if (fSaveExtra)
    fGenieInfo->FillGeneratorInfo(fGenieNtpl);

  // Run Initial, FSI, Final, Other ordering.
  fNUISANCEEvent->OrderStack();

  FitParticle *ISAnyLepton = fNUISANCEEvent->GetHMISAnyLeptons();
  if (ISAnyLepton) {
    fNUISANCEEvent->probe_E = ISAnyLepton->E();
    fNUISANCEEvent->probe_pdg = ISAnyLepton->PDG();
  }
  return;
}

void GENIEInputHandler::Print() {}
