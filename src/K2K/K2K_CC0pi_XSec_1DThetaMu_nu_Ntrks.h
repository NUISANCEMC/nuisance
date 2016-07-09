// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef K2K_CC0pi_1DThetaMu_ALLTRACKS_H_SEEN
#define K2K_CC0pi_1DThetaMu_ALLTRACKS_H_SEEN
#include "Measurement1D.h"

//********************************************************************     
class K2K_CC0pi_XSec_1DThetaMu_nu_Ntrks : public Measurement1D {
//********************************************************************     
public:

  K2K_CC0pi_XSec_1DThetaMu_nu_Ntrks(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~K2K_CC0pi_XSec_1DThetaMu_nu_Ntrks() {};
  
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);
  void ScaleEvents();
  void ResetAll();
  void FillHistograms();
 private:

  bool bad_particle;
  TH1D* ccqelike;
  double Pmu;
  int ncharged;
  bool matchnorm;
  double DelPhi;
  double CosThetaMu;

  TH1D* K2KI_holder;
  TH1D* K2KIIa_holder;
  bool finished_fit;
  unsigned int mcSignalEvt;
  unsigned int dataSignalEvt;
};
  
#endif
