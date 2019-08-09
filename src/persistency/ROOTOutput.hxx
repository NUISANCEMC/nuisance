#pragma once

#include "exception/exception.hxx"

#include "utility/ROOTUtility.hxx"

#include "TFile.h"

#include <iostream>
#include <memory>
#include <string>

namespace nuis {
namespace persistency {

NEW_NUIS_EXCEPT(WriteToOutputFile_nullptr);

/// Will get/open a TFile that is described in the global config
///
/// The named streams will be used to configure the file name and open mode from
/// the global config element persistency.<name>: {file: output.root opts:
/// CREATE}
std::unique_ptr<TFile> &GetOutputFile(std::string const &name = "default");

void NewStream(std::string const &name = "default",
               std::string file_name = "default.nuis.root",
               std::string opts = "CREATE");

template <typename T>
inline void WriteToOutputFile(T *object, std::string const &object_name,
                              std::string dir_name = "",
                              std::string const &file_name = "default") {

  if (!object) {
    throw WriteToOutputFile_nullptr();
  }

  TDirectory *ogdir = gDirectory;

  std::unique_ptr<TFile> &f = GetOutputFile(file_name);

  TDirectory *d = f.get();
  while (dir_name.length()) {
    size_t next_slash = dir_name.find_first_of('/');
    std::string next_dir =
        utility::SanitizeROOTObjectName(dir_name.substr(0, next_slash));

    if (next_slash != std::string::npos) {
      dir_name = dir_name.substr(next_slash + 1);
    } else {
      dir_name = "";
    }

    TDirectory *nd = d->GetDirectory(next_dir.c_str());
    if (!nd) {
      nd = d->mkdir(next_dir.c_str());
    }
    nd->cd();
    d = nd;
  }

  d->WriteTObject(object, utility::SanitizeROOTObjectName(object_name).c_str(),
                  "overwrite");

  if (ogdir) {
    ogdir->cd();
  }
}

template <typename T>
inline void WriteToOutputFile(std::unique_ptr<T> &object,
                              std::string const &object_name,
                              std::string dir_name = "",
                              std::string const &file_name = "default") {
  return WriteToOutputFile(object.get(), object_name, dir_name, file_name);
}

void CloseOpenTFiles();
} // namespace persistency
} // namespace nuis
