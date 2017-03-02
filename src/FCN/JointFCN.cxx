#include "JointFCN.h"
#include <stdio.h>
#include "FitUtils.h"

//***************************************************
JointFCN::JointFCN(std::string cardfile, TFile* outfile) {
  //***************************************************

  fOutputDir = gDirectory;
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
  fOutputDir->cd();
};


JointFCN::JointFCN(TFile* outfile) {

  fOutputDir = gDirectory;
  if (outfile)
    FitPar::Config().out = outfile;

  std::vector<nuiskey> samplekeys =  Config::QueryKeys("sample");
  LoadSamples(samplekeys);

  fCurIter = 0;
  fMCFilled = false;

  // fOutputDir->cd();

  fIterationTree = NULL;
  fDialVals = NULL;
  fNDials = 0;

  fUsingEventManager = FitPar::Config().GetParB("EventManager");
  fOutputDir->cd();

}

JointFCN::JointFCN(std::vector<nuiskey> samplekeys, TFile* outfile) {

  fOutputDir = gDirectory;
  if (outfile)
    FitPar::Config().out = outfile;

  LoadSamples(samplekeys);

  fCurIter = 0;
  fMCFilled = false;

  fOutputDir->cd();

  fIterationTree = NULL;
  fDialVals = NULL;
  fNDials = 0;

  fUsingEventManager = FitPar::Config().GetParB("EventManager");
  fOutputDir->cd();

}

//***************************************************
JointFCN::~JointFCN() {
  //***************************************************

  // Delete Samples
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;
    delete exp;
  }

  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++) {
    ParamPull* pull = *iter;
    delete pull;
  }

  // Sort Tree
  if (fIterationTree) DestroyIterationTree();
  if (fDialVals) delete fDialVals;
  if (fSampleLikes) delete fSampleLikes;
};

//***************************************************
void JointFCN::CreateIterationTree(std::string name, FitWeight* rw) {
  //***************************************************

  LOG(FIT) << " Creating new iteration tree! " << endl;
  if (fIterationTree && !name.compare(fIterationTree->GetName())) {
    fIterationTree->Reset();
    return;
  }

  fIterationTree = new TTree(name.c_str(), name.c_str());

  // Setup Main Branches
  fIterationTree->Branch("total_likelihood", &fLikelihood,
                         "total_likelihood/D");
  fIterationTree->Branch("total_ndof", &fNDOF, "total_ndof/I");

  // Setup Sample Arrays
  int ninputs = fSamples.size() + fPulls.size();
  fSampleLikes = new double[ninputs];
  fSampleNDOF = new int[ninputs];
  fNDOF = GetNDOF();

  // Setup Sample Branches
  int count = 0;
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;

    std::string name = exp->GetName();
    std::string liketag = name + "_likelihood";
    std::string ndoftag = name + "_ndof";

    fIterationTree->Branch(liketag.c_str(), &fSampleLikes[count],
                           (liketag + "/D").c_str());
    fIterationTree->Branch(ndoftag.c_str(), &fSampleNDOF[count],
                           (ndoftag + "/D").c_str());

    count++;
  }

  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++) {
    ParamPull* pull = *iter;

    std::string name = pull->GetName();
    std::string liketag = name + "_likelihood";
    std::string ndoftag = name + "_ndof";

    fIterationTree->Branch(liketag.c_str(), &fSampleLikes[count],
                           (liketag + "/D").c_str());
    fIterationTree->Branch(ndoftag.c_str(), &fSampleNDOF[count],
                           (ndoftag + "/D").c_str());

    count++;
  }

  // Add Dial Branches
  std::vector<std::string> dials = rw->GetDialNames();
  fNDials = dials.size();
  fDialVals = new double[fNDials];

  for (int i = 0; i < fNDials; i++) {
    fIterationTree->Branch(dials[i].c_str(), &fDialVals[i],
                           (dials[i] + "/D").c_str());
  }
}

//***************************************************
void JointFCN::DestroyIterationTree() {
  //***************************************************

  if (!fIterationTree) {
    delete fIterationTree;
  }
}

//***************************************************
void JointFCN::WriteIterationTree() {
  //***************************************************

  if (!fIterationTree) {
    ERR(FTL) << "Can't save empty iteration tree!" << endl;
    throw;
  }
  fIterationTree->Write();
}

