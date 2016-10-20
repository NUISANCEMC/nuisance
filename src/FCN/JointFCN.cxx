#include "JointFCN.h"
#include <stdio.h> 
#include "FitUtils.h"

//*************************************************** 
JointFCN::JointFCN(std::string cardfile,  TFile *outfile){
//***************************************************
  
  fOutputDir   = gDirectory;
  FitPar::Config().out = outfile;

  fCardFile = cardfile;
  
  LoadSamples(fCardFile);

  fCurIter = 0;
  fMCFilled = false;

  fOutputDir->cd();
  
  fIterationTree = NULL;
  fDialVals = NULL;
  fNDials = 0;

  fUsingEventManager = FitPar::Config().GetParB("EventManager");
};

//*************************************************** 
JointFCN::~JointFCN() {
//***************************************************

  // Delete Samples
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end(); iter++){
    MeasurementBase* exp = *iter;
    delete exp;
  }

  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++){
    ParamPull* pull = *iter;
    delete pull;
  }
  
  
  // Sort Tree
  if (fIterationTree) DestroyIterationTree();
  if (fDialVals) delete fDialVals;
  if (fSampleLikes) delete fSampleLikes;
  
};

//*************************************************** 
void JointFCN::CreateIterationTree(std::string name, FitWeight* rw){
//***************************************************

  LOG(FIT) << " Creating new iteration tree! " << endl;
  if (fIterationTree &&
      !name.compare(fIterationTree->GetName())){

    fIterationTree->Reset();
    return;
  }
  
  fIterationTree = new TTree(name.c_str(), name.c_str());

  // Setup Main Branches
  fIterationTree->Branch("total_likelihood",&fLikelihood,"total_likelihood/D");
  fIterationTree->Branch("total_ndof", &fNDOF, "total_ndof/I");

  // Setup Sample Arrays
  int ninputs = fSamples.size() + fPulls.size();
  fSampleLikes = new double[ninputs];
  fSampleNDOF  = new int[ninputs];
  fNDOF = GetNDOF();

  // Setup Sample Branches
  int count = 0;
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end(); iter++){
    MeasurementBase* exp = *iter;
    
    std::string name = exp->GetName();
    std::string liketag = name + "_likelihood";
    std::string ndoftag = name + "_ndof";
    
    fIterationTree->Branch(liketag.c_str(), &fSampleLikes[count], (liketag + "/D").c_str());
    fIterationTree->Branch(ndoftag.c_str(), &fSampleNDOF[count],  (ndoftag + "/D").c_str());
    
    count++;
  }

  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++){
    ParamPull* pull = *iter;

    std::string name = pull->GetName();
    std::string liketag = name + "_likelihood";
    std::string ndoftag = name + "_ndof";

    fIterationTree->Branch(liketag.c_str(), &fSampleLikes[count], (liketag + "/D").c_str());
    fIterationTree->Branch(ndoftag.c_str(), &fSampleNDOF[count],  (ndoftag + "/D").c_str());

    count++;
  }

  
  
  // Add Dial Branches
  std::vector<std::string> dials = rw->GetDialNames();
  fNDials = dials.size();
  fDialVals = new double[fNDials];
  
  for (int i = 0; i < fNDials; i++){
    fIterationTree->Branch( dials[i].c_str(), &fDialVals[i],
			    (dials[i] + "/D").c_str() );
  }
}

//***************************************************   
void JointFCN::DestroyIterationTree(){
//***************************************************
  
  if (!fIterationTree){
    delete fIterationTree;
  }
}

//***************************************************   
void JointFCN::WriteIterationTree(){
//***************************************************
  
  if (!fIterationTree){
    ERR(FTL) << "Can't save empty iteration tree!" << endl;
    throw;
  }
  fIterationTree->Write();
}

