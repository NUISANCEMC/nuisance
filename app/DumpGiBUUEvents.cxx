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

#include <iostream>

#include "StdHepEvt.h"

int main(int argv, char const *argc[]) {
  if (argv != 2) {
    std::cerr << "[ERROR]: expected a single input GiBUU rootracker file."
              << std::endl;
    return 1;
  }
  std::string inpf(argc[1]);

  if (!inpf.length()) {
    std::cerr << "[ERROR]: expected an input GiBUU rootracker file."
              << std::endl;
    return 2;
  }

  TChain tn("giRooTracker");
  tn.AddFile(inpf.c_str());

  GiBUUStdHepReader giRead;
  bool ok = giRead.SetBranchAddresses(&tn);

  if (!ok) {
    std::cerr
        << "[ERROR]: Could not correctly set branch address for input file."
        << std::endl;
    return 4;
  }

  for (Long64_t ievt = 0; ievt < tn.GetEntries(); ++ievt) {
    tn.GetEntry(ievt);
    std::cout << WriteGiBUUEvent(giRead) << std::endl;
  }
}
