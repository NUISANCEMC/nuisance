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

#include "MeasurementBase.h"

/*
  Constructor/Destructors
*/

//********************************************************************
// 2nd Level Constructor (Inherits From MeasurementBase.h)
MeasurementBase::MeasurementBase() {
//********************************************************************

  fScaleFactor = 1.0;
  fMCFilled = false;


};

//********************************************************************
// 2nd Level Destructor (Inherits From MeasurementBase.h)
MeasurementBase::~MeasurementBase() {
//********************************************************************

};

//********************************************************************
double MeasurementBase::TotalIntegratedFlux(std::string intOpt, double low, double high){
//********************************************************************

  // Set Energy Limits
  if (low == -9999.9)  low  = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;

  return GetInput()->TotalIntegratedFlux(low, high, intOpt);
};

//********************************************************************
double MeasurementBase::PredictedEventRate(std::string intOpt, double low, double high){
//********************************************************************

  // Set Energy Limits
  if (low == -9999.9)  low  = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;

  return GetInput()->PredictedEventRate(low, high, intOpt) * 1E-38;

};


//********************************************************************
void MeasurementBase::SetupInputs(std::string inputfile){
//********************************************************************

  // Add this infile to the global manager
  if (FitPar::Config().GetParB("EventManager")){
    FitBase::AddInput(fName, inputfile);

    // Get a pointer to the input so we can grab flux stuff
    // Slightly Convoluted...
    fInput  = FitBase::GetInput( FitBase::GetInputID(inputfile) );

  } else {
    fInput = new InputHandler(fName, inputfile);
  }

  fFluxHist      = fInput->GetFluxHistogram();
  fEventHist     = fInput->GetEventHistogram();
  fXSecHist      = fInput->GetXSecHistogram();
  fNEvents       = fInput->GetNEvents();
 
  inputfilename = inputfile;
}

//***********************************************
int MeasurementBase::GetInputID(){
//***********************************************
  return FitBase::GetInputID(inputfilename);
}

//***********************************************
void MeasurementBase::Reconfigure(){
//***********************************************
  LOG(REC) << " Reconfiguring sample "<<fName<<std::endl;

  bool using_evtmanager = FitPar::Config().GetParB("EventManager");
  int input_id = -1;
  if (using_evtmanager) input_id = FitBase::GetInputID(inputfilename);
  cust_event = fInput->GetEventPointer();

  if (FitPar::Config().GetParI("cachesize") > 0){
    fInput->SetupCache();
  }
  
  // Reset Histograms
  this->ResetAll();

  // READ in spline head for this input
  if (fInput->GetType() == kEVTSPLINE){
    FitBase::GetRW()->ReadSplineHead(fInput->GetSplineHead());
  }

  FitEvent* cust_event = fInput->GetEventPointer();
  int fNEvents = fInput->GetNEvents();
  int countwidth = (fNEvents/5);

  // Reset Signal Vectors
  fXVar_VECT.clear();
  fYVar_VECT.clear();
  fZVar_VECT.clear();
  this->fMode_VECT.clear();
  this->fIndex_VECT.clear();

  size_t NSignal = 0;
  // MAIN EVENT LOOP
  for (int i = 0; i < fNEvents; i++){

    // Read in the TChain and Calc Kinematics
    if (using_evtmanager){
      cust_event = FitBase::EvtManager().GetEvent(input_id, i);
    } else {
      fInput->ReadEvent(i);

      cust_event->RWWeight = FitBase::GetRW()->CalcWeight(cust_event);
      cust_event->Weight   = cust_event->RWWeight*cust_event->InputWeight;

      Weight = cust_event->Weight;
    }

    Weight = cust_event->Weight;

    // Initialize
    fXVar = 0.0;
    fYVar = 0.0;
    fZVar = 0.0;
    Signal = false;
    Mode = cust_event->Mode;

    // Extract Measurement Variables
    this->FillEventVariables(cust_event);
    Signal = this->isSignal(cust_event);

    // Push Back Signal
    if (Signal){
      fXVar_VECT .push_back(fXVar);
      fYVar_VECT .push_back(fYVar);
      fZVar_VECT .push_back(fZVar);
      this->fMode_VECT  .push_back(Mode);
      this->fIndex_VECT .push_back( (UInt_t)i);
      NSignal++;
    }

    // Fill Histogram Values
    this->FillHistograms();
    // this->FillExtraHistograms();

    // Print Out
    if (LOG_LEVEL(REC) && countwidth && !(i % countwidth))
      LOG(REC).unsetf(ios_base::fixed);
    std::cout << std::setw(7) << std::right << i << "/" << nevents
	      << " events (" << std::setw(2) << double(i)/double(nevents)*100.
	      << std::left << std::setw(5) << "%) "
	      << "[S,X,Y,Z,M,W] = ["
	      << std::fixed << std::setprecision(2) << std::right
	      << Signal << ", "
	      << std::setw(5) << fXVar  << ", " << std::setw(5) << fYVar <<  ", "
	      << std::setw(5) << fYVar  << ", " << std::setw(5) << Mode << ", "
	      << std::setw(5) << Weight << "] "<< std::endl;
    
  }

  int npassed = X_VAR_VECT.size();
  LOG(REC) << npassed << "/" << nevents << " passed selection " << std::endl;
  if (npassed == 0) {
    LOG(REC) << "WARNING: NO EVENTS PASSED SELECTION!" << std::endl;
  }

  // Finalise Histograms
  fMCFilled = true;
  this->ConvertEventRates();
}


