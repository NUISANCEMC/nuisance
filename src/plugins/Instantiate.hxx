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

#ifndef PLUGINS_PLUGINMANAGER_HXX_SEEN
#define PLUGINS_PLUGINMANAGER_HXX_SEEN

#include "plugins/traits.hxx"

#include "config/GlobalConfiguration.hxx"

#include "utility/FileSystemUtility.hxx"

#include "exception/exception.hxx"

#include "fhiclcpp/ParameterSet.h"
#include "string_parsers/to_string.hxx"

// linux
#include <dlfcn.h>

#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// #define DEBUG_INSTANTIATE

namespace nuis {
namespace plugins {

NEW_NUIS_EXCEPT(failed_to_find_instantiator);
NEW_NUIS_EXCEPT(malformed_plugin_interface);
NEW_NUIS_EXCEPT(failed_to_load_so);

typedef void *(*inst_fcn)();
typedef void (*dltr_fcn)(void *);

template <typename T> struct PluginInstantiator {

  std::string FQ_so_path;
  std::string Base_classname;
  std::string Classname;
  void *dllib;
  inst_fcn Instantiator;
  dltr_fcn Deleter;

  PluginInstantiator()
      : FQ_so_path(""), Base_classname(""), Classname(""), dllib(nullptr),
        Instantiator(nullptr), Deleter(nullptr) {}
  PluginInstantiator(PluginInstantiator const &) = delete;
  PluginInstantiator(PluginInstantiator &&other) {
    FQ_so_path = std::move(other.FQ_so_path);
    Base_classname = std::move(other.Base_classname);
    Classname = std::move(other.Classname);
    dllib = other.dllib;
    Instantiator = other.Instantiator;
    Deleter = other.Deleter;

    other.FQ_so_path = "";
    other.Base_classname = "";
    other.Classname = "";
    other.dllib = nullptr;
    other.Instantiator = nullptr;
    other.Deleter = nullptr;
  }

  typename plugin_traits<T>::unique_ptr_t Instantiate() {
    T *inst = reinterpret_cast<T *>((*Instantiator)());
    dltr_fcn dltr = Deleter;
    std::string cln = Classname;
    std::function<void(T *)> deleter = [=](T *inst) {
#ifdef DEBUG_INSTANTIATE
      std::cout << "[INFO]: Deleting instance of " << cln << " with "
                << (void *)dltr << std::endl;
#endif
      (*dltr)(inst);
    };
    return typename plugin_traits<T>::unique_ptr_t(inst, deleter);
  }
};

struct NamedSO {
  std::string name;
  void *dllib;

  NamedSO() : name(""), dllib(nullptr) {}
  NamedSO(NamedSO const &) = delete;
  NamedSO(NamedSO &&other) : name(std::move(other.name)), dllib(other.dllib) {
    other.dllib = nullptr;
  }

