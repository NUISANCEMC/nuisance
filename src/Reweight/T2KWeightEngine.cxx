#include "T2KWeightEngine.h"

T2KWeightEngine::T2KWeightEngine(std::string name) {
#ifdef __T2KREW_ENABLED__

	// Setup the NEUT Reweight engien
	fCalcName = name;
	LOG(FIT) << "Setting up T2K RW : " << fCalcName << endl;

	// Create RW Engine suppressing cout
	StopTalking();

	// Create Main RW Engine
	fT2KRW = new t2krew::T2KReWeight();

	// Setup Sub RW Engines (Only activated for neut and niwg)
	fT2KNeutRW = new t2krew::T2KNeutReWeight();
	fT2KNIWGRW = new t2krew::T2KNIWGReWeight();

	fT2KRW->AdoptWghtEngine("fNeutRW", fT2KNeutRW);
	fT2KRW->AdoptWghtEngine("fNIWGRW", fT2KNIWGRW);

	fT2KRW->Reconfigure();

	// allow cout again
	StartTalking();

	// Set Abs Twk Config
	fIsAbsTwk = (FitPar::Config().GetParB("setabstwk"));
	
#else	
	ERR(FTL) << "T2K RW NOT ENABLED" << std::endl;
#endif
};

void T2KWeightEngine::IncludeDial(std::string name, double startval) {
#ifdef __T2KREW_ENABLED__
	// Get NEUT Syst.
	t2krew::T2KSyst_t gensyst = t2krew::T2KSyst::FromString(name);
	int nuisenum = Reweight::ConvDial(name, kT2K);

	// Fill Maps
	int index = fValues.size();
	fValues.push_back(0.0);
	fT2KSysts.push_back(gensyst);

	fEnumIndex[nuisenum] = index;
	fNameIndex[name] = index;

	// Initialize dial
	fT2KRW->Systematics().Include(gensyst);

	// If Absolute
	if (fIsAbsTwk) {
		fT2KRW->Systematics().SetAbsTwk(gensyst);
	}

	// Set Value if given
	if (startval != -999.9) {
		SetDialValue(nuisenum, startval);
	}
#endif
}

void T2KWeightEngine::SetDialValue(int nuisenum, double val) {
#ifdef __T2KREW_ENABLED__
	fValues[fEnumIndex[nuisenum]] = val;
	fT2KRW->Systematics().SetTwkDial(fT2KSysts[fEnumIndex[nuisenum]], val);
#endif
}

void T2KWeightEngine::SetDialValue(std::string name, double val) {
#ifdef __T2KREW_ENABLED__
	fValues[fNameIndex[name]] = val;
	fT2KRW->Systematics().SetTwkDial(fT2KSysts[fNameIndex[name]], val);
#endif
}


void T2KWeightEngine::Reconfigure(bool silent) {
#ifdef __T2KREW_ENABLED__
	// Hush now...
	if (silent) StopTalking();

	// Reconf
	StopTalking();
	fT2KRW->Reconfigure();
	StartTalking();

	// Shout again
	if (silent) StartTalking();
#endif
}


double T2KWeightEngine::CalcWeight(BaseFitEvt* evt) {
	double rw_weight = 1.0;

#ifdef __T2KREW_ENABLED__
	// Skip Non GENIE
	if (evt->fType != kNEUT) return 1.0;

	// Hush now
	StopTalking();

	// Get Weight For NEUT
	rw_weight = fT2KRW->CalcWeight(evt->fNeutVect);

	// Speak Now
	StartTalking();
#endif

	// Return rw_weight
	return rw_weight;
}










