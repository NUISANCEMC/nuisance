// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef FIT_SPLINE_H
#define FIT_SPLINE_H

#include "TObject.h"
#include "FitParameters.h"
#include "FitBuild.h"
#include "PlotUtils.h"
#include "FitUtils.h"
#include "stdint.h"
#include "stdlib.h"
#include <list>

class FitSpline : public TObject {

 public:

  // Constructors
  FitSpline();
  ~FitSpline(){};
  FitSpline(std::string ident, std::string dist,
	    std::vector<int> list_enums, std::string points_def );
  
  void ReconfigureSpline(std::vector<int> dial_enums, std::vector<double> dial_values);

  double operator()(const Double_t* x, const Double_t* par) const;
  double DoEval(const double* par) const;
  double DoEval(const double* x, const double* par, const bool use_offset=true) const;
  
  void SetType(std::string type);
  void SetType(int t, int p, int d, bool f=true);
  void SetOffset(int i){ offset = i; };

  std::vector<double> GetSplineCoeff(double* weights);

  // Public Access Spline Variables
  std::vector<int> var_enums;
  std::vector<double> x;
  
  mutable  std::vector< std::vector<double> > x_vals;
  std::vector<double> x_low;
  std::vector<double> x_high;

  bool needs_fit;
  
  UInt_t offset;
  UInt_t npar;
  UInt_t ndim;  
  UInt_t spline;

  std::string id;
  std::string form;
  std::string points;

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

  ClassDef(FitSpline,1);
};
ClassImp(FitSpline);
#endif
