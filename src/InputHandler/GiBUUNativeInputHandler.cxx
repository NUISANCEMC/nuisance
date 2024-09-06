#ifdef GiBUU_ENABLED
#include "GiBUUNativeInputHandler.h"
#include "InputUtils.h"
#include "PhysConst.h"

bool GiBUUEventReader::SetBranchAddresses(TChain* tn){

  tn->SetBranchAddress("weight", &weight);
  tn->SetBranchAddress("evType", &mode);
  tn->SetBranchAddress("process_ID", &process_ID);
  tn->SetBranchAddress("flavor_ID", &flavor_ID);
  tn->SetBranchAddress("numEnsembles", &num_ensembles);
  tn->SetBranchAddress("numRuns", &num_runs);
  tn->SetBranchAddress("nucleus_A", &nucleus_A);
  tn->SetBranchAddress("nucleus_Z", &nucleus_Z);

  tn->SetBranchAddress("barcode", &pdg, &b_pdg);
  tn->SetBranchAddress("Px", &Px, &b_Px);
  tn->SetBranchAddress("Py", &Py, &b_Py);
  tn->SetBranchAddress("Pz", &Pz, &b_Pz);
  tn->SetBranchAddress("E", &E, &b_E);
  
  tn->SetBranchAddress("x", &x, &b_x);
  tn->SetBranchAddress("y", &y, &b_y);
  tn->SetBranchAddress("z", &z, &b_z);

  tn->SetBranchAddress("lepIn_Px", &lepIn_Px);
  tn->SetBranchAddress("lepIn_Py", &lepIn_Py);
  tn->SetBranchAddress("lepIn_Pz", &lepIn_Pz);
  tn->SetBranchAddress("lepIn_E", &lepIn_E);

  tn->SetBranchAddress("lepOut_Px", &lepOut_Px);
  tn->SetBranchAddress("lepOut_Py", &lepOut_Py);
  tn->SetBranchAddress("lepOut_Pz", &lepOut_Pz);
  tn->SetBranchAddress("lepOut_E", &lepOut_E);

  tn->SetBranchAddress("nuc_Px", &nuc_Px);
  tn->SetBranchAddress("nuc_Py", &nuc_Py);
  tn->SetBranchAddress("nuc_Pz", &nuc_Pz);
  tn->SetBranchAddress("nuc_E", &nuc_E);
  tn->SetBranchAddress("nuc_charge", &nuc_chrg);
  
  return 0;
}


int ConvertModeGiBUUtoNEUT(int gibuu_mode, int process_ID, int struck_nucleon_pdg, int first_part_pdg){

  bool IsCC = (abs(process_ID) == 2) ? true : false;
  bool IsNu = (process_ID > 0) ? true : false;

  switch(gibuu_mode){
  // QE/elastic
  case 1:
    if (IsCC) {
      return (IsNu ? 1 : -1);
    } else {
      return (IsNu ? 1 : -1) * ((struck_nucleon_pdg == 2212) ? 51 : 52);
    }
  // Different resonances
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
  case 15:
  case 16:
  case 17:
  case 18:
  case 19:
  case 20:
  case 21:
  case 22:
  case 23:
  case 24:
  case 25:
  case 26:
  case 27:
  case 28:
  case 29:
  case 30:
  case 31: {
    if (IsCC){
      if (IsNu){
	if (struck_nucleon_pdg == 2212) return 11;
	if (first_part_pdg == 111) return 12;
	return 13;
      } else {
	if (struck_nucleon_pdg == 2112) return -11;
	if (first_part_pdg == 111) return -12;
	return -13;
      }
    } else { 
      if (struck_nucleon_pdg == 2212) {
	if (first_part_pdg == 111) return 32 * (IsNu ? 1 : -1);
	return 34 * (IsNu ? 1 : -1);
      } else {
	if (first_part_pdg == 111) return 31 * (IsNu ? 1 : -1);
	return 33 * (IsNu ? 1 : -1);
      }
    }
  }
  case 32:
  case 33: { // 1Pi Bkg
    return (IsNu ? 1 : -1) * (10 + 20 * (!IsCC));
  }
  case 34: { // DIS
    return (IsNu ? 1 : -1) * (26 + 20 * (!IsCC));
  }
  case 35:
  case 36: { // MEC/2p-2h
    return (IsNu ? 1 : -1) * (2 + 40 * (!IsCC));
  }
  case 37: { // MultiPi
    return (IsNu ? 1 : -1) * (21 + 20 * (!IsCC));
  }
  default:
    NUIS_ERR(WRN, "Unable to map GiBUU code " << gibuu_mode << " to a NEUT mode");
  }    
  return 0;
}

