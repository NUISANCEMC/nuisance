#ifdef __NUANCE_ENABLED__
#include "NUANCEInputHandler.h"


NUANCEGeneratorInfo::~NUANCEGeneratorInfo() {
	DeallocateParticleStack();
}

void NUANCEGeneratorInfo::AddBranchesToTree(TTree * tn) {
	// tn->Branch("NEUTParticleN",          fNEUTParticleN,          "NEUTParticleN/I");
	// tn->Branch("NEUTParticleStatusCode", fNEUTParticleStatusCode, "NEUTParticleStatusCode[NEUTParticleN]/I");
	// tn->Branch("NEUTParticleAliveCode",  fNEUTParticleAliveCode,  "NEUTParticleAliveCode[NEUTParticleN]/I");
}

void NUANCEGeneratorInfo::SetBranchesFromTree(TTree* tn) {
	// tn->SetBranchAddress("NEUTParticleN",          &fNEUTParticleN );
	// tn->SetBranchAddress("NEUTParticleStatusCode", &fNEUTParticleStatusCode );
	// tn->SetBranchAddress("NEUTParticleAliveCode",  &fNEUTParticleAliveCode  );
}

void NUANCEGeneratorInfo::AllocateParticleStack(int stacksize) {
	// fNEUTParticleN = 0;
	// fNEUTParticleStatusCode = new int[stacksize];
	// fNEUTParticleStatusCode = new int[stacksize];
}

void NUANCEGeneratorInfo::DeallocateParticleStack() {
	// delete fNEUTParticleStatusCode;
	// delete fNEUTParticleAliveCode;
}

void NUANCEGeneratorInfo::FillGeneratorInfo(NuanceEvent* nevent) {
	Reset();
	// for (int i = 0; i < nevent->Npart(); i++) {
	// fNEUTParticleStatusCode[i] = nevent->PartInfo(i)->fStatus;
	// fNEUTParticleAliveCode[i]  = nevent->PartInfo(i)->fIsAlive;
	// fNEUTParticleN++;
	// }
}

void NUANCEGeneratorInfo::Reset() {
	// for (int i = 0; i < fNEUTParticleN; i++) {
	// fNEUTParticleStatusCode[i] = -1;
	// fNEUTParticleAliveCode[i]  = 9;
	// }
	// fNEUTParticleN = 0;
}


NUANCEInputHandler::NUANCEInputHandler(std::string const& handle, std::string const& rawinputs) {
	LOG(SAM) << "Creating NUANCEInputHandler : " << handle << std::endl;

	// Run a joint input handling
	fName = handle;
	fSaveExtra = FitPar::Config().GetParB("SaveExtraNUANCE");
	fCacheSize = FitPar::Config().GetParI("CacheSize");
	fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");

	// Parse Inputs
	std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
	if (inputs.size() > 1) {
		ERR(FTL) << "NUANCE is not currently setup to handle joint inputs sorry!" << std::endl
		         << "If you know how to correctly normalise the events for this"
		         << " please let us know!" << std::endl;
	}

	// Read in NUANCE Tree
	fNUANCETree = new TChain("h3");
	fNUANCETree->AddFile(rawinputs.c_str());

	// Get entries and fNuwroEvent
	int nevents = fNUANCETree->GetEntries();

	double EnuMin = 0.0;
	double EnuMax = 1000.0;

	TH1D* fluxhist = new TH1D((fName + "_FLUX").c_str(), (fName + "_FLUX").c_str(),
	                          100, EnuMin, EnuMax);
	for (int i = 0; i < fluxhist->GetNbinsX(); i++) {
		fluxhist->SetBinContent(i + 1, 1.0);
	}
	fluxhist->Scale(1.0 / fluxhist->Integral());

	TH1D* eventhist = new TH1D((fName + "_EVT").c_str(), (fName + "_EVT").c_str(), 100,
	                           EnuMin, EnuMax);
	for (int i = 0; i < fluxhist->GetNbinsX(); i++) {
		eventhist->SetBinContent(i + 1, 1.0);
	}
	eventhist->Scale(1.0 / eventhist->Integral());

	RegisterJointInput( rawinputs, nevents, fluxhist, eventhist );
	SetupJointInputs();

	// Setup Reader
	fNuanceEvent = new NuanceEvent();
	fNuanceEvent->SetBranchAddresses(fNUANCETree);
	fNUANCETree->GetEntry(0);

	// Setup Event in FitEvent
	fNUISANCEEvent = new FitEvent();
	fNUISANCEEvent->SetNuanceEvent(fNuanceEvent);

	// Setup extra if needed
	if (fSaveExtra){
		ERR(FTL) << "NO SAVEExtra Implemented for NUANCE YET!" << std::endl;
		throw;
		// fNuanceInfo = new NUANCEGeneratorInfo();
		// fNUISANCEEvent->AddGeneratorInfo(fNuanceInfo);
	}

};

NUANCEInputHandler::~NUANCEInputHandler() {
	if (fNuanceEvent) delete fNuanceEvent;
	if (fNUANCETree)  delete fNUANCETree;
	// if (fNuanceInfo)  delete fNuanceInfo;
}

