#ifdef __NEUT_ENABLED__
#include "NEUTInputHandler.h"

NEUTGeneratorInfo::~NEUTGeneratorInfo() {
	DeallocateParticleStack();
}

void NEUTGeneratorInfo::AddBranchesToTree(TTree * tn) {
	tn->Branch("NEUTParticleN",          fNEUTParticleN,          "NEUTParticleN/I");
	tn->Branch("NEUTParticleStatusCode", fNEUTParticleStatusCode, "NEUTParticleStatusCode[NEUTParticleN]/I");
	tn->Branch("NEUTParticleAliveCode",  fNEUTParticleAliveCode,  "NEUTParticleAliveCode[NEUTParticleN]/I");
}

void NEUTGeneratorInfo::SetBranchesFromTree(TTree* tn) {
	tn->SetBranchAddress("NEUTParticleN",          &fNEUTParticleN );
	tn->SetBranchAddress("NEUTParticleStatusCode", &fNEUTParticleStatusCode );
	tn->SetBranchAddress("NEUTParticleAliveCode",  &fNEUTParticleAliveCode  );

}

void NEUTGeneratorInfo::AllocateParticleStack(int stacksize) {
	fNEUTParticleN = 0;
	fNEUTParticleStatusCode = new int[stacksize];
	fNEUTParticleStatusCode = new int[stacksize];
}

void NEUTGeneratorInfo::DeallocateParticleStack() {
	delete fNEUTParticleStatusCode;
	delete fNEUTParticleAliveCode;
}

void NEUTGeneratorInfo::FillGeneratorInfo(NeutVect* nevent) {
	Reset();
	for (int i = 0; i < nevent->Npart(); i++) {
		fNEUTParticleStatusCode[i] = nevent->PartInfo(i)->fStatus;
		fNEUTParticleAliveCode[i]  = nevent->PartInfo(i)->fIsAlive;
		fNEUTParticleN++;
	}
}

void NEUTGeneratorInfo::Reset() {
	for (int i = 0; i < fNEUTParticleN; i++) {
		fNEUTParticleStatusCode[i] = -1;
		fNEUTParticleAliveCode[i]  = 9;
	}
	fNEUTParticleN = 0;
}



NEUTInputHandler::NEUTInputHandler(std::string const& handle, std::string const& rawinputs) {
	LOG(SAM) << "Creating NEUTInputHandler : " << handle << std::endl;

	// Run a joint input handling
	fName = handle;

	// Setup the TChain
	fNEUTTree = new TChain("neuttree");
	fSaveExtra = FitPar::Config().GetParB("SaveExtraNEUT");
	fCacheSize = FitPar::Config().GetParI("CacheSize");
	fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");

	// Loop over all inputs and grab flux, eventhist, and nevents
	std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
	for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {

		// Open File for histogram access
		TFile* inp_file = new TFile(inputs[inp_it].c_str(), "READ");
		if (!inp_file or inp_file->IsZombie()) {
			THROW( "NEUT File IsZombie() at : '" << inputs[inp_it] << "'" << std::endl
			       << "Check that your file paths are correct and the file exists!" << std::endl
			       << "$ ls -lh " << inputs[inp_it] );
		}

		// Get Flux/Event hist
		TH1D* fluxhist  = (TH1D*)inp_file->Get(
		                      (PlotUtils::GetObjectWithName(inp_file, "flux")).c_str());
		TH1D* eventhist = (TH1D*)inp_file->Get(
		                      (PlotUtils::GetObjectWithName(inp_file, "evt")).c_str());
		if (!fluxhist or !eventhist) {
			ERROR(FTL, "Input File Contents: " << inputs[inp_it] );
			inp_file->ls();
			THROW( "NEUT FILE doesn't contain flux/xsec info. You may have to regenerate your MC!" );
		}

		// Get N Events
		TTree* neuttree = (TTree*)inp_file->Get("neuttree");
		if (!neuttree) {
			ERROR(FTL, "neuttree not located in NEUT file: " << inputs[inp_it]);
			THROW("Check your inputs, they may need to be completely regenerated!");
			throw;
		}
		int nevents = neuttree->GetEntries();
		if (nevents <= 0) {
			THROW("Trying to a TTree with " << nevents << " to TChain from : " << inputs[inp_it]);
		}

		// Register input to form flux/event rate hists
		RegisterJointInput(inputs[inp_it], nevents, fluxhist, eventhist);

		// Add To TChain
		fNEUTTree->AddFile( inputs[inp_it].c_str() );
	}


	// Registor all our file inputs
	SetupJointInputs();

	// Assign to tree
	fEventType = kNEUT;
	fNeutVect = NULL;
	fNEUTTree->SetBranchAddress("vectorbranch", &fNeutVect);
	fNEUTTree->GetEntry(0);

	// Create Fit Event
	fNUISANCEEvent = new FitEvent();
	fNUISANCEEvent->SetNeutVect(fNeutVect);

	if (fSaveExtra) {
		fNeutInfo = new NEUTGeneratorInfo();
		fNUISANCEEvent->AddGeneratorInfo(fNeutInfo);
	}

	fNUISANCEEvent->HardReset();
};

