#include "HistogramInputHandler.h"

#include "InputUtils.h"

TH1 *HistoInputHandler::GetHistogram(int i) {
  if (size_t(i) >= fHistos.size()) {
    QTHROW("Requested histogram, index " << i << ", but only specified "
                                        << fHistos.size() << " input histos.");
  }
  return fHistos[i];
}
std::vector<TH1 *> HistoInputHandler::GetHistograms(int i, int j) {
  size_t from = (i < 0) ? 0 : i;
  size_t to = (j < 0) ? fHistos.size() : j;
  if (j <= i) {
    QTHROW("Lower bound of GetHistograms range is larger than or equal to the "
          "upper bound: ["
          << i << ", " << j << "].");
  }

  std::vector<TH1 *> rtnv;
  for (size_t it = from; it < to; ++it) {
    rtnv.push_back(GetHistogram(it));
  }
  return rtnv;
}

HistoInputHandler::HistoInputHandler(std::string const &handle,
                                     std::string const &rawinputs) {
  QLOG(SAM, "Creating HistoInputHandler : " << handle);

  // Run a joint input handling
  fName = handle;
  fNEvents = 1;
  fEventType = kHISTO;

  fFluxHist = new TH1D("flux", "dummy", 1, 1, 2);
  fFluxHist->SetBinContent(1, 1);
  fEventHist = new TH1D("event", "dummy", 1, 1, 2);
  fEventHist->SetBinContent(1, 1);

  // Loop over all inputs and grab flux, eventhist, and nevents
  std::vector<std::string> inputs = InputUtils::ParseInputFileList(rawinputs);
  for (size_t inp_it = 0; inp_it < inputs.size(); ++inp_it) {
    // Open File for histogram access
    QLOG(SAM, "Reading histograms from descriptor " << inputs[inp_it]);

    std::vector<TH1 *> histos = PlotUtils::GetTH1sFromRootFile(inputs[inp_it]);

    for (size_t h_it = 0; h_it < histos.size(); ++h_it) {
      QLOG(SAM, "Read " << histos[h_it]->GetName());
      fHistos.push_back(histos[h_it]);
    }
  }
};

void HistoInputHandler::Print() {}
