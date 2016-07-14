#include "MeasurementBase.h"

/*
  Constructor/Destructors
*/

//********************************************************************
// 2nd Level Constructor (Inherits From MeasurementBase.h)
MeasurementBase::MeasurementBase() {
//********************************************************************

  scaleFactor = 1.0;
  filledMC = false;
  
};

//********************************************************************
// 2nd Level Destructor (Inherits From MeasurementBase.h)
MeasurementBase::~MeasurementBase() {
//********************************************************************
  
};


//********************************************************************
void MeasurementBase::SetFluxHistogram(std::string fluxFile, int minE, int maxE, double fluxNorm){
  //********************************************************************

  // Note this expects the flux bins to be given in terms of MeV
  LOG(SAM) << "Reading flux from file: " << fluxFile << std::endl;

  TGraph f(fluxFile.c_str(),"%lg %lg");

  this->fluxHist = new TH1D((this->measurementName+"_flux").c_str(), (this->measurementName+"; E_{#nu} (GeV)").c_str(),\
			    f.GetN()-1, minE, maxE);

  Double_t *yVal = f.GetY();

  for (int i = 0; i<fluxHist->GetNbinsX(); ++i)
    this->fluxHist->SetBinContent(i+1, yVal[i]*fluxNorm);

};

//********************************************************************
double MeasurementBase::TotalIntegratedFlux(std::string intOpt, double low, double high){
  //********************************************************************

  // Set Energy Limits
  if (low == -9999.9)  low  = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;

  int minBin = this->fluxHist->GetXaxis()->FindBin(low);
  int maxBin = this->fluxHist->GetXaxis()->FindBin(high);

  // Get integral over custom range
  double integral = this->fluxHist->Integral(minBin, maxBin+1, intOpt.c_str());

  return integral;
};

//********************************************************************
double MeasurementBase::PredictedEventRate(std::string intOpt, double low, double high){
//********************************************************************

  // Set Energy Limits
  if (low == -9999.9)  low  = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;

  int minBin = this->eventHist->GetXaxis()->FindBin(low);
  int maxBin = this->eventHist->GetXaxis()->FindBin(high);

  // Get integral over custom range
  double integral = this->eventHist->Integral(minBin, maxBin+1, intOpt.c_str());

  return integral * 1E-38;
};


//********************************************************************   
void MeasurementBase::SetupInputs(std::string inputfile){
//********************************************************************

  // Add this infile to the global manager
  FitBase::AddInput(measurementName, inputfile);

  // Get a pointer to the input so we can grab flux stuff
  // Slightly Convoluted...
  InputHandler* input = FitBase::GetInput( FitBase::GetInputID(inputfile) );

  this->fluxHist      = input->GetFluxHistogram();
  this->eventHist     = input->GetEventHistogram();
  this->xsecHist      = input->GetXSecHistogram();
  this->nevents       = input->GetNEvents();

  std::cout<<"Got input"<<std::endl;
  std::cout<<"nevents = "<<nevents<<std::endl;
  
  inputfilename = inputfile;
  
}

