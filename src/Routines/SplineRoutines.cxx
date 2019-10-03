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
#include "SplineRoutines.h"

void SplineRoutines::Init() {

  fStrategy = "SaveEvents";
  fRoutines.clear();

  fCardFile = "";

  fSampleFCN = NULL;
  fRW = NULL;

  fAllowedRoutines = ("SaveEvents,TestEvents,SaveSplineEvents");
};

SplineRoutines::~SplineRoutines(){};

SplineRoutines::SplineRoutines(int argc, char *argv[]) {

  // Initialise Defaults
  Init();
  nuisconfig configuration = Config::Get();

  // Default containers
  std::string cardfile = "";
  std::string maxevents = "-1";
  int errorcount = 0;
  int verbocount = 0;
  std::vector<std::string> xmlcmds;
  std::vector<std::string> configargs;

  // Make easier to handle arguments.
  std::vector<std::string> args = GeneralUtils::LoadCharToVectStr(argc, argv);
  ParserUtils::ParseArgument(args, "-c", fCardFile, true);
  ParserUtils::ParseArgument(args, "-o", fOutputFile, false, false);
  ParserUtils::ParseArgument(args, "-n", maxevents, false, false);
  ParserUtils::ParseArgument(args, "-f", fStrategy, false, false);
  ParserUtils::ParseArgument(args, "-i", xmlcmds);
  ParserUtils::ParseArgument(args, "-q", configargs);
  ParserUtils::ParseCounter(args, "e", errorcount);
  ParserUtils::ParseCounter(args, "v", verbocount);
  ParserUtils::CheckBadArguments(args);

  // Add extra defaults if none given
  if (fCardFile.empty() and xmlcmds.empty()) {
    NUIS_ABORT("No input supplied!");
  }

  if (fOutputFile.empty() and !fCardFile.empty()) {
    fOutputFile = fCardFile + ".root";
    NUIS_ERR(WRN, "No output supplied so saving it to: " << fOutputFile);

  } else if (fOutputFile.empty()) {
    NUIS_ABORT("No output file or cardfile supplied!");
  }

  // Configuration Setup =============================

  // Check no comp key is available
  nuiskey fCompKey;
  if (Config::Get().GetNodes("nuiscomp").empty()) {
    fCompKey = Config::Get().CreateNode("nuiscomp");
  } else {
    fCompKey = Config::Get().GetNodes("nuiscomp")[0];
  }

  if (!fCardFile.empty())
    fCompKey.Set("cardfile", fCardFile);
  fCompKey.Set("outputfile", fOutputFile);
  if (!fStrategy.empty())
    fCompKey.Set("strategy", fStrategy);

  // Load XML Cardfile
  configuration.LoadSettings(fCompKey.GetS("cardfile"), "");

  // Add Config Args
  for (size_t i = 0; i < configargs.size(); i++) {
    configuration.OverrideConfig(configargs[i]);
  }
  if (maxevents.compare("-1")) {
    std::cout << "[ NUISANCE ] : Overriding "
              << "MAXEVENTS=" + maxevents << std::endl;
    configuration.OverrideConfig("MAXEVENTS=" + maxevents);
  }

  // Finish configuration XML
  configuration.FinaliseSettings(fCompKey.GetS("outputfile") + ".xml");

  // Add Error Verbo Lines
  verbocount += Config::GetParI("VERBOSITY");
  errorcount += Config::GetParI("ERROR");
  std::cout << "[ NUISANCE ]: Setting VERBOSITY=" << verbocount << std::endl;
  std::cout << "[ NUISANCE ]: Setting ERROR=" << errorcount << std::endl;
  // FitPar::log_verb = verbocount;
  SETVERBOSITY(verbocount);
  // ERR_VERB(errorcount);

  // Starting Setup
  // ---------------------------
  SetupRWEngine();

  return;
};

/*
  Setup Functions
*/
//*************************************
void SplineRoutines::SetupRWEngine() {
  //*************************************

  fRW = new FitWeight("splineweight");
  // std::vector<nuiskey> splinekeys    = Config::QueryKeys("spline");
  std::vector<nuiskey> parameterkeys = Config::QueryKeys("parameter");

  // Add Parameters
  for (size_t i = 0; i < parameterkeys.size(); i++) {
    nuiskey key = parameterkeys[i];

    std::string parname = key.GetS("name");
    std::string partype = key.GetS("type");
    double nom = key.GetD("nominal");

    fRW->IncludeDial(key.GetS("name"), FitBase::ConvDialType(key.GetS("type")),
                     nom);
    fRW->SetDialValue(key.GetS("name"), key.GetD("nominal"));
  }
  fRW->Reconfigure();

  return;
}

/*
  Fitting Functions
*/
//*************************************
void SplineRoutines::UpdateRWEngine(std::map<std::string, double> &updateVals) {
  //*************************************

  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];

    if (updateVals.find(name) == updateVals.end())
      continue;
    fRW->SetDialValue(name, updateVals.at(name));
  }

  fRW->Reconfigure();
  return;
}

//*************************************
void SplineRoutines::Run() {
  //*************************************
  std::cout << "Running " << std::endl;

  // Parse given routines
  fRoutines = GeneralUtils::ParseToStr(fStrategy, ",");
  if (fRoutines.empty()) {
    NUIS_ABORT("Trying to run ComparisonRoutines with no routines given!");
  }

  for (size_t i = 0; i < fRoutines.size(); i++) {

    NUIS_LOG(FIT, "Running Routine: " << fRoutines[i]);
    std::string rout = fRoutines[i];
    if (!rout.compare("SaveEvents"))
      SaveEvents();
    else if (!rout.compare("TestEvents"))
      TestEvents();
    else if (!rout.compare("GenerateEventSplines")) {
      GenerateEventWeights();
      BuildEventSplines();
    } else if (!rout.compare("GenerateEventWeights")) {
      GenerateEventWeights();
    } else if (!rout.compare("GenerateEventWeightChunks")) {
      GenerateEventWeightChunks(FitPar::Config().GetParI("spline_procchunk"));
    } else if (!rout.compare("BuildEventSplines")) {
      BuildEventSplines();
    } else if (!rout.compare("TestSplines_1DEventScan"))
      TestSplines_1DEventScan();
    else if (!rout.compare("TestSplines_NDEventThrow"))
      TestSplines_NDEventThrow();
    else if (!rout.compare("SaveSplinePlots"))
      SaveSplinePlots();
    else if (!rout.compare("TestSplines_1DLikelihoodScan"))
      TestSplines_1DLikelihoodScan();
    else if (!rout.compare("TestSplines_NDLikelihoodThrow"))
      TestSplines_NDLikelihoodThrow();
    else if (!rout.compare("BuildEventSplinesChunks")) {
      int chunk = FitPar::Config().GetParI("spline_procchunk");
      BuildEventSplines(chunk);
    } else if (!rout.compare("MergeEventSplinesChunks")) {
      MergeEventSplinesChunks();
    }
  }
}

//*************************************
void SplineRoutines::SaveEvents() {
  //*************************************

  if (fRW)
    delete fRW;
  SetupRWEngine();
  fRW->Reconfigure();
  fRW->Print();

  // Generate a set of nominal events
  // Method, Loop over inputs, create input handler, then create a ttree
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (size_t i = 0; i < eventkeys.size(); i++) {
    nuiskey key = eventkeys.at(i);

    // Get I/O
    std::string inputfilename = key.GetS("input");
    if (inputfilename.empty()) {
      NUIS_ABORT("No input given for set of input events!");
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()) {
      outputfilename = inputfilename + ".nuisance.root";
      NUIS_ERR(WRN, "No output give for set of output events! Saving to "
                      << outputfilename);
    }

    // Make new outputfile
    TFile *outputfile = new TFile(outputfilename.c_str(), "RECREATE");
    outputfile->cd();

    // Make a new input handler
    std::vector<std::string> file_descriptor =
        GeneralUtils::ParseToStr(inputfilename, ":");
    if (file_descriptor.size() != 2) {
      NUIS_ABORT("File descriptor had no filetype declaration: \""
             << inputfilename << "\". expected \"FILETYPE:file.root\"");
    }
    InputUtils::InputType inptype =
        InputUtils::ParseInputType(file_descriptor[0]);

    InputHandlerBase *input = InputUtils::CreateInputHandler(
        "eventsaver", inptype, file_descriptor[1]);

    // Get info from inputhandler
    int nevents = input->GetNEvents();
    int countwidth = (nevents / 10);
    FitEvent *nuisevent = input->FirstNuisanceEvent();

    // Setup a TTree to save the event
    outputfile->cd();
    TTree *eventtree = new TTree("nuisance_events", "nuisance_events");
    nuisevent->AddBranchesToTree(eventtree);

    // Loop over all events and fill the TTree
    int icount = 0;
    // int countwidth = nevents / 5;

    while (nuisevent) {

      // Get Event Weight
      nuisevent->RWWeight = fRW->CalcWeight(nuisevent);
      // if (nuisevent->RWWeight != 1.0){
      // std::cout << "Weight = " << nuisevent->RWWeight << std::endl;
      // }
      // Save everything
      eventtree->Fill();

      // Logging
      if (icount % countwidth == 0) {
        NUIS_LOG(REC, "Saved " << icount << "/" << nevents
                           << " nuisance events. [M, W] = [" << nuisevent->Mode
                           << ", " << nuisevent->RWWeight << "]");
      }

      // iterate
      nuisevent = input->NextNuisanceEvent();
      icount++;
    }

    // Save flux and close file
    outputfile->cd();
    eventtree->Write();
    input->GetFluxHistogram()->Write("nuisance_fluxhist");
    input->GetEventHistogram()->Write("nuisance_eventhist");

    // Close Output
    outputfile->Close();

    // Delete Inputs
    delete input;
  }

  // remove Keys
  eventkeys.clear();

  // Finished
  NUIS_LOG(FIT, "Finished processing all nuisance events.");
}