//***********************************************
void MeasurementBase::ReconfigureFast(){
//***********************************************
  LOG(REC) << " Reconfiguring signal "<<this->fName<<std::endl;

  bool using_evtmanager = FitPar::Config().GetParB("EventManager");
  int input_id = -1;

  if (using_evtmanager){
    input_id = FitBase::GetInputID(inputfilename);
  } else {
    cust_event = fInput->GetEventPointer();
  }

  // Check if we Can't Signal Reconfigure
  if (!fMCFilled){
    this->Reconfigure();
    return;
  }

  // Reset Histograms
  this->ResetAll();

  // READ in spline head for this input
  if (fInput->GetType() == kEVTSPLINE){
    FitBase::GetRW()->ReadSplineHead(fInput->GetSplineHead());
  }

  // Get Pointer To Base Event (Just Generator Formats)
  int countwidth = (fIndex_VECT.size() / 5);

  // Setup Iterators
  std::vector<double>::iterator X = fXVar_VECT.begin();
  std::vector<double>::iterator Y = fYVar_VECT.begin();
  std::vector<double>::iterator Z = fZVar_VECT.begin();
  std::vector<int>::iterator    M = fMode_VECT.begin();
  std::vector<UInt_t>::iterator I = fIndex_VECT.begin();
  
  // SIGNAL LOOP
  for (int i = 0; I != fIndex_VECT.end(); I++, i++){

    // Just Update Weight
    if (using_evtmanager){
      Weight = FitBase::EvtManager().GetEventWeight(input_id, (*I));
    } else {
      fInput->GetTreeEntry((*I));
      Weight = FitBase::GetRW()->CalcWeight(cust_event)	* cust_event->InputWeight;
    }

    fXVar = (*X);
    fYVar = (*Y);
    fZVar = (*Z);
    Mode  = (*M);

    // Set signal to true because here every event looped is true signal
    Signal = true;

    // Sort Histograms
    this->FillHistograms();

    // Get Next Iteration
    X++;
    Y++;
    Z++;
    M++;

    // Print Out
    if (LOG_LEVEL(REC) && (i) % countwidth == 0)
      LOG(REC) << "Reconfigured " << i <<" signal events. [X,Y,Z,M,W] = ["
	       << fXVar  << ", " << fYVar << ", "
	       << fZVar  << ", " << Mode  << ", "
	       << Weight << "] " << std::endl;
  }

  // Finalise histograms
  fMCFilled = true;
  this->ConvertEventRates();
}

//***********************************************
void MeasurementBase::ConvertEventRates(){
//***********************************************

  this->ScaleEvents();
  this->ApplyNormScale( FitBase::GetRW()->GetSampleNorm( this->fName ) ) ;

}

//***********************************************
InputHandler* MeasurementBase::GetInput(){
//***********************************************

  if(FitPar::Config().GetParB("EventManager")){
    return FitBase::GetInput(FitBase::GetInputID(inputfilename));
  } else {
    return this->fInput;
  }
  return NULL;
};

//***********************************************
void MeasurementBase::Renormalise(){
//***********************************************

  // Called when the fitter has changed a measurements normalisation but not any reweight dials
  // Means we don't have to call the time consuming reconfigure when this happens.
  double norm = FitBase::GetRW()->GetDialValue( this->fName + "_norm" );

  if ((this->fCurrentNorm == 0.0 and norm != 0.0) or not fMCFilled){
    this->ReconfigureFast();
    return;
  }

  if (this->fCurrentNorm == norm) return;

  this->ApplyNormScale( 1.0 / this->fCurrentNorm );
  this->ApplyNormScale( norm );

  return;
};


//***********************************************
void MeasurementBase::SetSignal(bool sig){
//***********************************************
  Signal = sig;
}

//***********************************************
void MeasurementBase::SetSignal(FitEvent* evt){
//***********************************************
  Signal = this->isSignal(evt);
}

//***********************************************
void MeasurementBase::SetWeight(double wght){
//***********************************************
  Weight = wght;
}

//***********************************************
void MeasurementBase::SetMode(int md){
//***********************************************
  Mode = md;
}

//***********************************************  
std::vector<TH1*> MeasurementBase::GetFluxList(){
//***********************************************  
  return GetInput()->GetFluxList();
}

//***********************************************
std::vector<TH1*> MeasurementBase::GetEventRateList(){
//***********************************************
  return GetInput()->GetEventList();
}

//***********************************************
std::vector<TH1*> MeasurementBase::GetXSecList(){
//***********************************************
  return GetInput()->GetXSecList();
}
