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

#include "Smearcepterton.h"

#include "EfficiencyApplicator.h"
#include "GaussianSmearer.h"
#include "MetaSimpleSmearcepter.h"
#include "ThresholdAccepter.h"
#include "TrackedMomentumMatrixSmearer.h"
#include "VisECoalescer.h"

#include <vector>

#include "TRegexp.h"

#include <dirent.h>

// linux
#include <dlfcn.h>

DynamicSmearceptorFactory::DynamicSmearceptorFactory()
    : NSmearceptors(0), NManifests(0) {
  LoadPlugins();
  NUIS_LOG(FIT, "Loaded " << NSmearceptors << " from " << NManifests
                      << " shared object libraries.");
}
DynamicSmearceptorFactory* DynamicSmearceptorFactory::glblDSF = NULL;
DynamicSmearceptorFactory::PluginManifest::~PluginManifest() {
  for (size_t i_it = 0; i_it < Instances.size(); ++i_it) {
    (*(DSF_DestroySmearceptor))(Instances[i_it]);
  }
}
std::string EnsureTrailingSlash(std::string const& inp) {
  if (!inp.length()) {
    return "/";
  }
  if (inp[inp.length() - 1] == '/') {
    return inp;
  }
  return inp + "/";
}
void DynamicSmearceptorFactory::LoadPlugins() {
  std::vector<std::string> SearchDirectories;

  if (Config::HasPar("dynamic_smearceptor.path")) {
    SearchDirectories = GeneralUtils::ParseToStr(
        Config::GetParS("dynamic_smearceptor.path"), ":");
  }

  char const* envPath = getenv("NUISANCE_DS_PATH");
  if (envPath) {
    std::vector<std::string> envPaths = GeneralUtils::ParseToStr(envPath, ":");
    for (size_t ep_it = 0; ep_it < envPaths.size(); ++ep_it) {
      SearchDirectories.push_back(envPaths[ep_it]);
    }
  }

  if (!SearchDirectories.size()) {
    char const* pwdPath = getenv("PWD");
    if (pwdPath) {
      SearchDirectories.push_back(pwdPath);
    }
  }

  for (size_t sp_it = 0; sp_it < SearchDirectories.size(); ++sp_it) {
    std::string dirpath = EnsureTrailingSlash(SearchDirectories[sp_it]);

    NUIS_LOG(FIT, "Searching for dynamic smearceptor manifests in: " << dirpath);

    Ssiz_t len = 0;
    DIR* dir;
    struct dirent* ent;
    dir = opendir(dirpath.c_str());
    if (dir != NULL) {
      TRegexp matchExp("*.so", true);
      while ((ent = readdir(dir)) != NULL) {
        if (matchExp.Index(TString(ent->d_name), &len) != Ssiz_t(-1)) {
          NUIS_LOG(FIT, "\tFound shared object: "
                        << ent->d_name << " checking for relevant methods...");

          void* dlobj =
              dlopen((dirpath + ent->d_name).c_str(), RTLD_NOW | RTLD_GLOBAL);
          char const* dlerr_cstr = dlerror();
          std::string dlerr;
          if (dlerr_cstr) {
            dlerr = dlerr_cstr;
          }

          if (dlerr.length()) {
            NUIS_ERR(WRN, "\tDL Load Error: " << dlerr);
            continue;
          }

          PluginManifest plgManif;
          plgManif.dllib = dlobj;
          plgManif.soloc = (dirpath + ent->d_name);

          plgManif.DSF_NSmearceptors = reinterpret_cast<DSF_NSmearceptors_ptr>(
              dlsym(dlobj, "DSF_NSmearceptors"));

          dlerr = "";
          dlerr_cstr = dlerror();
          if (dlerr_cstr) {
            dlerr = dlerr_cstr;
          }

          if (dlerr.length()) {
            NUIS_ERR(WRN, "\tFailed to load symbol \"DSF_NSmearceptors\" from "
                           << (dirpath + ent->d_name) << ": " << dlerr);
            dlclose(dlobj);
            continue;
          }

          plgManif.DSF_GetSmearceptorName =
              reinterpret_cast<DSF_GetSmearceptorName_ptr>(
                  dlsym(dlobj, "DSF_GetSmearceptorName"));

          dlerr = "";
          dlerr_cstr = dlerror();
          if (dlerr_cstr) {
            dlerr = dlerr_cstr;
          }

          if (dlerr.length()) {
            NUIS_ERR(WRN,
                  "\tFailed to load symbol \"DSF_GetSmearceptorName\" from "
                      << (dirpath + ent->d_name) << ": " << dlerr);
            dlclose(dlobj);
            continue;
          }

          plgManif.DSF_GetSmearceptor =
              reinterpret_cast<DSF_GetSmearceptor_ptr>(
                  dlsym(dlobj, "DSF_GetSmearceptor"));

          dlerr = "";
          dlerr_cstr = dlerror();
          if (dlerr_cstr) {
            dlerr = dlerr_cstr;
          }

          if (dlerr.length()) {
            NUIS_ERR(WRN, "\tFailed to load symbol \"DSF_GetSmearceptor\" from "
                           << (dirpath + ent->d_name) << ": " << dlerr);
            dlclose(dlobj);
            continue;
          }

          plgManif.DSF_DestroySmearceptor =
              reinterpret_cast<DSF_DestroySmearceptor_ptr>(
                  dlsym(dlobj, "DSF_DestroySmearceptor"));

          dlerr = "";
          dlerr_cstr = dlerror();
          if (dlerr_cstr) {
            dlerr = dlerr_cstr;
          }

          if (dlerr.length()) {
            NUIS_ERR(WRN, "Failed to load symbol \"DSF_DestroySmearceptor\" from "
                           << (dirpath + ent->d_name) << ": " << dlerr);
            dlclose(dlobj);
            continue;
          }

          plgManif.NSmearceptors = (*(plgManif.DSF_NSmearceptors))();
          NUIS_LOG(FIT, "\tSuccessfully loaded dynamic smearceptor manifest: "
                        << plgManif.soloc << ". Contains "
                        << plgManif.NSmearceptors << " smearceptors.");

          for (size_t smp_it = 0; smp_it < plgManif.NSmearceptors; ++smp_it) {
            char const* smp_name = (*(plgManif.DSF_GetSmearceptorName))(smp_it);
            if (!smp_name) {
              NUIS_ABORT("Could not load smearceptor "
                    << smp_it << " / " << plgManif.NSmearceptors << " from "
                    << plgManif.soloc);
            }

            if (Smearceptors.count(smp_name)) {
              NUIS_ERR(WRN, "Already loaded a smearceptor named: \""
                             << smp_name << "\". cannot load duplciates. This "
                                            "smearceptor will be skipped.");
              continue;
            }

            plgManif.SmearceptorsProvided.push_back(smp_name);
            Smearceptors[smp_name] = std::make_pair(plgManif.soloc, smp_it);
            NUIS_LOG(FIT, "\t\t" << smp_name);
          }

          if (plgManif.SmearceptorsProvided.size()) {
            Manifests[plgManif.soloc] = plgManif;

            NSmearceptors += plgManif.SmearceptorsProvided.size();
            NManifests++;
          } else {
            dlclose(dlobj);
          }
        }
      }
      closedir(dir);
    } else {
      NUIS_ERR(WRN, "Tried to open non-existant directory.");
    }
  }
}
DynamicSmearceptorFactory& DynamicSmearceptorFactory::Get() {
  if (!glblDSF) {
    glblDSF = new DynamicSmearceptorFactory();
  }
  return *glblDSF;
}
void DynamicSmearceptorFactory::Print() {
  std::map<std::string, std::vector<std::string> > ManifestSmearceptors;

  for (std::map<std::string, std::pair<std::string, int> >::iterator smp_it =
           Smearceptors.begin();
       smp_it != Smearceptors.end(); ++smp_it) {
    if (!ManifestSmearceptors.count(smp_it->second.first)) {
      ManifestSmearceptors[smp_it->second.first] = std::vector<std::string>();
    }
    ManifestSmearceptors[smp_it->second.first].push_back(smp_it->first);
  }

  NUIS_LOG(FIT, "Dynamic smearceptor manifest: ");
  for (std::map<std::string, std::vector<std::string> >::iterator m_it =
           ManifestSmearceptors.begin();
       m_it != ManifestSmearceptors.end(); ++m_it) {
    NUIS_LOG(FIT, "\tLibrary " << m_it->first << " contains: ");
    for (size_t s_it = 0; s_it < m_it->second.size(); ++s_it) {
      NUIS_LOG(FIT, "\t\t" << m_it->second[s_it]);
    }
  }
}
bool DynamicSmearceptorFactory::HasSmearceptor(std::string const& name) {
  return Smearceptors.count(name);
}
bool DynamicSmearceptorFactory::HasSmearceptor(nuiskey& smearceptorkey) {
  return HasSmearceptor(smearceptorkey.GetElementName());
}
ISmearcepter* DynamicSmearceptorFactory::CreateSmearceptor(
    nuiskey& smearceptorkey) {
  if (!HasSmearceptor(smearceptorkey)) {
    NUIS_ERR(WRN, "Asked to load unknown smearceptor: \""
                   << smearceptorkey.GetElementName() << "\".");
    return NULL;
  }

  std::pair<std::string, int> smearceptor =
      Smearceptors[smearceptorkey.GetElementName()];
  NUIS_LOG(SAM, "\tLoading smearceptor " << smearceptor.second << " from "
                                     << smearceptor.first);

  ISmearcepter* smear = (*(Manifests[smearceptor.first].DSF_GetSmearceptor))(
      smearceptor.second, &smearceptorkey);
  return smear;
}