NEUTInputHandler::~NEUTInputHandler() {
  //	if (fNEUTTree) delete fNEUTTree;
	//	if (fNeutVect) delete fNeutVect;
	//	if (fNeutInfo) delete fNeutInfo;
};

void NEUTInputHandler::CreateCache() {
	if (fCacheSize > 0) {
		// fNEUTTree->SetCacheEntryRange(0, fNEvents);
		fNEUTTree->AddBranchToCache("vectorbranch", 1);
		fNEUTTree->SetCacheSize(fCacheSize);
	}
}

void NEUTInputHandler::RemoveCache() {
	// fNEUTTree->SetCacheEntryRange(0, fNEvents);
	fNEUTTree->AddBranchToCache("vectorbranch", 0);
	fNEUTTree->SetCacheSize(0);
}

FitEvent* NEUTInputHandler::GetNuisanceEvent(const UInt_t entry, const bool lightweight) {

	// Catch too large entries
	if (entry >= (UInt_t)fNEvents) return NULL;

	// Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
	fNEUTTree->GetEntry(entry);

	// Setup Input scaling for joint inputs
	fNUISANCEEvent->InputWeight = GetInputWeight(entry);

	// Run NUISANCE Vector Filler
	if (!lightweight) {
		CalcNUISANCEKinematics();
	}

	// Return event pointer
	return fNUISANCEEvent;
}

int NEUTInputHandler::GetNeutParticleStatus(NeutPart * part) {

	// State
	int state = kUndefinedState;

	// fStatus == -1 means initial  state
	if (part->fIsAlive == false && part->fStatus == -1) {
		state = kInitialState;

		// NEUT has a bit of a strange convention for fIsAlive and fStatus
		// combinations
		// for NC and neutrino particle isAlive true/false and status 2 means
		// final state particle
		// for other particles in NC status 2 means it's an FSI particle
		// for CC it means it was an FSI particle
	} else if (part->fStatus == 2) {
		// NC case is a little strange... The outgoing neutrino might be alive or
		// not alive. Remaining particles with status 2 are FSI particles that
		// reinteracted
		if (abs(fNeutVect->Mode) > 30 &&
		        (abs(part->fPID) == 14 || abs(part->fPID) == 12)) {
			state = kFinalState;
			// The usual CC case
		} else if (part->fIsAlive == true) {
			state = kFSIState;
		}
	} else if (part->fIsAlive == true && part->fStatus == 2 &&
	           (abs(part->fPID) == 14 || abs(part->fPID) == 12)) {
		state = kFinalState;

	} else if (part->fIsAlive == true && part->fStatus == 0) {
		state = kFinalState;

	} else if (part->fIsAlive == true) {
		ERR(WRN) << "Undefined NEUT state "
		         << " Alive: " << part->fIsAlive << " Status: " << part->fStatus
		         << " PDG: " << part->fPID << std::endl;
		throw;
	}

	return state;
}