//***************************************************   
void JointFCN::FillIterationTree(FitWeight* rw) {
//***************************************************
  
  if (!fIterationTree){
    ERR(FTL) << "Trying to fill iteration_tree when it is NULL!" << endl;
    throw;
  }
  
  rw->GetAllDials( fDialVals, fNDials );
  fIterationTree->Fill();
}



//***************************************************   
double JointFCN::DoEval(const double *x)  {
//***************************************************
  
  // WEIGHT ENGINE
  fDialChanged = FitBase::GetRW()->HasRWDialChanged(x);
  FitBase::GetRW()->UpdateWeightEngine(x);
  if (fDialChanged) {
    FitBase::GetRW()->Reconfigure();
    FitBase::EvtManager().ResetWeightFlags();
  }

  // SORT SAMPLES
  ReconfigureSamples();

  // GET TEST STAT
  fLikelihood = GetLikelihood();

  // PRINT PROGRESS
  LOG(FIT) << "Current Stat (iter. "<< this->fCurIter << ") = "<<fLikelihood<<std::endl;

  // UPDATE TREE
  if (fIterationTree) FillIterationTree(FitBase::GetRW());
  
  return fLikelihood;
}

//***************************************************   
int JointFCN::GetNDOF() {
//***************************************************
  
  int totaldof = 0;
  int count = 0;
  
  // Total number of Free bins in each MC prediction
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end(); iter++){
    MeasurementBase* exp = *iter;
    int dof = exp->GetNDOF();

    // Save Seperate DOF
    if (fIterationTree){
      fSampleNDOF[count] = dof;
    }

    // Add to total
    totaldof += dof;
    count++;
  }

  // Loop over pulls
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++){
    ParamPull* pull = *iter;
    double dof = pull->GetLikelihood();

    // Save seperate DOF
    if (fIterationTree){
      fSampleNDOF[count] = dof;
    }

    // Add to total
    totaldof += dof;
    count++;
  }

  // Set Data Variable
  fNDOF = totaldof;
  
  return totaldof;
}

//*************************************************** 
double JointFCN::GetLikelihood()  {
//***************************************************
  LOG(MIN) << "Getting likelihoods..." << endl;
  
  // Loop and add up likelihoods in an uncorrelated way
  double like = 0.0;
  int count = 0;
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end(); iter++){
    
    MeasurementBase* exp = *iter;
    double newlike = exp->GetLikelihood();

    // Save seperate likelihoods
    if (fIterationTree){
      fSampleLikes[count] = newlike;
    }

    LOG(MIN) << "-> " << exp->GetName() << " : " << newlike << endl;

    // Add Weight Scaling
    like *= FitBase::GetRW()->GetSampleLikelihoodWeight( exp->GetName() );
    
    // Add to total
    like += newlike;
    count++;
  }

  // Loop over pulls
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++){
    ParamPull* pull = *iter;
    double newlike = pull->GetLikelihood();

    // Save seperate likelihoods
    if (fIterationTree){
      fSampleLikes[count] = newlike;
    }

    // Add to total
    like += newlike;
    count++;
  }

  // Set Data Variable
  LOG(MIN) << "Total Likelihood = " << like << endl;
  fLikelihood = like;

  return like;
};

