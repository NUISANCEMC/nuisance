#ifndef GENIEINPUTHANDLER_H
#define GENIEINPUTHANDLER_H
#include "InputHandler2.h"
#include "PlotUtils.h"

#ifdef __GENIE_ENABLED__
#include "Conventions/Units.h"
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepRecord.h"
#include "Ntuple/NtpMCEventRecord.h"
using namespace genie;
#endif


class GENIEInputHandler : public InputHandlerBase {
public:

	GENIEInputHandler(std::string const& handle, std::string const& rawinputs);
	~GENIEInputHandler(){};

	FitEvent* GetNuisanceEvent(const UInt_t entry);
	void CalcNUISANCEKinematics();
	double GetInputWeight(int entry);
	BaseFitEvt* GetBaseEvent(const UInt_t entry);
	void Print();

#ifdef __GENIE_ENABLED__
	int GetGENIEParticleStatus(genie::GHepParticle* part, int mode=0);
	int ConvertGENIEReactionCode(GHepRecord* gheprec);
#endif
	
	std::vector<TH1D*> jointfluxinputs;
	std::vector<TH1D*> jointeventinputs;
	std::vector<int> jointindexlow;
	std::vector<int> jointindexhigh;
	size_t jointindexswitch;
	bool jointinput;
	std::vector<double> jointindexscale;

#ifdef __GENIE_ENABLED__
	  GHepRecord* fGenieGHep;
	  NtpMCEventRecord* fGenieNtpl;
#endif

	TChain* fGENIETree;
};

#endif