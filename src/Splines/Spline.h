#ifndef SPLINE_H
#define SPLINE_H
#include <vector>
#include "TObject.h"
#include "FitParameters.h"

// #include "PlotUtils.h"
// #include "FitUtils.h"
#include "stdint.h"
#include "stdlib.h"
#include "TCanvas.h"
#include <list>
#include "TF1.h"
#include "TSpline.h"
#include "SplineUtils.h"
#include "TGraph2D.h"
#include "TF2.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TH1D.h"

// Spline Class
class Spline {
public:

  Spline(std::string splname, std::string form, std::string points);
  ~Spline() {};

  void Setup(int type, int ndim, int npar);

  double operator()(const Double_t* x, const Double_t* par) const;
  // double operator()(const Double_t* x, const Double_t* y, const Double_t* par) const;

  //  double DoEval(const Double_t* x, const Double_t* par) const;
  //  double DoEval(const Double_t* par, bool checkresponse=true) const;

  float operator()(const Float_t* x, const Float_t* par) const;

  float DoEval(const Float_t* x, const Float_t* par) const;
  float DoEval(const Float_t* par, bool checkresponse = true) const;

  //  void FitCoeff(int n, double* x, double* y, double* par, bool draw);
  void FitCoeff(std::vector< std::vector<double> > v, std::vector<double> w, float* coeff, bool draw);

  inline std::string GetName(void) { return fName; };
  inline int GetNDim(void) { return fNDim; };
  inline int GetType(void) { return fType; };
  inline int GetNPar(void) { return fNPar;  };
  inline std::string GetForm() {return fForm;};

  //void Reconfigure(double x);
  void Reconfigure(float x, int index = 0);
  void Reconfigure(std::string name, float x);


  std::string fName;
  int fType;
  int fNDim;
  int fNPar;
  std::string fForm;
  std::string fPoints;
  bool fOutsideLimits;

  std::vector<std::string> fSplitNames;
  std::vector<std::string> fSplitPoints;

  mutable std::vector<float> fVal;
  mutable std::vector<float> fValMin;
  mutable std::vector<float> fValMax;

  mutable std::vector< std::vector<float> > fSplitScan;




  mutable std::vector<float> fXScan;
  mutable float fX;
  mutable float fXMin;
  mutable float fXMax;

    mutable std::vector<float> fYScan;
  mutable float fY;
  mutable float fYMin;
  mutable float fYMax;

  int  fSplineOffset;

  // TSpline3 Objects.
  mutable std::vector<float>::iterator iter_low;
  mutable std::vector<float>::iterator iter_high;
  mutable int off;

  // Create a new function for fitting.
  ROOT::Math::Minimizer* minimizer;

  // Available Spline Functions
  float Spline1DPol1(const Float_t* par) const;
  float Spline1DPol2(const Float_t* par) const;
  float Spline1DPol3(const Float_t* par) const;
  float Spline1DPol4(const Float_t* par) const;
  float Spline1DPol5(const Float_t* par) const;
  float Spline1DPol6(const Float_t* par) const;
  float Spline2DPol(const Float_t* par, int n) const;
  float Spline2DGaus(const Float_t* par) const;

float GetMonomial(int p) const;
  float Spline1DTSpline3(const Float_t* par) const;
float Spline2DTSpline3(const Float_t* par) const;


// void FitCoeff2DGraph(std::vector< std::vector<double> > v, std::vector<double> w, float* coeff, bool draw);

};

// class SplineFCN {
// public:

//   SplineFCN(Spline* spl, std::vector<std::vector<double> > v, std::vector<double> w) { fSpl = spl; fVal = v; fWeight = w; };
//   ~SplineFCN() {};

//   double operator()(const double* x) const;
//   double DoEval(const double *x) const;
// void SaveAs(std::string name, const double* x);

//   std::vector< std::vector<double> > fVal;
//   std::vector< double > fWeight;
//   Spline* fSpl;

// };

namespace SplineUtils {
// // Available Fitting Functions
// void FitCoeff1DGraph(Spline* spl, int n, double* x, double* y, float* coeff, bool draw);
// void GetCoeff1DTSpline3(Spline* spl, int n, double* x, double* y, float* coeff, bool draw);
// // void FitCoeff2DGraph(Spline* spl, int n, double* x, double* y, double* z, float* coeff, bool draw);

// Spline List
enum spline_types {
  k1DPol1 = 1,
  k1DPol2,
  k1DPol3,
  k1DPol4,
  k1DPol5,
  k1DPol6,
  k1DPol1C,
  k1DPol2C,
  k1DPol3C,
  k1DPol4C,
  k1DPol5C,
  k1DPol5C_LX,
  k1DPol10,
  k1DTSpline3,
  k1DPol25,
  k2DPol6,
  k2DGaus,
  k2DTSpline3
};

}

#endif
