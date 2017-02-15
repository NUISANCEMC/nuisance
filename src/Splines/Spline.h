#ifndef SPLINE_H
#define SPLINE_H
#include <vector>
#include "TObject.h"
#include "FitParameters.h"

#include "PlotUtils.h"
#include "FitUtils.h"
#include "stdint.h"
#include "stdlib.h"
#include "TCanvas.h"
#include <list>

// Spline Class
class Spline{
 public:

  Spline(std::string splname, std::string form, std::vector<int> pos);
  ~Spline(){};
  double DoEval(const Double_t* x, const Double_t* par) const;
  double operator()(const Double_t* x, const Double_t* par) const;
  void Setup(int type, int ndim, int npar);

  void FitCoeff(const int n, const double* x, const double* y, double* par);

  inline std::string GetName(){ return fName; };
  inline int GetNDim(){ return fNDim; };
  inline int GetType(){ return fType; };
  inline int GetNPar(){ return fNPar;  };
  int fType;
  int fNDim;
  int* fXPos;
  int fNPar;
  std::string fName;
  std::string fForm;

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
    k1DTSpline3_5P,
    k1DPol25,
    k2DPol6
  };

  // Available Spline Functions
  double Spline1DPol1(const Double_t* x, const Double_t* par) const;
  double Spline1DPol2(const Double_t* x, const Double_t* par) const;
  double Spline1DPol3(const Double_t* x, const Double_t* par) const;
  double Spline1DPol4(const Double_t* x, const Double_t* par) const;
  double Spline1DPol5(const Double_t* x, const Double_t* par) const;
  double Spline1DPol6(const Double_t* x, const Double_t* par) const;
  
  double Spline1DTSpline3_5P(const Double_t* x, const Double_t* par) const;
};

#endif
