#ifndef HEPMCINPUTHANDLER_H
#define HEPMCINPUTHANDLER_H

#ifdef __HEPMC_ENABLED__
#include "SimpleVector.h"
#include "Flow.h"
#include "GenParticle.h"
#include "GenEvent.h"
#include "InputHandler2.h"
#include "TargetUtils.h"
#include "PlotUtils.h"

/// NEUT Input Convertor to read in NeutVects and convert to FitEvents
class HepMCTextInputHandler : public InputHandlerBase {
public:

	/// Main constructor. Can read in single or joint inputs.
	HepMCTextInputHandler(std::string const& handle, std::string const& rawinputs);
	~HepMCTextInputHandler();

	/// Returns NUISANCE Format event from entry in fNEUTTree
	FitEvent* GetNuisanceEvent(const UInt_t entry);

	/// Returns BaseEvent (just NeutVect pointer) from entry in fNEUTTree
	BaseFitEvt* GetBaseEvent(const UInt_t entry);

	/// eads fNeutVect and fills into fNUISANCEEvent.
	void CalcNUISANCEKinematics();

	/// Convert status codes to our format
	int ConvertHepMCStatus();

	/// Calculates weight if using joint inputs
	double GetInputWeight(const UInt_t entry);

	HepMC::GenEvent fHepMCEvent;
	std::ifstream fASCIIStream;
};
#endif
#endif