int GetGiBUUNuPDG(int flavor_ID, int process_ID){

  int pdg = 0;
  switch(flavor_ID){
  case 1:
    pdg = 12;
    break;
  case 2:
    pdg = 14;
    break;
  case 3:
    pdg = 16;
    break;
  }

  if (process_ID < 0) pdg*=-1;
  return pdg;
}


int GetGiBUULepPDG(int flavor_ID, int process_ID){

  int nuPDG = GetGiBUUNuPDG(flavor_ID, process_ID);

  // Check for EM
  if (abs(process_ID) == 1){
    NUIS_ABORT("GiBUU file includes electron scattering events, not currently supported!");
  } else if (abs(process_ID) == 3){
    return nuPDG;
  } else if (abs(process_ID) == 2){
    return (nuPDG > 0) ? nuPDG - 1 : nuPDG + 1;
  }
  NUIS_ABORT("Unknown GiBUU process_ID " << process_ID);
}


// Check whether the particle is on-shell or not
int CheckGiBUUParticleStatus(double E, int pdg, double dist, int targetA){

  double onshell_mass = PhysConst::GetMass(pdg);

  // Hard code some other values used in GiBUU...
  if (pdg == 2212 || pdg == 2112) onshell_mass = 0.938;
  if (abs(pdg) == 211 || pdg == 111) onshell_mass = 0.138;

  // If the particle is still within the nucleus, it's not final state
  // Not that this depends on the nucleus, and too few time steps could cause an issue
  // 6 fm is Ulrich's guess for Argon (and will be fine for smaller nuclei)
  // Note that hydrogen is an exception because of how the timesteps work
  if (dist < 6 && targetA > 1) {
    return kFSIState;
  }

  // Check for unknown particles and default to on shell
  if (onshell_mass == -1) return kFinalState;
  
  // Ulrich's second criteria
  if (E < onshell_mass) return kFSIState;

  return kFinalState;
}


GiBUUNativeInputHandler::~GiBUUNativeInputHandler() {
  jointtype     .clear();
  jointrequested.clear();
}