void NUANCEInputHandler::CreateCache() {
	if (fCacheSize > 0) {
		fNUANCETree->SetCacheEntryRange(0, fNEvents);
		fNUANCETree->AddBranchToCache("h3", 1);
		fNUANCETree->SetCacheSize(fCacheSize);
	}
}

void NUANCEInputHandler::RemoveCache() {
	fNUANCETree->SetCacheEntryRange(0, fNEvents);
	fNUANCETree->AddBranchToCache("h3", 0);
	fNUANCETree->SetCacheSize(0);
}

FitEvent* NUANCEInputHandler::GetNuisanceEvent(const UInt_t entry, const bool lightweight) {

	// Check out of bounds
	if (entry >= (UInt_t)fNEvents) return NULL;

	// Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
	fNUANCETree->GetEntry(entry);

	// Setup Input scaling for joint inputs
	fNUISANCEEvent->InputWeight = GetInputWeight(entry);

	// Run NUISANCE Vector Filler
	if (!lightweight) {
		CalcNUISANCEKinematics();
	}

	return fNUISANCEEvent;
}

void NUANCEInputHandler::CalcNUISANCEKinematics() {

	// Reset all variables
	fNUISANCEEvent->ResetEvent();

	// Get shortened pointer
	FitEvent* evt = fNUISANCEEvent;

	// Fill Global
	evt->fMode = ConvertNuanceMode(fNuanceEvent);
	evt->Mode = evt->fMode;
	evt->fEventNo = 0.0;
	evt->fTotCrs = 1.0;
	evt->fTargetA = 0.0;
	evt->fTargetZ = 0.0;
	evt->fTargetH = 0;
	evt->fBound = 0.0;

	// Fill particle Stack
	evt->fNParticles = 0;

	// Check Particle Stack
	UInt_t npart = 2 + fNuanceEvent->n_leptons + fNuanceEvent->n_hadrons;
	UInt_t kmax = evt->kMaxParticles;
	if (npart > kmax) {
		ERR(FTL) << "NUANCE has too many particles" << std::endl;
		ERR(FTL) << "npart=" << npart << " kMax=" << kmax << std::endl;
		throw;
	}

	// Fill Neutrino
	evt->fParticleState[0] = kInitialState;
	evt->fParticleMom[0][0] = fNuanceEvent->p_neutrino[0];
	evt->fParticleMom[0][1] = fNuanceEvent->p_neutrino[1];
	evt->fParticleMom[0][2] = fNuanceEvent->p_neutrino[2];
	evt->fParticleMom[0][3] = fNuanceEvent->p_neutrino[3];
	evt->fParticlePDG[0] = fNuanceEvent->neutrino;

	// Fill Target Nucleon
	evt->fParticleState[1] = kInitialState;
	evt->fParticleMom[1][0] = fNuanceEvent->p_targ[0];
	evt->fParticleMom[1][1] = fNuanceEvent->p_targ[1];
	evt->fParticleMom[1][2] = fNuanceEvent->p_targ[2];
	evt->fParticleMom[1][3] = fNuanceEvent->p_targ[3];
	evt->fParticlePDG[1] = fNuanceEvent->target;
	evt->fNParticles = 2;

	// Fill Outgoing Leptons
	for (int i = 0; i < fNuanceEvent->n_leptons; i++) {
		evt->fParticleState[evt->fNParticles] = kFinalState;
		evt->fParticleMom[evt->fNParticles][0] = fNuanceEvent->p_lepton[i][0];
		evt->fParticleMom[evt->fNParticles][1] = fNuanceEvent->p_lepton[i][1];
		evt->fParticleMom[evt->fNParticles][2] = fNuanceEvent->p_lepton[i][2];
		evt->fParticleMom[evt->fNParticles][3] = fNuanceEvent->p_lepton[i][3];
		evt->fParticlePDG[evt->fNParticles] = fNuanceEvent->lepton[i];
		evt->fNParticles++;
	}

	// Fill Outgoing Hadrons
	for (int i = 0; i < fNuanceEvent->n_hadrons; i++) {
		evt->fParticleState[evt->fNParticles] = kFinalState;
		evt->fParticleMom[evt->fNParticles][0] = fNuanceEvent->p_hadron[i][0];
		evt->fParticleMom[evt->fNParticles][1] = fNuanceEvent->p_hadron[i][1];
		evt->fParticleMom[evt->fNParticles][2] = fNuanceEvent->p_hadron[i][2];
		evt->fParticleMom[evt->fNParticles][3] = fNuanceEvent->p_hadron[i][3];
		evt->fParticlePDG[evt->fNParticles] = fNuanceEvent->hadron[i];
		evt->fNParticles++;
	}

	// Save Extra info
	if (fSaveExtra) {
		// fNuanceInfo->FillGeneratorInfo(fNuanceEvent);
	}

	// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent-> OrderStack();
	return;
}

void NUANCEInputHandler::Print() {}

