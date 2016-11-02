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
void SplineRoutines::Init(){
//************************
  
  fStrategy = "SaveEvents";
  fRoutines.clear();

  fCardFile = "";
  
  fSampleFCN    = NULL;
  fRW = NULL;
  
  fAllowedRoutines = ("SaveEvents,SaveSplineEvents");

};

//*************************************
SplineRoutines::~SplineRoutines(){
//*************************************
};

/*
  Input Functions
*/
//*************************************
SplineRoutines::SplineRoutines(int argc, char* argv[]){
//*************************************

  // Set everything to defaults
  Init();
  std::vector<std::string> configs_cmd;
  std::string maxevents_flag = "";
  int verbosity_flag = 0;
  int error_flag = 0;

  // If No Arguments print commands
  for (int i = 1; i< argc; ++i){
    if (i+1 != argc){

      // Cardfile
      if (!std::strcmp(argv[i], "-c"))      { fCardFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-f")) { fStrategy=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-q")) { configs_cmd.push_back(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-n")) { maxevents_flag=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-v")) { verbosity_flag -= 1; }
      else if (!std::strcmp(argv[i], "+v")) { verbosity_flag += 1; }
      else if (!std::strcmp(argv[i], "-e")) { error_flag -= 1; }
      else if (!std::strcmp(argv[i], "+e")) { error_flag += 1; }
      else {
	ERR(FTL) << "ERROR: unknown command line option given! - '"
		  <<argv[i]<<" "<<argv[i+1]<<"'"<< std::endl;
	throw;
      }
    }
  }

  if (fCardFile.empty()){
    ERR(FTL) << "ERROR: card file not specified."   << std::endl;
    ERR(FTL) << "Run with '-h' to see options." << std::endl;
    throw;
  }
    
  // Fill fit routines and check they are good
  fRoutines = GeneralUtils::ParseToStr(fStrategy,",");
  for (UInt_t i = 0; i < fRoutines.size(); i++){
    if (fAllowedRoutines.find(fRoutines[i]) == std::string::npos){
      ERR(FTL) << "Unknown fit routine given! "
	       << "Must be provided as a comma seperated list." << std::endl;
      ERR(FTL) << "Allowed Routines: " << fAllowedRoutines << std::endl;
      throw;
    }
  }
  
  // CONFIG
  // ---------------------------
  std::string par_dir =  GeneralUtils::GetTopLevelDir()+"/parameters/";
  FitPar::Config().ReadParamFile( par_dir + "config.list.dat" );
  FitPar::Config().ReadParamFile( fCardFile );

  for (UInt_t iter = 0; iter < configs_cmd.size(); iter++){
    FitPar::Config().ForceParam(configs_cmd[iter]);
  }

  if (!maxevents_flag.empty()){
    FitPar::Config().SetParI("input.maxevents", atoi(maxevents_flag.c_str()));
  }
  
  if (verbosity_flag != 0){
    int curverb = FitPar::Config().GetParI("VERBOSITY");
    FitPar::Config().SetParI("VERBOSITY", curverb + verbosity_flag);
  }
  
  if (error_flag != 0){
    int curwarn = FitPar::Config().GetParI("ERROR");
    FitPar::Config().SetParI("ERROR", curwarn + error_flag);
  }

  LOG_VERB(FitPar::Config().GetParI("VERBOSITY"));
  ERR_VERB(FitPar::Config().GetParI("ERROR"));

  // CARD
  // --------------------------- 
  // Parse Card Options
  ReadCard(fCardFile);

  // Outputs
  // ---------------------------
  // Save Configs to output file
  //  fOutputRootFile = new TFile(fOutputFile.c_str(),"RECREATE");
  FitPar::Config().Write();

  // Starting Setup
  // --------------------------- 
  SetupRWEngine();
  SetupSamples();
  SetupGenericInputs();
  
  return;
};

