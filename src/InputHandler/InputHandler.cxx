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
#include "InputHandler.h"

InputHandlerBase::InputHandlerBase() {
  fName = "";
  fFluxHist = NULL;
  fEventHist = NULL;
  fNEvents = 0;
  fNUISANCEEvent = NULL;
  fBaseEvent = NULL;
  kRemoveUndefParticles = FitPar::Config().GetParB("RemoveUndefParticles");
  kRemoveFSIParticles = FitPar::Config().GetParB("RemoveFSIParticles");
  kRemoveNuclearParticles = FitPar::Config().GetParB("RemoveNuclearParticles");
  fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");
  fTTreePerformance = NULL;

};

InputHandlerBase::~InputHandlerBase() {

  if (fFluxHist) delete fFluxHist;
  if (fEventHist) delete fEventHist;
  //  if (fXSecHist) delete fXSecHist;
  //  if (fNUISANCEEvent) delete fNUISANCEEvent;
  jointfluxinputs.clear();
  jointeventinputs.clear();
  jointindexlow.clear();
  jointindexhigh.clear();
  jointindexallowed.clear();
  jointindexscale.clear();

  //  if (fTTreePerformance) {
  //    fTTreePerformance->SaveAs(("ttreeperfstats_" + fName + ".root").c_str());
    //  }
}

void InputHandlerBase::Print() {
};

TH1D* InputHandlerBase::GetXSecHistogram(void) {
  fXSecHist = (TH1D*)fFluxHist->Clone();
  fXSecHist->Divide(fEventHist);
  return fXSecHist;
};

double InputHandlerBase::PredictedEventRate(double low, double high,
    std::string intOpt) {

  int minBin = fFluxHist->GetXaxis()->FindBin(low);
  int maxBin = fFluxHist->GetXaxis()->FindBin(high);

  return fEventHist->Integral(minBin, maxBin + 1, intOpt.c_str());
};

double InputHandlerBase::TotalIntegratedFlux(double low, double high,
    std::string intOpt) {

  Int_t minBin = fFluxHist->GetXaxis()->FindFixBin(low);
  Int_t maxBin = fFluxHist->GetXaxis()->FindFixBin(high);

  if ((fFluxHist->IsBinOverflow(minBin) && (low != -9999.9))) {
    minBin = 1;
  }

  if ((fFluxHist->IsBinOverflow(maxBin) && (high != -9999.9))) {
    maxBin = fFluxHist->GetXaxis()->GetNbins() + 1;
  }

  // If we are within a single bin
  if (minBin == maxBin) {
    // Get the contained fraction of the single bin's width
    return ((high - low) / fFluxHist->GetXaxis()->GetBinWidth(minBin)) *
           fFluxHist->Integral(minBin, minBin, intOpt.c_str());
  }

  double lowBinUpEdge = fFluxHist->GetXaxis()->GetBinUpEdge(minBin);
  double highBinLowEdge = fFluxHist->GetXaxis()->GetBinLowEdge(maxBin);

  double lowBinfracIntegral =
    ((lowBinUpEdge - low) / fFluxHist->GetXaxis()->GetBinWidth(minBin)) *
    fFluxHist->Integral(minBin, minBin, intOpt.c_str());
  double highBinfracIntegral =
    ((high - highBinLowEdge) / fFluxHist->GetXaxis()->GetBinWidth(maxBin)) *
    fFluxHist->Integral(maxBin, maxBin, intOpt.c_str());

  // If they are neighbouring bins
  if ((minBin + 1) == maxBin) {
    std::cout << "Get lowfrac + highfrac" << std::endl;
    // Get the contained fraction of the two bin's width
    return lowBinfracIntegral + highBinfracIntegral;
  }

  // If there are filled bins between them
  return lowBinfracIntegral + highBinfracIntegral +
         fFluxHist->Integral(minBin + 1, maxBin - 1, intOpt.c_str());
  // return fFluxHist->Integral(minBin + 1, maxBin - 1, intOpt.c_str());
}


std::vector<TH1*> InputHandlerBase::GetFluxList(void) {
  return std::vector<TH1*>(1, fFluxHist);
};

std::vector<TH1*> InputHandlerBase::GetEventList(void) {
  return std::vector<TH1*>(1, fEventHist);
};

std::vector<TH1*> InputHandlerBase::GetXSecList(void) {
  return std::vector<TH1*>(1, GetXSecHistogram());
};

FitEvent* InputHandlerBase::FirstNuisanceEvent() {
  fCurrentIndex = 0;
  return GetNuisanceEvent(fCurrentIndex);
};



