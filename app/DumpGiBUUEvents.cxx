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
