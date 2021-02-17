// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#ifndef ANL_CCQE_XSEC_1DENU_NU_H_SEEN
#define ANL_CCQE_XSEC_1DENU_NU_H_SEEN
#include "Measurement1D.h"
#include "CustomVariableBoxes.h"


/// /brief  ANL Charged Current Quasi-elastic Measurement 
///
/// 1D Flux unfolded cross-section in Enu 
/// Class supports multiple published datasets: \n
/// - PRD.16.3103 =  ANL_CCQE_XSec_1DEnu_nu_PRD16, or ANL_CCQE_XSec_1DEnu_nu
/// - PRL.31.844  = ANL_CCQE_XSec_1DEnu_nu_PRL31
class ANL_CCQE_XSec_1DEnu_nu : public Measurement1D {
public:

  ANL_CCQE_XSec_1DEnu_nu(nuiskey samplekey);
  virtual ~ANL_CCQE_XSec_1DEnu_nu() {};
  
  /// \brief Get Q2 assuming QE scattering
  void FillEventVariables(FitEvent *event);

  /// \brief Selection only true CCQE
  bool isSignal(FitEvent *event);

  /// \brief fill normal MC and corrected MC
  void FillHistograms();

  /// \brief scale normal MC and corrected MC
  void ScaleEvents();

  /// \brief Use Q2 Box to save correction info
  inline Q2VariableBox1D* GetQ2Box(){ return static_cast<Q2VariableBox1D*>(GetBox()); };

  /// \brief Create Q2 Box to save correction info
  inline MeasurementVariableBox* CreateBox(){ return new Q2VariableBox1D(); };

 private:
  bool applyQ2correction;  ///< Apply free nucleon to dueterium correction
  TH1D* CorrectionHist;    ///< Histogram to interpolate free nucleon correction
  TH1D* fMCHist_NoCorr;    ///< Uncorrected fMCHist

};
  
#endif
