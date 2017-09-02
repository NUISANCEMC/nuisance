#ifndef SPLINEREADER_H
#define SPLINEREADER_H
// #include "FitWeight.h"
#include "Spline.h"
#include "TTree.h"
#include "FitLogger.h"
#include "NuisConfig.h"
#include "NuisKey.h"

// #include "GeneralUtils.h"

class SplineReader {
public:
  SplineReader() {};
  ~SplineReader() {};

  void AddSpline(nuiskey splkey);
  void Read(TTree* tr);

  void Reconfigure(std::map< std::string, double >& vals);
  bool NeedsReconfigure();
  void SetNeedsReconfigure(bool val = true);

  int GetNPar();
  double CalcWeight(float* coeffs);

  std::vector<Spline> fAllSplines;
  std::vector<std::string> fSpline;
  std::vector<std::string> fType;
  std::vector<std::string> fForm;
  std::vector<std::string> fPoints;

  std::vector<double> fDialValues;
  std::vector<double> fParValues;

  bool fNeedsReconfigure;



};

#endif
