#ifdef __NUANCE_ENABLED__
#include "NuanceEvent.h"

void NuanceEvent::SetBranchAddresses(TTree* tn) {

	tn->SetBranchAddress("neutrino", &neutrino);
	tn->SetBranchAddress("target", &target);
	tn->SetBranchAddress("channel", &channel);
	tn->SetBranchAddress("iniQ", &iniQ);
	tn->SetBranchAddress("finQ", &finQ);
	tn->SetBranchAddress("lepton0", &lepton0);
	tn->SetBranchAddress("polar", &polar);
	tn->SetBranchAddress("qsq", &qsq);

	tn->SetBranchAddress("w", &w);
	tn->SetBranchAddress("x", &x);
	tn->SetBranchAddress("y", &y);

	tn->SetBranchAddress("p_neutrino", p_neutrino);
	tn->SetBranchAddress("p_targ", p_targ);
	tn->SetBranchAddress("vertex", vertex);
	tn->SetBranchAddress("start", start);
	tn->SetBranchAddress("depth", &depth);
	tn->SetBranchAddress("flux", &flux);

	tn->SetBranchAddress("n_leptons", &n_leptons);
	tn->SetBranchAddress("p_ltot", p_ltot);
	tn->SetBranchAddress("lepton", lepton);
	tn->SetBranchAddress("p_lepton", p_lepton);

	tn->SetBranchAddress("n_hadrons", &n_hadrons);
	tn->SetBranchAddress("p_htot", p_htot);
	tn->SetBranchAddress("hadron", hadron);
	tn->SetBranchAddress("p_hadron", p_hadron);

}
#endif