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

#ifndef FNAL_CCQE_Evt_1DQ2_NU_H_SEEN
#define FNAL_CCQE_Evt_1DQ2_NU_H_SEEN

#include "Measurement1D.h"
#include "CustomVariableBoxes.h"

//********************************************************************
class FNAL_CCQE_Evt_1DQ2_nu : public Measurement1D {
//********************************************************************

public:

  FNAL_CCQE_Evt_1DQ2_nu(nuiskey samplekey);
  virtual ~FNAL_CCQE_Evt_1DQ2_nu() {};
  
  /// \brief Get Q2 assuming QE
  void FillEventVariables(FitEvent *event);

  /// \brief Get True CCQE
  bool isSignal(FitEvent *event);

  /// \brief Fill main histograms and correction histograms             
  void FillHistograms();

  /// \brief scale the MC Hist and correction histograms
  void ScaleEvents();

  /// \brief Use Q2 Box to save correction info
  inline Q2VariableBox1D* GetQ2Box(){ return static_cast<Q2VariableBox1D*>(GetBox()); };

  /// \brief Create Q2 Box to save correction info
  inline MeasurementVariableBox* CreateBox(){ return new Q2VariableBox1D(); };

 private:

  bool applyQ2correction; ///< Flag of whether deut correction applied
  TH1D* CorrectionHist; ///< Correction factor
  TH1D* fMCHist_NoCorr; ///< Uncorrected fMCHist

  double ThetaMu;
  double q2qe;

};  
#endif
