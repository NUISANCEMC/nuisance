#include "WeightUtils.h"

//********************************************************************
TF1 FitBase::GetRWConvFunction(std::string type, std::string name) {
//********************************************************************

  std::string dialfunc = "x";
  std::string parType = type;
  double low = -10000.0;
  double high = 10000.0;
  if (parType.find("parameter") == std::string::npos) parType += "_parameter";

  string line;
  ifstream card(
    (GeneralUtils::GetTopLevelDir() + "/parameters/dial_conversion.card").c_str(),
    ifstream::in);

  while (std::getline(card >> std::ws, line, '\n')) {

    std::vector<std::string> inputlist = GeneralUtils::ParseToStr(line, " ");

    // Check the line length
    if (inputlist.size() < 4) continue;

    // Check whether this is a comment
    if (inputlist[0].c_str()[0] == '#') continue;

    // Check whether this is the correct parameter type
    if (inputlist[0].compare(parType) != 0) continue;

    // Check the parameter name
    if (inputlist[1].compare(name) != 0) continue;

    // inputlist[2] should be the units... ignore for now

    dialfunc = inputlist[3];

    // High and low are optional, check whether they exist
    if (inputlist.size() > 4) low  = GeneralUtils::StrToDbl(inputlist[4]);
    if (inputlist.size() > 5) high = GeneralUtils::StrToDbl(inputlist[5]);

  }

  TF1 convfunc = TF1((name + "_convfunc").c_str(), dialfunc.c_str(), low, high);
  return convfunc;
}

//********************************************************************
std::string FitBase::GetRWUnits(std::string type, std::string name) {
  //********************************************************************

  std::string unit = "sig.";
  std::string parType = type;

  if (parType.find("parameter") == std::string::npos) {
    parType += "_parameter";
  }

  std::string line;
  std::ifstream card((GeneralUtils::GetTopLevelDir() + "/parameters/dial_conversion.card").c_str(), ifstream::in);

  while (std::getline(card >> std::ws, line, '\n')) {

    std::vector<std::string> inputlist = GeneralUtils::ParseToStr(line, " ");

    // Check the line length
    if (inputlist.size() < 3) continue;

    // Check whether this is a comment
    if (inputlist[0].c_str()[0] == '#') continue;

    // Check whether this is the correct parameter type
    if (inputlist[0].compare(parType) != 0) continue;

    // Check the parameter name
    if (inputlist[1].compare(name) != 0) continue;

    unit = inputlist[2];
    break;
  }

  return unit;
}

//********************************************************************
double FitBase::RWAbsToSigma(std::string type, std::string name, double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.GetX(val);
  if (fabs(conv_val) < 1E-10) conv_val = 0.0;
  return conv_val;
}

//********************************************************************
double FitBase::RWSigmaToAbs(std::string type, std::string name, double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.Eval(val);
  return conv_val;
}

//********************************************************************
double FitBase::RWFracToSigma(std::string type, std::string name, double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.GetX((val * f1.Eval(0.0)));
  if (fabs(conv_val) < 1E-10) conv_val = 0.0;
  return conv_val;
}

//********************************************************************
double FitBase::RWSigmaToFrac(std::string type, std::string name, double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.Eval(val) / f1.Eval(0.0);
  return conv_val;
}



int FitBase::ConvDialType(std::string type) {

  if      (!type.compare("neut_parameter")) return kNEUT;
  else if (!type.compare("niwg_parameter")) return kNIWG;
  else if (!type.compare("nuwro_parameter")) return kNUWRO;
  else if (!type.compare("t2k_parameter")) return kT2K;
  else if (!type.compare("genie_parameter")) return kGENIE;
  else if (!type.compare("custom_parameter")) return kCUSTOM;
  else if (!type.compare("norm_parameter")) return kNORM;
  else if (!type.compare("modenorm_parameter")) return kMODENORM;
  else if (!type.compare("likeweight_parameter")) return kLIKEWEIGHT;
  else if (!type.compare("spline_parameter")) return kSPLINEPARAMETER;
  else return kUNKNOWN;

}

std::string FitBase::ConvDialType(int type) {

  switch (type) {
  case kNEUT:  { return "neut_parameter";  }
  case kNIWG:  { return "niwg_parameter";  }
  case kNUWRO: { return "nuwro_parameter"; }
  case kT2K:   { return "t2k_parameter";   }
  case kGENIE: { return "genie_parameter"; }
  case kNORM:  { return "norm_parameter";  }
  case kCUSTOM: {return "custom_parameter"; }
  case kMODENORM: { return "modenorm_parameter"; }
  case kLIKEWEIGHT: { return "likeweight_parameter"; }
  case kSPLINEPARAMETER: {return "spline_parameter";}
  default: return "unknown_parameter";
  }

}

