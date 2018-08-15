#include "config/GlobalConfiguration.hxx"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"

namespace nuis {
namespace config {
struct NamedDoc {
  std::string name;
  fhicl::ParameterSet document;
  std::vector<std::string> files_read;
};

std::vector<NamedDoc> Configurations;

NamedDoc &GetDocument_modifyable(std::string const &name) {
  for (NamedDoc &doc : Configurations) {
    if (doc.name == name) {
      return doc;
    }
  }
  Configurations.push_back(NamedDoc{name, fhicl::ParameterSet()});
  return GetDocument_modifyable(name);
}
fhicl::ParameterSet const &GetDocument(std::string const &name) {
  return GetDocument_modifyable(name).document;
}
bool EnsureConfigurationRead(std::string const &fhicl_file,
                             std::string const &name) {
  NamedDoc &doc = GetDocument_modifyable(name);

  if (std::count(doc.files_read.begin(), doc.files_read.end(), fhicl_file)) {
    return false;
  }

  doc.document.splice(fhicl::make_ParameterSet(fhicl_file));
  return true;
}
} // namespace config
} // namespace nuis
