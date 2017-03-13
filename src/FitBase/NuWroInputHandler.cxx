#ifdef __NUWRO_ENABLED__
#include "NuWroInputHandler.h"

void NuWroGeneratorInfo::AddBranchesToTree(TTree * tn) {
}

void NuWroGeneratorInfo::Reset() {
}

NuWroInputHandler::NuWroInputHandler(std::string const& handle, std::string const& rawinputs) {

	// Run a joint input handling
	fName = handle;
	jointinput = false;
	jointindexswitch = 0;
	fMaxEvents = FitPar::Config().GetParI("input.fMaxEvents");

	// Form list of all inputs, remove brackets if required.
	std::vector<std::string> inputs = GeneralUtils::ParseToStr(rawinputs, ",");
	if (inputs.front()[0] == '(') {
		inputs.front() = inputs.front().substr(1);
	}
	if (inputs.back()[inputs.back().size() - 1] == ')') {
		inputs.back() = inputs.back().substr(0, inputs.back().size() - 1);
	}
	for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
		LOG(SAM) << "\t -> Found input file: " << inputs[inp_it] << std::endl;
	}

	// Setup the TChain
	fNuWroTree = new TChain("treeout");

	// Loop over all inputs and grab flux, eventhist, and nevents
	// Also add it to the TChain
	int evtcounter = 0;
	if (inputs.size() > 1) jointinput = true;
	for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {

		// Add to TChain
		fNuWroTree->Add( inputs[inp_it].c_str() );

		// Open File for histogram access
		TFile* inp_file = new TFile(inputs[inp_it].c_str(), "READ");

		// Get Flux/Event hist
		TH1D* fluxhist  = (TH1D*)inp_file->Get(
		                      (PlotUtils::GetObjectWithName(inp_file, "FluxHist")).c_str());
		TH1D* eventhist = (TH1D*)inp_file->Get(
		                      (PlotUtils::GetObjectWithName(inp_file, "EvtHist")).c_str());

		// If no flux hist throw
		if (!fluxhist or !eventhist) {
			ERR(FTL) << "NuWro Inputs do not have input histograms!" << std::endl;

			if (FitPar::Config().GetParB("regennuwro")) {
				ProcessNuWroInputFlux(inputs[inp_it]);
			} else {
				ERR(FTL) << "If you would like NUISANCE to generate these for you "
				         << "please set parameter regennuwro=1 and re-run." << std::endl;
				throw;
			}
		}

		// Get N events
		TTree* nuwrotree = (TTree*)inp_file->Get("treeout");
		int nevents = nuwrotree->GetEntries();

		// Push into individual input vectors
		jointfluxinputs.push_back( (TH1D*) fluxhist->Clone() );
		jointeventinputs.push_back( (TH1D*) eventhist->Clone() );

		jointindexlow.push_back(evtcounter);
		jointindexhigh.push_back(evtcounter + nevents);
		evtcounter += nevents;

		// Add to the total flux/event hist
		if (!fFluxHist) fFluxHist = (TH1D*) fluxhist->Clone();
		else fFluxHist->Add(fluxhist);

		if (!fEventHist) fEventHist = (TH1D*) eventhist->Clone();
		else fEventHist->Add(eventhist);
	}

	// Setup NEvents and the FitEvent
	fNEvents = fNuWroTree->GetEntries();
	fEventType = kNUWRO;
	fNuWroEvent = NULL;
	fNuWroTree->SetBranchAddress("e", &fNuWroEvent);

	fNUISANCEEvent = new FitEvent(fNuWroEvent);
	fNUISANCEEvent->HardReset();
	
	fBaseEvent = static_cast<BaseFitEvt*>(fNUISANCEEvent);

	// Normalise event histograms for relative flux contributions.
	for (size_t i = 0; i < jointeventinputs.size(); i++) {
		TH1D* eventhist = (TH1D*) jointeventinputs.at(i)->Clone();

		// Determine nallowed
		int nallowed = jointindexhigh[i] - jointindexlow[i];
		if (fMaxEvents != -1) {
			nallowed = int( double(nallowed) *
			                (double(fMaxEvents) / double(fNEvents)) );
		}
		jointindexallowed.push_back(nallowed);

		// Set scale, undoing other scale factor.
		double scale = double(fNEvents) / fEventHist->Integral("width");
		scale *= eventhist->Integral("width");
		scale /= double(jointindexallowed[i]);

		jointindexscale .push_back(scale);
	}

	fEventHist->SetNameTitle((fName + "_EVT").c_str(), (fName + "_EVT").c_str());
	fFluxHist->SetNameTitle((fName + "_FLUX").c_str(), (fName + "_FLUX").c_str());

	// Setup extra flags
	save_extra = FitPar::Config().GetParB("save_extra_nuwro_info");
	if (save_extra) {
		fNuWroInfo = new NuWroGeneratorInfo();
		fNUISANCEEvent->fGenInfo = fNuWroInfo;
	}

	// Setup Max Events
	if (fMaxEvents > 1 && fMaxEvents < fNEvents) {
		LOG(SAM) << " -> Reading only " << fMaxEvents << " events from total."
		         << std::endl;
		fNEvents = fMaxEvents;
	}


};

