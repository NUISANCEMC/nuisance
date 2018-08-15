#ifndef SPLINEMerger_H
#define SPLINEMerger_H

#include "FitWeight.h"
#include "Spline.h"

#include "SplineReader.h"

class SplineMerger : public SplineReader {
 public:
  SplineMerger(){};
  ~SplineMerger(){};

  void AddSplineSetFromFile(TFile* file); 
  void SetupSplineSet();

  void Write(std::string name);
  void AddCoefficientsToTree(TTree* tree);
  void GetEntry(int entry);

  void FillMergedSplines(int entry);

  float* fCoEffStorer;
  int fNCoEff;

  std::vector< float[1000] > fSplineAddressList;
  std::vector< int > fSplineSizeList;
  std::vector< TTree* > fSplineTreeList;

  std::vector< std::vector<double> > fParVect;
  std::vector< int > fSetIndex;
  std::vector< double > fWeightList;
  std::vector< double > fValList;

  
};

#endif
