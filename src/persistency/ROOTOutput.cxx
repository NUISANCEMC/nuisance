#include "persistency/ROOTOutput.hxx"

#include "utility/ROOTUtility.hxx"

#include "config/GlobalConfiguration.hxx"

#include "fhiclcpp/ParameterSet.h"

#include "TFile.h"

namespace nuis {
namespace persistency {

struct NamedTFile {
  std::string name;
  std::shared_ptr<TFile> file;
};
static std::vector<NamedTFile> Files;

std::shared_ptr<TFile> GetOutputFile(std::string const &name) {
  for (NamedTFile &file : Files) {
    if (file.name == name) {
      return file.file;
    }
  }

  fhicl::ParameterSet const &persistency =
      config::GetDocument().get<fhicl::ParameterSet>("persistency");
  std::string file_name = persistency.get<std::string>(name + ".output_file");
  std::string open_opts =
      persistency.get<std::string>(name + ".open_mode", "CREATE");

  NamedTFile ntf;
  ntf.name = name;
  ntf.file = std::make_shared<TFile>(file_name.c_str(), open_opts.c_str());
  if (!ntf.file || !ntf.file->IsOpen()) {
    throw utility::failed_to_open_TFile()
        << "[ERROR]: Failed to open output file: " << std::quoted(file_name)
        << " in write mode with opts = " << std::quoted(open_opts);
  }
  Files.push_back(ntf);
  return Files.back().file;
}
} // namespace persistency
} // namespace nuis
