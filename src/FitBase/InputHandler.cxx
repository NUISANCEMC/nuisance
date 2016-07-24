// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "InputHandler.h"

//***********************************
InputHandler::InputHandler(std::string handle, std::string infile_name) {
  //***********************************

  LOG(SAM) << "Creating InputHandler for " << handle << "..." << std::endl;
  LOG(SAM) << " -> [" << infile_name << "]" << std::endl;

  // Read in parameters for handler
  this->maxEvents = FitPar::Config().GetParI("MAXEVENTS");
  isJointInput = false;

  // Setup a custom Event class
  this->cust_event = new FitEvent();
  this->signal_event = new BaseFitEvt();
  this->inFile = infile_name;
  this->handleName = handle;

  // Parse Infile to allow enviornmental flags
  this->inFile = this->ParseInputFile(this->inFile);

  LOG(SAM) << " -> Type  = " << inType << std::endl;
  LOG(SAM) << " -> Input = " << inFile << std::endl;

  // Automatically check what sort of event file it is
  if (inType.compare("JOINT"))
    this->inRootFile = new TFile(this->inFile.c_str(), "READ");

  // Setup the handler for each type
  if (!inType.compare("NEUT"))
    this->ReadNeutFile();
  else if (!inType.compare("NUWRO"))
    this->ReadNuWroFile();
  else if (!inType.compare("GENIE"))
    this->ReadGenieFile();
  else if (!inType.compare("GiBUU_nu"))
    this->ReadGiBUUFile(false);
  else if (!inType.compare("GiBUU_nub"))
    this->ReadGiBUUFile(true);
  else if (!inType.compare("HIST"))
    this->ReadHistogramFile();
  else if (!inType.compare("BNSPLN"))
    this->ReadBinSplineFile();
  else if (!inType.compare("EVSPLN"))
    this->ReadEventSplineFile();
  else if (!inType.compare("NUANCE"))
    this->ReadNuanceFile();
  else if (!inType.compare("JOINT"))
    this->ReadJointFile();
  else {
    LOG(FTL) << " -> ERROR: Invalid Event File Type" << std::endl;
    inRootFile->ls();
    exit(-1);
  }

  // Setup MaxEvents After setup of ttree
  if (maxEvents > 1 and maxEvents < nEvents) {
    LOG(SAM) << " -> Reading only " << maxEvents << " events from total."
             << std::endl;
    nEvents = maxEvents;
  }

  this->fluxList.push_back(this->fluxHist);
  this->eventList.push_back(this->eventHist);
  this->xsecList.push_back(this->xsecHist);

  LOG(SAM) << " -> Finished handler initialisation." << std::endl;
  return;
};

//********************************************************************
std::string InputHandler::ParseInputFile(std::string inputstring) {
  //********************************************************************

  // Parse out the input_type
  const int nfiletypes = 8;
  const std::string filetypes[nfiletypes] = {"NEUT",     "NUWRO",    "GENIE",
                                             "EVSPLN",   "JOINT",    "NUANCE",
                                             "GiBUU_nu", "GiBUU_nub"};

  for (int i = 0; i < nfiletypes; i++) {
    std::string tempTypes = filetypes[i] + ":";
    if (inputstring.find(tempTypes) != std::string::npos) {
      inType = filetypes[i];
      inputstring.replace(inputstring.find(tempTypes), tempTypes.size(), "");
      break;
    }
  }

  // Parse out envir flags
  const int nfiledir = 5;
  const std::string filedir[nfiledir] = {"NEUT_DIR", "NUWRO_DIR", "GENIE_DIR",
                                         "NUANCE_DIR", "EVSPLN_DIR"};

  for (int i = 0; i < nfiledir; i++) {
    std::string tempDir = "@" + filedir[i];
    if (inputstring.find(tempDir) != std::string::npos) {
      std::string event_folder = FitPar::Config().GetParS(filedir[i]);
      inputstring.replace(inputstring.find(tempDir), tempDir.size(),
                          event_folder);

      break;
    }
  }

  return inputstring;
}

//********************************************************************
bool InputHandler::CanIGoFast() {
  //********************************************************************

  if (eventType == 6) {
    return true;
  }
  return false;
}

