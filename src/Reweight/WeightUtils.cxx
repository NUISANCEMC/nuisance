#include "WeightUtils.h"

#include "FitLogger.h"
#ifdef T2KReWeight_ENABLED
#ifndef T2KReWeight_LEGACY_API_ENABLED
#include "NReWeightEngineI.h"
#include "T2KReWeight/WeightEngines/NEUT/T2KNEUTUtils.h"
#include "T2KReWeight/WeightEngines/T2KReWeightFactory.h"
#else
#include "T2KGenieReWeight.h"
#include "T2KNIWGReWeight.h"
#include "T2KNIWGUtils.h"
#include "T2KNeutReWeight.h"
#include "T2KNeutUtils.h"
#include "T2KReWeight.h"
using namespace t2krew;
#endif
#endif

#ifdef NIWGLegacy_ENABLED
#include "NIWGReWeight.h"
#include "NIWGSyst.h"
#endif

#ifdef NEUTReWeight_ENABLED
#ifdef NEUTReWeight_LEGACY_API_ENABLED
#include "NReWeight.h"
#endif
#include "NSyst.h"
#endif

#ifdef GENIEReWeight_ENABLED
#ifdef GENIE3_API_ENABLED
using namespace genie;
#include "RwFramework/GReWeight.h"
#include "RwFramework/GSyst.h"
using namespace genie::rew;
#else
#include "ReWeight/GReWeight.h"
#include "ReWeight/GSyst.h"
#endif
#endif

#ifdef NOvARwgt_ENABLED
#include "NOvARwgtEngine.h"
#endif

#ifdef nusystematics_ENABLED
#include "nusystematicsWeightEngine.h"
#endif

#include "GlobalDialList.h"
#include "ModeNormEngine.h"
#include "NUISANCESyst.h"

#ifdef Prob3plusplus_ENABLED
#include "OscWeightEngine.h"
#endif

//********************************************************************
TF1 FitBase::GetRWConvFunction(std::string const &type,
                               std::string const &name) {
  //********************************************************************

  std::string dialfunc = "x";
  std::string parType = type;
  double low = -10000.0;
  double high = 10000.0;
  if (parType.find("parameter") == std::string::npos)
    parType += "_parameter";

  std::string line;
  std::ifstream card(
      (GeneralUtils::GetTopLevelDir() + "/parameters/dial_conversion.card")
          .c_str(),
      std::ifstream::in);

  while (std::getline(card >> std::ws, line, '\n')) {
    std::vector<std::string> inputlist = GeneralUtils::ParseToStr(line, " ");

    // Check the line length
    if (inputlist.size() < 4)
      continue;

    // Check whether this is a comment
    if (inputlist[0].c_str()[0] == '#')
      continue;

    // Check whether this is the correct parameter type
    if (inputlist[0].compare(parType) != 0)
      continue;

    // Check the parameter name
    if (inputlist[1].compare(name) != 0)
      continue;

    // inputlist[2] should be the units... ignore for now

    dialfunc = inputlist[3];

    // High and low are optional, check whether they exist
    if (inputlist.size() > 4)
      low = GeneralUtils::StrToDbl(inputlist[4]);
    if (inputlist.size() > 5)
      high = GeneralUtils::StrToDbl(inputlist[5]);
  }

  TF1 convfunc = TF1((name + "_convfunc").c_str(), dialfunc.c_str(), low, high);
  return convfunc;
}

//********************************************************************
std::string FitBase::GetRWUnits(std::string const &type,
                                std::string const &name) {
  //********************************************************************

  std::string unit = "sig.";
  std::string parType = type;

  if (parType.find("parameter") == std::string::npos) {
    parType += "_parameter";
  }

  std::string line;
  std::ifstream card(
      (GeneralUtils::GetTopLevelDir() + "/parameters/dial_conversion.card")
          .c_str(),
      std::ifstream::in);

  while (std::getline(card >> std::ws, line, '\n')) {
    std::vector<std::string> inputlist = GeneralUtils::ParseToStr(line, " ");

    // Check the line length
    if (inputlist.size() < 3)
      continue;

    // Check whether this is a comment
    if (inputlist[0].c_str()[0] == '#')
      continue;

    // Check whether this is the correct parameter type
    if (inputlist[0].compare(parType) != 0)
      continue;

    // Check the parameter name
    if (inputlist[1].compare(name) != 0)
      continue;

    unit = inputlist[2];
    break;
  }

  return unit;
}

//********************************************************************
double FitBase::RWAbsToSigma(std::string const &type, std::string const &name,
                             double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.GetX(val);
  if (fabs(conv_val) < 1E-10)
    conv_val = 0.0;

  NUIS_LOG(FIT, "AbsToSigma(" << name << ") = " << val << " -> " << conv_val);
  return conv_val;
}

