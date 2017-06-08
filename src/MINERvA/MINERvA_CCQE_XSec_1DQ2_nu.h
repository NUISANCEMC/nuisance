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
#ifndef MINERVA_1DQ2_nu_H_SEEN
#define MINERVA_1DQ2_nu_H_SEEN

#include "Measurement1D.h"

///\brief MINERvA CCQE+2p2h Analysis : Q2 Distribution
///
///\n Ref:
///\n Input: CH events generated with at least CCQE+2p2h interaction modes with flux given in Ref.
//********************************************************************
class MINERvA_CCQE_XSec_1DQ2_nu : public Measurement1D {
//********************************************************************
public:

  ///\brief Setup data histograms and full covariance matrix
  ///\n Available fit options: FIX,FREE,SHAPE/FULL,DIAG/NORM/MASK
  ///\n Sample is given as /neutron.
  ///
  ///\n Valid Sample Names:
  ///\n 1. MINERvA_CCQE_XSec_1DQ2_nu - Latest update full phase space
  ///\n 2. MINERvA_CCQE_XSec_1DQ2_nu_20deg - Latest update restricted phase space
  ///\n 3. MINERvA_CCQE_XSec_1DQ2_nu_oldflux - Old flux from original release, full phase space
  ///\n 4. MINERvA_CCQE_XSec_1DQ2_nu_20deg_oldflux - Old flux from original release, restricted phase space
  MINERvA_CCQE_XSec_1DQ2_nu(nuiskey samplekey);
  virtual ~MINERvA_CCQE_XSec_1DQ2_nu() {};

  ///\brief Signal is True CCQE+2p2h
  ///
  ///\n 1. True Mode == 1 or 2
  ///\n 2. 1.5 < Enu < 10.0
  ///\n 3. Muon Angle < 20deg (Restricted PS)
  bool isSignal(FitEvent* event);

  ///\brief Determine Q2 from the muon
  void FillEventVariables(FitEvent *event);

 private:

  bool isFluxFix; /// Flag for using updated flux
  bool fullphasespace; /// Flag for restricting phase space

};

#endif
