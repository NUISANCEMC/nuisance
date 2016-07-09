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

#include "splineRoutines.h"

//************************************* 
splineRoutines::splineRoutines(int argc, char* argv[]){
//*************************************

  rw = NULL;
  
  // Set Defaults
  parseArgs(argc, argv);
  readCard();
  setupConfig();

}


//*************************************
void splineRoutines::parseArgs(int argc, char* argv[]){
//*************************************
  
  // If No Arguments print commands
  for (int i = 1; i< argc; ++i){
    if (i+1 != argc){

      // Cardfile
      if (!std::strcmp(argv[i], "-c")) { cardFile=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-f")) { splineStrategy=argv[i+1]; ++i;}
      else if (!std::strcmp(argv[i], "-q")) { configCmdFix.push_back(argv[i+1]); ++i;}
      else if (!std::strcmp(argv[i], "-o")) { outFile=argv[i+1]; ++i; }
    } else std::cerr << "ERROR: unknown command line option given! - '" <<argv[i]<<" "<<argv[i+1]<<"'"<< std::endl;
  }

  // Make output
  outRootFile = new TFile(outFile.c_str(),"RECREATE");
  
  // Parse fit routine
  std::string token;
  std::istringstream stream(splineStrategy);

  LOG(FIT)<< "Splines Routine = " << splineStrategy <<std::endl;
  while(std::getline(stream, token, ',')) spline_routines.push_back(token);
  if (spline_routines.empty()) spline_routines.push_back("Generate");
  

  return;
};




//*************************************
void splineRoutines::readCard(){
  //*************************************

  std::string line;
  std::ifstream card(this->cardFile.c_str(), ifstream::in);

  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);

    readParameters(line);
    readSplines(line);
    readSamples(line);
  }
  card.close();
  
  return;
};


//*****************************************
void splineRoutines::readParameters(std::string parstring){
//******************************************

  std::string token, parname;
  std::istringstream stream(parstring);   int val = 0;
  double entry;
  int partype = -1;
  std::string curparstate = "";

  if (parstring.c_str()[0] == '#') return;

  while(std::getline(stream, token, ' ')){

    stream >> std::ws;

    std::istringstream stoken(token);
    if (val > 1 and val < 6) stoken >> entry;

    // Allow (parameter name val FIX)
    if (val > 2 and val < 6 and token.find("FIX") != std::string::npos) {
      fixVals[parname] = true;
      break;
    }

    if (val == 0 &&
	token.compare("niwg_parameter") &&
	token.compare("neut_parameter") &&
	token.compare("genie_parameter") &&
	token.compare("nuwro_parameter") &&
	token.compare("custom_parameter") &&
	token.compare("t2k_parameter")) {

      return;

    } else if (val == 0){

      if (!token.compare("neut_parameter")) partype = 0;
      else if (!token.compare("niwg_parameter")) partype = 1;
      else if (!token.compare("genie_parameter")) partype = 2;
      else if (!token.compare("nuwro_parameter")) partype = 3;
      else if (!token.compare("custom_parameter")) partype = 4;
      else if (!token.compare("t2k_parameter")) partype = 5;

    } else if (val == 1) {
      params.push_back(token);
      parname = token;
      std::cout << parname << std::endl;

      // Set Type
      params_type[parname] = partype;

      // Defaults
      startVals[parname] = 0.0;
      currentVals[parname] = 0.0;
      errorVals[parname]   = 0.0;

      minVals[parname] = -1.0;
      maxVals[parname] = 1.0;
      stepVals[parname] = 0.5;

    } else if (val == 2){  // Nominal
      startVals[parname] = entry;
      currentVals[parname] = entry;
    } else if (val == 3){  minVals[parname] = entry;  // min
    } else if (val == 4){  maxVals[parname] = entry;  // max
    } else if (val == 5){  stepVals[parname] = entry; // step
    } else if (val == 6){ // type
      fixVals[parname] = ( token.find("FIX") != std::string::npos );

      curparstate = token;

    } else break;

    val++;
  }

  // Run Dial Conversions
  if (curparstate.find("ABS") != std::string::npos){
    double tempstart = startVals[parname];

    LOG(FIT)<<"Converting abs dial "<<parname<<" : "<<startVals[parname];
    TF1 convfunc;// = GetConversionFunction(parname);

    startVals[parname] = convfunc.GetX(startVals[parname],-999,999);

    if ((fabs(startVals[parname])) < 1E-10) startVals[parname] = 0.0;

    currentVals[parname] = convfunc.GetX(currentVals[parname],-999,999);
    minVals[parname] = convfunc.GetX(minVals[parname],-999,999);
    maxVals[parname] = convfunc.GetX(maxVals[parname],-999,999);

    stepVals[parname] = convfunc.GetX(stepVals[parname] + tempstart,-999,999) - startVals[parname];

    LOG(MIN)<<" -> "<<startVals[parname]<<std::endl;

  }
  return;
}




