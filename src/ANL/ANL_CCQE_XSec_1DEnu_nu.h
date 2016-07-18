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

#ifndef ANL_CCQE_XSEC_1DENU_NU_H_SEEN
#define ANL_CCQE_XSEC_1DENU_NU_H_SEEN
#include "Measurement1D.h"

/*!
  ANL Charged Current Quasi-elastic Measurement \n
  1D Flux unfolded cross-section in Enu \n\n
  Class supports multiple published datasets: \n
  \item PRD.16.3103 =  ANL_CCQE_XSec_1DEnu_nu_PRD16, or ANL_CCQE_XSec_1DEnu_nu
  \item PRL.31.844 = ANL_CCQE_XSec_1DEnu_nu_PRL31
*/
class ANL_CCQE_XSec_1DEnu_nu : public Measurement1D {
public:
  ANL_CCQE_XSec_1DEnu_nu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~ANL_CCQE_XSec_1DEnu_nu() {};
  
  //void ResetAll();
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);
  void FillHistograms();
  //void ScaleEvents(); 
  //void ApplyNormScale(double norm);
  //void Write();

 private:
  bool applyQ2correction;  //!< [FLAG] Apply free nucleon to dueterium correction
  TH1D* CorrectionHist;    //!< Histogram to interpolate free nucleon correction
};
  
#endif
