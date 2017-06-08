#ifndef WEIGHTUTILS_H
#define WEIGHTUTILS_H

#include "FitLogger.h"
#include "FitParameters.h"
#include "FitEvent.h"
#include "TF1.h"

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
#include "NUISANCESyst.h"
#include "GlobalDialList.h"

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
 static std::map<std::string, int> gNormEnums;
 static std::map<std::string, int> gLikeWeightEnums;
 static std::map<std::string, int> gSplineParameterEnums;
}


namespace Reweight {

	int ConvDial(std::string name, std::string type, bool exceptions=false);
	int ConvDial(std::string name, int type, bool exceptions=false);
	std::string ConvDial(int nuisenum);

    int ConvDialType(std::string type);
    std::string ConvDialType(int type);

	int NEUTEnumFromName(std::string name);
	int NIWGEnumFromName(std::string name);
	int NUWROEnumFromName(std::string name);
	int T2KEnumFromName(std::string name);
	int GENIEEnumFromName(std::string name);
	int CustomEnumFromName(std::string name); 
	
	int NUISANCEEnumFromName(std::string name, int type);

	static const int kNoDialFound       = -1;
	static const int kNoTypeFound       = -2;
	static const int kGeneratorNotBuilt = -3;
}



#endif