//*************************************
void SplineRoutines::ReadCard(std::string cardfile){
//*************************************

  // Read cardlines into vector
  std::vector<std::string> cardlines = GeneralUtils::ParseFileToStr(cardfile,"\n");
  FitPar::Config().cardLines = cardlines;
  
  // Read Inputs
  int linecount = 0;
  for (std::vector<std::string>::iterator iter = cardlines.begin();
       iter != cardlines.end(); iter++){
    std::string line = (*iter);
    linecount++;
    
    // Skip Comments
    if (line.empty()) continue;
    if (line.c_str()[0] == '#') continue;

    // Read Valid Samples
    int samstatus = ReadSamples(line);
    int genstatus = ReadGenericInputs(line);
    int parstatus = ReadParameters(line);
    int evtstatus = ReadEventSplines(line);
    int binstatus = ReadBinSplines(line);
    
    // Show line if bad to help user
    if (samstatus == kErrorStatus ||
	genstatus == kErrorStatus ||
	parstatus == kErrorStatus ||
	evtstatus == kErrorStatus ||
	binstatus == kErrorStatus) {
      ERR(FTL) << "Bad Input in cardfile " << fCardFile
	       << " at line " << linecount << "!" << endl;
      ERR(FTL) << line << endl;
      throw;
    }
    
  }
 
  return;
};

//***************************************** 
int SplineRoutines::ReadEventSplines(std::string splstring){
//*****************************************
  std::string inputspec = "eventspline dialnames type points";

  // Check spline input
  if (splstring.find("eventspline") == std::string::npos) return kGoodStatus;

  // Parse inputs
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(splstring, " ");

  // Skip if comment or parameter somewhere later in line
  if (strvct[0].c_str()[0] == '#' ||
      strvct[0].find("eventspline") == std::string::npos){
    return kGoodStatus;
  }

  // Check length
  if (strvct.size() < 4){
    ERR(FTL) << "Input rw dials need to provide at least 4 inputs." << std::endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }

  // Read Values
  std::string parname = strvct[1];
  std::string partype = strvct[2];
  std::string parpnts = strvct[3];

  // Fill Containers
  fSplineNames.push_back(parname);
  fSplineTypes[parname]  = partype;
  fSplinePoints[parname] = parpnts;

  // Return
  LOG(FIT) << "Read Spline: " << parname << " " <<partype << " " <<parpnts << endl;
  return kGoodStatus;
}

//*****************************************
int SplineRoutines::ReadParameters(std::string parstring){
//******************************************

  std::string inputspec = "RW Dial Inputs Syntax \n" 
    "free input w/ limits: TYPE  NAME  START  MIN  MAX  STEP  [STATE] \n"
    "fix  input: TYPE  NAME  VALUE  [STATE] \n"
    "free input w/o limits: TYPE  NAME  START  FREE,[STATE] \n"
    "Allowed Types: \n" 
    "neut_parameter,niwg_parameter,t2k_parameter," 
    "nuwro_parameter,gibuu_parameter";
    
  // Check sample input
  if (parstring.find("parameter") == std::string::npos) return kGoodStatus;
  
  // Parse inputs
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(parstring, " ");

  // Skip if comment or parameter somewhere later in line
  if (strvct[0].c_str()[0] == '#' ||
      strvct[0].find("parameter") == std::string::npos){
    return kGoodStatus;
  }
  
  // Check length
  if (strvct.size() < 3){
    ERR(FTL) << "Input rw dials need to provide at least 3 inputs." << std::endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }

  // Setup default inputs
  std::string partype = strvct[0];
  std::string parname = strvct[1];
  double parval  = GeneralUtils::StrToDbl(strvct[2]);
  double minval  = parval - 1.0;
  double maxval  = parval + 1.0;
  double stepval = 1.0;
  std::string state = "FIX"; //[DEFAULT]
  
  // Check Type
  if (FitBase::ConvDialType(partype) == kUNKNOWN){
    ERR(FTL) << "Unknown parameter type! " << partype << endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }

  // Check Parameter Name
  if (FitBase::GetDialEnum(partype, parname) == -1){
    ERR(FTL) << "Bad RW parameter name! " << partype << " " << parname << endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }
  
  // Option Extra (No Limits)
  if (strvct.size() == 4){
    state = strvct[3];
  }

  // Check for weirder inputs
  if (strvct.size() > 4 && strvct.size() < 6){
    ERR(FTL) << "Provided incomplete limits for " << parname << endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }
  
  // Option Extra (With limits and steps)
  if (strvct.size() >= 6){
    minval  = GeneralUtils::StrToDbl(strvct[3]);
    maxval  = GeneralUtils::StrToDbl(strvct[4]);
    stepval = GeneralUtils::StrToDbl(strvct[5]);
  }

  // Option Extra (dial state after limits)
  if (strvct.size() == 7){
    state = strvct[6];
  }

  // Run Parameter Conversion if needed
  if (state.find("ABS") != std::string::npos){
    parval  = FitBase::RWAbsToSigma( partype, parname, parval  );
    minval  = FitBase::RWAbsToSigma( partype, parname, minval  );
    maxval  = FitBase::RWAbsToSigma( partype, parname, maxval  );
    stepval = FitBase::RWAbsToSigma( partype, parname, stepval );
  } else if (state.find("FRAC") != std::string::npos){
    parval  = FitBase::RWFracToSigma( partype, parname, parval  );
    minval  = FitBase::RWFracToSigma( partype, parname, minval  );
    maxval  = FitBase::RWFracToSigma( partype, parname, maxval  );
    stepval = FitBase::RWFracToSigma( partype, parname, stepval );
  }

  // Check no repeat params
  if (std::find(fParams.begin(), fParams.end(), parname) != fParams.end()){
    ERR(FTL) << "Duplicate parameter names given for " << parname << endl;
    throw;
  }
  
  // Setup Containers
  fParams.push_back(parname);

  fTypeVals[parname]  = FitBase::ConvDialType(partype);
  
  fStartVals[parname] = parval;
  fCurVals[parname]   = fStartVals[parname];
  
  fErrorVals[parname] = 0.0;

  fStateVals[parname] = state;
  
  bool fixstate = state.find("FIX") != std::string::npos;
  fFixVals[parname]      = fixstate;
  fStartFixVals[parname] = fFixVals[parname];

  fMinVals[parname]  = minval;
  fMaxVals[parname]  = maxval;
  fStepVals[parname] = stepval;

  // Print the parameter
  LOG(MIN) << "Read Parameter " << parname << " " << parval << " "
	   << minval << " " << maxval << " "
	   << stepval << " " << state << std::endl;

  // Tell reader its all good
  return kGoodStatus;
}

