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
#ifndef MICROBOONE_NCpi0_NU_H_SEEN
#define MICROBOONE_NCpi0_NU_H_SEEN

#pragma once

#include <memory>

#include <TMatrixDfwd.h>
#include "Measurement1D.h"
#include "WireCellHelper.h"

class TH1D;
class TH2D;
class TFile;

template <distribution_t D, distribution_t... Ds>
class NCpi0Helper : public IWireCellHelper<D, Ds...> {
public:
  NCpi0Helper() :
    IWireCellHelper<D, Ds...>(FitPar::GetDataBase() +
                              "/MicroBooNE/NCpi0/real_bins.txt")
  {}
  // this gives us the measurements based on the blocks we requested
  void load_measurement() override {

    // load our histograms
    TFile* inputRootFile = TFile::Open((FitPar::GetDataBase() +
                                       "/MicroBooNE/NCpi0/MicroBooNE_NCpi0_data_release.root").c_str());
    TH1D* hFullData = (TH1D *)inputRootFile->Get("MicroBooNE_NCpi0_data");
    TH2D* hFullCov =  (TH2D *)inputRootFile->Get("MicroBooNE_NCpi0_unfcov");
    TH2D* hFullAc =   (TH2D *)inputRootFile->Get("MicroBooNE_NCpi0_Ac");

    int full_bins = hFullData->GetNbinsX()+2;
    auto m_fulldata = std::make_unique<TVectorD>   (full_bins, hFullData->GetArray());
    auto m_fullcov  = std::make_unique<TMatrixDSym>(full_bins, hFullCov->GetArray(), "D");
    auto m_fullac   = std::make_unique<TMatrixD>   (full_bins, full_bins, hFullAc->GetArray(), "D");
    // this needs to be transposed to get the right format for some reason
    m_fullac->Transpose(*m_fullac);
    m_fullcov->Transpose(*m_fullcov);

    // form subset of histograms based on cached Ds
    // loop over the row
    int curr_bin_i = 0;
    int offset = 2000;
    auto f_dist = (this->f_lookup).get_dists();
    for (auto it_i=f_dist.begin(); it_i != f_dist.end(); ++it_i) {
      distribution_t dist_i = *it_i;
      // get the block measurements
      int block_bins_i = (this->f_lookup).get_nbins(dist_i);
      for(int i = 0; i < block_bins_i; i++){
        (*(this->m_data))[curr_bin_i + i] = (*m_fulldata)(dist_i + i + 1 - offset);
        // now loop over the column
        int curr_bin_j = 0;
        for (auto it_j=f_dist.begin(); it_j != f_dist.end(); ++it_j) {
          distribution_t dist_j = *it_j;
          // get the block measurements
          int block_bins_j = (this->f_lookup).get_nbins(dist_j);
          for(int j = 0; j < block_bins_j; j++){
            (*(this->m_ac))(curr_bin_i + i, curr_bin_j + j) = (*m_fullac)(dist_i + i + 1 - offset, dist_j + j + 1 - offset);
            (*(this->m_cov))(curr_bin_i + i, curr_bin_j + j) = (*m_fullcov)(dist_i + i + 1 -offset, dist_j + j + 1 - offset);
          }
          curr_bin_j += block_bins_j;
        } // end column
      }
      curr_bin_i += block_bins_i;
    } // end row
    // free up the memory we just used
    inputRootFile->Close();
  }
};

template <distribution_t D, distribution_t... Ds>
class MicroBooNE_NCpi0_XSec_nu : public Measurement1D {

public:
  /// Basic Constructor.
  MicroBooNE_NCpi0_XSec_nu(nuiskey samplekey);

  /// Virtual Destructor
  ~MicroBooNE_NCpi0_XSec_nu() {};

  /// Apply signal definition
  bool isSignal(FitEvent* nvect);

  /// Fill (but don't actually) kinematic distributions
  void FillEventVariables(FitEvent* customEvent);

  /// we override the base class method and
  /// fill for each block
  /// since an event can belong to multiple bins
  void FillHistograms();

  /// Additional smearing matrix multiplication by Ac
  void ConvertEventRates();

private:
  TMatrixD*      fSmearingMatrix;
  LookupTable    fTable;
  dists_t        fDists;
  std::map<distribution_t, double> fXVars;

};
#endif

