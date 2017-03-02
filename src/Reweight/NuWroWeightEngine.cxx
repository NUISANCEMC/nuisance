#include "NuWroWeightEngine.h"

NuWroWeightEngine::NuWroWeightEngine(std::string name) {

	// Setup the NEUT Reweight engien
	fName = name;
	LOG(FIT) << "Setting up NEUT RW : " << fName << endl;

	// Create RW Engine suppressing cout
	StopTalking();

	// Create the engine
	fNuwroRW = new nuwro::rew::NuwroReWeight();

	// Get List of Veto Calcs (For Debugging)
	std::string rw_engine_list =
	    FitPar::Config().GetParS("FitWeight.fNuwroRW_veto");
	bool xsec_qel = rw_engine_list.find("nuwro_QEL") == std::string::npos;
	bool xsec_flag = rw_engine_list.find("nuwro_FlagNorm") == std::string::npos;
	bool xsec_res = rw_engine_list.find("nuwro_RES") == std::string::npos;

	// Add the RW Calcs
	if (xsec_qel)
		fNuwroRW->AdoptWghtCalc("nuwro_QEL", new nuwro::rew::NuwroReWeight_QEL);
	if (xsec_flag)
		fNuwroRW->AdoptWghtCalc("nuwro_FlagNorm",
		                        new nuwro::rew::NuwroReWeight_FlagNorm);
	if (xsec_res)  fNuwroRW->AdoptWghtCalc( "nuwro_RES",  new
		                                        nuwro::rew::NuwroReWeight_SPP );

	// Set Abs Twk Config
	fIsAbsTwk = (FitPar::Config().GetParB("setabstwk"));

	// allow cout again
	StartTalking();
};


void NuWroWeightEngine::IncludeDial(int nuisenum, double startval) {

	// Get RW Enum and name
	int rwenum = (nuisenum % 1000);
	nuwro::rew::NuwroSyst_t rwsyst = static_cast<nuwro::rew::NuwroSyst_t>(rwenum);
	std::string name = nuwro::rew::NuwroSyst::AsString(rwsyst);

	// Fill Maps
	fNuwroNameSysts[name]     = rwsyst;
	fNuwroEnumSysts[nuisenum] = rwsyst;

	// Initialize dial
	fNuwroRW->Systematics().Add( fNuwroEnumSysts[nuisenum] );

	// If Absolute
	if (fIsAbsTwk) {
		nuwro::rew::NuwroSystUncertainty::Instance()->SetUncertainty( fNuwroEnumSysts[nuisenum], 1.0, 1.0 );
	}

	// Set Value if given
	if (startval != -999.9) {
		SetDialValue(nuisenum, startval);
	}

};


void NuWroWeightEngine::SetDialValue(int nuisenum, double val) {
	// Set RW engine values
	int rwenum = (nuisenum % 1000);
	fNuwroRW->Systematics().SetSystVal(static_cast<nuwro::rew::NuwroSyst_t>(rwenum), val);
}


void NuWroWeightEngine::Reconfigure(bool silent) {
	// Hush now...
	if (silent) StopTalking();

	// Reconf
	fNuwroRW->Reconfigure();

	// Shout again
	if (silent) StartTalking();
}


double NuWroWeightEngine::CalcWeight(BaseFitEvt* evt) {

	// Skip Non GENIE
	if (evt->fType != kNUWRO) return 1.0;

	// Call Weight calculation
	double rw_weight = fNuwroRW->CalcWeight(evt->fNuwroEvent);
	
	// Return rw_weight
	return rw_weight;
}