//******************************************  
int SplineRoutines::ReadGenericInputs(std::string samstring){
//******************************************  
  std::string inputspec = "genericinput  inputfile  [outputfile]  [type]";

  // Check generic input
  if (samstring.find("genericinput") == std::string::npos)
    return kGoodStatus;

  // Parse inputs
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(samstring, " ");

  // Skip if comment or input somewhere later in line
  if (strvct[0].c_str()[0] == '#' ||
      strvct[0] != "genericinput"){
    return kGoodStatus;
  }

  // Check length
  if (strvct.size() < 3){
    ERR(FTL) << "genericinput need to provide at least 2 inputs." << std::endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }

  // Setup default inputs
  std::string inname  = strvct[1];
  std::string infile  = strvct[2];
  std::string outfile = infile + ".nuisancefile.root";
  std::string type    = "FULLSPLINES";

  // Optional Specify outputname
  if (strvct.size() > 3){
    outfile = strvct[3];
  }

  // Optional specify type
  if (strvct.size() > 4){
    type = strvct[4];
  }

  // Add to maps
  fGenericInputNames.push_back(inname);

  fGenericInputFiles[inname]  = infile;
  fGenericOutputFiles[inname] = outfile;
  fGenericOutputTypes[inname] = type;

  // Print out
  LOG(FIT) << "Read Generic Input: " << inname << " " << infile
	   << " " << outfile <<  " (" << type << ")" << endl;

  return kGoodStatus;
}    

//******************************************
int SplineRoutines::ReadSamples(std::string samstring){
//******************************************
  std::string inputspec = "";
  
  // Check sample input
  if (samstring.find("sample") == std::string::npos)
    return kGoodStatus;

  // Parse inputs
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(samstring, " ");

  // Skip if comment or sample somewhere later in line
  if (strvct[0].c_str()[0] == '#' ||
      strvct[0] != "sample"){
    return kGoodStatus;
  }

  // Check length
  if (strvct.size() < 3){
    ERR(FTL) << "Sample need to provide at least 3 inputs." << std::endl;
    std::cout << inputspec << std::endl;
    return kErrorStatus;
  }

  // Setup default inputs
  std::string samname = strvct[1];
  std::string samfile = strvct[2];
  std::string samtype = "DEFAULT";
  double      samnorm = 1.0;

  // Optional Type
  if (strvct.size() > 3){
    samtype = strvct[3];
  }

  // Optional Norm
  if (strvct.size() > 4) samnorm = GeneralUtils::StrToDbl(strvct[4]);

  // Add Sample Names as Norm Dials
  std::string normname = samname + "_norm";

  // Check no repeat params
  if (std::find(fParams.begin(), fParams.end(), normname) != fParams.end()){
    ERR(FTL) << "Duplicate samples given for " << samname << endl;
    throw;
  }
  
  fParams.push_back(normname);
  
  fTypeVals[normname]  = kNORM;
  fStartVals[normname] = samnorm;
  fCurVals[normname]   = fStartVals[normname];
  fErrorVals[normname] = 0.0;
  
  fMinVals[normname]  = 0.1;
  fMaxVals[normname]  = 10.0;
  fStepVals[normname] = 0.5;

  bool state = samtype.find("FREE") == std::string::npos;
  fFixVals[normname]      = state;
  fStartFixVals[normname] = state;

  // Print read in
  LOG(MIN) << "Read sample " << samname << " "
	   << samfile << " " << samtype << " "
	   << samnorm << endl;

  // Tell reader its all good
  return kGoodStatus;
}

