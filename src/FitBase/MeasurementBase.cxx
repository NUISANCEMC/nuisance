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
  NSignal = 0;

  // Set the default values
  // After-wards this gets set in SetupMeasurement
  EnuMin = 0.;
  EnuMax = 1.E5;

  fMeasurementSpeciesType = kSingleSpeciesMeasurement;
  fEventVariables = NULL;
  fIsJoint = false;

  fNPOT = 0xdeadbeef;
  fFluxIntegralOverride = 0xdeadbeef;
  fTargetVolume = 0xdeadbeef;
  fTargetMaterialDensity = 0xdeadbeef;
  fEvtRateScaleFactor = 0xdeadbeef;
};

void MeasurementBase::FinaliseMeasurement() {
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
  if (low == -9999.9)
    low = this->EnuMin;
  if (high == -9999.9)
    high = this->EnuMax;
  return GetInput()->TotalIntegratedFlux(low, high, intOpt);
};

//********************************************************************
double MeasurementBase::PredictedEventRate(std::string intOpt, double low,
                                           double high) {
  //********************************************************************

  // Set Energy Limits
  if (low == -9999.9)
    low = this->EnuMin;
  if (high == -9999.9)
    high = this->EnuMax;

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
      NUIS_ABORT("File descriptor had no filetype declaration: \""
             << inputfile << "\". expected \"FILETYPE:file.root\"");
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
    NUIS_ABORT("File descriptor had no filetype declaration: \""
           << inputfile << "\". expected \"FILETYPE:file.root\"");
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
  NUIS_LOG(SAM, "Loading Sample : " << setting.GetName());

  fEvtRateScaleFactor = 0xdeadbeef;

  if (!fIsJoint) {
    SetupInputs(setting.GetS("input"));

    fNPOT = samplekey.Has("NPOT") ? samplekey.GetD("NPOT") : 1;
    fFluxIntegralOverride = samplekey.Has("FluxIntegralOverride")
                                ? samplekey.GetD("FluxIntegralOverride")
                                : 0xdeadbeef;
    fTargetVolume = samplekey.Has("TargetVolume")
                        ? samplekey.GetD("TargetVolume")
                        : 0xdeadbeef;
    fTargetMaterialDensity = samplekey.Has("TargetMaterialDensity")
                                 ? samplekey.GetD("TargetMaterialDensity")
                                 : 0xdeadbeef;
    if ((fTargetVolume != 0xdeadbeef) &&
        (fTargetMaterialDensity != 0xdeadbeef)) {
      double TargetMass_kg = fTargetVolume * fTargetMaterialDensity;
      double NNucleons = TargetMass_kg / PhysConst::mass_nucleon_kg;
      double NNeutrinos =
          ((fFluxIntegralOverride == 0xdeadbeef) ? TotalIntegratedFlux()
                                                 : fFluxIntegralOverride) *
          fNPOT;
      fEvtRateScaleFactor = NNeutrinos * NNucleons;

      NUIS_LOG(SAM, "\tEvent rate prediction : ");
      NUIS_LOG(SAM, "\t\tTarget volume : " << fTargetVolume << " m^3");
      NUIS_LOG(SAM, "\t\tTarget density : " << fTargetMaterialDensity << " kg/m^3");
      NUIS_LOG(SAM, "\t\tTarget mass : " << TargetMass_kg << " kg");
      NUIS_LOG(SAM, "\t\tNTarget Nucleons : " << NNucleons);
      if ((fNPOT != 1)) {
        NUIS_LOG(SAM, "\t\tTotal POT : " << fNPOT);
      }
      NUIS_LOG(SAM, "\t\tNNeutrinos : "
                    << NNeutrinos << ((fNPOT != 1) ? " /cm^2" : " /POT /cm^2"));
      NUIS_LOG(SAM, "\t\tXSec -> EvtRate scale factor : " << fEvtRateScaleFactor);
    }
  }

  return setting;
}

//***********************************************
SampleSettings MeasurementBase::LoadSampleSettings(std::string name,
                                                   std::string input,
                                                   std::string type) {
  //***********************************************

  nuiskey samplekey = Config::CreateKey("sample");
  samplekey.SetS("name", name);
  samplekey.SetS("input", input);
  samplekey.SetS("type", type);

  return LoadSampleSettings(samplekey);
}

void MeasurementBase::FinaliseSampleSettings() {
  EnuMin = fSettings.GetD("enu_min");
  EnuMax = fSettings.GetD("enu_max");
}

