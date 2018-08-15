#include "JointFCN.h"
#include <stdio.h>
#include "FitUtils.h"

//***************************************************
JointFCN::JointFCN(TFile* outfile) {
  //***************************************************

  fOutputDir = gDirectory;
  if (outfile) Config::Get().out = outfile;

  std::vector<nuiskey> samplekeys = Config::QueryKeys("sample");
  LoadSamples(samplekeys);

  std::vector<nuiskey> covarkeys = Config::QueryKeys("covar");
  LoadPulls(covarkeys);

  fCurIter = 0;
  fMCFilled = false;

  fIterationTree = false;
  fDialVals = NULL;
  fNDials = 0;

  fUsingEventManager = FitPar::Config().GetParB("EventManager");
  fOutputDir->cd();
}

//***************************************************
JointFCN::JointFCN(std::vector<nuiskey> samplekeys, TFile* outfile) {
  //***************************************************

  fOutputDir = gDirectory;
  if (outfile) Config::Get().out = outfile;

  LoadSamples(samplekeys);

  fCurIter = 0;
  fMCFilled = false;

  fOutputDir->cd();

  fIterationTree = false;
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

  LOG(FIT) << " Creating new iteration container! " << std::endl;
  DestroyIterationTree();
  fIterationTreeName = name;

  // Add sample likelihoods and ndof
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;
    std::string name = exp->GetName();

    std::string liketag = name + "_likelihood";
    fNameValues.push_back(liketag);
    fCurrentValues.push_back(0.0);

    std::string ndoftag = name + "_ndof";
    fNameValues.push_back(ndoftag);
    fCurrentValues.push_back(0.0);
  }

  // Add Pull terms
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++) {
    ParamPull* pull = *iter;
    std::string name = pull->GetName();

    std::string liketag = name + "_likelihood";
    fNameValues.push_back(liketag);
    fCurrentValues.push_back(0.0);

    std::string ndoftag = name + "_ndof";
    fNameValues.push_back(ndoftag);
    fCurrentValues.push_back(0.0);
  }

  // Add Likelihoods
  fNameValues.push_back("total_likelihood");
  fCurrentValues.push_back(0.0);

  fNameValues.push_back("total_ndof");
  fCurrentValues.push_back(0.0);

  // Setup Containers
  fSampleN = fSamples.size() + fPulls.size();
  fSampleLikes = new double[fSampleN];
  fSampleNDOF = new int[fSampleN];

  // Add Dials
  std::vector<std::string> dials = rw->GetDialNames();
  for (size_t i = 0; i < dials.size(); i++) {
    fNameValues.push_back(dials[i]);
    fCurrentValues.push_back(0.0);
  }
  fNDials = dials.size();
  fDialVals = new double[fNDials];

  // Set IterationTree Flag
  fIterationTree = true;
}

//***************************************************
void JointFCN::DestroyIterationTree() {
  //***************************************************

  fIterationCount.clear();
  fCurrentValues.clear();
  fNameValues.clear();
  fIterationValues.clear();
}

//***************************************************
void JointFCN::WriteIterationTree() {
  //***************************************************
  LOG(FIT) << "Writing iteration tree" << std::endl;

  // Make a new TTree
  TTree* itree =
      new TTree(fIterationTreeName.c_str(), fIterationTreeName.c_str());

  double* vals = new double[fNameValues.size()];
  int count = 0;

  itree->Branch("iteration", &count, "Iteration/I");
  for (int i = 0; i < fNameValues.size(); i++) {
    itree->Branch(fNameValues[i].c_str(), &vals[i],
                  (fNameValues[i] + "/D").c_str());
  }

  // Fill Iterations
  for (size_t i = 0; i < fIterationValues.size(); i++) {
    std::vector<double> itervals = fIterationValues[i];

    // Fill iteration state
    count = fIterationCount[i];
    for (size_t j = 0; j < itervals.size(); j++) {
      vals[j] = itervals[j];
    }

    // Save to TTree
    itree->Fill();
  }

  // Write to file
  itree->Write();
}

