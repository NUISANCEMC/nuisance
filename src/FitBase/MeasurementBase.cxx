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
MeasurementBase::MeasurementBase(void) {
  //********************************************************************

  fScaleFactor = 1.0;
  fMCFilled = false;
  fNoData = false;
  fInput = NULL;

  // Set the default values
  // After-wards this gets set in SetupMeasurement
  EnuMin = 0.;
  EnuMax = 1.E5;

  std::cout << "Calling MeasurememntBase Constructor!" << std::endl;
  fMeasurementSpeciesType = kSingleSpeciesMeasurement;

};

void MeasurementBase::FinaliseMeasurement(){

  // Used to setup default data hists, covars, etc.



}

//********************************************************************
// 2nd Level Destructor (Inherits From MeasurementBase.h)
MeasurementBase::~MeasurementBase(){
    //********************************************************************

};

//********************************************************************
double MeasurementBase::TotalIntegratedFlux(std::string intOpt, double low,
                                            double high) {
  //********************************************************************

  // Set Energy Limits
  if (low == -9999.9) low = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;
  return GetInput()->TotalIntegratedFlux(low,high, intOpt);
};

//********************************************************************
double MeasurementBase::PredictedEventRate(std::string intOpt, double low,
                                           double high) {
  //********************************************************************

  // Set Energy Limits
  if (low == -9999.9) low = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;

  return GetInput()->PredictedEventRate(low, high, intOpt) * 1E-38;
};

//********************************************************************
void MeasurementBase::SetupInputs(std::string inputfile) {
  //********************************************************************

  
  // Add this infile to the global manager
  if (FitPar::Config().GetParB("EventManager")) {
    //fInput = FitBase::AddInput(fName, inputfile);
  } else {
    std::vector<std::string> file_descriptor =
        GeneralUtils::ParseToStr(inputfile, ":");
    if (file_descriptor.size() != 2) {
      ERR(FTL) << "File descriptor had no filetype declaration: \"" << inputfile
               << "\". expected \"FILETYPE:file.root\"" << std::endl;
      throw;
    }
    InputUtils::InputType inpType =
        InputUtils::ParseInputType(file_descriptor[0]);

//    fInput = new InputHandler(fName, inpType, file_descriptor[1]);
    fInput = InputUtils::CreateInputHandler(fName, inpType, file_descriptor[1]);
  }
  

  fNEvents = fInput->GetNEvents();

  // Expect INPUTTYPE:FileLocation(s)
  std::vector<std::string> file_descriptor =
      GeneralUtils::ParseToStr(inputfile, ":");
  if (file_descriptor.size() != 2) {
    ERR(FTL) << "File descriptor had no filetype declaration: \"" << inputfile
             << "\". expected \"FILETYPE:file.root\"" << std::endl;
    throw;
  }
  fInputType = InputUtils::ParseInputType(file_descriptor[0]);

  fInputFileName = file_descriptor[1];
  if (EnuMin == 0 && EnuMax == 1.E5) {
    EnuMin = fInput->GetFluxHistogram()->GetBinLowEdge(1);
    EnuMax = fInput->GetFluxHistogram()->GetBinLowEdge(
        fInput->GetFluxHistogram()->GetNbinsX() + 1);
  }

  fFluxHist = fInput->GetFluxHistogram();
  fEventHist = fInput->GetEventHistogram();
}

//***********************************************
int MeasurementBase::GetInputID() {
  //***********************************************
  return FitBase::GetInputID(fInputFileName);
}

//***********************************************
SampleSettings MeasurementBase::LoadSampleSettings(nuiskey samplekey){
//***********************************************
  SampleSettings setting = SampleSettings(samplekey);
  fName = setting.GetS("name");

  // Used as an initial setup function incase we need to do anything here.
  LOG(SAM) << "Loading Inputs for " << setting.GetName() << std::endl;
  SetupInputs( setting.GetS("input") );

  return setting;
}

void MeasurementBase::FinaliseSampleSettings(){

  // Set type options

  // Set Fit Options

}

void FinaliseMeasurement(){

  // Run SetupDefaultHist and Masking functions if needed.

  // Run checks on input species and targets if they have been given.
}