GiBUUNativeInputHandler::GiBUUNativeInputHandler(std::string const &handle,
						 std::string const &rawinputs) {

  NUIS_LOG(SAM, "Creating GiBUUNativeInputHandler : " << handle);

  // Run a joint input handling
  fName = handle;
  fEventType = kGiBUU;
  fGiBUUTree = new TChain("RootTuple");

  // Loop over all inputs and grab flux, eventhist, and nevents
  std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
  for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
    // Open File for histogram access
    NUIS_LOG(SAM, "Opening event file " << inputs[inp_it]);
    TFile *inp_file = new TFile(inputs[inp_it].c_str(), "READ");
    if ((!inp_file) || (!inp_file->IsOpen())) {
      NUIS_ABORT(
          "GiBUU file !IsOpen() at : '"
          << inputs[inp_it] << "'" << std::endl
          << "Check that your file paths are correct and the file exists!");
    }

    // Get Flux/Event hist
    TH1D *fluxhist  = (TH1D*)inp_file->Get(PlotUtils::GetObjectWithName(inp_file, "flux").c_str());
    TH1D *eventhist = (TH1D*)inp_file->Get(PlotUtils::GetObjectWithName(inp_file, "evtrt").c_str());
    if (!fluxhist || !eventhist) {
      NUIS_ERR(FTL, "Input File Contents: " << inputs[inp_it]);
      inp_file->ls();
      NUIS_ABORT("GiBUU file doesn't contain flux/xsec info. You may have to "
                 "use PrepareGiBUU!");
    }

    // Get N Events
    TChain *tempChain = new TChain("RootTuple");
    tempChain->Add(inputs[inp_it].c_str());

    if (!tempChain){
      NUIS_ERR(FTL, "RootTuple tree missing from GiBUU file "
                        << inputs[inp_it]);
      NUIS_ABORT(
          "Check your inputs, they may need to be completely regenerated!");
    }
    int nevents = tempChain->GetEntries();
    if (nevents <= 0) {
      NUIS_ABORT("Trying to a TTree with "
                 << nevents << " to TChain from : " << inputs[inp_it]);
    }

    // Get specific information about the config from the file
    GiBUUEventReader *tempReader = new GiBUUEventReader();
    tempReader->SetBranchAddresses(tempChain);
    tempChain ->GetEntry(0);

    // Register input to form flux/event rate hists
    RegisterJointInput(inputs[inp_it], tempReader->process_ID, tempReader->flavor_ID, 
		       tempReader->nucleus_A, nevents, 
		       tempReader->nucleus_A*tempReader->num_runs*tempReader->num_ensembles, 
		       fluxhist, eventhist);

    // Add To TChain
    fGiBUUTree->AddFile(inputs[inp_it].c_str());

    delete tempChain;
    delete tempReader;
  }

  // Registor all our file inputs
  SetupJointInputs();

  // Create Fit Event
  fNUISANCEEvent = new FitEvent();

  fGiReader = new GiBUUEventReader();
  fGiReader->SetBranchAddresses(fGiBUUTree);

  fNUISANCEEvent->HardReset();
};

FitEvent *GiBUUNativeInputHandler::GetNuisanceEvent(const UInt_t ent,
						    const bool lightweight) {
  UInt_t entry = ent + fSkip;
  // Check out of bounds
  if (entry >= (UInt_t)fNEvents)
    return NULL;
  
  fGiBUUTree->GetEntry(entry);

  fNUISANCEEvent->ResetEvent();
  fNUISANCEEvent->fEventNo = entry;

  // Run NUISANCE Vector Filler
  if (!lightweight) {
    CalcNUISANCEKinematics();
  }
#ifdef Prob3plusplus_ENABLED
  else {
    fNUISANCEEvent->probe_E = fGiReader->lepIn_E*1E3;
    fNUISANCEEvent->probe_pdg = GetGiBUUNuPDG(fGiReader->flavor_ID, fGiReader->process_ID);
  }
#endif

  fNUISANCEEvent->InputWeight *= GetInputWeight(entry);

  return fNUISANCEEvent;
}