//***************************************************
void JointFCN::FillIterationTree(FitWeight* rw) {
  //***************************************************

  // Loop over samples count
  int count = 0;
  for (int i = 0; i < fSampleN; i++) {
    fCurrentValues[count++] = fSampleLikes[i];
    fCurrentValues[count++] = double(fSampleNDOF[i]);
  }

  // Fill Totals
  fCurrentValues[count++] = fLikelihood;
  fCurrentValues[count++] = double(fNDOF);

  // Loop Over Parameter Counts
  rw->GetAllDials(fDialVals, fNDials);
  for (int i = 0; i < fNDials; i++) {
    fCurrentValues[count++] = double(fDialVals[i]);
  }

  // Push Back Into Container
  fIterationCount.push_back(fCurIter);
  fIterationValues.push_back(fCurrentValues);
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
  if (LOG_LEVEL(REC)) {
    FitBase::GetRW()->Print();
  }

  // SORT SAMPLES
  ReconfigureSamples();

  // GET TEST STAT
  fLikelihood = GetLikelihood();
  fNDOF = GetNDOF();

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
  if (fIterationTree) {
    fSampleNDOF[count] = totaldof;
  }
  return totaldof;
}

//***************************************************
double JointFCN::GetLikelihood() {
  //***************************************************

  LOG(MIN) << std::left << std::setw(43) << "Getting likelihoods..."
           << " : "
           << "-2logL" << std::endl;

  // Loop and add up likelihoods in an uncorrelated way
  double like = 0.0;
  int count = 0;
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;
    double newlike = exp->GetLikelihood();
    int ndof = exp->GetNDOF();
    // Save seperate likelihoods
    if (fIterationTree) {
      fSampleLikes[count] = newlike;
    }

    LOG(MIN) << "-> " << std::left << std::setw(40) << exp->GetName() << " : "
             << newlike << "/" << ndof << std::endl;

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
  if (fIterationTree) {
    fSampleLikes[count] = fLikelihood;
  }

  return like;
};

void JointFCN::LoadSamples(std::vector<nuiskey> samplekeys) {
  LOG(MIN) << "Loading Samples : " << samplekeys.size() << std::endl;
  for (size_t i = 0; i < samplekeys.size(); i++) {
    nuiskey key = samplekeys[i];

    // Get Sample Options
    std::string samplename = key.GetS("name");
    std::string samplefile = key.GetS("input");
    std::string sampletype = key.GetS("type");
    std::string fakeData = "";

    LOG(MIN) << "Loading Sample : " << samplename << std::endl;

    fOutputDir->cd();
    MeasurementBase* NewLoadedSample = SampleUtils::CreateSample(key);

    if (!NewLoadedSample) {
      ERR(FTL) << "Could not load sample provided: " << samplename << std::endl;
      ERR(FTL) << "Check spelling with that in src/FCN/SampleList.cxx"
               << std::endl;
      throw;
    } else {
      fSamples.push_back(NewLoadedSample);
    }
  }
}

//***************************************************
void JointFCN::LoadPulls(std::vector<nuiskey> pullkeys) {
  //***************************************************
  for (size_t i = 0; i < pullkeys.size(); i++) {
    nuiskey key = pullkeys[i];

    std::string pullname = key.GetS("name");
    std::string pullfile = key.GetS("input");
    std::string pulltype = key.GetS("type");

    fOutputDir->cd();
    fPulls.push_back(new ParamPull(pullname, pullfile, pulltype));
  }
}

//***************************************************
void JointFCN::ReconfigureSamples(bool fullconfig) {
  //***************************************************

  int starttime = time(NULL);
  LOG(REC) << "Starting Reconfigure iter. " << this->fCurIter << std::endl;
  // std::cout << fUsingEventManager << " " << fullconfig << " " << fMCFilled <<
  // std::endl;
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
           << time(NULL) - starttime << "s" << std::endl;

  fCurIter++;
}

