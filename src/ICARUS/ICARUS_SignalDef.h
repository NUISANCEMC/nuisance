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

#ifndef ICARUS_SIGNALDEF_H_SEEN
#define ICARUS_SIGNALDEF_H_SEEN

#include "FitEvent.h"

namespace SignalDef {


  struct helper_ICARUS_NuMI_CCQELike_numu{
    bool IsSignal{false};
    bool IsAntiNu{false};
    bool IsMuonPLT0p8{false};
    double MuonCos{-999.}, MuonProtonCos{-999.};
    double deltaPT{-999.}, deltaalphaT{-999.};
  };
  helper_ICARUS_NuMI_CCQELike_numu isICARUS_NuMI_CCQELike_numu(FitEvent *event, double emin, double emax);

}

#endif
