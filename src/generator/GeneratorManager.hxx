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

#include "exception/exception.hxx"

#include <string>
#include <vector>

namespace nuis {
/// Singleton class used to register generator compatibilty by string.
///
/// Used for
class GeneratorManager {

  GeneratorManager();

  static GeneratorManager *_global_inst;

  std::vector<std::string> Generators;

public:
  static GeneratorManager &Get();

  NEW_NUIS_EXCEPT(unknown_generator);
  using Generator_id_t = size_t;

  static Generator_id_t const kAll = std::numeric_limits<Generator_id_t>::max();

  Generator_id_t EnsureGeneratorRegistered(std::string const &);
  Generator_id_t GetGeneratorId(std::string const &) const;
  /// Checks whether two generator ids are compatible
  ///
  /// Generator id's are compatible if either one is equal to kAll (i.e. that an
  /// event plugin doesn't need any generator-specific information) or they are
  /// equal.
  static bool AreCompatible(Generator_id_t, Generator_id_t);
};
} // namespace nuis