//********************************************************************
void InputHandler::ReadEventSplineFile() {
  //********************************************************************

  LOG(SAM) << " -> Setting up SPLINE inputs" << std::endl;

  // Event Type 7 SPLINES
  this->eventType = 6;

  // Get flux histograms NEUT supplies
  this->fluxHist = (TH1D*)inRootFile->Get((this->handleName + "_FLUX").c_str());
  this->eventHist = (TH1D*)inRootFile->Get((this->handleName + "_EVT").c_str());
  this->xsecHist = (TH1D*)inRootFile->Get((this->handleName + "_XSEC").c_str());

  // Setup Spline Stuff
  this->splhead = (FitSplineHead*)inRootFile->Get(
      (this->handleName + "_splineHead").c_str());
  tn = new TChain(Form("%s", (this->handleName + "_splineEvents").c_str()), "");
  tn->Add(Form("%s/%s", this->inFile.c_str(),
               (this->handleName + "_splineEvents").c_str()));

  // Assign nvect
  nEvents = tn->GetEntries();
  cust_event = NULL;
  tn->SetBranchAddress("FitEvent", &cust_event);

  // Load Dial Coeffs into vector
  for (int i = 0; i < nEvents; i++) {
    tn->GetEntry(i);
    tn->Show(i);
    spline_list.push_back(*cust_event->dial_coeff);
  }
  sleep(5);

  // Set MAXEVENTS CALC Here before we load in splines
  if (maxEvents > 1 and maxEvents < nEvents) {
    LOG(SAM) << " -> Reading only " << maxEvents
             << " events from total spline events." << std::endl;
    nEvents = maxEvents;
  }

  // Load all the splines into signal memory
  //  for (int i = 0; i < nEvents; i++){
  //    tn->GetEntry(i);
  //    BaseFitEvt* base_event = (new BaseFitEvt(cust_event));
  //    base_event->fType=6;
  //    signal_events.push_back( base_event );
  //  }

  // Print out what was read in
  LOG(SAM) << " -> Successfully Read SPLINE file" << std::endl;
  if (LOG_LEVEL(SAM)) this->PrintStartInput();

  int cnt = 1;
  std::list<FitSpline*>::iterator spl_iter =
      this->splhead->SplineObjects.begin();
  for (; spl_iter != this->splhead->SplineObjects.end(); spl_iter++) {
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
  return this->splhead;
}

//********************************************************************
void InputHandler::ReadJointFile() {
  //********************************************************************

  LOG(SAM) << " -> Reading list of inputs from file" << std::endl;
  isJointInput = true;

  // Parse Input File
  std::string line;
  std::ifstream card(inFile.c_str(), ifstream::in);
  std::vector<std::string> input_lines;

  while (std::getline(card, line, '\n')) {
    std::istringstream stream(line);

    // Add normalisation option for second line
    input_lines.push_back(line);

    // Split to get normalisation
  }

  card.close();

  // Loop over input and get the flux files
  // Using a temporary input handler to do this, which is a bit dodge.
  int count_low = 0;
  int temp_type = -1;
  for (UInt_t i = 0; i < input_lines.size(); i++) {
    // Create Temporary InputHandlers inside
    InputHandler* temp_input = new InputHandler(
        std::string(Form("temp_input_%i", i)), input_lines.at(i));

    if (temp_type != temp_input->GetType() and i > 0) {
      ERR(FTL) << " Can't use joint events with mismatched trees yet!"
               << std::endl;
      ERR(FTL) << " Make them all the same type!" << std::endl;
    }

    temp_type = temp_input->GetType();

    TH1D* temp_flux = (TH1D*)temp_input->GetFluxHistogram()->Clone();
    TH1D* temp_evts = (TH1D*)temp_input->GetEventHistogram()->Clone();
    TH1D* temp_xsec = (TH1D*)temp_input->GetXSecHistogram()->Clone();
    int temp_events = temp_input->GetNEvents();

    temp_flux->SetName(
        (this->handleName + "_" + temp_input->GetInputStateString() + "_FLUX")
            .c_str());
    temp_evts->SetName(
        (this->handleName + "_" + temp_input->GetInputStateString() + "_EVT")
            .c_str());
    temp_xsec->SetName(
        (this->handleName + "_" + temp_input->GetInputStateString() + "_XSEC")
            .c_str());

    this->fluxList.push_back(temp_flux);
    this->eventList.push_back(temp_evts);
    this->xsecList.push_back(temp_xsec);

    this->joint_index_low.push_back(count_low);
    this->joint_index_high.push_back(count_low + temp_events);
    this->joint_index_hist.push_back((TH1D*)temp_evts->Clone());

    count_low += temp_events;

    if (i == 0) {
      this->fluxHist = (TH1D*)temp_flux->Clone();
      this->eventHist = (TH1D*)temp_evts->Clone();
    } else {
      this->fluxHist->Add(temp_flux);
      this->eventHist->Add(temp_evts);
    }
    std::cout << "Added Input File " << input_lines.at(i) << std::endl
              << " with " << temp_events << std::endl;
  }

  // Now have all correctly normalised histograms all we need to do is setup the
  // TChains

  // Input Assumes all the same type
  std::string tree_name = "";
  if (temp_type == 0)
    tree_name = "neuttree";
  else if (temp_type == 1)
    tree_name = "treeout";

  // Add up the TChains
  tn = new TChain(tree_name.c_str());
  for (UInt_t i = 0; i < input_lines.size(); i++) {
    // PARSE INPUT
    std::cout << "Adding new tchain " << input_lines.at(i) << std::endl;
    std::string temp_file = this->ParseInputFile(input_lines.at(i));
    tn->Add(temp_file.c_str());
  }

  // Setup Events
  nEvents = tn->GetEntries();
  if (temp_type == 0) {
#ifdef __NEUT_ENABLED__
    eventType = 0;
    neut_event = NULL;
    tn->SetBranchAddress("vectorbranch", &neut_event);
    this->cust_event->SetEventAddress(&neut_event);
#endif
  } else if (temp_type == 1) {
#ifdef __NUWRO_ENABLED__
    eventType = 1;
    nuwro_event = NULL;
    tn->SetBranchAddress("e", &nuwro_event);
    this->cust_event->SetEventAddress(&nuwro_event);
#endif
  }

  // Normalise event histogram PDFS for weights
  for (UInt_t i = 0; i < input_lines.size(); i++) {
    TH1D* temp_hist = (TH1D*)joint_index_hist.at(i)->Clone();
    joint_index_weight.push_back(
        double(nEvents) / eventHist->Integral("width") *
        joint_index_hist.at(i)->Integral("width") /
        double(joint_index_high.at(i) - joint_index_low.at(i)));

    temp_hist->Scale(double(nEvents) / eventHist->Integral("width"));
    temp_hist->Scale(joint_index_hist.at(i)->Integral("width") /
                     double(joint_index_high.at(i)));

    this->joint_index_hist.at(i) = temp_hist;
  }

  this->eventHist->SetNameTitle((this->handleName + "_EVT").c_str(),
                                (this->handleName + "_EVT").c_str());
  this->fluxHist->SetNameTitle((this->handleName + "_FLUX").c_str(),
                               (this->handleName + "_FLUX").c_str());

  return;
}

//********************************************************************
void InputHandler::ReadNeutFile() {
//********************************************************************

#ifdef __NEUT_ENABLED__

  LOG(SAM) << " -> Setting up NEUT inputs" << std::endl;

  // Event Type 0 Neut
  this->eventType = 0;

  // Get flux histograms NEUT supplies
  this->fluxHist = (TH1D*)inRootFile->Get(
      (PlotUtils::GetObjectWithName(inRootFile, "flux")).c_str());
  this->fluxHist->SetNameTitle((this->handleName + "_FLUX").c_str(),
                               (this->handleName + "; E_{#nu} (GeV)").c_str());

  this->eventHist = (TH1D*)inRootFile->Get(
      (PlotUtils::GetObjectWithName(inRootFile, "evtrt")).c_str());
  this->eventHist->SetNameTitle(
      (this->handleName + "_EVT").c_str(),
      (this->handleName + "; E_{#nu} (GeV); Event Rate").c_str());

  this->xsecHist = (TH1D*)eventHist->Clone();
  this->xsecHist->Divide(this->fluxHist);
  this->xsecHist->SetNameTitle(
      (this->handleName + "_XSEC").c_str(),
      (this->handleName + "_XSEC;E_{#nu} (GeV); XSec (1#times10^{-38} cm^{2})")
          .c_str());

  // Read in the file once only
  tn = new TChain("neuttree", "");
  tn->Add(Form("%s/neuttree", this->inFile.c_str()));

  // Assign nvect
  nEvents = tn->GetEntries();
  neut_event = NULL;
  tn->SetBranchAddress("vectorbranch", &neut_event);

  // Make the custom event read in nvect when calling CalcKinematics
  this->cust_event->SetEventAddress(&neut_event);

  // Print out what was read in
  LOG(SAM) << " -> Successfully Read NEUT file" << std::endl;
  if (LOG_LEVEL(SAM)) this->PrintStartInput();

#else
  ERR(FTL) << "ERROR: Invalid Event File Provided" << std::endl;
  ERR(FTL) << "NEUT Input Not Enabled." << std::endl;
  ERR(FTL) << "Rebuild with --enable-neut or check FitBuild.h!" << std::endl;
  exit(-1);
#endif

  return;
}

//********************************************************************
void InputHandler::ReadNuWroFile() {
//********************************************************************

#ifdef __NUWRO_ENABLED__

  LOG(SAM) << " -> Setting up Nuwro inputs" << std::endl;

  // Event Type 1 == NuWro
  this->eventType = 1;

  // Setup the TChain for nuwro event tree
  tn = new TChain("treeout");
  tn->AddFile(this->inFile.c_str());

  // Get entries and nuwro_event
  nEvents = tn->GetEntries();
  nuwro_event = NULL;
  tn->SetBranchAddress("e", &nuwro_event);
  this->cust_event->SetEventAddress(&nuwro_event);

  // Check if we have saved an xsec histogram before
  this->fluxHist = (TH1D*)inRootFile->Get(
      (PlotUtils::GetObjectWithName(inRootFile, "FluxHist")).c_str());
  this->eventHist = (TH1D*)inRootFile->Get(
      (PlotUtils::GetObjectWithName(inRootFile, "EvtHist")).c_str());

  // Check if we are forcing plot generation (takes time)
  bool regenFlux = FitPar::Config().GetParB("input.regen_nuwro_plots");
  if (regenFlux)
    LOG(SAM)
        << " -> Forcing NuWro XSec/Flux plots to be generated at the start. "
        << std::endl;

  // Already generated flux and event histograms
  if (fluxHist and eventHist and !regenFlux) {
    this->xsecHist = (TH1D*)inRootFile->Get(
        (PlotUtils::GetObjectWithName(inRootFile, "xsec")).c_str());

    this->fluxHist->SetNameTitle((this->handleName + "_FLUX").c_str(),
                                 (this->handleName + "_FLUX").c_str());
    this->eventHist->SetNameTitle((this->handleName + "_EVT").c_str(),
                                  (this->handleName + "_EVT").c_str());
    this->xsecHist->SetNameTitle((this->handleName + "_XSEC").c_str(),
                                 (this->handleName + "_XSEC").c_str());

    // Need to regenerate if not found
  } else {
    LOG(SAM)
        << " -> No NuWro XSec or Flux Histograms found, need to regenerate!"
        << std::endl;

    // Can grab flux histogram from the pars
    tn->GetEntry(0);
    std::string fluxstring = nuwro_event->par.beam_energy;

    LOG(SAM) << "Nuwro Input Flux = " << fluxstring << std::endl;
    std::vector<double> contents =
        PlotUtils::FillVectorDFromString(fluxstring, " ");

    if (fluxstring.empty() or contents.empty()) {
      ERR(WRN) << "Incorrect NuWro flux type " << std::endl;
      ERR(WRN) << "Need to provide as a beam_energy list" << std::endl;
      ERR(WRN) << "e.g. beam_energy = Low High Contents_i" << std::endl;
      exit(-1);
    }

    // Parse the input string values
    int count = 0;
    for (UInt_t i = 0; i < contents.size(); i++) {
      if (contents.at(i) <= 0.0001 and contents.at(i) != 0.0)
        count++;
      else
        break;
    }

    double nuwro_Elow = contents[count];
    double nuwro_Ehigh = contents[count + 1];
    int nuwro_NBins = contents.size() - 2 - count;
    std::cout << "CONTENTS VALS = " << contents[0] << " " << contents[1] << " "
              << contents[2] << " " << contents[3] << std::endl;

    std::cout << "Nuwro Range = " << nuwro_Elow << "-" << nuwro_Ehigh
              << std::endl;
    // Create Empty Histograms
    this->fluxHist =
        new TH1D("nuwro_flux", "nuwro_flux; E_{#nu} (GeV);#nu", nuwro_NBins,
                 nuwro_Elow / 1000.0, nuwro_Ehigh / 1000.0);

    this->eventHist = new TH1D(
        "nuwro_evt", "nuwro_evt; E_{#nu} (GeV);Events [#times 10^{-38}]",
        nuwro_NBins, nuwro_Elow / 1000.0, nuwro_Ehigh / 1000.0);

    this->xsecHist = new TH1D(
        "nuwro_xsec", "nuwro_xsec; E_{#nu} (GeV);#sigma [#times 10^{-38}]",
        nuwro_NBins, nuwro_Elow / 1000.0, nuwro_Ehigh / 1000.0);

    std::cout << "Flux Edges = " << nuwro_Elow << " " << nuwro_Ehigh
              << std::endl;

    // Fill Flux Histogram
    for (int i = 0; i < nuwro_NBins; i++) {
      this->fluxHist->SetBinContent(i + 1, contents.at(i + 2 + count));
    }

    // Start Processing
    LOG(SAM) << " -> Processing NuWro Input Flux for " << nEvents
             << " events (This can take a while...) " << std::endl;

    double Enu = 0.0;
    double TotXSec = 0.0;
    double totaleventmode = 0.0;
    double totalevents = 0.0;

    // --- loop
    for (int i = 0; i < nEvents; i++) {
      tn->GetEntry(i);

      // Get Variables
      Enu = nuwro_event->in[0].E() / 1000.0;
      TotXSec = nuwro_event->weight;

      // Fill a flux and xsec histogram
      this->eventHist->Fill(Enu);
      this->xsecHist->Fill(Enu, TotXSec);

      // Keep Tally
      totaleventmode += TotXSec;
      totalevents++;
    };

    LOG(SAM) << " -> Flux Processing Loop Finished." << std::endl;

    if (this->eventHist->Integral() == 0.0) {
      std::cout << "ERROR NO EVENTS FOUND IN RANGE! " << std::endl;
      exit(-1);
    }

    // Sort out plot scaling
    double AvgXSec = (totaleventmode * 1.0E38 / (totalevents + 0.));
    LOG(SAM) << " -> Average XSec = " << AvgXSec << std::endl;

    this->eventHist->Scale(1.0 / eventHist->Integral());  // Convert to PDF
    this->eventHist->Scale(this->fluxHist->Integral() *
                           AvgXSec);  // Convert to Proper Event Rate

    this->xsecHist->Add(eventHist);          // Get Event Rate Plot
    this->xsecHist->Divide(this->fluxHist);  // Make XSec Plot

    // this->eventHist = (TH1D*)this->fluxHist->Clone();
    // this->eventHist->Multiply(this->xsecHist);

    // Clear over/underflows incase they mess with integrals later.
    this->fluxHist->SetBinContent(0, 0.0);
    this->fluxHist->SetBinContent(this->fluxHist->GetNbinsX() + 2, 0.0);

    this->eventHist->SetBinContent(0, 0.0);
    this->eventHist->SetBinContent(this->eventHist->GetNbinsX() + 2, 0.0);

    LOG(SAM)
        << " -> Finished making NuWro event plots. Saving them for next time..."
        << std::endl;

    TFile* temp_save_file = new TFile(this->inFile.c_str(), "UPDATE");
    temp_save_file->cd();

    this->fluxHist->Write("nuwro_flux", TObject::kOverwrite);
    this->eventHist->Write("nuwro_evtrt", TObject::kOverwrite);
    this->xsecHist->Write("nuwro_xsec", TObject::kOverwrite);

    temp_save_file->Close();
    delete temp_save_file;

    this->fluxHist->SetNameTitle((this->handleName + "_FLUX").c_str(),
                                 (this->handleName + "_FLUX").c_str());
    this->eventHist->SetNameTitle((this->handleName + "_EVT").c_str(),
                                  (this->handleName + "_EVT").c_str());
    this->xsecHist->SetNameTitle((this->handleName + "_XSEC").c_str(),
                                 (this->handleName + "_XSEC").c_str());
  }

  // Print out what was read in
  LOG(SAM) << " -> Successfully Read NUWRO file" << std::endl;
  if (LOG_LEVEL(SAM)) this->PrintStartInput();

#else
  ERR(FTL) << "ERROR: Invalid Event File Provided" << std::endl;
  ERR(FTL) << "NuWro Input Not Enabled." << std::endl;
  ERR(FTL) << "Rebuild with --enable-nuwro or check FitBuild.h!" << std::endl;
  exit(-1);
#endif

  return;
}

//********************************************************************
void InputHandler::ReadGenieFile() {
//********************************************************************

#ifdef __GENIE_ENABLED__

  // Event Type 1 NuWro
  this->eventType = 5;

  // Open Root File
  LOG(SAM) << "Reading event file " << this->inFile << std::endl;

  // Get flux histograms NEUT supplies
  this->fluxHist = (TH1D*)inRootFile->Get(
      (PlotUtils::GetObjectWithName(inRootFile, "spectrum")).c_str());
  this->fluxHist->SetNameTitle((this->handleName + "_FLUX").c_str(),
                               (this->handleName + "; E_{#nu} (GeV)").c_str());

  this->eventHist = (TH1D*)inRootFile->Get(
      (PlotUtils::GetObjectWithName(inRootFile, "spectrum")).c_str());
  this->eventHist->SetNameTitle(
      (this->handleName + "_EVT").c_str(),
      (this->handleName + "; E_{#nu} (GeV); Event Rate").c_str());

  this->xsecHist = (TH1D*)inRootFile->Get(
      (PlotUtils::GetObjectWithName(inRootFile, "spectrum")).c_str());
  this->xsecHist->SetNameTitle(
      (this->handleName + "_XSEC").c_str(),
      (this->handleName + "; E_{#nu} (GeV); Event Rate").c_str());

  double average_xsec = 0.0;
  int total_events = 0;

  // Setup the TChain for nuwro event tree
  tn = new TChain("gtree");
  tn->AddFile(this->inFile.c_str());

  nEvents = tn->GetEntries();
  LOG(SAM) << "Number of GENIE Eevents " << tn->GetEntries() << std::endl;
  genie_event = NULL;
  mcrec = NULL;
  //  NtpMCEventRecord * mcrec = 0; tree->SetBranchAddress(gmrec, &mcrec);
  tn->SetBranchAddress("gmcrec", &mcrec);

  this->eventHist->Reset();

  // Make the custom event read in nvect when calling CalcKinematics
  this->cust_event->SetEventAddress(&mcrec);

  LOG(SAM) << "Processing GENIE flux events." << std::endl;
  for (int i = 0; i < nEvents; i++) {
    tn->GetEntry(i);

    EventRecord& event = *(mcrec->event);
    GHepParticle* neu = event.Probe();

    GHepRecord genie_record = static_cast<GHepRecord>(event);

    double xsec = (genie_record.XSec() / (1E-38 * genie::units::cm2));

    average_xsec += xsec;
    total_events += 1;

    this->eventHist->Fill(neu->E());
    this->xsecHist->Fill(neu->E(), xsec);

    mcrec->Clear();
  }

  // Sort xsec hist
  this->xsecHist->Divide(this->eventHist);

  // Sort eventHist as xsecHist * eventHist
  this->eventHist = (TH1D*)this->fluxHist->Clone();
  this->eventHist->Multiply(this->xsecHist);

  this->eventHist->SetNameTitle(
      (this->handleName + "_EVT").c_str(),
      (this->handleName + "_EVT;E_{#nu} (GeV); Events (1#times10^{-38})")
          .c_str());
  this->xsecHist->SetNameTitle(
      (this->handleName + "_XSEC").c_str(),
      (this->handleName + "_XSEC;E_{#nu} (GeV); XSec (1#times10^{-38} cm^{2})")
          .c_str());

#else
  ERR(FTL) << "ERROR: Invalid Event File Provided" << std::endl;
  ERR(FTL) << "GENIE Input Not Enabled." << std::endl;
  ERR(FTL) << "Rebuild with --enable-genie or check FitBuild.h!" << std::endl;
  exit(-1);
#endif

  return;
}

//********************************************************************
void InputHandler::ReadGiBUUFile(bool IsNuBarDominant) {
//********************************************************************
#ifdef __GiBUU_ENABLED__
  this->eventType = kGiBUU;

  // Open Root File
  LOG(SAM) << "Opening event file " << this->inFile << std::endl;
  TFile* rootFile = new TFile(this->inFile.c_str(), "READ");
  // Get flux histograms NEUT supplies
  TH1D* numuFlux = dynamic_cast<TH1D*>(rootFile->Get("numu_flux"));
  TH1D* numubFlux = dynamic_cast<TH1D*>(rootFile->Get("numub_flux"));

  if (numuFlux) {
    numuFlux = static_cast<TH1D*>(numuFlux->Clone());
    numuFlux->SetDirectory(NULL);
    numuFlux->SetNameTitle(
        (this->handleName + "_numu_FLUX").c_str(),
        (this->handleName + "; E_{#nu} (GeV); #Phi_{#nu} (A.U.)").c_str());
    fluxList.push_back(numuFlux);
  }
  if (numubFlux) {
    numubFlux = static_cast<TH1D*>(numubFlux->Clone());
    numubFlux->SetDirectory(NULL);
    numubFlux->SetNameTitle(
        (this->handleName + "_numub_FLUX").c_str(),
        (this->handleName + "; E_{#nu} (GeV); #Phi_{#bar{#nu}} (A.U.)")
            .c_str());
    fluxList.push_back(numubFlux);
  }
  rootFile->Close();

  // Set flux hist to the dominant mode
  fluxHist = IsNuBarDominant ? numubFlux : numuFlux;

  if (!fluxHist) {
    ERR(FTL) << "Couldn't find: "
             << (IsNuBarDominant ? "numub_flux" : "numu_flux")
             << " in input file: " << inRootFile->GetName() << std::endl;
    exit(1);
  }
  fluxHist->SetNameTitle(
      (this->handleName + "_FLUX").c_str(),
      (this->handleName + "; E_{#nu} (GeV);" +
       (IsNuBarDominant ? "#Phi_{#bar{#nu}} (A.U.)" : "#Phi_{#nu} (A.U.)"))
          .c_str());
  tn = new TChain("giRooTracker");
  tn->AddFile(this->inFile.c_str());

  eventHist =
      static_cast<TH1D*>(fluxHist->Clone((this->handleName + "_EVT").c_str()));
  eventHist->Reset();
  nEvents = tn->GetEntries();
  eventHist->SetBinContent(
      1, double(nEvents) / eventHist->GetXaxis()->GetBinWidth(1));

  GiBUUStdHepReader* giRead = new GiBUUStdHepReader();
  giRead->SetBranchAddresses(tn);
  cust_event->SetEventAddress(giRead);
#endif
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

#ifdef __NUANCE_ENABLED__
  // Read in Nuance output ROOT file (converted from hbook)
  LOG(SAM) << " Reading NUANCE " << std::endl;
  eventType = kNUANCE;

  // Read in NUANCE Tree
  tn = new TChain("h3");
  tn->AddFile(this->inFile.c_str());

  // Get entries and nuwro_event
  nEvents = tn->GetEntries();
  nuance_event = new NuanceEvent();

  // SetBranchAddress for Nuance
  //  tn->SetBranchAddress("cc",&nuance_event->cc);
  //  tn->SetBranchAddress("bound",&nuance_event->bound);
  tn->SetBranchAddress("neutrino", &nuance_event->neutrino);
  tn->SetBranchAddress("target", &nuance_event->target);
  tn->SetBranchAddress("channel", &nuance_event->channel);
  //  tn->SetBranchAddress("iniQ", &nuance_event->iniQ);
  //  tn->SetBranchAddress("finQ", &nuance_event->finQ);
  //  tn->SetBranchAddress("lepton0", &nuance_event->lepton0);
  //  tn->SetBranchAddress("polar", &nuance_event->polar);
  //  tn->SetBranchAddress("qsq", &nuance_event->qsq);

  //  tn->SetBranchAddress("w", &nuance_event->w);
  //  tn->SetBranchAddress("x",&nuance_event->x);
  //  tn->SetBranchAddress("y",&nuance_event->y);

  tn->SetBranchAddress("p_neutrino", &nuance_event->p_neutrino);
  tn->SetBranchAddress("p_targ", &nuance_event->p_targ);
  //  tn->SetBranchAddress("vertex", &nuance_event->vertex);
  //  tn->SetBranchAddress("start",&nuance_event->start);
  //  tn->SetBranchAddress("depth",&nuance_event->depth);
  //tn->SetBranchAddress("flux",&nuance_event->flux);

  tn->SetBranchAddress("n_leptons", &nuance_event->n_leptons);
  tn->SetBranchAddress("p_ltot", &nuance_event->p_ltot);
  tn->SetBranchAddress("lepton", &nuance_event->lepton);
  tn->SetBranchAddress("p_lepton", &nuance_event->p_lepton);

  tn->SetBranchAddress("n_hadrons", &nuance_event->n_hadrons);
  tn->SetBranchAddress("p_htot", &nuance_event->p_htot);
  tn->SetBranchAddress("hadron", &nuance_event->hadron);
  tn->SetBranchAddress("p_hadron", &nuance_event->p_hadron);

  this->cust_event->SetEventAddress(&nuance_event);

  this->fluxHist = new TH1D( (this->handleName + "_FLUX").c_str(),
			     (this->handleName + "_FLUX").c_str(),
			     1, 0.0, 1.0);
 
  this->fluxHist->SetBinContent(1, 1.0);

  this->eventHist = new TH1D( (this->handleName + "_EVT").c_str(),
			     (this->handleName + "_EVT").c_str(),
			     1, 0.0, 1.0);
  this->eventHist->SetBinContent(1, nEvents);

#else
  ERR(FTL) << "ERROR: Invalid Event File Provided" << std::endl;
  ERR(FTL) << "NUANCE Input Not Enabled." << std::endl;
  ERR(FTL) << "Rebuild with -DUSE_NUANCE=1!" << std::endl;
  exit(-1);
#endif
}

//********************************************************************
void InputHandler::PrintStartInput() {
  //********************************************************************

  LOG(SAM) << " -> Total events = " << nEvents << std::endl;
  LOG(SAM) << " -> Energy Range = " << fluxHist->GetXaxis()->GetXmin() << "-"
           << fluxHist->GetXaxis()->GetXmax() << " GeV" << std::endl;
  LOG(SAM) << " -> Integrated Flux Hist = "
           << fluxHist->Integral(0, fluxHist->GetNbinsX(), "width")
           << std::endl;

  LOG(SAM) << " -> Integrated Event Hist = "
           << eventHist->Integral(0, eventHist->GetNbinsX(), "width")
           << std::endl;

  LOG(SAM) << " -> Integrated Inclusive XSec = "
	   << (eventHist->Integral(0, eventHist->GetNbinsX(), "width") /
	       fluxHist->Integral(0, fluxHist->GetNbinsX(), "width")) * 1E-38
           << std::endl;

  if (eventType == kEVTSPLINE) return;

  // Get First event info
  tn->GetEntry(0);
  cust_event->CalcKinematics();
  LOG(SAM) << " -> Event 0. Neutrino PDG = " << cust_event->PartInfo(0)->fPID
           << std::endl;
  LOG(SAM) << "             Target A     = " << cust_event->TargetA
           << std::endl;
  LOG(SAM) << "             Target Z     = " << cust_event->TargetZ
           << std::endl;
}

//********************************************************************
std::string InputHandler::GetInputStateString() {
  //********************************************************************

  std::ostringstream state;
  state << "T" << eventType << "_PDG" << cust_event->PartInfo(0)->fPID << "_Z"
        << cust_event->TargetZ << "_A" << cust_event->TargetA;

  return state.str();
}

//********************************************************************
void InputHandler::ReadEvent(unsigned int i) {
  //********************************************************************

  bool using_events =
      (eventType == 0 or eventType == 5 or eventType == 1 or
       eventType == kEVTSPLINE or eventType == kNUANCE or eventType == kGiBUU);

  if (using_events) {
    tn->GetEntry(i);

    if (eventType != kEVTSPLINE) cust_event->CalcKinematics();

    cust_event->Index = i;
    cur_entry = i;
    cust_event->InputWeight = GetInputWeight(i);

  } else {
    this->GetTreeEntry(i);
  }
}

//********************************************************************
void InputHandler::GetTreeEntry(const Long64_t i) {
  //********************************************************************

  if (eventType != kEVTSPLINE)
    tn->GetEntry(i);
  else
    (*(cust_event->dial_coeff)) = spline_list.at(i);

  cur_entry = i;
  cust_event->InputWeight = GetInputWeight(i);
}

//********************************************************************
double InputHandler::GetInputWeight(const int entry) {
  //********************************************************************

  if (eventType == kGiBUU) {
    return cust_event->InputWeight;
  }

  if (!isJointInput) {
    return 1.0;
  }
  double weight = 1.0;

  // Find Histogram
  for (UInt_t j = 0; j < joint_index_low.size(); j++) {
    if (entry >= joint_index_low.at(j) and entry < joint_index_high.at(j)) {
      weight *= joint_index_weight.at(j);
      break;
    }
  }

  return weight;
}

//********************************************************************
int InputHandler::GetGenEvents() {
  //********************************************************************

  if (eventType == 6)
    return this->splhead->ngen_events;
  else
    return this->GetNEvents();
}

//********************************************************************
double InputHandler::TotalIntegratedFlux(double low, double high,
                                         std::string intOpt) {
  //********************************************************************

  throw;

  int minBin = this->fluxHist->GetXaxis()->FindBin(low);
  int maxBin = this->fluxHist->GetXaxis()->FindBin(high);

  double integral =
      this->fluxHist->Integral(minBin, maxBin + 1, intOpt.c_str());

  return integral;
};

//********************************************************************
double InputHandler::PredictedEventRate(double low, double high,
                                        std::string intOpt) {
//********************************************************************

  int minBin = this->fluxHist->GetXaxis()->FindBin(low);
  int maxBin = this->fluxHist->GetXaxis()->FindBin(high);

  return this->eventHist->Integral(minBin, maxBin + 1, intOpt.c_str());
}