void NEUTInputHandler::CalcNUISANCEKinematics() {

	// Reset all variables
	fNUISANCEEvent->ResetEvent();

	// Fill Globals
	fNUISANCEEvent->fMode    = fNeutVect->Mode;
	fNUISANCEEvent->Mode     = fNeutVect->Mode;
	fNUISANCEEvent->fEventNo = fNeutVect->EventNo;
	fNUISANCEEvent->fTargetA = fNeutVect->TargetA;
	fNUISANCEEvent->fTargetZ = fNeutVect->TargetZ;
	fNUISANCEEvent->fTargetH = fNeutVect->TargetH;
	fNUISANCEEvent->fBound   = bool(fNeutVect->Ibound);

	if (fNUISANCEEvent->fBound) {
		fNUISANCEEvent->fTargetPDG = TargetUtils::GetTargetPDGFromZA(fNUISANCEEvent->fTargetZ,
		                             fNUISANCEEvent->fTargetA);
	} else {
		fNUISANCEEvent->fTargetPDG = 1000010010;
	}

	// Check Particle Stack
	UInt_t npart = fNeutVect->Npart();
	UInt_t kmax = fNUISANCEEvent->kMaxParticles;
	if (npart > kmax) {
		ERR(FTL) << "NEUT has too many particles. Expanding stack." << std::endl;
		fNUISANCEEvent->ExpandParticleStack(npart);
		throw;
	}

	// Fill Particle Stack
	for (size_t i = 0; i < npart; i++) {

		// Get Current Count
		int curpart = fNUISANCEEvent->fNParticles;

		// Get NEUT Particle
		NeutPart* part = fNeutVect->PartInfo(i);

		// State
		int state = GetNeutParticleStatus(part);

		// Remove Undefined
		if (kRemoveUndefParticles &&
		        state == kUndefinedState) continue;

		// Remove FSI
		if (kRemoveFSIParticles &&
		        state == kFSIState) continue;

		// Remove Nuclear
		if (kRemoveNuclearParticles &&
		        (state == kNuclearInitial || state == kNuclearRemnant)) continue;

		// State
		fNUISANCEEvent->fParticleState[curpart] = state;

		// Mom
		fNUISANCEEvent->fParticleMom[curpart][0] = part->fP.X();
		fNUISANCEEvent->fParticleMom[curpart][1] = part->fP.Y();
		fNUISANCEEvent->fParticleMom[curpart][2] = part->fP.Z();
		fNUISANCEEvent->fParticleMom[curpart][3] = part->fP.T();

		// PDG
		fNUISANCEEvent->fParticlePDG[curpart] = part->fPID;

		// Add up particle count
		fNUISANCEEvent->fNParticles++;
	}

	// Save Extra Generator Info
	if (fSaveExtra) {
		fNeutInfo->FillGeneratorInfo(fNeutVect);
	}


	// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent-> OrderStack();
	return;
}

