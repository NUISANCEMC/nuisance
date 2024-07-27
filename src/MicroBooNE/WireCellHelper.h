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
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>

#include <TH1D.h>
#include <TH2D.h>
#include <TVectorD.h>
#include <TMatrixD.h>
#include <TMatrixDSym.h>

enum distribution_t {
  // text file with bin edges expects the global bin to be offset by this number
  // kAll is handled specially for the CC1Mu0pNp and NCpi0 measurements
  kAllNCpi0 = -2,
  kAllCC = -1,
  // CC1Mu0pNp
  kCC0pNpEMu = 0,
  kCC0pNpCosThetaMu = 22,
  kCC0pNpEnu = 56,
  kCC0pNpTransferEnergy = 76,
  kCC0pNpAvailEnergy = 85,
  kCCProtonKE = 99,
  kCCProtonCosTheta = 114,
  kCCProtonMult = 134,
  kCC0pNpEMuCosThetaMu = 138,
  kCCNpProtonKECosTheta = 262,
  kCCXpEMu = 358,
  kCCXpCosThetaMu = 369,
  kCCXpEMuCosThetaMu = 386,
  kCCXpAvailEnergyCosThetaMuEMu = 455,
  // CCinc 3D
  // the 1000 here is arbitrary, but text file is offset accordingly
  kCCEnuCosThetaMuEMu = 1000,
  // NCpi0
  // the 2000 here is arbitrary, but text file is offset accordingly
  kNC0pNpPpi0 = 2000,
  kNCXpPpi0 = 2012,
  kNC0pNpCosThetaPi0 = 2021,
  kNCXpCosThetaPi0 = 2044,
  kNCXpPpi0CosThetaPi0 = 2061,
};

// define an bin edges container
template <size_t N>
struct e_array {
  constexpr size_t size() { return 2*N; }

  // check if arguments is within the array of edges (low and high edges alternating)
  template <typename... Args>
  bool is_within(Args&& ... values) const {
    std::array<double, sizeof...(values)> v_a({static_cast<double>(values)...});
    if(sizeof...(values) > N) return false;
    int ret = 1;
    for(size_t i = 0; i < v_a.size(); ++i){
      ret *= (v_a.at(i) >= el.at(2*i)) && (v_a.at(i) < el.at(2*i+1));
    }
    return (bool) ret;
  }

  std::array<double, 2*N> el;
};

// don't want to type all these out
template<std::size_t N> using edges_t = std::map<distribution_t, std::vector<e_array<N>>>;
using widths_t     = std::map<distribution_t, std::vector<double>>;
using dimensions_t = std::map<distribution_t, int>;
using bins_t       = std::map<distribution_t, int>;
using dists_t      = std::vector<distribution_t>;

// our main helper class that returns a combination of measurements if asked for
class LookupTable {
public:
  LookupTable(std::string infile="")
    : f_txtfile(infile)
  {
  }
  LookupTable(const LookupTable&) = default;
  LookupTable& operator=(const LookupTable &) = default;
  void operator()(std::string newfile) { f_txtfile = newfile; }

  dimensions_t get_ndims()  const { return f_ndims;  }
  widths_t     get_widths() const { return f_widths; }
  bins_t       get_nbins()  const { return f_nbins;  }
  dists_t      get_dists()  const { return f_dists;  }

