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

#include "TFile.h"
#include "TTree.h"

#include "exception/exception.hxx"

#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

class TH1;
class TH1D;
class TH1F;
class TH2;
class TH2D;
class TH2F;

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
  static std::string name() { return "TH1"; }
};
template <> struct TH_traits<TH1D> {
  static std::string name() { return "TH1D"; }
};
template <> struct TH_traits<TH1F> {
  static std::string name() { return "TH1F"; }
};
template <> struct TH_traits<TH2> {
  static std::string name() { return "TH2"; }
};
template <> struct TH_traits<TH2D> {
  static std::string name() { return "TH2D"; }
};
template <> struct TH_traits<TH2F> {
  static std::string name() { return "TH2F"; }
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

template <typename HT>
inline std::unique_ptr<HT> CloneHistogram(std::unique_ptr<HT> const &source,
                                          bool clear = false) {
  std::unique_ptr<HT> target(dynamic_cast<HT *>(source->Clone()));
  if (!target) {
    throw failed_to_clone()
        << "[ERROR]: Failed to clone a " << TH_traits<HT>::name()
        << ", source = " << source.get();
  }
  target->SetDirectory(nullptr);

  if (clear) {
    target->Clear();
  }

  return target;
}

} // namespace utility
} // namespace nuis

#endif