void GiBUUNativeInputHandler::CalcNUISANCEKinematics() {
  // Reset all variables
  // fNUISANCEEvent->ResetEvent();
  FitEvent *evt = fNUISANCEEvent;
  evt->Mode = ConvertModeGiBUUtoNEUT(fGiReader->mode, fGiReader->process_ID, 
				     (fGiReader->nuc_chrg) ? 2212 : 2112, 
				     (*fGiReader->pdg)[0]);
  // evt->fEventNo = 0.0;
  evt->fTotCrs = 0;
  evt->fTargetA = fGiReader->nucleus_A;
  evt->fTargetZ = fGiReader->nucleus_Z;
  evt->fTargetH = 0;
  evt->fBound = 0.0;

  // Extra GiBUU Input Weight
  evt->InputWeight = fGiReader->weight;

  // Check number of particles in the stack. Note the additional 3 particles!
  uint npart = fGiReader->pdg->size();
  int kmax = evt->kMaxParticles;
  if ((UInt_t)npart+3 > (UInt_t)kmax) {
    NUIS_ERR(WRN, "GiBUU has too many particles (" << npart << ") Expanding Stack.");
    fNUISANCEEvent->ExpandParticleStack(npart+2);
  }

  // Stuff the leptons in
  evt->fParticleState[0] = kInitialState;
  evt->fParticleMom[0][0] = fGiReader->lepIn_Px * 1E3;
  evt->fParticleMom[0][1] = fGiReader->lepIn_Py * 1E3;
  evt->fParticleMom[0][2] = fGiReader->lepIn_Pz * 1E3;
  evt->fParticleMom[0][3] = fGiReader->lepIn_E * 1E3;
  evt->fParticlePDG[0] = GetGiBUUNuPDG(fGiReader->flavor_ID, fGiReader->process_ID);

  evt->fParticleState[1] = kFinalState;
  evt->fParticleMom[1][0] = fGiReader->lepOut_Px * 1E3;
  evt->fParticleMom[1][1] = fGiReader->lepOut_Py * 1E3;
  evt->fParticleMom[1][2] = fGiReader->lepOut_Pz * 1E3;
  evt->fParticleMom[1][3] = fGiReader->lepOut_E * 1E3;
  evt->fParticlePDG[1] = GetGiBUULepPDG(fGiReader->flavor_ID, fGiReader->process_ID);

  // Also the struck nucleon (note only one!)
  evt->fParticleState[2] = kInitialState;
  evt->fParticleMom[2][0] = fGiReader->nuc_Px * 1E3;
  evt->fParticleMom[2][1] = fGiReader->nuc_Py * 1E3;
  evt->fParticleMom[2][2] = fGiReader->nuc_Pz * 1E3;
  evt->fParticleMom[2][3] = fGiReader->nuc_E * 1E3;
  evt->fParticlePDG[2] = (fGiReader->nuc_chrg) ? 2212 : 2112;

  // Create Stack
  evt->fNParticles = 3;

  for (uint i = 0; i < npart; i++) {

    int curpart = evt->fNParticles;

    double dist = sqrt((*fGiReader->x)[i]*(*fGiReader->x)[i] + (*fGiReader->y)[i]*(*fGiReader->y)[i] + (*fGiReader->z)[i]*(*fGiReader->z)[i]);

    // If the particle is still within the nucleus, it's not final state
    // Not that this depends on the nucleus, and too few time steps could cause an issue
    // 6 fm is Ulrich's guess for Argon (and will be fine for smaller nuclei)
    // Note that hydrogen is an exception because of how the timesteps work
   
    if(dist<6) {
     
	std::cout<<"Dropping Particle in Neucleus"<<std::endl;
	continue;

    }

    // Set State
    evt->fParticleState[curpart] = CheckGiBUUParticleStatus((*fGiReader->E)[i], (*fGiReader->pdg)[i], dist, evt->fTargetA);
    // Mom
    evt->fParticleMom[curpart][0] = (*fGiReader->Px)[i] * 1E3;
    evt->fParticleMom[curpart][1] = (*fGiReader->Py)[i] * 1E3;
    evt->fParticleMom[curpart][2] = (*fGiReader->Pz)[i] * 1E3;
    evt->fParticleMom[curpart][3] = (*fGiReader->E)[i] * 1E3;

    // PDG
    evt->fParticlePDG[curpart] = (*fGiReader->pdg)[i];

    // Add to total particles
    evt->fNParticles++;
  }

  // Run Initial, FSI, Final, Other ordering.
  fNUISANCEEvent->OrderStack();

  FitParticle *ISAnyLepton = fNUISANCEEvent->GetHMISAnyLeptons();
  if (ISAnyLepton) {
    fNUISANCEEvent->probe_E = ISAnyLepton->E();
    fNUISANCEEvent->probe_pdg = ISAnyLepton->PDG();
  }

  return;
}

void GiBUUNativeInputHandler::Print() {}


void GiBUUNativeInputHandler::RegisterJointInput(std::string input, int process_ID, 
						 int flavor_ID, int nnucleons, int n, 
						 int nrequested, TH1D *f, TH1D *e){

  if (jointfluxinputs.size() == 0) {
    jointindexswitch = 0;
    fNEvents = 0;
  }

  // Push into individual input vectors
  jointfluxinputs.push_back((TH1D *)f->Clone());
  jointeventinputs.push_back((TH1D *)e->Clone());

  // This has to correspond to the real number of events
  jointindexlow .push_back(fNEvents);
  jointindexhigh.push_back(fNEvents + n);
  fNEvents += n;

  // This should be nnucleons*num_ensembles*num_runs (the number of requested events)
  jointrequested .push_back(nrequested);

  // To keep track of how many of each type there are
  jointtype .push_back(nnucleons*1000+process_ID*10 + flavor_ID);
  
  // For scaling by nnucleons later
  jointnnucl .push_back(nnucleons);

}