//******************************************
void splineRoutines::readSamples(std::string sampleString){
  //******************************************

  std::string token, samplename;
  std::istringstream stream(sampleString);   int val = 0;
  double entry;

  if (sampleString.c_str()[0] == '#') return;


  while(std::getline(stream, token, ' ')){
    stream >> std::ws;    // strip whitespace
    std::istringstream stoken(token);
    stoken >> entry;


    if (val == 0){
      if (token.compare("sample") != 0){ return; }
    } else if (val == 1){
      samplename = token;
      samples.push_back(token);
    } else if (val == 2) { sampleTypes[samplename]    = token;
    } else if (val == 3) { sampleInFiles[samplename]  = token;
    } else if (val == 4) { sampleOutTypes[samplename] = token;
    } else if (val == 5) { sampleOutFiles[samplename] = token; 
    }

    val++;
  }
  return;
}

//******************************************
void splineRoutines::readSplines(std::string splineString){
//******************************************

  std::string token, splinename;
  std::istringstream stream(splineString);   int val = 0;
  double entry;

  if (splineString.c_str()[0] == '#') return;


  while(std::getline(stream, token, ' ')){
    stream >> std::ws;    // strip whitespace
    std::istringstream stoken(token);
    stoken >> entry;


    if (val == 0){
      if (token.compare("spline") != 0){ return; }
    } else if (val == 1){
      splinename = token;
      splineNames.push_back(token);
    } else if (val == 2) {
      splineTypes[splinename]  = token;
    } else if (val == 3) {
      splinePoints[splinename] = token;
    }

    val++;
  }
  return;
}


//*************************************
void splineRoutines::setupConfig(){
//*************************************

  FitPar::Config().ReadParamFile( std::string(std::getenv("EXT_FIT"))+"/parameters/fitter.requirements.dat" );
  FitPar::Config().ReadParamFile( cardFile );

  for (unsigned int iter = 0; iter < configCmdFix.size(); iter++)
    FitPar::Config().ForceParam(configCmdFix[iter]);

  LOG_VERB(FitPar::Config().GetParS("VERBOSITY"));
  ERR_VERB(FitPar::Config().GetParS("VERBOSITY"));

  return;
};

//*************************************
void splineRoutines::setupRWEngine(){
//*************************************

  if (rw) delete rw;
  rw = new FitWeight();

  for (UInt_t i = 0; i < params.size(); i++){
    std::string name = params[i];
    std::cout << "Adding parameter " << name << std::endl;
    rw -> IncludeDial(name, params_type.at(name) );
  }
  updateRWEngine(startVals);

  return;
}


//*************************************
void splineRoutines::updateRWEngine(std::map<std::string,double>& updateVals){
//*************************************

  for (UInt_t i = 0; i < params.size(); i++){
    std::string name = params[i];

    if (updateVals.find(name) == updateVals.end()) continue;
    rw->SetDialValue(name,updateVals.at(name));
  }

  rw->Reconfigure();
  return;
}