//***************************************************
void JointFCN::FillIterationTree(FitWeight* rw) {
  //***************************************************

  if (!fIterationTree) {
    ERR(FTL) << "Trying to fill iteration_tree when it is NULL!" << endl;
    throw;
  }

  rw->GetAllDials(fDialVals, fNDials);
  fIterationTree->Fill();
}

//***************************************************
double JointFCN::DoEval(const double* x) {
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
  LOG(FIT) << "Current Stat (iter. " << this->fCurIter << ") = " << fLikelihood
           << std::endl;

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
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;
    int dof = exp->GetNDOF();

    // Save Seperate DOF
    if (fIterationTree) {
      fSampleNDOF[count] = dof;
    }

    // Add to total
    totaldof += dof;
    count++;
  }

  // Loop over pulls
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++) {
    ParamPull* pull = *iter;
    double dof = pull->GetLikelihood();

    // Save seperate DOF
    if (fIterationTree) {
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
double JointFCN::GetLikelihood() {
  //***************************************************

  LOG(MIN) << std::left << std::setw(43) << "Getting likelihoods..." << " : " << "-2logL" << endl;

  // Loop and add up likelihoods in an uncorrelated way
  double like = 0.0;
  int count = 0;
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;
    double newlike = exp->GetLikelihood();

    // Save seperate likelihoods
    if (fIterationTree) {
      fSampleLikes[count] = newlike;
    }

    LOG(MIN) << "-> " << std::left << std::setw(40) << exp->GetName() << " : " << newlike << endl;

    // Add Weight Scaling
    // like *= FitBase::GetRW()->GetSampleLikelihoodWeight(exp->GetName());

    // Add to total
    like += newlike;
    count++;
  }

  // Loop over pulls
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++) {
    ParamPull* pull = *iter;
    double newlike = pull->GetLikelihood();

    // Save seperate likelihoods
    if (fIterationTree) {
      fSampleLikes[count] = newlike;
    }

    // Add to total
    like += newlike;
    count++;
  }

  // Set Data Variable
  fLikelihood = like;

  return like;
};

void JointFCN::LoadSamples(std::vector<nuiskey> samplekeys) {

  LOG(MIN) << "Loading Samples " << std::endl;
  for (size_t i = 0; i < samplekeys.size(); i++) {
    nuiskey key = samplekeys[i];

    // Get Sample Options
    std::string samplename = key.GetS("name");
    std::string samplefile = key.GetS("input");
    std::string sampletype = key.GetS("type");
    std::string fakeData = "";

    fOutputDir->cd();
    MeasurementBase* NewLoadedSample
      = SampleUtils::CreateSample(key);

    if (!NewLoadedSample) {
      ERR(FTL) << "Could not load sample provided: " << samplename << std::endl;
      ERR(FTL) << "Check spelling with that in src/FCN/SampleList.cxx"
               << endl;
      throw;
    } else {
      fSamples.push_back(NewLoadedSample);
    }
  }
}


//***************************************************
void JointFCN::LoadSamples(std::string cardinput)
//***************************************************
{
  LOG(MIN) << "Initializing Samples" << std::endl;

  // Read the card file here and load objects
  std::string line;
  std::ifstream card(cardinput.c_str(), ifstream::in);

  // Make sure they are created in correct working DIR
  fOutputDir->cd();

  while (std::getline(card >> std::ws, line, '\n')) {
    // Skip Empties
    if (line.c_str()[0] == '#') continue;
    if (line.empty()) continue;

    // Parse line
    std::vector<std::string> samplevect = GeneralUtils::ParseToStr(line, " ");

    // Sample Inputs
    if (!samplevect[0].compare("sample")) {
      // Get all inputs
      std::string name = samplevect[1];
      std::string files = samplevect[2];
      std::string type = "DEFAULT";
      if (samplevect.size() > 3) type = samplevect[3];

      // Create Sample Class
      LOG(MIN) << "Loading up sample: " << name << " << " << files << " ("
               << type << ")" << std::endl;
      std::string fakeData = "";
      fOutputDir->cd();
      MeasurementBase* NewLoadedSample = SampleUtils::CreateSample(name, files, type,
                                         fakeData, FitBase::GetRW());

      if (!NewLoadedSample) {
        ERR(FTL) << "Could not load sample provided: " << name << std::endl;
        ERR(FTL) << "Check spelling with that in src/FCN/SampleList.cxx"
                 << endl;
        throw;
      } else {
        fSamples.push_back(NewLoadedSample);
      }
    }

    // Sample Inputs
    if (!samplevect[0].compare("covar") || !samplevect[0].compare("pulls") ||
        !samplevect[0].compare("throws")) {
      // Get all inputs
      std::string name = samplevect[1];
      std::string files = samplevect[2];
      std::string type = "DEFAULT";

      if (samplevect.size() > 3) {
        type = samplevect[3];
      } else if (!samplevect[0].compare("pull")) {
        type = "GAUSPULL";
      } else if (!samplevect[0].compare("throws")) {
        type = "GAUSTHROWS";
      }

      // Create Pull Class
      LOG(MIN) << "Loading up pull term: " << name << " << " << files << " ("
               << type << ")" << std::endl;
      std::string fakeData = "";
      fOutputDir->cd();
      fPulls.push_back(new ParamPull(name, files, type));
    }
  }
  card.close();
};

