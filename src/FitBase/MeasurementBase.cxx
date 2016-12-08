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
  fNoData = false;
  fInput = NULL;

  // Set the default values
  // After-wards this gets set in SetupMeasurement
  EnuMin = 0.;
  EnuMax = 1.E5;

  fMeasurementSpeciesType = kSingleSpeciesMeasurement;
};

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

    fInput = new InputHandler(fName, inpType, file_descriptor[1]);
  }

  fFluxHist = (TH1D*)  fInput->GetFluxHistogram()->Clone();
  fEventHist = (TH1D*) fInput->GetEventHistogram()->Clone();
  fXSecHist = (TH1D*)  fInput->GetXSecHistogram()->Clone();
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

  fFluxHist      = fInput->GetFluxHistogram();
  fEventHist     = fInput->GetEventHistogram();
  fXSecHist      = fInput->GetXSecHistogram();
  fNEvents       = fInput->GetNEvents();

  fInputFileName = file_descriptor[1];
  if (EnuMin == 0 && EnuMax == 1.E5) {
    EnuMin = fFluxHist->GetBinLowEdge(1);
    EnuMax = fFluxHist->GetBinLowEdge(fFluxHist->GetNbinsX()+1);
  }
}

//***********************************************
int MeasurementBase::GetInputID() {
  //***********************************************
  return FitBase::GetInputID(fInputFileName);
}

