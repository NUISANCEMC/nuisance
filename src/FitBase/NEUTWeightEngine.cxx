#include "NEUTWeightEngine.h"

NEUTWeightEngine::NEUTWeightEngine(std::string name) {

	// Setup the NEUT Reweight engien
	fName = name;
	LOG(FIT) << "Setting up NEUT RW : " << fName << endl;

	// Create RW Engine suppressing cout
	StopTalking();
	fNeutRW = new neut::rew::NReWeight();
	TDirectory* olddir = gDirectory;

	// get list of vetoed calc engines (just for debug really)
	std::string rw_engine_list = FitPar::Config().GetParS("FitWeight.fNeutRW_veto");
	bool xsec_ccqe = rw_engine_list.find("xsec_ccqe") == std::string::npos;
	bool xsec_res = rw_engine_list.find("xsec_res") == std::string::npos;
	bool xsec_ccres = rw_engine_list.find("xsec_ccres") == std::string::npos;
	bool xsec_coh = rw_engine_list.find("xsec_coh") == std::string::npos;
	bool xsec_dis = rw_engine_list.find("xsec_dis") == std::string::npos;
	bool xsec_ncel = rw_engine_list.find("xsec_ncel") == std::string::npos;
	bool xsec_nc = rw_engine_list.find("xsec_nc") == std::string::npos;
	bool xsec_ncres = rw_engine_list.find("xsec_ncres") == std::string::npos;
	bool nucl_casc = rw_engine_list.find("nucl_casc") == std::string::npos;
	bool nucl_piless = rw_engine_list.find("nucl_piless") == std::string::npos;

	// Activate each calc engine
	if (xsec_ccqe)
		fNeutRW->AdoptWghtCalc("xsec_ccqe", new neut::rew::NReWeightNuXSecCCQE);
	if (xsec_res)
		fNeutRW->AdoptWghtCalc("xsec_res", new neut::rew::NReWeightNuXSecRES);
	if (xsec_ccres)
		fNeutRW->AdoptWghtCalc("xsec_ccres", new neut::rew::NReWeightNuXSecCCRES);
	if (xsec_coh)
		fNeutRW->AdoptWghtCalc("xsec_coh", new neut::rew::NReWeightNuXSecCOH);
	if (xsec_dis)
		fNeutRW->AdoptWghtCalc("xsec_dis", new neut::rew::NReWeightNuXSecDIS);
	if (xsec_ncel)
		fNeutRW->AdoptWghtCalc("xsec_ncel", new neut::rew::NReWeightNuXSecNCEL);
	if (xsec_nc)
		fNeutRW->AdoptWghtCalc("xsec_nc", new neut::rew::NReWeightNuXSecNC);
	if (xsec_ncres)
		fNeutRW->AdoptWghtCalc("xsec_ncres", new neut::rew::NReWeightNuXSecNCRES);
	if (nucl_casc)
		fNeutRW->AdoptWghtCalc("nucl_casc", new neut::rew::NReWeightCasc);
	if (nucl_piless)
		fNeutRW->AdoptWghtCalc("nucl_piless", new neut::rew::NReWeightNuclPiless);
	fNeutRW->Reconfigure();
	olddir->cd();

	// Set Abs Twk Config
	fIsAbsTwk = (FitPar::Config().GetParB("setabstwk"));
	
	// allow cout again
	StartTalking();
};


void NEUTWeightEngine::IncludeDial(int nuisenum, double startval){

	// Get RW Enum and name
	int rwenum = (nuisenum % 1000);
	neut::rew::NSyst_t rwsyst = static_cast<neut::rew::NSyst_t>(rwenum);
	std::string name = NSyst::AsString(rwsyst);

	// Fill Maps
	fNeutNameSysts[name]     = rwsyst;
	fNeutEnumSysts[nuisenum] = rwsyst;
	
	// Initialize dial
	fNeutRW->Systematics().Init( fNeutEnumSysts[nuisenum] );

	// If Absolute
	if (fIsAbsTwk){
		NSystUncertainty::Instance()->SetUncertainty( fNeutEnumSysts[nuisenum], 1.0, 1.0 );
	} 

	// Set Value if given
	if (startval != -999.9){
		SetDialValue(nuisenum, startval);
	}

};


void NEUTWeightEngine::SetDialValue(int nuisenum, double val){
	// Set RW engine values
	int rwenum = (nuisenum % 1000);
	fNeutRW->Systematics().Set(static_cast<neut::rew::NSyst_t>(rwenum), val);
}


void NEUTWeightEngine::Reconfigure(bool silent){
	// Hush now...
	 if (silent) StopTalking();

	// Reconf
	fNeutRW->Reconfigure();

	// Shout again
	 if (silent) StartTalking();
}


double NEUTWeightEngine::CalcWeight(BaseFitEvt* evt){

	// Skip Non GENIE
	if (evt->fType != kNEUT) return 1.0;

	StopTalking();

	// Check if event has changed incase common blocks need filling
	// if (evt->eventid != fLastEventID or evt != fLastEventPointer){
		
		// Fill NEUT Common blocks
		GeneratorUtils::FillNeutCommons(evt->fNeutVect);

		// Save last one
		// fLastEventID = evt->eventid;
		// fLastEventPointer = evt;
	// }

	// Call Weight calculation
    double rw_weight = fNeutRW->CalcWeight();
    StartTalking();
    // Return rw_weight
    return rw_weight;
}










