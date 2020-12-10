#ifdef __NUWRO_ENABLED__
#include "NuWroInputHandler.h"
#include "InputUtils.h"

NuWroGeneratorInfo::~NuWroGeneratorInfo() { delete fNuWroParticlePDGs; }

void NuWroGeneratorInfo::AddBranchesToTree(TTree *tn) {
  tn->Branch("NuWroParticlePDGs", &fNuWroParticlePDGs, "NuWroParticlePDGs/I");
}

void NuWroGeneratorInfo::SetBranchesFromTree(TTree *tn) {
  tn->SetBranchAddress("NuWroParticlePDGs", &fNuWroParticlePDGs);
}

void NuWroGeneratorInfo::AllocateParticleStack(int stacksize) {
  fNuWroParticlePDGs = new int[stacksize];
}

void NuWroGeneratorInfo::DeallocateParticleStack() {
  delete fNuWroParticlePDGs;
}

void NuWroGeneratorInfo::FillGeneratorInfo(event *e) { Reset(); }

void NuWroGeneratorInfo::Reset() {
  for (int i = 0; i < kMaxParticles; i++) {
    fNuWroParticlePDGs[i] = 0;
  }
}

int event1_nof(event *e, int pdg) {
  int c = 0;
  for (size_t i = 0; i < e->out.size(); i++)
    if (e->out[i].pdg == pdg)
      c++;
  return c;
}

NuWroInputHandler::NuWroInputHandler(std::string const &handle,
                                     std::string const &rawinputs) {
  NUIS_LOG(SAM, "Creating NuWroInputHandler : " << handle);

  // Run a joint input handling
  fName = handle;
  fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");
  fSaveExtra = false; // FitPar::Config().GetParB("NuWroSaveExtra");
  // Setup the TChain
  fNuWroTree = new TChain("treeout");

  // Loop over all inputs and grab flux, eventhist, and nevents
  std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
  for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
    // Open File for histogram access
    TFile *inp_file = new TFile(inputs[inp_it].c_str(), "READ");
    if (!inp_file or inp_file->IsZombie()) {
      NUIS_ABORT("nuwro File IsZombie() at " << inputs[inp_it]);
    }

    // Get Flux/Event hist
    TH1D *fluxhist = (TH1D *)inp_file->Get(
        (PlotUtils::GetObjectWithName(inp_file, "FluxHist")).c_str());
    TH1D *eventhist = (TH1D *)inp_file->Get(
        (PlotUtils::GetObjectWithName(inp_file, "EvtHist")).c_str());
    if (!fluxhist or !eventhist) {
      NUIS_ERR(FTL, "nuwro FILE doesn't contain flux/xsec info");
      if (FitPar::Config().GetParB("regennuwro")) {
        NUIS_ERR(FTL,
                 "Regen NuWro has not been added yet. Email the developers!");
        // ProcessNuWroInputFlux(inputs[inp_it]);
        throw;
      } else {
        NUIS_ABORT("If you would like NUISANCE to generate these for you "
                   << "please set parameter regennuwro=1 and re-run.");
      }
    }

    // Get N Events
    TTree *nuwrotree = (TTree *)inp_file->Get("treeout");
    if (!nuwrotree) {
      NUIS_ABORT("treeout not located in nuwro file! " << inputs[inp_it]);
    }
    int nevents = nuwrotree->GetEntries();

    // Register input to form flux/event rate hists
    RegisterJointInput(inputs[inp_it], nevents, fluxhist, eventhist);

    // Add to TChain
    fNuWroTree->Add(inputs[inp_it].c_str());
  }

  // Registor all our file inputs
  SetupJointInputs();

  // Setup Events
  fNuWroEvent = NULL;
  fNuWroTree->SetBranchAddress("e", &fNuWroEvent);
  fNuWroTree->GetEntry(0);

  fNUISANCEEvent = new FitEvent();
  fNUISANCEEvent->fType = kNUWRO;
  fNUISANCEEvent->fNuwroEvent = fNuWroEvent;

  fNUISANCEEvent->HardReset();

  if (fSaveExtra) {
    fNuWroInfo = new NuWroGeneratorInfo();
    fNUISANCEEvent->AddGeneratorInfo(fNuWroInfo);
  }
};

NuWroInputHandler::~NuWroInputHandler() {
  if (fNuWroTree)
    delete fNuWroTree;
}

void NuWroInputHandler::CreateCache() {
  // fNuWroTree->SetCacheEntryRange(0, fNEvents);
  //    fNuWroTree->AddBranchToCache("*", 1);
  //    fNuWroTree->SetCacheSize(fCacheSize);
}

