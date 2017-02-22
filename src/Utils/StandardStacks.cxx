#include "StandardStacks.h"


/// Fake stack functions
FakeStack::FakeStack(TH1* hist) {
	fTemplate =  (TH1*)hist;
	fNDim = fTemplate->GetDimension();
}

FakeStack::~FakeStack() {
	fTemplate = NULL;
	fNDim = 0;
}

void FakeStack::Fill(double x, double y, double z, double weight) {
	if (fNDim == 1)      fTemplate->Fill(x, y);
	else if (fNDim == 2) ((TH2*)fTemplate)->Fill(x, y, z);
	else if (fNDim == 3) ((TH3*)fTemplate)->Fill(x, y, z, weight);
}

void FakeStack::Scale(double norm, std::string opt) {
	fTemplate->Scale(norm, opt.c_str());
}

void FakeStack::Reset() {
	fTemplate->Reset();
}

void FakeStack::Write() {
	fTemplate->Write();
}



/// TrueModeStack Functions
TrueModeStack::TrueModeStack(std::string name, std::string title, TH1* hist) {
	fName = name;
	fTitle = title;

	// CC
	AddMode(0, "CCQE",      "CCQE", kBlue, 2, 1001);
	AddMode(1, "CC2p2h",    "2p2h", kRed, 2, 1001);
	AddMode(2, "CC1piponp", "CC1#pi^{+} on p",  kGreen,   2, 1001);
	AddMode(3, "CC1pi0onn", "CC1#pi^{0} on n",  kGreen + 3, 2, 1001);
	AddMode(4, "CC1piponn", "CC1#pi^{+} on n",  kGreen - 2, 2, 1001);
	AddMode(5, "CCcoh",     "CC coherent",      kBlue,    2, 1001);
	AddMode(6, "CC1gamma",  "CC1#gamma",        kMagenta, 2, 1001);
	AddMode(7, "CCMultipi", "Multi #pi (1.3 < W < 2.0)", kYellow, 2, 1001);
	AddMode(8, "CC1eta",    "CC1#eta^{0} on n", kYellow - 2, 2, 1001);
	AddMode(9, "CC1lamkp",  "CC1#Lambda1K^{+}", kYellow - 6, 2, 1001);
	AddMode(10, "CCDIS",    "DIS (W > 2.0)",    kRed, 2, 1001);

	// NC
	AddMode(11, "NC1pi0onn", "NC1#pi^{0} on n", kBlue,   2, 3004);
	AddMode(12, "NC1pi0onp", "NC1#pi^{0} on p", kBlue + 3, 2, 3004);
	AddMode(13, "NC1pimonn",  "NC1#pi^{-} on n", kBlue - 2, 2, 3004);
	AddMode(14, "NC1piponp", "NC1#pi^{+} on p", kBlue - 8, 2, 3004);
	AddMode(15, "NCcoh",       "NC Coherent",     kBlue + 8, 2, 3004);
	AddMode(16, "NC1gammaonn",  "NC1#gamma on n",  kMagenta, 2, 3004);
	AddMode(17, "NC1gammaonp",  "NC1#gamma on p", kMagenta - 10, 2, 3004);
	AddMode(18, "NCMultipi", "Multi #pi (1.3 < W < 2.0)", kBlue - 10, 2, 3004);
	AddMode(19, "NC1etaonn", "NC1#eta^{0} on n", kYellow - 2, 2, 3004);
	AddMode(20, "NC1etaonp", "NC1#eta^{0} on p", kYellow - 4, 2, 3004);
	AddMode(21, "NC1kamk0",  "NC1#Lambda1K^{0} on n", kYellow - 6, 2, 3004);
	AddMode(22, "NC1lamkp",  "NC1#Lambda1K^{+}", kYellow - 10, 2, 3004);
	AddMode(23, "NCDIS", "DIS (W > 2.0)", kRed, 2, 3004);
	AddMode(24, "NCELonp", "NCEL on p", kBlack, 2, 3004);
	AddMode(25, "NCELonn", "NCEL on n", kGray, 2, 3004);

	// Undefined
	AddMode(26, "UNDEFINED", "Undefined", kRed + 1, 2, 3000);

	StackBase::SetupStack(hist);
};

