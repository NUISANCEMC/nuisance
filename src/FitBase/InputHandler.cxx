// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#include "InputHandler.h"
#include <csignal>

//****************************************************************************
InputHandler::InputHandler(std::string const& handle,
                           InputUtils::InputType inpType,
                           std::string const& inputs) {
  //****************************************************************************

  LOG(SAM) << "Creating InputHandler for " << handle << "..." << std::endl;
  LOG(SAM) << " -> [" << inputs << "]" << std::endl;

  // Initial Setup
  fMaxEvents = FitPar::Config().GetParI("input.maxevents");
  fIsExplicitJointInput = (inpType == InputUtils::kJOINT_Input);
  fIsJointInput = fIsExplicitJointInput || InputUtils::IsJointInput(inputs);
  fInputType = inpType;
  fEvent = new FitEvent();
  fSignalEvent = new BaseFitEvt();
  fInput = inputs;
  fInputFile = InputUtils::ExpandInputDirectories(inputs);
  fName = handle;

  LOG(SAM) << " -> Type  = " << fInputType
           << (fIsJointInput ? " (Composite)" : "") << std::endl;
  LOG(SAM) << " -> Input = " << fInputFile << std::endl;

  // Automatically check what sort of event file it is
  if (fIsJointInput) {
    ReadJointFile();
  } else {
    fInputRootFile = new TFile(fInputFile.c_str(), "READ");
    switch (fInputType) {
      // Setup the handler for each type
      case InputUtils::kNEUT_Input: {
        ReadNeutFile();
        break;
      }
      case InputUtils::kNUWRO_Input: {
        ReadNuWroFile();
        break;
      }
      case InputUtils::kGENIE_Input: {
        ReadGenieFile();
        break;
      }
      case InputUtils::kGiBUU_Input: {
        ReadGiBUUFile();
        break;
      }
      case InputUtils::kHIST_Input: {
        ReadHistogramFile();
        break;
      }
      case InputUtils::kBNSPLN_Input: {
        ReadBinSplineFile();
        break;
      }
      case InputUtils::kEVSPLN_Input: {
        ReadNewSplines();
        break;
      }
      case InputUtils::kNUANCE_Input: {
        ReadNuanceFile();
        break;
      }
      case InputUtils::kEMPTY_Input: {
        ReadEmptyEvents();  // For Validation
        break;
      }
      case InputUtils::kFEVENT_Input: {
        ReadFitEvents();
        break;
      }
      default: {
        LOG(FTL) << " -> ERROR: Invalid Event File Type" << std::endl;
        fInputRootFile->ls();
        throw;
      }
    }
  }

  // Setup MaxEvents After setup of ttree
  if (fMaxEvents > 1 && fMaxEvents < fNEvents) {
    LOG(SAM) << " -> Reading only " << fMaxEvents << " events from total."
             << std::endl;
    fNEvents = fMaxEvents;
  }

  fFluxList.push_back(fFluxHist);
  fEventList.push_back(this->fEventHist);
  fXSecList.push_back(this->fXSecHist);

  LOG(SAM) << " -> Finished handler initialisation." << std::endl;
  return;
};

//********************************************************************
bool InputHandler::CanIGoFast() {
  //********************************************************************

  if (fEventType == 6) {
    return true;
  }
  return false;
}

//********************************************************************
void InputHandler::ReadFitEvents() {
  //********************************************************************

  fEventType = kINPUTFITEVENT;

  fFluxHist = (TH1D*)fInputRootFile->Get("nuisance_fluxhist");
  fFluxHist->SetNameTitle((fName + "_FLUX").c_str(),
                          (fName + "; E_{#nu} (GeV)").c_str());

  fEventHist = (TH1D*)fInputRootFile->Get("nuisance_eventhist");
  fEventHist->SetNameTitle((fName + "_EVT").c_str(),
                           (fName + "; E_{#nu} (GeV); Event Rate").c_str());

  fXSecHist = (TH1D*)fEventHist->Clone();
  fXSecHist->Divide(fFluxHist);
  fXSecHist->SetNameTitle(
      (fName + "_XSEC").c_str(),
      (fName + "_XSEC;E_{#nu} (GeV); XSec (1#times10^{-38} cm^{2})").c_str());

  tn = new TChain("nuisance_events", "");
  tn->Add(Form("%s/nuisance_events", fInputFile.c_str()));

  // Assign nvect
  fNEvents = tn->GetEntries();
  fEvent->SetBranchAddress(tn);

  // Print out what was read in
  LOG(SAM) << " -> Successfully Read FEvent file" << std::endl;

  return;
}

//********************************************************************
void InputHandler::ReadNewSplines(){
//********************************************************************

  fEventType = kNEWSPLINE;

  fFluxHist = (TH1D*)fInputRootFile->Get("nuisance_fluxhist");
  fFluxHist->SetNameTitle((fName + "_FLUX").c_str(),
                          (fName + "; E_{#nu} (GeV)").c_str());

  fEventHist = (TH1D*)fInputRootFile->Get("nuisance_eventhist");
  fEventHist->SetNameTitle((fName + "_EVT").c_str(),
                           (fName + "; E_{#nu} (GeV); Event Rate").c_str());

  fXSecHist = (TH1D*)fEventHist->Clone();
  fXSecHist->Divide(fFluxHist);
  fXSecHist->SetNameTitle(
			  (fName + "_XSEC").c_str(),
			  (fName + "_XSEC;E_{#nu} (GeV); XSec (1#times10^{-38} cm^{2})").c_str());

  tn = new TChain("nuisance_events", "");
  tn->Add(Form("%s/nuisance_events", fInputFile.c_str()));

  fNEvents = tn->GetEntries();
  fEvent->SetType(kEVTSPLINE);
  fEvent->SetBranchAddress(tn);

  // Setup the reader and spline coeff-cients
  //  fEvent->fSplineReader = new SplineReader(FitBase::GetRW());
  //fEvent->fSplineReader->Read((TTree*)fInputRootFile->Get("spline_reader"));

  //  fSplineTree = (TTree*)fInputRootFile->Get("spline_coeff");
  //  fSplineTree->SetBranchAddress("coeff",&fEvent->fSplineCoeff);
  //  tn->AddFriend(fSplineTree);
  
}