void NuWroInputHandler::RemoveCache() {
  // fNuWroTree->SetCacheEntryRange(0, fNEvents);
  //    fNuWroTree->AddBranchToCache("*", 0);
  //    fNuWroTree->SetCacheSize(0);
}

void NuWroInputHandler::ProcessNuWroInputFlux(const std::string file) {}

FitEvent *NuWroInputHandler::GetNuisanceEvent(const UInt_t ent,
                                              const bool lightweight) {
  UInt_t entry = ent + fSkip;
  // Catch too large entries
  if (entry >= (UInt_t)fNEvents)
    return NULL;

  // Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
  fNuWroTree->GetEntry(entry);

  // Run NUISANCE Vector Filler
  if (!lightweight) {
    CalcNUISANCEKinematics();
  }
#ifdef __PROB3PP_ENABLED__
  for (size_t i = 0; i < fNUISANCEEvent->fNuwroEvent->in.size(); i++) {
    if (std::count(PhysConst::pdg_neutrinos, PhysConst::pdg_neutrinos + 4,
                   fNUISANCEEvent->fNuwroEvent->in[i].pdg)) {
      fNUISANCEEvent->probe_E = fNUISANCEEvent->fNuwroEvent->in[i].t;
      fNUISANCEEvent->probe_pdg = fNUISANCEEvent->fNuwroEvent->in[i].pdg;
      break;
    }
  }
#endif
  // Setup Input scaling for joint inputs
  fNUISANCEEvent->InputWeight = GetInputWeight(entry);

#ifdef __USE_NUWRO_SRW_EVENTS__
  if (!rwEvs.size()) {
    fNuwroParams = fNuWroEvent->par;
  }

  if (entry >= rwEvs.size()) {
    rwEvs.push_back(BaseFitEvt());
    rwEvs.back().fType = kNUWRO;
    rwEvs.back().Mode = fNUISANCEEvent->Mode;
    rwEvs.back().fNuwroSRWEvent = SRW::SRWEvent(*fNuWroEvent);
    rwEvs.back().fNuwroEvent = NULL;
    rwEvs.back().fNuwroParams = &fNuwroParams;
    rwEvs.back().probe_E = rwEvs.back().fNuwroSRWEvent.NeutrinoEnergy;
    rwEvs.back().probe_pdg = rwEvs.back().fNuwroSRWEvent.NeutrinoPDG;
  }

  fNUISANCEEvent->fNuwroSRWEvent = SRW::SRWEvent(*fNuWroEvent);
  fNUISANCEEvent->fNuwroParams = &fNuwroParams;
  fNUISANCEEvent->probe_E = fNUISANCEEvent->fNuwroSRWEvent.NeutrinoEnergy;
  fNUISANCEEvent->probe_pdg = fNUISANCEEvent->fNuwroSRWEvent.NeutrinoPDG;
#endif

  return fNUISANCEEvent;
}

