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
#ifndef MICROBOONE_CC1MUNP_2025_NU_H_SEEN
#define MICROBOONE_CC1MUNP_2025_NU_H_SEEN

#include <functional>
#include <memory>
#include <vector>

#include "Measurement1D.h"

class MicroBooNEBlockHandler;

class TH2D;

struct MyCut {
  MyCut( std::function< double(FitEvent*) > getter,
    std::function< bool(double) > tester ) : getter_( getter ),
    tester_( tester ) {}

  std::function< double(FitEvent*) > getter_;
  std::function< bool(double) > tester_;

  inline bool evaluate( FitEvent* event ) const {
    return tester_( getter_(event) );
  }
};

/// Implements comparisons with the MicroBooNE measurement described
/// in Phys. Rev. D 112, 112004 (2025) https://doi.org/10.1103/8v2y-l89l
class MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu : public Measurement1D {

public:

  /// Basic Constructor.
  MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu( nuiskey samplekey );

  /// Virtual Destructor
  inline virtual ~MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu() {
    fResidualHist = NULL;
    fChi2LessBinHist = NULL;
  };

  /// Apply signal definition
  bool isSignal( FitEvent* nvect ) override;

  /// Fill kinematic distributions
  void FillEventVariables( FitEvent* customEvent ) override;

  virtual void FillHistograms() override;

  ///  Adjust the standard event rate conversion to include
  ///  application of the additional smearing matrix A_C
  void ConvertEventRates() override;

  /// Work around some hard-coded assumptions in Measurement1D::GetLikelihood()
  double GetLikelihood() override;

  void Write( std::string drawOpt ) override;

private:

  // Sets up definitions for each bin based on an input file stored in the
  // data/ area
  void LoadBinDefinitions();

  // Sets up histograms that represent slices of the full measurement
  void PrepareSlices();

  // Each bin is defined as a series of cuts that are applied to a FitEvent to
  // determine whether it belongs
  std::vector< std::vector< MyCut > > fBinDefinitions;

  // Temporary storage for the index of each bin that passed all cuts for any
  // particular event
  std::vector< size_t > fPassingBins;

  // Additional smearing matrix A_C used to transform the input MC predictions
  // when computing a chi-squared score
  std::shared_ptr< TMatrixD > fAddSmear;

  // Copy of MC histogram multiplied by the additional smearing matrix A_C
  std::shared_ptr< TH1D > fMCHistWithAC;

  // Manages slice histograms for the data
  std::shared_ptr< MicroBooNEBlockHandler > fBlockHandler;

  // Slice histograms for the MC
  std::vector< std::shared_ptr< TH1D > > fMCHist_Slices;
  std::map< int, std::vector< std::shared_ptr< TH1D > > > fMCModeHists_Slices;
};

#endif