//***********************************************
void MeasurementBase::Reconfigure() {
  //***********************************************
  LOG(REC) << " Reconfiguring sample " << fName << std::endl;

  // Reset Histograms
  this->ResetAll();

  // FitEvent* cust_event = fInput->GetEventPointer();
  int fNEvents = fInput->GetNEvents();
  int countwidth = (fNEvents / 5);

  
  // MAIN EVENT LOOP
  FitEvent* cust_event = NULL;
  for (int i = 0; i < fNEvents; i++) {
      cust_event = fInput->GetNuisanceEvent(i);
      cust_event->RWWeight = FitBase::GetRW()->CalcWeight(cust_event);
      cust_event->Weight = cust_event->RWWeight * cust_event->InputWeight;
 
      Weight = cust_event->Weight;

      // Initialize
      fXVar = -999.9;
      fYVar = -999.9;
      fZVar = -999.9;
      Signal = false;
      Mode = cust_event->Mode;

    // Extract Measurement Variables
    this->FillEventVariables(cust_event);
    Signal = this->isSignal(cust_event);

    fEventVariables.fX = fXVar;
    fEventVariables.fY = fXVar;
    fEventVariables.fZ = fXVar;
    fEventVariables.fMode = Mode;
    fEventVariables.fSignal = Signal;
    
    // Fill Histogram Values
    this->FillHistogramsFromBox(fEventVariables, Weight);

    // this->FillHistograms();
    // this->ProcessExtraHistograms(kCMD_FillHistograms, []);
    // this->FillExtraHistograms();

    // Print Out
    if (LOG_LEVEL(REC) && countwidth > 0 && !(i % countwidth)) {
      std::stringstream ss("");
      ss.unsetf(ios_base::fixed);
      ss << std::setw(7) << std::right << i << "/" << fNEvents << " events ("
         << std::setw(2) << double(i) / double(fNEvents) * 100. << std::left
         << std::setw(5) << "%) "
         << "[S,X,Y,Z,M,W] = [" << std::fixed << std::setprecision(2)
         << std::right << Signal << ", " << std::setw(5) << fXVar << ", "
         << std::setw(5) << fYVar << ", " << std::setw(5) << fYVar << ", "
         << std::setw(3) << (int)Mode << ", " << std::setw(5) << Weight << "] "
         << std::endl;
      LOG(SAM) << ss.str();
    }
  }

  int npassed = fXVar_VECT.size();
  LOG(SAM) << npassed << "/" << fNEvents << " passed selection " << std::endl;
  if (npassed == 0) {
    LOG(SAM) << "WARNING: NO EVENTS PASSED SELECTION!" << std::endl;
  }

  // Finalise Histograms
  fMCFilled = true;
  this->ConvertEventRates();
}

void MeasurementBase::FillHistogramsFromBox(MeasurementVariablesBox var, double weight){

  fXVar  = fEventVariables.fX;
  fYVar  = fEventVariables.fY;
  fZVar  = fEventVariables.fZ;
  Signal = fEventVariables.fSignal;
  Mode   = fEventVariables.fMode;
  Weight = weight;

  FillHistograms();

}

void MeasurementBase::FillVariableBox(FitEvent* event){
  
  fEventVariables.Reset();

  this->FillEventVariables(event);
  Signal = this->isSignal(event);

  fEventVariables.fX = fXVar;
  fEventVariables.fY = fXVar;
  fEventVariables.fZ = fXVar;
  fEventVariables.fMode = Mode;
  fEventVariables.fSignal = Signal;

}

MeasurementVariablesBox MeasurementBase::GetVariableBox(){
  return fEventVariables;
}

//***********************************************
void MeasurementBase::ReconfigureFast() {
  //***********************************************
  this->Reconfigure();
}

//***********************************************
void MeasurementBase::ConvertEventRates() {
  //***********************************************

  this->ScaleEvents();
  this->ApplyNormScale(FitBase::GetRW()->GetSampleNorm(this->fName));
}

//***********************************************
InputHandlerBase* MeasurementBase::GetInput() {
  //***********************************************

  if (!fInput) {
    ERR(FTL) << "MeasurementBase::fInput not set. Please submit your command "
                "line options and input cardfile with a bug report to: "
                "nuisance@projects.hepforge.org"
             << std::endl;
    throw;
  }
  return fInput;
};

//***********************************************
void MeasurementBase::Renormalise() {
  //***********************************************

  // Called when the fitter has changed a measurements normalisation but not any
  // reweight dials
  // Means we don't have to call the time consuming reconfigure when this
  // happens.
  double norm = FitBase::GetRW()->GetDialValue(this->fName + "_norm");

  if ((this->fCurrentNorm == 0.0 and norm != 0.0) or not fMCFilled) {
    this->ReconfigureFast();
    return;
  }

  if (this->fCurrentNorm == norm) return;

  this->ApplyNormScale(1.0 / this->fCurrentNorm);
  this->ApplyNormScale(norm);

  return;
};

//***********************************************
void MeasurementBase::SetSignal(bool sig) {
  //***********************************************
  Signal = sig;
}

//***********************************************
void MeasurementBase::SetSignal(FitEvent* evt) {
  //***********************************************
  Signal = this->isSignal(evt);
}

//***********************************************
void MeasurementBase::SetWeight(double wght) {
  //***********************************************
  Weight = wght;
}

//***********************************************
void MeasurementBase::SetMode(int md) {
  //***********************************************
  Mode = md;
}

//***********************************************
std::vector<TH1*> MeasurementBase::GetFluxList() {
  //***********************************************
  return GetInput()->GetFluxList();
}

//***********************************************
std::vector<TH1*> MeasurementBase::GetEventRateList() {
  //***********************************************
  return GetInput()->GetEventList();
}

//***********************************************
std::vector<TH1*> MeasurementBase::GetXSecList() {
  //***********************************************
  return GetInput()->GetXSecList();
}