DynamicSmearceptorFactory::~DynamicSmearceptorFactory() { Manifests.clear(); }

Smearcepterton* Smearcepterton::_inst = NULL;
Smearcepterton& Smearcepterton::Get() {
  if (!_inst) {
    _inst = new Smearcepterton();
  }
  return *_inst;
}

Smearcepterton::Smearcepterton() { InitialiserSmearcepters(); }

void Smearcepterton::InitialiserSmearcepters() {
  // hard coded list of tag name -> smearcepter factories, add here to add your
  // own.
  std::map<std::string, SmearceptionFactory_fcn> factories;

  factories["ThresholdAccepter"] = &BuildSmearcepter<ThresholdAccepter>;
  factories["EfficiencyApplicator"] = &BuildSmearcepter<EfficiencyApplicator>;
  factories["GaussianSmearer"] = &BuildSmearcepter<GaussianSmearer>;
  factories["TrackedMomentumMatrixSmearer"] =
      &BuildSmearcepter<TrackedMomentumMatrixSmearer>;
  factories["VisECoalescer"] = &BuildSmearcepter<VisECoalescer>;
  factories["MetaSimpleSmearcepter"] = &BuildSmearcepter<MetaSimpleSmearcepter>;

  Config::Get().PrintXML(NULL);

  std::vector<nuiskey> smearcepterBlocks = Config::QueryKeys("smearcepters");

  // std::cout << "[INFO]: " << nodelist.size() << " smearcepter nodes." << std::endl;
  // for(size_t i = 0; i < nodelist.size(); ++i){
  //   Config::Get().PrintXML(nodelist[i]);
  // }

  for (size_t smearB_it = 0; smearB_it < smearcepterBlocks.size();
       ++smearB_it) {
    std::vector<nuiskey> smearcepters =
        smearcepterBlocks[smearB_it].GetListOfChildNodes();
    for (size_t smear_it = 0; smear_it < smearcepters.size(); ++smear_it) {
      std::string const& smearType = smearcepters[smear_it].GetElementName();

      ISmearcepter* smearer = NULL;
#ifdef __USE_DYNSAMPLES__
      if (DynamicSmearceptorFactory::Get().HasSmearceptor(smearType)) {
        smearer = DynamicSmearceptorFactory::Get().CreateSmearceptor(
            smearcepters[smear_it]);
      } else
#endif
      {
        if (!factories.count(smearType)) {
          NUIS_ERR(WRN, "No known smearer accepts elements named: \"" << smearType
                                                                   << "\"");
          continue;
        }
        smearer = factories[smearType](smearcepters[smear_it]);
      }

      if (!smearer) {
        NUIS_ABORT("Failed to load smearceptor.");
      }
      if (!smearer->GetName().length()) {
        NUIS_ABORT("Smearcepter type " << smearer->GetElementName()
                                  << " had no instance name.");
      }

      Smearcepters[smearer->GetName()] = smearer;

      NUIS_LOG(FIT, "Configured smearer named: " << smearer->GetName()
                                             << " of type: "
                                             << smearer->GetElementName());
    }
  }
}