  ~NamedSO() {
    if (dllib) {
#ifdef DEBUG_INSTANTIATE
      std::cout << "[INFO]: dlclose on shared object: " << std::quoted(name)
                << std::endl;
#endif
      dlclose(dllib);
    }
  }
};

NamedSO &GetSharedObject(std::string const &FQPath) {
  static std::vector<NamedSO> LoadedSharedObjects;

  for (NamedSO &so : LoadedSharedObjects) {
    if (so.name == FQPath) {
      return so;
    }
  }

  NamedSO so;
  so.name = FQPath;
  so.dllib = dlopen(FQPath.c_str(), RTLD_NOW | RTLD_GLOBAL);

  char const *dlerr_cstr = dlerror();
  std::string dlerr;
  if (dlerr_cstr) {
    dlerr = dlerr_cstr;
  }

  if (dlerr.length()) {
    throw failed_to_load_so()
        << "[INFO]: Failed to load shared object: " << FQPath
        << " with dlerror: " << dlerr;
  } else {
#ifdef DEBUG_INSTANTIATE
    std::cout << "[INFO]: Loaded shared object " << FQPath << std::endl;
#endif
  }

  LoadedSharedObjects.push_back(std::move(so));
  return LoadedSharedObjects.back();
}

template <typename T>
typename plugin_traits<T>::unique_ptr_t
Instantiate(std::string const &classname) {

  static std::vector<PluginInstantiator<T>> LoadedPlugins;

  fhicl::ParameterSet const &plugins =
      config::GetDocument().get<fhicl::ParameterSet>("plugins");
  fhicl::ParameterSet const &search_paths =
      plugins.get<fhicl::ParameterSet>("search_paths");

  std::vector<std::string> plugin_search_dirs;
  // Look for plugin search paths in sequence elements of the
  // plugins.search_paths table
  for (std::string const &key : search_paths.get_names()) {
    if (!search_paths.is_key_to_sequence(key)) {
      continue;
    }
    for (std::string const &path :
         search_paths.get<std::vector<std::string>>(key)) {
      plugin_search_dirs.push_back(path);
    }
  }

  for (std::string path : plugin_search_dirs) {
    path = utility::EnsureTrailingSlash(path);
    for (std::string const &so_name :
         utility::GetMatchingFiles(path, ".*\\.so")) {

      for (PluginInstantiator<T> &plugin : LoadedPlugins) {
        if (plugin.FQ_so_path == (path + so_name) &&
            (plugin.Base_classname == plugin_traits<T>::interface_name()) &&
            (plugin.Classname == classname)) {
#ifdef DEBUG_INSTANTIATE
          std::cout << "[INFO]: Using already loaded PluginInstantiator"
                    << std::endl;
#endif
          return plugin.Instantiate();
        }
      }

      PluginInstantiator<T> plugin;
      plugin.FQ_so_path = path + so_name;
      plugin.Base_classname = plugin_traits<T>::interface_name();
      plugin.Classname = classname;
      plugin.dllib = GetSharedObject(plugin.FQ_so_path).dllib;

      char const *dlerr_cstr = nullptr;
      std::string dlerr("");

      plugin.Instantiator = reinterpret_cast<inst_fcn>(dlsym(
          plugin.dllib,
          plugin_traits<T>::instantiator_function_name(classname).c_str()));

      dlerr_cstr = dlerror();
      if (dlerr_cstr) {
        dlerr = dlerr_cstr;
      }

      if (dlerr_cstr) {
#ifdef DEBUG_INSTANTIATE
        std::cout << "[INFO]: Failed to load appropriate instantiator method: "
                  << plugin_traits<T>::instantiator_function_name(classname)
                  << " from shared object " << plugin.FQ_so_path;
#endif
        continue;
      } else {
#ifdef DEBUG_INSTANTIATE
        std::cout << "[INFO]: Loaded instantiator method: "
                  << plugin_traits<T>::instantiator_function_name(classname)
                  << " from shared object " << plugin.FQ_so_path << std::endl;
#endif
      }

      plugin.Deleter = reinterpret_cast<dltr_fcn>(
          dlsym(plugin.dllib,
                plugin_traits<T>::deleter_function_name(classname).c_str()));

      dlerr_cstr = dlerror();
      if (dlerr_cstr) {
        dlerr = dlerr_cstr;
      }

      if (dlerr_cstr) {
        throw malformed_plugin_interface()
            << "[ERROR]: Failed to load appropriate deleter method: "
            << plugin_traits<T>::deleter_function_name(classname)
            << " from shared object " << plugin.FQ_so_path
            << " with error:  " << std::quoted(dlerr);
      } else {
#ifdef DEBUG_INSTANTIATE
        std::cout << "[INFO]: Loaded deleter method: "
                  << plugin_traits<T>::deleter_function_name(classname)
                  << " from shared object " << plugin.FQ_so_path << std::endl;
#endif
      }

#ifdef DEBUG_INSTANTIATE
      std::cout << "[INFO]: Checking if shared object "
                << std::quoted(plugin.FQ_so_path)
                << " knows how to instantiate class " << std::quoted(classname)
                << " via interface "
                << std::quoted(plugin_traits<T>::interface_name()) << std::endl;
#endif

      LoadedPlugins.push_back(std::move(plugin));
      return LoadedPlugins.back().Instantiate();
    }
  }
  throw failed_to_find_instantiator()
      << "[ERROR]: Failed to find instantiator for classname: "
      << std::quoted(classname) << " using interface "
      << std::quoted(plugin_traits<T>::interface_name())
      << " from configured search paths: "
      << fhicl::string_parsers::T2Str<std::vector<std::string>>(
             plugin_search_dirs);
}
} // namespace plugins
} // namespace nuis
#endif
