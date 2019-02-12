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
#include "TTree.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

namespace nuis {
namespace utility {

NEW_NUIS_EXCEPT(failed_to_open_TFile);
NEW_NUIS_EXCEPT(failed_to_get_TTree);

inline void CloseTFile(TFile *f = nullptr) {
  if (f) {
    std::cout << "[INFO]: Shutting TFile: " << f->GetName() << ", " << f
              << std::endl;
    f->Close();
  }
  delete f;
}

using TFile_ptr = std::unique_ptr<TFile, decltype(&CloseTFile)>;
inline TFile_ptr make_unique_TFile(TFile *f) {
  return TFile_ptr(f, &CloseTFile);
}

inline TFile_ptr CheckOpenTFile(std::string const &fname,
                                char const *opts = "") {

  TDirectory *ogDir = gDirectory;

  TFile *inpF = new TFile(fname.c_str(), opts);
  if (!inpF || !inpF->IsOpen()) {
    throw failed_to_open_TFile()
        << "[ERROR]: Couldn't open input file: " << std::quoted(fname);
  }

  if (ogDir) {
    ogDir->cd();
  }

  return make_unique_TFile(inpF);
}

struct TreeFile {
  TFile_ptr file;
  TTree *tree;
  bool file_owned;
  TreeFile()
      : file(make_unique_TFile(nullptr)), tree(nullptr), file_owned(false) {}
  TreeFile(TreeFile const &other) = delete;
  TreeFile &operator=(TreeFile &&other) {
    file = std::move(other.file);
    tree = other.tree;
    file_owned = other.file_owned;

    other.file = nullptr;
    other.tree = nullptr;
    other.file_owned = false;

    return *this;
  }

  TreeFile(TreeFile &&other)
      : file(std::move(other.file)), tree(other.tree),
        file_owned(other.file_owned) {
    other.file = nullptr;
    other.tree = nullptr;
    other.file_owned = false;
  }
  ~TreeFile() {
    if (!file_owned) {
      file.release();
    }
  }
};

inline TreeFile CheckGetTTree(TFile *file, std::string const &tname) {
  TreeFile tf;
  tf.file = make_unique_TFile(file);
  tf.tree = dynamic_cast<TTree *>(tf.file->Get(tname.c_str()));
  tf.file_owned = false;

  if (!tf.tree) {
    throw failed_to_get_TTree()
        << "[ERROR]: Failed to get TTree named: " << std::quoted(tname)
        << " from TFile: " << std::quoted(file->GetName());
  }
  std::cout << "[INFO]: Opened TFile: " << file->GetName() << ", " << file
            << std::endl;
  return tf;
}

inline TreeFile CheckGetTTree(std::string const &fname,
                              std::string const &tname, char const *opts = "") {
  TreeFile tf = CheckGetTTree(CheckOpenTFile(fname, opts).release(), tname);
  tf.file_owned = true;
  return tf;
}

inline TreeFile MakeNewTTree(std::string const &fname, std::string const &tname,
                             char const *opts = "") {
  TreeFile tf;
  tf.file = CheckOpenTFile(fname, opts);
  tf.tree = new TTree(tname.c_str(), "");
  tf.tree->SetDirectory(tf.file.get());
  tf.file_owned = true;
  return tf;
}

inline std::string SanitizeROOTObjectName(std::string name) { return name; }

} // namespace utility
} // namespace nuis

#endif
