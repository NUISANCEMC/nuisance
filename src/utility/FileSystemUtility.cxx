#include "utility/FileSystemUtility.hxx"
#include "utility/StringUtility.hxx"

#include "exception/exception.hxx"

#include "config/GlobalConfiguration.hxx"

#include "fhiclcpp/ParameterSet.h"

#include <dirent.h>

namespace nuis {
namespace utility {

std::vector<std::string> GetMatchingFiles(std::string directory,
                                          std::string pattern) {

  std::cout << "[INFO]: Looking for files matching: \"" << pattern
            << "\" in directory: " << directory << std::endl;
  directory = EnsureTrailingSlash(directory);
  pattern = DeGlobPattern(pattern);

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