//*************************************
void SplineRoutines::TestEvents() {
  //*************************************

  NUIS_LOG(FIT, "Testing events.");

  // Create a new file for the test samples
  if (!fOutputRootFile) {
    fOutputRootFile =
        new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");
  }

  // Loop over all tests
  int count = 0;
  std::vector<nuiskey> testkeys = Config::QueryKeys("sampletest");
  for (std::vector<nuiskey>::iterator iter = testkeys.begin();
       iter != testkeys.end(); iter++) {
    nuiskey key = (*iter);

    // 0. Create new measurement list
    std::list<MeasurementBase *> samplelist;

    // 1. Build Sample From Events
    std::string samplename = key.GetS("name");
    std::string eventsid = key.GetS("inputid");
    nuiskey eventskey = Config::QueryLastKey("events", "id=" + eventsid);
    std::string rawfile = eventskey.GetS("input");
    NUIS_LOG(FIT, "Creating sample " << samplename);
    MeasurementBase *rawsample = SampleUtils::CreateSample(
        samplename, rawfile, "", "", FitBase::GetRW());

    // 2. Build Sample From Nuisance Events
    std::string eventsfile = eventskey.GetS("output");
    NUIS_LOG(FIT, "Creating Fit Eevnt Sample " << samplename << " " << eventsfile);
    MeasurementBase *nuissample = SampleUtils::CreateSample(
        samplename, "FEVENT:" + eventsfile, "", "", FitBase::GetRW());

    // 3. Make some folders to save stuff
    TDirectory *sampledir = (TDirectory *)fOutputRootFile->mkdir(
        Form((samplename + "_test_%d").c_str(), count));
    TDirectory *rawdir = (TDirectory *)sampledir->mkdir("raw");
    TDirectory *nuisancedir = (TDirectory *)sampledir->mkdir("nuisance");
    TDirectory *difdir = (TDirectory *)sampledir->mkdir("difference");

    // 4. Reconfigure both
    rawdir->cd();
    rawsample->Reconfigure();
    rawsample->Write();

    nuisancedir->cd();
    nuissample->Reconfigure();
    nuissample->Write();

    // 4. Compare Raw to Nuisance Events

    // Loop over all keyse
    TIter next(rawdir->GetListOfKeys());
    TKey *dirkey;
    while ((dirkey = (TKey *)next())) {

      // If not a 1D/2D histogram skip
      TClass *cl = gROOT->GetClass(dirkey->GetClassName());
      if (!cl->InheritsFrom("TH1D") and !cl->InheritsFrom("TH2D"))
        continue;

      // Get TH1* from both dir
      TH1 *rawplot = (TH1 *)rawdir->Get(dirkey->GetName());
      TH1 *nuisanceplot = (TH1 *)nuisancedir->Get(dirkey->GetName());

      // Take Difference
      nuisanceplot->Add(rawplot, -1.0);

      // Save to dif folder
      difdir->cd();
      nuisanceplot->Write();
    }

    // 5. Tidy Up
    samplelist.clear();

    // Iterator
    count++;
  }
}

void SplineRoutines::GenerateEventWeightChunks(int procchunk) {
  if (fRW)
    delete fRW;
  SetupRWEngine();

  // Setup the spline reader
  SplineWriter *splwrite = new SplineWriter(fRW);
  std::vector<nuiskey> splinekeys = Config::QueryKeys("spline");

  // Add splines to splinewriter
  for (std::vector<nuiskey>::iterator iter = splinekeys.begin();
       iter != splinekeys.end(); iter++) {
    nuiskey splkey = (*iter);

    // Add Spline Info To Reader
    splwrite->AddSpline(splkey);
  }
  splwrite->SetupSplineSet();

  // Event Loop
  // Loop over all events and calculate weights for each parameter set.

  // Generate a set of nominal events
  // Method, Loop over inputs, create input handler, then create a ttree
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (size_t i = 0; i < eventkeys.size(); i++) {
    nuiskey key = eventkeys.at(i);

    // Get I/O
    std::string inputfilename = key.GetS("input");
    if (inputfilename.empty()) {
      NUIS_ABORT("No input given for set of input events!");
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()) {
      outputfilename = inputfilename + ".nuisance.root";
      NUIS_ERR(WRN,"No output give for set of output events! Saving to "
               << outputfilename);
    }
    outputfilename += ".weights.root";

    // Make new outputfile
    TFile *outputfile = new TFile(outputfilename.c_str(), "RECREATE");
    outputfile->cd();

    // Make a new input handler
    std::vector<std::string> file_descriptor =
        GeneralUtils::ParseToStr(inputfilename, ":");
    if (file_descriptor.size() != 2) {
      NUIS_ABORT("File descriptor had no filetype declaration: \""
             << inputfilename << "\". expected \"FILETYPE:file.root\"");
    }
    InputUtils::InputType inptype =
        InputUtils::ParseInputType(file_descriptor[0]);

    InputHandlerBase *input = InputUtils::CreateInputHandler(
        "eventsaver", inptype, file_descriptor[1]);

    // Get info from inputhandler
    int nevents = input->GetNEvents();
    // int countwidth = (nevents / 1000);
    FitEvent *nuisevent = input->FirstNuisanceEvent();

    // Setup a TTree to save the event
    outputfile->cd();
    TTree *eventtree = new TTree("nuisance_events", "nuisance_events");

    // Add a flag that allows just splines to be saved.
    nuisevent->AddBranchesToTree(eventtree);

    // Save the spline reader
    splwrite->Write("spline_reader");

    // Setup the spline TTree
    TTree *weighttree = new TTree("weight_tree", "weight_tree");
    splwrite->AddWeightsToTree(weighttree);

    // Make container for all weights
    int nweights = splwrite->GetNWeights();
    // int npar = splwrite->GetNPars();
    // double* weightcont = new double[nweights];

    int lasttime = time(NULL);

    // Load N Chunks of the Weights into Memory
    // Split into N processing chunks
    int nchunks = FitPar::Config().GetParI("spline_chunks");
    if (nchunks <= 0)
      nchunks = 1;
    if (nchunks >= nevents / 2)
      nchunks = nevents / 2;

    std::cout << "Starting NChunks " << nchunks << std::endl;
    for (int ichunk = 0; ichunk < nchunks; ichunk++) {

      // Skip to only do one processing chunk
      if (procchunk != -1 and procchunk != ichunk)
        continue;

      NUIS_LOG(FIT, "On Processing Chunk " << ichunk);
      int neventsinchunk = nevents / nchunks;
      int loweventinchunk = neventsinchunk * ichunk;
      // int higheventinchunk = neventsinchunk * (ichunk + 1);

      double **allweightcont = new double *[neventsinchunk];
      for (int k = 0; k < neventsinchunk; k++) {
        allweightcont[k] = new double[nweights];
      }

      // Start Set Processing Here.
      for (int iset = 0; iset < nweights; iset++) {

        splwrite->ReconfigureSet(iset);

        // Could reorder this to save the weightconts in order instead of
        // reconfiguring per event. Loop over all events and fill the TTree
        for (int i = 0; i < neventsinchunk; i++) {

          nuisevent = input->GetNuisanceEvent(i + loweventinchunk);
          double w = splwrite->GetWeightForThisSet(nuisevent);

          if (iset == 0) {
            allweightcont[i][0] = w;
          } else {
            allweightcont[i][iset] = w / allweightcont[i][0];
          }

          // Save everything
          if (iset == 0) {
            eventtree->Fill();
          }
        }

        std::ostringstream timestring;
        int timeelapsed = time(NULL) - lasttime;
        if (timeelapsed) {
          lasttime = time(NULL);

          int setsleft =
              (nweights - iset - 1) + (nweights * (nchunks - ichunk - 1));
          float proj = (float(setsleft) * timeelapsed) / 60 / 60;
          timestring << setsleft << " sets remaining. Last one took "
                     << timeelapsed << ". " << proj << " hours remaining.";
        }

        NUIS_LOG(REC, "Processed Set " << iset << "/" << nweights << " in chunk "
                                   << ichunk << "/" << nchunks << " "
                                   << timestring.str());
      }

      // Fill weights for this chunk into the TTree
      for (int k = 0; k < neventsinchunk; k++) {
        splwrite->SetWeights(allweightcont[k]);
        weighttree->Fill();
      }
    }

    // at end of the chunk, when all sets have been done
    // loop over the container and fill weights to ttree

    outputfile->cd();
    eventtree->Write();
    weighttree->Write();
    input->GetFluxHistogram()->Write("nuisance_fluxhist");
    input->GetEventHistogram()->Write("nuisance_eventhist");
    splwrite->Write("spline_reader");
    outputfile->Close();

    // Close Output
    outputfile->Close();

    // Delete Inputs
    delete input;
  }

  // remove Keys
  eventkeys.clear();
}

//*************************************
void SplineRoutines::GenerateEventWeights() {
  //*************************************
  if (fRW)
    delete fRW;
  SetupRWEngine();

  // Setup the spline reader
  SplineWriter *splwrite = new SplineWriter(fRW);
  std::vector<nuiskey> splinekeys = Config::QueryKeys("spline");

  // Add splines to splinewriter
  for (std::vector<nuiskey>::iterator iter = splinekeys.begin();
       iter != splinekeys.end(); iter++) {
    nuiskey splkey = (*iter);

    // Add Spline Info To Reader
    splwrite->AddSpline(splkey);
  }
  splwrite->SetupSplineSet();

  // Event Loop
  // Loop over all events and calculate weights for each parameter set.

  // Generate a set of nominal events
  // Method, Loop over inputs, create input handler, then create a ttree
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (size_t i = 0; i < eventkeys.size(); i++) {
    nuiskey key = eventkeys.at(i);

    // Get I/O
    std::string inputfilename = key.GetS("input");
    if (inputfilename.empty()) {
      NUIS_ABORT("No input given for set of input events!");
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()) {
      outputfilename = inputfilename + ".nuisance.root";
      NUIS_ERR(WRN, "No output give for set of output events! Saving to "
                      << outputfilename);
    }
    outputfilename += ".weights.root";

    // Make new outputfile
    TFile *outputfile = new TFile(outputfilename.c_str(), "RECREATE");
    outputfile->cd();

    // Make a new input handler
    std::vector<std::string> file_descriptor =
        GeneralUtils::ParseToStr(inputfilename, ":");
    if (file_descriptor.size() != 2) {
      NUIS_ABORT("File descriptor had no filetype declaration: \""
             << inputfilename << "\". expected \"FILETYPE:file.root\"");
    }
    InputUtils::InputType inptype =
        InputUtils::ParseInputType(file_descriptor[0]);

    InputHandlerBase *input = InputUtils::CreateInputHandler(
        "eventsaver", inptype, file_descriptor[1]);

    // Get info from inputhandler
    int nevents = input->GetNEvents();
    int countwidth = (nevents / 1000);
    FitEvent *nuisevent = input->FirstNuisanceEvent();

    // Setup a TTree to save the event
    outputfile->cd();
    TTree *eventtree = new TTree("nuisance_events", "nuisance_events");

    // Add a flag that allows just splines to be saved.
    nuisevent->AddBranchesToTree(eventtree);

    // Save the spline reader
    splwrite->Write("spline_reader");

    // Setup the spline TTree
    TTree *weighttree = new TTree("weight_tree", "weight_tree");
    splwrite->AddWeightsToTree(weighttree);

    // Make container for all weights
    int nweights = splwrite->GetNWeights();
    // int npar = splwrite->GetNPars();
    double *weightcont = new double[nweights];

    int lasttime = time(NULL);

    // Could reorder this to save the weightconts in order instead of
    // reconfiguring per event. Loop over all events and fill the TTree
    while (nuisevent) {

      // Calculate the weights for each parameter set
      splwrite->GetWeightsForEvent(nuisevent, weightcont);

      // Save everything

      eventtree->Fill();
      weighttree->Fill();

      // Logging
      if (i % countwidth == 0) {

        std::ostringstream timestring;
        int timeelapsed = time(NULL) - lasttime;
        if (i != 0 and timeelapsed) {
          lasttime = time(NULL);

          int eventsleft = nevents - i;
          float speed = float(countwidth) / float(timeelapsed);
          float proj = (float(eventsleft) / float(speed)) / 60 / 60;
          timestring << proj << " hours remaining.";
        }
        NUIS_LOG(REC, "Saved " << i << "/" << nevents
                           << " nuisance spline weights. " << timestring.str());
      }

      // Iterate
      i++;
      nuisevent = input->NextNuisanceEvent();
    }

    // at end of the chunk, when all sets have been done
    // loop over the container and fill weights to ttree

    outputfile->cd();
    eventtree->Write();
    weighttree->Write();
    input->GetFluxHistogram()->Write("nuisance_fluxhist");
    input->GetEventHistogram()->Write("nuisance_eventhist");
    splwrite->Write("spline_reader");
    outputfile->Close();

    // Close Output
    outputfile->Close();

    // Delete Inputs
    delete input;
  }

  // remove Keys
  eventkeys.clear();
}

