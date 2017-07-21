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

#ifndef MINERVA_CCNPIP_XSEC_1DTH_NU_H_SEEN
#define MINERVA_CCNPIP_XSEC_1DTH_NU_H_SEEN

#include "Measurement1D.h"
#include "MINERvAVariableBoxes.h"
class MINERvA_CCNpip_XSec_1Dth_nu : public Measurement1D {
public:
  MINERvA_CCNpip_XSec_1Dth_nu(nuiskey samplekey);
  virtual ~MINERvA_CCNpip_XSec_1Dth_nu() {};

  void FillEventVariables(FitEvent *event);
  void FillHistograms();
  bool isSignal(FitEvent *event);
  void Write(std::string drawOpt);
  bool fFullPhaseSpace;
  bool fUpdatedData;
  bool fFluxCorrection;

  /// \brief Use thpi Box to save all pion info on signal events
  inline NthpiVariableBox1D* GetPionBox(){ return static_cast<NthpiVariableBox1D*>(GetBox()); };

  /// \brief Create thpi Box for this sample
  inline MeasurementVariableBox* CreateBox(){ return new NthpiVariableBox1D(); };


private:
  bool isNew;

  TH1D *onePions;
  TH1D *twoPions;
  TH1D *threePions;
  TH1D *morePions;

};

#endif
