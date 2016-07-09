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

#ifndef MINIBOONE_NCEL_XSEC_TRECO_NU_H_SEEN
#define MINIBOONE_NCEL_XSEC_RECO_NU_H_SEEN

#include "Measurement1D.h"

//********************************************************************
class MiniBooNE_NCEL_XSec_Treco_nu : public Measurement1D {
//********************************************************************

public:

  MiniBooNE_NCEL_XSec_Treco_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~MiniBooNE_NCEL_XSec_Treco_nu() {};

  void Reconfigure(double norm, bool fullconfig=false);

  double GetChi2();

#ifdef __NEUT_ENABLED__
  bool isSignal(NeutVect *nvect);
#endif

  void SetDataValues(std::string inputFile);

  void SetFakeDataValues(std::string fakeDataFile);

 private:
  void SetCovarMatrix(std::string covarFile, int dim);

  void SetResponseMatrix(std::string responseFile, int dim);

  std::string inFile;
  TChain *tn;
  Int_t nevents;
#ifdef __NEUT_ENABLED__
  NeutVect *nvect;
#endif

  // Because the Treco binning is irregular, store an array of bin edges...
  double arr_treco[52];

  TH1D *BKGD_other;
  TH1D *BKGD_irrid;
  TH2D *response_mat;

  Double_t totIntFlux;

  FitWeight *rw_engine;

};

#endif
