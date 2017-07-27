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
#include "BaseFitEvt.h"

#include "FitParticle.h"
#include "GIBUUInputHandler.h"

BaseFitEvt::BaseFitEvt() {
  Mode = 0;
  probe_E = 0xdeadbeef;
  probe_pdg = 0;

  Weight = 1.0;
  InputWeight = 1.0;
  RWWeight = 1.0;
  CustomWeight = 1.0;
  SavedRWWeight = 1.0;

  fSplineCoeff = NULL;
  fSplineRead = NULL;

  fGenInfo = NULL;
  fType = 9999;

#ifdef __NEUT_ENABLED__
  fNeutVect = NULL;
#endif

#ifdef __NUWRO_ENABLED__
  fNuwroEvent = NULL;
#endif

#ifdef __GENIE_ENABLED__
  genie_event = NULL;
  genie_record = NULL;
#endif

#ifdef __NUANCE_ENABLED__
  nuance_event = NULL;
#endif

#ifdef __GiBUU_ENABLED__
  GiRead = NULL;
#endif
};

BaseFitEvt::~BaseFitEvt() {
#ifdef __NEUT_ENABLED__
  if (fNeutVect) delete fNeutVect;
#endif

#ifdef __NUWRO_ENABLED__
  if (fNuwroEvent) delete fNuwroEvent;
#endif

#ifdef __GENIE_ENABLED__
  if (genie_event) delete genie_event;
#endif

#ifdef __NUANCE_ENABLED__
  if (nuance_event) delete nuance_event;
#endif

#ifdef __GiBUU_ENABLED__
  if (GiRead) delete GiRead;
#endif
};

BaseFitEvt::BaseFitEvt(const BaseFitEvt* obj) {
  Mode = obj->Mode;
  probe_E = obj->probe_E;
  probe_pdg = obj->probe_pdg;

  Weight = obj->Weight;
  InputWeight = obj->InputWeight;
  RWWeight = obj->RWWeight;
  CustomWeight = obj->CustomWeight;
  SavedRWWeight = obj->SavedRWWeight;

  fSplineCoeff = obj->fSplineCoeff;
  fSplineRead = obj->fSplineRead;

  fGenInfo = obj->fGenInfo;
  fType = obj->fType;

#ifdef __NEUT_ENABLED__
  fNeutVect = obj->fNeutVect;
#endif

#ifdef __NUWRO_ENABLED__
  fNuwroEvent = obj->fNuwroEvent;
#endif

#ifdef __GENIE_ENABLED__
  genie_event = obj->genie_event;
#endif

#ifdef __NUANCE_ENABLED__
  nuance_event = obj->nuance_event;
#endif

#ifdef __GiBUU_ENABLED__
  GiRead = obj->GiRead;
#endif
};

void BaseFitEvt::ResetWeight() { InputWeight = 1.0; }

double BaseFitEvt::GetWeight() {
  return InputWeight * RWWeight * CustomWeight;
};

#ifdef __NEUT_ENABLED__
void BaseFitEvt::SetNeutVect(NeutVect* v) {
  fType = kNEUT;
  fNeutVect = v;

#ifdef __PROB3PP_ENABLED__
  for (size_t i = 0; i < npart; i++) {
    NeutPart* part = fNeutVect->PartInfo(i);
    if ((part->fIsAlive == false) && (part->fStatus == -1) &&
        std::count(PhysConst::pdg_neutrinos, PhysConst::pdg_neutrinos + 4,
                   part->fPID)) {
      probe_E = part->fP.T();
      probe_pdg = part->fPID;
      break;
    } else {
      continue;
    }
  }
#endif
}
#endif

#ifdef __NUWRO_ENABLED__
void BaseFitEvt::SetNuwroEvent(event* e) {
  fType = kNUWRO;
  fNuwroEvent = e;

#ifdef __PROB3PP_ENABLED__
  for (size_t i = 0; i < fNuwroEvent->in.size(); i++) {
    if (std::count(PhysConst::pdg_neutrinos, PhysConst::pdg_neutrinos + 4,
                   fNuwroEvent->in[i].pdg)) {
      probe_E = fNuwroEvent->in[i].t;
      probe_pdg = fNuwroEvent->in[i].pdg;
      break;
    }
  }
#endif
}
#endif

#ifdef __GENIE_ENABLED__
void BaseFitEvt::SetGenieEvent(NtpMCEventRecord* ntpl) {
  fType = kGENIE;
  genie_event = ntpl;

#ifdef __PROB3PP_ENABLED__
  GHepRecord* GenieGHep = static_cast<GHepRecord*>(fGenieNtpl->event);
  if (!GenieGHep) return;
  TObjArrayIter iter(GenieGHep);
  while ((p = (dynamic_cast<genie::GHepParticle*>((iter).Next())))) {
    if (!p) {
      continue;
    }

    // Get Status
    int state = GetGENIEParticleStatus(p, fNUISANCEEvent->fMode);
    if (state != genie::kIStInitialState) {
      continue;
    }
    probe_E = p->E() * 1.E3;
    probe_pdg = p->Pdg();
    break;
  }
#endif
}
#endif

#ifdef __NUANCE_ENABLED__
void BaseFitEvt::SetNuanceEvent(NuanceEvent* e) {
  fType = kNUANCE;
  nuance_event = e;
}
#endif

#ifdef __GiBUU_ENABLED__
void BaseFitEvt::SetGiBUUReader(GiBUUStdHepReader* g) {
  fType = kGiBUU;
  GiRead = g;

#ifdef __PROB3PP_ENABLED__
  for (int i = 0; i < GiRead->StdHepN; i++) {
    int state =
        GetGIBUUParticleStatus(GiRead->StdHepStatus[i], GiRead->StdHepPdg[i]);
    if (state != kInitialState) {
      continue;
    }
    if (std::count(PhysConst::pdg_neutrinos, PhysConst::pdg_neutrinos + 4,
                   GiRead->StdHepPdg[i])) {
      probe_E = GiRead->StdHepP4[i][3] * 1.E3;
      probe_pdg = GiRead->StdHepPdg[i];
      break;
    }
  }
#endif
}
#endif

void BaseFitEvt::SetInputFitEvent() { fType = kINPUTFITEVENT; }

void BaseFitEvt::SetInputFitSpline() { fType = kNEWSPLINE; }

void BaseFitEvt::SetInputHepMC() { fType = kHEPMC; }