//********************************************************************
void InputHandler::ReadEmptyEvents() {
  //********************************************************************

  fEventType = kEMPTY;

  // Set flux histograms to empty
  fFluxHist = new TH1D((fName + "_FLUX").c_str(),
                       (fName + "_FLUX;E_{#nu};Flux").c_str(), 1, 0.0, 1.0);
  fFluxHist->SetBinContent(1, 1);

  // Set Event Hist to empty
  fEventHist = new TH1D((fName + "_EVT").c_str(),
                        (fName + "_EVT;E_{#nu};Flux").c_str(), 1, 0.0, 1.0);
  fEventHist->SetBinContent(1, 1);

  // Set XSec hist to empty
  fXSecHist = new TH1D((fName + "_XSEC").c_str(),
                       (fName + "_XSEC;E_{#nu};XSec").c_str(), 1, 0.0, 1.0);
  fXSecHist->SetBinContent(1, 1);

  fNEvents = 0;
}

//********************************************************************
void InputHandler::ReadEventSplineFile() {
  //********************************************************************

  LOG(SAM) << " -> Setting up SPLINE inputs" << std::endl;

  // Event Type 7 SPLINES
  fEventType = 6;

  fFluxHist = (TH1D*)fInputRootFile->Get("FitFluxHist");
  fFluxHist->SetNameTitle((fName + "_FLUX").c_str(),
                          (fName + "; E_{#nu} (GeV)").c_str());

  fEventHist = (TH1D*)fInputRootFile->Get("FitEventHist");
  fEventHist->SetNameTitle((fName + "_EVT").c_str(),
                           (fName + "; E_{#nu} (GeV); Event Rate").c_str());

  fXSecHist = (TH1D*)fEventHist->Clone();
  fXSecHist->Divide(fFluxHist);
  fXSecHist->SetNameTitle(
      (fName + "_XSEC").c_str(),
      (fName + "_XSEC;E_{#nu} (GeV); XSec (1#times10^{-38} cm^{2})").c_str());

  tn = new TChain("FitEvents", "");
  tn->Add(Form("%s/FitEvents", fInputFile.c_str()));

  // Setup Spline Stuff
  fSplineHead = new FitSplineHead(fInputRootFile, "FitSplineHead");

  // Assign nvect
  fNEvents = tn->GetEntries();
  fEvent->SetBranchAddress(tn);
  fEvent->SetSplineCoeffAddress(tn);
  fEvent->SetType(kEVTSPLINE);

  // Print out what was read in
  LOG(SAM) << " -> Successfully Read FEvent file" << std::endl;

  // Load Dial Coeffs into vector
  tn->GetEntry(0);
  int ncoeff = fEvent->GetNCoeff();
  fSplineArray = new double*[fNEvents];
  for (int i = 0; i < fNEvents; i++) {
    tn->GetEntry(i);

    // Copy Splines over
    fSplineArray[i] = new double[ncoeff];
    for (int j = 0; j < fEvent->GetNCoeff(); j++) {
      fSplineArray[i][j] = fEvent->GetCoeff(j);
    }
  }

  LOG(DEB) << "Loaded all spline coeffs" << endl;

  // Set input.maxevents CALC Here before we load in splines
  if (fMaxEvents > 1 and fMaxEvents < fNEvents) {
    LOG(SAM) << " -> Reading only " << fMaxEvents
             << " events from total spline events." << std::endl;
    fNEvents = fMaxEvents;
  }

  // Print out what was read in
  LOG(SAM) << " -> Successfully Read SPLINE file" << std::endl;
  if (LOG_LEVEL(SAM)) PrintStartInput();

  int cnt = 1;
  std::list<FitSpline*>::iterator spl_iter =
      this->fSplineHead->SplineObjects.begin();
  for (; spl_iter != this->fSplineHead->SplineObjects.end(); spl_iter++) {
    FitSpline* spl = (*spl_iter);

    LOG(SAM) << " -> Spline " << cnt << ". " << spl->id << " " << spl->form
             << " "
             << "NDIM(" << spl->ndim << ") "
             << "NPAR(" << spl->npar << ") "
             << "PTS(" << spl->points << ") " << std::endl;
    cnt++;
  }
}

//********************************************************************
FitSplineHead* InputHandler::GetSplineHead() {
  //********************************************************************
  return fSplineHead;
}

//********************************************************************
void InputHandler::SetupCache() {
  //********************************************************************
  tn->SetCacheSize(FitPar::Config().GetParI("cachesize"));
  tn->AddBranchToCache("*", kTRUE);
  tn->StopCacheLearningPhase();
}

