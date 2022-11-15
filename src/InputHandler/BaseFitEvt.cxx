// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
  for (int i = 0; i < 6; ++i) {
    CustomWeightArray[i] = 1.0;
  }

  fSplineCoeff = NULL;
  fSplineRead = NULL;

  fGenInfo = NULL;
  fType = 9999;

#if defined(NEUT_ENABLED) || defined(NEUT_EVENT_ENABLED)
  fNeutVect = NULL;
#endif

#ifdef NuWro_ENABLED
  fNuwroEvent = NULL;
#endif

#ifdef GENIE_ENABLED
  genie_event = NULL;
  genie_record = NULL;
#endif

#ifdef NUANCE_ENABLED
  nuance_event = NULL;
#endif

#ifdef GiBUU_ENABLED
  GiRead = NULL;
#endif
};

BaseFitEvt::~BaseFitEvt() {
#if defined(NEUT_ENABLED) || defined(NEUT_EVENT_ENABLED)
  if (fNeutVect)
    delete fNeutVect;
#endif

#ifdef NuWro_ENABLED
  if (fNuwroEvent)
    delete fNuwroEvent;
#endif

#ifdef GENIE_ENABLED
  if (genie_event)
    delete genie_event;
#endif

#ifdef NUANCE_ENABLED
  if (nuance_event)
    delete nuance_event;
#endif

#ifdef GiBUU_ENABLED
  if (GiRead)
    delete GiRead;
#endif
};

BaseFitEvt::BaseFitEvt(const BaseFitEvt *obj) {
  Mode = obj->Mode;
  probe_E = obj->probe_E;
  probe_pdg = obj->probe_pdg;

  Weight = obj->Weight;
  InputWeight = obj->InputWeight;
  RWWeight = obj->RWWeight;
  SavedRWWeight = obj->SavedRWWeight;
  CustomWeight = obj->CustomWeight;
  for (int i = 0; i < 6; ++i) {
    CustomWeightArray[i] = obj->CustomWeightArray[i];
  }

  fSplineCoeff = obj->fSplineCoeff;
  fSplineRead = obj->fSplineRead;

  fGenInfo = obj->fGenInfo;
  fType = obj->fType;

#if defined(NEUT_ENABLED) || defined(NEUT_EVENT_ENABLED)
  fNeutVect = obj->fNeutVect;
#endif

#ifdef NuWro_ENABLED
  fNuwroEvent = obj->fNuwroEvent;
#endif

#ifdef GENIE_ENABLED
  genie_event = obj->genie_event;
#endif

#ifdef NUANCE_ENABLED
  nuance_event = obj->nuance_event;
#endif

#ifdef GiBUU_ENABLED
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
  for (int i = 0; i < 6; ++i) {
    CustomWeightArray[i] = other.CustomWeightArray[i];
  }

  fSplineCoeff = other.fSplineCoeff;
  fSplineRead = other.fSplineRead;

  fGenInfo = other.fGenInfo;
  fType = other.fType;

#if defined(NEUT_ENABLED) || defined(NEUT_EVENT_ENABLED)
  fNeutVect = other.fNeutVect;
#endif

#ifdef NuWro_ENABLED
  fNuwroEvent = other.fNuwroEvent;
#endif

#ifdef GENIE_ENABLED
  genie_event = other.genie_event;
#endif

#ifdef NUANCE_ENABLED
  nuance_event = other.nuance_event;
#endif

#ifdef GiBUU_ENABLED
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

  for (int i = 0; i < 6; ++i) {
    CustomWeightArray[i] = other.CustomWeightArray[i];
  }
  fSplineCoeff = other.fSplineCoeff;
  fSplineRead = other.fSplineRead;

  fGenInfo = other.fGenInfo;
  fType = other.fType;

#if defined(NEUT_ENABLED) || defined(NEUT_EVENT_ENABLED)
  fNeutVect = other.fNeutVect;
#endif

#ifdef NuWro_ENABLED
  fNuwroEvent = other.fNuwroEvent;
#endif

#ifdef GENIE_ENABLED
  genie_event = other.genie_event;
#endif

#ifdef NUANCE_ENABLED
  nuance_event = other.nuance_event;
#endif

#ifdef GiBUU_ENABLED
  GiRead = other.GiRead;
#endif
  return *this;
}

void BaseFitEvt::ResetWeight() {
  InputWeight = 1.0;
#ifdef GENIE_ENABLED
  for (int i = 0; i < 6; ++i) {
    CustomWeightArray[i] = 1.0;
  }
#endif
}

double BaseFitEvt::GetWeight() {
  return InputWeight * RWWeight * CustomWeight;
};

#if defined(NEUT_ENABLED) || defined(NEUT_EVENT_ENABLED)
void BaseFitEvt::SetNeutVect(NeutVect *v) {
  fType = kNEUT;
  fNeutVect = v;
}
#endif

#ifdef GENIE_ENABLED
void BaseFitEvt::SetGenieEvent(NtpMCEventRecord *ntpl) {
  fType = kGENIE;
  genie_event = ntpl;
}
#endif

#ifdef NUANCE_ENABLED
void BaseFitEvt::SetNuanceEvent(NuanceEvent *e) {
  fType = kNUANCE;
  nuance_event = e;
}
#endif

void BaseFitEvt::SetInputFitEvent() { fType = kINPUTFITEVENT; }

void BaseFitEvt::SetInputFitSpline() { fType = kNEWSPLINE; }

void BaseFitEvt::SetInputHepMC() { fType = kNuHepMC; }
