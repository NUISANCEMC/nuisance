#ifndef PERSITENCY_ROOTOUTPUT_HXX_SEEN
#define PERSITENCY_ROOTOUTPUT_HXX_SEEN

#include "TFile.h"

#include <memory>
#include <string>

namespace nuis {
namespace persistency {
/// Will get/open a TFile that is described in the global config
///
/// The named streams will be used to configure the file name and open mode from
/// the global config element persistency.<name>: {file: output.root opts:
/// CREATE}
std::shared_ptr<TFile> GetOutputFile(std::string const &name = "default");

template <typename T>
void WriteToOutputFile(T *object, std::string const &object_name,
           std::string const &dir_name = "",
           std::string const &file_name = "default") {
  std::shared_ptr<TFile> f = GetOutputFile(file_name);

  f->cd();
  if (dir_name.length()) {
    if (!f->cd(dir_name.c_str())) {
      f->mkdir(dir_name.c_str())->cd();
    }
  }

  object->Write(object_name.c_str(), TObject::kOverwrite);
}
} // namespace persistency
} // namespace nuis

#endif
