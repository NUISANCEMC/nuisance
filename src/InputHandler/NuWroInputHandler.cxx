#ifdef __NUWRO_ENABLED__
#include "NuWroInputHandler.h"

NuWroGeneratorInfo::~NuWroGeneratorInfo(){
	delete fNuWroParticlePDGs;
}

void NuWroGeneratorInfo::AddBranchesToTree(TTree* tn) {
	tn->Branch("NuWroParticlePDGs", &fNuWroParticlePDGs, "NuWroParticlePDGs/I");
}

void NuWroGeneratorInfo::SetBranchesFromTree(TTree* tn) {
	tn->SetBranchAddress("NuWroParticlePDGs", &fNuWroParticlePDGs);
}

void NuWroGeneratorInfo::AllocateParticleStack(int stacksize) {
	fNuWroParticlePDGs = new int[stacksize];
}

void NuWroGeneratorInfo::DeallocateParticleStack() {
	delete fNuWroParticlePDGs;
}

void NuWroGeneratorInfo::FillGeneratorInfo(event* e) {
	Reset();
}

void NuWroGeneratorInfo::Reset() {
	for (int i = 0; i < kMaxParticles; i++) {
		fNuWroParticlePDGs[i] = 0;
	}
}

NuWroInputHandler::NuWroInputHandler(std::string const& handle, std::string const& rawinputs) {
	LOG(SAM) << "Creating NuWroInputHandler : " << handle << std::endl;

	// Run a joint input handling
	fName = handle;
	fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");
	fSaveExtra =  false; //FitPar::Config().GetParB("NuWroSaveExtra");
	// Setup the TChain
	fNuWroTree = new TChain("treeout");

	// Loop over all inputs and grab flux, eventhist, and nevents
	std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
	for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {

		// Open File for histogram access
		TFile* inp_file = new TFile(inputs[inp_it].c_str(), "READ");
		if (!inp_file or inp_file->IsZombie()) {
			ERR(FTL) << "nuwro File IsZombie() at " << inputs[inp_it] << std::endl;
			throw;
		}

		// Get Flux/Event hist
		TH1D* fluxhist  = (TH1D*)inp_file->Get(
		                      (PlotUtils::GetObjectWithName(inp_file, "FluxHist")).c_str());
		TH1D* eventhist = (TH1D*)inp_file->Get(
		                      (PlotUtils::GetObjectWithName(inp_file, "EvtHist")).c_str());
		if (!fluxhist or !eventhist) {
			ERR(FTL) << "nuwro FILE doesn't contain flux/xsec info" << std::endl;
			if (FitPar::Config().GetParB("regennuwro")) {
				ERR(FTL) << "Regen NuWro has not been added yet. Email the developers!" << std::endl;
				         // ProcessNuWroInputFlux(inputs[inp_it]);
				throw;
			} else {
				ERR(FTL) << "If you would like NUISANCE to generate these for you "
				         << "please set parameter regennuwro=1 and re-run." << std::endl;
				throw;
			}
		}

		// Get N Events
		TTree* nuwrotree = (TTree*)inp_file->Get("treeout");
		if (!nuwrotree) {
			ERR(FTL) << "treeout not located in nuwro file! " << inputs[inp_it] << std::endl;
			throw;
		}
		int nevents = nuwrotree->GetEntries();

		// Register input to form flux/event rate hists
		RegisterJointInput(inputs[inp_it], nevents, fluxhist, eventhist);

		// Add to TChain
		fNuWroTree->Add( inputs[inp_it].c_str() );
	}

	// Registor all our file inputs
	SetupJointInputs();

	// Setup Events
	fNuWroEvent = NULL;
	fNuWroTree->SetBranchAddress("e", &fNuWroEvent);
	fNuWroTree->GetEntry(0);

	fNUISANCEEvent = new FitEvent();
	fNUISANCEEvent->SetNuwroEvent(fNuWroEvent);
	fNUISANCEEvent->HardReset();

	if (fSaveExtra) {
		fNuWroInfo = new NuWroGeneratorInfo();
		fNUISANCEEvent->AddGeneratorInfo(fNuWroInfo);
	}

};

NuWroInputHandler::~NuWroInputHandler(){
	if (fNuWroTree) delete fNuWroTree;
}

void NuWroInputHandler::CreateCache() {
	// fNuWroTree->SetCacheEntryRange(0, fNEvents);
  //	fNuWroTree->AddBranchToCache("*", 1);
  //	fNuWroTree->SetCacheSize(fCacheSize);
}

void NuWroInputHandler::RemoveCache() {
	// fNuWroTree->SetCacheEntryRange(0, fNEvents);
  //	fNuWroTree->AddBranchToCache("*", 0);
  //	fNuWroTree->SetCacheSize(0);
}

void NuWroInputHandler::ProcessNuWroInputFlux(const std::string file) {
}