//***************************************************
void JointFCN::ReconfigureSignal() {
  //***************************************************
  ReconfigureSamples(false);
}

//***************************************************
void JointFCN::ReconfigureAllEvents() {
  //***************************************************
  FitBase::GetRW()->Reconfigure();
  FitBase::EvtManager().ResetWeightFlags();
  ReconfigureSamples(true);
}

std::vector<InputHandlerBase*> JointFCN::GetInputList() {
  std::vector<InputHandlerBase*> InputList;
  fIsAllSplines = true;

  MeasListConstIter iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);

    std::vector<MeasurementBase*> subsamples = exp->GetSubSamples();
    for (size_t i = 0; i < subsamples.size(); i++) {
      InputHandlerBase* inp = subsamples[i]->GetInput();
      if (std::find(InputList.begin(), InputList.end(), inp) ==
          InputList.end()) {
        if (subsamples[i]->GetInput()->GetType() != kSPLINEPARAMETER)
          fIsAllSplines = false;

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

  // 'Slow' Event Manager Reconfigure
  LOG(REC) << "Event Manager Reconfigure" << std::endl;
  int timestart = time(NULL);

  // Reset all samples
  MeasListConstIter iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);
    exp->ResetAll();
  }

  // If we are siving signal, reset all containers.
  bool savesignal = (FitPar::Config().GetParB("SignalReconfigures"));

  if (savesignal) {
    // Reset all of our event signal vectors
    fSignalEventBoxes.clear();
    fSignalEventFlags.clear();
    fSampleSignalFlags.clear();
    fSignalEventSplines.clear();
  }

  // Make sure we have a list of inputs
  if (fInputList.empty()) {
    fInputList = GetInputList();
    fSubSampleList = GetSubSampleList();
  }

  // If all inputs are splines make sure the readers are told
  // they need to be reconfigured.
  std::vector<InputHandlerBase*>::iterator inp_iter = fInputList.begin();

  if (fIsAllSplines) {
    for (; inp_iter != fInputList.end(); inp_iter++) {
      InputHandlerBase* curinput = (*inp_iter);

      // Tell reader in each BaseEvent it needs a Reconfigure next weight calc.
      BaseFitEvt* curevent = curinput->FirstBaseEvent();
      if (curevent->fSplineRead) {
        curevent->fSplineRead->SetNeedsReconfigure(true);
      }
    }
  }

  // MAIN INPUT LOOP ====================

  int fillcount = 0;
  int inputcount = 0;
  inp_iter = fInputList.begin();

  // Loop over each input in manager
  for (; inp_iter != fInputList.end(); inp_iter++) {
    InputHandlerBase* curinput = (*inp_iter);

    // Get event information
    FitEvent* curevent = curinput->FirstNuisanceEvent();
    curinput->CreateCache();

    int i = 0;
    int nevents = curinput->GetNEvents();
    int countwidth = nevents / 5;

    // Start event loop iterating until we get a NULL pointer.
    while (curevent) {
      // Get Event Weight
      curevent->RWWeight = FitBase::GetRW()->CalcWeight(curevent);
      curevent->Weight = curevent->RWWeight * curevent->InputWeight;
      double rwweight = curevent->Weight;
      // std::cout << "RWWeight = " << curevent->RWWeight  << " " <<
      // curevent->InputWeight << std::endl;

      // Logging
      // std::cout << CHECKLOG(1) << std::endl;
      if (LOGGING(REC)) {
        if (countwidth && (i % countwidth == 0)) {
          QLOG(REC, curinput->GetName()
                        << " : Processed " << i << " events. [M, W] = ["
                        << curevent->Mode << ", " << rwweight << "]");
        }
      }

      // Setup flag for if signal found in at least one sample
      bool foundsignal = false;

      // Create a new signal bitset for this event
      std::vector<bool> signalbitset(fSubSampleList.size());

      // Create a new signal box vector for this event
      std::vector<MeasurementVariableBox*> signalboxes;

      // Start measurement iterator
      size_t measitercount = 0;
      std::vector<MeasurementBase*>::iterator meas_iter =
          fSubSampleList.begin();

      // Loop over all subsamples (sub in JointMeas)
      for (; meas_iter != fSubSampleList.end(); meas_iter++) {
        MeasurementBase* curmeas = (*meas_iter);

        // Compare input pointers, to current input, skip if not.
        // Pointer tells us if it matches without doing ID checks.
        if (curinput != curmeas->GetInput()) {
          if (savesignal) {
            // Set bit to 0 as definitely not signal
            signalbitset[measitercount] = 0;
          }

          // Count up what measurement we are on.
          measitercount++;

          // Skip sample as input not signal.
          continue;
        }

        // Fill events for matching inputs.
        MeasurementVariableBox* box = curmeas->FillVariableBox(curevent);

        bool signal = curmeas->isSignal(curevent);
        curmeas->SetSignal(signal);
        curmeas->FillHistograms(curevent->Weight);

        // If its Signal tally up fills
        if (signal) {
          fillcount++;
        }

        // If we are saving signal/splines fill the bitset
        if (savesignal) {
          signalbitset[measitercount] = signal;
        }

        // If signal save a clone of the event box for use later.
        if (savesignal and signal) {
          foundsignal = true;
          signalboxes.push_back(box->CloneSignalBox());
        }

        // Keep track of Measurement we are on.
        measitercount++;
      }

      // Once we've filled the measurements, if saving signal
      // push back if any sample flagged this event as signal
      if (savesignal) {
        fSignalEventFlags.push_back(foundsignal);
      }

      // Save the vector of signal boxes for this event
      if (savesignal and foundsignal) {
        fSignalEventBoxes.push_back(signalboxes);
        fSampleSignalFlags.push_back(signalbitset);
      }

      // If all inputs are splines we can save the spline coefficients
      // for fast in memory reconfigures later.
      if (fIsAllSplines and savesignal and foundsignal) {
        // Make temp vector to push back with
        std::vector<float> coeff;
        for (size_t l = 0; l < (UInt_t)curevent->fSplineRead->GetNPar(); l++) {
          coeff.push_back(curevent->fSplineCoeff[l]);
        }

        // Push back to signal event splines. Kept in sync with
        // fSignalEventBoxes size.
        // int splinecount = fSignalEventSplines.size();
        fSignalEventSplines.push_back(coeff);

        // if (splinecount % 1000 == 0) {
        // std::cout << "Pushed Back Coeff " << splinecount << " : ";
        // for (size_t l = 0; l < fSignalEventSplines[splinecount].size(); l++)
        // {
        // std::cout << " " << fSignalEventSplines[splinecount][l];
        // }
        // std::cout << std::endl;
        // }
      }

      // Clean up vectors once done with this event
      signalboxes.clear();
      signalbitset.clear();

      // Iterate to the next event.
      curevent = curinput->NextNuisanceEvent();
      i++;
    }

    //    curinput->RemoveCache();

    // Keep track of what input we are on.
    inputcount++;
  }

  // End of Event Loop ===============================

  // Now event loop is finished loop over all Measurements
  // Converting Binned events to XSec Distributions
  iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);
    exp->ConvertEventRates();
  }

  // Print out statements on approximate memory usage for profiling.
  LOG(REC) << "Filled " << fillcount << " signal events." << std::endl;
  if (savesignal) {
    int mem =
        (  // sizeof(fSignalEventBoxes) +
            // fSignalEventBoxes.size() * sizeof(fSignalEventBoxes.at(0)) +
            sizeof(MeasurementVariableBox1D) * fillcount) *
        1E-6;
    LOG(REC) << " -> Saved " << fillcount
             << " signal boxes for faster access. (~" << mem << " MB)"
             << std::endl;
    if (fIsAllSplines and !fSignalEventSplines.empty()) {
      int splmem = sizeof(float) * fSignalEventSplines.size() *
                   fSignalEventSplines[0].size() * 1E-6;
      LOG(REC) << " -> Saved " << fillcount << " " << fSignalEventSplines.size()
               << " spline sets into memory. (~" << splmem << " MB)"
               << std::endl;
    }
  }

  LOG(REC) << "Time taken ReconfigureUsingManager() : "
           << time(NULL) - timestart << std::endl;

  // Check SignalReconfigures works for all samples
  if (savesignal) {
    double likefull = GetLikelihood();
    ReconfigureFastUsingManager();
    double likefast = GetLikelihood();

    if (fabs(likefull - likefast) > 0.0001) {
      ERROR(FTL, "Fast and Full Likelihoods DIFFER! : " << likefull << " : "
                                                        << likefast);
      ERROR(FTL,
            "This means some samples you are using are not setup to use "
            "SignalReconfigures=1");
      ERROR(FTL, "Please turn OFF signal reconfigures.");
      throw;
    } else {
      LOG(FIT)
          << "Likelihoods for FULL and FAST match. Will use FAST next time."
          << std::endl;
    }
  }

  // End of reconfigure
  return;
};

