#ifndef SPLINEWRITER_H
#define SPLINEWRITER_H
#include "FitWeight.h"
#include "Spline.h"

#include "SplineUtils.h"
#ifdef __MINUIT2_ENABLED__

#ifdef ROOT6_USE_FIT_FITTER_INTERFACE
#include "Fit/Fitter.h"
#else
#include "TFitterMinuit.h"
#endif

#endif

class SplineFCN {
public:

  SplineFCN(Spline* spl, std::vector<std::vector<double> > v, std::vector<double> w) { fSpl = spl; fVal = v; fWeight = w; };
  ~SplineFCN() {};

  double operator()(const double* x) const;
  double DoEval(const double *x) const;
  void SaveAs(std::string name, const float* fx);
  void UpdateWeights(std::vector<double>& w);
  void SetCorrelated(bool state = true);

  bool uncorrelated;
  std::vector< std::vector<double> > fVal;
  std::vector< double > fWeight;
  Spline* fSpl;

};


class SplineWriter : public SplineReader {
public:
  SplineWriter(FitWeight* fw) {
    fRW = fw;
    fDrawSplines = FitPar::Config().GetParB("drawsplines");
  };
  ~SplineWriter() {};

  void SetupSplineSet();
  void Write(std::string name);
  void AddCoefficientsToTree(TTree* tree);
  void FitSplinesForEvent(TCanvas* fitcanvas = NULL, bool saveplot = false);
  void AddWeightsToTree(TTree* tr);
  void ReadWeightsFromTree(TTree* tr);
  void FitSplinesForEvent(double* weightvals, float* coeff);

  void GetWeightsForEvent(FitEvent* event, double* weights);
  void GetWeightsForEvent(FitEvent* event);
  void ReconfigureSet(int iset);
  double GetWeightForThisSet(FitEvent* event, int iset=-1);
  void SetWeights(double* weights);

  inline int GetNWeights(){return fParVect.size();};
  inline int GetNPars(){ return fNCoEff;};

  int fNCoEff;
  //  double* fCoEffStorer;
  float* fCoEffStorer;

  std::vector< std::vector<double> > fParVect;
  std::vector< int > fSetIndex;
  double* fWeightList;
  std::vector< std::vector<double> > fValList;
  int fCurrentSet;
  FitWeight* fRW;
  bool fDrawSplines;

  std::vector<TH1D*> fAllDrawnHists;
  std::vector<TGraph*> fAllDrawnGraphs;

#ifdef __MINUIT2_ENABLED__
  std::map<Spline*, SplineFCN*> fSplineFCNs;
  std::map<Spline*, ROOT::Math::Functor*> fSplineFunctors;
  std::map<Spline*, ROOT::Math::Minimizer*> fSplineMinimizers;
#endif

  //  Spline* gSpline;

  // Available Fitting Functions
  void FitCoeff(Spline* spl, std::vector< std::vector<double> >& v, std::vector<double>& w, float* coeff, bool draw);
  void FitCoeff1DGraph(Spline* spl, int n, double* x, double* y, float* coeff, bool draw);
  void GetCoeff1DTSpline3(Spline* spl, int n, double* x, double* y, float* coeff, bool draw);
  // void FitCoeff2DGraph(Spline* spl, std::vector< std::vector<double> >& v, std::vector<double>& w, float* coeff, bool draw);
  void FitCoeffNDGraph(Spline* spl, std::vector< std::vector<double> >& v, std::vector<double>& w, float* coeff, bool draw);
  void FitCoeff2DGraph(Spline* spl,  int n,  double* x,  double* y,  double* w, float* coeff, bool draw);
  //double Func2DWrapper(double* x, double* p);

};



#endif
