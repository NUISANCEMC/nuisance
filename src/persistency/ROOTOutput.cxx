#include "persistency/ROOTOutput.hxx"

#include "utility/ROOTUtility.hxx"

#include "config/GlobalConfiguration.hxx"

#include "fhiclcpp/ParameterSet.h"

#include "TFile.h"

namespace nuis {
namespace persistency {

struct NamedTFile {
  std::string name;
  std::unique_ptr<TFile> file;

  NamedTFile() : name(""), file(nullptr) {}
  NamedTFile(NamedTFile &&other)
      : name(std::move(other.name)), file(std::move(other.file)) {}
  ~NamedTFile() {
    if (file) {
      file->Write();
      file->Close();
    }
  }
};

static std::vector<NamedTFile> Files;

void NewStream(std::string const &name,
               std::string file_name,
               std::string opts) {
  NamedTFile ntf;
  ntf.name = name;
  ntf.file = std::make_unique<TFile>(file_name.c_str(), opts.c_str());
  if (!ntf.file || !ntf.file->IsOpen()) {
    if ((name == "default") && (opts == "CREATE")) {
      std::cout
          << "[WARN]: It appears the default file cannot be opened because it "
             "exists already, to stop you wasting processing time, the default "
             "output stream will be written to nuis.default.tmp.root in the "
             "current directory in RECREATE mode."
          << std::endl;
      file_name = "nuis.default.tmp.root";
      opts = "RECREATE";
      ntf.file = std::make_unique<TFile>(file_name.c_str(), opts.c_str());
      if (!ntf.file || !ntf.file->IsOpen()) {
        throw utility::failed_to_open_TFile()
            << "[ERROR]: Failed to open output file: " << std::quoted(file_name)
            << " in write mode with opts = " << std::quoted(opts);
      }
    } else {
      throw utility::failed_to_open_TFile()
          << "[ERROR]: Failed to open output file: " << std::quoted(file_name)
          << " in write mode with opts = " << std::quoted(opts);
    }
  }
  Files.push_back(std::move(ntf));
}

std::unique_ptr<TFile> &GetOutputFile(std::string const &name) {
  for (NamedTFile &file : Files) {
    if (file.name == name) {
      return file.file;
    }
  }

  fhicl::ParameterSet const &persistency =
      config::GetDocument().get<fhicl::ParameterSet>("persistency");
  std::string file_name = persistency.get<std::string>(name + ".output_file");
  std::string opts =
      persistency.get<std::string>(name + ".open_mode", "CREATE");

  NewStream(name, file_name, opts);

  return GetOutputFile(name);
}

void CloseOpenTFiles() {
  for (NamedTFile &f : Files) {
    std::cout << "[INFO]: Closing open TFile: " << f.name << " "
              << f.file->GetName() << std::endl;
  }
  Files.clear();
}
} // namespace persistency
} // namespace nuis
