#ifndef WEIGHTUTILS_H
#define WEIGHTUTILS_H

#include "FitEvent.h"

#include "TF1.h"

enum extra_reweight_types {
  kOSCILLATION = kLast_generator_event_type,
  kMODENORM,
  kNOvARWGT,
  kNuSystematics
};

namespace FitBase {

TF1 GetRWConvFunction(std::string const &type, std::string const &name);
std::string GetRWUnits(std::string const &type, std::string const &name);

double RWSigmaToFrac(std::string const &type, std::string const &name,
                     double val);
double RWSigmaToAbs(std::string const &type, std::string const &name,
                    double val);
double RWAbsToSigma(std::string const &type, std::string const &name,
                    double val);
double RWFracToSigma(std::string const &type, std::string const &name,
                     double val);

int ConvDialType(std::string const &type);
std::string ConvDialType(int type);
int GetDialEnum(std::string const &type, std::string const &name);
int GetDialEnum(int type, std::string const &name);
static std::map<std::string, int> gNormEnums;
static std::map<std::string, int> gLikeWeightEnums;
static std::map<std::string, int> gSplineParameterEnums;
}

namespace Reweight {

int ConvDial(std::string const &name, std::string const &type,
             bool exceptions = false);
int ConvDial(std::string const &name, int type, bool exceptions = false);
std::string ConvDial(int nuisenum);

int ConvDialType(std::string const &type);
std::string ConvDialType(int type);
int GetDialType(int type);
int RemoveDialType(int type);

int NEUTEnumFromName(std::string const &name);
int NIWGEnumFromName(std::string const &name);
int NUWROEnumFromName(std::string const &name);
int T2KEnumFromName(std::string const &name);
int GENIEEnumFromName(std::string const &name);
int CustomEnumFromName(std::string const &name);

int NUISANCEEnumFromName(std::string const &name, int type);
int OscillationEnumFromName(std::string const &name);

static const int kNoDialFound = -1;
static const int kNoTypeFound = -2;
static const int kGeneratorNotBuilt = -3;
}

#endif
