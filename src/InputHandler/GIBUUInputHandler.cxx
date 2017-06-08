#ifdef __GiBUU_ENABLED__
#include "GIBUUInputHandler.h"

GIBUUGeneratorInfo::~GIBUUGeneratorInfo() {
	DeallocateParticleStack();
}

void GIBUUGeneratorInfo::AddBranchesToTree(TTree * tn) {
	// tn->Branch("NEUTParticleN",          fNEUTParticleN,          "NEUTParticleN/I");
	// tn->Branch("NEUTParticleStatusCode", fNEUTParticleStatusCode, "NEUTParticleStatusCode[NEUTParticleN]/I");
	// tn->Branch("NEUTParticleAliveCode",  fNEUTParticleAliveCode,  "NEUTParticleAliveCode[NEUTParticleN]/I");
}

void GIBUUGeneratorInfo::SetBranchesFromTree(TTree* tn) {
	// tn->SetBranchAddress("NEUTParticleN",          &fNEUTParticleN );
	// tn->SetBranchAddress("NEUTParticleStatusCode", &fNEUTParticleStatusCode );
	// tn->SetBranchAddress("NEUTParticleAliveCode",  &fNEUTParticleAliveCode  );

}

void GIBUUGeneratorInfo::AllocateParticleStack(int stacksize) {
	// fNEUTParticleN = 0;
	// fNEUTParticleStatusCode = new int[stacksize];
	// fNEUTParticleStatusCode = new int[stacksize];
}

void GIBUUGeneratorInfo::DeallocateParticleStack() {
	// delete fNEUTParticleStatusCode;
	// delete fNEUTParticleAliveCode;
}

void GIBUUGeneratorInfo::FillGeneratorInfo(GiBUUStdHepReader* nevent) {
	Reset();
	// for (int i = 0; i < nevent->Npart(); i++) {
	// fNEUTParticleStatusCode[i] = nevent->PartInfo(i)->fStatus;
	// fNEUTParticleAliveCode[i]  = nevent->PartInfo(i)->fIsAlive;
	// fNEUTParticleN++;
	// }
}

void GIBUUGeneratorInfo::Reset() {
	// for (int i = 0; i < fNEUTParticleN; i++) {
	// fNEUTParticleStatusCode[i] = -1;
	// fNEUTParticleAliveCode[i]  = 9;
	// }
	// fNEUTParticleN = 0;
}