//********************************************************************
double FitBase::RWSigmaToAbs(std::string const &type, std::string const &name,
                             double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.Eval(val);
  return conv_val;
}

//********************************************************************
double FitBase::RWFracToSigma(std::string const &type, std::string const &name,
                              double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.GetX((val * f1.Eval(0.0)));
  if (fabs(conv_val) < 1E-10)
    conv_val = 0.0;
  return conv_val;
}

//********************************************************************
double FitBase::RWSigmaToFrac(std::string const &type, std::string const &name,
                              double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.Eval(val) / f1.Eval(0.0);
  return conv_val;
}

int FitBase::ConvDialType(std::string const &type) {
  if (!type.compare("neut_parameter"))
    return kNEUT;
  else if (!type.compare("niwg_parameter"))
    return kNIWG;
  else if (!type.compare("t2k_parameter"))
    return kT2K;
  else if (!type.compare("genie_parameter"))
    return kGENIE;
  else if (!type.compare("custom_parameter"))
    return kCUSTOM;
  else if (!type.compare("norm_parameter"))
    return kNORM;
  else if (!type.compare("likeweight_parameter"))
    return kLIKEWEIGHT;
  else if (!type.compare("spline_parameter"))
    return kSPLINEPARAMETER;
  else if (!type.compare("osc_parameter"))
    return kOSCILLATION;
  else if (!type.compare("modenorm_parameter"))
    return kMODENORM;
  else if (!type.compare("nova_parameter"))
    return kNOvARWGT;
  else if (!type.compare("nusyst_parameter"))
    return kNuSystematics;
  else
    return kUNKNOWN;
}

std::string FitBase::ConvDialType(int type) {
  switch (type) {
  case kNEUT: {
    return "neut_parameter";
  }
  case kNIWG: {
    return "niwg_parameter";
  }
  case kT2K: {
    return "t2k_parameter";
  }
  case kGENIE: {
    return "genie_parameter";
  }
  case kNORM: {
    return "norm_parameter";
  }
  case kCUSTOM: {
    return "custom_parameter";
  }
  case kLIKEWEIGHT: {
    return "likeweight_parameter";
  }
  case kSPLINEPARAMETER: {
    return "spline_parameter";
  }
  case kOSCILLATION: {
    return "osc_parameter";
  }
  case kMODENORM: {
    return "modenorm_parameter";
  }
  case kNOvARWGT: {
    return "nova_parameter";
  }
  case kNuSystematics: {
    return "nusyst_parameter";
  }
  default:
    return "unknown_parameter";
  }
}

int FitBase::GetDialEnum(std::string const &type, std::string const &name) {
  return FitBase::GetDialEnum(FitBase::ConvDialType(type), name);
}

