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

#ifndef SCIBOONEUTILS_H_SEEN
#define SCIBOONEUTILS_H_SEEN

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iostream>
#include <numeric>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <TLorentzVector.h>
#include <TH2D.h>

#include "FitParticle.h"
#include "FitLogger.h"

/*!
 *  \addtogroup Utils
 *  @{
 */

namespace SciBooNEUtils {

  double CalcEfficiency(TH2D *effHist, FitParticle *nu, FitParticle *muon);
  TVector3 DistanceInScintillator(FitParticle* particle);
  bool PassesCOHDistanceCut(FitParticle* particle);

}
#endif