//*************************************
void SplineRoutines::GenerateEventSplines() {
  //*************************************
  if (fRW)
    delete fRW;
  SetupRWEngine();

  // Setup the spline reader
  SplineWriter *splwrite = new SplineWriter(fRW);
  std::vector<nuiskey> splinekeys = Config::QueryKeys("spline");

  // Add splines to splinewriter
  for (std::vector<nuiskey>::iterator iter = splinekeys.begin();
       iter != splinekeys.end(); iter++) {
    nuiskey splkey = (*iter);

    // Add Spline Info To Reader
    splwrite->AddSpline(splkey);
  }
  splwrite->SetupSplineSet();

  // Make an ugly list for N cores
  int ncores = FitPar::Config().GetParI("NCORES"); // omp_get_max_threads();
  std::vector<SplineWriter *> splwriterlist;

  for (int i = 0; i < ncores; i++) {
    SplineWriter *tmpwriter = new SplineWriter(fRW);

    for (std::vector<nuiskey>::iterator iter = splinekeys.begin();
         iter != splinekeys.end(); iter++) {
      nuiskey splkey = (*iter);

      // Add Spline Info To Reader
      tmpwriter->AddSpline(splkey);
    }
    tmpwriter->SetupSplineSet();

    splwriterlist.push_back(tmpwriter);
  }

  // Event Loop
  // Loop over all events and calculate weights for each parameter set.

  // Generate a set of nominal events
  // Method, Loop over inputs, create input handler, then create a ttree
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (size_t i = 0; i < eventkeys.size(); i++) {
    nuiskey key = eventkeys.at(i);

    // Get I/O
    std::string inputfilename = key.GetS("input");
    if (inputfilename.empty()) {
      NUIS_ABORT("No input given for set of input events!");
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()) {
      outputfilename = inputfilename + ".nuisance.root";
      NUIS_ERR(WRN, "No output give for set of output events! Saving to "
                      << outputfilename);
    }

    // Make new outputfile
    TFile *outputfile = new TFile(outputfilename.c_str(), "RECREATE");
    outputfile->cd();

    // Make a new input handler
    std::vector<std::string> file_descriptor =
        GeneralUtils::ParseToStr(inputfilename, ":");
    if (file_descriptor.size() != 2) {
      NUIS_ABORT("File descriptor had no filetype declaration: \""
             << inputfilename << "\". expected \"FILETYPE:file.root\"");
    }
    InputUtils::InputType inptype =
        InputUtils::ParseInputType(file_descriptor[0]);

    InputHandlerBase *input = InputUtils::CreateInputHandler(
        "eventsaver", inptype, file_descriptor[1]);

    // Get info from inputhandler
    int nevents = input->GetNEvents();
    int countwidth = (nevents / 1000);
    FitEvent *nuisevent = input->FirstNuisanceEvent();

    // Setup a TTree to save the event
    outputfile->cd();
    TTree *eventtree = new TTree("nuisance_events", "nuisance_events");

    // Add a flag that allows just splines to be saved.
    nuisevent->AddBranchesToTree(eventtree);

    // Save the spline reader
    splwrite->Write("spline_reader");

    // Setup the spline TTree
    TTree *weighttree = new TTree("weight_tree", "weight_tree");
    splwrite->AddWeightsToTree(weighttree);

    // Make container for all weights
    int nweights = splwrite->GetNWeights();
    double **weightcont = new double *[nevents];
    for (int k = 0; k < nevents; k++) {
      weightcont[k] = new double[nweights];
    }

    int npar = splwrite->GetNPars();

    int lasttime = time(NULL);

    // Could reorder this to save the weightconts in order instead of
    // reconfiguring per event. Loop over all events and fill the TTree
    while (nuisevent) {

      // std::cout << "Fitting event " << i << std::endl;
      // Calculate the weights for each parameter set
      // splwrite->FitSplinesForEvent(nuisevent);
      splwrite->GetWeightsForEvent(nuisevent, weightcont[i]);
      bool hasresponse = false;
      for (int j = 0; j < nweights; j++) {

        if (weightcont[i][j] != 1.0) {
          //    std::cout << "Non Zero Weight at " << i << " " << j <<
          //    std::endl;
          hasresponse = true;
        } else {
          //    std::cout << "Empty Weight at " << i << " " << j << std::endl;
        }
      }
      if (!hasresponse) {
        //  std::cout << "Deleting flat response " << nuisevent->Mode <<
        //  std::endl;
        delete weightcont[i];
        weightcont[i] = NULL;
      }

      // Save everything
      eventtree->Fill();
      weighttree->Fill();
      // splinetree->Fill();

      // nuisevent->Print();
      // std::cout << "Done with event " << i << std::endl;

      // Push weight sets into a the array

      // sleep(4);
      // Logging
      if (i % countwidth == 0) {

        std::ostringstream timestring;
        int timeelapsed = time(NULL) - lasttime;
        if (i != 0 and timeelapsed) {
          lasttime = time(NULL);

          int eventsleft = nevents - i;
          float speed = float(countwidth) / float(timeelapsed);
          float proj = (float(eventsleft) / float(speed)) / 60 / 60;
          timestring << proj << " hours remaining.";
        }
        NUIS_LOG(REC, "Saved " << i << "/" << nevents
                           << " nuisance spline weights. " << timestring.str());
      }

      // Iterate
      i++;
      nuisevent = input->NextNuisanceEvent();
    }

    outputfile->cd();
    eventtree->Write();
    weighttree->Write();
    input->GetFluxHistogram()->Write("nuisance_fluxhist");
    input->GetEventHistogram()->Write("nuisance_eventhist");
    outputfile->Close();

    outputfile = new TFile(outputfilename.c_str(), "UPDATE");
    outputfile->cd();

    weighttree = (TTree *)outputfile->Get("weight_tree");
    //    splwrite->ReadWeightsFromTree(weighttree);

    // Divide weights container into Ncores.
    // Parrallelise this loop checking for what core we are on.
    // for (int i = 0; i < nevents; i++){
    // splwriterlist[int(i / (nevents/4))]->FitSplinesForEvent(coeff);
    // }

    // // Now loop over weights tree
    // for (int i = 0; i < weighttree->GetEntries(); i++) {
    //   weighttree->GetEntry(i);
    //   splwrite->FitSplinesForEvent();
    //   splinetree->Fill();

    //   if (i % countwidth == 0) {

    //     std::ostringstream timestring;
    //     int timeelapsed = time(NULL) - lasttime;
    //     if (i != 0 and timeelapsed) {
    //       lasttime = time(NULL);

    //       int eventsleft = nevents - i;
    //       float speed = float(countwidth) / float(timeelapsed);
    //       float proj = (float(eventsleft) / float(speed)) / 60 / 60;
    //       timestring << proj << " hours remaining.";

    //     }
    //     LOG(REC) << "Built " << i << "/" << nevents << " nuisance spline
    //     events. " << timestring.str() << std::endl;
    //   }
    // }

    // Get Splines
    float **allcoeff = new float *[nevents];
    for (int k = 0; k < nevents; k++) {
      allcoeff[k] = new float[npar];
    }

    //    #pragma omp parallel for num_threads(ncores)
    for (int i = 0; i < nevents; i++) {

      //#pragma omp atomic
      //      printf("Using Thread %d to build event %d \n",
      //      int(omp_get_thread_num()), (int)i ); std::cout<< " -> Writer = "
      //      << splwriterlist[ i / (nevents/ncores) ] << std::endl;

      //      #pragma omp atomic
      if (weightcont[i]) {
        splwriterlist[int(omp_get_thread_num())]->FitSplinesForEvent(
            weightcont[i], allcoeff[i]);
      } else {
        for (int j = 0; j < npar; j++) {
          allcoeff[i][j] = float(0.0);
        }
      }

      //      splwrite->FitSplinesForEvent(weightcont[i], allcoeff[i]);

      if (i % 500 == 0) {

        if (LOG_LEVEL(REC)) {
          printf("Using Thread %d to build event %d \n",
                 int(omp_get_thread_num()), (int)i);
        }
      }
      /*

        std::ostringstream timestring;
        int timeelapsed = time(NULL) - lasttime;
        if (i != 0 and timeelapsed) {
          lasttime = time(NULL);

          int eventsleft = nevents - i;
          float speed = float(countwidth) / float(timeelapsed);
          float proj = (float(eventsleft) / float(speed)) / 60 / 60;
          timestring << proj << " hours remaining.";
      timestring << " Using Writer at " << i / (nevents/ncores) << " = " <<
      splwriterlist[ i / (nevents/ncores) ] << std::endl;

        }
        LOG(REC) << "Built " << i << "/" << nevents << " nuisance spline events.
      " << timestring.str() << std::endl;
      }
      */
    }

    // Save Splines into TTree
    float *coeff = new float[npar];
    outputfile->cd();
    TTree *splinetree = new TTree("spline_tree", "spline_tree");

    splinetree->Branch("SplineCoeff", coeff, Form("SplineCoeff[%d]/F", npar));

    std::cout << "Saving to the allcoeff" << std::endl;
    for (int k = 0; k < nevents; k++) {
      for (int l = 0; l < npar; l++) {
        coeff[l] = allcoeff[k][l];
      }
      std::cout << "Coeff 0, 1, 2 = " << coeff[0] << " " << coeff[1] << " "
                << coeff[2] << std::endl;
      splinetree->Fill();
    }

    // Save flux and close file
    outputfile->cd();
    splinetree->Write();

    // Delete the container.
    for (int k = 0; k < nevents; k++) {
      delete weightcont[k];
    }
    delete weightcont;
    delete coeff;

    // Close Output
    outputfile->Close();

    // Delete Inputs
    delete input;
  }

  // remove Keys
  eventkeys.clear();
}

