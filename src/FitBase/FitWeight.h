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

#ifndef FIT_WEIGHT_2
#define FIT_WEIGHT_2

#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <deque>
#include <time.h>
#include <vector>
#include <map>

#include "FitBuild.h"
#include "TObject.h"
#include "FitEvent.h"
using namespace std;

#ifdef __T2KREW_ENABLED__
#include "T2KReWeight.h"
#include "T2KNeutReWeight.h"
#include "T2KNIWGReWeight.h"
#include "T2KGenieReWeight.h"
#include "T2KNeutUtils.h"
#include "T2KNIWGUtils.h"
using namespace t2krew;
#endif

#ifdef __NIWG_ENABLED__
#include "NIWGSyst.h"
#include "NIWGReWeight.h"
#include "NIWGReWeight2012a.h"
#include "NIWGReWeightDeltaMass.h"
#include "NIWGReWeight1piAngle.h"
#include "NIWGReWeightSplineEnu.h"
#include "NIWGReWeight2010a.h"
#include "NIWGReWeightPiMult.h"
#include "NIWGReWeightSpectralFunc.h"
#include "NIWGReWeightMEC.h"
#include "NIWGReWeightRPA.h"
#include "NIWGReWeightEffectiveRPA.h"
#include "NIWGReWeight2014a.h"
#include "NIWGReWeightProtonFSIbug.h"
#include "NIWGReWeightHadronMultSwitch.h"
#endif

#ifdef __NEUT_ENABLED__
#include "NSyst.h"
#include "NReWeight.h"
#include "NReWeightNuXSecCCQE.h"
#include "NReWeightNuXSecRES.h"
#include "NReWeightNuXSecCCRES.h"
#include "NReWeightNuXSecCOH.h"
#include "NReWeightNuXSecDIS.h"
#include "NReWeightNuXSecNCEL.h"
#include "NReWeightNuXSecNC.h"
#include "NReWeightNuXSecNCRES.h"
#include "NReWeightCasc.h"
#include "NReWeightNuclPiless.h"
#include "neutvect.h"
#include "neutpart.h"
#endif

#ifdef __NUWRO_ENABLED__
#include "event1.h"
#endif

#ifdef __NUWRO_REWEIGHT_ENABLED__
#include "NuwroReWeight.h"
#include "NuwroReWeight_QEL.h"
#include "NuwroReWeight_FlagNorm.h"
#include "NuwroSyst.h"
#endif

#ifdef __GENIE_ENABLED__
#include "GSyst.h"
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepRecord.h"
#include "Ntuple/NtpMCEventRecord.h"
#include "ReWeight/GReWeight.h"
#include "EVGCore/EventRecord.h"
#include "ReWeight/GReWeightNuXSecNCEL.h"
#include "ReWeight/GReWeightNuXSecCCQE.h"
#include "ReWeight/GReWeightNuXSecCCRES.h"
#include "ReWeight/GReWeightNuXSecCOH.h"
#include "ReWeight/GReWeightNonResonanceBkg.h"
#include "ReWeight/GReWeightFGM.h"
#include "ReWeight/GReWeightDISNuclMod.h"
#include "ReWeight/GReWeightResonanceDecay.h"
#include "ReWeight/GReWeightFZone.h"
#include "ReWeight/GReWeightINuke.h"
#include "ReWeight/GReWeightAGKY.h"
#include "ReWeight/GReWeightNuXSecCCQEvec.h"
#include "ReWeight/GReWeightNuXSecNCRES.h"
#include "ReWeight/GReWeightNuXSecDIS.h"
#include "ReWeight/GReWeightNuXSecNC.h"
using namespace genie;
using namespace genie::rew;
#endif

#include "FitSplineHead.h"
#include "TCanvas.h"
#include "TGraph2D.h"

enum RWDialType{
  kNeutDial=0,
  kNIWGDial,
  kNuWroDial,
  kGenieDial,
  kCustomDial,
  kNormDial
} RWDialType_t;


//******************************************
class FitWeight : public TObject {
//****************************************** 

 public:

  FitWeight(std::string rw_name, std::string inputfile); // Open a fit result file and grab the ttree and setup like that.  
  FitWeight(std::string rw_name="FitWeight");
  ~FitWeight(){};