int NuWroInputHandler::ConvertNuwroMode(event *e) {
  Int_t proton_pdg, neutron_pdg, pion_pdg, pion_plus_pdg, pion_minus_pdg,
      lambda_pdg, eta_pdg, kaon_pdg, kaon_plus_pdg;
  proton_pdg = 2212;
  eta_pdg = 221;
  neutron_pdg = 2112;
  pion_pdg = 111;
  pion_plus_pdg = 211;
  pion_minus_pdg = -211;
  lambda_pdg = 3122;
  kaon_pdg = 311;
  kaon_plus_pdg = 321;

  // Antineutrino modes are *-1
  int nu_nubar = 1;
  if (e->flag.anty) nu_nubar = -1;

  // Quasielastic
  if (e->flag.qel){
    if (e->flag.cc)
      return 1*nu_nubar;
    else {
      if (event1_nof(e, proton_pdg))
	return 51*nu_nubar;
      else if (event1_nof(e, neutron_pdg))
	return 52*nu_nubar;
    }
  }

  if (e->flag.mec) {
    return 2*nu_nubar;
  }

  // Pion production
  if (e->flag.res) {

    int npions = event1_nof(e, pion_pdg) + event1_nof(e, pion_plus_pdg) +
      event1_nof(e, pion_minus_pdg);
    int nkaons= event1_nof(e, kaon_pdg) + event1_nof(e, kaon_pdg);

    // Multipion?
    if (npions > 1 || npions == 0) {
      if (e->flag.cc)
	return 21*nu_nubar;
      else
	return 41*nu_nubar;
    }

    if (npions == 1) {
      if (e->flag.cc) {
	// CC
	if (e->flag.anty){
	  // Antineutrino
	  if (event1_nof(e, neutron_pdg) && event1_nof(e, pion_minus_pdg))
            return -11;
          if (event1_nof(e, neutron_pdg) && event1_nof(e, pion_pdg))
            return -12;
          if (event1_nof(e, proton_pdg) && event1_nof(e, pion_minus_pdg))
            return -13;
	} else {
	  // Neutrino
          if (event1_nof(e, proton_pdg) && event1_nof(e, pion_plus_pdg))
            return 11;
          if (event1_nof(e, proton_pdg) && event1_nof(e, pion_pdg))
            return 12;
          if (event1_nof(e, neutron_pdg) && event1_nof(e, pion_plus_pdg))
            return 13;	  
	}
      } else {
	// Now NC
	if (event1_nof(e, proton_pdg)) {
	  if (event1_nof(e, pion_minus_pdg))
	    return 33*nu_nubar;
	  else if (event1_nof(e, pion_pdg))
	    return 32*nu_nubar;
	} else if (event1_nof(e, neutron_pdg)) {
	  if (event1_nof(e, pion_plus_pdg))
	    return 34*nu_nubar;
	  else if (event1_nof(e, pion_pdg))
	    return 31*nu_nubar;
	}
      }
    }
    
    // Eta production
    if (event1_nof(e, eta_pdg)) {
      if (e->flag.cc)
	return 22*nu_nubar;
      else {
	if (event1_nof(e, neutron_pdg))
	  return 42*nu_nubar;
	else if (event1_nof(e, proton_pdg))
	  return 43*nu_nubar;
      }
    }

    // Resonant kaon production
    if (event1_nof(e, lambda_pdg) == 1 && nkaons == 1) {
      if (e->flag.cc && event1_nof(e, kaon_pdg))
	return 23*nu_nubar;
      else {
	if (event1_nof(e, kaon_pdg))
	  return 44*nu_nubar;
	else if (event1_nof(e, kaon_plus_pdg))
	  return 45*nu_nubar;
      }
    }
  }

  // Coherent
  if (e->flag.coh) {
    if (e->flag.cc && (event1_nof(e, pion_minus_pdg) + 
		       event1_nof(e, pion_plus_pdg)))
      return 16*nu_nubar;
    else if (event1_nof(e, pion_pdg))
      return 36*nu_nubar;
  }

  // DIS
  if (e->flag.dis) {
    if (e->flag.cc)
      return 26*nu_nubar;
    else
      return 46*nu_nubar;
  } 

  // If we got here, something is wrong, see what happened...
  NUIS_ERR(WRN, "Unable to interpret NUWRO event, dumping info...");
  Print();

  return 9999;
}

