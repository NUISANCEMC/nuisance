#include "FitWeight.h"

#include "GENIEWeightEngine.h"
#include "LikelihoodWeightEngine.h"
#include "ModeNormEngine.h"
#include "NEUTWeightEngine.h"
#include "NIWGWeightEngine.h"
#include "NUISANCEWeightEngine.h"
#include "NuWroWeightEngine.h"
#include "OscWeightEngine.h"
#include "SampleNormEngine.h"
#include "SplineWeightEngine.h"
#include "T2KWeightEngine.h"

#ifdef __NOVA_ENABLED__
#include "NOvARwgtEngine.h"
#endif

#ifdef __NUSYST_ENABLED__
#include "nusystematicsWeightEngine.h"
#endif

void FitWeight::AddRWEngine(int type) {
  NUIS_LOG(FIT, "Adding reweight engine " << type);
  switch (type) {
  case kNEUT:
    fAllRW[type] = new NEUTWeightEngine("neutrw");
    break;

  case kNUWRO:
    fAllRW[type] = new NuWroWeightEngine("nuwrorw");
    break;

  case kGENIE:
    fAllRW[type] = new GENIEWeightEngine("genierw");
    break;

  case kNORM:
    fAllRW[type] = new SampleNormEngine("normrw");
    break;

  case kLIKEWEIGHT:
    fAllRW[type] = new LikelihoodWeightEngine("likerw");
    break;

  case kT2K:
    fAllRW[type] = new T2KWeightEngine("t2krw");
    break;

  case kCUSTOM:
    fAllRW[type] = new NUISANCEWeightEngine("nuisrw");
    break;

  case kSPLINEPARAMETER:
    fAllRW[type] = new SplineWeightEngine("splinerw");
    break;

  case kNIWG:
    fAllRW[type] = new NIWGWeightEngine("niwgrw");
    break;
  case kOSCILLATION:
    fAllRW[type] = new OscWeightEngine();
    break;
  case kMODENORM:
    fAllRW[type] = new ModeNormEngine();
    break;
#ifdef __NOVA_ENABLED__
  case kNOvARWGT:
    fAllRW[type] = new NOvARwgtEngine();
    break;
#endif
#ifdef __NUSYST_ENABLED__
  case kNuSystematics:
    fAllRW[type] = new nusystematicsWeightEngine();
    break;
#endif
  default:
    NUIS_ABORT("CANNOT ADD RW Engine for unknown dial type: " << type);
    break;
  }
}

WeightEngineBase *FitWeight::GetRWEngine(int type) {
  if (HasRWEngine(type)) {
    return fAllRW[type];
  }
  NUIS_ABORT("CANNOT get RW Engine for dial type: " << type);
}

bool FitWeight::HasRWEngine(int type) {
  switch (type) {
  case kNEUT:
  case kNUWRO:
  case kGENIE:
  case kNORM:
  case kLIKEWEIGHT:
  case kT2K:
  case kCUSTOM:
  case kSPLINEPARAMETER:
  case kNIWG:
  case kOSCILLATION:
#ifdef __NOVA_ENABLED__
  case kNOvARWGT:
#endif
#ifdef __NUSYST_ENABLED__
  case kNuSystematics:
#endif

  {
    return fAllRW.count(type);
  }
  default: { NUIS_ABORT("CANNOT get RW Engine for dial type: " << type); }
  }
}

void FitWeight::IncludeDial(std::string name, std::string type, double val) {
  // Should register the dial here.
  int typeenum = Reweight::ConvDialType(type);
  IncludeDial(name, typeenum, val);
}

void FitWeight::IncludeDial(std::string name, int dialtype, double val) {
  // Get the dial enum
  int nuisenum = Reweight::ConvDial(name, dialtype);

  if (nuisenum == -1) {
    NUIS_ERR(FTL, "Could not include dial " << name);
    NUIS_ERR(FTL, "With dialtype: " << dialtype);
    NUIS_ERR(FTL, "With value: " << val);
    NUIS_ABORT("With nuisenum: " << nuisenum);
  }

  // Setup RW Engine Pointer
  if (fAllRW.find(dialtype) == fAllRW.end()) {
    AddRWEngine(dialtype);
  }
  WeightEngineBase *rw = fAllRW[dialtype];

  // Include the dial
  rw->IncludeDial(name, val);

  // Set Dial Value
  if (val != -9999.9) {
    rw->SetDialValue(name, val);
  }

  // Sort Maps
  fAllEnums[name] = nuisenum;
  fAllValues[nuisenum] = val;

  // Sort Lists
  fNameList.push_back(name);
  fEnumList.push_back(nuisenum);
  fValueList.push_back(val);
}