//*************************************
void SplineRoutines::BuildEventSplines(int procchunk) {
  //*************************************
  if (fRW)
    delete fRW;
  SetupRWEngine();

  // Setup the spline reader
  SplineWriter *splwrite = new SplineWriter(fRW);
  std::vector<nuiskey> splinekeys = Config::QueryKeys("spline");

  // Add splines to splinewriter
  for (std::vector<nuiskey>::iterator iter = splinekeys.begin();
       iter != splinekeys.end(); iter++) {
    nuiskey splkey = (*iter);

    // Add Spline Info To Reader
    splwrite->AddSpline(splkey);
  }
  splwrite->SetupSplineSet();

  // Make an ugly list for N cores
  int ncores =
      FitPar::Config().GetParI("spline_cores"); // omp_get_max_threads();
  if (ncores > omp_get_max_threads())
    ncores = omp_get_max_threads();
  if (ncores <= 0)
    ncores = 1;

  std::vector<SplineWriter *> splwriterlist;

  for (int i = 0; i < ncores; i++) {
    SplineWriter *tmpwriter = new SplineWriter(fRW);

    for (std::vector<nuiskey>::iterator iter = splinekeys.begin();
         iter != splinekeys.end(); iter++) {
      nuiskey splkey = (*iter);

      // Add Spline Info To Reader
      tmpwriter->AddSpline(splkey);
    }
    tmpwriter->SetupSplineSet();

    splwriterlist.push_back(tmpwriter);
  }

  // Event Loop
  // Loop over all events and calculate weights for each parameter set.

  // Generate a set of nominal events
  // Method, Loop over inputs, create input handler, then create a ttree
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (size_t i = 0; i < eventkeys.size(); i++) {
    nuiskey key = eventkeys.at(i);

    // Get I/O
    std::string inputfilename = key.GetS("input");
    if (inputfilename.empty()) {
      NUIS_ABORT("No input given for set of input events!");
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()) {
      outputfilename = inputfilename + ".nuisance.root";
      NUIS_ERR(WRN, "No output give for set of output events! Saving to "
                      << outputfilename);
    }

    // Make new outputfile
    TFile *outputfile;
    if (procchunk == -1)
      outputfile = new TFile(outputfilename.c_str(), "RECREATE");
    else
      outputfile = new TFile(
          (outputfilename + std::string(Form(".coeffchunk_%d.root", procchunk)))
              .c_str(),
          "RECREATE");

    outputfile->cd();

    // Get Weights File
    TFile *weightsfile =
        new TFile((outputfilename + ".weights.root").c_str(), "READ");
    TTree *weighttree = (TTree *)weightsfile->Get("weight_tree");

    // Get SPLWRite Info
    // splwrite->ReadWeightsFromTree(weighttree);
    int nevents = weighttree->GetEntries();
    // int countwidth = (nevents / 1000);
    int nweights = splwrite->GetNWeights();
    int npar = splwrite->GetNPars();

    // Access Weights
    double *eventweights = new double[nweights];
    weighttree->SetBranchAddress("SplineWeights", eventweights);

    // Make counter
    // int lasttime = time(NULL);

    // Setup Splines To Be Saved into TTree
    outputfile->cd();
    TTree *splinetree = new TTree("spline_tree", "spline_tree");

    float *coeff = new float[npar];
    splinetree->Branch("SplineCoeff", coeff, Form("SplineCoeff[%d]/F", npar));

    // Load N Chunks of the Weights into Memory
    // Split into N processing chunks
    int nchunks = FitPar::Config().GetParI("spline_chunks");
    if (nchunks <= 0)
      nchunks = 1;
    if (nchunks >= nevents / 2)
      nchunks = nevents / 2;

    std::cout << "Starting NChunks " << nchunks << std::endl;
    sleep(1);
    for (int ichunk = 0; ichunk < nchunks; ichunk++) {

      // Skip to only do one processing chunk
      if (procchunk != -1 and procchunk != ichunk)
        continue;

      NUIS_LOG(FIT, "On Processing Chunk " << ichunk);
      int neventsinchunk = nevents / nchunks;
      int loweventinchunk = neventsinchunk * ichunk;
      // int higheventinchunk = neventsinchunk * (ichunk + 1);

      // Build Chunk Containers for Event Weights
      double **weightcont = new double *[nevents];
      float **allcoeff = new float *[nevents];

      // Load Chunks into Containers
      for (int k = 0; k < neventsinchunk; k++) {
        weighttree->GetEntry(loweventinchunk + k);

        weightcont[k] = new double[nweights];
        allcoeff[k] = new float[npar];

        bool hasresponse = false;
        for (int j = 0; j < nweights; j++) {
          weightcont[k][j] = eventweights[j];
          if (eventweights[j] != 1.0)
            hasresponse = true;
        }
        if (!hasresponse)
          delete weightcont[k];
      }

      // Loop over ncores and process chunks
      //      #pragma omp parallel for num_threads(ncores)
      for (int k = 0; k < neventsinchunk; k++) {

        if (weightcont[k]) {
          splwriterlist[int(omp_get_thread_num())]->FitSplinesForEvent(
              weightcont[k], allcoeff[k]);
        } else {
          for (int j = 0; j < npar; j++) {
            allcoeff[k][j] = float(0.0);
          }
        }

        if (k + loweventinchunk % 500 == 0) {

          if (LOG_LEVEL(REC)) {
            printf("Using Thread %d to build event %d in chunk %d \n",
                   int(omp_get_thread_num()), (int)loweventinchunk + k, ichunk);
          }
        }
      }

      // Save Coeff To Tree
      std::cout << "Saving coeffs to Tree in Chunk " << ichunk << std::endl;
      for (int k = 0; k < neventsinchunk; k++) {
        for (int l = 0; l < npar; l++) {
          coeff[l] = allcoeff[k][l];
        }
        // std::cout << "Coeff 0, 1, 2 = " << coeff[0] << " " << coeff[1] << " "
        // << coeff[2] << std::endl;
        splinetree->Fill();
      }

      // Delete the container.
      for (int k = 0; k < neventsinchunk; k++) {
        if (weightcont[k])
          delete weightcont[k];
        if (allcoeff[k])
          delete allcoeff[k];
      }
      delete allcoeff;
      delete weightcont;
    }
    // Save flux and close file
    outputfile->cd();
    splinetree->Write();

    if (procchunk == -1 or procchunk == 0) {
      outputfile->cd();
      splwrite->Write("spline_reader");

      TTree *nuisanceevents = (TTree *)weightsfile->Get("nuisance_events");
      nuisanceevents->CloneTree()->Write();
      weighttree->CloneTree()->Write();

      TH1D *nuisance_fluxhist = (TH1D *)weightsfile->Get("nuisance_fluxhist");
      TH1D *nuisance_eventhist = (TH1D *)weightsfile->Get("nuisance_eventhist");
      nuisance_fluxhist->Write("nuisance_fluxhist");
      nuisance_eventhist->Write("nuisance_eventhist");
    }
    weightsfile->Close();

    // Add option to build seperate chunks

    // Close Output
    outputfile->Close();
  }

  // remove Keys
  eventkeys.clear();
}

void SplineRoutines::MergeEventSplinesChunks() {

  if (fRW)
    delete fRW;
  SetupRWEngine();

  // Setup the spline reader
  SplineWriter *splwrite = new SplineWriter(fRW);
  std::vector<nuiskey> splinekeys = Config::QueryKeys("spline");

  // Add splines to splinewriter
  for (std::vector<nuiskey>::iterator iter = splinekeys.begin();
       iter != splinekeys.end(); iter++) {
    nuiskey splkey = (*iter);

    // Add Spline Info To Reader
    splwrite->AddSpline(splkey);
  }
  splwrite->SetupSplineSet();

  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (size_t i = 0; i < eventkeys.size(); i++) {
    nuiskey key = eventkeys.at(i);

    // Get I/O
    std::string inputfilename = key.GetS("input");
    if (inputfilename.empty()) {
      NUIS_ABORT("No input given for set of input events!");
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()) {
      outputfilename = inputfilename + ".nuisance.root";
      NUIS_ERR(WRN, "No output give for set of output events! Saving to "
                      << outputfilename);
    }

    // Make new outputfile
    TFile *outputfile = new TFile(outputfilename.c_str(), "RECREATE");
    outputfile->cd();

    // Get Weights File
    TFile *weightsfile =
        new TFile((outputfilename + ".weights.root").c_str(), "READ");
    TTree *weighttree = (TTree *)weightsfile->Get("weight_tree");

    // Get SPLWRite Info
    // splwrite->ReadWeightsFromTree(weighttree);
    int nevents = weighttree->GetEntries();
    // int countwidth = (nevents / 1000);
    // int nweights = splwrite->GetNWeights();
    int npar = splwrite->GetNPars();

    // Make counter
    // int lasttime = time(NULL);

    // Setup Splines To Be Saved into TTree
    outputfile->cd();
    TTree *splinetree = new TTree("spline_tree", "spline_tree");

    float *coeff = new float[npar];
    splinetree->Branch("SplineCoeff", coeff, Form("SplineCoeff[%d]/F", npar));

    // Load N Chunks of the Weights into Memory
    // Split into N processing chunks
    int nchunks = FitPar::Config().GetParI("spline_chunks");
    if (nchunks <= 0)
      nchunks = 1;
    if (nchunks >= nevents / 2)
      nchunks = nevents / 2;
    int neventsinchunk = nevents / nchunks;

    for (int ichunk = 0; ichunk < nchunks; ichunk++) {

      // Get Output File
      TFile *chunkfile = new TFile(
          (outputfilename + std::string(Form(".coeffchunk_%d.root", ichunk)))
              .c_str());

      // Get TTree for spline coeffchunk
      TTree *splinetreechunk = (TTree *)chunkfile->Get("spline_tree");

      // Set Branch Address to coeffchunk
      float *coeffchunk = new float[npar];
      splinetreechunk->SetBranchAddress("SplineCoeff", coeffchunk);

      // Loop over nevents in chunk
      for (int k = 0; k < neventsinchunk; k++) {
        splinetreechunk->GetEntry(k);
        for (int j = 0; j < npar; j++) {
          coeff[j] = coeffchunk[j];
        }
        splinetree->Fill();
      }

      // Close up
      chunkfile->Close();
      delete coeffchunk;

      std::cout << "Merged chunk " << ichunk << std::endl;
    }

    // Save flux and close file
    outputfile->cd();
    splinetree->Write();

    outputfile->cd();
    splwrite->Write("spline_reader");

    TTree *nuisanceevents = (TTree *)weightsfile->Get("nuisance_events");
    nuisanceevents->CloneTree()->Write();
    weighttree->CloneTree()->Write();

    TH1D *nuisance_fluxhist = (TH1D *)weightsfile->Get("nuisance_fluxhist");
    TH1D *nuisance_eventhist = (TH1D *)weightsfile->Get("nuisance_eventhist");
    nuisance_fluxhist->Write("nuisance_fluxhist");
    nuisance_eventhist->Write("nuisance_eventhist");

    weightsfile->Close();

    // Add option to build seperate chunks

    // Close Output
    outputfile->Close();
  }

  // remove Keys
  eventkeys.clear();
}

