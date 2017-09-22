#ifndef SPLINE_H
#define SPLINE_H
#include <vector>
#include "TObject.h"

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
#include "Math/IFunction.h"
#include "Math/IParamFunction.h"
#include "FitLogger.h"

// Spline Class
class Spline : public  ROOT::Math::ParamFunctor { 
private:

   const double* pars;
 
public:
   double DoEvalPar(const double* x, const double* p) const ;
 
   unsigned int NDim() const{
      return fNDim;
   }
   // ROOT::Math::IParametricFunctionMultiDim* Clone() const{
   //    return new Spline(this->fName, this->fForm, this->fPoints);
   // }
 
   const double* Parameters() const{
      return pars;
   }
 
   void SetParameters(const double* p){
      pars = p;
   }
 
   unsigned int NPar() const{
      return fNPar;
   }
 
  Spline(std::string splname, std::string form, std::string points);
  ~Spline() {};

  void Setup(int type, int ndim, int npar);

  double operator()(const Double_t* x, const Double_t* par);
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

   // Available Spline Functions
  float Spline1DPol1(const Float_t* par) const;
  float Spline1DPol2(const Float_t* par) const;
  float Spline1DPol3(const Float_t* par) const;
  float Spline1DPol4(const Float_t* par) const;
  float Spline1DPol5(const Float_t* par) const;
  float Spline1DPol6(const Float_t* par) const;
  float Spline2DPol(const Float_t* par, int n) const;
  float Spline2DGaus(const Float_t* par) const;

  float Spline1DTSpline3(const Float_t* par) const;
  float Spline2DTSpline3(const Float_t* par) const;


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

  TF1* fROOTFunction;
  TF1* GetFunction();

};


namespace SplineUtils {

  double Func2DWrapper(double* x, double* p);
  extern Spline* gSpline;

}



namespace SplineUtils {

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
