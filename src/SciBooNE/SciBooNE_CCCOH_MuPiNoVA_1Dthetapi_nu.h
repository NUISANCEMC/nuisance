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

// This class corresponds to Fig 12 from PRD78 112004 (2008)

#ifndef SCIBOONE_CCCOH_MUPINOVA_1DTHETAPI_NU_H_SEEN
#define SCIBOONE_CCCOH_MUPINOVA_1DTHETAPI_NU_H_SEEN

#include "Measurement1D.h"
#include "SciBooNEUtils.h"

//******************************************************************** 
class SciBooNE_CCCOH_MuPiNoVA_1Dthetapi_nu : public Measurement1D {
//******************************************************************** 

public:

  SciBooNE_CCCOH_MuPiNoVA_1Dthetapi_nu(nuiskey samplekey);
  virtual ~SciBooNE_CCCOH_MuPiNoVA_1Dthetapi_nu() {};
  
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);
  void FillExtraHistograms(MeasurementVariableBox* vars, double weight);
  SciBooNEUtils::ModeStack *fMCStack;
  SciBooNEUtils::MainPIDStack *fPIDStack;

 private:
  double thetapi; ///<! X_Variable
  int nProtons, nPiMus, nVertex, mainIndex;
  FitParticle *mainTrack, *secondTrack;
  TH2D *muonStopEff;
  TH2D *protonEff;
};
  
#endif