//***************************************************
void JointFCN::ReconfigureFastUsingManager() {
  //***************************************************

  LOG(FIT) << " -> Doing FAST using manager" << std::endl;
  // Get Start time for profilling
  int timestart = time(NULL);

  // Reset all samples
  MeasListConstIter iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);
    exp->ResetAll();
  }

  // Check for saved variables if not do a full reconfigure.
  if (fSignalEventFlags.empty()) {
    ERR(WRN) << "Signal Flags Empty! Using normal manager." << std::endl;
    ReconfigureUsingManager();
    return;
  }

  bool fFillNuisanceEvent =
      FitPar::Config().GetParB("FullEventOnSignalReconfigure");

  // Setup fast vector iterators.
  std::vector<bool>::iterator inpsig_iter = fSignalEventFlags.begin();
  std::vector<std::vector<MeasurementVariableBox*> >::iterator box_iter =
      fSignalEventBoxes.begin();
  std::vector<std::vector<float> >::iterator spline_iter =
      fSignalEventSplines.begin();
  std::vector<std::vector<bool> >::iterator samsig_iter =
      fSampleSignalFlags.begin();
  int splinecount = 0;

  // Setup stuff for logging
  int fillcount = 0;
  int nevents = fSignalEventFlags.size();
  int countwidth = nevents / 20;

  // If All Splines tell splines they need a reconfigure.
  std::vector<InputHandlerBase*>::iterator inp_iter = fInputList.begin();
  if (fIsAllSplines) {
    LOG(REC) << "All Spline Inputs so using fast spline loop." << std::endl;
    for (; inp_iter != fInputList.end(); inp_iter++) {
      InputHandlerBase* curinput = (*inp_iter);

      // Tell each fSplineRead in BaseFitEvent to reconf next weight calc
      BaseFitEvt* curevent = curinput->FirstBaseEvent();
      if (curevent->fSplineRead)
        curevent->fSplineRead->SetNeedsReconfigure(true);
    }
  }

  // Loop over all possible spline inputs
  double* coreeventweights = new double[fSignalEventBoxes.size()];
  splinecount = 0;

  inp_iter = fInputList.begin();
  inpsig_iter = fSignalEventFlags.begin();
  spline_iter = fSignalEventSplines.begin();

  // Loop over all signal flags
  // For each valid signal flag add one to splinecount
  // Get Splines from that count and add to weight
  // Add splinecount
  int sigcount = 0;
  splinecount = 0;

  // #pragma omp parallel for shared(splinecount,sigcount)
  for (uint iinput = 0; iinput < fInputList.size(); iinput++) {
    InputHandlerBase* curinput = fInputList[iinput];
    BaseFitEvt* curevent = curinput->FirstBaseEvent();

    for (int i = 0; i < curinput->GetNEvents(); i++) {
      double rwweight = 0.0;
      if (fSignalEventFlags[sigcount]) {
        // Get Event Info
        if (!fIsAllSplines) {
          if (fFillNuisanceEvent)
            curevent = curinput->GetNuisanceEvent(i);
          else
            curevent = curinput->GetBaseEvent(i);
        } else {
          curevent->fSplineCoeff = &fSignalEventSplines[splinecount][0];
        }

        curevent->RWWeight = FitBase::GetRW()->CalcWeight(curevent);
        curevent->Weight = curevent->RWWeight * curevent->InputWeight;
        rwweight = curevent->Weight;

        coreeventweights[splinecount] = rwweight;
        if (countwidth && ((splinecount % countwidth) == 0)) {
          LOG(REC) << "Processed " << splinecount
                   << " event weights. W = " << rwweight << std::endl;
        }

        // #pragma omp atomic
        splinecount++;
      }

      // #pragma omp atomic
      sigcount++;
    }
  }
  LOG(SAM) << "Processed event weights." << std::endl;

  // #pragma omp barrier

  // Reset Iterators
  inpsig_iter = fSignalEventFlags.begin();
  spline_iter = fSignalEventSplines.begin();
  box_iter = fSignalEventBoxes.begin();
  samsig_iter = fSampleSignalFlags.begin();
  int nsplineweights = splinecount;
  splinecount = 0;

  // Start of Fast Event Loop ============================

  // Start input iterators
  // Loop over number of inputs
  for (int ispline = 0; ispline < nsplineweights; ispline++) {
    double rwweight = coreeventweights[ispline];

    // Get iterators for this event
    std::vector<bool>::iterator subsamsig_iter = (*samsig_iter).begin();
    std::vector<MeasurementVariableBox*>::iterator subbox_iter =
        (*box_iter).begin();

    // Loop over all sub measurements.
    std::vector<MeasurementBase*>::iterator meas_iter = fSubSampleList.begin();
    for (; meas_iter != fSubSampleList.end(); meas_iter++, subsamsig_iter++) {
      MeasurementBase* curmeas = (*meas_iter);

      // If event flagged as signal for this sample fill from the box.
      if (*subsamsig_iter) {
        curmeas->SetSignal(true);
        curmeas->FillHistogramsFromBox((*subbox_iter), rwweight);

        // Move onto next box if there is one.
        subbox_iter++;
        fillcount++;
      }
    }

    if (ispline % countwidth == 0) {
      LOG(REC) << "Filled " << ispline << " sample weights." << std::endl;
    }

    // Iterate over the main signal event containers.
    samsig_iter++;
    box_iter++;
    spline_iter++;
    splinecount++;
  }
  // End of Fast Event Loop ===================

  LOG(SAM) << "Filled sample distributions." << std::endl;

  // Now loop over all Measurements
  // Convert Binned events
  iterSam = fSamples.begin();
  for (; iterSam != fSamples.end(); iterSam++) {
    MeasurementBase* exp = (*iterSam);
    exp->ConvertEventRates();
  }

  // Cleanup coreeventweights
  if (fIsAllSplines) {
    delete coreeventweights;
  }

  // Print some reconfigure profiling.
  LOG(REC) << "Filled " << fillcount << " signal events." << std::endl;
  LOG(REC) << "Time taken ReconfigureFastUsingManager() : "
           << time(NULL) - timestart << std::endl;
}

