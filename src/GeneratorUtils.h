#ifndef GENERATOR_UTILS_H
#define GENERATOR_UTILS_H
#include "FitBuild.h"

#ifdef __NEUT_ENABLED__
#include "nefillverC.h"
#include "necardC.h"
#include "neutmodelC.h"
#include "neutparamsC.h"
#include "neworkC.h"
#include "fsihistC.h"
//Comment out as NEUT does not have the necessary proton FSI information yet
#include "nucleonfsihistC.h"
#include "neutcrsC.h"
#include "neutvect.h"
#include "neutpart.h"
#include "neutfsipart.h"
#include "neutfsivert.h"
#include "neutnucfsivert.h"
#include "neutnucfsistep.h"
#include "neutrootTreeSingleton.h"
#include "NModeDefn.h"

#include "NSyst.h"
#include "NFortFns.h" // Contains all the NEUT common blocks
#endif

#ifdef __NIWG_ENABLED__
#include "NIWGEvent.h"
#include "NIWGSyst.h"
#endif

namespace GeneratorUtils {
  
#ifdef __NEUT_ENABLED__
  void FillNeutCommons(NeutVect* nvect);
#endif
  
#ifdef __NIWG_ENABLED__
  niwg::rew::NIWGEvent* GetNIWGEvent(NeutVect* nvect);
#endif
  
#ifdef __NUWRO_ENABLED__
  int ConvertNuwroMode (event * e);
#endif
  
};
#endif