//***************************************************
void JointFCN::ReconfigureSamples(bool fullconfig) {
  //***************************************************

  int starttime = time(NULL);
  LOG(REC) << "Starting Reconfigure iter. " << this->fCurIter << endl;

  // Event Manager Reconf
  if (fUsingEventManager) {
    if (!fullconfig and fMCFilled)
      ReconfigureFastUsingManager();
    else
      ReconfigureUsingManager();

  } else {
    // Loop over all Measurement Classes
    for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
         iter++) {
      MeasurementBase* exp = *iter;

      // If RW Either do signal or full reconfigure.
      if (fDialChanged or !fMCFilled or fullconfig) {
        if (!fullconfig and fMCFilled)
          exp->ReconfigureFast();
        else
          exp->Reconfigure();

        // If RW Not needed just do normalisation
      } else {
        exp->Renormalise();
      }
    }
  }

  // Loop over pulls and update
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++) {
    ParamPull* pull = *iter;
    pull->Reconfigure();
  }

  fMCFilled = true;
  LOG(MIN) << "Finished Reconfigure iter. " << fCurIter << " in "
           << time(NULL) - starttime << "s" << endl;

  fCurIter++;
}

//***************************************************
void JointFCN::ReconfigureSignal() {
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

std::vector<InputHandlerBase*> JointFCN::GetInputList() {

  std::vector<InputHandlerBase*> InputList;

  MeasListConstIter iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);

    std::vector<MeasurementBase*> subsamples = exp->GetSubSamples();
    for (size_t i = 0; i < subsamples.size(); i++) {

      InputHandlerBase* inp = subsamples[i]->GetInput();
      if (std::find(InputList.begin(), InputList.end(), inp) ==  InputList.end()) {
        InputList.push_back(subsamples[i]->GetInput());

      }
    }
  }

  return InputList;
}

std::vector<MeasurementBase*> JointFCN::GetSubSampleList() {

  std::vector<MeasurementBase*> SampleList;

  MeasListConstIter iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);

    std::vector<MeasurementBase*> subsamples = exp->GetSubSamples();
    for (size_t i = 0; i < subsamples.size(); i++) {
      SampleList.push_back(subsamples[i]);
    }
  }

  return SampleList;
}