//***************************************************
void JointFCN::LoadSamples(std::string cardinput)
//***************************************************
{
  LOG(MIN)<<"Initializing Samples"<<std::endl;

  // Read the card file here and load objects
  std::string line;
  std::ifstream card(cardinput.c_str(), ifstream::in);

  // Make sure they are created in correct working DIR
  fOutputDir->cd();
  
  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);

    // Skip Empties
    stream >> std::ws;
    if (line.c_str()[0] == '#') continue;
    if (line.empty()) continue;

    // Parse line
    std::vector<std::string> samplevect = PlotUtils::ParseToStr(line," ");

    // Sample Inputs
    if (!samplevect[0].compare("sample")){;
      
      // Get all inputs
      std::string name  = samplevect[1];
      std::string files = samplevect[2];
      std::string type = "DEFAULT";
      if (samplevect.size() > 3)
	type  = samplevect[3];
      
      // Create Sample Class
      LOG(MIN) << "Loading up sample: "<<name<<" << " <<files << " ("<<type<<")"<<std::endl;
      std::string fakeData = "";
      fOutputDir->cd();
      bool LoadedSample = SampleUtils::LoadSample( &fSamples, name, files, type, fakeData, FitBase::GetRW() );
      
      if (!LoadedSample) {
	ERR(FTL) << "Could not load sample provided: "<<name<<std::endl;
	ERR(FTL) << "Check spelling with that in src/FCN/SampleList.cxx" << endl;
	throw;
      }
    }

    // Sample Inputs
    if (!samplevect[0].compare("covar") ||
	!samplevect[0].compare("pulls") ||
	!samplevect[0].compare("throws")) {

      // Get all inputs
      std::string name  = samplevect[1];
      std::string files = samplevect[2];
      std::string type = "DEFAULT";
      
      if (samplevect.size() > 3){
	type  = samplevect[3];
      } else if (!samplevect[0].compare("pull")){
	type = "GAUSPULL";
      } else if (!samplevect[0].compare("throws")){
	type = "GAUSTHROWS";
      }

      // Create Pull Class
      LOG(MIN) << "Loading up pull term: "<<name<<" << " <<files << " ("<<type<<")"<<std::endl;
      std::string fakeData = "";
      fOutputDir->cd();
      fPulls.push_back( new ParamPull( name, files, type ) );
    }
  }
  card.close();
};


//***************************************************
void JointFCN::ReconfigureSamples(bool fullconfig) {
//***************************************************
  
  int starttime = time(NULL);
  LOG(MIN) << "Starting Reconfigure iter. "<<this->fCurIter<<endl;


  // Event Manager Reconf
  if (fUsingEventManager){
    
    if (!fullconfig and fMCFilled) ReconfigureFastUsingManager();
    else ReconfigureUsingManager();
    
  } else {
  
    // Loop over all Measurement Classes
    for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end(); iter++){
      MeasurementBase* exp = *iter;
      
      
      // If RW Either do signal or full reconfigure.
      if (fDialChanged or !fMCFilled or fullconfig){	
	if (!fullconfig and fMCFilled) exp->ReconfigureFast();
	else	  exp->Reconfigure();
      
	// If RW Not needed just do normalisation
      } else {      
	exp->Renormalise();
      }    
    }
  }

  // Loop over pulls and update
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++){
    ParamPull* pull = *iter;
    pull->Reconfigure();
  }
  
  fMCFilled = true;
  LOG(MIN) << "Finished Reconfigure iter. " << fCurIter 
	   << " in " << time(NULL) - starttime << "s"<<endl;

  fCurIter++;
}

//*************************************************** 
void JointFCN::ReconfigureSignal(){
//*************************************************** 
  this->ReconfigureSamples(false);
}

//*************************************************** 
void JointFCN::ReconfigureAllEvents() {
//***************************************************
  FitBase::GetRW()->Reconfigure();
  FitBase::EvtManager().ResetWeightFlags();
  this->ReconfigureSamples(true);
}