int NUANCEInputHandler::ConvertNuanceMode(NuanceEvent * evt) {
	int ch = evt->channel;
	int sg = 1;
	if (evt->neutrino < 0) sg = -1;

	switch (ch) {
	//  1 NUANCE CCQE -> NEUT CCQE 1
	case 1: return sg * 1;
	//  2 NUANCE NCEL -> NEUT NCEL 51,52 -> Set from whether target is p or n
	case 2:
		if (evt->target == 2212) return sg * 51;
		else return sg * 52;

	// 3 NUANCE CCPIP -> NEUT CCPIP 11
	case 3: return sg * 11;
	// 4 NUANCE CCPI0 -> NEUT CCPI0 = 12
	case 4: return sg * 12;
	// 5 NUANCE CCPIPn -> NEUT CCPIPn 13
	case 5: return sg * 13;
	// 6 NUANCE NCpPI0 -> NEUT NCpPI0  32
	case 6: return sg * 32;
	// 7 NUANCE NCpPI+ -> NEUT NCpPI+  34
	case 7: return sg * 34;
	// 8 NUANCE NCnPI0 -> NEUT NCnPI0  31
	case 8: return sg * 31;
	// 9  NUANCE NCnPIM -> NEUT NCnPIM  33
	case 9: return sg * 33;
	// 10 NUANCE CCPIP -> NEUT CCPIP -11
	case 10: return sg * 11;
	// 11 NUANCE CCPI0 -> NEUT CCPI0 -12
	case 11: return sg * 12;
	// 12 NUANCE CCPIPn -> NEUT CCPIPn 13
	case 12: return sg * 13;
	// 13 NUANCE NCpPI0 -> NEUT NCnPI0 -32
	case 13: return sg * 32;
	// 14 NUANCE NCpPI+ -> NEUT NCpPI+ -34
	case 14: return sg * 34;
	// 15 NUANCE NCnPI0 -> NEUT NCnPI0 -31
	case 15: return sg * 31;
	// 16 NUANCE NCnPIM -> NEUT NCnPIM -33
	case 16: return sg * 33;
	// 17 NUANCE -> NEUT 21 CC MULTIPI
	case 17: return sg * 21;
	// 18 NUANCE -> NEUT 21 CC MULTIPI
	case 18: return sg * 21;
	// 19 NUANCE -> NEUT 21 CC MULTIPI
	case 19: return sg * 21;
	// 20 NUANCE -> NEUT 21  CC MULTIPI
	case 20: return sg * 21;
	// 21 NUANCE -> NEUT 21  CC MULTIPI
	case 21: return sg * 21;
	// 22 NUANCE -> NEUT 41 NC MULTIPI
	case 22: return sg * 41;
	// 23 NUANCE -> NEUT 41 NC MULTIPI
	case 23: return sg * 41;
	// 24 NUANCE -> NEUT 41 NC MULTIPI
	case 24: return sg * 41;
	// 25 NUANCE -> NEUT 41 NC MULTIPI
	case 25: return sg * 41;
	// 26 NUANCE -> NEUT 41 NC MULTIPI
	case 26: return sg * 41;
	// 27 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV)
	case 27: return sg * 41;
	// 28 NUANCE -> NEUT -21 CC (1.3 < W < 2 GeV)
	case 28: return sg * 21;
	// 29 NUANCE -> NEUT -21 CC (1.3 < W < 2 GeV)
	case 29: return sg * 21;
	// 30 NUANCE -> NEUT -21 CC (1.3 < W < 2 GeV)
	case 30: return sg * 21;
	// 31 NUANCE -> NEUT -21 CC (1.3 < W < 2 GeV)
	case 31: return sg * 21;
	// 32 NUANCE -> NEUT -21 CC (1.3 < W < 2 GeV)
	case 32: return sg * 21;
	// 33 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV)
	case 33: return sg * 41;
	// 34 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV)
	case 34: return sg * 41;
	// 35 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV)
	case 35: return sg * 41;
	// 36 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV)
	case 36: return sg * 41;
	// 37 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV)
	case 37: return sg * 41;
	// 38 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV)
	case 38: return sg * 41;

	// 39 NUANCE -> NEUT 22
	case 39: return sg * 22;
	// 40 NUANCE -> NEUT 22
	case 40: return sg * 22;
	// 41 NUANCE -> NEUT 22
	case 41: return sg * 22;
	// 42 NUANCE -> NEUT 43
	case 42: return sg * 43;
	// 43 NUANCE -> NEUT 43
	case 43: return sg * 43;
	// 44 NUANCE -> NUET 42
	case 44: return sg * 42;
	// 45 NUANCE -> NEUT -42
	case 45: return sg * 42;
	// 46 NUANCE -> NEUT -22
	case 46: return sg * 22;
	// 47 NUANCE -> NEUT -22
	case 47: return sg * 22;
	// 48 NUANCE -> NEUT -22
	case 48: return sg * 22;
	// 49 NUANCE -> NEUT -43
	case 49: return sg * 43;
	// 50 NUANCE -> NEUT -43
	case 50: return sg * 43;
	// 51 NUANCE -> NEUT -42
	case 51: return sg * 42;
	// 52 NUANCE -> NEUT -42
	case 52: return sg * 42;

	// 53 NUANCE -> NEUT 23 CC 1K
	case 53: return sg * 23;
	// 54 NUANCE -> NEUT 23 CC 1K
	case 54: return sg * 23;
	// 55 NUANCE -> NEUT 23 CC 1K
	case 55: return sg * 23;
	// 56 NUANCE -> NEUT 45 NC 1K
	case 56: return sg * 45;
	// 57 NUANCE -> NEUT 44 NC 1K
	case 57: return sg * 44;
	// 58 NUANCE -> NEUT 44 NC 1K
	case 58: return sg * 44;
	// 59 NUANCE -> NEUT 44 NC 1K
	case 59: return sg * 44;
	// 60 NUANCE -> NEUT -23 CC 1K
	case 60: return sg * 23;
	// 61 NUANCE -> NEUT -23 CC 1K
	case 61: return sg * 23;
	// 62 NUANCE -> NEUT -23 CC 1K
	case 62: return sg * 23;
	// 63 NUANCE -> NEUT -23 CC 1K
	case 63: return sg * 23;
	// 64 NUANCE -> NEUT -44 NC 1K
	case 64: return sg * 44;
	// 65 NUANCE -> NEUT -44 NC 1K
	case 65: return sg * 44;
	// 66 NUANCE -> NEUT -45 NC 1K
	case 66: return sg * 45;
	// 67  NUANCE -> NEUT 22  CC1eta
	case 67: return sg * 22;
	// 68 NUANCE -> NEUT 43 NC p eta
	case 68: return sg * 43;
	// 69 NUANCE -> NEUT 43 NC n eta
	case 69: return sg * 43;
	// 70 NUANCE -> NEUT -22 CC1eta
	case 70: return sg * 22;
	// 71 NUANCE -> NEUT -43 NC p eta
	case 71: return sg * 43;
	// 72 NUANCE -> NEUT 42 NC n eta
	case 72: return sg * 42;

	// 73 NUANCE -> NEUT 21 CC Multi Pi
	case 73: return sg * 21;
	// 74 NUANCE -> NEUT 41 NC Multi Pi
	case 74: return sg * 41;
	// 75 NUANCE -> NEUT 41 NC Multi Pi
	case 75: return sg * 41;
	// 76 NUANCE -> NEUT -21 CC Multi Pi
	case 76: return sg * 21;
	// 77 NUANCE -> NEUT -41 NC Multi Pi
	case 77: return sg * 41;
	// 78 NUANCE -> NEUT -41 NC Multi Pi
	case 78: return sg * 41;
	//  79  NUANCE -> NEUT 21 CC Multi Pi
	case 79: return sg * 21;
	// 80 NUANCE -> NEUT 21 CC Multi Pi
	case 80: return sg * 21;
	// 81 NUANCE -> NEUT 41 NC Multi Pi
	case 81: return sg * 41;
	// 82 NUANCE -> NEUT 41 NC Multi Pi
	case 82: return sg * 41;
	// 83 NUANCE -> NEUT 41 NC Multi Pi
	case 83: return sg * 41;
	// 84 NUANCE -> NEUT 41 NC Multi Pi
	case 84: return sg * 84;
	// 85 NUANCE -> NEUT -21 CC Multi Pi
	case 85: return sg * 21;
	// 86 NUANCE -> NEUT -21  CC Multi Pi
	case 86: return sg * 21;
	// 87 NUANCE -> NEUT -41 CC Multi Pi
	case 87: return sg * 41;
	// 88 NUANCE -> NEUT -41
	case 88: return sg * 41;
	// 89 NUANCE -> NEUT -41
	case 89: return sg * 41;
	// 90 NUANCE -> NEUT -41
	case 90: return sg * 41;

	// 91 NUANCE -> NEUT 26  CC DIS
	case 91: return sg * 26;
	// 92 NUANCE -> NEUT 46  NC DIS
	case 92: return sg * 46;
	// 93 NUANCE -> NEUT 17 1#gamma from #Delta
	case 93: return sg * 17;
	// 94 NUANCE -> NEUT 39 1#gamma from #Delta
	case 94: return sg * 39;
	// 95 -> UNKOWN NEUT MODE
	case 95: return sg * 0;
	// 96 NUANCE -> NEUT 36 NC COH
	case 96: return sg * 36;
	// 97 NUANCE -> NEUT 16
	case 97: return sg * 16;
	// 98 -> UNKNOWN NEUT MODE
	case 98: return sg * 0;
	// 99 -> UNKNOWN NEUT MODE
	case 99: return sg * 0;
	default:
		ERR(FTL) << "Unknown Nuance Channel ID = " << ch << std::endl;
		throw ("Exiting.");
		return 0;
	}
	return 0;

}


