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

#ifndef __FIT_WEIGHT_H_SEEN__
#define __FIT_WEIGHT_H_SEEN__

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "FitEvent.h"
#include "TF1.h"
#include "TObject.h"
using namespace std;

#ifdef __T2KREW_ENABLED__
#include "T2KGenieReWeight.h"
#include "T2KNIWGReWeight.h"
#include "T2KNIWGUtils.h"
#include "T2KNeutReWeight.h"
#include "T2KNeutUtils.h"
#include "T2KReWeight.h"
using namespace t2krew;
#endif

#ifdef __NIWG_ENABLED__
#include "NIWGReWeight.h"
#include "NIWGReWeight1piAngle.h"
#include "NIWGReWeight2010a.h"
#include "NIWGReWeight2012a.h"
#include "NIWGReWeight2014a.h"
#include "NIWGReWeightDeltaMass.h"
#include "NIWGReWeightEffectiveRPA.h"
#include "NIWGReWeightHadronMultSwitch.h"
#include "NIWGReWeightMEC.h"
#include "NIWGReWeightPiMult.h"
#include "NIWGReWeightProtonFSIbug.h"
#include "NIWGReWeightRPA.h"
#include "NIWGReWeightSpectralFunc.h"
#include "NIWGReWeightSplineEnu.h"
#include "NIWGSyst.h"
#include "NIWGSystUncertainty.h"
#endif

#ifdef __NEUT_ENABLED__
#include "NReWeight.h"
#include "NReWeightCasc.h"
#include "NReWeightNuXSecCCQE.h"
#include "NReWeightNuXSecCCRES.h"
#include "NReWeightNuXSecCOH.h"
#include "NReWeightNuXSecDIS.h"
#include "NReWeightNuXSecNC.h"
#include "NReWeightNuXSecNCEL.h"
#include "NReWeightNuXSecNCRES.h"
#include "NReWeightNuXSecRES.h"
#include "NReWeightNuclPiless.h"
#include "NSyst.h"
#include "NSystUncertainty.h"
#include "neutpart.h"
#include "neutvect.h"
#endif

#ifdef __NUWRO_ENABLED__
#include "event1.h"
#endif

#ifdef __NUWRO_REWEIGHT_ENABLED__
#include "NuwroReWeight.h"
#include "NuwroReWeight_FlagNorm.h"
#include "NuwroReWeight_QEL.h"
#include "NuwroReWeight_SPP.h"
#include "NuwroSyst.h"
#include "NuwroSystUncertainty.h"
#endif

#ifdef __GENIE_ENABLED__
#include "EVGCore/EventRecord.h"
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepRecord.h"
#include "GSyst.h"
#include "GSystUncertainty.h"
#include "Ntuple/NtpMCEventRecord.h"
#include "ReWeight/GReWeight.h"
#include "ReWeight/GReWeightAGKY.h"
#include "ReWeight/GReWeightDISNuclMod.h"
#include "ReWeight/GReWeightFGM.h"
#include "ReWeight/GReWeightFZone.h"
#include "ReWeight/GReWeightINuke.h"
#include "ReWeight/GReWeightNonResonanceBkg.h"
#include "ReWeight/GReWeightNuXSecCCQE.h"
#include "ReWeight/GReWeightNuXSecCCQEvec.h"
#include "ReWeight/GReWeightNuXSecCCRES.h"
#include "ReWeight/GReWeightNuXSecCOH.h"
#include "ReWeight/GReWeightNuXSecDIS.h"
#include "ReWeight/GReWeightNuXSecNC.h"
#include "ReWeight/GReWeightNuXSecNCEL.h"
#include "ReWeight/GReWeightNuXSecNCRES.h"
#include "ReWeight/GReWeightResonanceDecay.h"
using namespace genie;
using namespace genie::rew;
#endif

#include "FitSplineHead.h"
#include "GeneratorUtils.h"
#include "TCanvas.h"
#include "TGraph2D.h"

//#include "GeneratorWeightBase.h"
//#include "GeneratorNEUTReweight.h"

//******************************************
class FitWeight {
  //******************************************

 public:
  FitWeight(std::string name, std::string inputfile);  // Open a fit result
                                                          // file and grab the
                                                          // ttree and setup
                                                          // like that.
  FitWeight(std::string name = "FitWeight");
  ~FitWeight(){};

  int GetDialEnum(std::string name, int type = -1);
  int GetRWEnum(int dial_enum);

  void IncludeDial(std::string name, int type, double startval = 0.0);

