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
#ifndef MICROBOONE_CC1MU0pNp_NU_H_SEEN
#define MICROBOONE_CC1MU0pNp_NU_H_SEEN

#pragma once

#include <TMatrixDfwd.h>
#include "Measurement1D.h"
#include "WireCellHelper.h"

class TH1D;
class TH2D;
class TFile;

template <distribution_t D, distribution_t... Ds>
class CC1Mu0pNpHelper : public IWireCellHelper<D, Ds...> {
public:
  CC1Mu0pNpHelper() :
    IWireCellHelper<D, Ds...>(FitPar::GetDataBase() +
                              "/MicroBooNE/CC1Mu0pNp/real_bins.txt")
  {}
  // this gives us the measurements based on the blocks we requested
  void load_measurement() override {

    // load our histograms
    TFile* inputRootFile = TFile::Open((FitPar::GetDataBase() +
                                       "/MicroBooNE/CC1Mu0pNp/MicroBooNE_CC1Mu0pNp_data_release.root").c_str());
    TH1D* hFullData = (TH1D *)inputRootFile->Get("MicroBooNE_CC1Mu0pNp_data");
    TH2D* hFullCov =  (TH2D *)inputRootFile->Get("MicroBooNE_CC1Mu0pNp_unfcov");
    TH2D* hFullAc =   (TH2D *)inputRootFile->Get("MicroBooNE_CC1Mu0pNp_Ac");

    int full_bins = hFullData->GetNbinsX()+2;
    TVectorD*    m_fulldata = new TVectorD    (full_bins, hFullData->GetArray());
    TMatrixDSym* m_fullcov  = new TMatrixDSym (full_bins, hFullCov->GetArray(), "D");
    TMatrixD*    m_fullac   = new TMatrixD    (full_bins, full_bins, hFullAc->GetArray(), "D");
    // this needs to be transposed to get the right format for some reason
    // for the covariance its okay, since its symmetric
    m_fullac->Transpose(*m_fullac);
    // form subset of histograms based on cached Ds
    int curr_bin = 0;
    auto f_dist = (this->f_lookup).get_dists();
    for (auto it=f_dist.begin(); it != f_dist.end(); ++it) {
      distribution_t dist = *it;
      // get the sub measurements
      int  sub_bins = (this->f_lookup).get_nbins(dist);
      auto sub_data = m_fulldata->GetSub(dist+1, dist+sub_bins);
      auto sub_cov  = m_fullcov ->GetSub(dist+1, dist+sub_bins,
                                         dist+1, dist+sub_bins);
      auto sub_ac   = m_fullac  ->GetSub(dist+1, dist+sub_bins,
                                         dist+1, dist+sub_bins);
      // store it in our new bins
      (this->m_data)->SetSub(curr_bin, sub_data);
      (this->m_cov) ->SetSub(curr_bin, sub_cov);
      (this->m_ac)  ->SetSub(curr_bin, curr_bin, sub_ac);

      curr_bin += sub_bins;
    }
    // free up the memory we just used
    hFullData->Reset();
    hFullCov ->Reset();
    hFullAc  ->Reset();
    delete m_fulldata;
    delete m_fullcov;
    delete m_fullac;
    inputRootFile->Close();
  }
};

template <distribution_t D, distribution_t... Ds>
class MicroBooNE_CC1Mu0pNp_XSec_nu : public Measurement1D {

public:
  /// Basic Constructor.
  MicroBooNE_CC1Mu0pNp_XSec_nu(nuiskey samplekey);

  /// Virtual Destructor
  ~MicroBooNE_CC1Mu0pNp_XSec_nu() {};

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