// void SplineRoutines::BuildSplineChunk(){
//}

// void SplineRoutines::MergeSplineChunks(){
//}

//*************************************
void SplineRoutines::MergeSplines() {
  //*************************************
  // Loop over all 'splinemerge' keys.
  // Add them to the Merger.
  // Call setup splines.

  // Get the key with eventinput
  // - remaining keys should have splineinput
  // - Loop over number of entries.
  // - FillEntry in merger.
  // - Fill NUISANCEEvent into a new TTree.

  SplineMerger *splmerge = new SplineMerger();
  std::vector<nuiskey> splinekeys = Config::QueryKeys("splinemerge");
  for (std::vector<nuiskey>::iterator iter = splinekeys.begin();
       iter != splinekeys.end(); iter++) {
    nuiskey splkey = (*iter);

    TFile *infile = new TFile(splkey.GetS("input").c_str(), "READ");
    splmerge->AddSplineSetFromFile(infile);
  }
  splmerge->SetupSplineSet();

  // Now get Event File
  std::vector<nuiskey> eventkeys = Config::QueryKeys("eventmerge");
  nuiskey key = eventkeys[0];

  std::string inputfilename = key.GetS("input");

  // Make a new input handler
  std::vector<std::string> file_descriptor =
      GeneralUtils::ParseToStr(inputfilename, ":");
  if (file_descriptor.size() != 2) {
    NUIS_ABORT("File descriptor had no filetype declaration: \""
           << inputfilename << "\". expected \"FILETYPE:file.root\"");
  }
  InputUtils::InputType inptype =
      InputUtils::ParseInputType(file_descriptor[0]);

  InputHandlerBase *input =
      InputUtils::CreateInputHandler("eventsaver", inptype, file_descriptor[1]);

  std::string outputfilename = key.GetS("output");
  if (outputfilename.empty()) {
    outputfilename = inputfilename + ".nuisance.root";
    NUIS_ERR(WRN, "No output give for set of output events! Saving to "
                    << outputfilename);
  }

  // Make new outputfile
  TFile *outputfile = new TFile(outputfilename.c_str(), "RECREATE");
  outputfile->cd();

  // Get info from inputhandler
  int nevents = input->GetNEvents();
  int countwidth = (nevents / 1000);
  FitEvent *nuisevent = input->FirstNuisanceEvent();

  // Setup a TTree to save the event
  outputfile->cd();
  TTree *eventtree = new TTree("nuisance_events", "nuisance_events");

  // Add a flag that allows just splines to be saved.
  nuisevent->AddBranchesToTree(eventtree);

  // Save the spline reader
  splmerge->Write("spline_reader");

  // Setup the spline TTree
  TTree *splinetree = new TTree("spline_tree", "spline_tree");
  splmerge->AddCoefficientsToTree(splinetree);

  int lasttime = time(NULL);
  int i = 0;
  // Loop over all events and fill the TTree
  while (nuisevent) {

    // Calculate the weights for each parameter set
    splmerge->FillMergedSplines(i);

    // Save everything
    eventtree->Fill();
    splinetree->Fill();

    // Logging
    if (i % countwidth == 0) {

      std::ostringstream timestring;
      int timeelapsed = time(NULL) - lasttime;
      if (i != 0 and timeelapsed) {
        lasttime = time(NULL);

        int eventsleft = nevents - i;
        float speed = float(countwidth) / float(timeelapsed);
        float proj = (float(eventsleft) / float(speed)) / 60 / 60;
        timestring << proj << " hours remaining.";
      }
      NUIS_LOG(REC, "Saved " << i << "/" << nevents << " nuisance spline events. "
                         << timestring.str());
    }

    // Iterate
    i++;
    nuisevent = input->NextNuisanceEvent();
  }

  // Save flux and close file
  outputfile->cd();
  eventtree->Write();
  splinetree->Write();

  input->GetFluxHistogram()->Write("nuisance_fluxhist");
  input->GetEventHistogram()->Write("nuisance_eventhist");

  // Close Output
  outputfile->Close();

  // Delete Inputs
  delete input;
}

//*************************************
void SplineRoutines::TestSplines_1DEventScan() {
  //*************************************

  // Setup RW Engine
  if (fRW)
    delete fRW;
  SetupRWEngine();

  // Make a spline RW Engine too.
  FitWeight *splweight = new FitWeight("splinerwaweight");
  // std::vector<nuiskey> splinekeys    = Config::QueryKeys("spline");
  std::vector<nuiskey> parameterkeys = Config::QueryKeys("parameter");
  TH1D *parhisttemplate = new TH1D("parhist", "parhist", parameterkeys.size(),
                                   0.0, float(parameterkeys.size()));

  // Add Parameters
  for (size_t i = 0; i < parameterkeys.size(); i++) {
    nuiskey key = parameterkeys[i];

    std::string parname = key.GetS("name");
    std::string partype = key.GetS("type");
    double nom = key.GetD("nominal");

    parhisttemplate->SetBinContent(i + 1, nom);
    parhisttemplate->GetXaxis()->SetBinLabel(i + 1, parname.c_str());

    splweight->IncludeDial(key.GetS("name"), kSPLINEPARAMETER, nom);
    splweight->SetDialValue(key.GetS("name"), key.GetD("nominal"));
  }
  splweight->Reconfigure();

  // Make a high resolution spline set.
  std::vector<double> nomvals = fRW->GetDialValues();
  // int testres = FitPar::Config().GetParI("spline_test_resolution");

  std::vector<std::vector<double> > scanparset_vals;
  std::vector<TH1D *> scanparset_hists;

  // Loop over all params
  // Add Parameters
  for (size_t i = 0; i < parameterkeys.size(); i++) {
    nuiskey key = parameterkeys[i];

    // Get Par Name
    std::string name = key.GetS("name");

    if (!key.Has("low") or !key.Has("high") or !key.Has("step")) {
      continue;
    }

    // Push Back Scan
    double low = key.GetD("low");
    double high = key.GetD("high");
    double cur = low;
    double step = key.GetD("step");

    while (cur <= high) {

      // Make new set
      std::vector<double> newvals = nomvals;
      newvals[i] = cur;

      // Add to vects
      scanparset_vals.push_back(newvals);

      TH1D *parhist = (TH1D *)parhisttemplate->Clone();
      for (size_t j = 0; j < newvals.size(); j++) {
        parhist->SetBinContent(j + 1, newvals[j]);
      }
      scanparset_hists.push_back(parhist);

      // Move to next one
      cur += step;
    }
  }

  // Print out the parameter set to test
  for (uint i = 0; i < scanparset_vals.size(); i++) {
    std::cout << "Parset " << i;
    for (uint j = 0; j < scanparset_vals[i].size(); j++) {
      std::cout << " " << scanparset_vals[i][j];
    }
    std::cout << std::endl;
  }

  // Weight holders
  double *rawweights = new double[scanparset_vals.size()];
  double *splweights = new double[scanparset_vals.size()];
  double *difweights = new double[scanparset_vals.size()];
  int nweights = scanparset_vals.size();
  // int NParSets = scanparset_vals.size();

  // Loop over all event I/O
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (size_t i = 0; i < eventkeys.size(); i++) {
    nuiskey key = eventkeys.at(i);

    // Get I/O
    std::string inputfilename = key.GetS("input");
    if (inputfilename.empty()) {
      NUIS_ABORT("No input given for set of input events!")
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()) {
      outputfilename = inputfilename + ".nuisance.root";
      NUIS_ERR(WRN, "No output give for set of output events! Saving to "
                      << outputfilename);
    }

    // Make a new input handler
    std::vector<std::string> file_descriptor =
        GeneralUtils::ParseToStr(inputfilename, ":");
    if (file_descriptor.size() != 2) {
      NUIS_ABORT("File descriptor had no filetype declaration: \""
             << inputfilename << "\". expected \"FILETYPE:file.root\"");
    }
    InputUtils::InputType inptype =
        InputUtils::ParseInputType(file_descriptor[0]);

    // Make handlers for input and output
    InputHandlerBase *input = InputUtils::CreateInputHandler(
        "rawevents", inptype, file_descriptor[1]);
    InputHandlerBase *output = InputUtils::CreateInputHandler(
        "splineevents", InputUtils::kEVSPLN_Input, outputfilename);

    // Get Base Events for each case.
    FitEvent *rawevent = input->FirstNuisanceEvent();
    FitEvent *splevent = output->FirstNuisanceEvent();

    // Setup outputfile
    std::string outputtest = outputfilename + ".splinetest.1DEventScan.root";
    TFile *outputtestfile = new TFile(outputtest.c_str(), "RECREATE");
    outputtestfile->cd();

    // Save Parameter Sets
    for (size_t i = 0; i < scanparset_hists.size(); i++) {
      scanparset_hists[i]->Write(Form("Paramater_Set_%i", (int)i));
    }

    // Save a TTree of weights and differences.
    TTree *weighttree = new TTree("weightscan", "weightscan");

    // Make a branch for each weight set
    for (size_t i = 0; i < scanparset_hists.size(); i++) {
      weighttree->Branch(Form("RawWeights_Set_%i", (int)i), &rawweights[i],
                         Form("RawWeights_Set_%i/D", (int)i));
      weighttree->Branch(Form("SplineWeights_Set_%i", (int)i), &splweights[i],
                         Form("SplineWeights_Set_%i/D", (int)i));
      weighttree->Branch(Form("DifWeights_Set_%i", (int)i), &difweights[i],
                         Form("DifWeights_Set_%i/D", (int)i));
    }

    // Count
    // int i = 0;
    int nevents = input->GetNEvents();
    int lasttime = time(NULL);

    // Load N Chunks of the Weights into Memory
    // Split into N processing chunks
    int nchunks = FitPar::Config().GetParI("spline_chunks");
    if (nchunks <= 0)
      nchunks = 1;
    if (nchunks >= nevents / 2)
      nchunks = nevents / 2;

    std::cout << "Starting NChunks " << nchunks << std::endl;
    for (int ichunk = 0; ichunk < nchunks; ichunk++) {

      // Skip to only do one processing chunk
      //      if (procchunk != -1 and procchunk != ichunk) continue;

      NUIS_LOG(FIT, "On Processing Chunk " << ichunk);
      int neventsinchunk = nevents / nchunks;
      int loweventinchunk = neventsinchunk * ichunk;
      // int higheventinchunk = neventsinchunk * (ichunk + 1);

      double **allrawweights = new double *[neventsinchunk];
      double **allsplweights = new double *[neventsinchunk];
      double **alldifweights = new double *[neventsinchunk];
      for (int k = 0; k < neventsinchunk; k++) {
        allrawweights[k] = new double[nweights];
        allsplweights[k] = new double[nweights];
        alldifweights[k] = new double[nweights];
      }

      // Start Set Processing Here.
      for (int iset = 0; iset < nweights; iset++) {

        // Reconfigure
        fRW->SetAllDials(&scanparset_vals[iset][0],
                         scanparset_vals[iset].size());
        fRW->Reconfigure();

        // Reconfigure spline RW
        splweight->SetAllDials(&scanparset_vals[iset][0],
                               scanparset_vals[iset].size());
        splweight->Reconfigure();

        splevent->fSplineRead->SetNeedsReconfigure(true);

        // Could reorder this to save the weightconts in order instead of
        // reconfiguring per event. Loop over all events and fill the TTree
        for (int i = 0; i < neventsinchunk; i++) {

          rawevent = input->GetNuisanceEvent(i + loweventinchunk);
          splevent = output->GetNuisanceEvent(i + loweventinchunk);

          allrawweights[i][iset] = fRW->CalcWeight(rawevent);
          allsplweights[i][iset] = splweight->CalcWeight(splevent);
          alldifweights[i][iset] =
              allsplweights[i][iset] - allrawweights[i][iset];
        }

        std::ostringstream timestring;
        int timeelapsed = time(NULL) - lasttime;
        if (timeelapsed) {
          lasttime = time(NULL);

          int setsleft =
              (nweights - iset - 1) + (nweights * (nchunks - ichunk - 1));
          float proj = (float(setsleft) * timeelapsed) / 60 / 60;
          timestring << setsleft << " sets remaining. Last one took "
                     << timeelapsed << ". " << proj << " hours remaining.";
        }

        NUIS_LOG(REC, "Processed Set " << iset << "/" << nweights << " in chunk "
                                   << ichunk << "/" << nchunks << " "
                                   << timestring.str());
      }

      // Fill weights for this chunk into the TTree
      for (int k = 0; k < neventsinchunk; k++) {
        for (int l = 0; l < nweights; l++) {
          rawweights[l] = allrawweights[k][l];
          splweights[l] = allsplweights[k][l];
          difweights[l] = alldifweights[k][l];
        }
        weighttree->Fill();
      }
    }

    // Loop over nchunks

    // Loop over parameter sets
    // Set All Dials and reconfigure

    // Loop over events in chunk
    // Fill Chunkweightcontainers

    // Once all dials are done, fill the weight tree

    // Iterator to next chunk

    outputtestfile->cd();
    weighttree->Write();
    outputtestfile->Close();
  }
}

