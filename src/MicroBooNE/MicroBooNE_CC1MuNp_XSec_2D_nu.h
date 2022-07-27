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
#ifndef MICROBOONE_CC1MUNP_2D_NU_H_SEEN
#define MICROBOONE_CC1MUNP_2D_NU_H_SEEN

#include "Measurement1D.h"

class TH2D;

class MicroBooNE_CC1MuNp_XSec_2D_nu : public Measurement1D {

public:

  /// Basic Constructor.
  MicroBooNE_CC1MuNp_XSec_2D_nu( nuiskey samplekey );

  /// Virtual Destructor
  ~MicroBooNE_CC1MuNp_XSec_2D_nu() {};

  /// Apply signal definition
  bool isSignal( FitEvent* nvect );

  /// Fill kinematic distributions
  void FillEventVariables( FitEvent* customEvent );

  virtual void FillHistograms() override;

private:

  void LoadBinDefinitions();

  // Strings containing the cuts used to define each bin
  std::vector< std::string > fBinDefinitions;

  // Temporary storage for the index of each bin that passed all cuts for any
  // particular event
  std::vector< size_t > fPassingBins;

};

#endif
