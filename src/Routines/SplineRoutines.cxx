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
SplineRoutines::~SplineRoutines(){
    //*************************************
};

/*
  Input Functions
*/
//*************************************
SplineRoutines::SplineRoutines(int argc, char* argv[]) {
  //*************************************

  // Set everything to defaults
  Init();
  std::vector<std::string> configs_cmd;
  std::string maxevents_flag = "";
  int verbosity_flag = 0;
  int error_flag = 0;

  // If No Arguments print commands
  for (int i = 1; i < argc; ++i) {
    if (i + 1 != argc) {
      // Cardfile
      if (!std::strcmp(argv[i], "-c")) {
        fCardFile = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-f")) {
        fStrategy = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-q")) {
        configs_cmd.push_back(argv[i + 1]);
        ++i;
      } else if (!std::strcmp(argv[i], "-n")) {
        maxevents_flag = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-v")) {
        verbosity_flag -= 1;
      } else if (!std::strcmp(argv[i], "+v")) {
        verbosity_flag += 1;
      } else if (!std::strcmp(argv[i], "-e")) {
        error_flag -= 1;
      } else if (!std::strcmp(argv[i], "+e")) {
        error_flag += 1;
      } else {
        ERR(FTL) << "ERROR: unknown command line option given! - '" << argv[i]
                 << " " << argv[i + 1] << "'" << std::endl;
        throw;
      }
    }
  }

  if (fCardFile.empty()) {
    ERR(FTL) << "ERROR: card file not specified." << std::endl;
    ERR(FTL) << "Run with '-h' to see options." << std::endl;
    throw;
  }

  // Fill fit routines and check they are good
  fRoutines = GeneralUtils::ParseToStr(fStrategy, ",");
  for (UInt_t i = 0; i < fRoutines.size(); i++) {
    if (fAllowedRoutines.find(fRoutines[i]) == std::string::npos) {
      ERR(FTL) << "Unknown fit routine given! "
               << "Must be provided as a comma seperated list." << std::endl;
      ERR(FTL) << "Allowed Routines: " << fAllowedRoutines << std::endl;
      throw;
    }
  }

  // CONFIG
  // ---------------------------
  nuisconfig conf = Config::Get();

  // Read Card Inputs
  conf.LoadConfig( fCardFile, "xmlinput" );
  
  // Add the CMD Overrides
  for (UInt_t iter = 0; iter < configs_cmd.size(); iter++) {
    //  conf.AddConfig(configs_cmd[iter])
  }
  // Call reconfigure
  conf.Reconfigure();

  // Save Config
  conf.WriteConfig( fOutputFile + ".xml" );

  // ---------------------------

  // Start Setting up other stuff
  std::string par_dir = GeneralUtils::GetTopLevelDir() + "/parameters/";
  FitPar::Config().ReadParamFile(par_dir + "config.list.dat");
  FitPar::Config().ReadParamFile(fCardFile);

  for (UInt_t iter = 0; iter < configs_cmd.size(); iter++) {
    FitPar::Config().ForceParam(configs_cmd[iter]);
  }

  if (!maxevents_flag.empty()) {
    FitPar::Config().SetParI("input.maxevents", atoi(maxevents_flag.c_str()));
  }

  if (verbosity_flag != 0) {
    int curverb = FitPar::Config().GetParI("VERBOSITY");
    FitPar::Config().SetParI("VERBOSITY", curverb + verbosity_flag);
  }

  if (error_flag != 0) {
    int curwarn = FitPar::Config().GetParI("ERROR");
    FitPar::Config().SetParI("ERROR", curwarn + error_flag);
  }

  LOG_VERB(FitPar::Config().GetParI("VERBOSITY"));
  ERR_VERB(FitPar::Config().GetParI("ERROR"));

  // Outputs
  // ---------------------------
  // Save Configs to output file
  //  fOutputRootFile = new TFile(fOutputFile.c_str(),"RECREATE");
  FitPar::Config().Write();

  // Starting Setup
  // ---------------------------
  SetupRWEngine();

    //SaveEvents();
    //TestEvents();
  //GenerateEventSplines();
  return;
};