/*
  Setup Functions
*/
//*************************************
void SplineRoutines::SetupRWEngine(){
//*************************************

  fRW = new FitWeight("splineweight");
  
  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string name = fParams[i];
    fRW -> IncludeDial(name, fTypeVals.at(name) );
  }
  UpdateRWEngine(fStartVals);
  
  return;
}

//*************************************  
void SplineRoutines::SetupGenericInputs(){
//*************************************
  
  fGenericInputs.clear();
  for (unsigned int i = 0; i < fGenericInputNames.size(); i++){
    std::string name = fGenericInputNames[i];
    std::string file = fGenericInputFiles[name];
    fGenericInputs[name] = ( new InputHandler( name, file ) );
  }
 
}


//*************************************
void SplineRoutines::SetupSamples(){
//*************************************

  /*
  LOG(FIT)<<"Making the jointFCN"<<std::endl;
  if (fSampleFCN) delete fSampleFCN;
  fSampleFCN = new JointFCN(fCardFile, fOutputRootFile);
  fSamples = fSampleFCN->GetSampleList();
  */
  
  return;
}

/*
  Fitting Functions
*/
//*************************************
void SplineRoutines::UpdateRWEngine(std::map<std::string,double>& updateVals){
//*************************************

  for (UInt_t i = 0; i < fParams.size(); i++){
    std::string name = fParams[i];

    if (updateVals.find(name) == updateVals.end()) continue;
    fRW->SetDialValue(name,updateVals.at(name));
  }

  fRW->Reconfigure();
  return;
}

//************************************* 
void SplineRoutines::Run(){
//************************************* 
  
  for (UInt_t i = 0; i < fRoutines.size(); i++){

    std::string routine = fRoutines.at(i);
    int fitstate = kFitUnfinished;
    LOG(FIT)<<"Running Routine: "<<routine<<std::endl;

    // Try Routines
    if (!routine.compare("SaveSplineEvents")) SaveEventSplines();
    else if (!routine.compare("SaveEvents")) SaveEvents();
    
    // If ending early break here
    if (fitstate == kFitFinished || fitstate == kNoChange){
      LOG(FIT) << "Ending fit routines loop." << endl;
      break;
    }
  }

  return;
}

//*************************************
void SplineRoutines::SaveEvents(){
//*************************************

  // Make a new RWm Engine
  if (fRW) delete fRW;
  SetupRWEngine();

  // Set RW engine to central values
  UpdateRWEngine(fCurVals);

  // iterate over generic events
  std::map<std::string, InputHandler*>::const_iterator iter = fGenericInputs.begin();
  
  // Iterate over all inputs
  for( ; iter != fGenericInputs.end(); iter++){

    std::string name = (iter->first);

    LOG(FIT) << "Creating new nuisance event set in:" << endl;
    LOG(FIT) << "Name = " << name << endl;
    LOG(FIT) << "InputFile = " << fGenericInputFiles[name] << endl;
    LOG(FIT) << "OutputFile = " << fGenericOutputFiles[name] << endl;
    LOG(FIT) << "Type = " << fGenericOutputTypes[name] << endl;
    
    // Create a new TFile
    TFile* eventfile = new TFile( fGenericOutputFiles[name].c_str(), "RECREATE" );
    eventfile->cd();

    // Get Input
    InputHandler* curinput = (iter->second);
    int nevents = curinput->GetNEvents();    

    // Setup Event
    FitEvent* custevent = curinput->GetEventPointer();

    // Setup TTree
    eventfile->cd();
    TTree* evttree = new TTree("FitEvents","FitEvents");
    custevent->AddBranchesToTree(evttree);

    int countwidth = (nevents/50);
    
    // Run Loop and Fill Event Tree
    for (int i = 0; i < nevents; i++){

      // Grab new event
      curinput->ReadEvent(i);

      // Fill event info
      custevent->CalcKinematics();

      // Fill Spline/Weight Info
      fRW->CalcWeight(custevent);
     
      // Save everything
      evttree->Fill();

      if (i % countwidth == 0){
	LOG(REC) << "Filled " << i << "/" << nevents << " nuisance events." << endl;
      }

    }

    // Save TTree alongside flux info
    eventfile->cd();
    evttree->Write();
    curinput->GetFluxHistogram()->Write("FitFluxHist");
    curinput->GetEventHistogram()->Write("FitEventHist");

    // Close file
    eventfile->Close();
  }
}


