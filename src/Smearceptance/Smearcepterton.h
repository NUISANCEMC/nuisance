// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#ifndef SMEARCEPTERTON_HXX_SEEN
#define SMEARCEPTERTON_HXX_SEEN

#include "FitLogger.h"

#include "ISmearcepter.h"

#include <map>
#include <string>

/// Singleton handling the loading and configuring of known smearcepters.
class Smearcepterton {
  Smearcepterton();

  void InitialiserSmearcepters();

  static Smearcepterton *_inst;

  std::map<std::string, ISmearcepter *> Smearcepters;

 public:
  static Smearcepterton &Get();

  ISmearcepter &GetSmearcepter(std::string const &name) {
    if (!Smearcepters.count(name) || !Smearcepters[name]) {
      ERROR(FTL, "Known smearcepters:");
      for (std::map<std::string, ISmearcepter *>::iterator sm_it =
               Smearcepters.begin();
           sm_it != Smearcepters.end(); ++sm_it) {
        ERROR(FTL, "\t" << sm_it->first);
      }
      THROW("No smearcepter named: \"" << name << "\" defined.");
    }
    return *Smearcepters[name];
  }
};

#endif