int FitBase::GetDialEnum(std::string type, std::string name) {
  return FitBase::GetDialEnum( FitBase::ConvDialType(type), name );
}



int FitBase::GetDialEnum(int type, std::string name) {

  int offset = type * 1000;
  int this_enum = -1; //Not Found

  std::cout << "Getting dial enum " << type << " " << name << std::endl;
  // Select Types
  switch (type) {

  // NEUT DIAL TYPE
  case kNEUT: {
#ifdef __NEUT_ENABLED__
    int neut_enum = (int)neut::rew::NSyst::FromString(name);
    if (neut_enum != 0) { this_enum = neut_enum + offset; }
#else
    this_enum = -2; //Not enabled
#endif
    break;
  }

  // NIWG DIAL TYPE
  case kNIWG: {
#ifdef __NIWG_ENABLED__
    int niwg_enum = (int)niwg::rew::NIWGSyst::FromString(name);
    if (niwg_enum != 0) { this_enum = niwg_enum + offset; }
#else
    this_enum = -2;
#endif
    break;
  }

  // NUWRO DIAL TYPE
  case kNUWRO: {
#ifdef __NUWRO_REWEIGHT_ENABLED__
    int nuwro_enum = (int)nuwro::rew::NuwroSyst::FromString(name);
    if (nuwro_enum > 0) { this_enum = nuwro_enum + offset; }
#else
    this_enum = -2;
#endif
  }

  // GENIE DIAL TYPE
  case kGENIE: {
#ifdef __GENIE_ENABLED__
    int genie_enum = (int)genie::rew::GSyst::FromString(name);
    if (genie_enum > 0) { this_enum = genie_enum + offset; }
#else
    this_enum = -2;
#endif
    break;
  }

  case kCUSTOM: {
    int custom_enum = 0;  // PLACEHOLDER
    this_enum = custom_enum + offset;
    break;
  }

  // T2K DIAL TYPE
  case kT2K: {
#ifdef __T2KREW_ENABLED__
    int t2k_enum = (int)t2krew::T2KSyst::FromString(name);
    if (t2k_enum > 0) { this_enum = t2k_enum + offset; }
#else
    this_enum = -2;
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

  case kMODENORM: {
    size_t us_pos = name.find_first_of('_');
    std::string numstr = name.substr(us_pos + 1);
    int mode_num = std::atoi(numstr.c_str());
    LOG(FTL) << "Getting mode num " << mode_num << std::endl;
    if (!mode_num) {
      ERR(FTL) << "Attempting to parse dial name: \"" << name
               << "\" as a mode norm dial but failed." << std::endl;
      throw;
    }
    this_enum = 60 + mode_num + offset;
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
  }

  }

  // If Not Enabled
  if (this_enum == -2) {
    ERR(FTL) << "RW Engine not supported for " << FitBase::ConvDialType(type) << std::endl;
    ERR(FTL) << "Check dial " << name << std::endl;
  }

  // If Not Found
  if (this_enum == -1) {
    ERR(FTL) << "Dial " << name << " not found." << std::endl;
  }

  return this_enum;
}









int Reweight::ConvDialType(std::string type) {

  if      (!type.compare("neut_parameter")) return kNEUT;
  else if (!type.compare("niwg_parameter")) return kNIWG;
  else if (!type.compare("nuwro_parameter")) return kNUWRO;
  else if (!type.compare("t2k_parameter")) return kT2K;
  else if (!type.compare("genie_parameter")) return kGENIE;
  else if (!type.compare("norm_parameter")) return kNORM;
  else if (!type.compare("modenorm_parameter")) return kMODENORM;
  else if (!type.compare("custom_parameter")) return kCUSTOM;
  else if (!type.compare("likeweight_parameter")) return kLIKEWEIGHT;
  else if (!type.compare("spline_parameter")) return kSPLINEPARAMETER;
  else return kUNKNOWN;

}

std::string Reweight::ConvDialType(int type) {

  switch (type) {
  case kNEUT:  { return "neut_parameter";  }
  case kNIWG:  { return "niwg_parameter";  }
  case kNUWRO: { return "nuwro_parameter"; }
  case kT2K:   { return "t2k_parameter";   }
  case kGENIE: { return "genie_parameter"; }
  case kNORM:  { return "norm_parameter";  }
  case kCUSTOM: {return "custom_parameter"; }

  case kMODENORM: { return "modenorm_parameter"; }
  case kLIKEWEIGHT: { return "likeweight_parameter"; }
  case kSPLINEPARAMETER: {return "spline_parameter";}
  default: return "unknown_parameter";
  }

}







int Reweight::NEUTEnumFromName(std::string name) {
#ifdef __NEUT_ENABLED__
  int neutenum = (int)neut::rew::NSyst::FromString(name);
  return (neutenum > 0) ? neutenum : kNoDialFound;
#else
  return kGeneratorNotBuilt;
#endif
}

int Reweight::NIWGEnumFromName(std::string name) {
#ifdef __NIWG_ENABLED__
  int niwgenum = (int)niwg::rew::NIWGSyst::FromString(name);
  return (niwgenum != 0) ? niwgenum : kNoDialFound;
#else
  return kGeneratorNotBuilt;
#endif
}

int Reweight::NUWROEnumFromName(std::string name) {
#ifdef __NUWRO_REWEIGHT_ENABLED__
  int nuwroenum = (int)nuwro::rew::NuwroSyst::FromString(name);
  return (nuwroenum > 0) ? nuwroenum : kNoDialFound;
#else
  return kGeneratorNotBuilt;
#endif
}

int Reweight::GENIEEnumFromName(std::string name) {
#ifdef __GENIE_ENABLED__
  int genieenum = (int)genie::rew::GSyst::FromString(name);
  return (genieenum > 0) ? genieenum : kNoDialFound;
#else
  return kGeneratorNotBuilt;
#endif
}

int Reweight::T2KEnumFromName(std::string name) {
#ifdef __T2KREW_ENABLED__
  int t2kenum = (int)t2krew::T2KSyst::FromString(name);
  return (t2kenum > 0) ? t2kenum : kNoDialFound;
#else
  return kGeneratorNotBuilt;
#endif
}

int Reweight::NUISANCEEnumFromName(std::string name, int type) {
  int nuisenum = Reweight::DialList().EnumFromNameAndType(name, type);
  return nuisenum;
}

int Reweight::CustomEnumFromName(std::string name) {
  int custenum = Reweight::ConvertNUISANCEDial(name);
  return custenum;
}

int Reweight::ConvDial(std::string name, std::string type, bool exceptions) {
  return Reweight::ConvDial( name, Reweight::ConvDialType(type), exceptions );
}

int Reweight::ConvDial(std::string fullname, int type, bool exceptions) {

  std::string name = GeneralUtils::ParseToStr(fullname,",")[0]; // Only use first dial given

  // Produce offset seperating each type.
  int offset   = type * 1000;
  int genenum = kNoDialFound;

  switch (type) {

  case kNEUT:
    genenum = NEUTEnumFromName(name);
    break;

  case kNIWG:
    genenum = NIWGEnumFromName(name);
    break;

  case kNUWRO:
    genenum = NUWROEnumFromName(name);
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
  case kMODENORM:
  case kLIKEWEIGHT:
  case kSPLINEPARAMETER:
  case kNEWSPLINE:
    genenum = NUISANCEEnumFromName(name, type);
    break;

  default:
    genenum = kNoTypeFound;
    break;

  }

  // Throw if required.
  if (exceptions) {
    // If Not Enabled
    if (genenum == kGeneratorNotBuilt) {
      ERR(FTL) << "RW Engine not supported for " << FitBase::ConvDialType(type) << std::endl;
      ERR(FTL) << "Check dial " << name << std::endl;
      throw;
    }

    // If no type enabled
    if (genenum == kNoTypeFound) {
      ERR(FTL) << "Type mismatch inside ConvDialEnum" << std::endl;
      throw;
    }

    // If Not Found
    if (genenum == kNoDialFound) {
      ERR(FTL) << "Dial " << name << " not found." << std::endl;
      throw;
    }
  }

  // Add offset if no issue
  int nuisenum = genenum;
  if (genenum != kGeneratorNotBuilt and
      genenum != kNoTypeFound and
      genenum != kNoDialFound) {
    nuisenum += offset;
  }

  // Now register dial
  // std::cout << "Returning " << nuisenum << std::endl;
  Reweight::DialList().RegisterDialEnum(name, type, nuisenum);

  return nuisenum;
}


std::string Reweight::ConvDial(int nuisenum) {
  //GlobalDialList* temp;
  for (size_t i = 0; i < Reweight::DialList().fAllDialEnums.size(); i++) {
    if (Reweight::DialList().fAllDialEnums[i] == nuisenum) {
      return Reweight::DialList().fAllDialNames[i];
    }
  }

  LOG(FIT) << "Cannot find dial with enum = " << nuisenum << std::endl;
  return "";
}


