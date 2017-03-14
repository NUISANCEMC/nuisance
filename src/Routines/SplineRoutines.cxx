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

#include "StatusMessage.h"

#include "SplineRoutines.h"

/*
  Constructor/Destructor
*/
//************************
void SplineRoutines::Init() {
  //************************

  fStrategy = "SaveEvents";
  fRoutines.clear();

  fCardFile = "";

  fSampleFCN = NULL;
  fRW = NULL;

  fAllowedRoutines = ("SaveEvents,TestEvents,SaveSplineEvents");
};

//*************************************
SplineRoutines::~SplineRoutines() {
  //*************************************
};

/*
  Input Functions
*/
//*************************************
SplineRoutines::SplineRoutines(int argc, char* argv[]) {
//*************************************

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
    ERR(FTL) << "No input supplied!" << std::endl;
    throw;
  }

  if (fOutputFile.empty() and !fCardFile.empty()) {
    fOutputFile = fCardFile + ".root";
    ERR(WRN) << "No output supplied so saving it to: " << fOutputFile << std::endl;

  } else if (fOutputFile.empty()) {
    ERR(FTL) << "No output file or cardfile supplied!" << std::endl;
    throw;
  }

  // Configuration Setup =============================

  // Check no comp key is available
  nuiskey fCompKey;
  if (Config::Get().GetNodes("nuiscomp").empty()) {
    fCompKey = Config::Get().CreateNode("nuiscomp");
  } else {
    fCompKey = Config::Get().GetNodes("nuiscomp")[0];
  }

  if (!fCardFile.empty())   fCompKey.AddS("cardfile", fCardFile);
  if (!fOutputFile.empty()) fCompKey.AddS("outputfile", fOutputFile);
  if (!fStrategy.empty())   fCompKey.AddS("strategy", fStrategy);

  // Load XML Cardfile
  configuration.LoadConfig( fCompKey.GetS("cardfile"), "");

  // Add CMD XML Structs
  for (size_t i = 0; i < xmlcmds.size(); i++) {
    configuration.AddXMLLine(xmlcmds[i]);
  }

  // Add Config Args
  for (size_t i = 0; i < configargs.size(); i++) {
    configuration.OverrideConfig(configargs[i]);
  }
  if (!maxevents.compare("-1")) {
    configuration.OverrideConfig("MAXEVENTS=" + maxevents);
  }

  // Finish configuration XML
  configuration.FinaliseConfig(fCompKey.GetS("outputfile") + ".xml");

  // Add Error Verbo Lines
  verbocount += Config::Get().GetParI("VERBOSITY");
  errorcount += Config::Get().GetParI("ERROR");
  std::cout << "[ NUISANCE ]: Setting VERBOSITY=" << verbocount << std::endl;
  std::cout << "[ NUISANCE ]: Setting ERROR=" << errorcount << std::endl;
  FitPar::log_verb = verbocount;
  LOG_VERB(verbocount);
  ERR_VERB(errorcount);

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

    fRW->IncludeDial( key.GetS("name"),
                      FitBase::ConvDialType(key.GetS("type")), nom);
    fRW->SetDialValue( key.GetS("name"), key.GetD("nominal") );

  }
  fRW->Reconfigure();

  return;
}


/*
  Fitting Functions
*/
//*************************************
void SplineRoutines::UpdateRWEngine(std::map<std::string, double>& updateVals) {
//*************************************

  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];

    if (updateVals.find(name) == updateVals.end()) continue;
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
    ERR(FTL) << "Trying to run ComparisonRoutines with no routines given!" << std::endl;
    throw;
  }

  for (size_t i = 0; i < fRoutines.size(); i++) {

    LOG(FIT) << "Running Routine: " << fRoutines[i] << std::endl;
    std::string rout = fRoutines[i];
    if       (!rout.compare("SaveEvents")) SaveEvents();
    else if  (!rout.compare("TestEvents")) TestEvents();
    else if  (!rout.compare("GenerateEventSplines")) GenerateEventSplines();

  }


}

