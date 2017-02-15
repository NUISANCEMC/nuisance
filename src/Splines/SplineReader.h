#ifndef SPLINEREADER_H
#define SPLINEREADER_H
#include "FitWeight.h"
#include "Spline.h"

class SplineReader {
 public:
  SplineReader(FitWeight* fw){
    fRW = fw;
  };
  ~SplineReader(){};

  void AddSpline(std::string splname, std::string type, std::string form, std::string points);
  void SetupSplineSet();
  void Write(std::string name);
  void AddCoefficientsToTree(TTree* tree);
  void FitSplinesForEvent(FitEvent* event);
  void Read(TTree* tr);
  void Reconfigure();
  //  double CalcWeight(FitEvent* event);
  int fNPar;
  int fNWeight;
  
  double* fCoEffStorer;
  int fNCoEff;
  std::vector< double> fMaxVect;
  std::vector<double> fMinVect;
  std::vector< std::vector<double> > fParVect;
  std::vector< int > fSetIndex;
  std::vector< double > fValList;

  double** fParSets;
  double* fWeights;
  FitWeight* fRW;
  std::vector< double > fWeightList;
  std::vector<Spline> fAllSplines;
  int totalcount;
  std::vector<std::string> fSpline;
  std::vector<std::string> fType;
  std::vector<std::string> fForm;
  std::vector<std::string> fPoints;
  std::vector< std::vector<int> > fPositions;
};

#endif
