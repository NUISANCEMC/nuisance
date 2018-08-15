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

namespace nuis {
namespace utility {

NEW_NUIS_EXCEPT(failed_to_open_TFile);
NEW_NUIS_EXCEPT(failed_to_get_TTree);

inline TFile *CheckOpenTFile(std::string const &fname, char const *opts="") {

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
                                               std::string const &tname) {
  TreeFile tf;
  tf.file = CheckOpenTFile(fname);
  tf.tree = dynamic_cast<TTree *>(tf.file->Get(tname.c_str()));
  if (!tf.tree) {
    throw failed_to_get_TTree()
        << "[ERROR]: Failed to get TTree named: " << std::quoted(tname)
        << " from TFile: " << std::quoted(fname);
  }
  return std::make_unique<TreeFile>(std::move(tf));
}

} // namespace utility
} // namespace nuis

#endif
