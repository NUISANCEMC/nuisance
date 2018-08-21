// Copyright 2018 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#ifndef UTILITY_ROOTUTILITY_HXX_SEEN
#define UTILITY_ROOTUTILITY_HXX_SEEN

#include "exception/exception.hxx"

#include "TFile.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TH2F.h"
#include "TTree.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

namespace nuis {
namespace utility {

NEW_NUIS_EXCEPT(failed_to_open_TFile);
NEW_NUIS_EXCEPT(failed_to_get_TTree);
NEW_NUIS_EXCEPT(invalid_histogram_name);
NEW_NUIS_EXCEPT(failed_to_clone);

inline TFile *CheckOpenTFile(std::string const &fname, char const *opts = "") {

  TDirectory *ogDir = gDirectory;

  TFile *inpF = new TFile(fname.c_str(), opts);
  if (!inpF || !inpF->IsOpen()) {
    throw failed_to_open_TFile()
        << "[ERROR]: Couldn't open input file: " << std::quoted(fname);
  }

  if (ogDir) {
    ogDir->cd();
  }

  return inpF;
}

struct TreeFile {
  TFile *file;
  TTree *tree;
  TreeFile() : file(nullptr), tree(nullptr) {}
  TreeFile(TreeFile const &other) = delete;
  TreeFile(TreeFile &&other) : file(other.file), tree(other.tree) {
    other.file = nullptr;
    other.tree = nullptr;
  }
  ~TreeFile() {
    if (file) {
      file->Close();
    }
  }
};

inline std::unique_ptr<TreeFile> CheckGetTTree(std::string const &fname,
                                               std::string const &tname,
                                               char const *opts = "") {
  TreeFile tf;
  tf.file = CheckOpenTFile(fname, opts);
  tf.tree = dynamic_cast<TTree *>(tf.file->Get(tname.c_str()));
  if (!tf.tree) {
    throw failed_to_get_TTree()
        << "[ERROR]: Failed to get TTree named: " << std::quoted(tname)
        << " from TFile: " << std::quoted(fname);
  }
  return std::make_unique<TreeFile>(std::move(tf));
}

inline std::unique_ptr<TreeFile> MakeNewTTree(std::string const &fname,
                                              std::string const &tname,
                                              char const *opts = "") {
  TreeFile tf;
  tf.file = CheckOpenTFile(fname, opts);
  tf.tree = new TTree(tname.c_str(), "");
  tf.tree->SetDirectory(tf.file);
  return std::make_unique<TreeFile>(std::move(tf));
}

template <typename HT> struct TH_traits {};

template <> struct TH_traits<TH1> {
  static size_t const NDims = 1;
  static std::string name() { return "TH1"; }
  static Int_t NbinsIncludeFlow(TH1 *const h) {
    return h->GetXaxis()->GetNbins() + 2;
  }
};
template <> struct TH_traits<TH1D> {
  static size_t const NDims = 1;
  static std::string name() { return "TH1D"; }
  static Int_t NbinsIncludeFlow(TH1D *const h) {
    return h->GetXaxis()->GetNbins() + 2;
  }
};
template <> struct TH_traits<TH1F> {
  static size_t const NDims = 1;
  static std::string name() { return "TH1F"; }
  static Int_t NbinsIncludeFlow(TH1F *const h) {
    return h->GetXaxis()->GetNbins() + 2;
  }
};
template <> struct TH_traits<TH2> {
  static size_t const NDims = 2;
  static std::string name() { return "TH2"; }
  static Int_t NbinsIncludeFlow(TH2 *const h) {
    return (h->GetXaxis()->GetNbins() + 2) * (h->GetYaxis()->GetNbins() + 2);
  }
};
template <> struct TH_traits<TH2D> {
  static size_t const NDims = 2;
  static std::string name() { return "TH2D"; }
  static Int_t NbinsIncludeFlow(TH2D *const h) {
    return (h->GetXaxis()->GetNbins() + 2) * (h->GetYaxis()->GetNbins() + 2);
  }
};
template <> struct TH_traits<TH2F> {
  static size_t const NDims = 2;
  static std::string name() { return "TH2F"; }
  static Int_t NbinsIncludeFlow(TH2F *const h) {
    return (h->GetXaxis()->GetNbins() + 2) * (h->GetYaxis()->GetNbins() + 2);
  }
};

template <typename HT>
inline std::unique_ptr<HT> GetHistogramFromROOTFile(std::string const &fname,
                                                    std::string const &hname) {
  TFile *f = CheckOpenTFile(fname, "READ");
  HT *h = dynamic_cast<HT *>(f->Get(hname.c_str()));
  if (!h) {
    throw invalid_histogram_name()
        << "[ERROR]: Failed to get " << TH_traits<HT>::name() << " named "
        << std::quoted(hname) << " from input file " << std::quoted(fname);
  }
  std::unique_ptr<HT> clone(dynamic_cast<HT *>(h->Clone()));
  clone->SetDirectory(nullptr);

  f->Close();
  delete f;

  return clone;
}

template <typename HT> void Clear(HT *h) {
  for (Int_t bin_it = 0; bin_it < TH_traits<HT>::NbinsIncludeFlow(h);
       ++bin_it) {
    h->SetBinContent(bin_it, 0);
    h->SetBinError(bin_it, 0);
  }
}

// Fill for 1D histograms uses type system to enforce correct number of
// dimensions
template <typename HT, typename T>
typename std::enable_if<TH_traits<HT>::NDims == 1, void>::type
Fill(HT *h, std::array<T, TH_traits<HT>::NDims> const &val, double weight = 1) {
  h->Fill(val[0], weight);
}

/// Fill for 2D histograms uses type system to enforce correct number of
/// dimensions
template <typename HT, typename T>
typename std::enable_if<TH_traits<HT>::NDims == 2, void>::type
Fill(HT *h, std::array<T, TH_traits<HT>::NDims> const &val, double weight = 1) {
  h->Fill(val[0], val[1], weight);
}

template <typename HT>
inline std::unique_ptr<HT> Clone(std::unique_ptr<HT> const &source,
                                 bool clear = false) {
  std::unique_ptr<HT> target(dynamic_cast<HT *>(source->Clone()));
  if (!target) {
    throw failed_to_clone()
        << "[ERROR]: Failed to clone a " << TH_traits<HT>::name()
        << ", source = " << source.get();
  }
  target->SetDirectory(nullptr);

  if (clear) {
    Clear(target.get());
  }

  return target;
}

} // namespace utility
} // namespace nuis

#endif