/*
  // Make a high resolution spline set.
  std::vector<double> nomvals = fRW->GetDialValues();
  int testres = FitPar::Config().GetParI("spline_test_resolution");

  std::vector< std::vector<double> > scanparset_vals;
  std::vector< TH1D* > scanparset_hists;

  // Loop over all params
  // Add Parameters
  for (size_t i = 0; i < parameterkeys.size(); i++) {
    nuiskey key = parameterkeys[i];

    // Get Par Name
    std::string name = key.GetS("name");

    if (!key.Has("low") or !key.Has("high") or !key.Has("step")) {
      continue;
    }

    // Push Back Scan
    double low  = key.GetD("low");
    double high = key.GetD("high");
    double cur = low;
    double step = key.GetD("step");

    while (cur <= high) {

      // Make new set
      std::vector<double> newvals = nomvals;
      newvals[i] = cur;

      // Add to vects
      scanparset_vals.push_back(newvals);

      TH1D* parhist = (TH1D*)parhisttemplate->Clone();
      for (size_t j = 0; j < newvals.size(); j++) {
        parhist->SetBinContent(j + 1, newvals[j]);
      }
      scanparset_hists.push_back(parhist);


      // Move to next one
      cur += step;
    }
  }

  // Print out the parameter set to test
  for (int i = 0; i < scanparset_vals.size(); i++) {
    std::cout << "Parset " << i;
    for (int j = 0 ; j < scanparset_vals[i].size(); j++) {
      std::cout << " " << scanparset_vals[i][j];
    }
    std::cout << std::endl;
  }


  // Weight holders
  double* rawweights = new double[scanparset_vals.size()];
  double* splweights = new double[scanparset_vals.size()];
  double* difweights = new double[scanparset_vals.size()];

  int NParSets = scanparset_vals.size();

  // Loop over all event I/O
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (size_t i = 0; i < eventkeys.size(); i++) {
    nuiskey key = eventkeys.at(i);

    // Get I/O
    std::string inputfilename  = key.GetS("input");
    if (inputfilename.empty()) {
      ERR(FTL) << "No input given for set of input events!" << std::endl;
      throw;
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()) {
      outputfilename = inputfilename + ".nuisance.root";
      ERR(FTL) << "No output give for set of output events! Saving to "
               << outputfilename << std::endl;
    }

    // Make a new input handler
    std::vector<std::string> file_descriptor =
      GeneralUtils::ParseToStr(inputfilename, ":");
    if (file_descriptor.size() != 2) {
      ERR(FTL) << "File descriptor had no filetype declaration: \"" <<
inputfilename
               << "\". expected \"FILETYPE:file.root\"" << std::endl;
      throw;
    }
    InputUtils::InputType inptype =
      InputUtils::ParseInputType(file_descriptor[0]);


    // Make handlers for input and output
    InputHandlerBase* input  = InputUtils::CreateInputHandler("rawevents",
inptype, file_descriptor[1]); InputHandlerBase* output =
InputUtils::CreateInputHandler("splineevents", InputUtils::kEVSPLN_Input,
outputfilename);

    // Get Base Events for each case.
    FitEvent* rawevent = input->FirstNuisanceEvent();
    FitEvent* splevent = output->FirstNuisanceEvent();


    // Setup outputfile
    std::string outputtest = outputfilename + ".splinetest.1DEventScan.root";
    TFile* outputtestfile = new TFile(outputtest.c_str(), "RECREATE");
    outputtestfile->cd();

    // Save Parameter Sets
    for (size_t i = 0; i < scanparset_hists.size(); i++) {
      scanparset_hists[i]->Write(Form("Paramater_Set_%i", (int)i));
    }

    // Save a TTree of weights and differences.
    TTree* weighttree = new TTree("weightscan", "weightscan");

    // Make a branch for each weight set
    for (size_t i = 0; i < scanparset_hists.size(); i++) {
      weighttree->Branch(Form("RawWeights_Set_%i", (int)i), &rawweights[i],
Form("RawWeights_Set_%i/D", (int)i) );
      weighttree->Branch(Form("SplineWeights_Set_%i", (int)i), &splweights[i],
Form("SplineWeights_Set_%i/D", (int)i) );
      weighttree->Branch(Form("DifWeights_Set_%i", (int)i), &difweights[i],
Form("DifWeights_Set_%i/D", (int)i) );

    }

    // Count
    int i = 0;
    int nevents = input->GetNEvents();
    while (rawevent and splevent) {

      // Loop over 1D parameter sets.
      for (size_t j = 0; j < scanparset_vals.size(); j++) {

        // Reconfigure
        fRW->SetAllDials(&scanparset_vals[j][0], scanparset_vals[j].size());
        fRW->Reconfigure();

        // Reconfigure spline RW
        splweight->SetAllDials(&scanparset_vals[j][0],
scanparset_vals[j].size()); splweight->Reconfigure();

        splevent->fSplineRead->SetNeedsReconfigure(true);

        // Calc weight for both events
        rawweights[j] = fRW->CalcWeight(rawevent);
        splweights[j] = splweight->CalcWeight(splevent);
        difweights[j] = splweights[j] - rawweights[j];
      }


      if (i % 1000 == 0) {
        LOG(FIT) << "Processed " << i << "/" << nevents << std::endl;
      }

      // Fill Array
      weighttree->Fill();

      // Iterate to next event.
      i++;
      rawevent = input->NextNuisanceEvent();
      splevent = output->NextNuisanceEvent();
    }

    outputtestfile->cd();
    weighttree->Write();
    outputtestfile->Close();
  }
}

*/

