#ifndef HISTOINPUTHANDLER_H
#define HISTOINPUTHANDLER_H
/*!
 *  \addtogroup InputHandler
 *  @{
 */
#include "InputHandler.h"
#include "PlotUtils.h"
#include "StdHepEvt.h"

/// Histogram input handler to read root histograms
class HistoInputHandler : public InputHandlerBase {
 public:
  /// Standard constructor given name and inputs
  HistoInputHandler(std::string const& handle, std::string const& rawinputs);
  ~HistoInputHandler(){};

  /// Returns NUISANCE Format Event from GiReader
  FitEvent* GetNuisanceEvent(const UInt_t entry,
                             const bool lightweight = false) {
    return NULL;
  }

  size_t NHistograms() { return fHistos.size(); }
  TH1* GetHistogram(int i = -1);
  std::vector<TH1*> GetHistograms(int i = -1, int j = -1);

  /// Print event information
  void Print();

  std::vector<TH1*> fHistos;
};
#endif
