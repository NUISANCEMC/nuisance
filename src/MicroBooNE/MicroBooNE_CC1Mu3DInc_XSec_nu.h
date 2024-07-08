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
#ifndef MICROBOONE_CC1MU3DINC_NU_H_SEEN
#define MICROBOONE_CC1MU3DINC_NU_H_SEEN

#pragma once

#include <TMatrixDfwd.h>
#include "Measurement1D.h"
#include "WCAnaHelper.h"

class TH1D;
class TH2D;
class TFile;

class CCInc3DHelper {

public:
  CCInc3DHelper() {

    f_lookup(FitPar::GetDataBase() +
             "/MicroBooNE/CCinc3D/real_bins.txt");
    f_lookup.cache_realbins<kEnuCosThetaMuEMu>();

    int nbins = f_lookup.get_totalbins();
    m_data     = new TVectorD(nbins);
    m_cov      = new TMatrixDSym(nbins);
    m_ac       = new TMatrixD(nbins, nbins);
  }

  TH1D* get_data()         const { return new TH1D(*m_data); }
  TH2D* get_cov()          const { return new TH2D(*m_cov); }
  TH2D* get_ac()           const { return new TH2D(*m_ac); }
  TVectorD* get_data_v()   const { return m_data; }
  TMatrixDSym* get_cov_m() const { return m_cov; }
  TMatrixD* get_ac_m()     const { return m_ac; }

  LookupTable get_lookuptable() const { return f_lookup; }

private:
  LookupTable f_lookup;
  TMatrixDSym* m_cov = NULL;
  TMatrixD* m_ac = NULL;
  TVectorD* m_data = NULL;

public:
  void load_measurement() {

    // load our histograms
    TFile* inputRootFile = TFile::Open((FitPar::GetDataBase() +
                                       "/MicroBooNE/CCinc3D/MicroBooNE_CCInc3D_data_release.root").c_str());
    TH1D* hFullData = (TH1D *)inputRootFile->Get("MicroBooNE_CCInc3D_data");
    TH2D* hFullCov =  (TH2D *)inputRootFile->Get("MicroBooNE_CCInc3D_unfcov");
    TH2D* hFullAc =   (TH2D *)inputRootFile->Get("MicroBooNE_CCInc3D_Ac");

    int full_bins = hFullData->GetNbinsX()+2;
    TVectorD*    m_fulldata = new TVectorD    (full_bins, hFullData->GetArray());
    TMatrixDSym* m_fullcov  = new TMatrixDSym (full_bins, hFullCov->GetArray(), "D");
    TMatrixD*    m_fullac   = new TMatrixD    (full_bins, full_bins, hFullAc->GetArray(), "D");
    // this needs to be transposed to get the right format for some reason
    // for the covariance its okay, since its symmetric
    m_fullac->Transpose(*m_fullac);
    *m_data = m_fulldata->GetSub(1, full_bins - 2);
    *m_cov  = m_fullcov ->GetSub(1, full_bins - 2,
                                 1, full_bins - 2);
    *m_ac   = m_fullac  ->GetSub(1, full_bins - 2,
                                 1, full_bins - 2);
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

class MicroBooNE_CC1Mu3DInc_XSec_nu : public Measurement1D {

public:
  /// Basic Constructor.
  MicroBooNE_CC1Mu3DInc_XSec_nu(nuiskey samplekey);

  /// Virtual Destructor
  ~MicroBooNE_CC1Mu3DInc_XSec_nu() {};

  /// Apply signal definition
  bool isSignal(FitEvent* nvect);

  /// Fill kinematic distributions
  void FillEventVariables(FitEvent* customEvent);

  /// Additional smearing matrix multiplication by Ac
  void ConvertEventRates();

private:
  TMatrixD*      fSmearingMatrix;
  LookupTable    fTable;

};
#endif