//********************************************************************
void InputHandler::ReadJointFile() {
  //********************************************************************
/*
  std::vector<std::string> inputs;

  if (fIsExplicitJointInput) {  // Included for backwards compatibility.
    std::string line;
    std::ifstream card(fInputFile.c_str(), ifstream::in);

    LOG(FIT) << "Parsing input card: \'" << fInputFile << "\'" << endl;
    while (std::getline(card >> std::ws, line, '\n')) {
      if (line.empty()) {
        continue;
      }

      std::vector<std::string> file_descriptor =
          GeneralUtils::ParseToStr(line, ":");

      if (file_descriptor.size() != 2) {
        ERR(FTL) << "Found JOINT card file line: \"" << line
                 << "\", expected \"INPUTTYPE:File.root\"." << std::endl;
        throw;
      }
      InputUtils::InputType inpType =
          InputUtils::ParseInputType(file_descriptor[0]);

      if (!inputs.size()) {
        fInputType = inpType;
        LOG(SAM) << " -> InputHandler type: " << fInputType << std::endl;
      } else if (inpType != fInputType) {
        ERR(FTL) << "First input type in JOINT card was: " << fInputType
                 << " but found: " << inpType
                 << ", all files in a JOINT must be the same." << std::endl;
        throw;
      }

      LOG(SAM) << "\t -> Found input file: " << file_descriptor[1] << std::endl;
      inputs.push_back(file_descriptor[1]);
    }
  } else {
    LOG(SAM) << " -> Parsing list of inputs for composite input." << std::endl;

    inputs = GeneralUtils::ParseToStr(fInputFile, ",");
    inputs.front() = inputs.front().substr(1);
    inputs.back() = inputs.back().substr(0, inputs.back().size() - 1);

    for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
      LOG(SAM) << "\t -> Found input file: " << inputs[inp_it] << std::endl;
    }
  }

  // Loop over input and get the flux files
  // Using a temporary input handler to do this, which is a bit dodge.
  int count_low = 0;
  int temp_EventType = kUNKNOWN;
  for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
    LOG(SAM) << "Creating temporary handler to read file: " << inputs.at(inp_it)
             << endl;

    // Create Temporary InputHandlers inside
    InputHandler temp_input(std::string(Form("temp_input_%li", inp_it)),
                            fInputType, inputs.at(inp_it));

    if (temp_EventType != temp_input.GetType() and inp_it > 0) {
      ERR(FTL) << "Can't use joint events with mismatched trees!" << std::endl;
      ERR(FTL) << "This should not have happened. Please report this as a bug "
                  "along with your input card file."
               << std::endl;
      throw;
    }

    LOG(FIT) << "Getting objects from " << temp_input.fInputFile << endl;
    temp_EventType = temp_input.GetType();

    TH1D* temp_flux = (TH1D*)temp_input.GetFluxHistogram()->Clone();
    TH1D* temp_evts = (TH1D*)temp_input.GetEventHistogram()->Clone();
    TH1D* temp_xsec = (TH1D*)temp_input.GetXSecHistogram()->Clone();
    int temp_events = temp_input.GetNEvents();

    temp_flux->SetName(
        (fName + "_" + temp_input.GetInputStateString() + "_FLUX").c_str());
    temp_evts->SetName(
        (fName + "_" + temp_input.GetInputStateString() + "_EVT").c_str());
    temp_xsec->SetName(
        (fName + "_" + temp_input.GetInputStateString() + "_XSEC").c_str());

    fFluxList.push_back(temp_flux);
    fEventList.push_back(temp_evts);
    fXSecList.push_back(temp_xsec);

    fJointIndexLow.push_back(count_low);
    fJointIndexHigh.push_back(count_low + temp_events);
    fJointIndexHist.push_back((TH1D*)temp_evts->Clone());

    count_low += temp_events;
    LOG(FIT) << "Temp input has " << temp_events << " events." << endl;

    if (inp_it == 0) {
      fFluxHist = (TH1D*)temp_flux->Clone();
      fEventHist = (TH1D*)temp_evts->Clone();
    } else {
      fFluxHist->Add(temp_flux);
      fEventHist->Add(temp_evts);
    }
    LOG(SAM) << "Added Input File " << inputs.at(inp_it) << std::endl
             << " which contained " << temp_events << " events." << std::endl;
  }

  // Now have all correctly normalised histograms all we need to do is setup the
  // TChains

  // Input Assumes all the same type
  std::string tree_name = "";
  if (temp_EventType == kNEUT) {
    tree_name = "neuttree";
  } else if (temp_EventType == kNUWRO) {
    tree_name = "treeout";
  } else if (temp_EventType == kGENIE) {
    tree_name = "gtree";
  }

  // Add up the TChains
  tn = new TChain(tree_name.c_str());
  for (UInt_t i = 0; i < inputs.size(); i++) {
    // PARSE INPUT
    LOG(DEB) << "Adding new tchain " << inputs.at(i) << std::endl;
    tn->Add(inputs.at(i).c_str());
  }

  // Setup Events
  fNEvents = tn->GetEntries();
  if (temp_EventType == kNEUT) {
#ifdef __NEUT_ENABLED__
    fEventType = kNEUT;
    fNeutVect = NULL;
    tn->SetBranchAddress("vectorbranch", &fNeutVect);
    //fEvent->SetEventAddress(&fNeutVect);
#endif
  } else if (temp_EventType == kNUWRO) {
#ifdef __NUWRO_ENABLED__
    fEventType = kNUWRO;
    fNuwroEvent = NULL;
    tn->SetBranchAddress("e", &fNuwroEvent);
    fEvent->SetEventAddress(&fNuwroEvent);
#endif
  } else if (temp_EventType == kGENIE) {
#ifdef __GENIE_ENABLED__
    fEventType = kGENIE;
    fGenieGHep = NULL;
    fGenieNtpl = NULL;
    tn->SetBranchAddress("gmcrec", &fGenieNtpl);
    //fEvent->SetEventAddress(&fGenieNtpl);
#endif
  }

  // Normalise event histogram PDFS for weights
  for (UInt_t i = 0; i < inputs.size(); i++) {
    TH1D* temp_hist = (TH1D*)fJointIndexHist.at(i)->Clone();
    fJointIndexScale.push_back(
        double(fNEvents) / fEventHist->Integral("width") *
        fJointIndexHist.at(i)->Integral("width") /
        double(fJointIndexHigh.at(i) - fJointIndexLow.at(i)));

    temp_hist->Scale(double(fNEvents) / fEventHist->Integral("width"));
    temp_hist->Scale(fJointIndexHist.at(i)->Integral("width") /
                     double(fJointIndexHigh.at(i)));

    fJointIndexHist.at(i) = temp_hist;
  }

  fEventHist->SetNameTitle((fName + "_EVT").c_str(), (fName + "_EVT").c_str());
  fFluxHist->SetNameTitle((fName + "_FLUX").c_str(), (fName + "_FLUX").c_str());

  fXSecHist = (TH1D*)fEventHist->Clone();
  fXSecHist->Divide(fFluxHist);
  fXSecHist->SetNameTitle((fName + "_XSEC").c_str(), (fName + "_XSEC").c_str());
*/
  return;
}