//***********************************************
void MeasurementBase::Reconfigure() {
  //***********************************************

  NUIS_LOG(REC, " Reconfiguring sample " << fName);

  // Reset Histograms
  ResetExtraHistograms();
  AutoResetExtraTH1();
  this->ResetAll();

  // FitEvent* cust_event = fInput->GetEventPointer();
  int fNEvents = fInput->GetNEvents();
  int countwidth = (fNEvents / 5);

  // MAIN EVENT LOOP
  FitEvent *cust_event = fInput->FirstNuisanceEvent();
  int i = 0;
  int npassed = 0;
  while (cust_event) {
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
    if (Signal)
      npassed++;

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
         << std::setw(2) << int(double(i) / double(fNEvents) * 100.) + 1
         << std::left << std::setw(5) << "%) "
         << "[S,X,Y,Z,M,W] = [" << std::fixed << std::setprecision(2)
         << std::right << Signal << ", " << std::setw(5) << fXVar << ", "
         << std::setw(5) << fYVar << ", " << std::setw(5) << fYVar << ", "
         << std::setw(3) << (int)Mode << ", " << std::setw(5) << Weight << "] "
         << std::endl;
      NUIS_LOG(SAM, ss.str());
    }

    // iterate
    cust_event = fInput->NextNuisanceEvent();
    i++;
  }

  NUIS_LOG(SAM, npassed << "/" << fNEvents << " passed selection ");
  if (npassed == 0) {
    NUIS_LOG(SAM, "WARNING: NO EVENTS PASSED SELECTION!");
  }
  NUIS_LOG(REC,
       std::setw(10) << std::right << NSignal << "/" << fNEvents
                     << " events passed selection + binning after reweight");

  // Finalise Histograms
  fMCFilled = true;
  this->ConvertEventRates();
}

void MeasurementBase::FillHistogramsFromBox(MeasurementVariableBox *var,
                                            double weight) {
  fXVar = var->GetX();
  fYVar = var->GetY();
  fZVar = var->GetZ();
  Weight = weight;
  fEventVariables = var;

  FillHistograms();
  FillExtraHistograms(var, weight);
}

void MeasurementBase::FillHistograms(double weight) {
  Weight = weight * GetBox()->GetSampleWeight();
  FillHistograms();
  FillExtraHistograms(GetBox(), Weight);
}

MeasurementVariableBox *MeasurementBase::FillVariableBox(FitEvent *event) {
  GetBox()->Reset();
  Mode = event->Mode;
  Weight = 1.0; // event->Weight;

  this->FillEventVariables(event);
  Signal = this->isSignal(event);

  GetBox()->FillBoxFromEvent(event);

  GetBox()->SetX(fXVar);
  GetBox()->SetY(fYVar);
  GetBox()->SetZ(fZVar);
  GetBox()->SetMode(event->Mode);
  GetBox()->SetSampleWeight(Weight);
  // GetBox()->fSignal = Signal;

  return GetBox();
}

MeasurementVariableBox *MeasurementBase::GetBox() {
  if (!fEventVariables)
    fEventVariables = CreateBox();
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
  if (normval < 0.01 or normval > 10.0) {
    NUIS_ERR(WRN,
           "Norm Value inside MeasurementBase::ConvertEventRates() looks off!");
    NUIS_ERR(WRN,
           "It could have become out of sync with the minimizer norm list.");
    NUIS_ERR(WRN, "Setting it to 1.0");
    normval = 1.0;
  }
  AutoNormExtraTH1(normval);
  NormExtraHistograms(GetBox(), normval);
  this->ApplyNormScale(normval);
}

