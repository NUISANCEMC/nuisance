// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

/// Expect each .so containing smearceptors to supply 4 c-style methods.
/// int DSF_NSmearceptors();
/// char const * DSF_GetSmearceptorName(int);
/// ISmearcepter* DSF_GetSmearceptor(int, nuiskey *);
/// void DSF_DestroySmearceptor(ISmearcepter *);
class DynamicSmearceptorFactory {
  size_t NSmearceptors;
  size_t NManifests;

  DynamicSmearceptorFactory();

  static DynamicSmearceptorFactory* glblDSF;

  typedef int (*DSF_NSmearceptors_ptr)(void);
  typedef char const* (*DSF_GetSmearceptorName_ptr)(int);
  typedef ISmearcepter* (*DSF_GetSmearceptor_ptr)(int, nuiskey *);
  typedef void (*DSF_DestroySmearceptor_ptr)(ISmearcepter*);

  struct PluginManifest {
    void* dllib;

    DSF_NSmearceptors_ptr DSF_NSmearceptors;
    DSF_GetSmearceptorName_ptr DSF_GetSmearceptorName;
    DSF_GetSmearceptor_ptr DSF_GetSmearceptor;
    DSF_DestroySmearceptor_ptr DSF_DestroySmearceptor;

    std::string soloc;
    std::vector<ISmearcepter*> Instances;
    std::vector<std::string> SmearceptorsProvided;
    size_t NSmearceptors;
    ~PluginManifest();
  };

  std::map<std::string, PluginManifest> Manifests;
  std::map<std::string, std::pair<std::string, int> > Smearceptors;

  void LoadPlugins();

 public:
  static DynamicSmearceptorFactory& Get();

  void Print();

  bool HasSmearceptor(std::string const& name);
  bool HasSmearceptor(nuiskey& smearceptorkey);

  ISmearcepter* CreateSmearceptor(nuiskey& smearceptorkey);

  ~DynamicSmearceptorFactory();
};

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
      NUIS_ERR(FTL, "Known smearcepters:");
      for (std::map<std::string, ISmearcepter *>::iterator sm_it =
               Smearcepters.begin();
           sm_it != Smearcepters.end(); ++sm_it) {
        NUIS_ERR(FTL, "\t" << sm_it->first);
      }
      NUIS_ABORT("No smearcepter named: \"" << name << "\" defined.");
    }
    return *Smearcepters[name];
  }
};

#endif