/*
// Notes copied from NuanceChannels.pdf
1 NUANCE CCQE -> NEUT CCQE 1
CC, numu n --> mu- p
Cabibbo-allowed quasi-elastic scattering from nucleons
2 NUANCE NCEL -> NEUT NCEL 51,52 -> Set from whether target is p or n
NC, numu N --> num N, (N=n,p)
(quasi)-elastic scattering from nucleons
3 NUANCE CCPIP -> NEUT CCPIP 11
CC, numu p --> mu- p pi+
resonant single pion production
4 NUANCE CCPI0 -> NEUT CCPI0 = 12
CC, numu n --> mu- p pi0
resonant single pion production
5 NUANCE CCPIPn -> NEUT CCPIPn 13
CC, numu n --> mu- n pi+
resonant single pion production
6 NUANCE NCpPI0 -> NEUT NCpPI0  32
NC, numu p --> numu p pi0
resonant single pion production
7 NUANCE NCpPI+ -> NEUT NCpPI+  34
NC, numu p --> numu n pi+
resonant single pion production
8 NUANCE NCnPI0 -> NEUT NCnPI0  31
NC, numu n --> numu n pi0
resonant single pion production
9  NUANCE NCnPIM -> NEUT NCnPIM  33
NC, numu n --> numu p pi-
resonant single pion production
10 NUANCE CCPIP -> NEUT CCPIP -11
CC, numubar p --> mu- p pi+
resonant single pion production
11 NUANCE CCPI0 -> NEUT CCPI0 -12
CC, numubar n --> mu- p pi0
resonant single pion production
12 NUANCE CCPIPn -> NEUT CCPIPn -13
CC, numubar n --> mu- n pi+
resonant single pion production
13 NUANCE NCpPI0 -> NEUT NCnPI0 -32
NC, numubar p --> numubar p pi0
resonant single pion production
14 NUANCE NCpPI+ -> NEUT NCpPI+ -34
NC, numubar p --> numubar n pi+
resonant single pion production
15 NUANCE NCnPI0 -> NEUT NCnPI0 -31
NC, numubar n --> numubar n pi0
resonant single pion production
16 NUANCE NCnPIM -> NEUT NCnPIM -33
NC, numubar n --> numubar p pi-
resonant single pion production


17 NUANCE -> NEUT 21 CC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow l^{-} N' multi-#pi
CC, numu p --> mu- Delta+ pi+
resonant processes involving more than a single pion
18 NUANCE -> NEUT 21 CC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow l^{-} N' multi-#pi
CC, numu p --> mu- Delta++ pi0
resonant processes involving more than a single pion
19 NUANCE -> NEUT 21 CC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow l^{-} N' multi-#pi
CC, numu n --> mu- Delta+ pi0
resonant processes involving more than a single pion
20 NUANCE -> NEUT 21 CC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow l^{-} N' multi-#pi
CC, numu n --> mu- Delta0 pi+
resonant processes involving more than a single pion
21 NUANCE -> NEUT 21 CC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow l^{-} N' multi-#pi
CC, numu n --> mu- Delta++ pi-
resonant processes involving more than a single pion

22 NUANCE -> NEUT 41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC, numu p+ --> numu Delta+ pi0
resonant processes involving more than a single pion
23 NUANCE -> NEUT 41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC,numu p --> numu Delta0 pi+
resonant processes involving more than a single pion
24 NUANCE -> NEUT 41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC, numu p --> numu Delta++ pi-
resonant processes involving more than a single pion
25 NUANCE -> NEUT 41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC, numu n --> numu Delta+ pi-
resonant processes involving more than a single pion
26 NUANCE -> NEUT 41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC, numu n --> numu Delta0 pi0
resonant processes involving more than a single pion

27 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC, numubar n --> numubar Delta- pi+
resonant processes involving more than a single pion
28 NUANCE -> NEUT -21 CC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow l^{-} N' multi-#pi
CC, numubar p --> mu- Delta+ pi+
resonant processes involving more than a single pion
29 UANCE -> NEUT -21 CC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow l^{-} N' multi-#pi
CC, numubar p --> mu- Delta++ pi0
resonant processes involving more than a single pion
30 NUANCE -> NEUT -21 CC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow l^{-} N' multi-#pi
CC, numubar n --> mu- Delta+ pi0
resonant processes involving more than a single pion
31 NUANCE -> NEUT -21 CC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow l^{-} N' multi-#pi
CC, numubar n --> mu- Delta0 pi+
resonant processes involving more than a single pion
32 NUANCE -> NEUT -21 CC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow l^{-} N' multi-#pi
CC, numubar n --> mu- Delta++ pi-
resonant processes involving more than a single pion
33 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC, numubar p+ --> numubar Delta+ pi0
resonant processes involving more than a single pion
34 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC,numubar p --> numubar Delta0 pi+
resonant processes involving more than a single pion
35 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC, numubar p --> numubar Delta++ pi-
resonant processes involving more than a single pion
36 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC, numubar n --> numubar Delta+ pi-
resonant processes involving more than a single pion
37 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC, numubar n --> numubar Delta0 pi0
resonant processes involving more than a single pion
38 NUANCE -> NEUT -41 "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi"
NC, numubar n --> numubar Delta- pi+
resonant processes involving more than a single pion


// RHO Production lumped in with eta production
22 CCeta
43 NCeta on p
42 NCeta on n

39 NUANCE -> NEUT 22
CC, numu p --> mu- p rho+(770)
resonant processes involving more than a single pion
40 NUANCE -> NEUT 22
CC, numu n --> mu- p rho0(770)
resonant processes involving more than a single pion
41 NUANCE -> NEUT 22
CC, numu n --> mu- n rho+(770)
resonant processes involving more than a single pion
42 NUANCE -> NEUT 43
NC, numu p --> numu p rho0(770)
resonant processes involving more than a single pion
43 NUANCE -> NEUT 43
NC, numu p --> numu n rho+(770)
resonant processes involving more than a single pion
44 NUANCE -> NUET 42
NC, numu n --> numu n rho0(770)
resonant processes involving more than a single pion
45 NUANCE -> NEUT -42
NC, numubar n --> numubar p rho-(770)
resonant processes involving more than a single pion
46 NUANCE -> NEUT -22
CC, numubar p --> mu- p rho+(770)
resonant processes involving more than a single pion
47 NUANCE -> NEUT -22
CC, numubar n --> mu- p rho0(770)
resonant processes involving more than a single pion
48 NUANCE -> NEUT -22
CC, numubar n --> mu- n rho+(770)
resonant processes involving more than a single pion
49 NUANCE -> NEUT -43
NC, numubar p --> numubar p rho0(770)
resonant processes involving more than a single pion
50 NUANCE -> NEUT -43
NC, numubar p --> numubar n rho+(770)
resonant processes involving more than a single pion
51 NUANCE -> NEUT -42
NC, numubar n --> numubar n rho0(770)
resonant processes involving more than a single pion
52 NUANCE -> NEUT -42
NC, numubar n --> numubar p rho-(770)
resonant processes involving more than a single pion


53 NUANCE -> NEUT 23 CC 1K: #nu_{l} n #rightarrow l^{-} #Lambda K^{+}
CC, numu p --> mu- Sigma+ K+
resonant processes involving more than a single pion
54 NUANCE -> NEUT 23 CC 1K: #nu_{l} n #rightarrow l^{-} #Lambda K^{+}
CC, numu n --> mu- Sigma0 K+
resonant processes involving more than a single pion
55 NUANCE -> NEUT 23 CC 1K: #nu_{l} n #rightarrow l^{-} #Lambda K^{+}
CC, numu n --> mu- Sigma+ K0
resonant processes involving more than a single pion
56 NUANCE -> NEUT 45 NC 1K: #nu_{l} n #rightarrow #nu_{l} #Lambda K^{+}
NC, numu p --> numu Sigma0 K+
resonant processes involving more than a single pion
57 NUANCE -> NEUT 44 NC 1K: #nu_{l} n #rightarrow #nu_{l} #Lambda K^{0}
NC, numu p --> numu Sigma+ K0
resonant processes involving more than a single pion
58 NUANCE -> NEUT 44 NC 1K: #nu_{l} n #rightarrow #nu_{l} #Lambda K^{0}
NC, numu n --> numu Sigma0 K0
resonant processes involving more than a single pion
59 NUANCE -> NEUT 45 NC 1K: #nu_{l} n #rightarrow #nu_{l} #Lambda K^{+}
NC, numu n --> numu Sigma- K+
resonant processes involving more than a single pion
60 NUANCE -> NEUT -23 CC 1K: #nu_{l} n #rightarrow l^{-} #Lambda K^{+}
CC, numubar p --> mu- Sigma+ K+
resonant processes involving more than a single pion
61 NUANCE -> NEUT -23 CC 1K: #nu_{l} n #rightarrow l^{-} #Lambda K^{+}
CC, numubar n --> mu- Sigma0 K+
resonant processes involving more than a single pion
62 NUANCE -> NEUT -23 CC 1K: #nu_{l} n #rightarrow l^{-} #Lambda K^{+}
CC, numubar n --> mu- Sigma+ K0
resonant processes involving more than a single pion
63 NUANCE -> NEUT -45 NC 1K: #nu_{l} n #rightarrow #nu_{l} #Lambda K^{+}
NC, numubar p --> numubar Sigma0 K+
resonant processes involving more than a single pion
64 NUANCE -> NEUT -44 NC 1K: #nu_{l} n #rightarrow #nu_{l} #Lambda K^{0}
NC, numubar p --> numubar Sigma+ K0
resonant processes involving more than a single pion
65 NUANCE -> NEUT -44 NC 1K: #nu_{l} n #rightarrow #nu_{l} #Lambda K^{0}
NC, numubar n --> numubar Sigma0 K0
resonant processes involving more than a single pion
66 NUANCE -> NEUT -45 NC 1K: #nu_{l} n #rightarrow #nu_{l} #Lambda K^{+}
NC, numubar n --> numubar Sigma- K+
resonant processes involving more than a single pion

67  NUANCE -> NEUT 22
ModeStack[22]->SetTitle("CC1#eta^{0} on n");
CC, numu n --> mu- p eta
resonant processes involving more than a single pion
68 NUANCE -> NEUT 43
NC, numu p --> numu p eta
resonant processes involving more than a single pion
69 NUANCE -> NEUT 42
NC, numu n --> numu n eta
resonant processes involving more than a single pion
70 NUANCE -> NEUT -22
ModeStack[22]->SetTitle("CC1#eta^{0} on n");
CC, numubar n --> mu- p eta
resonant processes involving more than a single pion
71 NUANCE -> NEUT -43
ModeStack[43]->SetTitle("NC1#eta^{0} on p");
NC, numubar p --> numubar p eta
resonant processes involving more than a single pion
72 NUANCE -> NEUT -42
ModeStack[42]->SetTitle("NC1#eta^{0} on n");
NC, numubar n --> numubar n eta
resonant processes involving more than a single pion

73 NUANCE -> NEUT 21
ModeStack[21]->SetTitle("Multi #pi (1.3 < W < 2.0)");
CC, numu n --> mu- K+ Lambda
resonant processes involving more than a single pion
74 NUANCE -> NEUT 41
ModeStack[41]->SetTitle("Multi #pi (1.3 < W < 2.0)");
NC, numu p --> numu K+ Lambda
resonant processes involving more than a single pion
75 NUANCE -> NEUT 41
ModeStack[41]->SetTitle("Multi #pi (1.3 < W < 2.0)");
NC, numu n --> numu K0 Lambda
resonant processes involving more than a single pion
76 NUANCE -> NEUT -21
ModeStack[21]->SetTitle("Multi #pi (1.3 < W < 2.0)");
CC, numubar n --> mu- K+ Lambda
resonant processes involving more than a single pion
77 NUANCE -> NEUT -41
ModeStack[41]->SetTitle("Multi #pi (1.3 < W < 2.0)");
NC, numubar p --> numubar K+ Lambda
resonant processes involving more than a single pion
78 NUANCE -> NEUT -41
ModeStack[41]->SetTitle("Multi #pi (1.3 < W < 2.0)");
NC, numubar n --> numubar K0 Lambda
resonant processes involving more than a single pion

CC Multipi  ModeStack[21]->SetTitle("Multi #pi (1.3 < W < 2.0)");
NC Multipi  ModeStack[41]->SetTitle("Multi #pi (1.3 < W < 2.0)");
79  NUANCE -> NEUT 21
CC, numu n --> mu- p pi+ pi-
two pion production
80 NUANCE -> NEUT 21
CC, numu n --> mu- p pi0 pi0
two pion production
81 NUANCE -> NEUT 41
NC, numu p --> numu p pi+ pi-
two pion production
82 NUANCE -> NEUT 41
NC, numu p --> numu p pi0 pi0
two pion production
83 NUANCE -> NEUT 41
NC, numu n --> numu n pi+ pi-
two pion production
84 NUANCE -> NEUT 41
NC, numu n --> numu n pi0 pi0
two pion production
85 NUANCE -> NEUT -21
CC, numubar n --> mu- p pi+ pi-
two pion production
86 NUANCE -> NEUT -21
CC, numubar n --> mu- p pi0 pi0
two pion production
87 NUANCE -> NEUT -41
ModeStack[41]->SetTitle("Multi #pi (1.3 < W < 2.0)");
NC, numubar p --> numubar p pi+ pi-
two pion production
88 NUANCE -> NEUT -41
ModeStack[41]->SetTitle("Multi #pi (1.3 < W < 2.0)");
NC, numubar p --> numubar p pi0 pi0
two pion production
89 NUANCE -> NEUT -41
ModeStack[41]->SetTitle("Multi #pi (1.3 < W < 2.0)");
NC, numubar n --> numubar n pi+ pi-
two pion production
90 NUANCE -> NEUT -41
ModeStack[41]->SetTitle("Multi #pi (1.3 < W < 2.0)");
NC, numubar n --> numubar n pi0 pi0
two pion production


91 NUANCE -> NEUT 26
ModeStack[26]->SetTitle("DIS (W > 2.0)");
CC, numu N --> mu- X (where N=n,p)
deep inelastic scattering (nu or nubar)
92 NUANCE -> NEUT 46
ModeStack[46]->SetTitle("DIS (W > 2.0)");
NC, numu N --> numu X (where N=n,p)
deep inelastic scattering (nu or nubar)

93 NUANCE -> NEUT 17 1#gamma from #Delta: #nu_{l} n #rightarrow l^{-} p #gamma
CC, numu n --> mu- p gamma
Delta radiative decay, Delta --> N gamma (only in NUANCE versions v3 and and higher)
94 NUANCE -> NEUT 39 1#gamma from #Delta: #nu_{l} p #rightarrow #nu_{l} p #gamma
neutModeID[15] = 38;  neutModeName[15] = "ncngam"; neutModeTitle[15] = "1#gamma from #Delta: #nu_{l} n #rightarrow #nu_{l} n #gamma";
neutModeID[16] = 39;  neutModeName[16] = "ncpgam"; neutModeTitle[16] = "1#gamma from #Delta: #nu_{l} p #rightarrow #nu_{l} p #gamma";
NC, numu N --> numu N gamma
Delta radiative decay, Delta --> N gamma (only in NUANCE versions v3 and and higher)

95 -> UNKOWN NEUT MODE
CC, numubar p --> mu+ Lambda, numubar n -- > mu+ Sigma-, numubar p --> mu+ Sigma0
Cabibbo-suppressed QE hyperon production from nucleons

96 NUANCE -> NEUT 36
neutModeID[14] = 36;  neutModeName[14] = "nccoh";  neutModeTitle[14] = "NC coherent-#pi: #nu_{l} ^{16}O #rightarrow #nu_{l} ^{16}O #pi^{0}";
NC, numu A --> numu pi0 A
coherent or diffractive pi0 production
97 NUANCE -> NEUT 16
neutModeID[4] = 16;   neutModeName[4] = "cccoh";   neutModeTitle[4] = "CC coherent-#pi: #nu_{l} ^{16}O #rightarrow l^{-} ^{16}O #pi^{+}";
CC, numu A --> mu- pi+ A (or numubar A -->
coherent or diffractive pi0 production

98 -> UNKNOWN NEUT MODE
NC, numu e- --> numu e- (or numubar e- -->
neutrino + electron elastic scattering
99 -> UNKNOWN NEUT MODE
CC, numu e- --> mu- nue
neutrino + electron inverse muon decay

NEUT Modes:
// CC Modes
 neutModeID[0] = 1;    neutModeName[0] = "ccqe";    neutModeTitle[0] = "CCQE: #nu_{l} n #rightarrow l^{-} p";
 neutModeID[1] = 11;   neutModeName[1] = "ccppip";  neutModeTitle[1] = "CC 1#pi: #nu_{l} p #rightarrow l^{-} p #pi^{+}";
 neutModeID[2] = 12;   neutModeName[2] = "ccppi0";  neutModeTitle[2] = "CC 1#pi: #nu_{l} n #rightarrow l^{-} p #pi^{0}";
 neutModeID[3] = 13;   neutModeName[3] = "ccnpip";  neutModeTitle[3] = "CC 1#pi: #nu_{l} n #rightarrow l^{-} n #pi^{+}";
 neutModeID[4] = 16;   neutModeName[4] = "cccoh";   neutModeTitle[4] = "CC coherent-#pi: #nu_{l} ^{16}O #rightarrow l^{-} ^{16}O #pi^{+}";
 neutModeID[5] = 17;   neutModeName[5] = "ccgam";   neutModeTitle[5] = "1#gamma from #Delta: #nu_{l} n #rightarrow l^{-} p #gamma";
 neutModeID[6] = 21;   neutModeName[6] = "ccmpi";   neutModeTitle[6] = "CC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow l^{-} N' multi-#pi";
 neutModeID[7] = 22;   neutModeName[7] = "cceta";   neutModeTitle[7] = "CC 1#eta: #nu_{l} n #rightarrow l^{-} p #eta";
 neutModeID[8] = 23;   neutModeName[8] = "cck";     neutModeTitle[8] = "CC 1K: #nu_{l} n #rightarrow l^{-} #Lambda K^{+}";
 neutModeID[9] = 26;   neutModeName[9] = "ccdis";   neutModeTitle[9] = "CC DIS (2 GeV < W): #nu_{l} N #rightarrow l^{-} N' mesons";

neutModeID[10] = 31;  neutModeName[10] = "ncnpi0"; neutModeTitle[10] = "NC 1#pi: #nu_{l} n #rightarrow #nu_{l} n #pi^{0}";
neutModeID[11] = 32;  neutModeName[11] = "ncppi0"; neutModeTitle[11] = "NC 1#pi: #nu_{l} p #rightarrow #nu_{l} p #pi^{0}";
neutModeID[12] = 33;  neutModeName[12] = "ncppim"; neutModeTitle[12] = "NC 1#pi: #nu_{l} n #rightarrow #nu_{l} p #pi^{-}";
neutModeID[13] = 34;  neutModeName[13] = "ncnpip"; neutModeTitle[13] = "NC 1#pi: #nu_{l} p #rightarrow #nu_{l} n #pi^{+}";

neutModeID[14] = 36;  neutModeName[14] = "nccoh";  neutModeTitle[14] = "NC coherent-#pi: #nu_{l} ^{16}O #rightarrow #nu_{l} ^{16}O #pi^{0}";
neutModeID[15] = 38;  neutModeName[15] = "ncngam"; neutModeTitle[15] = "1#gamma from #Delta: #nu_{l} n #rightarrow #nu_{l} n #gamma";
neutModeID[16] = 39;  neutModeName[16] = "ncpgam"; neutModeTitle[16] = "1#gamma from #Delta: #nu_{l} p #rightarrow #nu_{l} p #gamma";

neutModeID[17] = 41;  neutModeName[17] = "ncmpi";  neutModeTitle[17] = "NC (1.3 < W < 2 GeV): #nu_{l} N #rightarrow #nu_{l} N multi-#pi";

neutModeID[18] = 42;  neutModeName[18] = "ncneta"; neutModeTitle[18] = "NC 1#eta: #nu_{l} n #rightarrow #nu_{l} n #eta";
neutModeID[19] = 43;  neutModeName[19] = "ncpeta"; neutModeTitle[19] = "NC 1#eta: #nu_{l} p #rightarrow #nu_{l} p #eta";

neutModeID[20] = 44;  neutModeName[20] = "nck0";   neutModeTitle[20] = "NC 1K: #nu_{l} n #rightarrow #nu_{l} #Lambda K^{0}";
neutModeID[21] = 45;  neutModeName[21] = "nckp";   neutModeTitle[21] = "NC 1K: #nu_{l} n #rightarrow #nu_{l} #Lambda K^{+}";

neutModeID[22] = 46;  neutModeName[22] = "ncdis";  neutModeTitle[22] = "NC DIS (2 GeV < W): #nu_{l} N #rightarrow #nu_{l} N' mesons";

neutModeID[23] = 51;  neutModeName[23] = "ncqep";  neutModeTitle[23] = "NC elastic: #nu_{l} p #rightarrow #nu_{l} p";
neutModeID[24] = 52;  neutModeName[24] = "ncqen";  neutModeTitle[24] = "NC elastic: #nu_{l} n #rightarrow #nu_{l} n";
*/

#endif