void FitWeight::Reconfigure(bool silent) {
  // Reconfigure all added RW engines
  for (std::map<int, WeightEngineBase *>::iterator iter = fAllRW.begin();
       iter != fAllRW.end(); iter++) {
    (*iter).second->Reconfigure(silent);
  }
}

void FitWeight::SetDialValue(std::string name, double val) {
  // Add extra check, if name not found look for one with name in it.
  int nuisenum = fAllEnums[name];
  SetDialValue(nuisenum, val);
}

// Allow for name aswell using GlobalList to determine sample name.
void FitWeight::SetDialValue(int nuisenum, double val) {
  // Conv dial type
  int dialtype = Reweight::GetDialType(nuisenum);

  if (fAllRW.find(dialtype) == fAllRW.end()) {
    NUIS_ERR(FTL, "Can't find RW engine for parameter " << fNameList[dialtype]);
    NUIS_ERR(FTL, "With dialtype " << dialtype << ", "
                                   << Reweight::RemoveDialType(nuisenum));
    NUIS_ABORT("Are you sure you enabled the right engines?");
  }

  // Get RW Engine for this dial
  fAllRW[dialtype]->SetDialValue(nuisenum, val);
  fAllValues[nuisenum] = val;

  // Update ValueList
  for (size_t i = 0; i < fEnumList.size(); i++) {
    if (fEnumList[i] == nuisenum) {
      fValueList[i] = val;
    }
  }
}

void FitWeight::SetAllDials(const double *x, int n) {
  for (size_t i = 0; i < (UInt_t)n; i++) {
    int rwenum = fEnumList[i];
    SetDialValue(rwenum, x[i]);
  }
  Reconfigure();
}

double FitWeight::GetDialValue(std::string name) {
  // Add extra check, if name not found look for one with name in it.
  int nuisenum = fAllEnums[name];
  return GetDialValue(nuisenum);
}

double FitWeight::GetDialValue(int nuisenum) { return fAllValues[nuisenum]; }

int FitWeight::GetDialPos(std::string name) {
  int rwenum = fAllEnums[name];
  return GetDialPos(rwenum);
}

int FitWeight::GetDialPos(int nuisenum) {
  for (size_t i = 0; i < fEnumList.size(); i++) {
    if (fEnumList[i] == nuisenum) {
      return i;
    }
  }
  NUIS_ABORT("No Dial Found! (enum = " << nuisenum << ") ");
}

bool FitWeight::DialIncluded(std::string name) {
  return (fAllEnums.find(name) != fAllEnums.end());
}

bool FitWeight::DialIncluded(int rwenum) {
  return (fAllValues.find(rwenum) != fAllValues.end());
}

double FitWeight::CalcWeight(BaseFitEvt *evt) {
  double rwweight = 1.0;
  for (std::map<int, WeightEngineBase *>::iterator iter = fAllRW.begin();
       iter != fAllRW.end(); iter++) {
    double w = (*iter).second->CalcWeight(evt);
    rwweight *= w;
  }
  return rwweight;
}

void FitWeight::UpdateWeightEngine(const double *x) {
  size_t count = 0;
  for (std::vector<int>::iterator iter = fEnumList.begin();
       iter != fEnumList.end(); iter++) {
    SetDialValue((*iter), x[count]);
    count++;
  }
}

void FitWeight::GetAllDials(double *x, int n) {
  for (int i = 0; i < n; i++) {
    x[i] = GetDialValue(fEnumList[i]);
  }
}

// bool FitWeight::NeedsEventReWeight(const double* x) {
//   bool haschange = false;
//   size_t count = 0;

//   // Compare old to new and decide if RW needed.
//   for (std::vector<int>::iterator iter = fEnumList.begin();
//        iter != fEnumList.end(); iter++) {
//     int nuisenum = (*iter);
//     int type = (nuisenum / 1000) - (nuisenum % 1000);

//     // Compare old to new
//     double oldval = GetDialValue(nuisenum);
//     double newval = x[count];
//     if (oldval != newval) {
//       if (fAllRW[type]->NeedsEventReWeight()) {
//         haschange = true;
//       }
//     }

//     count++;
//   }

//   return haschange;
// }

double FitWeight::GetSampleNorm(std::string name) {
  if (name.empty())
    return 1.0;

  // Find norm dial
  if (fAllEnums.find(name + "_norm") != fAllEnums.end()) {
    if (fAllValues.find(fAllEnums[name + "_norm"]) != fAllValues.end()) {
      return fAllValues[fAllEnums[name + "_norm"]];
    } else {
      return 1.0;
    }
  } else {
    return 1.0;
  }
}

void FitWeight::Print() {
  NUIS_LOG(REC, "Fit Weight State: ");
  for (size_t i = 0; i < fNameList.size(); i++) {
    NUIS_LOG(REC,
             " -> Par " << i << ". " << fNameList[i] << " " << fValueList[i]);
  }
}