//***********************************************
InputHandlerBase *MeasurementBase::GetInput() {
  //***********************************************

  if (!fInput) {
    NUIS_ABORT("MeasurementBase::fInput not set. Please submit your command "
           "line options and input cardfile with a bug report to: "
           "nuisance@projects.hepforge.org");
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

  if (this->fCurrentNorm == norm)
    return;

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
void MeasurementBase::SetSignal(FitEvent *evt) {
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
std::vector<TH1 *> MeasurementBase::GetFluxList() {
  //***********************************************
  return GetInput()->GetFluxList();
}

//***********************************************
std::vector<TH1 *> MeasurementBase::GetEventRateList() {
  //***********************************************
  return GetInput()->GetEventList();
}

//***********************************************
std::vector<TH1 *> MeasurementBase::GetXSecList() {
  //***********************************************
  return GetInput()->GetXSecList();
}

void MeasurementBase::ProcessExtraHistograms(int cmd,
                                             MeasurementVariableBox *vars,
                                             double weight) {
  // This should be overriden if we have extra histograms!!!
  // Add a flag to tell user this...
  return;
}

void MeasurementBase::FillExtraHistograms(MeasurementVariableBox *vars,
                                          double weight) {
  ProcessExtraHistograms(kCMD_Fill, vars, weight);
}

void MeasurementBase::ScaleExtraHistograms(MeasurementVariableBox *vars) {
  ProcessExtraHistograms(kCMD_Scale, vars, 1.0);
}
void MeasurementBase::ResetExtraHistograms() {
  ProcessExtraHistograms(kCMD_Reset, NULL, 1.0);
}
void MeasurementBase::NormExtraHistograms(MeasurementVariableBox *vars,
                                          double norm) {
  ProcessExtraHistograms(kCMD_Norm, vars, norm);
}
void MeasurementBase::WriteExtraHistograms() {
  ProcessExtraHistograms(kCMD_Write, NULL, 1.00);
}

void MeasurementBase::SetAutoProcessTH1(TH1 *hist, int c1, int c2, int c3,
                                        int c4, int c5) {
  FakeStack *fake = new FakeStack(hist);
  SetAutoProcessTH1(fake, c1, c2, c3, c4,
                    c5); // Need to add a destroy command!
}

void MeasurementBase::SetAutoProcess(TH1 *hist, int c1, int c2, int c3, int c4,
                                     int c5) {
  FakeStack *fake = new FakeStack(hist);
  SetAutoProcessTH1(fake, c1, c2, c3, c4,
                    c5); // Need to add a destroy command!
}

void MeasurementBase::SetAutoProcess(TGraph *g, int c1, int c2, int c3, int c4,
                                     int c5) {
  FakeStack *fake = new FakeStack(g);
  SetAutoProcessTH1(fake, c1, c2, c3, c4,
                    c5); // Need to add a destroy command!
}

void MeasurementBase::SetAutoProcess(TF1 *f, int c1, int c2, int c3, int c4,
                                     int c5) {
  FakeStack *fake = new FakeStack(f);
  SetAutoProcessTH1(fake, c1, c2, c3, c4,
                    c5); // Need to add a destroy command!
}

void MeasurementBase::SetAutoProcess(StackBase *hist, int c1, int c2, int c3,
                                     int c4, int c5) {
  SetAutoProcessTH1(hist, c1, c2, c3, c4, c5);
}

void MeasurementBase::SetAutoProcessTH1(StackBase *hist, int c1, int c2, int c3,
                                        int c4, int c5) {
  // Set Defaults
  // int ncommands = kCMD_extraplotflags;
  int givenflags[5];
  givenflags[0] = c1;
  givenflags[1] = c2;
  givenflags[2] = c3;
  givenflags[3] = c4;
  givenflags[4] = c5;
  fExtraTH1s[hist] = std::vector<int>(5, 0);

  // Setup a default one.
  if (c1 == -1 && c2 == -1 && c3 == -1 && c4 == -1 && c5 == -1) {
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

    case kCMD_Fill: {
      NUIS_ERR(FTL, "Can't auto fill yet!");
      break;
    }

    default:
      break;
    }
  }

}

void MeasurementBase::AutoFillExtraTH1() {
  NUIS_ABORT("Can't auto fill yet! it's too inefficent!");
  return;
}

void MeasurementBase::AutoResetExtraTH1() {
  for (std::map<StackBase *, std::vector<int> >::iterator iter =
           fExtraTH1s.begin();
       iter != fExtraTH1s.end(); iter++) {
    if (!((*iter).second)[kCMD_Reset])
      continue;
    (*iter).first->Reset();
  }
};

void MeasurementBase::AutoScaleExtraTH1() {
  for (std::map<StackBase *, std::vector<int> >::iterator iter =
           fExtraTH1s.begin();
       iter != fExtraTH1s.end(); iter++) {
    if (!((*iter).second)[kCMD_Scale])
      continue;
    if (fIsNoWidth) {
      (*iter).first->Scale(fScaleFactor);
    } else {
      (*iter).first->Scale(fScaleFactor, "width");
    }
  }
};

void MeasurementBase::AutoNormExtraTH1(double norm) {
  double sfactor = 0.0;
  if (norm != 0.0)
    sfactor = 1.0 / norm;

  for (std::map<StackBase *, std::vector<int> >::iterator iter =
           fExtraTH1s.begin();
       iter != fExtraTH1s.end(); iter++) {
    if (!((*iter).second)[kCMD_Norm])
      continue;
    (*iter).first->Scale(sfactor);
  }
};

void MeasurementBase::AutoWriteExtraTH1() {
  for (std::map<StackBase *, std::vector<int> >::iterator iter =
           fExtraTH1s.begin();
       iter != fExtraTH1s.end(); iter++) {
    if (!(((*iter).second)[kCMD_Write]))
      continue;
    (*iter).first->Write();
  }
};
