#ifndef SPLINEWRITER_H
#define SPLINEWRITER_H
#include "FitWeight.h"
#include "Spline.h"
#include "FitParameters.h"

class SplineWriter : public SplineReader {
 public:
  SplineWriter(FitWeight* fw){
    fRW = fw;
    fDrawSplines = FitPar::Config().GetParB("drawsplines");
  };
  ~SplineWriter(){};

  void SetupSplineSet();
  void Write(std::string name);
  void AddCoefficientsToTree(TTree* tree);
  void FitSplinesForEvent(FitEvent* event);


  int fNCoEff;
  double* fCoEffStorer;

  std::vector< std::vector<double> > fParVect;
  std::vector< int > fSetIndex;
  std::vector< double > fWeightList;
  std::vector< double > fValList;

  FitWeight* fRW;
  bool fDrawSplines;
  
};

#endif