  // useful functions for each distribution within the cache
  int get_totalbins() const {
    int n = 0;
    for(auto &i: f_nbins) { n += i.second; }
    return n;
  }
  int get_nbins(distribution_t D) const {
    return f_nbins.at(D);
  }
  double get_width(distribution_t D, int bin) const {
    return f_widths.at(D).at(bin);
  }
  // apply a general function on the bin edges
  // it simply finds the relevant set of bin edges and applies the function on it
  // func expects a vector of bin edges as the first argument and then a variable set of arguments after
  // func also should return a double
  template<typename F, typename... Args>
  double apply(distribution_t D, int bin, F& func, Args&& ... args) const {
    int dim = f_ndims.at(D);
    auto bin_edges = (f_bins.at(D)).at(bin).el;
    // apply the function
    return func(std::vector<double>{bin_edges.begin(), bin_edges.end()},
                std::forward<Args>(args)...);
  }
  // similar to the above, this just finds the local bin number for a given distribution
  // based on input values for individual physics observables
  template <typename... Args>
  int find_bin(distribution_t D, Args&& ... values) const {
    int dim = f_ndims.at(D);
    if(dim != sizeof...(values)) return -1;
    auto bin_list = f_bins.at(D);
    // find bin in which values belongs to
    for(auto it = bin_list.begin(); it != bin_list.end(); ++it){
      if(it->is_within(values...))
        return it - bin_list.begin();
    }
    return -1;
  }

private:
  std::string  f_txtfile;
  bins_t       f_nbins;
  dimensions_t f_ndims;
  widths_t     f_widths;
  dists_t      f_dists;
  // max dimension of measurement is 3
  edges_t<3>   f_bins;

public:
  // actually read in the bin edges text file here
  // require atleast one input D
  template <distribution_t D, distribution_t... Ds>
  void cache_realbins() {
    f_dists = dists_t{D, Ds...};

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
      auto i_dist = std::find(f_dists.begin(), f_dists.end(), global_bin-bin);
      if(i_dist == std::end(f_dists)) continue;
      distribution_t curr_D = f_dists.at(i_dist - f_dists.begin());

      int    n_dim = (lo_vars[2] != -1000.) + (lo_vars[1] != -1000.) + 1;
      double dx3   = (lo_vars[2] != -1000.) ?  hi_vars[2]-lo_vars[2] : 1;
      double dx2   = (lo_vars[1] != -1000.) ?  hi_vars[1]-lo_vars[1] : 1;
      double dx1   = (lo_vars[0] != -1000.) ?  hi_vars[0]-lo_vars[0] : 1;
      double diff  = dx1*dx2*dx3;

      // save our lookup tables
      f_nbins[curr_D] = bin + 1;
      f_widths[curr_D].push_back(diff);
      f_ndims[curr_D] = n_dim;
      f_bins[curr_D].push_back({lo_vars[0], hi_vars[0],
                                lo_vars[1], hi_vars[1],
                                lo_vars[2], hi_vars[2]});
    } // eof
  }
};
// specialize it for all CC blocks (doesn't include CCinc 3D because that's separate)
template <>
inline void LookupTable::cache_realbins<kAllCC>() {
  this->template cache_realbins<
                   kCC0pNpEMu,
                   kCC0pNpCosThetaMu,
                   kCC0pNpEnu,
                   kCC0pNpTransferEnergy,
                   kCC0pNpAvailEnergy,
                   kCCProtonKE,
                   kCCProtonCosTheta,
                   kCCProtonMult,
                   kCC0pNpEMuCosThetaMu,
                   kCCNpProtonKECosTheta,
                   kCCXpEMu,
                   kCCXpCosThetaMu,
                   kCCXpEMuCosThetaMu,
                   kCCXpAvailEnergyCosThetaMuEMu
                  >();
}
// specialization for all NCpi0 blocks
template <>
inline void LookupTable::cache_realbins<kAllNCpi0>() {
  this->template cache_realbins<
                   kNC0pNpPpi0,
                   kNCXpPpi0,
                   kNC0pNpCosThetaPi0,
                   kNCXpCosThetaPi0,
                   kNCXpPpi0CosThetaPi0
                  >();
}

// helper classes for various wirecell measurements
template <distribution_t D, distribution_t... Ds>
class IWireCellHelper {
public:
  IWireCellHelper(std::string txtfile) {
    f_lookup(txtfile);
    f_lookup.cache_realbins<D, Ds...>();

    int nbins = f_lookup.get_totalbins();
    m_data     = new TVectorD(nbins);
    m_cov      = new TMatrixDSym(nbins);
    m_ac       = new TMatrixD(nbins, nbins);
  }

  TH1D*        get_data()        const { return new TH1D(*m_data); }
  TH2D*        get_cov()         const { return new TH2D(*m_cov); }
  TH2D*        get_ac()          const { return new TH2D(*m_ac); }
  TVectorD*    get_data_v()      const { return m_data; }
  TMatrixDSym* get_cov_m()       const { return m_cov; }
  TMatrixD*    get_ac_m()        const { return m_ac; }

  LookupTable  get_lookuptable() const { return f_lookup; }
  // implement your own method to load
  // a large blockwise reported result or otherwise
  // and distribute into the helper class' measurements
  virtual void load_measurement() = 0;

protected:
  LookupTable  f_lookup;
  TVectorD*    m_data = NULL;
  TMatrixDSym* m_cov  = NULL;
  TMatrixD*    m_ac   = NULL;
};

// the below functions are useful for Enu based measurements which wirecell has multiple results of
// get the flux fraction based on energy ranges
// expects energy range to be in the 1st two elements
inline double GetFluxFraction(std::vector<double> edges, TH1D* fluxHist){
  int lo_bin = fluxHist->FindBin(edges[0]);
  int hi_bin = fluxHist->FindBin(edges[1]);

  double lo_width   = fluxHist->GetBinWidth(lo_bin);
  double hi_width   = fluxHist->GetBinWidth(hi_bin);
  double lo_content = fluxHist->GetBinContent(lo_bin);
  double hi_content = fluxHist->GetBinContent(hi_bin);
  double lo_edge    = fluxHist->GetBinLowEdge(lo_bin);
  double hi_edge    = fluxHist->GetBinLowEdge(hi_bin+1);

  // get the total integral
  double tot_flux = fluxHist->Integral(lo_bin, hi_bin);
  // remove fraction upto the low edge and from the high edge
  tot_flux -= (edges[0]-lo_edge)*lo_content/lo_width;
  tot_flux -= (hi_edge-edges[1])*hi_content/hi_width;
  // fraction of total flux
  tot_flux /= fluxHist->Integral();

  return tot_flux;
}

// get the bin width based on energy ranges
// expects energy range to be in the 1st two elements
inline double GetEnergyBinWidth(std::vector<double> edges){
  return (edges[1] - edges[0]);
}