//*************************************  
void splineRoutines::GenerateSampleSplines(){
//*************************************

  // Make a new RW Engine
  if (rw) delete rw;
  this->setupRWEngine();

  // Set RW engine to central values
  this->updateRWEngine(currentVals);
  
  // Add Splines to RW Engine
  for (UInt_t i = 0; i < splineNames.size(); i++){
    std::string name = splineNames.at(i);
    std::vector<std::string> pos_spline_types = PlotUtils::FillVectorSFromString(splineTypes[name],",");
    rw->SetupSpline( name, pos_spline_types[0], splinePoints[name] );
  }
  
  // Make list of samples
  std::list<MeasurementBase*> sample_class_list;
  for (UInt_t i = 0; i < samples.size(); i++){
    std::string name = samples.at(i);
    SampleUtils::LoadSample(&sample_class_list,
			    name, sampleInFiles[name], sampleTypes[name], "", rw);
    
  }

  // Sort verbosity before
  int verbosity      = FitPar::Config().GetParI("VERBOSITY");
  
  // Loop over list of samples
  std::list<MeasurementBase*>::iterator iter = sample_class_list.begin();
  int count = 0;
  for ( ; iter != sample_class_list.end(); iter++){
    std::string name = samples.at(count);
    std::string output_name = sampleOutFiles[name];
    
    
    bool onlySignal   = (sampleOutTypes[name].find("SIG") != std::string::npos); // Otherwise its ALL
    bool onlyBaseEvt  = (sampleOutTypes[name].find("BAS") != std::string::npos); // Otherwise its FUL
    LOG(FIT) <<"Generating for "<< name << " "
	     << sampleTypes[name] << " " << sampleInFiles[name] << " " 
	     << sampleOutTypes[name] << " " << sampleOutFiles[name] << std::endl;


    // GENERATING CODE -------------------
    MeasurementBase* exp = (*iter);

    // Check that the experiment is not a joint measurement.

    
    InputHandler* input = exp->GetInput();

    FitEvent* cust_event = NULL;
    //    FitEventBase* base_event = NULL;
    TFile* splineFile = new TFile(output_name.c_str(), "RECREATE");
    TTree* eventTree = new TTree( (name + "_splineEvents").c_str(),
				  (name + "_splineEvents").c_str());
    
    cust_event = static_cast<FitEvent*>(input->GetEventPointer());
    //    base_event = static_cast<FitEventBase*>(input->GetSignalPointer());
    
    //    if (!onlyBaseEvt){
    rw->SetupEventCoeff(cust_event);
    eventTree->Branch("splineEvent",&cust_event);
    //} else {
    //      rw->SetupEventCoeff(static_cast<FitEvent*>(base_event));
    //      eventTree->Branch("splineEvent",&base_event);
    //    }
  
    bool signal;
    int nevents = input->GetNEvents();
    int countwidth = (nevents / 100);
    
    for (int i = 0; i < nevents; i++){

      input->ReadEvent(i);
      exp->FillEventVariables(cust_event);
    
      cust_event->Signal = false;
      signal = exp->isSignal(cust_event);
      
      if (signal or !onlySignal){
	rw->GenSplines(static_cast<FitEventBase*>(cust_event));
	rw->CalcWeight(cust_event);

	//	if (onlyBaseEvt)
	//	  (*base_event) = static_cast<FitEventBase*>(cust_event);
	
	eventTree->Fill();
      }

      if (i % countwidth == 0){
	LOG(FIT) << "Produced " << i <<" spline events."<<std::endl;
      }
    }
    std::cout<<"Saving spline results to "<<output_name.c_str()<<std::endl;
    
    // Save the tree and header to file
    //    rw->GetSplineHeader()->ngen_events = input->GetNEvents();
    std::cout<<"Writing head"<<std::endl;
    std::cout<<rw->GetSplineHeader()<<std::endl;
    rw->GetSplineHeader()->Write( (name + "_splineHead").c_str() );

    std::cout<<"Writing TTree"<<std::endl;
    eventTree->Write( (name + "_splineEvents").c_str() );

    std::cout<<"Saving Histograms"<<std::endl;
    input->GetFluxHistogram()->Write();
    input->GetEventHistogram()->Write();
    input->GetXSecHistogram()->Write();

    std::cout<<"Close"<<std::endl;
    // Tidy up
    splineFile->Close();

    count++;
  }

  // Delete RW Engine and measurements to tidy up
  delete rw;
}

//************************************* 
void splineRoutines::Run(){
//************************************* 

  for (UInt_t i = 0; i < spline_routines.size(); i++){
    std::string routine = spline_routines.at(i);
    if (!routine.compare("Generate")) this->GenerateSampleSplines();
    if (!routine.compare("MakePlots")) this->MakeSplinePlots();
    if (!routine.compare("ValiBins")) this->ValidateSplineBinResponse();
    if (!routine.compare("ValiEvents")) this->ValidateSplineEventResponse();
    if (!routine.compare("ValiPlots")) this->ValidePlots();
    if (!routine.compare("CheckPlots")) this->CheckSplinePlots();
  }
}

//************************************* 
void splineRoutines::CheckSplinePlots(){
//************************************* 

  outRootFile->cd();
  TDirectory* valDIR = (TDirectory*) outRootFile->mkdir( "check_plots" );
  valDIR->cd();


  
  
  

}