//*************************************************** 
void JointFCN::ReconfigureUsingManager() {
//***************************************************
  LOG(MIN) << "Using manager." << endl;
  FitBase::EvtManager().ResetWeightFlags();
  
  // Get list of inputs
  std::map<int, InputHandler*> fInputs = FitBase::EvtManager().GetInputs();
  std::map<int, InputHandler*>::const_iterator iterInp;
  
  // Check Matchup
  for (iterInp = fInputs.begin(); iterInp != fInputs.end(); iterInp++) {
    int input_id = (iterInp->first);
    LOG(REC) << "Input " << input_id << " Valid for : " << endl;

    for (MeasListConstIter iterSam = fSamples.begin();
	 iterSam != fSamples.end(); iterSam++){
      
      MeasurementBase* exp = (*iterSam);
      int exp_id = exp->GetInputID();
      if (exp_id != input_id) continue;
      
      LOG(REC) << " ---> " << exp->GetName() << endl;
    }
  }

  // Start looping over inputs
  for (iterInp = fInputs.begin(); iterInp != fInputs.end(); iterInp++) {
    
    int input_id = (iterInp->first);
    InputHandler* cur_input = (iterInp->second);
    
    FitEvent* cevent = cur_input->GetEventPointer();
    int fNEvents = cur_input->GetNEvents();
    int countwidth = (fNEvents / 10);
    
    // MAIN EVENT LOOP
    for (int i = 0; i < fNEvents; i++) {
      
      // Get Event from input list
      cevent = FitBase::EvtManager().GetEvent(input_id, i);

      // Get Weight
      double Weight = (FitBase::GetRW()->CalcWeight(cevent)	\
		       * cevent->InputWeight);

      // Skip if dodgy event
      if (fabs(cevent->Mode) > 60 || cevent->Mode == 0)
	continue;

      // Remove bad weights
      if (Weight > 200.0 || Weight <= 0.0)
	Weight = 0.0;
      
      // Loop over samples and fill histograms
      bool foundsignal = false;
      int j = 0;
      for (MeasListConstIter iterSam = fSamples.begin();
	   iterSam != fSamples.end(); iterSam++, j++){
	
	MeasurementBase* exp = (*iterSam);
	int exp_id = exp->GetInputID();
	if (exp_id != input_id) continue;

	// Reset State of event by event vars
	exp->SetXVar(-999.9);
	exp->SetYVar(-999.9);
	exp->SetZVar(-999.9);
	exp->SetMode(-999);
	exp->SetSignal(false);
	
	// Fill exp
	exp->SetMode(cevent->Mode);
	exp->FillEventVariables(cevent);
	bool signal = exp->isSignal(cevent);
	if (signal) foundsignal = true;
	exp->SetSignal(signal);
	
	exp->SetWeight(Weight);
	exp->FillHistograms();
      }

      // Print Out
      if (LOG_LEVEL(REC) and i % countwidth == 0)
	LOG(REC) << "Reconfigured " << i << " total events. W=" << Weight
		 << " for input " << input_id 
		 << std::endl;
    }
  }

  // Convert Binned events
  MeasListConstIter iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);
    exp->ConvertEventRates();
  }

  return;
}
  
//*************************************************** 
void JointFCN::ReconfigureFastUsingManager() {
//***************************************************

  // Using normal event manager for now until this is developed further.
  ReconfigureUsingManager();
  return;
  
  return;
}

//*************************************************** 
void JointFCN::Write(){
//*************************************************** 
 
  // Loop over individual experiments and call Write  
  LOG(MIN)<<"Writing each of the data classes..."<<endl;
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end(); iter++){
    
    MeasurementBase* exp = *iter;
    exp->Write();
    
  }
  
  // Save Pull Terms
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++){
    ParamPull* pull = *iter;
    pull->Write();
  }

  if (FitPar::Config().GetParB("EventManager")){

    // Get list of inputs 
    std::map<int, InputHandler*> fInputs = FitBase::EvtManager().GetInputs();
    std::map<int, InputHandler*>::const_iterator iterInp;

    for (iterInp = fInputs.begin(); iterInp != fInputs.end(); iterInp++) {
      InputHandler* input = (iterInp->second);

      input->GetFluxHistogram()->Write();
      input->GetXSecHistogram()->Write();
      input->GetEventHistogram()->Write();
    }
  }
  
};

//*************************************************** 
void JointFCN::SetFakeData(std::string fakeinput){
//*************************************************** 

  LOG(MIN) << "Setting fake data from " << fakeinput << endl;
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end(); iter++){

    MeasurementBase* exp = *iter;
    exp->SetFakeDataValues(fakeinput);

  }

  return;
}
