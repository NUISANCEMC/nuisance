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

#ifndef MINERVA_CC1PIP_XSEC_1DTPI_NU_H_SEEN
#define MINERVA_CC1PIP_XSEC_1DTPI_NU_H_SEEN

#include "Measurement1D.h"

class MINERvA_CC1pip_XSec_1DTpi_nu : public Measurement1D {
public:
  MINERvA_CC1pip_XSec_1DTpi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~MINERvA_CC1pip_XSec_1DTpi_nu() {};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  private:
};

#endif