GIBUUInputHandler::GIBUUInputHandler(std::string const& handle, std::string const& rawinputs) {
	LOG(SAM) << "Creating GiBUUInputHandler : " << handle << std::endl;

	// Run a joint input handling
	fName = handle;
	fEventType = kGiBUU;

	// Open Root File
	LOG(SAM) << "Opening event file " << rawinputs << std::endl;
	TFile* rootFile = new TFile(rawinputs.c_str(), "READ");
	// Get flux histograms NEUT supplies
	TH1D* numuFlux = dynamic_cast<TH1D*>(rootFile->Get("numu_flux"));
	TH1D* numubFlux = dynamic_cast<TH1D*>(rootFile->Get("numub_flux"));
	TH1D* nueFlux = dynamic_cast<TH1D*>(rootFile->Get("nue_flux"));
	TH1D* nuebFlux = dynamic_cast<TH1D*>(rootFile->Get("nueb_flux"));
	std::vector<TH1D*> fFluxList;

	// Replace local pointers with NULL dir'd clones.
	if (numuFlux) {
		numuFlux = static_cast<TH1D*>(numuFlux->Clone());
		numuFlux->Scale(1.0 / numuFlux->Integral("width"));
		std::cout << "GiBUU Flux: numuFlux, Width integral = "
		          << numuFlux->Integral("width") << std::endl;
		numuFlux->SetDirectory(NULL);
		numuFlux->SetNameTitle(
		    (fName + "_numu_FLUX").c_str(),
		    (fName + "; E_{#nu} (GeV); #Phi_{#nu} (A.U.)").c_str());
		fFluxList.push_back(numuFlux);
	}
	if (numubFlux) {
		numubFlux = static_cast<TH1D*>(numubFlux->Clone());
		numubFlux->Scale(1.0 / numubFlux->Integral("width"));
		std::cout << "GiBUU Flux: numubFlux, Width integral = "
		          << numubFlux->Integral("width") << std::endl;
		numubFlux->SetDirectory(NULL);
		numubFlux->SetNameTitle(
		    (fName + "_numub_FLUX").c_str(),
		    (fName + "; E_{#nu} (GeV); #Phi_{#bar{#nu}} (A.U.)").c_str());
		fFluxList.push_back(numubFlux);
	}
	if (nueFlux) {
		nueFlux = static_cast<TH1D*>(nueFlux->Clone());
		nueFlux->Scale(1.0 / nueFlux->Integral("width"));
		std::cout << "GiBUU Flux: nueFlux, Width integral = "
		          << nueFlux->Integral("width") << std::endl;
		nueFlux->SetDirectory(NULL);
		nueFlux->SetNameTitle(
		    (fName + "_nue_FLUX").c_str(),
		    (fName + "; E_{#nu} (GeV); #Phi_{#nu} (A.U.)").c_str());
		fFluxList.push_back(nueFlux);
	}
	if (nuebFlux) {
		nuebFlux = static_cast<TH1D*>(nuebFlux->Clone());
		nuebFlux->Scale(1.0 / nuebFlux->Integral("width"));
		std::cout << "GiBUU Flux: nuebFlux, Width integral = "
		          << nuebFlux->Integral("width") << std::endl;
		nuebFlux->SetDirectory(NULL);
		nuebFlux->SetNameTitle(
		    (fName + "_nueb_FLUX").c_str(),
		    (fName + "; E_{#nu} (GeV); #Phi_{#bar{#nu}} (A.U.)").c_str());
		fFluxList.push_back(nuebFlux);
	}
	rootFile->Close();

	fGIBUUTree = new TChain("giRooTracker");
	fGIBUUTree->AddFile(rawinputs.c_str());

	fGiReader = new GiBUUStdHepReader();
	fGiReader->SetBranchAddresses(fGIBUUTree);

	bool IsNuBarDominant = false;
	size_t Found_nu = 0;
	size_t Found_nuMask = ((numuFlux ? 1 : 0) + (numubFlux ? 2 : 0) +
	                       (nueFlux ? 4 : 0) + (nuebFlux ? 8 : 0));

	static const char* specNames[] = {"numu", "numubar", "nue", "nuebar"};
	size_t nExpected = (Found_nuMask & (1 << 0)) + (Found_nuMask & (1 << 1)) +
	                   (Found_nuMask & (1 << 2)) + (Found_nuMask & (1 << 3));
	size_t nFound = 0;
	std::string expectStr = "";
	for (size_t sn_it = 0; sn_it < 4; ++sn_it) {
		if (Found_nuMask & (1 << sn_it)) {
			if (!nFound) {
				expectStr = "(";
			}
			expectStr += specNames[sn_it];
			nFound++;
			if (nFound == nExpected) {
				expectStr += ")";
			} else {
				expectStr += ", ";
			}
		}
	}

	LOG(SAM) << "Looking for dominant vector species in GiBUU file ("
	         << rawinputs << ") expecting to find: " << expectStr << std::endl;

	size_t maskHW = GeneralUtils::GetHammingWeight(Found_nuMask);
	if (maskHW > 2) {
		LOG(SAM) << "We are looking for more than two species... this will have to "
		         "loop through a large portion of the vector. Please be patient."
		         << std::endl;
	}

	double SpeciesWeights[] = {0, 0, 0, 0};
	Long64_t nevt = 0;
	fNEvents = fGIBUUTree->GetEntries();
	fNUISANCEEvent = new FitEvent();
	TH1D* flux = NULL;

	while ((Found_nu != Found_nuMask) && (nevt < fNEvents)) {
		if ((maskHW == 2) && flux) {  // If we have found the dominant one can
			// now guess the other
			size_t OtherBit = GeneralUtils::GetFirstOnBit(Found_nuMask - Found_nu);
			SpeciesWeights[OtherBit] = 1 - fGiReader->SpeciesWght;
			Found_nu += (1 << OtherBit);

			LOG(SAM) << "\tGuessing other species weight as we are only expecting "
			         "two species. Other species weight: "
			         << SpeciesWeights[OtherBit] << std::endl;
			continue;
		}

		this->GetNuisanceEvent(nevt++);
		FitParticle* isnu = fNUISANCEEvent->GetHMISParticle(PhysConst::pdg_neutrinos);
		if (!isnu) {
			continue;
		}
		switch (isnu->fPID) {
		case 12: {
			if ((Found_nu & 4)) {
				continue;
			}
			Found_nu += 4;
			SpeciesWeights[2] = fGiReader->SpeciesWght;
			LOG(SAM) << "\tGiBUU File: " << rawinputs << " -- ev: " << nevt
			         << " has IS nu (" << isnu->fPID
			         << "), species weight: " << fGiReader->SpeciesWght << std::endl;
			if ((fGiReader->SpeciesWght < 0.5) &&
			        (maskHW > 1)) {  // If we only care about a single species, then
				// species-weight might not be filled.
				continue;
			}
			flux = nueFlux;
			LOG(SAM) << "\tInput file: " << rawinputs
			         << " determined to be nue dominated vector." << std::endl;
			break;
		}
		case -12: {
			if ((Found_nu & 8)) {
				continue;
			}
			Found_nu += 8;
			SpeciesWeights[3] = fGiReader->SpeciesWght;
			LOG(SAM) << "\tGiBUU File: " << rawinputs << " -- ev: " << nevt
			         << " has IS nu (" << isnu->fPID
			         << "), species weight: " << fGiReader->SpeciesWght << std::endl;
			if ((fGiReader->SpeciesWght < 0.5) &&
			        (maskHW > 1)) {  // If we only care about a single species, then
				// species-weight might not be filled.
				continue;
			}
			IsNuBarDominant = true;
			flux = nuebFlux;
			LOG(SAM) << "\tInput file: " << rawinputs
			         << " determined to be nuebar dominated vector." << std::endl;
			break;
		}
		case 14: {
			if ((Found_nu & 1)) {
				continue;
			}
			Found_nu += 1;
			SpeciesWeights[0] = fGiReader->SpeciesWght;
			LOG(SAM) << "\tGiBUU File: " << rawinputs << " -- ev: " << nevt
			         << " has IS nu (" << isnu->fPID
			         << "), species weight: " << fGiReader->SpeciesWght << std::endl;
			if ((fGiReader->SpeciesWght < 0.5) &&
			        (maskHW > 1)) {  // If we only care about a single species, then
				// species-weight might not be filled.
				continue;
			}
			flux = numuFlux;
			LOG(SAM) << "\tInput file: " << rawinputs
			         << " determined to be numu dominated vector." << std::endl;
			break;
		}
		case -14: {
			if ((Found_nu & 2)) {
				continue;
			}
			Found_nu += 2;
			SpeciesWeights[1] = fGiReader->SpeciesWght;
			LOG(SAM) << "\tGiBUU File: " << rawinputs << " -- ev: " << nevt
			         << " has IS nu (" << isnu->fPID
			         << "), species weight: " << fGiReader->SpeciesWght << std::endl;
			if ((fGiReader->SpeciesWght < 0.5) &&
			        (maskHW > 1)) {  // If we only care about a single species, then
				// species-weight might not be filled.
				continue;
			}
			IsNuBarDominant = true;
			flux = numubFlux;
			LOG(SAM) << "\tInput file: " << rawinputs
			         << " determined to be numubar dominated vector." << std::endl;
			break;
		}
		default: {}
		}
	}

	if (Found_nu != Found_nuMask) {
		ERR(FTL) << "Input GiBUU file (" << rawinputs
		         << ") appeared to not contain all the relevant incoming neutrino "
		         "species: Found (numu:"
		         << ((Found_nu & (1 << 0)) ? 1 : 0)
		         << ",numub:" << ((Found_nu & (1 << 1)) ? 1 : 0)
		         << ",nue:" << ((Found_nu & (1 << 2)) ? 1 : 0)
		         << ",nueb:" << ((Found_nu & (1 << 3)) ? 1 : 0)
		         << "), expected: (numu:" << ((Found_nuMask & (1 << 0)) ? 1 : 0)
		         << ",numub:" << ((Found_nuMask & (1 << 1)) ? 1 : 0)
		         << ",nue:" << ((Found_nuMask & (1 << 2)) ? 1 : 0)
		         << ",nueb:" << ((Found_nuMask & (1 << 3)) ? 1 : 0) << ")"
		         << std::endl;
		throw;
	}

	if (!flux) {
		ERR(FTL) << "Couldn't find: "
		         << (IsNuBarDominant ? "nuXb_flux" : "nuX_flux")
		         << " in input file: " << rootFile->GetName() << std::endl;
		throw;
	}

	if (numuFlux) {
		if ((maskHW > 1) && !GeneralUtils::IsSmallNum(SpeciesWeights[0])) {
			numuFlux->Scale(SpeciesWeights[0]);
		}

		TH1D* numuEvt =
		    static_cast<TH1D*>(numuFlux->Clone((fName + "_numu_EVT").c_str()));
		numuEvt->Reset();
		numuEvt->SetBinContent(1, SpeciesWeights[0] * double(fNEvents) /
		                       numuEvt->GetXaxis()->GetBinWidth(1));

		TH1D* numuXSec =
		    static_cast<TH1D*>(numuEvt->Clone((fName + "_numu_XSEC").c_str()));
		numuXSec->Divide(flux);

		numuXSec->SetTitle((fName + "; E_{#nu} (GeV);XSec").c_str());
	}
	if (numubFlux) {
		if ((maskHW > 1) && !GeneralUtils::IsSmallNum(SpeciesWeights[1])) {
			numubFlux->Scale(SpeciesWeights[1]);
		}
		TH1D* numubEvt =
		    static_cast<TH1D*>(numubFlux->Clone((fName + "_numub_EVT").c_str()));
		numubEvt->Reset();
		numubEvt->SetBinContent(1, SpeciesWeights[1] * double(fNEvents) /
		                        numubEvt->GetXaxis()->GetBinWidth(1));

		TH1D* numubXSec =
		    static_cast<TH1D*>(numubEvt->Clone((fName + "_numub_XSEC").c_str()));
		numubXSec->Divide(flux);

		numubXSec->SetTitle((fName + "; E_{#nu} (GeV);XSec").c_str());
	}
	if (nueFlux) {
		if ((maskHW > 1) && !GeneralUtils::IsSmallNum(SpeciesWeights[2])) {
			nueFlux->Scale(SpeciesWeights[2]);
		}
		TH1D* nueEvt =
		    static_cast<TH1D*>(nueFlux->Clone((fName + "_nue_EVT").c_str()));
		nueEvt->Reset();
		nueEvt->SetBinContent(1, SpeciesWeights[2] * double(fNEvents) /
		                      nueEvt->GetXaxis()->GetBinWidth(1));

		TH1D* nueXSec =
		    static_cast<TH1D*>(nueEvt->Clone((fName + "_nue_XSEC").c_str()));
		nueXSec->Divide(flux);

		nueXSec->SetTitle((fName + "; E_{#nu} (GeV);XSec").c_str());
	}
	if (nuebFlux) {
		if ((maskHW > 1) && !GeneralUtils::IsSmallNum(SpeciesWeights[3])) {
			nuebFlux->Scale(SpeciesWeights[3]);
		}
		TH1D* nuebEvt =
		    static_cast<TH1D*>(nuebFlux->Clone((fName + "_nueb_EVT").c_str()));
		nuebEvt->Reset();
		nuebEvt->SetBinContent(1, SpeciesWeights[3] * double(fNEvents) /
		                       nuebEvt->GetXaxis()->GetBinWidth(1));

		TH1D* nuebXSec =
		    static_cast<TH1D*>(nuebEvt->Clone((fName + "_nueb_XSEC").c_str()));
		nuebXSec->Divide(flux);

		nuebXSec->SetTitle((fName + "; E_{#nu} (GeV);XSec").c_str());
	}

	fGIBUUTree->GetEntry(0);

	LOG(SAM) << "\tInput GiBUU file species weights: (numu:" << SpeciesWeights[0]
	         << ",numub:" << SpeciesWeights[1] << ",nue:" << SpeciesWeights[2]
	         << ",nueb:" << SpeciesWeights[3] << ")" << std::endl;

	flux->SetNameTitle(
	    (fName + "_FLUX").c_str(),
	    (fName + "; E_{#nu} (GeV);" +
	     (IsNuBarDominant ? "#Phi_{#bar{#nu}} (A.U.)" : "#Phi_{#nu} (A.U.)"))
	    .c_str());

	TH1D* eventhist = static_cast<TH1D*>(flux->Clone((fName + "_EVT").c_str()));
	eventhist->Reset();
	eventhist->SetBinContent(1, double(fNEvents) *
	                         TotalIntegratedFlux(0, 1.E5, "width") /
	                         fEventHist->GetXaxis()->GetBinWidth(1));

	TH1D* xsechist = static_cast<TH1D*>(eventhist->Clone((fName + "_XSEC").c_str()));
	xsechist->Divide(flux);

	xsechist->SetTitle((fName + "; E_{#nu} (GeV);XSec").c_str());

	RegisterJointInput(rawinputs, fNEvents, flux, eventhist);
	SetupJointInputs();


};


