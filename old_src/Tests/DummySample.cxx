#include "ConstructibleFitEvent.h"
#include "ConstructibleInputHandler.h"
#include "Measurement1D.h"

struct DummySample : public Measurement1D {

  std::vector<ConstructibleFitEvent *> FitEvents;
  DummySample(nuiskey samplekey) {
    ConstructibleInputHandler *cih = new ConstructibleInputHandler("DummyIHandler");
    fInput = cih;

    fSettings = SampleSettings(samplekey);
    fSettings.SetTitle("DummySample");
    FinaliseSampleSettings();

    fDataHist = new TH1D("data", "", 3, 1, 4);
    fMCHist = new TH1D("data", "", 3, 1, 4);

    double FakeDataError = Config::GetParD("FakeDataError");

    fScaleFactor = 1;

    int IS[] = {14};
    int FS_CC0pi_1[] = {13, 2212};
    int FS_CC1pi_1[] = {13, 2212, 211};
    int FS_CC2pi_1[] = {13, 2212, 211, -211};
    FitEvents.push_back(new ConstructibleFitEvent(MakePDGStackEvent(IS, FS_CC0pi_1)));
    FitEvents.back()->SetMode(1);
    FitEvents.push_back(new ConstructibleFitEvent(MakePDGStackEvent(IS, FS_CC1pi_1)));
    FitEvents.back()->SetMode(2);
    FitEvents.push_back(new ConstructibleFitEvent(MakePDGStackEvent(IS, FS_CC2pi_1)));
    FitEvents.back()->SetMode(3);

    for(size_t fe_it = 0; fe_it < FitEvents.size(); ++fe_it){
      cih->AddFitEvent(FitEvents[fe_it]);
      fDataHist->SetBinContent(FitEvents[fe_it]->Mode, 1.0 + float(fe_it)*0.5);
      fDataHist->SetBinError(FitEvents[fe_it]->Mode, FakeDataError);
    }

    SetupDefaultHist();
    SetCovarFromDiagonal();
    FinaliseMeasurement();
  }
  void FillEventVariables(FitEvent* nvect) { fXVar = nvect->Mode; }

  bool isSignal(FitEvent* nvect) { return true; }

  virtual ~DummySample(){
    for(size_t fe_it = 0; fe_it < FitEvents.size(); ++fe_it){
      delete FitEvents[fe_it];
    }
  }
};

static char const* SampleNames[] = {"DummySample"};
static int const NSamples = 1;

extern "C" {
int DSF_NSamples() { return NSamples; }
char const* DSF_GetSampleName(int i) {
  if (i < NSamples) {
    return SampleNames[i];
  }
  return 0;
}
MeasurementBase* DSF_GetSample(int i, void* samplekey) {
  nuiskey* sk = reinterpret_cast<nuiskey*>(samplekey);
  if (!sk) {
    return 0;
  }

  if (sk->GetS("name") != DSF_GetSampleName(i)) {
    std::cout
        << "[ERROR]: When instantiating dynamic sample. Samplekey named: "
        << sk->GetS("name")
        << ", but requested sample named: " << DSF_GetSampleName(i)
        << ". It is possible that the nuiskey object is lost in translation. "
           "Was NUISANCE and this dynamic sample manifest built with the same "
           "environment and compiler?"
        << std::endl;
  }

  if (i == 0) {
    return new DummySample(*sk);
  }
  return 0;
}
void DSF_DestroySample(MeasurementBase* mb) { delete mb; }
}
