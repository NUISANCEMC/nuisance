#include "TFile.h"
#include "TH1D.h"

#include <fstream>
#include <iostream>
#include <sstream>

void makefluxroot() {

  std::ifstream input("flux_fhc.txt");

  std::string line;

  double low, up, totval, nueval, nuebarval, numuval, numubarval;
  std::vector<double> bins;
  std::vector<double> vals;
  while (std::getline(input, line)) {
    if (line.front() == '#') {
      continue;
    }
    std::istringstream ss(line);
    ss >> low;
    ss >> up;
    ss >> totval;
    ss >> nueval;
    ss >> nuebarval;
    ss >> numuval;
    ss >> numubarval;

    if (!bins.size()) {
      bins.push_back(low);
    }
    bins.push_back(up);
    vals.push_back(numuval);
  }

  TFile f("sciboone_fhc.root", "RECREATE");

  TH1D *flux = new TH1D("flux_numu", ";E_{#nu} (GeV); #Phi_{#nu};",
                        bins.size() - 1, bins.data());
  for (size_t i = 0; i < vals.size(); ++i) {
    flux->SetBinContent(i + 1, vals[i]);
  }

  f.Write();
  f.Close();
}