//********************************************************************
void InputHandler::ReadNeutFile() {
//********************************************************************
/*
#ifdef __NEUT_ENABLED__

  LOG(SAM) << " -> Setting up NEUT inputs" << std::endl;

  // Event Type 0 Neut
  fEventType = kNEUT;

  // Get flux histograms NEUT supplies
  fFluxHist = (TH1D*)fInputRootFile->Get(
      (PlotUtils::GetObjectWithName(fInputRootFile, "flux")).c_str());
  if (!fFluxHist) {
    ERR(FTL) << "No Flux Hist in NEUT ROOT file." << std::endl;
    throw;
  }
  fFluxHist->SetNameTitle((fName + "_FLUX").c_str(),
                          (fName + "; E_{#nu} (GeV)").c_str());

  fEventHist = (TH1D*)fInputRootFile->Get(
      (PlotUtils::GetObjectWithName(fInputRootFile, "evtrt")).c_str());
  if (!fEventHist) {
    ERR(FTL) << "No Event Hist in NEUT ROOT file." << std::endl;
    throw;
  }

  fEventHist->SetNameTitle((fName + "_EVT").c_str(),
                           (fName + "; E_{#nu} (GeV); Event Rate").c_str());

  fXSecHist = (TH1D*)fEventHist->Clone();
  fXSecHist->Divide(fFluxHist);
  fXSecHist->SetNameTitle(
      (fName + "_XSEC").c_str(),
      (fName + "_XSEC;E_{#nu} (GeV); XSec (1#times10^{-38} cm^{2})").c_str());

  // Read in the file once only
  tn = new TChain("neuttree", "");
  tn->Add(Form("%s/neuttree", fInputFile.c_str()));

  // Assign nvect
  fNEvents = tn->GetEntries();
  fNeutVect = NULL;
  tn->SetBranchAddress("vectorbranch", &fNeutVect);

  // Make the custom event read in nvect when calling CalcKinematics
  //fEvent->SetEventAddress(&fNeutVect);

  // Print out what was read in
  LOG(SAM) << " -> Successfully Read NEUT file" << std::endl;
  if (LOG_LEVEL(SAM)) {
    PrintStartInput();
  }

#else
  ERR(FTL) << "ERROR: Invalid Event File Provided" << std::endl;
  ERR(FTL) << "NEUT Input Not Enabled." << std::endl;
  ERR(FTL) << "Rebuild with --enable-neut or check FitBuild.h!" << std::endl;
  exit(-1);
#endif
*/
  return;
}

//********************************************************************
void InputHandler::ReadNuWroFile() {
//********************************************************************

/*
#ifdef __NUWRO_ENABLED__

  LOG(SAM) << " -> Setting up Nuwro inputs" << std::endl;

  // Event Type 1 == NuWro
  fEventType = kNUWRO;

  // Setup the TChain for nuwro event tree
  tn = new TChain("treeout");
  tn->AddFile(fInputFile.c_str());

  // Get entries and fNuwroEvent
  fNEvents = tn->GetEntries();
  fNuwroEvent = NULL;
  tn->SetBranchAddress("e", &fNuwroEvent);
  fEvent->SetEventAddress(&fNuwroEvent);

  // Check if we have saved an xsec histogram before
  fFluxHist = (TH1D*)fInputRootFile->Get(
      (PlotUtils::GetObjectWithName(fInputRootFile, "FluxHist")).c_str());
  fEventHist = (TH1D*)fInputRootFile->Get(
      (PlotUtils::GetObjectWithName(fInputRootFile, "EvtHist")).c_str());

  // Check if we are forcing plot generation (takes time)
  bool regenFlux = FitPar::Config().GetParB("input.regen_nuwro_plots");
  if (regenFlux)
    LOG(SAM)
        << " -> Forcing NuWro XSec/Flux plots to be generated at the start. "
        << std::endl;

  // Already generated flux and event histograms
  if (fFluxHist and fEventHist and !regenFlux) {
    fXSecHist = (TH1D*)fInputRootFile->Get(
        (PlotUtils::GetObjectWithName(fInputRootFile, "xsec")).c_str());

    fFluxHist->SetNameTitle((fName + "_FLUX").c_str(),
                            (fName + "_FLUX").c_str());
    fEventHist->SetNameTitle((fName + "_EVT").c_str(),
                             (fName + "_EVT").c_str());
    fXSecHist->SetNameTitle((fName + "_XSEC").c_str(),
                            (fName + "_XSEC").c_str());

    // Need to regenerate if not found
  } else {
    LOG(SAM)
        << " -> No NuWro XSec or Flux Histograms found, need to regenerate!"
        << std::endl;

    // Can grab flux histogram from the pars
    tn->GetEntry(0);

    int beamtype = fNuwroEvent->par.beam_type;

    if (beamtype == 0) {
      std::string fluxstring = fNuwroEvent->par.beam_energy;
      std::vector<double> fluxvals = GeneralUtils::ParseToDbl(fluxstring, " ");
      int pdg = fNuwroEvent->par.beam_particle;
      double Elow = double(fluxvals[0]) / 1000.0;
      double Ehigh = double(fluxvals[1]) / 1000.0;

      LOG(SAM) << " - Adding new nuwro flux "
               << "pdg: " << pdg << "Elow: " << Elow << "Ehigh: " << Ehigh
               << std::endl;

      fFluxHist =
          new TH1D("fluxplot", "fluxplot", fluxvals.size() - 2, Elow, Ehigh);
      for (UInt_t j = 2; j < fluxvals.size(); j++) {
        LOG(DEB) << "Flux bin:" << j << " = " << fluxvals[j] << endl;
        fFluxHist->SetBinContent(j - 1, fluxvals[j]);
      }
    } else if (beamtype == 1) {
      std::string fluxstring = fNuwroEvent->par.beam_content;

      std::vector<std::string> fluxlines =
          GeneralUtils::ParseToStr(fluxstring, "\n");
      for (UInt_t i = 0; i < fluxlines.size(); i++) {
        std::vector<double> fluxvals =
            GeneralUtils::ParseToDbl(fluxlines[i], " ");

        int pdg = int(fluxvals[0]);
        double pctg = double(fluxvals[1]) / 100.0;
        double Elow = double(fluxvals[2]) / 1000.0;
        double Ehigh = double(fluxvals[3]) / 1000.0;

        LOG(DEB) << " - Adding new nuwro flux "
                 << "pdg: " << pdg << "pctg: " << pctg << "Elow: " << Elow
                 << "Ehigh: " << Ehigh << std::endl;

        TH1D* fluxplot =
            new TH1D("fluxplot", "fluxplot", fluxvals.size() - 4, Elow, Ehigh);
        for (UInt_t j = 4; j < fluxvals.size(); j++) {
          fluxplot->SetBinContent(j + 1, fluxvals[j]);
        }

        if (fFluxHist)
          fFluxHist->Add(fluxplot);
        else
          fFluxHist = (TH1D*)fluxplot->Clone();
      }
    }

    fFluxHist->SetNameTitle("nuwro_flux", "nuwro_flux;E_{#nu} (GeV); Flux");

    fEventHist = (TH1D*)fFluxHist->Clone();
    fEventHist->Reset();
    fEventHist->SetNameTitle("nuwro_evt", "nuwro_evt");

    fXSecHist = (TH1D*)fFluxHist->Clone();
    fXSecHist->Reset();
    fXSecHist->SetNameTitle("nuwro_xsec", "nuwro_xsec");

    // Start Processing
    LOG(SAM) << " -> Processing NuWro Input Flux for " << fNEvents
             << " events (This can take a while...) " << std::endl;

    double Enu = 0.0;
    double TotXSec = 0.0;
    double totaleventmode = 0.0;
    double totalevents = 0.0;

    // --- loop
    for (int i = 0; i < fNEvents; i++) {
      tn->GetEntry(i);

      if (i % 100000 == 0) LOG(SAM) << " i " << i << std::endl;
      // Get Variables
      Enu = fNuwroEvent->in[0].E() / 1000.0;
      TotXSec = fNuwroEvent->weight;

      // Fill a flux and xsec histogram
      fEventHist->Fill(Enu);
      fXSecHist->Fill(Enu, TotXSec);

      // Keep Tally
      totaleventmode += TotXSec;
      totalevents++;
    }

    LOG(SAM) << " -> Flux Processing Loop Finished." << std::endl;

    if (fEventHist->Integral() == 0.0) {
      ERR(FTL) << "NO EVENTS FOUND IN RANGE! " << std::endl;
      exit(-1);
    }

    // Sort out plot scaling
    double AvgXSec = (totaleventmode * 1.0E38 / (totalevents + 0.));
    LOG(SAM) << " -> Average XSec = " << AvgXSec << std::endl;

    fEventHist->Scale(1.0 / fEventHist->Integral());  // Convert to PDF
    fEventHist->Scale(fFluxHist->Integral() *
                      AvgXSec);  // Convert to Proper Event Rate

    fXSecHist->Add(fEventHist);    // Get Event Rate Plot
    fXSecHist->Divide(fFluxHist);  // Make XSec Plot

    // fEventHist = (TH1D*)fFluxHist->Clone();
    // fEventHist->Multiply(fXSecHist);

    // Clear over/underflows incase they mess with integrals later.
    fFluxHist->SetBinContent(0, 0.0);
    fFluxHist->SetBinContent(fFluxHist->GetNbinsX() + 2, 0.0);

    fEventHist->SetBinContent(0, 0.0);
    fEventHist->SetBinContent(fEventHist->GetNbinsX() + 2, 0.0);

    LOG(SAM)
        << " -> Finished making NuWro event plots. Saving them for next time..."
        << std::endl;

    TFile* temp_save_file = new TFile(fInputFile.c_str(), "UPDATE");
    temp_save_file->cd();

    fFluxHist->Write("FluxHist", TObject::kOverwrite);
    fEventHist->Write("EventHist", TObject::kOverwrite);
    fXSecHist->Write("XSecHist", TObject::kOverwrite);
    temp_save_file->ls();

    temp_save_file->Close();
    delete temp_save_file;

    fFluxHist->SetNameTitle((fName + "_FLUX").c_str(),
                            (fName + "_FLUX").c_str());
    fEventHist->SetNameTitle((fName + "_EVT").c_str(),
                             (fName + "_EVT").c_str());
    fXSecHist->SetNameTitle((fName + "_XSEC").c_str(),
                            (fName + "_XSEC").c_str());
  }  // end regenerate histos

  // Print out what was read in
  LOG(SAM) << " -> Successfully Read NUWRO file" << std::endl;
  if (LOG_LEVEL(SAM)) PrintStartInput();

#else
  ERR(FTL) << "ERROR: Invalid Event File Provided" << std::endl;
  ERR(FTL) << "NuWro Input Not Enabled." << std::endl;
  ERR(FTL) << "Rebuild with --enable-nuwro or check FitBuild.h!" << std::endl;
  exit(-1);
#endif
*/
  return;
}

