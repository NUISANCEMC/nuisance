#ifndef FITWEIGHT2_H
#define FITWEIGHT2_H

#include "WeightUtils.h"
#include "WeightEngineBase.h"

#define UNDEF_DIAL_VALUE -9999.9

#include <map>
#include <vector>

class FitWeight {
public:
  FitWeight(std::string name = "") {};

  // Add a new RW engine given type
  void AddRWEngine(int rwtype);
  WeightEngineBase* GetRWEngine(int type);
  bool HasRWEngine(int type);

  // Includes
  void IncludeDial(std::string name, std::string type, double val = UNDEF_DIAL_VALUE);
  void IncludeDial(std::string name, int type, double val = UNDEF_DIAL_VALUE);

  // Update RW Engines
  void Reconfigure(bool silent = false);

  void SetDialValue(std::string name, double val);
  void SetDialValue(int rwenum, double val);

  double GetDialValue(std::string name);
  double GetDialValue(int rwenum);

  int GetDialPos(std::string name);
  int GetDialPos(int rwenum);

  bool DialIncluded(std::string name);
  bool DialIncluded(int rwenum);

  double CalcWeight(BaseFitEvt* evt);
  bool HasRWDialChanged(const double* x) { return true; };
  // bool NeedsEventReWeight(const double* x);

  void SetAllDials(const double* x, int n);

  double GetSampleNorm(std::string name);

  void UpdateWeightEngine(const double* x);

  inline std::vector<int> GetDialEnums() { return fEnumList; };
  inline std::vector<std::string> GetDialNames() { return fNameList; };
  inline std::vector<double> GetDialValues() { return fValueList; };
  void GetAllDials(double* x, int n);

  void Print();

  std::vector<int> fEnumList;
  std::vector<std::string> fNameList;
  std::vector<double> fValueList;

  std::map<std::string, int> fAllEnums;
  std::map<int, double> fAllValues;
  std::map<int, WeightEngineBase*> fAllRW;

};

#endif