  int GetDialEnum(std::string name, int type = -1);
  int GetRWEnum(int dial_enum);

  void IncludeDial(std::string name, int type, double startval=0.0);
  
  void SetDialValue(std::string name, double val);
  void SetDialValue(int dial, double val);

  double GetDialValue(std::string name, std::string option="");
  double GetDialValue(int dial);

  unsigned int GetDialPos(std::string name);
  unsigned int GetDialPos(int this_enum);

  std::string GetDialType(int this_enum);

  bool HasDialChanged(){ return dial_changed;};

  void UpdateWeightEngine(const double* x);
  void SetAllDials(const double* x, int npt);
  void GetAllDials(double* x, int npt);

  std::vector<std::string> GetDialNames();
  std::vector<int> GetDialEnums();
  std::vector<double> GetDialValues();

  double GetSampleNorm(std::string samplename);

  double CalcWeight(FitEvent* evt);
  double CalcWeight(FitEventBase* evt);
  
  void PrintState();
  
  void Reconfigure();


  // GENERATOR SPECIFIC RW OBJECTS

#ifdef __NEUT_ENABLED__ // --- NEUT BLOCK
  void SetupNeutRW();
  neut::rew::NReWeight* neut_rw;
#endif

#ifdef __NIWG_ENABLED__ // --- NIWG BLOCK
  void SetupNIWGRW();
  niwg::rew::NIWGReWeight* niwg_rw;
#endif

#ifdef __NUWRO_REWEIGHT_ENABLED__ // --- NUWRO BLOCK     
  void SetupNuwroRW();
  nuwro::rew::NuwroReWeight* nuwro_rw;
#endif

#ifdef __T2KREW_ENABLED__ // --- T2KRW BLOCK
  void SetupT2KRW();
  t2krew::T2KReWeight  *t2k_rw; //!< T2K RW Object
  t2krew::T2KNeutReWeight* t2k_neutrw;
  t2krew::T2KNIWGReWeight* t2k_niwgrw;
#endif

#ifdef __GENIE_ENABLED__ // --- GENIE BLOCK
  void SetupGenieRW();
  genie::rew::GReWeight  *genie_rw; //!< Genie RW Object   
#endif

  

  // SPLINE FUNCTIONS ------------
  FitSplineHead* GetSplineHeader(){ return spline_head; };
  void SetupEventCoeff(FitEventBase* event);
  void SetupSpline(std::string dialname, std::string splinename, std::string pointsdef);
  double CalcSplineWeight(FitEvent* evt);
  void ReadSplineHead(FitSplineHead* splhead);
  void ResetSplines();
  void GenSplines(FitEventBase* evt, bool save_graph=false);

  void Fit1DSplineCoeff(FitEventBase* event, FitSpline* spl, double nom, bool save_graph);
  void Fit2DSplineCoeff(FitEventBase* event, FitSpline* spl, double nom, bool save_graph);
  void FitNDSplineCoeff(FitEventBase* event, FitSpline* spl, double norm, bool save_graph){
    (void) event;
    (void) spl;
    (void) norm;
    (void) save_graph;
  };

 private:

  int norm_enum;
  
  vector<std::string> dial_names;
  vector<int> dial_enums;
  vector<double> dial_values;
  vector<TF1> dial_func;
  vector<std::string> dial_units;

  std::string rw_name;
  bool dial_changed;
  bool using_neut, using_genie, using_niwg, using_t2k, using_nuwro;
  bool neut_changed, genie_changed, niwg_changed, nuwro_changed, t2k_changed;
  
  FitSplineHead* spline_head;

  ClassDef(FitWeight, 1);
};

// GLOBAL FUNCTIONS FOR PAR CONV ----
namespace FitBase{

  TF1 GetRWConvFunction(std::string type, std::string name);
  std::string GetRWUnits(std::string type, std::string name);

  double RWSigmaToFrac(std::string type, std::string name, double val);
  double RWSigmaToAbs(std::string type, std::string name, double val);
  double RWAbsToSigma(std::string type, std::string name, double val);
  double RWFracToSigma(std::string type, std::string name, double val);
    
}

#endif