//********************************************************************
void InputHandler::ReadGenieFile() {
//********************************************************************
/*
#ifdef __GENIE_ENABLED__

  // Event Type 5 GENIE
  fEventType = kGENIE;

  // Open Root File
  LOG(SAM) << "Reading event file " << fInputFile << std::endl;

  // Get flux histograms NEUT supplies
  fFluxHist = (TH1D*)fInputRootFile->Get(
      (PlotUtils::GetObjectWithName(fInputRootFile, "nuisance_flux")).c_str());

  if (!fFluxHist) {
    ERR(FTL) << "GENIE FILE doesn't contain flux/xsec info" << std::endl;
    ERR(FTL) << "Run app/PrepareGENIE first" << std::endl;
    throw;
  }

  fFluxHist->SetNameTitle((fName + "_FLUX").c_str(),
                          (fName + "; E_{#nu} (GeV)").c_str());

  fEventHist = (TH1D*)fInputRootFile->Get(
      (PlotUtils::GetObjectWithName(fInputRootFile, "nuisance_events"))
          .c_str());
  fEventHist->SetNameTitle((fName + "_EVT").c_str(),
                           (fName + "; E_{#nu} (GeV); Event Rate").c_str());

  fXSecHist = (TH1D*)fInputRootFile->Get(
      (PlotUtils::GetObjectWithName(fInputRootFile, "nuisance_xsec")).c_str());
  fXSecHist->SetNameTitle((fName + "_XSEC").c_str(),
                          (fName + "; E_{#nu} (GeV); Event Rate").c_str());

  // Setup the TChain for GENIE event tree
  tn = new TChain("gtree");
  tn->AddFile(fInputFile.c_str());

  fNEvents = tn->GetEntries();
  StopTalking();
  fGenieGHep = NULL;
  fGenieNtpl = NULL;
  //  NtpMCEventRecord * fGenieNtpl = 0; tree->SetBranchAddress(gmrec,
  //  &fGenieNtpl);
  tn->SetBranchAddress("gmcrec", &fGenieNtpl);

  // Make the custom event read in nvect when calling CalcKinematics
  //fEvent->SetEventAddress(&fGenieNtpl);

  // Set Titles
  fEventHist->SetNameTitle(
      (fName + "_EVT").c_str(),
      (fName + "_EVT;E_{#nu} (GeV); Events (1#times10^{-38})").c_str());

  fXSecHist->SetNameTitle(
      (fName + "_XSEC").c_str(),
      (fName + "_XSEC;E_{#nu} (GeV); XSec (1#times10^{-38} cm^{2})").c_str());
  StartTalking();
  // Print out what was read in
  LOG(SAM) << " -> Successfully Read GENIE file" << std::endl;
  if (LOG_LEVEL(SAM)) PrintStartInput();

#else
  ERR(FTL) << "ERROR: Invalid Event File Provided" << std::endl;
  ERR(FTL) << "GENIE Input Not Enabled." << std::endl;
  ERR(FTL) << "Rebuild with --enable-genie or check FitBuild.h!" << std::endl;
  exit(-1);
#endif
*/
  return;
}

