#ifndef SigmaQ0HistogramInputHandler_H
#define SigmaQ0HistogramInputHandler_H

#include "InputHandler.h"
#include "TargetUtils.h"
#include "PlotUtils.h"
#include "TRandom3.h"

class SigmaQ0HistogramInputHandler : public InputHandlerBase {
public:

	/// Main constructor. Can read in single or joint inputs.
	SigmaQ0HistogramInputHandler(std::string const& handle, std::string const& rawinputs);
	~SigmaQ0HistogramInputHandler();

	/// Returns NUISANCE Format event from entry in fNEUTTree
	FitEvent* GetNuisanceEvent(const UInt_t entry, const bool lightweight);

	/// eads fNeutVect and fills into fNUISANCEEvent.
	void FillNuisanceEvent(double q0, double sig);

	double ThrowQ0();

	double fEnergy;
	double fTheta;
	int fQ0Column;
	int fSigmaColumn;
	int fBeamPDG;
	std::string fDelim;
	double fScaling;
	std::string fFilePath;
	std::vector<FitEvent> fNuisanceEventList;
	
	bool fApplyInterpolation;
	TGraph* fInputGraph;
	FitEvent fCurEvent;
	bool fUseAcceptReject;
	TRandom3 fRandom;
	double fMaxValue;
	double fMinX;
	double fMaxX;


};

#endif