//***************************************************
void JointFCN::ReconfigureUsingManager() {
//***************************************************

  // New event manager plan
  // ReconfigureEvtManager(){
  // - Resets all our nice event vectors
  // - Setflag: save variables
  // - Gets list of inputs, and list of measurements, and list of input pointers for each measurement.
  // - Loops over all inputs events
  // - Create new SignalBoxVector
  // - Compares event input pointer to current input pointer.
  // - WeightCalc
  // - if match: Run Event filling.
  // - Do this using multithreading.
  // - Call isSignal
  // - Call FillEventVariables
  // - GetPointerToBOX
  // - Call FillHistogramsFromBox()
  // - PushBack BOX->CloneSignalBox() into vector.
  // - After looping over all measurements, if SignBoxVector not empty push it back into a vector and push bool back into another vector.
  // - If pushing back signal, also push back double for event weight.
  // - Call ScaleEvents, etc.

  LOG(SAM) << "Reconfuguringusingeventmanager" << std::endl;
  int timestart = time(NULL);

  // Reset all samples
  MeasListConstIter iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);
    exp->ResetAll();
  }

  // Check saving variables flag
  bool savesignal = (FitPar::Config().GetParB("SignalReconfigures"));
  if (savesignal) {

    // Reset all of our event signal vectors
    fSignalEventBoxes.clear();
    fSignalEventFlags.clear();
    fSampleSignalFlags.clear();

  }

  // Check out list of inputs
  if (fInputList.empty()) {
    fInputList = GetInputList();
    fSubSampleList = GetSubSampleList();
  }

  int fillcount = 0;
  // Loop over all inputs
  int inputcount = 0;
  std::vector<InputHandlerBase*>::iterator inp_iter = fInputList.begin();
  for (; inp_iter != fInputList.end(); inp_iter++) {
    InputHandlerBase* curinput = (*inp_iter);

    FitEvent* curevent = curinput->FirstNuisanceEvent();
    int i = 0;
    int nevents = curinput->GetNEvents();
    int countwidth = nevents / 5;

    // Start event loop
    while (curevent) {

      // Logging
      if (LOG_LEVEL(REC)) {
        if (i % countwidth == 0) {
          LOG(REC) << "Processed " << i << " events." << std::endl;
        }
      }

      // Get Event Weight
      curevent->RWWeight = FitBase::GetRW()->CalcWeight(curevent);
      curevent->Weight = curevent->RWWeight * curevent->InputWeight;
      double rwweight = curevent->Weight;

      // std::cout << "Event " << i << " Weight = " << rwweight << std::endl;

      // Setup flag for if signal found in at least one sample
      bool foundsignal = false;

      // Create a new signal bitset for this event
      std::vector<bool> signalbitset(fSubSampleList.size());

      // Create a new signal box vector for this event
      std::vector<MeasurementVariableBox*> signalboxes;

      // Loop over all samples
      size_t measitercount = 0;
      std::vector<MeasurementBase*>::iterator meas_iter = fSubSampleList.begin();
      for (; meas_iter != fSubSampleList.end(); meas_iter++) {

        // Get Measurement
        MeasurementBase* curmeas = (*meas_iter);

        // Compare input pointers, if != then skip
        if (curinput != curmeas->GetInput()) {

          if (savesignal) {
            // Set bit to 0 as definitely not signal
            signalbitset[measitercount] = 0;
          }
          measitercount++;

          // Skip sample
          continue;
        }


        // Fill events
        MeasurementVariableBox* box = curmeas->FillVariableBox(curevent);
        bool signal = curmeas->isSignal(curevent);

        // Get the event box after fill event variable
        // std::cout << "Filling Meas Full = " << curmeas << std::endl;

        curmeas->FillHistograms(rwweight);
        if (signal) {
          fillcount++;
        }

        if (savesignal) {

          // Fill signal bitset
          signalbitset[measitercount] = signal;

          // If signal save a clone of the event box.
          if (signal) {
            foundsignal = true;
            signalboxes.push_back( box->CloneSignalBox() );
          }

        }

        measitercount++;
      }

      // push into vectors
      if (savesignal) {
        fSignalEventFlags.push_back(foundsignal);

        if (foundsignal) {
          fSignalEventBoxes.push_back(signalboxes);
          fSampleSignalFlags.push_back(signalbitset);
        }
      }

      // iterate
      curevent = curinput->NextNuisanceEvent();
      i++;
    }
    inputcount++;
  }

  // Now loop over all Measurements
  // Convert Binned events
  iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);
    exp->ConvertEventRates();
  }

  LOG(REC) << "Filled " << fillcount << " signal events." << std::endl;
  if (savesignal){
    double mem = ( //sizeof(fSignalEventBoxes) + 
                  // fSignalEventBoxes.size() * sizeof(fSignalEventBoxes.at(0)) +
                  sizeof(MeasurementVariableBox1D) * fillcount) * 1E-6;
    LOG(REC) << " -> Saved " << fillcount << " signal boxes for faster access. (~" << mem << " MB)" << std::endl;
  }


  LOG(REC) << "Time taken ReconfigureUsingManager() : " << time(NULL) - timestart << std::endl;
};