int TrueModeStack::ConvertModeToIndex(int mode) {
	switch (abs(mode)) {
	case 1:  return 0;  // CCQE
	case 2:  return 1;  // CC2p2h
	case 11: return 2;  // CC1piponp
	case 12: return 3;  // CC1pi0onn
	case 13: return 4;  // CC1piponn
	case 16: return 5;  // CCcoh
	case 17: return 6;  // CC1gamma
	case 21: return 7;  // CCMultipi
	case 22: return 8;  // CC1eta
	case 23: return 9;  // CC1lamkp
	case 26: return 10; // CCDIS

	case 31: return 11; // NC1pi0onn
	case 32: return 12; // NC1pi0onp
	case 33: return 13; // NC1pimonn
	case 34: return 14; // NC1piponp
	case 36: return 15; // NCcoh
	case 38: return 16; // NC1gammaonn
	case 39: return 17; // NC1gammaonp
	case 41: return 18; // NC Multipi
	case 42: return 19; // NC1etaonn
	case 43: return 20; // NC1etaonp
	case 44: return 21; // NC1kamk0
	case 45: return 22; // NC1lamkp
	case 46: return 23; // NCDIS
	case 51: return 24; // NCEL on p
	case 52: return 25; // NCEL on n
	default: return 26; // Undefined
	}
};

void TrueModeStack::Fill(int mode, double x, double y, double z, double weight) {
	StackBase::FillStack(ConvertModeToIndex(mode), x, y, z, weight);
};

void TrueModeStack::Fill(FitEvent* evt, double x, double y, double z, double weight) {
	StackBase::FillStack(ConvertModeToIndex(evt->fMode), x, y, z, weight);
};

void TrueModeStack::Fill(BaseFitEvt* evt, double x, double y, double z, double weight) {
	StackBase::FillStack(ConvertModeToIndex(evt->Mode), x, y, z, weight);
};



