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

// Spline Class
class Spline {
public:

  Spline(std::string splname, std::string form, std::vector<double> x);
  ~Spline() {};

  void Setup(int type, int ndim, int npar);

  double operator()(const Double_t* x, const Double_t* par) const;
  //  double DoEval(const Double_t* x, const Double_t* par) const;
  //  double DoEval(const Double_t* par, bool checkresponse=true) const;

  float operator()(const Float_t* x, const Float_t* par) const;
  float DoEval(const Float_t* x, const Float_t* par) const;
  float DoEval(const Float_t* par, bool checkresponse=true) const;
  
  //  void FitCoeff(int n, double* x, double* y, double* par, bool draw);
  void FitCoeff(int n, double* x, double* y, float* par, bool draw);

  inline std::string GetName(void) { return fName; };
  inline int GetNDim(void) { return fNDim; };
  inline int GetType(void) { return fType; };
  inline int GetNPar(void) { return fNPar;  };
  inline std::string GetForm() {return fForm;};

  //void Reconfigure(double x);
  void Reconfigure(float x);

  std::string fName;
  std::vector<std::string> fSplineNames;
  int fType;
  int fNDim;
  int fNPar;
  std::string fForm;
  bool fOutsideLimits;

  mutable std::vector<float> fXScan;
  mutable float fX;
  mutable float fXMin;
  mutable float fXMax;
  int  fSplineOffset;

  mutable std::vector<float>::iterator iter_low;
  mutable std::vector<float>::iterator iter_high;
  mutable int off;

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
    k2DPol6
  };

  // Available Spline Functions
  float Spline1DPol1(const Float_t* par) const;
  float Spline1DPol2(const Float_t* par) const;
  float Spline1DPol3(const Float_t* par) const;
  float Spline1DPol4(const Float_t* par) const;
  float Spline1DPol5(const Float_t* par) const;
  float Spline1DPol6(const Float_t* par) const;

  float Spline1DTSpline3(const Float_t* par) const;

  // Available Fitting Functions
  void FitCoeff1DGraph(int n, double* x, double* y, float* coeff, bool draw);
  void GetCoeff1DTSpline3(int n, double* x, double* y, float* coeff, bool draw);



};

#endif