//********************************************************************
void InputHandler::ReadGiBUUFile() {
//********************************************************************
  /*
#ifdef __GiBUU_ENABLED__
  fEventType = kGiBUU;

  // Open Root File
  LOG(SAM) << "Opening event file " << fInputFile << std::endl;
  TFile* rootFile = new TFile(fInputFile.c_str(), "READ");
  // Get flux histograms NEUT supplies
  TH1D* numuFlux = dynamic_cast<TH1D*>(rootFile->Get("numu_flux"));
  TH1D* numubFlux = dynamic_cast<TH1D*>(rootFile->Get("numub_flux"));
  TH1D* nueFlux = dynamic_cast<TH1D*>(rootFile->Get("nue_flux"));
  TH1D* nuebFlux = dynamic_cast<TH1D*>(rootFile->Get("nueb_flux"));

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

  tn = new TChain("giRooTracker");
  tn->AddFile(fInputFile.c_str());

  GiBUUStdHepReader* giRead = new GiBUUStdHepReader();
  giRead->SetBranchAddresses(tn);
  fEvent->SetEventAddress(giRead);

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
           << fInputFile << ") expecting to find: " << expectStr << std::endl;

  size_t maskHW = GeneralUtils::GetHammingWeight(Found_nuMask);
  if (maskHW > 2) {
    LOG(SAM) << "We are looking for more than two species... this will have to "
                "loop through a large portion of the vector. Please be patient."
             << std::endl;
  }

  double SpeciesWeights[] = {0, 0, 0, 0};
  Long64_t nevt = 0;
  fNEvents = tn->GetEntries();
  fFluxHist = NULL;
  while ((Found_nu != Found_nuMask) && (nevt < fNEvents)) {
    if ((maskHW == 2) && fFluxHist) {  // If we have found the dominant one can
                                       // now guess the other
      size_t OtherBit = GeneralUtils::GetFirstOnBit(Found_nuMask - Found_nu);
      SpeciesWeights[OtherBit] = 1 - giRead->SpeciesWght;
      Found_nu += (1 << OtherBit);

      LOG(SAM) << "\tGuessing other species weight as we are only expecting "
                  "two species. Other species weight: "
               << SpeciesWeights[OtherBit] << std::endl;
      continue;
    }

    tn->GetEntry(nevt++);
    fEvent->CalcKinematics();
    FitParticle* isnu = fEvent->GetHMISParticle(PhysConst::pdg_neutrinos);
    if (!isnu) {
      continue;
    }
    switch (isnu->fPID) {
      case 12: {
        if ((Found_nu & 4)) {
          continue;
        }
        Found_nu += 4;
        SpeciesWeights[2] = giRead->SpeciesWght;
        LOG(SAM) << "\tGiBUU File: " << fInputFile << " -- ev: " << nevt
                 << " has IS nu (" << isnu->fPID
                 << "), species weight: " << giRead->SpeciesWght << std::endl;
        if ((giRead->SpeciesWght < 0.5) &&
            (maskHW > 1)) {  // If we only care about a single species, then
                             // species-weight might not be filled.
          continue;
        }
        fFluxHist = nueFlux;
        LOG(SAM) << "\tInput file: " << fInputFile
                 << " determined to be nue dominated vector." << std::endl;
        break;
      }
      case -12: {
        if ((Found_nu & 8)) {
          continue;
        }
        Found_nu += 8;
        SpeciesWeights[3] = giRead->SpeciesWght;
        LOG(SAM) << "\tGiBUU File: " << fInputFile << " -- ev: " << nevt
                 << " has IS nu (" << isnu->fPID
                 << "), species weight: " << giRead->SpeciesWght << std::endl;
        if ((giRead->SpeciesWght < 0.5) &&
            (maskHW > 1)) {  // If we only care about a single species, then
                             // species-weight might not be filled.
          continue;
        }
        IsNuBarDominant = true;
        fFluxHist = nuebFlux;
        LOG(SAM) << "\tInput file: " << fInputFile
                 << " determined to be nuebar dominated vector." << std::endl;
        break;
      }
      case 14: {
        if ((Found_nu & 1)) {
          continue;
        }
        Found_nu += 1;
        SpeciesWeights[0] = giRead->SpeciesWght;
        LOG(SAM) << "\tGiBUU File: " << fInputFile << " -- ev: " << nevt
                 << " has IS nu (" << isnu->fPID
                 << "), species weight: " << giRead->SpeciesWght << std::endl;
        if ((giRead->SpeciesWght < 0.5) &&
            (maskHW > 1)) {  // If we only care about a single species, then
                             // species-weight might not be filled.
          continue;
        }
        fFluxHist = numuFlux;
        LOG(SAM) << "\tInput file: " << fInputFile
                 << " determined to be numu dominated vector." << std::endl;
        break;
      }
      case -14: {
        if ((Found_nu & 2)) {
          continue;
        }
        Found_nu += 2;
        SpeciesWeights[1] = giRead->SpeciesWght;
        LOG(SAM) << "\tGiBUU File: " << fInputFile << " -- ev: " << nevt
                 << " has IS nu (" << isnu->fPID
                 << "), species weight: " << giRead->SpeciesWght << std::endl;
        if ((giRead->SpeciesWght < 0.5) &&
            (maskHW > 1)) {  // If we only care about a single species, then
                             // species-weight might not be filled.
          continue;
        }
        IsNuBarDominant = true;
        fFluxHist = numubFlux;
        LOG(SAM) << "\tInput file: " << fInputFile
                 << " determined to be numubar dominated vector." << std::endl;
        break;
      }
      default: {}
    }
  }

  if (Found_nu != Found_nuMask) {
    ERR(FTL) << "Input GiBUU file (" << fInputFile
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

  if (!fFluxHist) {
    ERR(FTL) << "Couldn't find: "
             << (IsNuBarDominant ? "nuXb_flux" : "nuX_flux")
             << " in input file: " << fInputRootFile->GetName() << std::endl;
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
    numuXSec->Divide(fFluxHist);

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
    numubXSec->Divide(fFluxHist);

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
    nueXSec->Divide(fFluxHist);

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
    nuebXSec->Divide(fFluxHist);

    nuebXSec->SetTitle((fName + "; E_{#nu} (GeV);XSec").c_str());
  }

  tn->GetEntry(0);

  LOG(SAM) << "\tInput GiBUU file species weights: (numu:" << SpeciesWeights[0]
           << ",numub:" << SpeciesWeights[1] << ",nue:" << SpeciesWeights[2]
           << ",nueb:" << SpeciesWeights[3] << ")" << std::endl;

  fFluxHist->SetNameTitle(
      (fName + "_FLUX").c_str(),
      (fName + "; E_{#nu} (GeV);" +
       (IsNuBarDominant ? "#Phi_{#bar{#nu}} (A.U.)" : "#Phi_{#nu} (A.U.)"))
          .c_str());

  fEventHist = static_cast<TH1D*>(fFluxHist->Clone((fName + "_EVT").c_str()));
  fEventHist->Reset();
  fEventHist->SetBinContent(1, double(fNEvents) *
                                   TotalIntegratedFlux(0, 1.E5, "width") /
                                   fEventHist->GetXaxis()->GetBinWidth(1));

  fXSecHist = static_cast<TH1D*>(fEventHist->Clone((fName + "_XSEC").c_str()));
  fXSecHist->Divide(fFluxHist);

  fXSecHist->SetTitle((fName + "; E_{#nu} (GeV);XSec").c_str());

#else
  ERR(FTL) << "ERROR: Invalid Event File Provided" << std::endl;
  ERR(FTL) << "GiBUU Input Not Enabled." << std::endl;
  ERR(FTL) << "Rebuild with -DUSE_GiBUU=1." << std::endl;
  exit(-1);
#endif
*/
}

