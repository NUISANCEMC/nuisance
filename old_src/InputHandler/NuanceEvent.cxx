// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
/*!
 *  \addtogroup FitBase
 *  @{
 */
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