//*************************************  
void splineRoutines::MakeSplinePlots(){
//*************************************

  outRootFile->cd();
  TDirectory* valDIR = (TDirectory*) outRootFile->mkdir( "spline_plots" );
  valDIR->cd();
  
  // Add Splines to RW Engine
  for (UInt_t i = 0; i < splineNames.size(); i++){

    // Make a new RW Engine
    if (!rw) this->setupRWEngine();

    // Set RW engine to central values
    this->updateRWEngine(currentVals);

    std::string name = splineNames.at(i);
    TDirectory* splineDIR = (TDirectory*) valDIR->mkdir( name.c_str() );
    splineDIR->cd();
    
    std::vector<std::string> pos_spline_types = PlotUtils::FillVectorSFromString(splineTypes[name],",");
    UInt_t n_pos_spline_types = pos_spline_types.size();

    rw->ResetSplines();
    for (UInt_t j = 0; j < n_pos_spline_types; j++){
      rw->SetupSpline( name, pos_spline_types[j], splinePoints[name] );
    }

    // Loop over measurements and make plots
    for (UInt_t i = 0; i < samples.size(); i++){
      std::string name = samples.at(i);
          
      InputHandler* input  = new InputHandler( name, sampleInFiles[name] );

      FitEvent* cust_event = static_cast<FitEvent*>(input->GetEventPointer());
      rw->SetupEventCoeff(cust_event);
      
      int nevents = 1000; //input->GetNEvents();
      
      splineDIR->cd();
      for (int i = 0; i < nevents; i++){
	input->ReadEvent(i);
	rw->GenSplines(static_cast<FitEventBase*>(cust_event), true);
      }
      
      delete input;
    }
  }
  return;
}


//*************************************  
void splineRoutines::ValidePlots(){
//*************************************

  outRootFile->cd();

  // Make a new RW Engine
  if (!rw) this->setupRWEngine();

  // Set RW engine to central values
  this->updateRWEngine(currentVals);

  double rw_weight_splines;
  double rw_weight_events;
  double rw_weight_dif;

  // Loop over samples
  for (UInt_t i = 0; i < samples.size(); i++){
    std::string name = samples.at(i);

    // Generate input handler using splines.
    InputHandler* input_splines = new InputHandler( name, "EVSPLN:" + sampleOutFiles[name] );
    FitEvent* cust_event_splines = static_cast<FitEvent*>(input_splines->GetEventPointer());

    // Generate input handler using events.
    InputHandler* input_events = new InputHandler( name, sampleInFiles[name] );
    FitEvent* cust_event_events = static_cast<FitEvent*>(input_events->GetEventPointer());

    // Create a loop that compares event weights
    int nevents = 1000; //input_splines->GetNEvents();

    
    // 1D Scans
    for (int i = 0; i < nevents; i++){

      input_splines->GetTreeEntry(i);
      input_events->GetTreeEntry(i);

      for (UInt_t j = 0; j < params.size(); j++){

	std::string syst = params[j];
	if (fixVals[syst]) continue;

	double cur  = currentVals[syst];
	double low  = minVals[syst];
	double high = maxVals[syst];
	double step = stepVals[syst];
	int npoints = abs((low - high) / step);

	TGraph* gr_splines = new TGraph();
	TGraph* gr_events  = new TGraph();

	bool hasresponse = false;
	
	for (UInt_t k = 0; k < npoints+1; k++){

	  double val = low + (k + 0.)*(high - low)/(npoints+0.);
	  rw->SetDialValue(rw->GetDialEnum(syst), val);
	  rw->Reconfigure();

	  rw->ReadSplineHead(input_splines->GetSplineHead());
	  rw->Reconfigure();

	  rw_weight_splines = rw->CalcWeight(cust_event_splines);
	  rw_weight_events  = rw->CalcWeight(cust_event_events);

	  std::cout<<"splines event val "<<rw_weight_splines<<" "<<rw_weight_events<<" "<<val<<std::endl;

	  if (rw_weight_splines != 1.0 or rw_weight_events != 1.0) hasresponse = true;
	  
	  gr_splines->SetPoint( gr_splines->GetN(), val, rw_weight_splines );
	  gr_events ->SetPoint( gr_events ->GetN(), val, rw_weight_events  );
	  
	}

	// Save Graph
	outRootFile->cd();
	if (hasresponse){
	  gr_splines->Write(Form("spline_points_%s_evt_%i",syst.c_str(),i));
	  gr_events->Write(Form("event_points_%s_evt_%i",syst.c_str(),i));
	}

	// Reset RW Engine
	rw->SetDialValue(rw->GetDialEnum(syst), cur);
	rw->Reconfigure();
	
      }
    }
  }
}





