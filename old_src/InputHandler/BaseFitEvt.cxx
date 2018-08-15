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
#ifndef __USE_NUWRO_SRW_EVENTS__
  fNuwroEvent = NULL;
#endif
#endif

#ifdef __GENIE_ENABLED__
  genie_event = NULL;
  #ifdef __DUNERWT_ENABLED__
    HasDUNERwtPolyResponses = false;
    HasDUNERwtResponses = false;
  #endif
#endif

#ifdef __NUANCE_ENABLED__
  nuance_event = NULL;
#endif

#ifdef __GiBUU_ENABLED__
  GiRead = NULL;
#endif
};

BaseFitEvt::~BaseFitEvt(){};

BaseFitEvt::BaseFitEvt(const BaseFitEvt *obj) {
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
#ifdef __DUNERWT_ENABLED__
  HasDUNERwtPolyResponses = obj->HasDUNERwtPolyResponses;
  DUNERwtPolyResponses = obj->DUNERwtPolyResponses;
  HasDUNERwtResponses = obj->HasDUNERwtResponses;
  DUNERwtResponses = obj->DUNERwtResponses;
#endif
#endif

#ifdef __NUANCE_ENABLED__
  nuance_event = obj->nuance_event;
#endif

#ifdef __GiBUU_ENABLED__
  GiRead = obj->GiRead;
#endif
};

BaseFitEvt::BaseFitEvt(BaseFitEvt const &other) {
  Mode = other.Mode;
  probe_E = other.probe_E;
  probe_pdg = other.probe_pdg;

  Weight = other.Weight;
  InputWeight = other.InputWeight;
  RWWeight = other.RWWeight;
  CustomWeight = other.CustomWeight;
  SavedRWWeight = other.SavedRWWeight;

  fSplineCoeff = other.fSplineCoeff;
  fSplineRead = other.fSplineRead;

  fGenInfo = other.fGenInfo;
  fType = other.fType;

#ifdef __NEUT_ENABLED__
  fNeutVect = other.fNeutVect;
#endif

#ifdef __NUWRO_ENABLED__
  fNuwroEvent = other.fNuwroEvent;
#ifdef __USE_NUWRO_SRW_EVENTS__
  fNuwroSRWEvent = other.fNuwroSRWEvent; ///< Pointer to Nuwro event
  fNuwroParams = other.fNuwroParams;
#endif
#endif

#ifdef __GENIE_ENABLED__
  genie_event = other.genie_event;
#ifdef __DUNERWT_ENABLED__
  HasDUNERwtPolyResponses = other.HasDUNERwtPolyResponses;
  DUNERwtPolyResponses = other.DUNERwtPolyResponses;
  HasDUNERwtResponses = other.HasDUNERwtResponses;
  DUNERwtResponses = other.DUNERwtResponses;
#endif
#endif

#ifdef __NUANCE_ENABLED__
  nuance_event = other.nuance_event;
#endif

#ifdef __GiBUU_ENABLED__
  GiRead = other.GiRead;
#endif
};

BaseFitEvt BaseFitEvt::operator=(BaseFitEvt const &other) {
  Mode = other.Mode;
  probe_E = other.probe_E;
  probe_pdg = other.probe_pdg;

  Weight = other.Weight;
  InputWeight = other.InputWeight;
  RWWeight = other.RWWeight;
  CustomWeight = other.CustomWeight;
  SavedRWWeight = other.SavedRWWeight;

  fSplineCoeff = other.fSplineCoeff;
  fSplineRead = other.fSplineRead;

  fGenInfo = other.fGenInfo;
  fType = other.fType;

#ifdef __NEUT_ENABLED__
  fNeutVect = other.fNeutVect;
#endif

#ifdef __NUWRO_ENABLED__
  fNuwroEvent = other.fNuwroEvent;
#ifdef __USE_NUWRO_SRW_EVENTS__
  fNuwroSRWEvent = other.fNuwroSRWEvent; ///< Pointer to Nuwro event
  fNuwroParams = other.fNuwroParams;
#endif
#endif

#ifdef __GENIE_ENABLED__
  genie_event = other.genie_event;
#ifdef __DUNERWT_ENABLED__
  HasDUNERwtPolyResponses = other.HasDUNERwtPolyResponses;
  DUNERwtPolyResponses = other.DUNERwtPolyResponses;
  HasDUNERwtResponses = other.HasDUNERwtResponses;
  DUNERwtResponses = other.DUNERwtResponses;
#endif
#endif

#ifdef __NUANCE_ENABLED__
  nuance_event = other.nuance_event;
#endif

#ifdef __GiBUU_ENABLED__
  GiRead = other.GiRead;
#endif
  return *this;
}

void BaseFitEvt::ResetWeight() { InputWeight = 1.0; }

double BaseFitEvt::GetWeight() {
  return InputWeight * RWWeight * CustomWeight;
};

#ifdef __NEUT_ENABLED__
void BaseFitEvt::SetNeutVect(NeutVect *v) {
  fType = kNEUT;
  fNeutVect = v;
}
#endif

#ifdef __GENIE_ENABLED__
void BaseFitEvt::SetGenieEvent(NtpMCEventRecord *ntpl) {
  fType = kGENIE;
  genie_event = ntpl;
}
#endif

#ifdef __NUANCE_ENABLED__
void BaseFitEvt::SetNuanceEvent(NuanceEvent *e) {
  fType = kNUANCE;
  nuance_event = e;
}
#endif

void BaseFitEvt::SetInputFitEvent() { fType = kINPUTFITEVENT; }

void BaseFitEvt::SetInputFitSpline() { fType = kNEWSPLINE; }

void BaseFitEvt::SetInputHepMC() { fType = kHEPMC; }
