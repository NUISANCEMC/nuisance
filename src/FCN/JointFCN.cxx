#include "JointFCN.h"
#include <stdio.h> 
#include "FitUtils.h"

//*************************************************** 
JointFCN::JointFCN(std::string cardfile,  TFile *outfile){
//***************************************************
  
  fOutputDir   = gDirectory;
  fCardFile = cardfile;
  
  LoadSamples(fCardFile);

  fCurIter = 0;
  fMCFilled = false;

  fOutputDir->cd();
  
  fIterationTree = NULL;
  fDialVals = NULL;
  fNDials = 0;
  
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

  // Loop over all Measurement Classes
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end(); iter++){
    MeasurementBase* exp = *iter;

    // If RW Either do signal or full reconfigure.
    if (fDialChanged or !fMCFilled or fullconfig){
      if (!fullconfig and fMCFilled) exp->ReconfigureFast();
      else                           exp->Reconfigure();
      
    // If RW Not needed just do normalisation
    } else {      
      exp->Renormalise();
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