//*************************************  
void splineRoutines::ValidateSplineBinResponse(){
//*************************************  

  // Do same as below, but take the difference for all bins, aswell as a total integral, as well as a chi2 result.
  // Do same sorts of plots but with these values being filled.

}


//*************************************
void splineRoutines::ValidateSplineEventResponse(){
//*************************************

  outRootFile->cd();

  // Make a new RW Engine
  if (!rw) this->setupRWEngine();

  // Set RW engine to central values
  this->updateRWEngine(currentVals);
  
  double rw_weight_splines;
  double rw_weight_events; 
  double rw_weight_dif;
  
  // Loop over samples
  for (UInt_t i = 0; i < samples.size(); i++){
    std::string name = samples.at(i);

    // Generate input handler using splines.
    InputHandler* input_splines = new InputHandler( name, "EVSPLN:" + sampleOutFiles[name] );
    FitEvent* cust_event_splines = static_cast<FitEvent*>(input_splines->GetSignalPointer());
    
    // Generate input handler using events.
    InputHandler* input_events = new InputHandler( name, sampleInFiles[name] );
    FitEvent* cust_event_events = static_cast<FitEvent*>(input_events->GetEventPointer());

    // Create a loop that compares event weights
    int nevents = input_splines->GetNEvents();
    
    // 1D Scans
    for (UInt_t j = 0; j < params.size(); j++){
      
      std::string syst = params[j];
      if (fixVals[syst]) continue;
      
      double low  = minVals[syst];
      double high = maxVals[syst];
      double step = stepVals[syst];
      int npoints = abs((high - low) / step);
      
      TGraph* gr_1D = new TGraph();
      gr_1D->SetNameTitle((syst+"_avg_dif").c_str(),(syst+"_avg_dif").c_str());

      TGraph* gr_1D_total = new TGraph();
      gr_1D_total->SetNameTitle((syst+"_tot_dif").c_str(),(syst+"_tot_dif").c_str());
      
      TGraph* gr_1D_event = new TGraph();
      gr_1D_event->SetNameTitle((syst+"_evt").c_str(),(syst+"_evt").c_str());

      TGraph* gr_1D_spline = new TGraph();
      gr_1D_spline->SetNameTitle((syst+"_spl").c_str(),(syst+"_spl").c_str());

      TGraph* gr_1D_max = new TGraph();
      gr_1D_max->SetNameTitle((syst+"_max_dif").c_str(),(syst+"_max_dif").c_str());
      
      for (UInt_t k = 0; k < npoints+1; k++){

	double val = low + (k + 0.)*(high - low)/(npoints+0.);
	rw->SetDialValue(rw->GetDialEnum(syst), val);
	rw->Reconfigure();
	
	double total = 0.0;
	double total_splines = 0.0;
	double total_events = 0.0;
	double total_max = 0.0;
	
	int count = 0;
	
	rw->ReadSplineHead(input_splines->GetSplineHead());
	rw->Reconfigure();
	
	for (int i = 0; i < nevents; i++){
	  	  
	  input_splines->GetTreeEntry(i);
	  input_events->GetTreeEntry(i);

	  rw_weight_splines = rw->CalcWeight(cust_event_splines);
	  rw_weight_events  = rw->CalcWeight(cust_event_events);
	  
	  rw_weight_dif = fabs(rw_weight_splines - rw_weight_events);

	  if (rw_weight_splines != 1.0 or rw_weight_events != 0.0){
	    total_splines += rw_weight_splines;
	    total_events += rw_weight_events;
	    total += fabs(rw_weight_dif);
	    total_max = max(total_max, rw_weight_dif);
	    count += 1;
	  }
	}
	
	gr_1D_spline->SetPoint(gr_1D_spline->GetN(), val, total_splines / (count + 0.));
	gr_1D_event->SetPoint(gr_1D_event->GetN(),   val, total_events / (count + 0.));
	gr_1D->SetPoint( gr_1D->GetN(), val, total / (count + 0.) );
	gr_1D_total->SetPoint(gr_1D_total->GetN(), val, total);
	gr_1D_max->SetPoint(gr_1D_max->GetN(), val, total_max);
      }
      outRootFile->cd();
      gr_1D_spline->Write();
      gr_1D_event->Write();
      gr_1D->Write();
      gr_1D_total->Write();
      gr_1D_max->Write();
    }
  }  
}
