#ifndef FITEVENT_INPUTHANDLER_H
#define FITEVENT_INPUTHANDLER_H

#include "InputHandler2.h"
#include "FitEvent.h"
#include "PlotUtils.h"

class FitEventInputHandler : public InputHandlerBase {
public:

	FitEventInputHandler(std::string const& handle, std::string const& rawinputs);
	~FitEventInputHandler(){};

	FitEvent* GetNuisanceEvent(const UInt_t entry);
	double GetInputWeight(int entry);
	BaseFitEvt* GetBaseEvent(const UInt_t entry);
	void Print();

	std::vector<TH1D*> jointfluxinputs;
	std::vector<TH1D*> jointeventinputs;
	std::vector<int> jointindexlow;
	std::vector<int> jointindexhigh;
	size_t jointindexswitch;
	bool jointinput;
	std::vector<double> jointindexscale;

	bool save_extra;
	TChain* fFitEventTree;
};

#endif