//************************************* 
void SplineRoutines::TestEventSplines(){
//************************************* 

  // Make a new RW Engine
  if (fRW) delete fRW;
  SetupRWEngine();

  // Set RW engine to central values
  UpdateRWEngine(fCurVals);

  // Create list of new temp InputHandlers
  std::map<std::string, InputHandler*> evtsplinehandles;
  for (UInt_t i = 0; i < fGenericInputNames.size(); i++){
    std::string name = fGenericInputNames[i];

    evtsplinehandles[name] = (new InputHandler(name + "_splines",
					       "EVSPLN:"+fGenericOutputFiles[name]));
  }

  // Iterate over both inputs/outputs
  std::map<std::string, InputHandler*>::const_iterator initer = fGenericInputs.begin();
  std::map<std::string, InputHandler*>::const_iterator outiter = evtsplinehandles.begin();

  // Iterate over all inputs
  for( ; initer != fGenericInputs.end(); initer++,outiter++){

    std::string name = (initer->first);
    LOG(FIT) << "Testing event splines in " << name << endl;

    // Create a new TFile
    TFile* splinefile = new TFile( fGenericOutputFiles[name].c_str(), "UPDATE" );
    splinefile->cd();

    // Get Input
    InputHandler* rawinput = (initer->second);
    InputHandler* splinput = (outiter->second);
    int nevents = splinput->GetNEvents(); // Should match...
    nevents = 100;
    
    FitEvent* rawevent = rawinput->GetEventPointer();
    FitEvent* splevent = splinput->GetEventPointer();

    fRW->ReadSplineHead(splinput->GetSplineHead());
    
    // Small Plots on event-by-event
    // -------------------------------

    // Make folder for graphs
    TDirectory* graphdir = splinefile->mkdir("testgraphs");
    graphdir->cd();
    
    // Create Plot for each event
    for (int i = 0; i < nevents; i++){

      // Grab new event from each sample
      rawinput->ReadEvent(i);
      splinput->ReadEvent(i);

      // Start Loop for each parameter //TODO Make it test ND splines
      for (unsigned int j = 0; j < fParams.size(); j++){
	std::string name = fParams.at(j);
	if (fFixVals[name]) continue;
	
	double startval = fCurVals[name];

	// 1D Parameter Loop
	std::vector<double> xvals;
	std::vector<double> yvals_raw;
	std::vector<double> yvals_spl;
	
	// Start at min and loop
	fCurVals[name] = fMinVals[name];
	while (fCurVals[name] < fMaxVals[name]){

	  // Update
	  UpdateRWEngine(fCurVals);
	  fRW->ReadSplineHead(splinput->GetSplineHead());

	  // Calc Weights
	  xvals.push_back(fCurVals[name]);
	  yvals_raw.push_back( fRW->CalcWeight(rawevent) );
	  yvals_spl.push_back( fRW->CalcWeight(splevent) );
	  
	  // Step
	  fCurVals[name] = fCurVals[name] + fStepVals[name];
	  
	}

	// Reset to normal
	fCurVals[name] = startval;
	splinefile->cd();
	TGraph* graw = new TGraph(xvals.size(),&xvals[0],&yvals_raw[0]);
	graw->SetNameTitle( ("evspltest_raw_" + name).c_str(),
			  ("evspltest_raw_" + name + ";" + name + ";weight").c_str());
	graw->SetLineColor(kBlue);
	graw->SetMarkerColor(kBlue);
	graw->SetLineWidth(2);
	graw->SetMarkerStyle(0);
	
	TGraph* gspl = new TGraph(xvals.size(),&xvals[0],&yvals_spl[0]);
	gspl->SetNameTitle( ("evspltest_spl_" + name).c_str(),
			    ("evspltest_spl_" + name + ";" + name + ";weight").c_str());
	gspl->SetLineColor(kRed);
	gspl->SetMarkerColor(kRed);
	gspl->SetLineWidth(2);
	gspl->SetMarkerStyle(20);
	
	TCanvas* c1 = new TCanvas(("evspltest_" + name).c_str(),
				  ("evspltest_" + name).c_str(),
				  800,600);

	TLegend* l1 = new TLegend(0.6,0.6,0.9,0.9);
	l1->AddEntry(graw,"RAW","l");
	l1->AddEntry(gspl,"SPL","l");
       	
	c1->cd();
	graw->Draw("APL");
	gspl->Draw("PL SAME");
	l1->Draw("SAME");

	graphdir->cd();
	c1->Write();

	delete graw;
	delete gspl;
	delete l1;
	
      }
    }

    // Clean up
    splinefile->Close();
  }

  // Finish Up
  evtsplinehandles.clear();

}

