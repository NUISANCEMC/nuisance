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

#include <TMatrixDfwd.h>
#include "Measurement1D.h"
#include "MicroBooNE_CC1Mu1p_XSec_1D_nu.h"

#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
#include <array>
#include <vector>
#include <map>
#include <cassert>

class TH1D;
class TH2D;
class TFile;

enum distribution_t {
  kAll = -1,
  k0pNpEMu = 0,
  k0pNpCosThetaMu = 22,
  k0pNpEnu = 56,
  k0pNpTransferEnergy = 76,
  k0pNpAvailEnergy = 85,
  kProtonKE = 99,
  kProtonCosTheta = 114,
  kProtonMult = 134,
  k0pNpEMuCosThetaMu = 138,
  kNpProtonKECosTheta = 262,
  kXpEMu = 358,
  kXpCosThetaMu = 369,
  kXpEMuCosThetaMu = 386,
  kXpAvailEnergyCosThetaMuEMu = 455
};

static std::map<distribution_t, int> kProtonBoundary = {
  {distribution_t::k0pNpEMu, 11},
  {distribution_t::k0pNpCosThetaMu, 39},
  {distribution_t::k0pNpEnu, 66},
  {distribution_t::k0pNpTransferEnergy, 79},
  {distribution_t::k0pNpAvailEnergy, 90},
  {distribution_t::k0pNpEMuCosThetaMu, 193}
};

template<std::size_t N> using edges_t = std::map<distribution_t, std::vector<std::array<double, 2*N>>>;
using widths_t = std::map<distribution_t, std::vector<double>>;
using dimensions_t = std::map<distribution_t, int>;
using bins_t = std::map<distribution_t, int>;

class LookupTable {
public:
  LookupTable(std::string infile="")
    : f_txtfile(infile)
  {
  }
  LookupTable(const LookupTable&) = default;
  LookupTable& operator=(const LookupTable &) = default;
  void operator()(std::string newfile) { f_txtfile = newfile; }

  dimensions_t get_ndims() const { return f_ndims; }
  widths_t get_widths()    const { return f_widths; }
  bins_t get_nbins()       const { return f_nbins; }

  int get_nbins(distribution_t D) const {
    assert(!(D == distribution_t::kAll));
    return f_nbins.at(D);
  }

  int get_totalbins() const {
    int n = 0;
    for(auto &i: f_nbins)
      n += i.second;
    return n;
  }

  double get_width(distribution_t D, int bin) const {
    assert(!(D == distribution_t::kAll));
    return f_widths.at(D).at(bin);
  }

