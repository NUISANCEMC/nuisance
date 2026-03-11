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
#ifndef MICROBOONE_CC1MU1P_2D_NU_H_SEEN
#define MICROBOONE_CC1MU1P_2D_NU_H_SEEN

#include <TMatrixDfwd.h>
#include "Measurement1D.h"

#include <iostream>
#include <fstream>
#include <regex>

class TH2D;

class Slice {
public:
  // Constructor
  Slice(std::string name, int id);
  // Virtual destructor
  ~Slice();
  // Add single bin to slice
  void AddBin(int global, double low, double high);
  // Get slice name
  inline std::string GetSliceName() const { return slice_name; };
  // Get slice index
  inline int GetSliceId() const { return slice_id; };
  // Get slice edges
  inline std::pair<double, double> GetSliceEdges() const { return slice_edges; };
  inline double GetSliceWidth() const { return slice_edges.second - slice_edges.first; };
  // Get global bin numbers for slice
  inline int GetNumberBins() const { return global_bins.size(); };
  inline std::vector<int> GetGlobalBins() const { return global_bins; };
  // Get bin edges for slice (different formats)
  inline std::vector<std::pair<double, double>> GetBins() const { return bin_edges; };
  double* GetBinsForTH1() const;

private:
  std::string slice_name;
  int slice_id;
  std::pair<double, double> slice_edges;
  std::vector<std::pair<double, double>> bin_edges;
  std::vector<int> global_bins;

};

class BinScheme {
public:
  // Constructor
  BinScheme();
  // Virtual destructor
  ~BinScheme();
  // Add single bin to scheme
  void AddBin(std::vector<std::string> bin_config);
  void AddBin(std::string slice_name, int global, double low, double high);
  // Get all slices
  inline std::vector<Slice> GetSlices() { return slice_vec; };
  // Get total number of bins
  int GetNumberBins();
  // Get slice containing given global bin number
  Slice GetSliceFromGlobal(int global);

private:
  int current_slice = 0;
  std::vector<Slice> slice_vec;
};

class MicroBooNE_CC1Mu1p_XSec_2D_nu : public Measurement1D {
public:
  /// Basic Constructor
  MicroBooNE_CC1Mu1p_XSec_2D_nu(nuiskey samplekey);

  /// Virtual Destructor
  ~MicroBooNE_CC1Mu1p_XSec_2D_nu() {};

  /// Apply signal definition
  bool isSignal(FitEvent* nvect);

  /// Read histograms in a special way because format is different
  void SetHistograms();

  /// Fill kinematic distributions
  void FillEventVariables(FitEvent* customEvent);

  // Read data histograms from file
  void FillHistograms();

  /// Additional smearing matrix multiplication by Ac
  void ConvertEventRates();

  /// Read bin configuration from file
  void LoadBinScheme();

private:
  enum Distribution { kDeltaPT=0,
                      kDeltaAlphaT=1,
                      kDeltaPhiT=2,
                      kMuonCosTheta=3,
                      kProtonCosTheta=4,
                      kMuonMomentum=5,
                      kProtonMomentum=6,
                      kDeltaPn=7,
                      kDeltaPtx=8,
                      kDeltaPty=9,
                      kECal=10,
                      kEQE=11 };

  std::vector<TH1D *> fMCHist_Slices;
  std::vector<TH1D *> fDataHist_Slices;
  TH2D *fMCHist_Fine2D;
  TMatrixD* fSmearingMatrix;

  // Input distribution information
  std::string fSuffix;
  Distribution fSlice;
  Distribution fDist;
  std::string fSliceTitle;

  BinScheme fBinScheme;

  void FillMCSlice(double x, double y, double w);

};

#endif
