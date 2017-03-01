#ifndef SPLINEMerger_H
#define SPLINEMerger_H
/*
#include "FitWeight.h"
#include "Spline.h"
#include "FitParameters.h"
#include "SplineReader.h"

class SplineMerger {
 public:
  SplineMerger(FitWeight* fw){
    fDrawSplines = FitPar::Config().GetParB("drawsplines");
  };
  ~SplineMerger(){};

  void LoadSplinesForMerge(SplineReader* spl);
  void SetupSplineSet();
  void Write(std::string name);
  void AddCoefficientsToTree(TTree* tree);

  int fNCoEff;
  double* fCoEffStorer;

  std::vector< std::vector<double> > fParVect;
  std::vector< int > fSetIndex;
  std::vector< double > fWeightList;
  std::vector< double > fValList;

  FitWeight* fRW;
  bool fDrawSplines;
  
};
*/
#endif