/// TrueModeStack Functions
NuNuBarTrueModeStack::NuNuBarTrueModeStack(std::string name, std::string title, TH1* hist) {
	fName = name;
	fTitle = title;

	// Neutrino
	// CC
	AddMode(0, "NU_CCQE",       "#nu CCQE", kBlue, 2, 1001);
	AddMode(1, "NU_CC2p2h",     "#nu 2p2h", kRed, 2, 1001);
	AddMode(2, "NU_CC1piponp",  "#nu CC1#pi^{+} on p",  kGreen,   2, 1001);
	AddMode(3, "NU_CC1pi0onn",  "#nu CC1#pi^{0} on n",  kGreen + 3, 2, 1001);
	AddMode(4, "NU_CC1piponn",  "#nu CC1#pi^{+} on n",  kGreen - 2, 2, 1001);
	AddMode(5, "NU_CCcoh",      "#nu CC coherent",      kBlue,    2, 1001);
	AddMode(6, "NU_CC1gamma",   "#nu CC1#gamma",        kMagenta, 2, 1001);
	AddMode(7, "NU_CCMultipi",  "#nu Multi #pi (1.3 < W < 2.0)", kYellow, 2, 1001);
	AddMode(8, "NU_CC1eta",     "#nu CC1#eta^{0} on n", kYellow - 2, 2, 1001);
	AddMode(9, "NU_CC1lamkp",   "#nu CC1#Lambda1K^{+}", kYellow - 6, 2, 1001);
	AddMode(10, "NU_CCDIS",     "#nu DIS (W > 2.0)",     kRed, 2, 1001);

	// NC
	AddMode(11, "NU_NC1pi0onn",    "#nu NC1#pi^{0} on n", kBlue,   2, 3004);
	AddMode(12, "NU_NC1pi0onp",    "#nu NC1#pi^{0} on p", kBlue + 3, 2, 3004);
	AddMode(13, "NU_NC1pimonn",    "#nu NC1#pi^{-} on n", kBlue - 2, 2, 3004);
	AddMode(14, "NU_NC1piponp",    "#nu NC1#pi^{+} on p", kBlue - 8, 2, 3004);
	AddMode(15, "NU_NCcoh",        "#nu NC Coherent",     kBlue + 8, 2, 3004);
	AddMode(16, "NU_NC1gammaonn",  "#nu NC1#gamma on n",  kMagenta, 2, 3004);
	AddMode(17, "NU_NC1gammaonp",  "#nu NC1#gamma on p", kMagenta - 10, 2, 3004);
	AddMode(18, "NU_NCMultipi",    "#nu Multi #pi (1.3 < W < 2.0)", kBlue - 10, 2, 3004);
	AddMode(19, "NU_NC1etaonn",    "#nu NC1#eta^{0} on n", kYellow - 2, 2, 3004);
	AddMode(20, "NU_NC1etaonp",    "#nu NC1#eta^{0} on p", kYellow - 4, 2, 3004);
	AddMode(21, "NU_NC1kamk0",     "#nu NC1#Lambda1K^{0} on n", kYellow - 6, 2, 3004);
	AddMode(22, "NU_NC1lamkp",     "#nu NC1#Lambda1K^{+}", kYellow - 10, 2, 3004);
	AddMode(23, "NU_NCDIS",        "#nu DIS (W > 2.0)", kRed, 2, 3004);
	AddMode(24, "NU_NCELonp",      "#nu NCEL on p", kBlack, 2, 3004);
	AddMode(25, "NU_NCELonn",      "#nu NCEL on n", kGray, 2, 3004);

	// Undefined
	AddMode(26, "NU_UNDEFINED",    "#nu Undefined", kRed + 2, 2, 3000);

	// CC
	AddMode(27, "ANTINU_CCQE",       "#bar{#nu} CCQE", kBlue, 2, 1001);
	AddMode(28, "ANTINU_CC2p2h",     "#bar{#nu} 2p2h", kRed, 2, 1001);
	AddMode(29, "ANTINU_CC1piponp",  "#bar{#nu} CC1#pi^{+} on p",  kGreen,   2, 1001);
	AddMode(30, "ANTINU_CC1pi0onn",  "#bar{#nu} CC1#pi^{0} on n",  kGreen + 3, 2, 1001);
	AddMode(31, "ANTINU_CC1piponn",  "#bar{#nu} CC1#pi^{+} on n",  kGreen - 2, 2, 1001);
	AddMode(32, "ANTINU_CCcoh",      "#bar{#nu} CC coherent",      kBlue,    2, 1001);
	AddMode(33, "ANTINU_CC1gamma",   "#bar{#nu} CC1#gamma",        kMagenta, 2, 1001);
	AddMode(34, "ANTINU_CCMultipi",  "#bar{#nu} Multi #pi (1.3 < W < 2.0)", kYellow, 2, 1001);
	AddMode(35, "ANTINU_CC1eta",     "#bar{#nu} CC1#eta^{0} on n", kYellow - 2, 2, 1001);
	AddMode(36, "ANTINU_CC1lamkp",   "#bar{#nu} CC1#Lambda1K^{+}", kYellow - 6, 2, 1001);
	AddMode(37, "ANTINU_CCDIS",     "#bar{#nu} DIS (W > 2.0)",     kRed, 2, 1001);

	// NC
	AddMode(38, "ANTINU_NC1pi0onn",    "#bar{#nu} NC1#pi^{0} on n", kBlue,   2, 3004);
	AddMode(39, "ANTINU_NC1pi0onp",    "#bar{#nu} NC1#pi^{0} on p", kBlue + 3, 2, 3004);
	AddMode(40, "ANTINU_NC1pimonn",    "#bar{#nu} NC1#pi^{-} on n", kBlue - 2, 2, 3004);
	AddMode(41, "ANTINU_NC1piponp",    "#bar{#nu} NC1#pi^{+} on p", kBlue - 8, 2, 3004);
	AddMode(42, "ANTINU_NCcoh",        "#bar{#nu} NC Coherent",     kBlue + 8, 2, 3004);
	AddMode(43, "ANTINU_NC1gammaonn",  "#bar{#nu} NC1#gamma on n",  kMagenta, 2, 3004);
	AddMode(44, "ANTINU_NC1gammaonp",  "#bar{#nu} NC1#gamma on p", kMagenta - 10, 2, 3004);
	AddMode(45, "ANTINU_NCMultipi",    "#bar{#nu} Multi #pi (1.3 < W < 2.0)", kBlue - 10, 2, 3004);
	AddMode(46, "ANTINU_NC1etaonn",    "#bar{#nu} NC1#eta^{0} on n", kYellow - 2, 2, 3004);
	AddMode(47, "ANTINU_NC1etaonp",    "#bar{#nu} NC1#eta^{0} on p", kYellow - 4, 2, 3004);
	AddMode(48, "ANTINU_NC1kamk0",     "#bar{#nu} NC1#Lambda1K^{0} on n", kYellow - 6, 2, 3004);
	AddMode(49, "ANTINU_NC1lamkp",     "#bar{#nu} NC1#Lambda1K^{+}", kYellow - 10, 2, 3004);
	AddMode(50, "ANTINU_NCDIS",        "#bar{#nu} DIS (W > 2.0)", kRed, 2, 3004);
	AddMode(51, "ANTINU_NCELonp",      "#bar{#nu} NCEL on p", kBlack, 2, 3004);
	AddMode(52, "ANTINU_NCELonn",      "#bar{#nu} NCEL on n", kGray, 2, 3004);

	// Undefined
	AddMode(53, "NU_UNDEFINED",    "#bar{#nu} Undefined", kRed + 2, 2, 3000);

	// Non Neutrino
	AddMode(54, "UNDEFINED", "Non-#nu Undefined", kBlack, 2, 3000);

	StackBase::SetupStack(hist);
};

