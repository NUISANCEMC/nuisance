#ifndef WEIGHTENGINE_BASE_H
#define WEIGHTENGINE_BASE_H

#include "BaseFitEvt.h"
#include "FitLogger.h"
#include "FitUtils.h"

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#define _UNDEF_DIAL_VALUE_ -9999.9

class WeightEngineBase {
 public:
  WeightEngineBase(){};
  virtual ~WeightEngineBase(){};

  // Functions requiring Override
  virtual void IncludeDial(std::string name, double startval){};

  virtual void SetDialValue(int nuisenum, double val){};
  virtual void SetDialValue(std::string name, double val){};

  virtual bool IsDialIncluded(std::string name);
  virtual bool IsDialIncluded(int nuisenum);

  virtual double GetDialValue(std::string name);
  virtual double GetDialValue(int nuisenum);

  virtual void Reconfigure(bool silent){};

  virtual double CalcWeight(BaseFitEvt* evt) { return 1.0; };
  virtual bool NeedsEventReWeight() = 0;

  std::string GetNameFromEnum(int nuisenum);

  bool fHasChanged;
  bool fIsAbsTwk;

  std::vector<double> fValues;
  std::map<int, std::vector<size_t> > fEnumIndex;
  std::map<std::string, std::vector<size_t> > fNameIndex;

  std::string fCalcName;
};

#endif