//*************************************
void SplineRoutines::SaveEvents() {
//*************************************

  if (fRW) delete fRW;
  SetupRWEngine();
  fRW->Reconfigure();
  fRW->Print();

  // Generate a set of nominal events
  // Method, Loop over inputs, create input handler, then create a ttree
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

    // Make new outputfile
    TFile* outputfile = new TFile(outputfilename.c_str(), "RECREATE");
    outputfile->cd();

    // Make a new input handler
    std::vector<std::string> file_descriptor =
      GeneralUtils::ParseToStr(inputfilename, ":");
    if (file_descriptor.size() != 2) {
      ERR(FTL) << "File descriptor had no filetype declaration: \"" << inputfilename
               << "\". expected \"FILETYPE:file.root\"" << std::endl;
      throw;
    }
    InputUtils::InputType inptype =
      InputUtils::ParseInputType(file_descriptor[0]);

    InputHandlerBase* input = InputUtils::CreateInputHandler("eventsaver", inptype, file_descriptor[1]);

    // Get info from inputhandler
    int nevents = input->GetNEvents();
    int countwidth = (nevents / 10);
    FitEvent* nuisevent = input->FirstNuisanceEvent();

    // Setup a TTree to save the event
    outputfile->cd();
    TTree* eventtree = new TTree("nuisance_events", "nuisance_events");
    nuisevent->AddBranchesToTree(eventtree);

    // Loop over all events and fill the TTree
    int i = 0;
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
      if (i % countwidth == 0) {
        LOG(REC) << "Saved " << i << "/" << nevents
                 << " nuisance events. [M, W] = ["
                 << nuisevent->Mode << ", " << nuisevent->RWWeight << "]" << std::endl;
      }

      // iterate
      nuisevent = input->NextNuisanceEvent();
      i++;
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
  LOG(FIT) << "Finished processing all nuisance events." << std::endl;
}