//********************************************************************
void InputHandler::ReadBinSplineFile() {
  //********************************************************************

  // Bin Splines are saved as one event for each histogram bin.
  // So just read in as normal event splines and it'll all get sorted easily.
}

//********************************************************************
void InputHandler::ReadHistogramFile() {
  //********************************************************************

  // Convert the raw histogram into a series of events with X variables

  // So we don't have to pass stuff upsteam
}

//********************************************************************
void InputHandler::ReadNuanceFile() {
//********************************************************************
/*
#ifdef __NUANCE_ENABLED__
  // Read in Nuance output ROOT file (converted from hbook)
  LOG(SAM) << " Reading NUANCE " << std::endl;
  fEventType = kNUANCE;

  // Read in NUANCE Tree
  tn = new TChain("h3");
  tn->AddFile(fInputFile.c_str());

  // Get entries and fNuwroEvent
  fNEvents = tn->GetEntries();
  fNuanceEvt = new NuanceEvent();

  // SetBranchAddress for Nuance
  //  tn->SetBranchAddress("cc",&fNuanceEvt->cc);
  //  tn->SetBranchAddress("bound",&fNuanceEvt->bound);
  tn->SetBranchAddress("neutrino", &fNuanceEvt->neutrino);
  tn->SetBranchAddress("target", &fNuanceEvt->target);
  tn->SetBranchAddress("channel", &fNuanceEvt->channel);
  //  tn->SetBranchAddress("iniQ", &fNuanceEvt->iniQ);
  //  tn->SetBranchAddress("finQ", &fNuanceEvt->finQ);
  //  tn->SetBranchAddress("lepton0", &fNuanceEvt->lepton0);
  //  tn->SetBranchAddress("polar", &fNuanceEvt->polar);
  //  tn->SetBranchAddress("qsq", &fNuanceEvt->qsq);

  //  tn->SetBranchAddress("w", &fNuanceEvt->w);
  //  tn->SetBranchAddress("x",&fNuanceEvt->x);
  //  tn->SetBranchAddress("y",&fNuanceEvt->y);

  tn->SetBranchAddress("p_neutrino", &fNuanceEvt->p_neutrino);
  tn->SetBranchAddress("p_targ", &fNuanceEvt->p_targ);
  //  tn->SetBranchAddress("vertex", &fNuanceEvt->vertex);
  //  tn->SetBranchAddress("start",&fNuanceEvt->start);
  //  tn->SetBranchAddress("depth",&fNuanceEvt->depth);
  // tn->SetBranchAddress("flux",&fNuanceEvt->flux);

  tn->SetBranchAddress("n_leptons", &fNuanceEvt->n_leptons);
  tn->SetBranchAddress("p_ltot", &fNuanceEvt->p_ltot);
  tn->SetBranchAddress("lepton", &fNuanceEvt->lepton);
  tn->SetBranchAddress("p_lepton", &fNuanceEvt->p_lepton);

  tn->SetBranchAddress("n_hadrons", &fNuanceEvt->n_hadrons);
  tn->SetBranchAddress("p_htot", &fNuanceEvt->p_htot);
  tn->SetBranchAddress("hadron", &fNuanceEvt->hadron);
  tn->SetBranchAddress("p_hadron", &fNuanceEvt->p_hadron);

  fEvent->SetEventAddress(&fNuanceEvt);
  double EnuMin = 0.0;     // tn->GetMinimum("p_neutrino[3]");
  double EnuMax = 1000.0;  // tn->GetMaximum("p_neutrino[3]");

  fFluxHist = new TH1D((fName + "_FLUX").c_str(), (fName + "_FLUX").c_str(),
                       100, EnuMin, EnuMax);
  for (int i = 0; i < fFluxHist->GetNbinsX(); i++) {
    fFluxHist->SetBinContent(i + 1, 1.0);
  }
  fFluxHist->Scale(1.0 / fFluxHist->Integral());

  fEventHist = new TH1D((fName + "_EVT").c_str(), (fName + "_EVT").c_str(), 100,
                        EnuMin, EnuMax);
  for (int i = 0; i < fFluxHist->GetNbinsX(); i++) {
    fEventHist->SetBinContent(i + 1, 1.0);
  }
  fEventHist->Scale(1.0 / fEventHist->Integral());

  fXSecHist = (TH1D*)fEventHist->Clone();
  fXSecHist->Divide(fFluxHist);

  // Print out what was read in
  LOG(SAM) << " -> Successfully Read NUANCE file" << std::endl;
  if (LOG_LEVEL(SAM)) PrintStartInput();

#else
  ERR(FTL) << "ERROR: Invalid Event File Provided" << std::endl;
  ERR(FTL) << "NUANCE Input Not Enabled." << std::endl;
  ERR(FTL) << "Rebuild with -DUSE_NUANCE=1!" << std::endl;
  exit(-1);
#endif
*/
}

