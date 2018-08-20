#include "utility/FileSystemUtility.hxx"

#include "exception/exception.hxx"

#include "config/GlobalConfiguration.hxx"

#include "fhiclcpp/ParameterSet.h"

#include <dirent.h>

#include <regex>

namespace nuis {
namespace utility {

NEW_NUIS_EXCEPT(unexpected_empty_string);

std::string EnsureTrailingSlash(std::string str) {
  if (!str.size()) {
    throw unexpected_empty_string();
  }
  if (str.back() != '/') {
    return str + '/';
  }
  return str;
}

std::vector<std::string> GetMatchingFiles(std::string directory, std::string const &pattern) {
  directory = EnsureTrailingSlash(directory);
  std::regex rpattern(pattern);

  std::vector<std::string> matches;

  DIR *dir;
  struct dirent *ent;
  dir = opendir(directory.c_str());
  if (dir != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (std::regex_match(ent->d_name, rpattern)) {
        matches.push_back(ent->d_name);
      }
    }
  }
  return matches;
}

std::string GetDataDir() {
  return EnsureTrailingSlash(
      config::GetDocument().get<std::string>("data_dir"));
}

} // namespace utility
} // namespace nuis