//*************************************
void SplineRoutines::SaveEventSplines(){
//*************************************

  // Make a new RWm Engine
  if (fRW) delete fRW;
  SetupRWEngine();

  // Set RW engine to central values
  UpdateRWEngine(fCurVals);

  // Add Splines to RW Engine
  for (UInt_t i = 0; i < fSplineNames.size(); i++){
    std::string name = fSplineNames.at(i);
    fRW->SetupSpline( name, fSplineTypes[name], fSplinePoints[name] );
  }

  LOG(FIT) << "Starting spline inputs" <<  endl;
  // iterate over generic events
  std::map<std::string, InputHandler*>::const_iterator iter = fGenericInputs.begin();

  
  
  // Iterate over all inputs
  for( ; iter != fGenericInputs.end(); iter++){

    std::string name = (iter->first);

    LOG(FIT) << "Creating new spline set in:" << endl;
    LOG(FIT) << "Name = " << name << endl;
    LOG(FIT) << "InputFile = " << fGenericInputFiles[name] << endl;
    LOG(FIT) << "OutputFile = " << fGenericOutputFiles[name] << endl;
    LOG(FIT) << "Type = " << fGenericOutputTypes[name] << endl;
    
    // Create a new TFile
    TFile* splinefile = new TFile( fGenericOutputFiles[name].c_str(), "RECREATE" );
    splinefile->cd();

    // Get Input
    InputHandler* curinput = (iter->second);
    int nevents = curinput->GetNEvents();    

    // Setup Event
    FitEvent* custevent = curinput->GetEventPointer();

    // Setup TTree
    splinefile->cd();
    TTree* evttree = new TTree("FitEvents","FitEvents");
    fRW->SetupEventCoeff(static_cast<BaseFitEvt*>(custevent));
    custevent->AddBranchesToTree(evttree);
    custevent->AddSplineCoeffToTree(evttree);  
    LOG(REC) << "Added events+splines to TTree" << endl;
    int countwidth = (nevents/400);

    // Make folder for graphs
    TDirectory* graphdir = splinefile->mkdir("gengraphs");
    graphdir->cd();
    
    // Run Loop and Fill Event Tree
    for (int i = 0; i < nevents; i++){

      // Grab new event
      curinput->ReadEvent(i);

      // Fill event info
      custevent->CalcKinematics();

      bool save = false;
      if (i % nevents/20 == 0) save = true;
      // Fill Spline/Weight Info
      fRW->GenSplines(custevent,save);
      fRW->CalcWeight(custevent);
     
      // Save everything
      evttree->Fill();

      if (i % countwidth == 0){
	LOG(REC) << "Filled " << i << "/" << nevents << " spline events." << endl;
      }

    }

    // Save TTree alongside flux info
    splinefile->cd();
    fRW->GetSplineHeader()->Write("FitSplineHead");
    evttree->Write();
    curinput->GetFluxHistogram()->Write("FitFluxHist");
    curinput->GetEventHistogram()->Write("FitEventHist");

    // Close file
    splinefile->Close();
  }
  TestEventSplines();
}

/*
  MISC Functions
*/
//*************************************
int SplineRoutines::GetStatus(){
//*************************************

  return 0;
}