FitEvent* NuWroInputHandler::GetNuisanceEvent(const UInt_t entry, const bool lightweight) {

	// Catch too large entries
	if (entry >= (UInt_t)fNEvents) return NULL;

	// Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
	fNuWroTree->GetEntry(entry);

	// Setup Input scaling for joint inputs
	fNUISANCEEvent->InputWeight = GetInputWeight(entry);

	// Run NUISANCE Vector Filler
	if (!lightweight) {
		CalcNUISANCEKinematics();
	}
	
	return fNUISANCEEvent;
}

int NuWroInputHandler::ConvertNuwroMode (event * e) {

	Int_t proton_pdg, neutron_pdg, pion_pdg, pion_plus_pdg, pion_minus_pdg,
	      lambda_pdg, eta_pdg, kaon_pdg, kaon_plus_pdg;
	proton_pdg = 2212;
	eta_pdg = 221;
	neutron_pdg = 2112;
	pion_pdg = 111;
	pion_plus_pdg = 211;
	pion_minus_pdg = -211;
	//O_16_pdg = 100069;   // oznacznie z Neuta
	lambda_pdg = 3122;
	kaon_pdg = 311;
	kaon_plus_pdg = 321;


	if (e->flag.qel)		// kwiazielastyczne oddziaływanie
	{
		if (e->flag.anty)		// jeśli jest to oddziaływanie z antyneutrinem
		{
			if (e->flag.cc)
				return -1;
			else
			{
				if (e->nof (proton_pdg))
					return -51;
				else if (e->nof (neutron_pdg))
					return -52;	// sprawdzam dodatkowo ?
			}
		}
		else			// oddziaływanie z neutrinem
		{
			if (e->flag.cc)
				return 1;
			else
			{
				if (e->nof (proton_pdg))
					return 51;
				else if (e->nof (neutron_pdg))
					return 52;
			}
		}
	}

	if (e->flag.mec) {
		if (e->flag.anty) return -2;
		else return 2;
	}


	if (e->flag.res)		//rezonansowa produkcja: pojedynczy pion, pojed.eta, kaon, multipiony
	{
		Int_t liczba_pionow, liczba_kaonow;

		liczba_pionow =
		    e->nof (pion_pdg) + e->nof (pion_plus_pdg) + e->nof (pion_minus_pdg);
		liczba_kaonow = e->nof (kaon_pdg) + e->nof (kaon_pdg);

		if (liczba_pionow > 1 || liczba_pionow == 0)	// multipiony
		{
			if (e->flag.anty)
			{
				if (e->flag.cc)
					return -21;
				else
					return -41;
			}
			else
			{
				if (e->flag.cc)
					return 21;
				else
					return 41;
			}
		}

		if (liczba_pionow == 1)
		{
			if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
			{
				if (e->flag.cc)
				{
					if (e->nof (neutron_pdg) && e->nof (pion_minus_pdg))
						return -11;
					if (e->nof (neutron_pdg) && e->nof (pion_pdg))
						return -12;
					if (e->nof (proton_pdg) && e->nof (pion_minus_pdg))
						return -13;
				}
				else
				{
					if (e->nof (proton_pdg))
					{
						if (e->nof (pion_minus_pdg))
							return -33;
						else if (e->nof (pion_pdg))
							return -32;
					}
					else if (e->nof (neutron_pdg))
					{
						if (e->nof (pion_plus_pdg))
							return -34;
						else if (e->nof (pion_pdg))
							return -31;
					}
				}
			}
			else			// oddziaływanie z neutrinem
			{
				if (e->flag.cc)
				{
					if (e->nof (proton_pdg) && e->nof (pion_plus_pdg))
						return 11;
					if (e->nof (proton_pdg) && e->nof (pion_pdg))
						return 12;
					if (e->nof (neutron_pdg) && e->nof (pion_plus_pdg))
						return 13;
				}
				else
				{
					if (e->nof (proton_pdg))
					{
						if (e->nof (pion_minus_pdg))
							return 33;
						else if (e->nof (pion_pdg))
							return 32;
					}
					else if (e->nof (neutron_pdg))
					{
						if (e->nof (pion_plus_pdg))
							return 34;
						else if (e->nof (pion_pdg))
							return 31;
					}
				}
			}
		}

		if (e->nof (eta_pdg))	// produkcja rezonansowa ety
		{
			if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
			{
				if (e->flag.cc)
					return -22;
				else
				{
					if (e->nof (neutron_pdg))
						return -42;
					else if (e->nof (proton_pdg))
						return -43;	// sprawdzam dodatkowo ?
				}
			}
			else			// oddziaływanie z neutrinem
			{
				if (e->flag.cc)
					return 22;
				else
				{
					if (e->nof (neutron_pdg))
						return 42;
					else if (e->nof (proton_pdg))
						return 43;
				}
			}
		}

		if (e->nof (lambda_pdg) == 1 && liczba_kaonow == 1)	// produkcja rezonansowa kaonu
		{
			if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
			{
				if (e->flag.cc && e->nof (kaon_pdg))
					return -23;
				else
				{
					if (e->nof (kaon_pdg))
						return -44;
					else if (e->nof (kaon_plus_pdg))
						return -45;
				}
			}
			else			// oddziaływanie z neutrinem
			{
				if (e->flag.cc && e->nof (kaon_plus_pdg))
					return 23;
				else
				{
					if (e->nof (kaon_pdg))
						return 44;
					else if (e->nof (kaon_plus_pdg))
						return 45;
				}
			}


		}

	}

	if (e->flag.coh)		// koherentne  oddziaływanie tylko na O(16)
	{
		Int_t _target;
		_target = e->par.nucleus_p + e->par.nucleus_n;	// liczba masowa  O(16)

		if (_target == 16)
		{
			if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
			{
				if (e->flag.cc && e->nof (pion_minus_pdg))
					return -16;
				else if (e->nof (pion_pdg))
					return -36;
			}
			else			// oddziaływanie z neutrinem
			{
				if (e->flag.cc && e->nof (pion_plus_pdg))
					return 16;
				else if (e->nof (pion_pdg))
					return 36;
			}
		}
	}

	// gleboko nieelastyczne rozpraszanie
	if (e->flag.dis)
	{
		if (e->flag.anty)
		{
			if (e->flag.cc)
				return -26;
			else
				return -46;
		}
		else
		{
			if (e->flag.cc)
				return 26;
			else
				return 46;
		}
	}

	return 9999;
}