  template <distribution_t D, typename... Args>
  int find_bin(Args&& ... values){
    static_assert(!(D == distribution_t::kAll), "Invalid lookup!");

    std::array<double, sizeof...(values)> array_vals({static_cast<double>(values)...});
    int dim = f_ndims[D];
    if(dim != sizeof...(values)) return -1;

    if(dim == 1){
      auto bin_list = f_bins_1d[D];
      for(auto it = bin_list.begin(); it != bin_list.end(); ++it){
        if(array_vals[0] > it->at(0) && array_vals[0] <= it->at(1))
          return it - bin_list.begin();
      }
    }
    if(dim == 2){
      auto bin_list = f_bins_2d[D];
      for(auto it = bin_list.begin(); it != bin_list.end(); ++it){
        if(array_vals[0] > it->at(0) && array_vals[0] <= it->at(1) &&
           array_vals[1] > it->at(2) && array_vals[1] <= it->at(3))
          return it - bin_list.begin();
      }
    }
    if(dim == 3){
      auto bin_list = f_bins_3d[D];
      for(auto it = bin_list.begin(); it != bin_list.end(); ++it){
        if(array_vals[0] > it->at(0) && array_vals[0] <= it->at(1) &&
           array_vals[1] > it->at(2) && array_vals[1] <= it->at(3) &&
           array_vals[2] > it->at(4) && array_vals[2] <= it->at(5))
          return it - bin_list.begin();
      }
    }
    return -1;
  }

private:
  std::string f_txtfile;
  bins_t f_nbins;
  dimensions_t f_ndims;
  widths_t f_widths;
  // max dimension of measurement is 3
  edges_t<3> f_bins_3d;
  edges_t<2> f_bins_2d;
  edges_t<1> f_bins_1d;

public:
  // require atleast one input D
  template <distribution_t D, distribution_t... Ds>
  void cache_realbins() {

    constexpr std::array<distribution_t, sizeof...(Ds) + 1> array_dist({D, Ds...});

    std::string line;
    // Skip the header line which contains the data column names.
    std::ifstream inputfile(f_txtfile);
    std::getline( inputfile, line );
    while(std::getline(inputfile, line)) {
      if(line == "end") break;

      // Read the given line.
      int global_bin, bin;
      double lo_vars[3], hi_vars[3];
      std::stringstream ss( line );
      ss >> global_bin >> bin >>
        lo_vars[2] >> hi_vars[2] >>
        lo_vars[1] >> hi_vars[1] >>
        lo_vars[0] >> hi_vars[0];

      // difference between global and local bin indices are constant when we are within the same distribution
      auto i_dist = std::find(array_dist.begin(), array_dist.end(),
                             global_bin-bin);
      if(i_dist == std::end(array_dist))
        continue;
      distribution_t curr_D = array_dist.at(i_dist - array_dist.begin());

      int    n_dim = (lo_vars[2] != -1000.) + (lo_vars[1] != -1000.) + 1;
      double dx3   = (lo_vars[2] != -1000.) ? hi_vars[2]-lo_vars[2] : 1;
      double dx2   = (lo_vars[1] != -1000.) ? hi_vars[1]-lo_vars[1] : 1;
      double dx1   = (lo_vars[0] != -1000.) ? hi_vars[0]-lo_vars[0] : 1;
      double diff  = dx1*dx2*dx3;

      // save our lookup tables
      f_nbins[curr_D] = bin + 1;
      f_widths[curr_D].push_back(diff);

      // add an extra dimension for 0p Np lookup table with a 35 MeV proton KE boundary
      if((curr_D <= k0pNpAvailEnergy) || (curr_D == k0pNpEMuCosThetaMu)){
        if(bin < kProtonBoundary[curr_D]){
          lo_vars[n_dim] = std::numeric_limits<double>::lowest();
          hi_vars[n_dim] = 0.035;
        }
        else{
          lo_vars[n_dim] = 0.035;
          hi_vars[n_dim] = std::numeric_limits<double>::max();
        }
        n_dim += 1; // extra for 0pNp dimensions
      }

      f_ndims[curr_D] = n_dim;
      if(n_dim == 1)
        f_bins_1d[curr_D].push_back({lo_vars[0], hi_vars[0]
                                            });
      if(n_dim == 2)
        f_bins_2d[curr_D].push_back({lo_vars[0], hi_vars[0],
                                              lo_vars[1], hi_vars[1]
                                            });
      if(n_dim == 3)
        f_bins_3d[curr_D].push_back({lo_vars[0], hi_vars[0],
                                              lo_vars[1], hi_vars[1],
                                              lo_vars[2], hi_vars[2]
                                            });
    } // eof
  }
};
// specialize it for all bins
template <>
void LookupTable::cache_realbins<distribution_t::kAll>() {
  this->template cache_realbins<
                   k0pNpEMu,
                   k0pNpCosThetaMu,
                   k0pNpEnu,
                   k0pNpTransferEnergy,
                   k0pNpAvailEnergy,
                   kProtonKE,
                   kProtonCosTheta,
                   kProtonMult,
                   k0pNpEMuCosThetaMu,
                   kNpProtonKECosTheta,
                   kXpEMu,
                   kXpCosThetaMu,
                   kXpEMuCosThetaMu,
                   kXpAvailEnergyCosThetaMuEMu
                  >();
};

template <distribution_t D, distribution_t... Ds>
class CC1Mu0pNpHelper {

public:
  CC1Mu0pNpHelper() {

    f_lookup(FitPar::GetDataBase() +
             "/MicroBooNE/CC1Mu0pNp/real_bins.txt");
    f_lookup.cache_realbins<D, Ds...>();

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
  std::array<distribution_t, sizeof...(Ds) + 1> f_dist{D, Ds...};
  LookupTable f_lookup;
  TMatrixDSym* m_cov = NULL;
  TMatrixD* m_ac = NULL;
  TVectorD* m_data = NULL;

public:
  void load_measurement() {

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
    // form subset of histograms based on cached Ds
    int curr_bin = 0;
    for (auto it=this->f_dist.begin(); it != this->f_dist.end(); ++it) {
      distribution_t dist = *it;
      // get the sub measurements
      int  sub_bins = f_lookup.get_nbins(dist);
      auto sub_data = m_fulldata->GetSub(dist+1, dist+sub_bins);
      auto sub_cov  = m_fullcov ->GetSub(dist+1, dist+sub_bins,
                                         dist+1, dist+sub_bins);
      auto sub_ac   = m_fullac  ->GetSub(dist+1, dist+sub_bins,
                                         dist+1, dist+sub_bins);
      // store it in our new bins
      m_data->SetSub(curr_bin, sub_data);
      m_cov ->SetSub(curr_bin, sub_cov);
      m_ac  ->SetSub(curr_bin, curr_bin, sub_ac);

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

template <distribution_t D>
class MicroBooNE_CC1Mu0pNp_XSec_nu : public Measurement1D {

public:
  /// Basic Constructor.
  MicroBooNE_CC1Mu0pNp_XSec_nu(nuiskey samplekey);

  /// Virtual Destructor
  ~MicroBooNE_CC1Mu0pNp_XSec_nu() {};

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