int FitBase::GetDialEnum(int type, std::string const &name) {
  int offset = type * NUIS_DIAL_OFFSET;
  int this_enum = Reweight::kNoDialFound; // Not Found

  NUIS_LOG(DEB, "Getting dial enum " << type << " " << name);
  // Select Types
  switch (type) {
  // NEUT DIAL TYPE
  case kNEUT: {
#ifdef NEUTReWeight_ENABLED
#if NEUTReWeight_LEGACY_API_ENABLED
    int neut_enum = (int)neut::rew::NSyst::FromString(name);
#else
    int neut_enum = (int)neut::NSyst::DialFromString(name);
#endif
    if (neut_enum != 0) {
      this_enum = neut_enum + offset;
    }
#else
    this_enum = Reweight::kNoTypeFound; // Not enabled
#endif
    break;
  }

  // NIWG DIAL TYPE
  case kNIWG: {
#ifdef NIWGLegacy_ENABLED
    int niwg_enum = (int)niwg::rew::NIWGSyst::FromString(name);
    if (niwg_enum != 0) {
      this_enum = niwg_enum + offset;
    }
#else
    this_enum = Reweight::kNoTypeFound;
#endif
    break;
  }

  // GENIE DIAL TYPE
  case kGENIE: {
#ifdef GENIEReWeight_ENABLED
    int genie_enum = (int)genie::rew::GSyst::FromString(name);
    if (genie_enum > 0) {
      this_enum = genie_enum + offset;
    }
#else
    this_enum = Reweight::kNoTypeFound;
#endif
    break;
  }

  case kCUSTOM: {
    int custom_enum = 0; // PLACEHOLDER
    this_enum = custom_enum + offset;
    break;
  }

  // T2K DIAL TYPE
  case kT2K: {
#ifdef T2KReWeight_ENABLED
#ifndef T2KReWeight_LEGACY_API_ENABLED
    // This is possibly inefficient, this should probably not be called per fit
    // step.
    if (!t2krew::T2KNEUTUtils::CardIsSet()) {
      std::string neut_card = FitPar::Config().GetParS("NEUT_CARD");
      if (neut_card.size()) {
        t2krew::T2KNEUTUtils::SetCardFile(neut_card);
      }
    }

    int t2k_enum = t2krew::T2KSystToInt(
        t2krew::MakeT2KReWeightInstance(t2krew::Event::kNEUT)
            ->DialFromString(name));
#else
    int t2k_enum = (int)t2krew::T2KSyst::FromString(name);
#endif
    if (t2k_enum > 0) {
      this_enum = t2k_enum + offset;
    }
#else
    this_enum = Reweight::kNoTypeFound;
#endif
    break;
  }

  case kNORM: {
    if (gNormEnums.find(name) == gNormEnums.end()) {
      gNormEnums[name] = gNormEnums.size() + 1 + offset;
    }
    this_enum = gNormEnums[name];
    break;
  }

  case kLIKEWEIGHT: {
    if (gLikeWeightEnums.find(name) == gLikeWeightEnums.end()) {
      gLikeWeightEnums[name] = gLikeWeightEnums.size() + 1 + offset;
    }
    this_enum = gLikeWeightEnums[name];
    break;
  }

  case kSPLINEPARAMETER: {
    if (gSplineParameterEnums.find(name) == gSplineParameterEnums.end()) {
      gSplineParameterEnums[name] = gSplineParameterEnums.size() + 1 + offset;
    }
    this_enum = gSplineParameterEnums[name];
    break;
  }
  case kOSCILLATION: {
#ifdef Prob3plusplus_ENABLED
    int oscEnum = OscWeightEngine::SystEnumFromString(name);
    if (oscEnum != 0) {
      this_enum = oscEnum + offset;
    }
#else
    this_enum = Reweight::kNoTypeFound; // Not enabled
#endif
    break;
  }
  case kMODENORM: {
    size_t us_pos = name.find_first_of('_');
    std::string numstr = name.substr(us_pos + 1);
    int mode_num = std::atoi(numstr.c_str());
    NUIS_LOG(FTL, "Getting mode num " << mode_num);
    if (!mode_num) {
      NUIS_ABORT("Attempting to parse dial name: \""
                 << name << "\" as a mode norm dial but failed.");
    }
    this_enum = 60 + mode_num + offset;
    break;
  }
  }

  // If Not Enabled
  if (this_enum == Reweight::kNoTypeFound) {
    NUIS_ERR(FTL,
             "RW Engine not supported for " << FitBase::ConvDialType(type));
    NUIS_ABORT("Check dial " << name);
  }

  // If Not Found
  if (this_enum == Reweight::kNoDialFound) {
    NUIS_ABORT("Dial " << name << " not found.");
  }

  return this_enum;
}

int Reweight::ConvDialType(std::string const &type) {
  return FitBase::ConvDialType(type);
}

std::string Reweight::ConvDialType(int type) {
  return FitBase::ConvDialType(type);
}

int Reweight::GetDialType(int type) {
  int t = (type / NUIS_DIAL_OFFSET);
  return t > kNuSystematics ? Reweight::kNoDialFound : t;
}

int Reweight::RemoveDialType(int type) { return (type % NUIS_DIAL_OFFSET); }

int Reweight::NEUTEnumFromName(std::string const &name) {
#ifdef NEUTReWeight_ENABLED
#ifdef NEUTReWeight_LEGACY_API_ENABLED
  int neutenum = (int)neut::rew::NSyst::FromString(name);
  return (neutenum > 0) ? neutenum : Reweight::kNoDialFound;
#else
  return neut::NSyst::DialFromString(name);
#endif
#else
  (void)name;
  return Reweight::kGeneratorNotBuilt;
#endif
}

int Reweight::NIWGEnumFromName(std::string const &name) {
#ifdef NIWGLegacy_ENABLED
  int niwgenum = (int)niwg::rew::NIWGSyst::FromString(name);
  return (niwgenum != 0) ? niwgenum : Reweight::kNoDialFound;
#else
  (void)name;
  return Reweight::kGeneratorNotBuilt;
#endif
}

int Reweight::GENIEEnumFromName(std::string const &name) {
#ifdef GENIEReWeight_ENABLED
  int genieenum = (int)genie::rew::GSyst::FromString(name);
  return (genieenum > 0) ? genieenum : Reweight::kNoDialFound;
#else
  (void)name;
  return Reweight::kGeneratorNotBuilt;
#endif
}

