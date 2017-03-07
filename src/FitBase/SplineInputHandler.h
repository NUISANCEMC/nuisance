#ifndef SPLINE_INPUTHANDLER_H
#define SPLINE_INPUTHANDLER_H

#include "InputHandler2.h"
#include "FitEvent.h"
#include "PlotUtils.h"

class SplineInputHandler : public InputHandlerBase {
public:

	SplineInputHandler(std::string const& handle, std::string const& rawinputs);
	~SplineInputHandler(){};

	FitEvent* GetNuisanceEvent(const UInt_t entry);
	double GetInputWeight(int entry);
	BaseFitEvt* GetBaseEvent(const UInt_t entry);
	void Print();

	bool save_extra;
	TChain* fFitEventTree;
	SplineReader* fSplRead;
	float fSplineCoeff[1000];
	TTree* fSplTree;

	std::vector<float> fStartingWeights;

	std::vector< std::vector<float> > fAllSplineCoeff;
	int fNPar;
};
#endif
