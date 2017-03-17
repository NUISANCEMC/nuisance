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

#ifndef SCIBOONEUTILS_H_SEEN
#define SCIBOONEUTILS_H_SEEN

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iostream>
#include <numeric>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <TLorentzVector.h>
#include <TH2D.h>

#include "FitParticle.h"
#include "FitEvent.h"
#include "FitLogger.h"

/*!
 *  \addtogroup Utils
 *  @{
 */

namespace FitPar {
  extern double SciBarDensity;
  extern double SciBarRecoDist;
  extern double PenetratingMuonE;
}

namespace SciBooNEUtils {

  double StoppedEfficiency(TH2D *effHist, FitParticle *nu, FitParticle *muon);
  double PenetratedEfficiency(FitParticle *nu, FitParticle *muon);
  double BetheBlochCH(double beta, double mass);
  double RangeInScintillator(FitParticle* particle, int nsteps=50);

  bool DistanceInScintillator(FitParticle* beam, FitParticle* particle);

  // Functions to break the plots into modes
  void CreateModeArray(TH1* hist, TH1* modearray[]);
  void FillModeArray(TH1* hist[], int mode, double xval, double weight = 1.0);
  void ResetModeArray(TH1* hist[]);
  void ScaleModeArray(TH1* hist[], double factor, std::string option = "");
  void DeleteModeArray(TH1* modearray[]);
  void WriteModeArray(TH1* hist[]);

  // Default to being agnostic about VA
  bool isMuPi(FitEvent *event, int VA=0);
  bool isMuPr(FitEvent *event, int VA=0);
  bool is1TRK(FitEvent *event);

  double CalcThetaPr(FitEvent *event);
  double CalcThetaPi(FitEvent *event);
  FitParticle* GetSecondaryTrack(FitEvent *event);

}
#endif
