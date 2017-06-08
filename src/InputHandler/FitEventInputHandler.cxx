#include "FitEventInputHandler.h"

FitEventInputHandler::FitEventInputHandler(std::string const& handle, std::string const& rawinputs) {
    LOG(SAM) << "Creating FitEventInputHandler : " << handle << std::endl;

    // Run a joint input handling
    fName = handle;
    fFitEventTree = new TChain("nuisance_events");
    fCacheSize = FitPar::Config().GetParI("CacheSize");

    std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
    for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {


        // Open File for histogram access
        TFile* inp_file = new TFile(inputs[inp_it].c_str(), "READ");
        if (!inp_file or inp_file->IsZombie()) {
            ERR(FTL) << "FitEvent File IsZombie() at " << inputs[inp_it] << std::endl;
            throw;
        }

        // Get Flux/Event hist
        TH1D* fluxhist  = (TH1D*)inp_file->Get("nuisance_fluxhist");
        TH1D* eventhist = (TH1D*)inp_file->Get("nuisance_eventhist");
        if (!fluxhist or !eventhist) {
            ERR(FTL) << "FitEvent FILE doesn't contain flux/xsec info" << std::endl;
            throw;
        }

        // Get N Events
        TTree* eventtree = (TTree*)inp_file->Get("nuisance_events");
        if (!eventtree) {
            ERR(FTL) << "nuisance_events not located in GENIE file! " << inputs[inp_it] << std::endl;
            throw;
        }
        int nevents = eventtree->GetEntries();

        // Register input to form flux/event rate hists
        RegisterJointInput(inputs[inp_it], nevents, fluxhist, eventhist);

        // Add to TChain
        fFitEventTree->Add( inputs[inp_it].c_str() );
    }

    // Registor all our file inputs
    SetupJointInputs();

    // Assign to tree
    fEventType = kINPUTFITEVENT;

    // Create Fit Event
    fNUISANCEEvent = new FitEvent();
    fNUISANCEEvent->HardReset();
    fNUISANCEEvent->SetBranchAddress(fFitEventTree);

}

FitEventInputHandler::~FitEventInputHandler(){
    if (fFitEventTree) delete fFitEventTree;
}

void FitEventInputHandler::CreateCache() {
    fFitEventTree->SetCacheEntryRange(0, fNEvents);
    fFitEventTree->AddBranchToCache("*", 1);
    fFitEventTree->SetCacheSize(fCacheSize);
}

void FitEventInputHandler::RemoveCache() {
    fFitEventTree->SetCacheEntryRange(0, fNEvents);
    fFitEventTree->AddBranchToCache("*", 0);
    fFitEventTree->SetCacheSize(0);
}

FitEvent* FitEventInputHandler::GetNuisanceEvent(const UInt_t entry, const bool lightweight) {

    // Return NULL if out of bounds
    if (entry >= (UInt_t)fNEvents) return NULL;

    // Reset all variables before tree read
    fNUISANCEEvent->ResetEvent();

    // Read NUISANCE Tree
    fFitEventTree->GetEntry(entry);

    // Setup Input scaling for joint inputs
    fNUISANCEEvent->InputWeight = GetInputWeight(entry);

    return fNUISANCEEvent;
}


double FitEventInputHandler::GetInputWeight(int entry) {
    double w = InputHandlerBase::GetInputWeight(entry);
    return w * fNUISANCEEvent->SavedRWWeight;
}

void FitEventInputHandler::Print() {
}


