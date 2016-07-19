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

#ifndef ANL_CCQE_Evt_1DQ2_NU_H_SEEN
#define ANL_CCQE_Evt_1DQ2_NU_H_SEEN

#include "Measurement1D.h"

/*!
  ANL Charged Current Quasi-elastic Measurement \n
  1D Event Rate in Q2QE \n
  Class supports multiple published datasets: \n
  \item PRD.26.537  = ANL_CCQE_XSec_1DQ2_nu_PRD26, or ANL_CCQE_XSec_1DEnu_nu
  \item PRD.16.3103 = ANL_CCQE_XSec_1DQ2_nu_PRD16
  \item PRL.31.844  = ANL_CCQE_XSec_1DEQ2_nu_PRL31
*/
class ANL_CCQE_Evt_1DQ2_nu : public Measurement1D {

public:

  ANL_CCQE_Evt_1DQ2_nu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~ANL_CCQE_Evt_1DQ2_nu() {}; 
  
  void FillEventVariables(FitEvent *event);       
  bool isSignal(FitEvent *event);                 
  void FillHistograms();
  void ResetAll();
  void ScaleEvents();
  void Write(std::string drawOpt);
 private:

  bool applyQ2correction; ///< Flag of whether deut correction applied
  TH1D* CorrectionHist; ///< Correction factor
  TH1D* mcHist_NoCorr; ///< Uncorrected mcHist
  double q2qe;       ///< X_VAR
  double scaleF; ///< Nominal Scale Factor
  bool applyEnucorrection;
  TH2D* EnuvsQ2Plot; // For Scaling

  TH1D* EnuFluxUnfoldPlot;
  TH1D* EnuRatePlot;

};
  
#endif