FitEvent* InputHandlerBase::NextNuisanceEvent() {
  fCurrentIndex++;
  if((fMaxEvents != -1) && (fCurrentIndex > fMaxEvents)){
    return NULL;
  }

  return GetNuisanceEvent(fCurrentIndex);
};


BaseFitEvt* InputHandlerBase::FirstBaseEvent() {
  fCurrentIndex = 0;
  return GetBaseEvent(fCurrentIndex);
};

BaseFitEvt* InputHandlerBase::NextBaseEvent() {
  fCurrentIndex++;

  if (jointinput and fMaxEvents != -1) {
    while ( fCurrentIndex < jointindexlow[jointindexswitch] ||
            fCurrentIndex >= jointindexhigh[jointindexswitch] ) {
      jointindexswitch++;

      // Loop Around
      if (jointindexswitch == jointindexlow.size()) {
        jointindexswitch = 0;
      }
    }


    if (fCurrentIndex > jointindexlow[jointindexswitch] + jointindexallowed[jointindexswitch]) {
      fCurrentIndex = jointindexlow[jointindexswitch];
    }
  }

  return GetBaseEvent(fCurrentIndex);
};


void InputHandlerBase::RegisterJointInput(std::string input, int n, TH1D* f, TH1D* e) {

  if (jointfluxinputs.size() == 0) {
    jointindexswitch = 0;
    fNEvents = 0;
  }

  // Push into individual input vectors
  jointfluxinputs.push_back(  (TH1D*) f->Clone() );
  jointeventinputs.push_back( (TH1D*) e->Clone() );

  jointindexlow.push_back(fNEvents);
  jointindexhigh.push_back(fNEvents + n);
  fNEvents += n;

  // Add to the total flux/event hist
  if (!fFluxHist) fFluxHist = (TH1D*) f->Clone();
  else fFluxHist->Add(f);

  if (!fEventHist) fEventHist = (TH1D*) e->Clone();
  else fEventHist->Add(e);

}


void InputHandlerBase::SetupJointInputs() {

  if (jointeventinputs.size() <= 1) {
    jointinput = false;
  } else if (jointeventinputs.size() > 1) {
    jointinput = true;
    jointindexswitch = 0;
  }
  fMaxEvents = FitPar::Config().GetParI("MAXEVENTS");
  if (fMaxEvents != -1 and jointeventinputs.size() > 1){
    THROW("Can only handle joint inputs when config MAXEVENTS = -1!");
  }

  for (size_t i = 0; i < jointeventinputs.size(); i++) {
    TH1D* eventhist = (TH1D*) jointeventinputs.at(i)->Clone();

    double scale = double(fNEvents) / fEventHist->Integral("width");
    scale *= eventhist->Integral("width");
    scale /= double(jointindexhigh[i] - jointindexlow[i]);

    jointindexscale .push_back(scale);
  }

  fEventHist->SetNameTitle((fName + "_EVT").c_str(), (fName + "_EVT").c_str());
  fFluxHist->SetNameTitle((fName + "_FLUX").c_str(), (fName + "_FLUX").c_str());

  // Setup Max Events
  if (fMaxEvents > 1 && fMaxEvents < fNEvents) {
    if (LOG_LEVEL(SAM)) {
      std::cout << "\t\t|-> Read Max Entries : " << fMaxEvents << std::endl;
    }
    fNEvents = fMaxEvents;
  }

  // Print out Status
  if (LOG_LEVEL(SAM)) {
    std::cout << "\t\t|-> Total Entries    : " << fNEvents << std::endl
              << "\t\t|-> Event Integral   : " << fEventHist->Integral("width") * 1.E-38 << " events/nucleon" << std::endl
              << "\t\t|-> Flux Integral    : " << fFluxHist->Integral("width") << " /cm2" << std::endl
              << "\t\t|-> Event/Flux       : "
              << fEventHist->Integral("width") * 1.E-38 / fFluxHist->Integral("width") << " cm2/nucleon" <<  std::endl;
  }

}

BaseFitEvt* InputHandlerBase::GetBaseEvent(const UInt_t entry) {
  return static_cast<BaseFitEvt*>(GetNuisanceEvent(entry, true));
}

double InputHandlerBase::GetInputWeight(int entry) {

  if (!jointinput) return 1.0;

  // Find Switch Scale
  while ( entry < jointindexlow[jointindexswitch] ||
          entry >= jointindexhigh[jointindexswitch] ) {
    jointindexswitch++;

    // Loop Around
    if (jointindexswitch >= jointindexlow.size()) {
      jointindexswitch = 0;
    }
  }

  return jointindexscale[jointindexswitch];
};