void NuWroInputHandler::CalcNUISANCEKinematics() {
  // std::cout << "NuWro Event Address " << fNuWroEvent << std::endl;
  // Reset all variables
  fNUISANCEEvent->ResetEvent();
  FitEvent *evt = fNUISANCEEvent;

  // Sort Event Info
  evt->Mode = ConvertNuwroMode(fNuWroEvent);

  if (abs(evt->Mode) > 60) {
    evt->Mode = 0;
  }

  evt->fEventNo = 0.0;
  evt->fTotCrs = 0.0;
  evt->fTargetA = fNuWroEvent->par.nucleus_p + fNuWroEvent->par.nucleus_n;
  evt->fTargetZ = fNuWroEvent->par.nucleus_p;
  evt->fTargetPDG =
      TargetUtils::GetTargetPDGFromZA(evt->fTargetZ, evt->fTargetA);
  evt->fTargetH = 0;
  evt->fBound = (evt->fTargetA != 1);

  // Check Particle Stack
  UInt_t npart_in = fNuWroEvent->in.size();
  UInt_t npart_out = fNuWroEvent->out.size();
  UInt_t npart_post = fNuWroEvent->post.size();
  UInt_t npart = npart_in + npart_out + npart_post;
  UInt_t kmax = evt->kMaxParticles;

  if (npart > kmax) {
    NUIS_ERR(WRN, "NUWRO has too many particles. Expanding stack.");
    fNUISANCEEvent->ExpandParticleStack(npart);
  }

  evt->fNParticles = 0;
  std::vector<particle>::iterator p_iter;

  // Get the Initial State
  for (p_iter = fNuWroEvent->in.begin(); p_iter != fNuWroEvent->in.end();
       p_iter++) {
    AddNuWroParticle(fNUISANCEEvent, (*p_iter), kInitialState, true);
  }

  // Try to find the FSI state particles
  // Loop over the primary vertex particles
  // If they match the post-FSI they haven't undergone FSI.
  // If they don't match post-FSI they have undergone FSI.
  for (p_iter = fNuWroEvent->out.begin(); p_iter != fNuWroEvent->out.end();
       p_iter++) {
    // Get the particle
    particle p = (*p_iter);
    // Check against all the post particles, match them
    std::vector<particle>::iterator p2_iter;
    bool match = false;
    for (p2_iter = fNuWroEvent->post.begin();
         p2_iter != fNuWroEvent->post.end(); p2_iter++) {
      particle p2 = (*p2_iter);
      // Check energy and pdg
      // A very small cascade which changes the energy by 1E-5 MeV should be
      // matched
      match = (fabs(p2.E() - p.E()) < 1E-5 && p2.pdg == p.pdg);
      // If we match p to p2 break the loop
      if (match)
        break;
    }
    // If we've looped through the whole particle stack of post-FSI and haven't
    // found a match it's a primary particle that has been FSIed
    if (!match)
      AddNuWroParticle(fNUISANCEEvent, (*p_iter), kFSIState, true);
  }

  // Loop over the final state particles
  for (p_iter = fNuWroEvent->post.begin(); p_iter != fNuWroEvent->post.end();
       p_iter++) {
    particle p = (*p_iter);
    // To find if it's primary or not we have to loop through the primary ones
    // and match, just like above
    bool match = false;
    std::vector<particle>::iterator p2_iter;
    for (p2_iter = fNuWroEvent->out.begin(); p2_iter != fNuWroEvent->out.end();
         p2_iter++) {
      particle p2 = (*p2_iter);
      match = (fabs(p2.E() - p.E()) < 1E-5 && p2.pdg == p.pdg);
      if (match)
        break;
    }
    AddNuWroParticle(fNUISANCEEvent, (*p_iter), kFinalState, match);
  }

  // Fill Generator Info
  if (fSaveExtra)
    fNuWroInfo->FillGeneratorInfo(fNuWroEvent);

  // Run Initial, FSI, Final, Other ordering.
  fNUISANCEEvent->OrderStack();

  FitParticle *ISAnyLepton = fNUISANCEEvent->GetHMISAnyLeptons();
  if (ISAnyLepton) {
    fNUISANCEEvent->probe_E = ISAnyLepton->E();
    fNUISANCEEvent->probe_pdg = ISAnyLepton->PDG();
  }

  return;
}

void NuWroInputHandler::AddNuWroParticle(FitEvent *evt, particle &p, int state,
                                         bool primary = false) {
  // Add Mom
  evt->fParticleMom[evt->fNParticles][0] = static_cast<vect &>(p).x;
  evt->fParticleMom[evt->fNParticles][1] = static_cast<vect &>(p).y;
  evt->fParticleMom[evt->fNParticles][2] = static_cast<vect &>(p).z;
  evt->fParticleMom[evt->fNParticles][3] = static_cast<vect &>(p).t;

  // For NuWro a particle that we've given a FSI state is a pre-FSI particle
  // An initial state particle is also a primary vertex praticle
  evt->fPrimaryVertex[evt->fNParticles] = primary;

  // Status/PDG
  evt->fParticleState[evt->fNParticles] = state;
  evt->fParticlePDG[evt->fNParticles] = p.pdg;

  // Add to particle count
  evt->fNParticles++;
}

void NuWroInputHandler::Print(){
  NUIS_LOG(EVT, "NuWro event information:" << std::endl
	   << "\t\t|->      dyn = " << fNuWroEvent->dyn << std::endl
	   << "\t\t|->       qel = " << fNuWroEvent->flag.qel << std::endl
	   << "\t\t|->       res = " << fNuWroEvent->flag.res << std::endl
	   << "\t\t|->       dis = " << fNuWroEvent->flag.dis << std::endl
	   << "\t\t|->       coh = " << fNuWroEvent->flag.coh << std::endl
	   << "\t\t|->       mec = " << fNuWroEvent->flag.mec << std::endl
	   << "\t\t|->       hip = " << fNuWroEvent->flag.hip << std::endl
	   << "\t\t|->        nc = " << fNuWroEvent->flag.nc << std::endl
	   << "\t\t|->        cc = " << fNuWroEvent->flag.cc << std::endl
	   << "\t\t|->      anty = " << fNuWroEvent->flag.anty << std::endl
	   << "\t\t|-> res_delta = " << fNuWroEvent->flag.res_delta);
}

#endif