int NuNuBarTrueModeStack::ConvertModeToIndex(int mode) {
	switch (abs(mode)) {
	case 1:  return 0;  // CCQE
	case 2:  return 1;  // CC2p2h
	case 11: return 2;  // CC1piponp
	case 12: return 3;  // CC1pi0onn
	case 13: return 4;  // CC1piponn
	case 16: return 5;  // CCcoh
	case 17: return 6;  // CC1gamma
	case 21: return 7;  // CCMultipi
	case 22: return 8;  // CC1eta
	case 23: return 9;  // CC1lamkp
	case 26: return 10; // CCDIS

	case 31: return 11; // NC1pi0onn
	case 32: return 12; // NC1pi0onp
	case 33: return 13; // NC1pimonn
	case 34: return 14; // NC1piponp
	case 36: return 15; // NCcoh
	case 38: return 16; // NC1gammaonn
	case 39: return 17; // NC1gammaonp
	case 41: return 18; // NC Multipi
	case 42: return 19; // NC1etaonn
	case 43: return 20; // NC1etaonp
	case 44: return 21; // NC1kamk0
	case 45: return 22; // NC1lamkp
	case 46: return 23; // NCDIS
	case 51: return 24; // NCEL on p
	case 52: return 25; // NCEL on n
	default: return 26; // Undefined
	}
};

void NuNuBarTrueModeStack::Fill(int species, int mode, double x, double y, double z, double weight) {
	int modeindex = ConvertModeToIndex(mode);
	int index = 54; // undefined

	if       (species == 12 or species == 14 or species == 16) index = modeindex;
	else if  (species == -12 or species == -14 or species == -16) index = modeindex + 27;

	StackBase::FillStack(index, x, y, z, weight);
};




// Species Stack Functions
BeamSpeciesStack::BeamSpeciesStack(std::string name, std::string title, TH1* hist) {
	fName = name;
	fTitle = title;

	// charged eptons
	AddMode(0, "electron", "e^{-}", kBlue, 2, 1001);
	AddMode(1, "positron", "e^{+}", kBlue - 2, 2, 3004);
	AddMode(2, "muon", "#mu^{-}", kRed, 2, 1001);
	AddMode(3, "antimuon", "#mu^{+}", kRed - 2, 2, 3004);
	AddMode(4, "tau", "#tau^{-}", kGreen, 2, 1001);
	AddMode(5, "antitau", "#tau^{+}", kGreen - 2, 2, 3004);

	// neutrinos
	AddMode(6, "nue", "#nu_e", kBlue, 2, 1001);
	AddMode(7, "antinue", "#bar{#nu}_e", kBlue - 2, 2, 3004);
	AddMode(8, "numu", "#nu_#mu", kRed, 2, 1001);
	AddMode(9, "antinumu", "#bar{#nu}_#mu", kRed - 2, 2, 3004);
	AddMode(10, "nutau", "#nu_#tau", kGreen, 2, 1001);
	AddMode(11, "antinutau", "#bar{#nu}_#tau", kGreen - 2, 2, 3004);

	StackBase::SetupStack(hist);
};

