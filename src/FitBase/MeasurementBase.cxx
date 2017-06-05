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

#include <ios>
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

  fMeasurementSpeciesType = kSingleSpeciesMeasurement;
  fEventVariables = NULL;
  fIsJoint = false;
};

void MeasurementBase::FinaliseMeasurement() {

  // Used to setup default data hists, covars, etc.



}

//********************************************************************
// 2nd Level Destructor (Inherits From MeasurementBase.h)
MeasurementBase::~MeasurementBase() {
  //********************************************************************

};

//********************************************************************
double MeasurementBase::TotalIntegratedFlux(std::string intOpt, double low,
    double high) {
//********************************************************************

  // Set Energy Limits
  if (low == -9999.9) low = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;
  return GetInput()->TotalIntegratedFlux(low, high, intOpt);
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
    fInput = FitBase::AddInput(fName, inputfile);
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
SampleSettings MeasurementBase::LoadSampleSettings(nuiskey samplekey) {
//***********************************************
  SampleSettings setting = SampleSettings(samplekey);
  fName = setting.GetS("name");

  // Used as an initial setup function incase we need to do anything here.
  LOG(SAM) << "Loading Sample : " << setting.GetName() << std::endl;
  if (!fIsJoint) SetupInputs( setting.GetS("input") );

  return setting;
}

//***********************************************
SampleSettings MeasurementBase::LoadSampleSettings(std::string name, std::string input, std::string type) {
//***********************************************

  nuiskey samplekey = Config::CreateKey("sample");
  samplekey.SetS("name",name);
  samplekey.SetS("input",input);
  samplekey.SetS("type",type);

  return LoadSampleSettings(samplekey);
}

void MeasurementBase::FinaliseSampleSettings() {

  EnuMin = fSettings.GetD("enu_min");
  EnuMax = fSettings.GetD("enu_max");

  std::cout << "SetEnuMin = " << EnuMin << " "<< EnuMax << std::endl;

}


//***********************************************
void MeasurementBase::Reconfigure() {
//***********************************************
  
  LOG(REC) << " Reconfiguring sample " << fName << std::endl;

  // Reset Histograms
  ResetExtraHistograms();
  AutoResetExtraTH1();
  this->ResetAll();

  // FitEvent* cust_event = fInput->GetEventPointer();
  int fNEvents = fInput->GetNEvents();
  int countwidth = (fNEvents / 5);


  // MAIN EVENT LOOP
  FitEvent* cust_event = fInput->FirstNuisanceEvent();
  int i = 0;
  int npassed = 0;
  while(cust_event){

    cust_event->RWWeight = fRW->CalcWeight(cust_event);
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
    if (Signal) npassed++;

    GetBox()->SetX(fXVar);
    GetBox()->SetY(fYVar);
    GetBox()->SetZ(fZVar);
    GetBox()->SetMode(Mode);
    // GetBox()->fSignal = Signal;

    // Fill Histogram Values
    GetBox()->FillBoxFromEvent(cust_event);
    // this->FillExtraHistograms(GetBox(), Weight);
    this->FillHistogramsFromBox(GetBox(), Weight);

    // Print Out
    if (LOG_LEVEL(REC) && countwidth > 0 && !(i % countwidth)) {
      std::stringstream ss("");
      ss.unsetf(std::ios_base::fixed);
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

    // iterate
    cust_event = fInput->NextNuisanceEvent();
    i++;
  }

  LOG(SAM) << npassed << "/" << fNEvents << " passed selection " << std::endl;
  if (npassed == 0) {
    LOG(SAM) << "WARNING: NO EVENTS PASSED SELECTION!" << std::endl;
  }
  LOG(REC) << std::setw(10) << std::right << NSignal << "/"
           << fNEvents << " events passed selection + binning after reweight"
           << std::endl;

  // Finalise Histograms
  fMCFilled = true;
  this->ConvertEventRates();
}

void MeasurementBase::FillHistogramsFromBox(MeasurementVariableBox* var, double weight) {

  fXVar  = var->GetX();
  fYVar  = var->GetY();
  fZVar  = var->GetZ();
  // Signal = var->fSignal;
  // Mode   = var->fMode;
  Weight = weight;

  FillHistograms();
  FillExtraHistograms(var, weight);

}

void MeasurementBase::FillHistograms(double weight){
  Weight = weight;
  FillHistograms();
  FillExtraHistograms(GetBox(), Weight);
}


MeasurementVariableBox* MeasurementBase::FillVariableBox(FitEvent* event) {

  GetBox()->Reset();
  Mode = event->Mode;
  
  this->FillEventVariables(event);
  Signal = this->isSignal(event);
  GetBox()->FillBoxFromEvent(event);

  GetBox()->SetX(fXVar);
  GetBox()->SetY(fYVar);
  GetBox()->SetZ(fZVar);
  GetBox()->SetMode(event->Mode);
  // GetBox()->fSignal = Signal;

  return GetBox();
}

MeasurementVariableBox* MeasurementBase::GetBox() {
  if (!fEventVariables) fEventVariables = CreateBox();
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

  AutoScaleExtraTH1();
  ScaleExtraHistograms(GetBox());
  this->ScaleEvents();

  double normval = fRW->GetSampleNorm(this->fName);
  if (normval < 0.01 or normval > 10.0){
    ERR(WRN) << "Norm Value inside MeasurementBase::ConvertEventRates() looks off!" << std::endl;
    ERR(WRN) << "It could have become out of sync with the minimizer norm list." << std::endl;
    ERR(WRN) << "Setting it to 1.0" << std::endl;
    normval = 1.0;
  }
  AutoNormExtraTH1(normval);
  NormExtraHistograms(GetBox(), normval);
  this->ApplyNormScale(normval);

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
  double norm = fRW->GetDialValue(this->fName + "_norm");

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


void MeasurementBase::ProcessExtraHistograms(int cmd,
    MeasurementVariableBox* vars,
    double weight) {
  // This should be overriden if we have extra histograms!!!
  // Add a flag to tell user this...
  return;
}

void MeasurementBase::FillExtraHistograms(MeasurementVariableBox* vars,
    double weight) {
  ProcessExtraHistograms(kCMD_Fill, vars, weight);
}

void MeasurementBase::ScaleExtraHistograms(MeasurementVariableBox* vars) {
  ProcessExtraHistograms(kCMD_Scale, vars, 1.0);
}
void MeasurementBase::ResetExtraHistograms() {
  ProcessExtraHistograms(kCMD_Reset, NULL, 1.0);
}
void MeasurementBase::NormExtraHistograms(MeasurementVariableBox* vars,
    double norm) {
  ProcessExtraHistograms(kCMD_Norm, vars, norm);
}
void MeasurementBase::WriteExtraHistograms() {
  ProcessExtraHistograms(kCMD_Write, NULL, 1.00);
}

void MeasurementBase::SetAutoProcessTH1(TH1* hist, int c1, int c2, int c3, int c4, int c5) {
  FakeStack* fake = new FakeStack(hist);
  SetAutoProcessTH1(fake, c1, c2, c3, c4, c5); // Need to add a destroy command!
}

void MeasurementBase::SetAutoProcessTH1(StackBase* hist,  int c1, int c2, int c3, int c4, int c5) {

  // Set Defaults
  // int ncommands = kCMD_extraplotflags;
  bool autoflags[5];
  autoflags[0] = false;
  autoflags[1] = false;
  autoflags[2] = false;
  autoflags[3] = false;
  autoflags[4] = false;

  int givenflags[5];
  givenflags[0] = c1;
  givenflags[1] = c2;
  givenflags[2] = c3;
  givenflags[3] = c4;
  givenflags[4] = c5;
  fExtraTH1s[hist] = std::vector<int>(5,0);

  // Setup a default one.
  if (c1 == -1 && c2 == -1 && c3 == -1 && c4 == -1 && c5 == -1){
    fExtraTH1s[hist][kCMD_Reset] = 1;
    fExtraTH1s[hist][kCMD_Scale] = 1;
    fExtraTH1s[hist][kCMD_Norm] = 1;
    fExtraTH1s[hist][kCMD_Write] = 1;
  }

  for (int i = 0; i < 5; i++) {
    switch (givenflags[i]) {

    // Skip over...
    case -1:
      break;

    case kCMD_Reset:
    case kCMD_Scale:
    case kCMD_Norm:
    case kCMD_Write:
      fExtraTH1s[hist][givenflags[i]] = 1;
      break;

    case kCMD_Fill:
      ERR(FTL) << "Can't auto fill yet!" << std::endl;
      autoflags[givenflags[i]] = 1;
      break;

    default:
      break;
    }
  }

  // LOG(SAM) << "AutoProcessing " << hist->GetName() << std::endl;
};

void MeasurementBase::AutoFillExtraTH1() {
  ERR(FTL) << "Can't auto fill yet! it's too inefficent!" << std::endl;
  return;
}

void MeasurementBase::AutoResetExtraTH1() {

  for (std::map<StackBase*, std::vector<int> >::iterator iter = fExtraTH1s.begin();
       iter != fExtraTH1s.end(); iter++) {

    if (!((*iter).second)[kCMD_Reset]) continue;
    (*iter).first->Reset();
  }
};

void MeasurementBase::AutoScaleExtraTH1() {
  for (std::map<StackBase*, std::vector<int> >::iterator iter = fExtraTH1s.begin();
       iter != fExtraTH1s.end(); iter++) {

    if (!((*iter).second)[kCMD_Scale]) continue;
    (*iter).first->Scale(fScaleFactor, "width");
  }
};

void MeasurementBase::AutoNormExtraTH1(double norm) {
  double sfactor = 0.0;
  if (norm != 0.0) sfactor = 1.0 / norm;

  for (std::map<StackBase*, std::vector<int> >::iterator iter = fExtraTH1s.begin();
       iter != fExtraTH1s.end(); iter++) {

    if (!((*iter).second)[kCMD_Norm]) continue;
    (*iter).first->Scale(sfactor);
  }
};

void MeasurementBase::AutoWriteExtraTH1() {
  for (std::map<StackBase*, std::vector<int> >::iterator iter = fExtraTH1s.begin();
       iter != fExtraTH1s.end(); iter++) {

    if (!(((*iter).second)[kCMD_Write])) continue;
    (*iter).first->Write();
  }
};