  void SetDialValue(std::string name, double val);
  void SetDialValue(int dial, double val);

  double GetDialValue(std::string name, std::string option = "");
  double GetDialValue(int dial);

  unsigned int GetDialPos(std::string name);
  unsigned int GetDialPos(int this_enum);

  std::string GetDialType(int this_enum);

  bool HasDialChanged();

  void UpdateWeightEngine(const double* x);
  void SetAllDials(const double* x, int npt);
  void GetAllDials(double* x, int npt);
  bool DialIncluded(std::string name);
    
  std::vector<std::string> GetDialNames();
  std::vector<int> GetDialEnums();
  std::vector<double> GetDialValues();

  double GetSampleNorm(std::string samplename);

  double CalcWeight(BaseFitEvt* evt);
  double GetSampleLikelihoodWeight(std::string samplename);
  
  void PrintState();

  void Reconfigure(bool silent = false);

// GENERATOR SPECIFIC RW OBJECTS

#ifdef __NEUT_ENABLED__  // --- NEUT BLOCK
  void SetupNeutRW();
  neut::rew::NReWeight* fNeutRW;
#endif

#ifdef __NIWG_ENABLED__  // --- NIWG BLOCK
  void SetupNIWGRW();
  niwg::rew::NIWGReWeight* fNIWGRW;
#endif

#ifdef __NUWRO_REWEIGHT_ENABLED__  // --- NUWRO BLOCK
  void SetupNuwroRW();
  nuwro::rew::NuwroReWeight* fNuwroRW;
#endif

#ifdef __T2KREW_ENABLED__  // --- T2KRW BLOCK
  void SetupT2KRW();
  t2krew::T2KReWeight* fT2KRW;  //!< T2K RW Object
  t2krew::T2KNeutReWeight* fT2KNeutRW;
  t2krew::T2KNIWGReWeight* fT2KNIWGRW;
#endif

#ifdef __GENIE_ENABLED__  // --- GENIE BLOCK
  void SetupGenieRW();
  genie::rew::GReWeight* fGenieRW;  //!< Genie RW Object
#endif

  // SPLINE FUNCTIONS ------------
  inline FitSplineHead* GetSplineHeader() { return fSplineHead; };
  void SetupEventCoeff(BaseFitEvt* event);
  void SetupSpline(std::string dialname, std::string splinename,
                   std::string pointsdef);
  double CalcSplineWeight(BaseFitEvt* evt);
  void ReadSplineHead(FitSplineHead* splhead);
  void ResetSplines();
  void GenSplines(BaseFitEvt* evt, bool save_graph = false);

  void Fit1DSplineCoeff(BaseFitEvt* event, FitSpline* spl, double nom,
                        bool save_graph);
  void Fit2DSplineCoeff(BaseFitEvt* event, FitSpline* spl, double nom,
                        bool save_graph);
  void FitNDSplineCoeff(BaseFitEvt* event, FitSpline* spl, double norm,
                        bool save_graph) {
    (void)event;
    (void)spl;
    (void)norm;
    (void)save_graph;
  };

  bool HasRWDialChanged(const double* x);

 private:
  int fNormEnum;

  vector<std::string> fDialNames;
  vector<int>  fDialEnums;
  vector<double> fDialValues;
  vector<TF1> fDialFuncs;
  vector<std::string> fDialUnits;

  std::string fName;

  bool fIsUsingNeut;
  bool fIsUsingGenie;
  bool fIsUsingNIWG;
  bool fIsUsingT2K;
  bool fIsUsingNuwro;
  bool fIsUsingModeNorm;

  bool fIsDialChanged;
  bool fIsNeutChanged;
  bool fIsGenieChanged;
  bool fIsNIWGChanged;
  bool fIsNuwroChanged;
  bool fIsT2KChanged;

  bool fSilenceWeightCalc;
  
  bool fSetAbsTwk;

  FitSplineHead* fSplineHead;
};

// GLOBAL FUNCTIONS FOR PAR CONV ----
namespace FitBase {

TF1 GetRWConvFunction(std::string type, std::string name);
std::string GetRWUnits(std::string type, std::string name);

double RWSigmaToFrac(std::string type, std::string name, double val);
double RWSigmaToAbs(std::string type, std::string name, double val);
double RWAbsToSigma(std::string type, std::string name, double val);
double RWFracToSigma(std::string type, std::string name, double val);

 int ConvDialType(std::string type);
 std::string ConvDialType(int type);
 int GetDialEnum(std::string type, std::string name);
 int GetDialEnum(int type, std::string name);
}

#endif