int BeamSpeciesStack::ConvertSpeciesToIndex(int species) {
	switch (species) {
	case 11:  return 0; // e-
	case -11: return 1; // e+
	case 13:  return 2; // mu-
	case -13: return 3; // mu+
	case 15: return 4; // tau-
	case -15: return 5; // tau+
	case 12: return 6; // nue
	case -12: return 7; // nuebar
	case 14: return 8; // numu
	case -14: return 9; // numubar
	case 16: return 10; // nutau
	case -16: return 11; //nutaubar
	}
};

void BeamSpeciesStack::FillStack(int species, double x, double y, double z, double weight) {
	StackBase::FillStack(ConvertSpeciesToIndex(species), x, y, z, weight);
}



// Target Stack Functions
TargetTypeStack::TargetTypeStack(std::string name, std::string title, TH1* hist){
	fName = name;
	fTitle = title;

	AddMode(0, "H", "Hydrogen", kBlue, 2, 1001);
	AddMode(1, "C", "Carbon", kRed, 2, 1001);
	AddMode(2, "O", "Oxygen", kViolet, 2, 1001);
	AddMode(3, "UNDEFINED", "Undefined", kBlack, 2, 1001 );

	StackBase::SetupStack(hist);
}

void TargetTypeStack::FillStack(int pdg, double x, double y, double z, double weight){
	int index = ConvertPDGToIndex(pdg);
	StackBase::FillStack(index, x, y, z, weight);
}

int TargetTypeStack::ConvertPDGToIndex(int pdg){
	switch(pdg){
		case 1000010010: return 0; // H
		case 1000060120: return 1; // C
		case 1000080160: return 2; // O
		default: return 3; // Undef
	}
}

bool TargetTypeStack::IncludeInStack(TH1* hist){
	return (hist->Integral() > 0.0);
}




// CC Topology Stack Functions
CCTopologyStack::CCTopologyStack(std::string name, std::string title, TH1* hist) {
	fName = name;
	fTitle = title;

	AddMode(0, "CC0pi", "CC-0#pi", kBlue, 2, 1001);
	AddMode(1, "CC1pip", "CC-1#pi^{+}", kRed, 2, 1001);
	AddMode(2, "CC1pim", "CC-1#pi^{-}", kGreen, 2, 1001);
	AddMode(3, "CC1pi0", "CC-1#pi^{0}", kYellow, 2, 1001);
	AddMode(4, "CCNpi", "CC-N#pi", kGray, 2, 1001);
	AddMode(5, "CCOther", "CC-Other", kViolet, 2, 1001);
	AddMode(6, "NC",  "NC", kMagenta, 2, 1001);
	AddMode(7, "UNDEFINED", "Undefined", kBlack, 2, 1001);


}

void CCTopologyStack::FillStack(FitEvent* evt, double x, double y, double z, double weight) {
	int index = GetIndexFromEventParticles(evt);
	StackBase::FillStack(index, x, y, z, weight);
}

int CCTopologyStack::GetIndexFromEventParticles(FitEvent* evt) {

	int nleptons = evt->NumFSLeptons();
	int npiplus  = evt->NumFSParticle(211);
	int npineg   = evt->NumFSParticle(-211);
	int npi0     = evt->NumFSParticle(111);
	int npions = npiplus + npineg + npi0;

	if (nleptons == 1) {
		if (npions == 0) {
			return 0; // CC0pi
		} else if (npions == 1) {
			if (npiplus == 1) return 1; //CC1pi+
			else if (npineg == 1) return 2; //CC1pi-
			else if (npi0 == 1) return 3; //CC1pi0
		} else if (npions > 1) {
			return 4; // CCNpi
		}
	} else if (nleptons > 1) {
		return 5; // CCOther
	} else if (nleptons < 1) {
		return 6;
	}

	return 7; // Undefined?

}











