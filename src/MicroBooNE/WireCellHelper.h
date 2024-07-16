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
  // text file with bin edges expects the global bin to be offset by this number
  // kAll is handled specially for the CC1Mu0pNp measurement
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
  kXpAvailEnergyCosThetaMuEMu = 455,
  // separate file and data release
  // the 1000 here is arbitrary, but text file is offset accordingly
  kEnuCosThetaMuEMu = 1000
};

// some of CC1Mu0pNp reported measurements is "joint" 0pNp
// with a given observable but split into different hadron final states
// we store those boundaries
static std::map<distribution_t, int> kProtonBoundary = {
  {distribution_t::k0pNpEMu, 11},
  {distribution_t::k0pNpCosThetaMu, 39},
  {distribution_t::k0pNpEnu, 66},
  {distribution_t::k0pNpTransferEnergy, 79},
  {distribution_t::k0pNpAvailEnergy, 90},
  {distribution_t::k0pNpEMuCosThetaMu, 193}
};

// don't want to type all these out
template<std::size_t N> using edges_t = std::map<distribution_t, std::vector<std::array<double, 2*N>>>;
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

  int get_totalbins() const {
    int n = 0;
    for(auto &i: f_nbins)
      n += i.second;
    return n;
  }
  // useful functions for each distribution within the cache
  int get_nbins(distribution_t D) const {
    assert(!(D == distribution_t::kAll) && "Invalid Lookup!");
    return f_nbins.at(D);
  }
  double get_width(distribution_t D, int bin) const {
    assert(!(D == distribution_t::kAll) && "Invalid Lookup!");
    return f_widths.at(D).at(bin);
  }
  // apply a general function on the bin edges
  // it simply finds the relevant set of bin edges and applies the function on it
  // func expects a vector of bin edges as the first argument and then a variable set of arguments after
  // func also should return a double
  template<typename F, typename... Args>
  double apply(distribution_t D, int bin, F func, Args&& ... args) const {
    assert(!(D == distribution_t::kAll) && "Invalid Lookup!");
    int dim = f_ndims.at(D);
    // this might be a bit ugly but feel like it comes together later
    if(dim == 1){
      auto bin_edges = (f_bins_1d.at(D)).at(bin);
      return func(std::vector<double>{bin_edges.begin(), bin_edges.end()},
                  args...);
    }
    if(dim == 2){
      auto bin_edges = (f_bins_2d.at(D)).at(bin);
      return func(std::vector<double>{bin_edges.begin(), bin_edges.end()},
                  args...);
    }
    if(dim == 3){
      auto bin_edges = (f_bins_3d.at(D)).at(bin);
      return func(std::vector<double>{bin_edges.begin(), bin_edges.end()},
                  args...);
    }
    return -1.;
  }
  // similar to the above, this just finds the local bin number for a given distribution
  // based on input values for individual physics observables
  template <typename... Args>
  int find_bin(distribution_t D, Args&& ... values) const {
    assert(!(D == distribution_t::kAll) && "Invalid Lookup!");

    std::array<double, sizeof...(values)> array_vals({static_cast<double>(values)...});
    int dim = f_ndims.at(D);
    if(dim != sizeof...(values)) return -1;
    // this might be a bit ugly but feel like it comes together later
    if(dim == 1){
      auto bin_list = f_bins_1d.at(D);
      for(auto it = bin_list.begin(); it != bin_list.end(); ++it){
        if(array_vals[0] >= it->at(0) && array_vals[0] < it->at(1))
          return it - bin_list.begin();
      }
    }
    if(dim == 2){
      auto bin_list = f_bins_2d.at(D);
      for(auto it = bin_list.begin(); it != bin_list.end(); ++it){
        if(array_vals[0] >= it->at(0) && array_vals[0] < it->at(1) &&
           array_vals[1] >= it->at(2) && array_vals[1] < it->at(3))
          return it - bin_list.begin();
      }
    }
    if(dim == 3){
      auto bin_list = f_bins_3d.at(D);
      for(auto it = bin_list.begin(); it != bin_list.end(); ++it){
        if(array_vals[0] >= it->at(0) && array_vals[0] < it->at(1) &&
           array_vals[1] >= it->at(2) && array_vals[1] < it->at(3) &&
           array_vals[2] >= it->at(4) && array_vals[2] < it->at(5))
          return it - bin_list.begin();
      }
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
  edges_t<3>   f_bins_3d;
  edges_t<2>   f_bins_2d;
  edges_t<1>   f_bins_1d;

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
      // for Enu 1D or 3D or proton multiplicity, we don't divide by bin widths anyway
      // need flux scaling which will be done later
      if(curr_D == k0pNpEnu || curr_D == kProtonMult)
        diff = 1.;
      if(curr_D == kEnuCosThetaMuEMu)
        diff = dx2*dx3;

      // save our lookup tables
      f_nbins[curr_D] = bin + 1;
      f_widths[curr_D].push_back(diff);

      // add an extra dimension for 0p Np lookup table with a 35 MeV proton KE boundary
      if((curr_D <= k0pNpAvailEnergy) || (curr_D == k0pNpEMuCosThetaMu)){
        if(global_bin < kProtonBoundary[curr_D]){
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
        f_bins_1d[curr_D].push_back({lo_vars[0], hi_vars[0]});
      if(n_dim == 2)
        f_bins_2d[curr_D].push_back({lo_vars[0], hi_vars[0],
                                     lo_vars[1], hi_vars[1]});
      if(n_dim == 3)
        f_bins_3d[curr_D].push_back({lo_vars[0], hi_vars[0],
                                     lo_vars[1], hi_vars[1],
                                     lo_vars[2], hi_vars[2]});
    } // eof
  }
};
// specialize it for all bins (doesn't include CCinc 3D because that's separate)
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
  TMatrixDSym* m_cov  = NULL;
  TMatrixD*    m_ac   = NULL;
  TVectorD*    m_data = NULL;
};

// get the flux fraction based on energy ranges
// expects energy range to be in the 1st two elements
// include this here because wirecell reports multiple Enu based measurements
double GetFluxFraction(std::vector<double> edges, TH1D* fluxHist){
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
};
