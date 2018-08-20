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

#ifndef CORE_INPUTMANAGER_HXX_SEEN
#define CORE_INPUTMANAGER_HXX_SEEN

#include "input/IInputHandler.hxx"

#include "plugins/traits.hxx"

#include "exception/exception.hxx"

#include <string>
#include <vector>

namespace fhicl {
class ParameterSet;
}

namespace nuis {
namespace input {
class InputManager {
  struct NamedInputHandler {
    NamedInputHandler(std::string const &,
                      plugins::plugin_traits<IInputHandler>::unique_ptr_t &&);
    std::string name;
    plugins::plugin_traits<IInputHandler>::unique_ptr_t handler;
  };
  std::vector<NamedInputHandler> Inputs;

  InputManager();

  static InputManager *_global_inst;
public:

  static InputManager &Get();

  NEW_NUIS_EXCEPT(unknown_input);
  typedef size_t Input_id_t;

  Input_id_t EnsureInputLoaded(fhicl::ParameterSet const &);
  Input_id_t GetInputId(std::string const &) const;
  IInputHandler const &GetInputHandler(Input_id_t) const;
};
} // namespace input
} // namespace nuis

#endif
