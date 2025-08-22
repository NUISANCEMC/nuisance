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

#define UNDEF_DIAL_VALUE -9999.9
#define NUIS_DIAL_OFFSET 100000

class WeightEngineBase {
 public:
  WeightEngineBase(){};
  virtual ~WeightEngineBase(){};

  // Functions requiring Override
  virtual void IncludeDial(std::string name, double startval) = 0;

  virtual void SetDialValue(int nuisenum, double val) = 0;
  virtual void SetDialValue(std::string name, double val) = 0;

  virtual bool IsDialIncluded(std::string name);
  virtual bool IsDialIncluded(int nuisenum);

  virtual double GetDialValue(std::string name);
  virtual double GetDialValue(int nuisenum);

  virtual void Reconfigure(bool silent) = 0;

  virtual double CalcWeight(BaseFitEvt* evt) = 0;
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