void GiBUUNativeInputHandler::SetupJointInputs() {

  // Always apply the scaling
  jointinput = true;

  if (jointeventinputs.size() > 1) {
    jointindexswitch = 0;
  }
  fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");
  if (fMaxEvents != -1 and jointeventinputs.size() > 1) {
    NUIS_ABORT("Can only handle joint inputs when config MAXEVENTS = -1!");
  }

  // Need to know what the total number of nucleons is for correct normalization...
  int total_unique_nucl = 0;
  std::vector<int> unique_nucl;
  std::vector<int> nsame_vect;

  for (size_t i = 0; i < jointeventinputs.size(); i++) {
    
    // Assume all of the fluxes are the same
    if (!fFluxHist) fFluxHist = (TH1D*)jointfluxinputs[i]->Clone();

    // Get the list of unique nuclei nuclei
    int this_nucl = jointnnucl[i];
    if (std::find(unique_nucl.begin(), unique_nucl.end(), this_nucl) == unique_nucl.end()){
      unique_nucl.push_back(this_nucl);
      total_unique_nucl += this_nucl;
    }

    // Get the total event rate and number of requested events
    TH1D *this_evt_total = NULL;
    int nsame = 0;

    // How many files were set up in the same way?
    for (size_t j = 0; j < jointeventinputs.size(); j++) {
      if (jointtype[j] != jointtype[i]) continue;
      nsame++;
      
      if (!this_evt_total) this_evt_total = (TH1D*)jointeventinputs[j]->Clone();
      else this_evt_total->Add(jointeventinputs[j]);
    }
    nsame_vect.push_back(nsame);

    // Need to average events with the same run config, and add others
    this_evt_total->Scale(jointnnucl[i]/double(nsame));
    if (!fEventHist) fEventHist = (TH1D*)this_evt_total->Clone();
    else fEventHist ->Add(this_evt_total);
    delete this_evt_total;
  }

  // Get the correct / nucleon event rate
  fEventHist->Scale(1/double(total_unique_nucl));

  // Loop over the samples to get the event scaling correct
  for (size_t i = 0; i < jointeventinputs.size(); i++) {

    // This simply reverses the usual scaling to get to a flux averaged XSEC used everywhere else in NUISANCE
    double scale = fFluxHist->Integral("width")*fNEvents/fEventHist->Integral("width");

    // Need to correctly weight by the number of nucleons to arrive at the /nucleon XSEC for a compound target
    scale *= jointnnucl[i]/double(total_unique_nucl)/nsame_vect[i];

    jointindexscale.push_back(scale);
  }

  fEventHist->SetNameTitle((fName + "_EVT").c_str(), (fName + "_EVT").c_str());
  fFluxHist->SetNameTitle((fName + "_FLUX").c_str(), (fName + "_FLUX").c_str());

  // Setup Max Events
  if (fMaxEvents > 1 && fMaxEvents < fNEvents) {
    if (LOG_LEVEL(SAM)) {
      std::cout << "\t\t|-> Read Max Entries : " << fMaxEvents << std::endl;
    }
    fNEvents = fMaxEvents;
  }

  // Print out Status
  if (LOG_LEVEL(SAM)) {
    std::cout << "\t\t|-> Total Entries    : " << fNEvents << std::endl
              << "\t\t|-> Event Integral   : "
              << fEventHist->Integral("width") * 1.E-38 << " events/nucleon"
              << std::endl
              << "\t\t|-> Flux Integral    : " << fFluxHist->Integral("width")
              << " /cm2" << std::endl
              << "\t\t|-> Event/Flux       : "
              << fEventHist->Integral("width") * 1.E-38 /
                     fFluxHist->Integral("width")
              << " cm2/nucleon" << std::endl;
  }
}

#endif
