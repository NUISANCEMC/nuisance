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

#if defined(__NEUT_ENABLED__) || defined(NEUT_EVENT_ENABLED)
  fNeutVect = NULL;
#endif

#ifdef __NUWRO_ENABLED__
#ifndef __USE_NUWRO_SRW_EVENTS__
  fNuwroEvent = NULL;
#endif
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
#if defined(__NEUT_ENABLED__) || defined(NEUT_EVENT_ENABLED)
  if (fNeutVect)
    delete fNeutVect;
#endif

#ifdef __NUWRO_ENABLED__
#ifndef __USE_NUWRO_SRW_EVENTS__
  if (fNuwroEvent)
    delete fNuwroEvent;
#endif
#endif

#ifdef __GENIE_ENABLED__
  if (genie_event)
    delete genie_event;
#endif

#ifdef __NUANCE_ENABLED__
  if (nuance_event)
    delete nuance_event;
#endif

#ifdef __GiBUU_ENABLED__
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

#if defined(__NEUT_ENABLED__) || defined(NEUT_EVENT_ENABLED)
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

#if defined(__NEUT_ENABLED__) || defined(NEUT_EVENT_ENABLED)
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

  for (int i = 0; i < 6; ++i) {
    CustomWeightArray[i] = other.CustomWeightArray[i];
  }
  fSplineCoeff = other.fSplineCoeff;
  fSplineRead = other.fSplineRead;

  fGenInfo = other.fGenInfo;
  fType = other.fType;

#if defined(__NEUT_ENABLED__) || defined(NEUT_EVENT_ENABLED)
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
#endif

#ifdef __NUANCE_ENABLED__
  nuance_event = other.nuance_event;
#endif

#ifdef __GiBUU_ENABLED__
  GiRead = other.GiRead;
#endif
  return *this;
}

void BaseFitEvt::ResetWeight() {
  InputWeight = 1.0;
#ifdef __GENIE_ENABLED__
  for (int i = 0; i < 6; ++i) {
    CustomWeightArray[i] = 1.0;
  }
#endif
}

double BaseFitEvt::GetWeight() {
  return InputWeight * RWWeight * CustomWeight;
};

#if defined(__NEUT_ENABLED__) || defined(NEUT_EVENT_ENABLED)
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
