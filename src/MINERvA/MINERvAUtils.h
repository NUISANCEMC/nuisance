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

#ifndef MINERvAUtils_H_SEEN
#define MINERvAUtils_H_SEEN

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
#include "StandardStacks.h"

/*!
 *  \addtogroup Utils
 *  @{
 */

namespace MINERvAPar {
  extern double MINERvADensity;
  extern double MINERvARecoDist;
  extern double PenetratingMuonE;
  extern double NumRangeSteps;
}

namespace MINERvAUtils {

  double StoppedEfficiency(TH2D *effHist, FitParticle *nu, FitParticle *muon);
  double PenetratedEfficiency(FitParticle *nu, FitParticle *muon);
  double BetheBlochCH(double beta, double mass);
  double RangeInScintillator(FitParticle* particle, int nsteps=50);
  double GetEDepositOutsideRangeInScintillator(FitParticle* particle, double rangelimit);
  double GetEDepositInsideRangeInScintillator(FitParticle* particle, double rangelimit);
  bool PassesDistanceCut(FitParticle* beam, FitParticle* particle);

  int GetMainTrack(FitEvent *event, TH2D *effHist, FitParticle*& mainTrk, double& weight, bool penetrated=false);
  void GetOtherTrackInfo(FitEvent *event, int mainIndex, int& nProtons, int& nPiMus, int& nVertex, FitParticle*& secondTrk);

  double CalcThetaPr(FitEvent *event, FitParticle *main, FitParticle *second, bool penetrated=false);
  double CalcThetaPi(FitEvent *event, FitParticle *second);

  /// Break down the plots as in the MINERvA CCQE Papers
  class ModeStack : public StackBase {
  public:

    /// Main constructor listing true mode categories.  
    ModeStack(std::string name, std::string title, TH1* hist);

    /// List to convert Modes to Index.      
    /// Should be kept in sync with constructor.        
    int ConvertModeToIndex(int mode);
    /// Fill from given mode integer         
    void Fill(int mode, double x, double y = 1.0, double z = 1.0, double weight = 1.0);
    /// Extracts Mode from FitEvent and fills
    void Fill(FitEvent* evt, double x, double y = 1.0, double z = 1.0, double weight = 1.0);
    /// Extracts Mode from BaseFitEvt        
    void Fill(BaseFitEvt* evt, double x, double y = 1.0, double z = 1.0, double weight = 1.0);

  };

  /// Break down the plots in terms of 1-N pion contributions
  class PionStack : public StackBase {
  public:

    /// Main constructor listing true mode categories.
    PionStack(std::string name, std::string title, TH1* hist);

    /// List to convert Pions to Index.
    /// Should be kept in sync with constructor.
    int ConvertNPionsToIndex(int npions);

    /// Fill from given mode integer
    void Fill(int npions, double x, double y = 1.0, double z = 1.0, double weight = 1.0);
  };
  

}
#endif