//*************************************
void SplineRoutines::TestSplines_NDEventThrow() {
  //*************************************

  // Setup RW Engine
  if (fRW)
    delete fRW;
  SetupRWEngine();

  // Make a spline RW Engine too.
  FitWeight *splweight = new FitWeight("splinerwaweight");
  std::vector<nuiskey> parameterkeys = Config::QueryKeys("parameter");
  TH1D *parhisttemplate = new TH1D("parhist", "parhist", parameterkeys.size(),
                                   0.0, float(parameterkeys.size()));

  // Add Parameters
  for (size_t i = 0; i < parameterkeys.size(); i++) {
    nuiskey key = parameterkeys[i];

    std::string parname = key.GetS("name");
    std::string partype = key.GetS("type");
    double nom = key.GetD("nominal");

    parhisttemplate->SetBinContent(i + 1, nom);
    parhisttemplate->GetXaxis()->SetBinLabel(i + 1, parname.c_str());

    splweight->IncludeDial(key.GetS("name"), kSPLINEPARAMETER, nom);
    splweight->SetDialValue(key.GetS("name"), key.GetD("nominal"));
  }
  splweight->Reconfigure();

  // Make a high resolution spline set.
  std::vector<double> nomvals = fRW->GetDialValues();
  // int testres = FitPar::Config().GetParI("spline_test_resolution");

  std::vector<std::string> scanparset_names;
  std::vector<std::vector<double> > scanparset_vals;
  std::vector<TH1D *> scanparset_hists;

  // Loop over all params
  // Add Parameters
  int nthrows = FitPar::Config().GetParI("spline_test_throws");
  for (int i = 0; i < nthrows; i++) {

    std::vector<double> newvals = nomvals;

    for (size_t j = 0; j < parameterkeys.size(); j++) {
      nuiskey key = parameterkeys[j];

      if (!key.Has("low") or !key.Has("high") or !key.Has("step")) {
        continue;
      }

      // Push Back Scan
      double low = key.GetD("low");
      double high = key.GetD("high");
      newvals[j] = gRandom->Uniform(low, high);
    }
    // Add to vects
    scanparset_vals.push_back(newvals);

    TH1D *parhist = (TH1D *)parhisttemplate->Clone();
    for (size_t j = 0; j < newvals.size(); j++) {
      parhist->SetBinContent(j + 1, newvals[j]);
    }
    scanparset_hists.push_back(parhist);
  }

  // Print out the parameter set to test
  for (uint i = 0; i < scanparset_vals.size(); i++) {
    std::cout << "Parset " << i;
    for (uint j = 0; j < scanparset_vals[i].size(); j++) {
      std::cout << " " << scanparset_vals[i][j];
    }
    std::cout << std::endl;
  }

  // Weight holders
  double *rawweights = new double[scanparset_vals.size()];
  double *splweights = new double[scanparset_vals.size()];
  double *difweights = new double[scanparset_vals.size()];
  int nweights = scanparset_vals.size();
  // int NParSets = scanparset_vals.size();

  // Loop over all event I/O
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (size_t i = 0; i < eventkeys.size(); i++) {
    nuiskey key = eventkeys.at(i);

    // Get I/O
    std::string inputfilename = key.GetS("input");
    if (inputfilename.empty()) {
      NUIS_ABORT("No input given for set of input events!");
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()) {
      outputfilename = inputfilename + ".nuisance.root";
      NUIS_ERR(WRN, "No output give for set of output events! Saving to "
                      << outputfilename);
    }

    // Make a new input handler
    std::vector<std::string> file_descriptor =
        GeneralUtils::ParseToStr(inputfilename, ":");
    if (file_descriptor.size() != 2) {
      NUIS_ABORT("File descriptor had no filetype declaration: \""
             << inputfilename << "\". expected \"FILETYPE:file.root\"");
    }
    InputUtils::InputType inptype =
        InputUtils::ParseInputType(file_descriptor[0]);

    // Make handlers for input and output
    InputHandlerBase *input = InputUtils::CreateInputHandler(
        "rawevents", inptype, file_descriptor[1]);
    InputHandlerBase *output = InputUtils::CreateInputHandler(
        "splineevents", InputUtils::kEVSPLN_Input, outputfilename);

    // Get Base Events for each case.
    FitEvent *rawevent = input->FirstNuisanceEvent();
    FitEvent *splevent = output->FirstNuisanceEvent();

    // Setup outputfile
    std::string outputtest = outputfilename + ".splinetest.NDEventThrow.root";
    TFile *outputtestfile = new TFile(outputtest.c_str(), "RECREATE");
    outputtestfile->cd();

    // Save Parameter Sets
    for (size_t i = 0; i < scanparset_hists.size(); i++) {
      scanparset_hists[i]->Write(Form("Paramater_Set_%i", (int)i));
    }

    // Save a TTree of weights and differences.
    TTree *weighttree = new TTree("weightscan", "weightscan");

    // Make a branch for each weight set
    for (size_t i = 0; i < scanparset_hists.size(); i++) {
      weighttree->Branch(Form("RawWeights_Set_%i", (int)i), &rawweights[i],
                         Form("RawWeights_Set_%i/D", (int)i));
      weighttree->Branch(Form("SplineWeights_Set_%i", (int)i), &splweights[i],
                         Form("SplineWeights_Set_%i/D", (int)i));
      weighttree->Branch(Form("DifWeights_Set_%i", (int)i), &difweights[i],
                         Form("DifWeights_Set_%i/D", (int)i));
    }

    // Count
    // int i = 0;
    int nevents = input->GetNEvents();
    int lasttime = time(NULL);

    // Load N Chunks of the Weights into Memory
    // Split into N processing chunks
    int nchunks = FitPar::Config().GetParI("spline_chunks");
    if (nchunks <= 0)
      nchunks = 1;
    if (nchunks >= nevents / 2)
      nchunks = nevents / 2;

    std::cout << "Starting NChunks " << nchunks << std::endl;
    for (int ichunk = 0; ichunk < nchunks; ichunk++) {

      // Skip to only do one processing chunk
      //      if (procchunk != -1 and procchunk != ichunk) continue;

      NUIS_LOG(FIT, "On Processing Chunk " << ichunk);
      int neventsinchunk = nevents / nchunks;
      int loweventinchunk = neventsinchunk * ichunk;
      // int higheventinchunk = neventsinchunk * (ichunk + 1);

      double **allrawweights = new double *[neventsinchunk];
      double **allsplweights = new double *[neventsinchunk];
      double **alldifweights = new double *[neventsinchunk];
      for (int k = 0; k < neventsinchunk; k++) {
        allrawweights[k] = new double[nweights];
        allsplweights[k] = new double[nweights];
        alldifweights[k] = new double[nweights];
      }

      // Start Set Processing Here.
      for (int iset = 0; iset < nweights; iset++) {

        // Reconfigure
        fRW->SetAllDials(&scanparset_vals[iset][0],
                         scanparset_vals[iset].size());
        fRW->Reconfigure();

        // Reconfigure spline RW
        splweight->SetAllDials(&scanparset_vals[iset][0],
                               scanparset_vals[iset].size());
        splweight->Reconfigure();

        splevent->fSplineRead->SetNeedsReconfigure(true);

        // Could reorder this to save the weightconts in order instead of
        // reconfiguring per event. Loop over all events and fill the TTree
        for (int i = 0; i < neventsinchunk; i++) {

          rawevent = input->GetNuisanceEvent(i + loweventinchunk);
          splevent = output->GetNuisanceEvent(i + loweventinchunk);

          allrawweights[i][iset] = fRW->CalcWeight(rawevent);
          allsplweights[i][iset] = splweight->CalcWeight(splevent);
          alldifweights[i][iset] =
              allsplweights[i][iset] - allrawweights[i][iset];
        }

        std::ostringstream timestring;
        int timeelapsed = time(NULL) - lasttime;
        if (timeelapsed) {
          lasttime = time(NULL);

          int setsleft =
              (nweights - iset - 1) + (nweights * (nchunks - ichunk - 1));
          float proj = (float(setsleft) * timeelapsed) / 60 / 60;
          timestring << setsleft << " sets remaining. Last one took "
                     << timeelapsed << ". " << proj << " hours remaining.";
        }

        NUIS_LOG(REC, "Processed Set " << iset << "/" << nweights << " in chunk "
                                   << ichunk << "/" << nchunks << " "
                                   << timestring.str());
      }

      // Fill weights for this chunk into the TTree
      for (int k = 0; k < neventsinchunk; k++) {
        for (int l = 0; l < nweights; l++) {
          rawweights[l] = allrawweights[k][l];
          splweights[l] = allsplweights[k][l];
          difweights[l] = alldifweights[k][l];
        }
        weighttree->Fill();
      }
    }

    // Loop over nchunks

    // Loop over parameter sets
    // Set All Dials and reconfigure

    // Loop over events in chunk
    // Fill Chunkweightcontainers

    // Once all dials are done, fill the weight tree

    // Iterator to next chunk

    outputtestfile->cd();
    weighttree->Write();
    outputtestfile->Close();
  }
}

void SplineRoutines::SaveSplinePlots() {

  if (fRW)
    delete fRW;
  SetupRWEngine();

  // Setup the spline reader
  SplineWriter *splwrite = new SplineWriter(fRW);
  std::vector<nuiskey> splinekeys = Config::QueryKeys("spline");

  // Add splines to splinewriter
  for (std::vector<nuiskey>::iterator iter = splinekeys.begin();
       iter != splinekeys.end(); iter++) {
    nuiskey splkey = (*iter);

    // Add Spline Info To Reader
    splwrite->AddSpline(splkey);
  }
  splwrite->SetupSplineSet();

  // Event Loop
  // Loop over all events and calculate weights for each parameter set.

  // Generate a set of nominal events
  // Method, Loop over inputs, create input handler, then create a ttree
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (size_t i = 0; i < eventkeys.size(); i++) {
    nuiskey key = eventkeys.at(i);

    // Get I/O
    std::string inputfilename = key.GetS("input");
    if (inputfilename.empty()) {
      NUIS_ABORT("No input given for set of input events!");
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()) {
      outputfilename = inputfilename + ".nuisance.root";
      NUIS_ERR(WRN, "No output give for set of output events! Saving to "
                      << outputfilename);
    }

    // Make new outputfile
    outputfilename += ".SplinePlots.root";
    TFile *outputfile = new TFile(outputfilename.c_str(), "RECREATE");
    outputfile->cd();

    // Make a new input handler
    std::vector<std::string> file_descriptor =
        GeneralUtils::ParseToStr(inputfilename, ":");
    if (file_descriptor.size() != 2) {
      NUIS_ABORT("File descriptor had no filetype declaration: \""
             << inputfilename << "\". expected \"FILETYPE:file.root\"");
    }
    InputUtils::InputType inptype =
        InputUtils::ParseInputType(file_descriptor[0]);

    InputHandlerBase *input = InputUtils::CreateInputHandler(
        "eventsaver", inptype, file_descriptor[1]);

    // Get info from inputhandler
    int nevents = input->GetNEvents();
    int countwidth = (nevents / 1000);
    FitEvent *nuisevent = input->FirstNuisanceEvent();

    outputfile->cd();

    // int lasttime = time(NULL);
    TCanvas *fitcanvas = NULL;

    // Loop over all events and fill the TTree
    while (nuisevent) {

      // std::cout << "Fitting event " << i << std::endl;
      // Calculate the weights for each parameter set
      splwrite->GetWeightsForEvent(nuisevent);
      splwrite->FitSplinesForEvent(fitcanvas, true);

      if (fitcanvas) {
        outputfile->cd();
        fitcanvas->Write(Form("Event_SplineCanvas_%i", (int)i));
      }

      // Logging
      if (i % countwidth == 0) {
        NUIS_LOG(REC,
             "Saved " << i << "/" << nevents << " nuisance spline plots. ");
      }

      // Iterate
      i++;
      nuisevent = input->NextNuisanceEvent();
    }
    // Save flux and close file
    outputfile->cd();

    // Close Output
    outputfile->Close();

    // Delete Inputs
    delete input;
  }

  // remove Keys
  eventkeys.clear();
}

