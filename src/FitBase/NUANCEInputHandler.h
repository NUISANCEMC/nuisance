#ifndef NUANCEINPUTHANDLER_H
#define NUANCEINPUTHANDLER_H
#include "InputHandler2.h"
#ifdef __NUANCE_ENABLED__

#include "PlotUtils.h"

class NUANCEInputHandler : public InputHandlerBase {
public:

	NUANCEInputHandler(std::string const& handle, std::string const& rawinputs);
	~NUANCEInputHandler() {};

	FitEvent* GetNuisanceEvent(const UInt_t entry);
	void CalcNUISANCEKinematics();
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

	NuanceEvent* fNuanceEvent;
	TChain* fNUANCETree;
};
#endif
#endif