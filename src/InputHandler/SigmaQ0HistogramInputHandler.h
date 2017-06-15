#ifndef SigmaQ0HistogramInputHandler_H
#define SigmaQ0HistogramInputHandler_H

#ifdef __NEUT_ENABLED__
#include "InputHandler.h"
#include "TargetUtils.h"
#include "neutpart.h"
#include "neutvect.h"
#include "PlotUtils.h"
#include "TTreePerfStats.h"
#include "nefillverC.h"
#include "necardC.h"
#include "neutmodelC.h"
#include "neutparamsC.h"
#include "neworkC.h"
#include "fsihistC.h"
#include "neutcrsC.h"
#include "neutvect.h"
#include "neutpart.h"
#include "neutfsipart.h"
#include "neutfsivert.h"
#include "neutrootTreeSingleton.h"
#include "NModeDefn.h"
#include "NSyst.h"
#include "NFortFns.h" // Contains all the NEUT common blocks

#ifdef __NEUT_NUCFSI_ENABLED__
#include "nucleonfsihistC.h"
#include "neutnucfsivert.h"
#include "neutnucfsistep.h"
#endif

class SigmaQ0HistogramInputHandler : public InputHandlerBase {
public:

	/// Main constructor. Can read in single or joint inputs.
	SigmaQ0HistogramInputHandler(std::string const& handle, std::string const& rawinputs);
	~SigmaQ0HistogramInputHandler();

	/// Returns NUISANCE Format event from entry in fNEUTTree
	FitEvent* GetNuisanceEvent(const UInt_t entry, const bool lightweight);

	/// eads fNeutVect and fills into fNUISANCEEvent.
	FitEvent CreateNuisanceEvent(double q0, double sig);

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

};

#endif
#endif