void SplineRoutines::TestSplines_NDLikelihoodThrow() {

  // Setup RW Engine
  if (fRW)
    delete fRW;
  SetupRWEngine();

  // Make a spline RW Engine too.
  FitWeight *splweight = new FitWeight("splinerwaweight");
  std::vector<nuiskey> parameterkeys = Config::QueryKeys("parameter");
  TH1D *parhisttemplate = new TH1D("parhist", "parhist", parameterkeys.size(),
                                   0.0, float(parameterkeys.size()));

  // Add Parameters
  for (size_t i = 0; i < parameterkeys.size(); i++) {
    nuiskey key = parameterkeys[i];

    std::string parname = key.GetS("name");
    std::string partype = key.GetS("type");
    double nom = key.GetD("nominal");

    parhisttemplate->SetBinContent(i + 1, nom);
    parhisttemplate->GetXaxis()->SetBinLabel(i + 1, parname.c_str());

    splweight->IncludeDial(key.GetS("name"), kSPLINEPARAMETER, nom);
    splweight->SetDialValue(key.GetS("name"), key.GetD("nominal"));
  }
  splweight->Reconfigure();

  // Make a high resolution spline set.
  std::vector<double> nomvals = fRW->GetDialValues();
  // int testres = FitPar::Config().GetParI("spline_test_resolution");

  std::vector<std::string> scanparset_names;
  std::vector<std::vector<double> > scanparset_vals;
  std::vector<TH1D *> scanparset_hists;

  // Loop over all params
  // Add Parameters
  int nthrows = FitPar::Config().GetParI("spline_test_throws");
  for (int i = 0; i < nthrows; i++) {

    std::vector<double> newvals = nomvals;

    for (size_t j = 0; j < parameterkeys.size(); j++) {
      nuiskey key = parameterkeys[j];

      if (!key.Has("low") or !key.Has("high") or !key.Has("step")) {
        continue;
      }

      // Push Back Scan
      double low = key.GetD("low");
      double high = key.GetD("high");
      newvals[j] = gRandom->Uniform(low, high);
    }
    // Add to vects
    scanparset_vals.push_back(newvals);

    TH1D *parhist = (TH1D *)parhisttemplate->Clone();
    for (size_t j = 0; j < newvals.size(); j++) {
      parhist->SetBinContent(j + 1, newvals[j]);
    }
    scanparset_hists.push_back(parhist);
  }

  // Print out the parameter set to test
  for (uint i = 0; i < scanparset_vals.size(); i++) {
    std::cout << "Parset " << i;
    for (uint j = 0; j < scanparset_vals[i].size(); j++) {
      std::cout << " " << scanparset_vals[i][j];
    }
    std::cout << std::endl;
  }

  // Make a new set of Raw/Spline Sample Keys
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  std::vector<nuiskey> testkeys = Config::QueryKeys("sampletest");

  std::vector<nuiskey> rawkeys;
  std::vector<nuiskey> splkeys;

  for (std::vector<nuiskey>::iterator iter = testkeys.begin();
       iter != testkeys.end(); iter++) {
    nuiskey key = (*iter);

    std::string samplename = key.GetS("name");
    std::string eventsid = key.GetS("inputid");
    nuiskey eventskey = Config::QueryLastKey("events", "id=" + eventsid);
    std::string rawfile = eventskey.GetS("input");
    std::string splfile = eventskey.GetS("output");

    nuiskey rawkeytemp = Config::CreateKey("sample");
    rawkeytemp.SetS("name", samplename);
    rawkeytemp.SetS("input", rawfile);

    nuiskey splkeytemp = Config::CreateKey("sample");
    splkeytemp.SetS("name", samplename);
    splkeytemp.SetS("input", "EVSPLN:" + splfile);

    rawkeys.push_back(rawkeytemp);
    splkeys.push_back(splkeytemp);
  }

  if (fOutputRootFile)
    delete fOutputRootFile;
  fOutputRootFile = new TFile(fOutputFile.c_str(), "RECREATE");

  fOutputRootFile->ls();
  // Make two new JointFCN
  JointFCN *rawfcn = new JointFCN(rawkeys, fOutputRootFile);
  JointFCN *splfcn = new JointFCN(splkeys, fOutputRootFile);

  // Create iteration tree in output file
  fOutputRootFile->cd();
  rawfcn->CreateIterationTree("raw_iterations", fRW);
  splfcn->CreateIterationTree("spl_iterations", splweight);

  // Loop over parameter sets.
  for (size_t j = 0; j < scanparset_vals.size(); j++) {

    FitBase::SetRW(fRW);
    double rawtotal = rawfcn->DoEval(&scanparset_vals[j][0]);

    FitBase::SetRW(splweight);
    double spltotal = splfcn->DoEval(&scanparset_vals[j][0]);

    NUIS_LOG(FIT, "RAW SPLINE DIF = " << rawtotal << " " << spltotal << " "
                                  << spltotal - rawtotal);
  }

  fOutputRootFile->cd();

  rawfcn->WriteIterationTree();
  splfcn->WriteIterationTree();
}

void SplineRoutines::TestSplines_1DLikelihoodScan() {

  // Setup RW Engine.
  if (fRW)
    delete fRW;
  SetupRWEngine();

  // Setup Parameter Set.
  // Make a spline RW Engine too.
  FitWeight *splweight = new FitWeight("splinerwaweight");
  // std::vector<nuiskey> splinekeys    = Config::QueryKeys("spline");
  std::vector<nuiskey> parameterkeys = Config::QueryKeys("parameter");
  TH1D *parhisttemplate = new TH1D("parhist", "parhist", parameterkeys.size(),
                                   0.0, float(parameterkeys.size()));

  // Add Parameters
  for (size_t i = 0; i < parameterkeys.size(); i++) {
    nuiskey key = parameterkeys[i];

    std::string parname = key.GetS("name");
    std::string partype = key.GetS("type");
    double nom = key.GetD("nominal");

    parhisttemplate->SetBinContent(i + 1, nom);
    parhisttemplate->GetXaxis()->SetBinLabel(i + 1, parname.c_str());

    splweight->IncludeDial(key.GetS("name"), kSPLINEPARAMETER, nom);
    splweight->SetDialValue(key.GetS("name"), key.GetD("nominal"));
  }
  splweight->Reconfigure();

  // Make a high resolution spline set.
  std::vector<double> nomvals = fRW->GetDialValues();
  // int testres = FitPar::Config().GetParI("spline_test_resolution");

  std::vector<std::vector<double> > scanparset_vals;
  std::vector<TH1D *> scanparset_hists;

  // Loop over all params
  // Add Parameters
  for (size_t i = 0; i < parameterkeys.size(); i++) {
    nuiskey key = parameterkeys[i];

    // Get Par Name
    std::string name = key.GetS("name");

    if (!key.Has("low") or !key.Has("high") or !key.Has("step")) {
      continue;
    }

    // Push Back Scan
    double low = key.GetD("low");
    double high = key.GetD("high");
    double cur = low;
    double step = key.GetD("step");

    while (cur <= high) {

      // Make new set
      std::vector<double> newvals = nomvals;
      newvals[i] = cur;

      // Add to vects
      scanparset_vals.push_back(newvals);

      TH1D *parhist = (TH1D *)parhisttemplate->Clone();
      for (size_t j = 0; j < newvals.size(); j++) {
        parhist->SetBinContent(j + 1, newvals[j]);
      }
      scanparset_hists.push_back(parhist);

      // Move to next one
      cur += step;
    }
  }

  // Print out the parameter set to test
  for (uint i = 0; i < scanparset_vals.size(); i++) {
    std::cout << "Parset " << i;
    for (uint j = 0; j < scanparset_vals[i].size(); j++) {
      std::cout << " " << scanparset_vals[i][j];
    }
    std::cout << std::endl;
  }

  // Make a new set of Raw/Spline Sample Keys
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  std::vector<nuiskey> testkeys = Config::QueryKeys("sampletest");

  std::vector<nuiskey> rawkeys;
  std::vector<nuiskey> splkeys;

  for (std::vector<nuiskey>::iterator iter = testkeys.begin();
       iter != testkeys.end(); iter++) {
    nuiskey key = (*iter);

    std::string samplename = key.GetS("name");
    std::string eventsid = key.GetS("inputid");
    nuiskey eventskey = Config::QueryLastKey("events", "id=" + eventsid);
    std::string rawfile = eventskey.GetS("input");
    std::string splfile = eventskey.GetS("output");

    nuiskey rawkeytemp = Config::CreateKey("sample");
    rawkeytemp.SetS("name", samplename);
    rawkeytemp.SetS("input", rawfile);

    nuiskey splkeytemp = Config::CreateKey("sample");
    splkeytemp.SetS("name", samplename);
    splkeytemp.SetS("input", "EVSPLN:" + splfile);

    rawkeys.push_back(rawkeytemp);
    splkeys.push_back(splkeytemp);
  }

  if (fOutputRootFile)
    delete fOutputRootFile;
  fOutputRootFile = new TFile(fOutputFile.c_str(), "RECREATE");

  fOutputRootFile->ls();
  // Make two new JointFCN
  JointFCN *rawfcn = new JointFCN(rawkeys, fOutputRootFile);
  JointFCN *splfcn = new JointFCN(splkeys, fOutputRootFile);

  // Create iteration tree in output file
  fOutputRootFile->cd();
  rawfcn->CreateIterationTree("raw_iterations", fRW);
  splfcn->CreateIterationTree("spl_iterations", splweight);

  // Loop over parameter sets.
  for (size_t j = 0; j < scanparset_vals.size(); j++) {

    FitBase::SetRW(fRW);
    double rawtotal = rawfcn->DoEval(&scanparset_vals[j][0]);

    FitBase::SetRW(splweight);
    double spltotal = splfcn->DoEval(&scanparset_vals[j][0]);

    NUIS_LOG(FIT, "RAW SPLINE DIF = " << rawtotal << " " << spltotal << " "
                                  << spltotal - rawtotal);
  }

  fOutputRootFile->cd();

  rawfcn->WriteIterationTree();
  splfcn->WriteIterationTree();
}

/*
  MISC Functions
*/
//*************************************
int SplineRoutines::GetStatus() {
  //*************************************

  return 0;
}