//*********************************************** 
void MeasurementBase::Reconfigure(){
//*********************************************** 
  LOG(REC) << " Reconfiguring sample "<<this->measurementName<<std::endl;
  int input_id = FitBase::GetInputID(inputfilename);
  InputHandler* input = FitBase::GetInput(input_id);
  
  // Reset Histograms
  this->ResetAll();

  // READ in spline head for this input
  if (input->GetType() == kEVTSPLINE){
    FitBase::GetRW()->ReadSplineHead(input->GetSplineHead());
  }
  
  FitEvent* cust_event = input->GetEventPointer();
  int nevents = input->GetNEvents();
  int countwidth = (nevents/20);

  // Reset Signal Vectors
  this->X_VAR_VECT.clear();
  this->Y_VAR_VECT.clear();
  this->Z_VAR_VECT.clear();
  this->MODE_VECT.clear();
  this->INDEX_VECT.clear();

  // MAIN EVENT LOOP
  for (int i = 0; i < nevents; i++){

    // Read in the TChain and Calc Kinematics
    cust_event = FitBase::EvtManager().GetEvent(input_id, i);
    Weight = cust_event->Weight;

    std::cout<<"Custom Event Mode = "<<cust_event->Mode<<std::endl;
    std::cout<<"Custom Type "<<cust_event->fType<<std::endl;

    std::cout<<"NPART = "<<cust_event->Npart()<<std::endl;
    
    // Initialize
    X_VAR = 0.0;
    Y_VAR = 0.0;
    Z_VAR = 0.0;
    Signal = false;
    Mode = cust_event->Mode;

    // Extract Measurement Variables
    this->FillEventVariables(cust_event);
    Signal = this->isSignal(cust_event);

    // Push Back Signal
    if (Signal){
      this->X_VAR_VECT .push_back(X_VAR);
      this->Y_VAR_VECT .push_back(Y_VAR);
      this->Z_VAR_VECT .push_back(Z_VAR);
      this->MODE_VECT  .push_back(Mode);
      this->INDEX_VECT .push_back( (UInt_t)i);
    }

    // Fill Histogram Values
    this->FillHistograms();
    //    this->FillExtraHistograms();
    
    // Print Out
    if (LOG_LEVEL(REC) and  i % countwidth == 0)
      LOG(REC) << "Reconfigured " << i <<" total events. [S,X,Y,Z,M,W] = ["
	       << Signal << ", " 
	       << X_VAR  << ", "<< Y_VAR <<  ", "
	       << Z_VAR  << ", "<< Mode << ", " 
	       << Weight << "] "<< std::endl;
    
    
  }

  FitBase::EvtManager().SetRWFlag(input_id, false);
  
  // Finalise Histograms
  filledMC = true;
  this->ConvertEventRates();
}


//*********************************************** 
void MeasurementBase::ReconfigureFast(){
//***********************************************

  int input_id = FitBase::GetInputID(inputfilename);
  InputHandler* input = FitBase::GetInput(input_id);
  
  // Check if we Can't Signal Reconfigure
  if (!filledMC){
    this->Reconfigure();
    return;
  }

  // Reset Histograms
  this->ResetAll();

  // READ in spline head for this input
  if (input->GetType() == kEVTSPLINE){
    FitBase::GetRW()->ReadSplineHead(input->GetSplineHead());
  }

  // Get Pointer To Base Event (Just Generator Formats)
  int countwidth = (nevents / 20);

  // Setup Iterators
  std::vector<double>::iterator X = X_VAR_VECT.begin();
  std::vector<double>::iterator Y = Y_VAR_VECT.begin();
  std::vector<double>::iterator Z = Z_VAR_VECT.begin();
  std::vector<int>::iterator    M = MODE_VECT.begin();
  std::vector<UInt_t>::iterator I = INDEX_VECT.begin();
  
  // SIGNAL LOOP
  for (int i = 0; I != INDEX_VECT.end(); I++, i++){

    // Just Update Weight
    Weight = FitBase::EvtManager().GetEventWeight(input_id, (*I));
      
    X_VAR = (*X);
    Y_VAR = (*Y);
    Z_VAR = (*Z);
    Mode  = (*M);
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
	       << X_VAR  << ", " << Y_VAR << ", "
	       << Z_VAR  << ", " << Mode  << ", "
	       << Weight << "] " << std::endl;    
  }

  FitBase::EvtManager().SetRWFlag(input_id, false);
  
  // Finalise histograms
  filledMC = true;
  this->ConvertEventRates();
}

//*********************************************** 
void MeasurementBase::ConvertEventRates(){
//***********************************************

  this->ScaleEvents();
  this->ApplyNormScale( FitBase::GetRW()->GetSampleNorm( this->measurementName ) ) ;

}

//***********************************************
void MeasurementBase::Renormalise(){
//***********************************************

  // Called when the fitter has changed a measurements normalisation but not any reweight dials
  // Means we don't have to call the time consuming reconfigure when this happens.
  double new_norm = FitBase::GetRW()->GetDialValue( this->measurementName + "_norm" );
  double norm = 0.0;
  if (new_norm >= 0.0) norm = 1.0/new_norm;
  
  if ((this->currentNorm == 0.0 and norm != 0.0) or not filledMC){
    this->ReconfigureFast();
    return;
  }
  
  if (this->currentNorm == norm) return;  

  this->ApplyNormScale( 1.0 / this->currentNorm );
  this->ApplyNormScale( norm );

  return;
};