void NEUTUtils::FillNeutCommons(NeutVect* nvect) {

	// WARNING: This has only been implemented for a neuttree and not GENIE
	// This should be kept in sync with T2KNIWGUtils::GetNIWGEvent(TTree)

	//NEUT version info.  Can't get it to compile properly with this yet
	//neutversion_.corev  =   nvect->COREVer;
	//neutversion_.nucev  =   nvect->NUCEVer;
	//neutversion_.nuccv  =   nvect->NUCCVer;

	// Documentation: See nework.h
	nework_.modene = nvect->Mode;
	nework_.numne = nvect->Npart();

	nemdls_.mdlqeaf = nvect->QEVForm;
	nemdls_.mdlqe = nvect->QEModel;
	nemdls_.mdlspi = nvect->SPIModel;
	nemdls_.mdldis = nvect->DISModel;
	nemdls_.mdlcoh = nvect->COHModel;
	neutcoh_.necohepi = nvect->COHModel;

	nemdls_.xmaqe = nvect->QEMA;
	nemdls_.xmvqe = nvect->QEMV;
	nemdls_.kapp  = nvect->KAPPA;

	//nemdls_.sccfv = SCCFVdef;
	//nemdls_.sccfa = SCCFAdef;
	//nemdls_.fpqe = FPQEdef;

	nemdls_.xmaspi = nvect->SPIMA;
	nemdls_.xmvspi = nvect->SPIMV;
	nemdls_.xmares = nvect->RESMA;
	nemdls_.xmvres = nvect->RESMV;

	neut1pi_.xmanffres = nvect->SPIMA;
	neut1pi_.xmvnffres = nvect->SPIMV;
	neut1pi_.xmarsres = nvect->RESMA;
	neut1pi_.xmvrsres = nvect->RESMV;
	neut1pi_.neiff    = nvect->SPIForm;
	neut1pi_.nenrtype = nvect->SPINRType;
	neut1pi_.rneca5i  = nvect->SPICA5I;
	neut1pi_.rnebgscl = nvect->SPIBGScale;

	nemdls_.xmacoh = nvect->COHMA;
	nemdls_.rad0nu = nvect->COHR0;
	//nemdls_.fa1coh = nvect->COHA1err;
	//nemdls_.fb1coh = nvect->COHb1err;

	//neutdis_.nepdf = NEPDFdef;
	//neutdis_.nebodek = NEBODEKdef;

	neutcard_.nefrmflg  = nvect->FrmFlg;
	neutcard_.nepauflg  = nvect->PauFlg;
	neutcard_.nenefo16  = nvect->NefO16;
	neutcard_.nemodflg  = nvect->ModFlg;
	//neutcard_.nenefmodl = 1;
	//neutcard_.nenefmodh = 1;
	//neutcard_.nenefkinh = 1;
	//neutpiabs_.neabspiemit = 1;

	nenupr_.iformlen    =  nvect->FormLen;

	neutpiless_.ipilessdcy = nvect->IPilessDcy;
	neutpiless_.rpilessdcy = nvect->RPilessDcy;


	neutpiless_.ipilessdcy = nvect->IPilessDcy;
	neutpiless_.rpilessdcy = nvect->RPilessDcy;

	neffpr_.fefqe   = nvect->NuceffFactorPIQE;
	neffpr_.fefqeh  = nvect->NuceffFactorPIQEH;
	neffpr_.fefinel = nvect->NuceffFactorPIInel;
	neffpr_.fefabs  = nvect->NuceffFactorPIAbs;
	neffpr_.fefcx   = nvect->NuceffFactorPICX;
	neffpr_.fefcxh  = nvect->NuceffFactorPICXH;

	neffpr_.fefcoh =  nvect->NuceffFactorPICoh;
	neffpr_.fefqehf = nvect->NuceffFactorPIQEHKin;
	neffpr_.fefcxhf = nvect->NuceffFactorPICXKin;
	neffpr_.fefcohf = nvect->NuceffFactorPIQELKin;

	for ( int i = 0; i < nework_.numne; i++ ) {
		nework_.ipne[i] = nvect->PartInfo(i)->fPID;
		nework_.pne[i][0] = (float)nvect->PartInfo(i)->fP.X() / 1000; // VC(NE)WORK in M(G)eV
		nework_.pne[i][1] = (float)nvect->PartInfo(i)->fP.Y() / 1000; // VC(NE)WORK in M(G)eV
		nework_.pne[i][2] = (float)nvect->PartInfo(i)->fP.Z() / 1000; // VC(NE)WORK in M(G)eV
	}
	// fsihist.h


	// neutroot fills a dummy object for events with no FSI to prevent memory leak when
	// reading the TTree, so check for it here

	if ( (int)nvect->NfsiVert() == 1 ) { // An event with FSI must have at least two vertices
		//    if (nvect->NfsiPart()!=1 || nvect->Fsiprob!=-1)
		//      ERR(WRN) << "T2KNeutUtils::fill_neut_commons(TTree) NfsiPart!=1 or Fsiprob!=-1 when NfsiVert==1" << std::endl;

		fsihist_.nvert = 0;
		fsihist_.nvcvert = 0;
		fsihist_.fsiprob = 1;
	}
	else { // Real FSI event
		fsihist_.nvert = (int)nvect->NfsiVert();
		for (int ivert = 0; ivert < fsihist_.nvert; ivert++) {
			fsihist_.iflgvert[ivert] = nvect->FsiVertInfo(ivert)->fVertID;
			fsihist_.posvert[ivert][0] = (float)nvect->FsiVertInfo(ivert)->fPos.X();
			fsihist_.posvert[ivert][1] = (float)nvect->FsiVertInfo(ivert)->fPos.Y();
			fsihist_.posvert[ivert][2] = (float)nvect->FsiVertInfo(ivert)->fPos.Z();
		}

		fsihist_.nvcvert = nvect->NfsiPart();
		for (int ip = 0; ip < fsihist_.nvcvert; ip++) {
			fsihist_.abspvert[ip] = (float)nvect->FsiPartInfo(ip)->fMomLab;
			fsihist_.abstpvert[ip] = (float)nvect->FsiPartInfo(ip)->fMomNuc;
			fsihist_.ipvert[ip] = nvect->FsiPartInfo(ip)->fPID;
			fsihist_.iverti[ip] = nvect->FsiPartInfo(ip)->fVertStart;
			fsihist_.ivertf[ip] = nvect->FsiPartInfo(ip)->fVertEnd;
			fsihist_.dirvert[ip][0] = (float)nvect->FsiPartInfo(ip)->fDir.X();
			fsihist_.dirvert[ip][1] = (float)nvect->FsiPartInfo(ip)->fDir.Y();
			fsihist_.dirvert[ip][2] = (float)nvect->FsiPartInfo(ip)->fDir.Z();
		}
		fsihist_.fsiprob = nvect->Fsiprob;
	}

	neutcrscom_.crsx = nvect->Crsx;
	neutcrscom_.crsy = nvect->Crsy;
	neutcrscom_.crsz = nvect->Crsz;
	neutcrscom_.crsphi = nvect->Crsphi;
	neutcrscom_.crsq2 = nvect->Crsq2;

	neuttarget_.numbndn = nvect->TargetA - nvect->TargetZ;
	neuttarget_.numbndp = nvect->TargetZ;
	neuttarget_.numfrep = nvect->TargetH;
	neuttarget_.numatom = nvect->TargetA;
	posinnuc_.ibound = nvect->Ibound;

	// put empty nucleon FSI history (since it is not saved in the NeutVect format)
	//Comment out as NEUT does not have the necessary proton FSI information yet
	//  nucleonfsihist_.nfnvert = 0;
	//  nucleonfsihist_.nfnstep = 0;


}


#endif
