
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

#ifndef T2K2017_FakeData_H_SEEN
#define T2K2017_FakeData_H_SEEN
#include "Measurement1D.h"

//********************************************************************
class T2K2017_FakeData : public Measurement1D {
//********************************************************************

public:

  T2K2017_FakeData(nuiskey samplekey);
  virtual ~T2K2017_FakeData() {};

  //! Grab info from event
  void FillEventVariables(FitEvent *event);

  //! Fill Custom Histograms
  void FillHistograms();

  //! ResetAll
  void ResetAll();

  //! Scale
  void ScaleEvents();

  //! Norm
  void ApplyNormScale(float norm);

  //! Define this samples signal
  bool isSignal(FitEvent *nvect);

  //! Write Files
  void Write(std::string drawOpt);

  //! Get Chi2
  float GetChi2();

  //! Fill all signal flags we currently have
  void FillSignalFlags(FitEvent *event);

  void AddEventVariablesToTree();
  void AddSignalFlagsToTree();

 private:

 	TTree* fEventTree;

 	double fTMu;
 	double fCosMu;
 	double fPPip;
 	double fCosPip;
 	double fPPim;
 	double fCosPim;
 	double fPPi0;
 	double fCosPi0;
 	double fEnu;
 	int fMode;
 	int fNuPDG;


};

#endif
