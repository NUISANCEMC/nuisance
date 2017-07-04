#ifndef FITEVENT_INPUTHANDLER_H
#define FITEVENT_INPUTHANDLER_H
/*!
 *  \addtogroup InputHandler
 *  @{
 */
#include "InputHandler.h"
#include "FitEvent.h"
#include "PlotUtils.h"

/// Class to read in NUISANCE FitEvents that have been saved to tree
class FitEventInputHandler : public InputHandlerBase {
public:

	/// Standard constructor given name and inputs
	FitEventInputHandler(std::string const& handle, std::string const& rawinputs);
	virtual ~FitEventInputHandler();

	/// Create a TTree Cache to speed up file read
	void CreateCache();

	/// Remove TTree Cache to save memory
	void RemoveCache();

	/// Returns NUISANCE FitEvent from the TTree. If lightweight does nothing.
	FitEvent* GetNuisanceEvent(const UInt_t entry, const bool lightweight=false);

	/// Alongside InputWeight also returns any saved RWWeights
	double GetInputWeight(int entry);

	/// Print out event information
	void Print();

	TChain* fFitEventTree; ///< TTree from FitEvent file.

	int fReadNParticles;
	double fReadParticleMom[400][4];
	UInt_t fReadParticleState[400];
	int fReadParticlePDG[400];

};
/*! @} */
#endif
