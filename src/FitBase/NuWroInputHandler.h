#ifndef NuWroINPUTHANDLER_H
#define NuWroINPUTHANDLER_H
#include "InputHandler2.h"


#include "PlotUtils.h"

class NuWroInputHandler : public InputHandlerBase {
public:

	NuWroInputHandler(std::string const& handle, std::string const& rawinputs);
	~NuWroInputHandler(){};

	FitEvent* GetNuisanceEvent(const UInt_t entry);
	void CalcNUISANCEKinematics();
	double GetInputWeight(int entry);
	BaseFitEvt* GetBaseEvent(const UInt_t entry);
	void Print();

	void ProcessNuWroInputFlux(const std::string file);
	void AddNuWroParticle(FitEvent* evt, const particle& p, int state);

	std::vector<TH1D*> jointfluxinputs;
	std::vector<TH1D*> jointeventinputs;
	std::vector<int> jointindexlow;
	std::vector<int> jointindexhigh;
	size_t jointindexswitch;
	bool jointinput;
	std::vector<double> jointindexscale;

	TChain* fNuWroTree;

#ifdef __NUWRO_ENABLED__
  int ConvertNuwroMode (event * e);
#endif

#ifdef __NUWRO_ENABLED__
    event* fNuWroEvent;  //!< Pointer to NuWro Events (Set to bool if NUWRO disabled)
#endif

};

#endif