//********************************************************************
void InputHandler::PrintStartInput() {
  //********************************************************************

  LOG(SAM) << " -> Total events = " << fNEvents << std::endl;
  LOG(SAM) << " -> Energy Range = " << fFluxHist->GetXaxis()->GetXmin() << "-"
           << fFluxHist->GetXaxis()->GetXmax() << " GeV" << std::endl;
  LOG(SAM) << " -> Integrated Flux Hist = "
           << fFluxHist->Integral(0, fFluxHist->GetNbinsX(), "width")
           << std::endl;

  LOG(SAM) << " -> Integrated Event Hist = "
           << fEventHist->Integral(0, fEventHist->GetNbinsX(), "width")
           << std::endl;

  LOG(SAM) << " -> Integrated Inclusive XSec = "
           << (fEventHist->Integral(0, fEventHist->GetNbinsX(), "width") /
               fFluxHist->Integral(0, fFluxHist->GetNbinsX(), "width")) *
                  1E-38
           << std::endl;
  LOG(SAM) << " -> Integrated XSec Hist = " << fXSecHist->Integral("width")
           << endl;

  if (fEventType == kEVTSPLINE) return;

  // Get First event info
  StopTalking();
  tn->GetEntry(0);
  StartTalking();

  fEvent->CalcKinematics();
  LOG(SAM) << " -> Event 0. Neutrino PDG = " << fEvent->PartInfo(0)->fPID
           << std::endl;
  LOG(SAM) << "             Target A     = " << fEvent->GetTargetA()
           << std::endl;
  LOG(SAM) << "             Target Z     = " << fEvent->GetTargetZ()
           << std::endl;
}

//********************************************************************
std::string InputHandler::GetInputStateString() {
  //********************************************************************

  tn->GetEntry(0);
  fEvent->CalcKinematics();
  std::ostringstream state;
  state << "T" << fEventType << "_PDG" << fEvent->PartInfo(0)->fPID << "_Z"
        << fEvent->GetTargetZ() << "_A" << fEvent->GetTargetA();

  return state.str();
}

//********************************************************************
void InputHandler::ReadEvent(unsigned int i) {
  //********************************************************************

  bool using_events = (fEventType == kNEUT || fEventType == kGENIE ||
                       fEventType == kNUWRO || fEventType == kNEWSPLINE || fEventType == kINPUTFITEVENT || 
                       fEventType == kNUANCE || fEventType == kGiBUU );

  if (using_events) {
    tn->LoadTree(i);
    tn->GetEntry(i);
    fEvent->CalcKinematics();
    fEvent->Index = i;
    fEventIndex = i;
    fEvent->InputWeight = GetInputWeight(i);
  } else {
    GetTreeEntry(i);
    fEvent->CalcKinematics();
  }
}

//********************************************************************
void InputHandler::GetTreeEntry(const Long64_t i) {
  //********************************************************************

  // If we're just reading from the input root file
  if (fEventType != kEVTSPLINE) {
    tn->GetEntry(i);
  } else {
    fEvent->FillCoeff(fSplineArray[i]);
  }

  fEventIndex = i;
  fEvent->InputWeight = GetInputWeight(i);
}

//********************************************************************
double InputHandler::GetInputWeight(const int entry) {
  //********************************************************************

  if (fEventType == kGiBUU) {
    return fEvent->InputWeight;
  }

  //  if (fEventType == kGENIE) {
  //    return fEvent->InputWeight;
  //  }

  if (!fIsJointInput) {
    return 1.0;
  }
  double weight = 1.0;

  // Find Histogram
  for (UInt_t j = 0; j < fJointIndexLow.size(); j++) {
    if (entry >= fJointIndexLow.at(j) and entry < fJointIndexHigh.at(j)) {
      weight *= fJointIndexScale.at(j);
      break;
    }
  }

  return weight;
}

//********************************************************************
int InputHandler::GetGenEvents() {
  //********************************************************************

  if (fEventType == 6)
    return fSplineHead->ngen_events;
  else
    return GetNEvents();
}

//********************************************************************
double InputHandler::TotalIntegratedFlux(double low, double high,
                                         std::string intOpt) {
  //********************************************************************

  Int_t minBin = fFluxHist->GetXaxis()->FindFixBin(low);
  Int_t maxBin = fFluxHist->GetXaxis()->FindFixBin(high);

  if ((fFluxHist->IsBinOverflow(minBin) && (low != -9999.9))) {
    minBin = 1;
  }

  if ((fFluxHist->IsBinOverflow(maxBin) && (high != -9999.9))) {
    maxBin = fFluxHist->GetXaxis()->GetNbins() + 1;
  }


  // If we are within a single bin
  if (minBin == maxBin) {
    // Get the contained fraction of the single bin's width
    return ((high - low) / fFluxHist->GetXaxis()->GetBinWidth(minBin)) *
           fFluxHist->Integral(minBin, minBin, intOpt.c_str());
  }

  double lowBinUpEdge = fFluxHist->GetXaxis()->GetBinUpEdge(minBin);
  double highBinLowEdge = fFluxHist->GetXaxis()->GetBinLowEdge(maxBin);

  double lowBinfracIntegral =
      ((lowBinUpEdge - low) / fFluxHist->GetXaxis()->GetBinWidth(minBin)) *
      fFluxHist->Integral(minBin, minBin, intOpt.c_str());
  double highBinfracIntegral =
      ((high - highBinLowEdge) / fFluxHist->GetXaxis()->GetBinWidth(maxBin)) *
      fFluxHist->Integral(maxBin, maxBin, intOpt.c_str());

  // If they are neighbouring bins
  if ((minBin + 1) == maxBin) {
    // Get the contained fraction of the two bin's width
    return lowBinfracIntegral + highBinfracIntegral;
  }

  // If there are filled bins between them
  return lowBinfracIntegral + highBinfracIntegral +
         fFluxHist->Integral(minBin + 1, maxBin - 1, intOpt.c_str());
}

//********************************************************************
double InputHandler::PredictedEventRate(double low, double high,
                                        std::string intOpt) {
  //********************************************************************

  int minBin = fFluxHist->GetXaxis()->FindBin(low);
  int maxBin = fFluxHist->GetXaxis()->FindBin(high);

  return fEventHist->Integral(minBin, maxBin + 1, intOpt.c_str());
}