int Reweight::T2KEnumFromName(std::string const &name) {
#ifdef T2KReWeight_ENABLED
#ifndef T2KReWeight_LEGACY_API_ENABLED
  // This is possibly inefficient, this should probably not be called per fit
  // step.
  if (!t2krew::T2KNEUTUtils::CardIsSet()) {
    std::string neut_card = FitPar::Config().GetParS("NEUT_CARD");
    if (neut_card.size()) {
      t2krew::T2KNEUTUtils::SetCardFile(neut_card);
    }
  }

  int t2kenum =
      t2krew::T2KSystToInt(t2krew::MakeT2KReWeightInstance(t2krew::Event::kNEUT)
                               ->DialFromString(name));
#else
  int t2kenum = (int)t2krew::T2KSyst::FromString(name);
#endif
  return (t2kenum > 0) ? t2kenum : Reweight::kNoDialFound;
#else
  (void)name;
  return Reweight::kGeneratorNotBuilt;
#endif
}

int Reweight::OscillationEnumFromName(std::string const &name) {
#ifdef Prob3plusplus_ENABLED
  int oscEnum = OscWeightEngine::SystEnumFromString(name);
  return (oscEnum > 0) ? oscEnum : Reweight::kNoDialFound;
#else
  (void)name;
  return Reweight::kGeneratorNotBuilt;
#endif
}

int Reweight::NUISANCEEnumFromName(std::string const &name, int type) {
  int nuisenum = Reweight::DialList().EnumFromNameAndType(name, type);
  return nuisenum;
}

int Reweight::CustomEnumFromName(std::string const &name) {
  int custenum = Reweight::ConvertNUISANCEDial(name);
  return (custenum != kUnknownNUISANCEDial ? custenum : kNoDialFound);
}

int Reweight::ConvDial(std::string const &name, std::string const &type,
                       bool exceptions) {
  return Reweight::ConvDial(name, Reweight::ConvDialType(type), exceptions);
}

int Reweight::ConvDial(std::string const &fullname, int type, bool exceptions) {
  std::string name =
      GeneralUtils::ParseToStr(fullname, ",")[0]; // Only use first dial given

  // Produce offset seperating each type.
  int offset = type * NUIS_DIAL_OFFSET;
  int genenum = Reweight::kNoDialFound;

  switch (type) {
  case kNEUT:
    genenum = NEUTEnumFromName(name);
    break;

  case kNIWG:
    genenum = NIWGEnumFromName(name);
    break;

  case kGENIE:
    genenum = GENIEEnumFromName(name);
    break;

  case kT2K:
    genenum = T2KEnumFromName(name);
    break;

  case kCUSTOM:
    genenum = CustomEnumFromName(name);
    break;

  case kNORM:
  case kLIKEWEIGHT:
  case kSPLINEPARAMETER:
  case kNEWSPLINE:
    genenum = NUISANCEEnumFromName(name, type);
    break;

  case kOSCILLATION:
    genenum = OscillationEnumFromName(name);
    break;

  case kMODENORM:
    genenum = ModeNormEngine::SystEnumFromString(name);
    break;

#ifdef NOvARwgt_ENABLED
  case kNOvARWGT:
    genenum = NOvARwgtEngine::GetWeightGeneratorIndex(name);
    break;
#endif

#ifdef nusystematics_ENABLED
  case kNuSystematics: {
    // Super inefficient...
    nusystematicsWeightEngine we;
    genenum = we.ConvDial(name);
    break;
  }
#endif

  default:
    genenum = Reweight::kNoTypeFound;
    break;
  }

  // Throw if required.
  if (exceptions) {
    // If Not Enabled
    if (genenum == Reweight::kGeneratorNotBuilt) {
      NUIS_ERR(FTL,
               "RW Engine not supported for " << FitBase::ConvDialType(type));
      NUIS_ABORT("Check dial " << name);
    }

    // If no type enabled
    if (genenum == Reweight::kNoTypeFound) {
      NUIS_ABORT("Type mismatch inside ConvDialEnum");
    }

    // If Not Found
    if (genenum == Reweight::kNoDialFound) {
      NUIS_ABORT("Dial " << name << " not found.");
    }
  }

  // Add offset if no issue
  int nuisenum = genenum;
  if ((genenum != Reweight::kGeneratorNotBuilt) &&
      (genenum != Reweight::kNoTypeFound) &&
      (genenum != Reweight::kNoDialFound)) {
    nuisenum += offset;
  }

  // Now register dial
  Reweight::DialList().RegisterDialEnum(name, type, nuisenum);

  return nuisenum;
}

std::string Reweight::ConvDial(int nuisenum) {
  for (size_t i = 0; i < Reweight::DialList().fAllDialEnums.size(); i++) {
    if (Reweight::DialList().fAllDialEnums[i] == nuisenum) {
      return Reweight::DialList().fAllDialNames[i];
    }
  }

  NUIS_LOG(FIT, "Cannot find dial with enum = " << nuisenum);
  return "";
}