void NuWroInputHandler::CalcNUISANCEKinematics() {
	// std::cout << "NuWro Event Address " << fNuWroEvent << std::endl;
	// Reset all variables
	fNUISANCEEvent->ResetEvent();
	FitEvent* evt = fNUISANCEEvent;

	// Sort Event Info
	evt->fMode = ConvertNuwroMode(fNuWroEvent);

	if (abs(evt->fMode) > 60) {
		evt->fMode = 0;

		// Remove failed mode converts
		// return;
	}

	evt->Mode = evt->fMode;
	evt->fEventNo = 0.0;
	evt->fTotCrs = 0.0;
	evt->fTargetA = fNuWroEvent->par.nucleus_p + fNuWroEvent->par.nucleus_n;
	evt->fTargetZ = fNuWroEvent->par.nucleus_p;
	evt->fTargetH = 0;
	evt->fBound = (evt->fTargetA) == 1;

	// Check Particle Stack
	UInt_t npart_in = fNuWroEvent->in.size();
	UInt_t npart_out = fNuWroEvent->out.size();
	UInt_t npart_post = fNuWroEvent->post.size();
	UInt_t npart = npart_in + npart_out + npart_post;
	UInt_t kmax = evt->kMaxParticles;

	if (npart > kmax) {
		ERR(WRN) << "NUWRO has too many particles. Expanding stack." << std::endl;
	    fNUISANCEEvent->ExpandParticleStack(npart);
	}

	// Sort Particles
	evt->fNParticles = 0;
	std::vector<particle>::iterator p_iter;

	// Initial State
	for (p_iter = fNuWroEvent->in.begin(); p_iter != fNuWroEvent->in.end(); p_iter++) {
		AddNuWroParticle(fNUISANCEEvent, (*p_iter), kInitialState);
	}

	// FSI State
	// for (size_t i = 0; i < npart_in; i++ ) {
	// 	AddNuWroParticle(fNUISANCEEvent, (*p_iter), kFSIState);
	// }

	// Final State
	for (p_iter = fNuWroEvent->post.begin(); p_iter != fNuWroEvent->post.end(); p_iter++) {
		AddNuWroParticle(fNUISANCEEvent, (*p_iter), kFinalState);
	}

	// Fill Generator Info
	if (fSaveExtra) fNuWroInfo->FillGeneratorInfo(fNuWroEvent);

	// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent->OrderStack();
	return;
}

void NuWroInputHandler::AddNuWroParticle(FitEvent * evt, particle & p, int state) {

	// Add Mom
	evt->fParticleMom[evt->fNParticles][0] = p.p4().x;
	evt->fParticleMom[evt->fNParticles][1] = p.p4().y;
	evt->fParticleMom[evt->fNParticles][2] = p.p4().z;
	evt->fParticleMom[evt->fNParticles][3] = p.p4().t;

	// Status/PDG
	evt->fParticleState[evt->fNParticles] = state;
	evt->fParticlePDG[evt->fNParticles] = p.pdg;

	// Add to particle count
	evt->fNParticles++;
}

void NuWroInputHandler::Print(){
}

#endif