//***************************************************
void JointFCN::ReconfigureFastUsingManager() {
  //***************************************************

// ReconfigureFastEvtManager(){
// - Check for saved variables: Use normal if not.
// - Iterate over signalboolvector and look for true.
// - get tree entry and calculate new event weight.
// - Loop over measurements looking for ones that are actually signal.
// - MultiThread the FillHistogramsFromBox thing.
// - Create an array of what each box corresponds to.

// - If signal, get 'next' box and call FillHistogramsFromBox(weight)
// - Finish loop.
// - Call ScaleEvents, etc.

  int timestart = time(NULL);

  // Reset all samples
  MeasListConstIter iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);
    exp->ResetAll();
  }

  // Check for saved variables
  if (fSignalEventFlags.empty()) {
    ReconfigureUsingManager();
    return;
  }

  // Loop over all inputs
  int curindex = 0;

  // Get iterators
  std::vector<bool>::iterator inpsig_iter = fSignalEventFlags.begin();
  std::vector< std::vector<MeasurementVariableBox*> >::iterator box_iter = fSignalEventBoxes.begin();
  std::vector< std::vector<bool> >::iterator samsig_iter = fSampleSignalFlags.begin();

  // Setup stuff for logging
  int fillcount = 0;
  int nevents = fSignalEventFlags.size();
  int countwidth = nevents / 5;
 
  // Start input iterators
  std::vector<InputHandlerBase*>::iterator inp_iter = fInputList.begin();
  for (; inp_iter != fInputList.end(); inp_iter++) {
    InputHandlerBase* curinput = (*inp_iter);

    // Get Events
    FitEvent* curevent = curinput->FirstNuisanceEvent();
    int i = 0;
    while (curevent != 0) {

      // Logging
      if (LOG_LEVEL(REC)){
        if (i % countwidth == 0){
          LOG(REC) << "Processed " << i << " signal events." << std::endl;
        }
      }

      // If event is not signal skip it.
      if (!(*inpsig_iter)) {
        inpsig_iter++;
        i++;
        continue;
      }

      // Get Event
      curevent = curinput->GetNuisanceEvent(i);
      if (!curevent) break;

      // Get Event Weight
      curevent->RWWeight = FitBase::GetRW()->CalcWeight(curevent);
      curevent->Weight = curevent->RWWeight * curevent->InputWeight;
      double rwweight = curevent->Weight;

      // Iterate over the measurements and get the corresponding signal boxes.
      size_t measitercount = 0;
      size_t boxitercount = 0;

      // Get vectors for this event
      std::vector<MeasurementBase*>::iterator meas_iter = fSubSampleList.begin();
      std::vector<bool>::iterator subsamsig_iter = (*samsig_iter).begin();
      std::vector<MeasurementVariableBox*>::iterator subbox_iter = (*box_iter).begin();

      for (; meas_iter != fSubSampleList.end(); meas_iter++, subsamsig_iter++) {

        // Get Measurement
        MeasurementBase* curmeas = (*meas_iter);

        // If not signal continue
        if (*subsamsig_iter) {
          curmeas->FillHistogramsFromBox((*subbox_iter), rwweight);
          subbox_iter++;
          fillcount++;
        }
      }

      // Iterate over boxes
      samsig_iter++;
      box_iter++;

      // iterate to next signal event
      inpsig_iter++;
      i++;
    }
  }

  // Now loop over all Measurements
  // Convert Binned events
  iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);
    exp->ConvertEventRates();
  }

  LOG(REC) << "Filled " << fillcount << " signal events." << std::endl;
  LOG(REC) << "Time taken ReconfigureFastUsingManager() : " << time(NULL) - timestart << std::endl;
}

//***************************************************
void JointFCN::Write() {
  //***************************************************

  // Loop over individual experiments and call Write
  LOG(MIN) << "Writing each of the data classes..." << endl;
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;
    exp->Write();
  }

  // Save Pull Terms
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++) {
    ParamPull* pull = *iter;
    pull->Write();
  }

  if (FitPar::Config().GetParB("EventManager")) {
    // Get list of inputs
    std::map<int, InputHandlerBase*> fInputs = FitBase::EvtManager().GetInputs();
    std::map<int, InputHandlerBase*>::const_iterator iterInp;

    for (iterInp = fInputs.begin(); iterInp != fInputs.end(); iterInp++) {
      InputHandlerBase* input = (iterInp->second);

      input->GetFluxHistogram()->Write();
      input->GetXSecHistogram()->Write();
      input->GetEventHistogram()->Write();
    }
  }
};

//***************************************************
void JointFCN::SetFakeData(std::string fakeinput) {
  //***************************************************

  LOG(MIN) << "Setting fake data from " << fakeinput << endl;
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;
    exp->SetFakeDataValues(fakeinput);
  }

  return;
}