//***************************************************
void JointFCN::Write() {
  //***************************************************

  // Save a likelihood/ndof plot
  LOG(MIN) << "Writing likelihood plot.." << std::endl;
  std::vector<double> likes;
  std::vector<double> ndofs;
  std::vector<std::string> names;
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;
    double like = exp->GetLikelihood();
    double ndof = exp->GetNDOF();
    std::string name = exp->GetName();
    likes.push_back(like);
    ndofs.push_back(ndof);
    names.push_back(name);
  }
  TH1D likehist = TH1D("likelihood_hist", "likelihood_hist", likes.size(), 0.0,
                       double(likes.size()));
  TH1D ndofhist =
      TH1D("ndof_hist", "ndof_hist", ndofs.size(), 0.0, double(ndofs.size()));
  TH1D divhist = TH1D("likedivndof_hist", "likedivndof_hist", likes.size(), 0.0,
                      double(likes.size()));
  for (size_t i = 0; i < likehist.GetNbinsX(); i++) {
    likehist.SetBinContent(i + 1, likes[i]);
    ndofhist.SetBinContent(i + 1, ndofs[i]);
    if (ndofs[i] != 0.0) {
      divhist.SetBinContent(i + 1, likes[i] / ndofs[i]);
    }
    likehist.GetXaxis()->SetBinLabel(i + 1, names[i].c_str());
    ndofhist.GetXaxis()->SetBinLabel(i + 1, names[i].c_str());
    divhist.GetXaxis()->SetBinLabel(i + 1, names[i].c_str());
  }
  likehist.Write();
  ndofhist.Write();
  divhist.Write();

  // Loop over individual experiments and call Write
  LOG(MIN) << "Writing each of the data classes..." << std::endl;
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
    std::map<int, InputHandlerBase*> fInputs =
        FitBase::EvtManager().GetInputs();
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

  LOG(MIN) << "Setting fake data from " << fakeinput << std::endl;
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;
    exp->SetFakeDataValues(fakeinput);
  }

  return;
}