//***********************************************
void MeasurementBase::Reconfigure() {
  //***********************************************
  LOG(REC) << " Reconfiguring sample " << fName << std::endl;

  bool using_evtmanager = FitPar::Config().GetParB("EventManager");
  int input_id = -1;
  if (using_evtmanager) {
    input_id = FitBase::GetInputID(fInputFileName);
  }
  cust_event = fInput->GetEventPointer();

  if (FitPar::Config().GetParI("cachesize") > 0) {
    fInput->SetupCache();
  }

  // Reset Histograms
  this->ResetAll();

  // READ in spline head for this input
  if (fInput->GetType() == kEVTSPLINE) {
    FitBase::GetRW()->ReadSplineHead(fInput->GetSplineHead());
  }

  FitEvent* cust_event = fInput->GetEventPointer();
  int fNEvents = fInput->GetNEvents();
  int countwidth = (fNEvents / 5);

  // Reset Signal Vectors
  fXVar_VECT.clear();
  fYVar_VECT.clear();
  fZVar_VECT.clear();
  this->fMode_VECT.clear();
  this->fIndex_VECT.clear();


  #ifdef __GiBUU_ENABLED__
  bool UsingGiBUU = (fInput->GetType() == kGiBUU);
  #endif
  
  size_t NSignal = 0;
  // MAIN EVENT LOOP
  for (int i = 0; i < fNEvents; i++) {
    // Read in the TChain and Calc Kinematics
    if (using_evtmanager) {
      cust_event = FitBase::EvtManager().GetEvent(input_id, i);
    } else {
      fInput->ReadEvent(i);

      cust_event->RWWeight = FitBase::GetRW()->CalcWeight(cust_event);
      cust_event->Weight = cust_event->RWWeight * cust_event->InputWeight;
    }

    Weight = cust_event->Weight;


    #ifdef __GiBUU_ENABLED__
    
    /// For multi species measurements the flux scalings must be correctly
    /// applied here
    if (UsingGiBUU) {
      switch (fMeasurementSpeciesType) {
        case kSingleSpeciesMeasurement:
        default: { break; }
        case kNumuWithWrongSignMeasurement: {
          Weight *= cust_event->GiRead->SpeciesWght_numu;
          break;
        }
        case kNueWithWrongSignMeasurement: {
          Weight *= cust_event->GiRead->SpeciesWght_nue;
          break;
        }
        case kFourSpeciesMeasurement: {
          Weight *= cust_event->GiRead->SpeciesWght;
          break;
        }
      }
    }

    #endif
    
    // Initialize
    fXVar = -999.9;
    fYVar = -999.9;
    fZVar = -999.9;
    Signal = false;
    Mode = cust_event->Mode;

    // Extract Measurement Variables
    this->FillEventVariables(cust_event);
    Signal = this->isSignal(cust_event);

    // Push Back Signal
    if (Signal) {
      fXVar_VECT.push_back(fXVar);
      fYVar_VECT.push_back(fYVar);
      fZVar_VECT.push_back(fZVar);
      this->fMode_VECT.push_back(Mode);
      this->fIndex_VECT.push_back((UInt_t)i);
      NSignal++;
    }

    // Fill Histogram Values
    this->FillHistograms();
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

//***********************************************
void MeasurementBase::ReconfigureFast() {
  //***********************************************
  LOG(REC) << " Reconfiguring signal " << this->fName << std::endl;

  bool using_evtmanager = FitPar::Config().GetParB("EventManager");
  int input_id = -1;

  if (using_evtmanager) {
    input_id = FitBase::GetInputID(fInputFileName);
  } else {
    cust_event = fInput->GetEventPointer();
  }

  // Check if we Can't Signal Reconfigure
  if (!fMCFilled) {
    this->Reconfigure();
    return;
  }

  // Reset Histograms
  this->ResetAll();

  // READ in spline head for this input
  if (fInput->GetType() == kEVTSPLINE) {
    FitBase::GetRW()->ReadSplineHead(fInput->GetSplineHead());
  }

  // Get Pointer To Base Event (Just Generator Formats)
  int countwidth = (fIndex_VECT.size() / 5);

  // Setup Iterators
  std::vector<double>::iterator X = fXVar_VECT.begin();
  std::vector<double>::iterator Y = fYVar_VECT.begin();
  std::vector<double>::iterator Z = fZVar_VECT.begin();
  std::vector<int>::iterator M = fMode_VECT.begin();
  std::vector<UInt_t>::iterator I = fIndex_VECT.begin();


  #ifdef __GiBUU_ENABLED__
  bool UsingGiBUU = (fInput->GetType() == kGiBUU);
  #endif
  
  // SIGNAL LOOP
  for (int i = 0; I != fIndex_VECT.end(); I++, i++) {
    // Just Update Weight
    if (using_evtmanager) {
      Weight = FitBase::EvtManager().GetEventWeight(input_id, (*I));
    } else {
      fInput->GetTreeEntry((*I));
      Weight =
          FitBase::GetRW()->CalcWeight(cust_event) * cust_event->InputWeight;
    }


    #ifdef __GiBUU_ENABLED__
    /// For multi species measurements the flux scalings must be correctly
    /// applied here
    if (UsingGiBUU) {
      switch (fMeasurementSpeciesType) {
        case kSingleSpeciesMeasurement:
        default: { break; }
        case kNumuWithWrongSignMeasurement: {
          Weight *= cust_event->GiRead->SpeciesWght_numu;
          break;
        }
        case kNueWithWrongSignMeasurement: {
          Weight *= cust_event->GiRead->SpeciesWght_nue;
          break;
        }
        case kFourSpeciesMeasurement: {
          Weight *= cust_event->GiRead->SpeciesWght;
          break;
        }
      }
    }
    #endif

    fXVar = (*X);
    fYVar = (*Y);
    fZVar = (*Z);
    Mode = (*M);

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
      LOG(REC) << "Reconfigured " << std::setw(7) << std::right << i
               << " signal events. [X,Y,Z,M,W] = [" << std::setprecision(2)
               << std::setw(5) << std::right << fXVar << ", " << std::setw(5)
               << std::right << fYVar << ", " << std::setw(5) << std::right
               << fYVar << ", " << std::setw(3) << std::right << (int)Mode
               << ", " << std::setw(5) << std::right << Weight << "] "
               << std::endl;
  }

  // Finalise histograms
  fMCFilled = true;
  this->ConvertEventRates();
}

//***********************************************
void MeasurementBase::ConvertEventRates() {
  //***********************************************

  this->ScaleEvents();
  this->ApplyNormScale(FitBase::GetRW()->GetSampleNorm(this->fName));
}

//***********************************************
InputHandler* MeasurementBase::GetInput() {
  //***********************************************

  if (!fInput) {
    ERR(FTL) << "MeasurementBase::fInput not set. Please submit your command "
                "line options and input cardfile with a bug report to: "
                "nuisance@projects.hepforge.org"
             << std::endl;
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
