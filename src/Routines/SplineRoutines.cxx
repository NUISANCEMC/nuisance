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
  std::string cardfile = "";
  int maxevents = -1;
  int errorcount = Config::Get().GetParI("ERROR");
  int verbocount = Config::Get().GetParI("VERBOSITY");
  std::vector<std::string> xmlcmds;
  std::vector<std::string> configargs;

  // Make easier to handle arguments.
  std::vector<std::string> args = GeneralUtils::LoadCharToVectStr(argc, argv);
  ParserUtils::ParseArgument(args, "-c", fCardFile, true);
  ParserUtils::ParseArgument(args, "-o", fOutputFile, false, false);
  ParserUtils::ParseArgument(args, "-n", maxevents, false, false);
  ParserUtils::ParseArgument(args, "-f", fStrategy, false, false);
  ParserUtils::ParseSplitArgument(args, "-i", xmlcmds);
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
    ERR(FTL) << "No output file supplied!" << std::endl;
    throw;
  }

  // Setup this configuration
  fCompKey = Config::Get().CreateNode("nuiscomp");
  fCompKey.AddS("cardfile", fCardFile);
  fCompKey.AddS("outputfile", fOutputFile);
  fCompKey.AddS("strategy", fStrategy);

  // Load XML Cardfile
  configuration.LoadConfig( fCompKey.GetS("cardfile"), "");

  // Add CMD XML Structs
  for (size_t i = 0; i < xmlcmds.size(); i++) {
    // std::cout << "Adding XML Line " << xmlcmds[i] << std::endl;
    configuration.AddXMLLine(xmlcmds[i]);
  }

  // Add Config Args
  for (size_t i = 0; i < configargs.size(); i++) {
    configuration.OverrideConfig(configargs[i]);
  }

  // Add Error Verbo Lines
  FitPar::log_verb = verbocount;
  LOG_VERB(verbocount);
  ERR_VERB(errorcount);

  // Finish configuration XML
  configuration.FinaliseConfig(fCompKey.GetS("outputfile") + ".xml");

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
  for (int i = 0; i < parameterkeys.size(); i++) {
    nuiskey key = parameterkeys[i];

    std::string parname = key.GetS("name");
    std::string partype = key.GetS("type");
    double nom = key.GetD("nominal");

    fRW->IncludeDial( key.GetS("name"),
                      FitBase::ConvDialType(key.GetS("type")), nom);
    fRW->SetDialValue( key.GetS("name"), key.GetD("nominal") );

  }
  fRW->Reconfigure();

  // for (int i = 0; i < parameterkeys.size(); i++){
  //   nuiskey key = parameterkeys[i];
  //   fRW->IncludeDial( key.GetS("name"),
  //       FitBase::ConvDialType(key.GetS("type")));
  // }

  //   LOG(FIT) << "Setting up FitWeight Engine" << std::endl;
  // for (UInt_t i = 0; i < fParams.size(); i++) {
  //   std::string name = fParams[i];
  //   FitBase::GetRW()->IncludeDial(name, fTypeVals.at(name));
  // }


  // UpdateRWEngine(fStartVals);

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

  for (int i = 0; i < fRoutines.size(); i++) {

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
  for (int i = 0; i < eventkeys.size(); i++) {
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
  for (int i = 0; i < eventkeys.size(); i++) {
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
	if (i != 0 and timeelapsed){
	lasttime = time(NULL);

	int eventsleft = nevents - i;
	float speed = float(countwidth) / float(timeelapsed);
	float proj = (float(eventsleft)/float(speed))/60/60; 
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
// void SplineRoutines::TestEventSplines() {
//*************************************

// Make a spline fit weight and a normal fit weight.

// Loop over test samples.

// Loop over splines

// Set spline parameters and rw parameters to same value

// Compare weight response by binning against sample set.




// }

/*
  MISC Functions
*/
//*************************************
int SplineRoutines::GetStatus() {
  //*************************************

  return 0;
}
