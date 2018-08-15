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

#include <memory>
#include <string>
#include <vector>

namespace nuis {
namespace core {
class IInputHandler;
}
} // namespace nuis

namespace nuis {
namespace core {
class InputManager {
  struct NamedInputHandler {
    std::string name;
    std::unique_ptr<IInputHandler> handler;
  };
  std::vector<NamedInputHandler> Inputs;

public:
  typedef size_t Input_id_t;

  Input_id_t EnsureInputLoaded(std::string const &file_name);
  Input_id_t GetInputId(std::string const &file_name);
  IInputHandler &GetInputHandler(Input_id_t id);
};
} // namespace core
} // namespace nuis

#endif