FitEvent* GIBUUInputHandler::GetNuisanceEvent(const UInt_t entry, const bool lightweight) {

	// Check out of bounds
	if (entry >= (UInt_t)fNEvents) return NULL;

	// Read Entry from TTree to fill NEUT Vect in BaseFitEvt;
	fGIBUUTree->GetEntry(entry);

	// Run NUISANCE Vector Filler
	if (!lightweight) {
		CalcNUISANCEKinematics();
	}

	return fNUISANCEEvent;
}

int GIBUUInputHandler::GetGIBUUParticleStatus(int status, int pdg) {
	int state = kUndefinedState;
	switch (status) {
	case 0:   // Incoming
	case 11:  // Struck nucleon
		state = kInitialState;
		break;

	case 1:  // Good Final State
		state = kFinalState;
		break;

	default:  // Other
		break;
	}

	// Set Nuclear States Flag
	if (pdg > 1000000) {
		if (state == kInitialState)
			state = kNuclearInitial;
		else if (state == kFinalState)
			state = kNuclearRemnant;
		else
			state = kUndefinedState;
	}

	return state;
}

void GIBUUInputHandler::CalcNUISANCEKinematics() {

	// Reset all variables
	fNUISANCEEvent->ResetEvent();
	FitEvent* evt = fNUISANCEEvent;
	evt->fMode = fGiReader->GiBUU2NeutCode;
	evt->Mode = evt->fMode;
	evt->fEventNo = 0.0;
	evt->fTotCrs = 0;
	evt->fTargetA = 0.0; // Change to get these from nuclear remnant.
	evt->fTargetZ = 0.0;
	evt->fTargetH = 0;
	evt->fBound = 0.0;

	// Extra GiBUU Input Weight
	evt->InputWeight = fGiReader->EvtWght;

	// Check Stack N
	int npart = fGiReader->StdHepN;
	int kmax = evt->kMaxParticles;
	if ((UInt_t)npart > (UInt_t)kmax) {
		ERR(FTL) << "GiBUU has too many particles. Expanding Stack." << std::endl;
		fNUISANCEEvent->ExpandParticleStack(npart);
	}

	// Create Stack
	evt->fNParticles = 0;
	for (int i = 0; i < npart; i++) {

		// State
		int state = GetGIBUUParticleStatus(fGiReader->StdHepStatus[i],
		                                   fGiReader->StdHepPdg[i]);
		int curpart = evt->fNParticles;

		// Set State
		evt->fParticleState[evt->fNParticles] = state;

		// Mom
		evt->fParticleMom[curpart][0] = fGiReader->StdHepP4[i][0] * 1.E3;
		evt->fParticleMom[curpart][1] = fGiReader->StdHepP4[i][1] * 1.E3;
		evt->fParticleMom[curpart][2] = fGiReader->StdHepP4[i][2] * 1.E3;
		evt->fParticleMom[curpart][3] = fGiReader->StdHepP4[i][3] * 1.E3;

		// PDG
		evt->fParticlePDG[curpart] = fGiReader->StdHepPdg[i];

		// Add to total particles
		evt->fNParticles++;
	}

	// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent->OrderStack();
	return;
}

void GIBUUInputHandler::Print() {}
#endif