/*
  Setup Functions
*/
//*************************************
void SplineRoutines::SetupRWEngine() {
  //*************************************

  fRW = new FitWeight("splineweight");
  std::vector<nuiskey> splinekeys    = Config::QueryKeys("spline");
  std::vector<nuiskey> parameterkeys = Config::QueryKeys("parameter");

  // Add Parameters
  for (int i = 0; i < splinekeys.size(); i++){
    nuiskey key = splinekeys[i];
    std::cout <<  key.GetS("name") << " " << key.GetS("type") << " " << FitBase::ConvDialType(key.GetS("type")) << std::endl;
    fRW->IncludeDial( key.GetS("name"), 
		      FitBase::ConvDialType(key.GetS("type")));
		      
  }

  for (int i = 0; i < parameterkeys.size(); i++){
    nuiskey key = parameterkeys[i];
    fRW->IncludeDial( key.GetS("name"), 
		      FitBase::ConvDialType(key.GetS("type")));
  }

  UpdateRWEngine(fStartVals);

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
void SplineRoutines::Run(){
//*************************************

  for (int i = 0; i < fRoutines.size(); i++){

    LOG(FIT) << "Running Routine: " << fRoutines[i] << std::endl;
    std::string rout = fRoutines[i];
    if       (!rout.compare("SaveEvents")) SaveEvents();
    else if  (!rout.compare("TestEvents")) TestEvents();

  }


}

//*************************************
void SplineRoutines::SaveEvents() {
//*************************************

  // Generate a set of nominal events
  // Method, Loop over inputs, create input handler, then create a ttree
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (int i = 0; i < eventkeys.size(); i++){
    nuiskey key = eventkeys.at(i);

    // Get I/O
    std::string inputfilename  = key.GetS("input");
    if (inputfilename.empty()){
      ERR(FTL) << "No input given for set of input events!" << std::endl;
      throw;
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()){
      outputfilename = inputfilename + ".nuisance.root";
      ERR(FTL) << "No output give for set of output events! Saving to " 
	       << outputfilename << std::endl;
    }

    // Make new outputfile
    TFile* outputfile = new TFile(outputfilename.c_str(),"RECREATE");
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
    
    InputHandler* input = new InputHandler("eventsaver", inptype, file_descriptor[1]);
    
    // Get info from inputhandler
    int nevents = input->GetNEvents();
    int countwidth = (nevents / 50);
    FitEvent* nuisevent = input->GetEventPointer();

    // Setup a TTree to save the event
    outputfile->cd();
    TTree* eventtree = new TTree("nuisance_events","nuisance_events");
    nuisevent->AddBranchesToTree(eventtree);

    // Loop over all events and fill the TTree
    for (int i = 0; i < nevents; i++) {
      // Grab new event
      input->ReadEvent(i);

      // Fill event info          
      nuisevent->CalcKinematics();

      // Save everything          
      eventtree->Fill();

      // Logging
      if (i % countwidth == 0) {
        LOG(REC) << "Saved " << i << "/" << nevents << " nuisance events." << std::endl;
      }
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
void SplineRoutines::TestEvents(){
//*************************************

  // Create a new file for the test samples
  TFile* testfile = new TFile("testfile.root","RECREATE");

  // Loop over all tests
  int count = 0;
  std::vector<nuiskey> testkeys = Config::QueryKeys("sampletest");
  for (std::vector<nuiskey>::iterator iter = testkeys.begin();
       iter != testkeys.end(); iter++){
    nuiskey key = (*iter);
    
    // 0. Create new measurement list
    std::list<MeasurementBase*> samplelist;
    
    // 1. Build Sample From Events
    std::string samplename = key.GetS("name");
    std::string eventsid = key.GetS("inputid");
    nuiskey eventskey = Config::QueryLastKey("events","id=" + eventsid);
    std::string rawfile = eventskey.GetS("input");
    MeasurementBase* rawsample = SampleUtils::CreateSample(samplename, rawfile, "", "", FitBase::GetRW());

    // 2. Build Sample From Nuisance Events
    std::string eventsfile = eventskey.GetS("output");
    MeasurementBase* nuissample = SampleUtils::CreateSample(samplename, "FEVENT:" + eventsfile, "", "", FitBase::GetRW());

    // 3. Make some folders to save stuff
    TDirectory* sampledir   = (TDirectory*) testfile->mkdir(Form((samplename+"_test_%d").c_str(),count));
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
      nuisanceplot->Add(rawplot,-1.0);

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
void SplineRoutines::GenerateEventSplines(){
//************************************* 
  if (fRW) delete fRW;
  SetupRWEngine();

  // Setup the spline reader
  SplineReader* splinereader = new SplineReader(fRW);
  std::vector<nuiskey> splinekeys = Config::QueryKeys("spline");
  // Add splines to splinereader
  for (std::vector<nuiskey>::iterator iter = splinekeys.begin();
       iter != splinekeys.end(); iter++){
    nuiskey splkey = (*iter);

    // Spline Info
    std::string splname = splkey.GetS("name");
    std::string type    = splkey.GetS("type");
    std::string form    = splkey.GetS("form");
    std::string points  = splkey.GetS("points");

    // Add Spline Info To Reader
    splinereader->AddSpline(splname, type, form, points);
  }
  splinereader->SetupSplineSet();

  // Event Loop
  // Loop over all events and calculate weights for each parameter set.
  
  // Generate a set of nominal events        
  // Method, Loop over inputs, create input handler, then create a ttree   
  std::vector<nuiskey> eventkeys = Config::QueryKeys("events");
  for (int i = 0; i < eventkeys.size(); i++){
    nuiskey key = eventkeys.at(i);

    // Get I/O 
    std::string inputfilename  = key.GetS("input");
    if (inputfilename.empty()){
      ERR(FTL) << "No input given for set of input events!" << std::endl;
      throw;
    }

    std::string outputfilename = key.GetS("output");
    if (outputfilename.empty()){
      outputfilename = inputfilename + ".nuisance.root";
      ERR(FTL) << "No output give for set of output events! Saving to "
	       << outputfilename << std::endl;
    }

    // Make new outputfile    
    TFile* outputfile = new TFile(outputfilename.c_str(),"RECREATE");
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

    InputHandler* input = new InputHandler("eventsaver", inptype, file_descriptor[1]);

    // Get info from inputhandler            
    int nevents = input->GetNEvents();
    int countwidth = (nevents / 50);
    FitEvent* nuisevent = input->GetEventPointer();

    // Setup a TTree to save the event      
    outputfile->cd();
    TTree* eventtree = new TTree("nuisance_events","nuisance_events");
    nuisevent->AddBranchesToTree(eventtree);

    // Save the spline reader
    splinereader->Write("spline_reader");

    // Setup the spline TTree
    TTree* splinetree = new TTree("spline_tree","spline_tree");
    splinereader->AddCoefficientsToTree(splinetree);

    // Loop over all events and fill the TTree                 
    for (int i = 0; i < nevents; i++) {
      // Grab new event  
      input->ReadEvent(i);

      // Fill event info 
      nuisevent->CalcKinematics();

      // Calculate the weights for each parameter set
      splinereader->FitSplinesForEvent(nuisevent);

      // Save everything 
      eventtree->Fill();
      splinetree->Fill();

      // Logging         
      if (i % countwidth == 0) {
        LOG(REC) << "Saved " << i << "/" << nevents << " nuisance events." << std::endl;
      }
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

/*
  MISC Functions
*/
//*************************************
int SplineRoutines::GetStatus() {
  //*************************************

  return 0;
}