void NuWroInputHandler::ProcessNuWroInputFlux(const std::string file) {


}

FitEvent* NuWroInputHandler::GetNuisanceEvent(const UInt_t entry) {

	// Catch too large entries
	if (entry >= fNEvents) return NULL;

	// Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
	fNuWroTree->GetEntry(entry);

	// Get latest TTree and get entry, loop round in parrallel and grab entries from other TTrees.
	// Get event corresponding to this TTree and replace pointer in fNuWroEvent with it.

	// Setup Input scaling for joint inputs
	if (jointinput) {
		fNUISANCEEvent->InputWeight = GetInputWeight(entry);
	} else {
		fNUISANCEEvent->InputWeight = 1.0;
	}

	// Run NUISANCE Vector Filler
	CalcNUISANCEKinematics();

	return fNUISANCEEvent;
}

#ifdef __NUWRO_ENABLED__
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
#endif

void NuWroInputHandler::CalcNUISANCEKinematics() {
	// std::cout << "NuWro Event Address " << fNuWroEvent << std::endl;
	// Reset all variables
	fNUISANCEEvent->ResetEvent();
	FitEvent* evt = fNUISANCEEvent;

	// Sort Event Info
	evt->fMode = GeneratorUtils::ConvertNuwroMode(fNuWroEvent);
	if (abs(evt->fMode) > 60) evt->fMode = 0;

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
		ERR(FTL) << "NUWRO has too many particles" << std::endl;
		ERR(FTL) << "npart=" << npart << " kMax=" << kmax
		         << " in,out,post = " << npart_in << "," << npart_out << ","
		         << npart_post << std::endl;
		throw;
	}

	// Setup Infomation for extra stuff
	// if (fSaveExtraInfo){
	// }

	// Sort Particles
	evt->fNParticles = 0;
	std::vector<particle>::iterator p_iter;

	// Initial State
	for (p_iter = fNuWroEvent->in.begin(); p_iter != fNuWroEvent->in.end(); p_iter++) {
		AddNuWroParticle(fNUISANCEEvent, (*p_iter), kInitialState);
	}

	for (p_iter = fNuWroEvent->post.begin(); p_iter != fNuWroEvent->post.end(); p_iter++) {
		AddNuWroParticle(fNUISANCEEvent, (*p_iter), kFinalState);
	}

	// // FSI State
	// for (size_t i = 0; i < npart_in; i++ ) {
	// 	AddNuWroParticle(fNUISANCEEvent, &fNuWroEvent->out[i], kFSIState);
	// }

	// // Final Particles
	// for (size_t i = 0; i < npart_in; i++ ) {
	// 	AddNuWroParticle(fNUISANCEEvent, &fNuWroEvent->post[i], kFinalState);
	// }


	// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent->OrderStack();
	return;
}

void NuWroInputHandler::AddNuWroParticle(FitEvent* evt, const particle& p, int state) {

	// Add Mom
	evt->fParticleMom[evt->fNParticles][0] = p.p4().x;
	evt->fParticleMom[evt->fNParticles][1] = p.p4().y;
	evt->fParticleMom[evt->fNParticles][2] = p.p4().z;
	evt->fParticleMom[evt->fNParticles][3] = p.p4().t;

	// Status/PDG
	evt->fParticleState[evt->fNParticles] = state;
	evt->fParticlePDG[evt->fNParticles] = p.pdg;

	// Flag setup for saving extra information.
	// if (fSaveExtraInfo){
	// }

	// Add to particle count
	evt->fNParticles++;
}


double NuWroInputHandler::GetInputWeight(int entry) {

	// Find Switch Scale
	while ( entry < jointindexlow[jointindexswitch] ||
	        entry >= jointindexhigh[jointindexswitch] ) {
		jointindexswitch++;

		// Loop Around
		if (jointindexswitch == jointindexlow.size()) {
			jointindexswitch = 0;
		}
	}
	return jointindexscale[jointindexswitch];
};


BaseFitEvt* NuWroInputHandler::GetBaseEvent(const UInt_t entry) {

	// Catch too large entries
	if (entry >= fNEvents) return NULL;

	// Read entry from TTree to fill NEUT Vect in BaseFitEvt;
	fNuWroTree->GetEntry(entry);

	// Set joint scaling if required
	if (jointinput) {
	  fBaseEvent->InputWeight = GetInputWeight(entry);
	} else {
	  fBaseEvent->InputWeight = 1.0;
	}

	return fBaseEvent;
}

#endif

