#ifndef NEUTINPUTHANDLER_H
#define NEUTINPUTHANDLER_H
#include "InputHandler2.h"
#ifdef __NEUT_ENABLED__
#include "neutpart.h"
#include "neutvect.h"
#endif

#include "PlotUtils.h"

class GIBUUInputHandler : public InputHandlerBase {
public:

	GIBUUInputHandler(std::string const& handle, std::string const& rawinputs);
	~GIBUUInputHandler(){};

	FitEvent* GetNuisanceEvent(const UInt_t entry);
	int GetGIBUUParticleStatus(int status, int pdg);
	void CalcNUISANCEKinematics();
	double GetInputWeight(int entry);
	BaseFitEvt* GetBaseEvent(const UInt_t entry);
	void Print();

	GiBUUStdHepReader* fGiReader;
	TChain* fGIBUUTree;
	std::vector<TH1D*> fFluxList;
};

#endif