//*************************************
void SplineRoutines::TestEvents() {
//*************************************

  LOG(FIT) << "Testing events." << std::endl;

  // Create a new file for the test samples
  if (!fOutputRootFile) {
    fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");
  }

  // Loop over all tests
  int count = 0;
  std::vector<nuiskey> testkeys = Config::QueryKeys("sampletest");
  for (std::vector<nuiskey>::iterator iter = testkeys.begin();
       iter != testkeys.end(); iter++) {
    nuiskey key = (*iter);

    // 0. Create new measurement list
    std::list<MeasurementBase*> samplelist;

    // 1. Build Sample From Events
    std::string samplename = key.GetS("name");
    std::string eventsid = key.GetS("inputid");
    nuiskey eventskey = Config::QueryLastKey("events", "id=" + eventsid);
    std::string rawfile = eventskey.GetS("input");
    LOG(FIT) << "Creating sample " << samplename << std::endl;
    MeasurementBase* rawsample = SampleUtils::CreateSample(samplename, rawfile, "", "", FitBase::GetRW());

    // 2. Build Sample From Nuisance Events
    std::string eventsfile = eventskey.GetS("output");
    LOG(FIT) << "Creating Fit Eevnt Sample " << samplename << " " << eventsfile << std::endl;
    MeasurementBase* nuissample = SampleUtils::CreateSample(samplename, "FEVENT:" + eventsfile, "", "", FitBase::GetRW());

    // 3. Make some folders to save stuff
    TDirectory* sampledir   = (TDirectory*) fOutputRootFile->mkdir(Form((samplename + "_test_%d").c_str(), count));
    TDirectory* rawdir      = (TDirectory*) sampledir->mkdir("raw");
    TDirectory* nuisancedir = (TDirectory*) sampledir->mkdir("nuisance");
    TDirectory* difdir      = (TDirectory*) sampledir->mkdir("difference");

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
    while ((dirkey = (TKey*)next())) {

      // If not a 1D/2D histogram skip
      TClass *cl = gROOT->GetClass(dirkey->GetClassName());
      if (!cl->InheritsFrom("TH1D") and !cl->InheritsFrom("TH2D")) continue;

      // Get TH1* from both dir
      TH1 *rawplot      = (TH1*)rawdir->Get(dirkey->GetName());
      TH1 *nuisanceplot = (TH1*)nuisancedir->Get(dirkey->GetName());

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



//*************************************
void SplineRoutines::GenerateEventSplines() {
//*************************************
  if (fRW) delete fRW;
  SetupRWEngine();

  // Setup the spline reader
  SplineWriter* splwrite = new SplineWriter(fRW);
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

    // Make new outputfile
    TFile* outputfile = new TFile(outputfilename.c_str(), "RECREATE");
    outputfile->cd();

    // Make a new input handler
    std::vector<std::string> file_descriptor =
      GeneralUtils::ParseToStr(inputfilename, ":");
    if (file_descriptor.size() != 2) {
      ERR(FTL) << "File descriptor had no filetype declaration: \"" << inputfilename
               << "\". expected \"FILETYPE:file.root\"" << std::endl;
      throw;
    }
    InputUtils::InputType inptype =
      InputUtils::ParseInputType(file_descriptor[0]);

    InputHandlerBase* input = InputUtils::CreateInputHandler("eventsaver", inptype, file_descriptor[1]);

    // Get info from inputhandler
    int nevents = input->GetNEvents();
    int countwidth = (nevents / 1000);
    FitEvent* nuisevent = input->FirstNuisanceEvent();

    // Setup a TTree to save the event
    outputfile->cd();
    TTree* eventtree = new TTree("nuisance_events", "nuisance_events");

    // Add a flag that allows just splines to be saved.
    nuisevent->AddBranchesToTree(eventtree);

    // Save the spline reader
    splwrite->Write("spline_reader");

    // Setup the spline TTree
    TTree* splinetree = new TTree("spline_tree", "spline_tree");
    splwrite->AddCoefficientsToTree(splinetree);
    int lasttime = time(NULL);
    // Loop over all events and fill the TTree
    while (nuisevent) {

      // std::cout << "Fitting event " << i << std::endl;
      // Calculate the weights for each parameter set
      splwrite->FitSplinesForEvent(nuisevent);

      // Save everything
      eventtree->Fill();
      splinetree->Fill();

      // nuisevent->Print();
      // std::cout << "Done with event " << i << std::endl;

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
        LOG(REC) << "Saved " << i << "/" << nevents << " nuisance spline events. " << timestring.str() << std::endl;
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

  // remove Keys
  eventkeys.clear();

}


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

  SplineMerger* splmerge = new SplineMerger();
  std::vector<nuiskey> splinekeys = Config::QueryKeys("splinemerge");
  for (std::vector<nuiskey>::iterator iter = splinekeys.begin();
       iter != splinekeys.end(); iter++) {
    nuiskey splkey = (*iter);

    TFile* infile = new TFile(splkey.GetS("input").c_str(), "READ");
    splmerge->AddSplineSetFromFile(infile);

  }
  splmerge->SetupSplineSet();

  // Now get Event File
  std::vector<nuiskey> eventkeys = Config::QueryKeys("eventmerge");
  nuiskey key = eventkeys[0];

  std::string inputfilename  = key.GetS("input");

  // Make a new input handler
  std::vector<std::string> file_descriptor =
    GeneralUtils::ParseToStr(inputfilename, ":");
  if (file_descriptor.size() != 2) {
    ERR(FTL) << "File descriptor had no filetype declaration: \"" << inputfilename
             << "\". expected \"FILETYPE:file.root\"" << std::endl;
    throw;
  }
  InputUtils::InputType inptype =
    InputUtils::ParseInputType(file_descriptor[0]);

  InputHandlerBase* input = InputUtils::CreateInputHandler("eventsaver", inptype, file_descriptor[1]);

  std::string outputfilename = key.GetS("output");
  if (outputfilename.empty()) {
    outputfilename = inputfilename + ".nuisance.root";
    ERR(FTL) << "No output give for set of output events! Saving to "
             << outputfilename << std::endl;
  }

  // Make new outputfile
  TFile* outputfile = new TFile(outputfilename.c_str(), "RECREATE");
  outputfile->cd();


  // Get info from inputhandler
  int nevents = input->GetNEvents();
  int countwidth = (nevents / 1000);
  FitEvent* nuisevent = input->FirstNuisanceEvent();

  // Setup a TTree to save the event
  outputfile->cd();
  TTree* eventtree = new TTree("nuisance_events", "nuisance_events");

  // Add a flag that allows just splines to be saved.
  nuisevent->AddBranchesToTree(eventtree);

  // Save the spline reader
  splmerge->Write("spline_reader");

  // Setup the spline TTree
  TTree* splinetree = new TTree("spline_tree", "spline_tree");
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
      LOG(REC) << "Saved " << i << "/" << nevents << " nuisance spline events. " << timestring.str() << std::endl;
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
  if (fRW) delete fRW;
  SetupRWEngine();

  // Make a high resolution spline set.
  std::vector<double> nomvals = fRW->GetDialValues();
  int testres = FitPar::Config().GetParI("spline_test_resolution");

  std::vector< std::string > scanparset_names;
  std::vector< std::vector<double> > scanparset_vals;


  // Loop over all params
  std::vector<nuiskey> parameterkeys = Config::QueryKeys("parameter");
  // Add Parameters
  for (size_t i = 0; i < parameterkeys.size(); i++) {
    nuiskey key = parameterkeys[i];

    // Get Par Name
    std::string name = key.GetS("name");

    // Push Back Scan
    double low  = key.GetD("low");
    double high = key.GetD("high");
    double cur = low;
    while (cur < high){

      // Make new set
      std::vector<double> newvals = nomvals;
      newvals[i] = cur;

      // Add to vects
      scanparset_names.push_back(name);
      scanparset_vals.push_back(newvals);

      // Move to next one
      cur += (high-low)/double(testres);
    }
  }

 
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

    // Make new outputfile
    TFile* outputfile = new TFile(outputfilename.c_str(), "RECREATE");
    outputfile->cd();

    // Make a new input handler
    std::vector<std::string> file_descriptor =
      GeneralUtils::ParseToStr(inputfilename, ":");
    if (file_descriptor.size() != 2) {
      ERR(FTL) << "File descriptor had no filetype declaration: \"" << inputfilename
               << "\". expected \"FILETYPE:file.root\"" << std::endl;
      throw;
    }
    InputUtils::InputType inptype =
      InputUtils::ParseInputType(file_descriptor[0]);


    // Make handlers for input and output
    InputHandlerBase* input  = InputUtils::CreateInputHandler("rawevents", inptype, file_descriptor[1]);
    InputHandlerBase* output =  InputUtils::CreateInputHandler("splineevents", InputUtils::kEVSPLN_Input, outputfilename);

    // Get Base Events for each case.
    BaseFitEvt* rawevent = input->FirstBaseEvent();
    BaseFitEvt* splevent = output->FirstBaseEvent();


    // Setup outputfile
    std::string outputtest = outputfilename + ".splinetest.1DEventScan.root";
    TFile* outputtestfile = new TFile(outputtest.c_str(), "READ");
    outputtestfile->cd();

    // Save a TTree of weights and differences.


    // Count
    int i = 0;
    while (rawevent and splevent) {

      // Loop over 1D parameter sets.
      for (size_t j = 0; j < scanparset_vals.size(); j++){

        // Reconfigure

        // Calc weight for both events

        // Fill Array

      }



      // Iterate to next event.
      i++;
      rawevent = input->NextBaseEvent();
      splevent = output->NextBaseEvent();
    }
  }





}


/*
  MISC Functions
*/
//*************************************
int SplineRoutines::GetStatus() {
  //*************************************

  return 0;
}
