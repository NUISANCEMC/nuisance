// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef MINIBOONE_NCEL_XSEC_TRECO_NU_H_SEEN
#define MINIBOONE_NCEL_XSEC_RECO_NU_H_SEEN

#include "Measurement1D.h"

//********************************************************************
class MiniBooNE_NCEL_XSec_Treco_nu : public Measurement1D {
//********************************************************************

public:
  void Write(std::string arg);

  MiniBooNE_NCEL_XSec_Treco_nu(nuiskey samplekey);

  virtual ~MiniBooNE_NCEL_XSec_Treco_nu() {};

  void FillEventVariables(FitEvent *event);

  bool isSignal(FitEvent *event);

  void ScaleEvents();

  void SetDataValues(std::string inputFile, double* arr_treco);

 private:
  void SetCovarMatrix(std::string covarFile, int dim);

  void SetResponseMatrix(std::string responseFile, int dim, double* arr_treco);

  void SetFluxHistogram(std::string dataFile);

  TH1D *newFluxHist;
  TH1D *BKGD_other;
  TH1D *BKGD_irrid;
  TH2D *response_mat;

};

#endif
