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

#include "GeneralUtils.h"

#include "TCollection.h"
#include "TError.h"
#include "TFile.h"
#include "TStreamerInfo.h"
#include "TString.h"
#include "TTree.h"

#include <algorithm>
#include <fstream>
#include <iostream>

int main(int argc, char const* argv[]) {
  // shhh ROOT it okay.
  gErrorIgnoreLevel = kError;

  if (argc != 4) {
    std::cout << "[ERROR](DumpROOTClassesFromVector): Recieved " << (argc - 1)
              << " CLI opts." << std::endl;
    for (int i = 1; i < argc; ++i) {
      std::cout << "[" << i << "] \"" << argv[i] << "\"" << std::endl;
    }
    std::cout << "Usage: " << argv[0]
              << " <Input ROOT filename> "
                 "<Project source output directory> <ClassName Prefix To "
                 "Include in LinkDef>"
              << std::endl;
    return 1;
  }
  char const* ROOTFileName = argv[1];
  char const* OutputDirectoryName = argv[2];
  char const* CSLClasses = argv[3];
  TFile* fl = TFile::Open(ROOTFileName);
  if (!fl || !fl->IsOpen()) {
    std::cerr
        << "[ERROR](DumpROOTClassesFromVector): Failed to open root file: "
        << fl << std::endl;
    return 1;
  }
  std::cout << "[INFO]: Writing project files to " << OutputDirectoryName
            << std::endl;
  fl->MakeProject(OutputDirectoryName);

  TIter next(fl->GetStreamerInfoList());
  TStreamerInfo* inf = 0;
  std::vector<std::string> ClassesToFind =
      GeneralUtils::ParseToStr(CSLClasses, ",");

  if (!ClassesToFind.size() || !ClassesToFind[0].length()) {
    std::cerr
        << "[ERROR](DumpROOTClassesFromVector): No classes to find passed in."
        << std::endl;
    return 2;
  }

  std::vector<std::string> ClassesToLink;
  while ((inf = dynamic_cast<TStreamerInfo*>(next()))) {
    std::cout << "[INFO]: Have StreamerInfo for class: " << inf->GetName()
              << std::endl;
    for (size_t ClTF_it = 0; ClTF_it < ClassesToFind.size(); ++ClTF_it) {
      if (std::string(inf->GetName()) == ClassesToFind[ClTF_it]) {
        ClassesToLink.push_back(inf->GetName());
        std::cout << "[INFO]: Including streamer info for: " << inf->GetName()
                  << " in generated LinkDef" << std::endl;
      }
    }
  }

  if (ClassesToFind.size() > ClassesToLink.size()) {
    std::cerr << "[ERROR](DumpROOTClassesFromVector): Only found "
              << ClassesToLink.size()
              << " classes to include in the ROOT library, expected at least "
              << ClassesToFind.size() << std::endl;
    return 1;
  }

  std::string OutputDir(OutputDirectoryName);

  size_t pos = OutputDir.find_last_of('/');
  pos = (pos == std::string::npos) ? 0 : pos;

  std::string LinkDef_loc =
      OutputDir + "/" + OutputDir.substr(pos) + "LinkDef.h";
  std::cout << "[INFO]: Writing LinkDef to: " << LinkDef_loc << std::endl;
  std::ofstream LinkDefos(LinkDef_loc.c_str());

  if (!LinkDefos.is_open()) {
    std::cerr << "[ERROR] Couldn't open " << LinkDef_loc << " for writing."
              << std::endl;
  }

  LinkDefos << "#ifdef __CINT__" << std::endl;
  for (size_t ClTL_it = 0; ClTL_it < ClassesToLink.size(); ++ClTL_it) {
    LinkDefos << "#pragma link C++ class  " << ClassesToLink[ClTL_it] << "+;"
              << std::endl;
  }
  LinkDefos << "#endif" << std::endl;
  LinkDefos.close();

  std::string Instances_loc =
      OutputDir + "/" + OutputDir.substr(pos) + "ProjectInstances.h";
  std::ofstream Instancesos(Instances_loc.c_str());
  if (!Instancesos.is_open()) {
    std::cerr << "[ERROR] Couldn't open " << Instances_loc << " for writing."
              << std::endl;
  }
  Instancesos.close();
}
