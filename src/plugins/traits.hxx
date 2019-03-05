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

#pragma once

#include <functional>
#include <memory>
#include <string>

namespace nuis {
namespace plugins {
template <typename T> struct plugin_traits {};
} // namespace plugins
} // namespace nuis

#define DECLARE_PLUGIN_INTERFACE(INTERFACE_CLASS_NAME)                         \
  namespace nuis {                                                             \
  namespace plugins {                                                          \
  template <> struct plugin_traits<INTERFACE_CLASS_NAME> {                     \
    typedef std::unique_ptr<INTERFACE_CLASS_NAME,                              \
                            std::function<void(INTERFACE_CLASS_NAME *)>>       \
        unique_ptr_t;                                                          \
    static std::string interface_name() { return #INTERFACE_CLASS_NAME; }      \
    static std::string                                                         \
    instantiator_function_name(std::string const &classname) {                 \
      return std::string("nuis_plugins_") +                                    \
             plugin_traits<INTERFACE_CLASS_NAME>::interface_name() + "_" +     \
             classname + "_instantiator";                                      \
    }                                                                          \
    static std::string deleter_function_name(std::string const &classname) {   \
      return std::string("nuis_plugins_") +                                    \
             plugin_traits<INTERFACE_CLASS_NAME>::interface_name() + "_" +     \
             classname + "_deleter";                                           \
    }                                                                          \
  };                                                                           \
  }                                                                            \
  }

#define BUILDMETHODNAME(CN, PN, POST) nuis_plugins_##CN##_##PN##POST

#define DECLARE_PLUGIN(INTERFACE_CLASS_NAME, PLUGIN_CLASS_NAME)                \
  extern "C" {                                                                 \
  void *BUILDMETHODNAME(INTERFACE_CLASS_NAME, PLUGIN_CLASS_NAME,               \
                        _instantiator)() {                                     \
    return new PLUGIN_CLASS_NAME();                                            \
  }                                                                            \
  void BUILDMETHODNAME(INTERFACE_CLASS_NAME, PLUGIN_CLASS_NAME,                \
                       _deleter)(void *instance) {                             \
    delete reinterpret_cast<PLUGIN_CLASS_NAME *>(instance);                    \
  }                                                                            \
  }