//***************************************************
void JointFCN::ThrowDataToy() {
  //***************************************************

  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;
    exp->ThrowDataToy();
  }

  return;
}

//***************************************************
std::vector<std::string> JointFCN::GetAllNames() {
  //***************************************************

  // Vect of all likelihoods and total
  std::vector<std::string> namevect;

  // Loop over samples first
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;

    // Get Likelihoods and push to vector
    namevect.push_back(exp->GetName());
  }

  // Loop over pulls second
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++) {
    ParamPull* pull = *iter;

    // Push back to vector
    namevect.push_back(pull->GetName());
  }

  // Finally add the total
  namevect.push_back("total");

  return namevect;
}

//***************************************************
std::vector<double> JointFCN::GetAllLikelihoods() {
  //***************************************************

  // Vect of all likelihoods and total
  std::vector<double> likevect;
  double total_likelihood = 0.0;
  LOG(MIN) << "Likelihoods : " << std::endl;

  // Loop over samples first
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;

    // Get Likelihoods and push to vector
    double singlelike = exp->GetLikelihood();
    likevect.push_back(singlelike);
    total_likelihood += singlelike;

    // Print Out
    LOG(MIN) << "-> " << std::left << std::setw(40) << exp->GetName() << " : "
             << singlelike << std::endl;
  }

  // Loop over pulls second
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++) {
    ParamPull* pull = *iter;

    // Push back to vector
    double singlelike = pull->GetLikelihood();
    likevect.push_back(singlelike);
    total_likelihood += singlelike;

    // Print Out
    LOG(MIN) << "-> " << std::left << std::setw(40) << pull->GetName() << " : "
             << singlelike << std::endl;
  }

  // Finally add the total likelihood
  likevect.push_back(total_likelihood);

  return likevect;
}

//***************************************************
std::vector<int> JointFCN::GetAllNDOF() {
  //***************************************************

  // Vect of all ndof and total
  std::vector<int> ndofvect;
  int total_ndof = 0;

  // Loop over samples first
  for (MeasListConstIter iter = fSamples.begin(); iter != fSamples.end();
       iter++) {
    MeasurementBase* exp = *iter;

    // Get Likelihoods and push to vector
    int singlendof = exp->GetNDOF();
    ndofvect.push_back(singlendof);
    total_ndof += singlendof;
  }

  // Loop over pulls second
  for (PullListConstIter iter = fPulls.begin(); iter != fPulls.end(); iter++) {
    ParamPull* pull = *iter;

    // Push back to vector
    int singlendof = pull->GetNDOF();
    ndofvect.push_back(singlendof);
    total_ndof += singlendof;
  }

  // Finally add the total ndof
  ndofvect.push_back(total_ndof);

  return ndofvect;
}
