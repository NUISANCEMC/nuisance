#include <iostream>
#include <string>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

std::vector<std::string> inputdescriptors;
std::string outputfilename;
std::string treename;
std::string branchname;
bool isdouble = true;

void SayUsage(char const *argv[]) {
  std::cout
      << "[USAGE]: " << argv[0] << "\n"
      << "\t-i <file1.root [file2.root [file3.root [...]]]> : Input file list\n"
      << "\t-o <outfilename.root>                           : Input file list\n"
      << "\t-t <treename>                                   : flat tree name\n"
      << "\t-b <branchname>                                 : xsec weighting "
         "branch name\n"
      << "\t-f                                              : xsec weighting "
         "branch is float\n"
      << std::endl;
}

void handleOpts(int argc, char const *argv[]) {
  int opt = 1;
  while (opt < argc) {
    if (std::string(argv[opt]) == "-?" || std::string(argv[opt]) == "--help") {
      SayUsage(argv);
      exit(0);
    } else if (std::string(argv[opt]) == "-i") {
      // Keep adding until run out of options or the next string starts with a -
      while (((opt + 1) != argc) && (argv[opt + 1][0] != '-')) {
        inputdescriptors.push_back(argv[++opt]);
      }
    } else if (std::string(argv[opt]) == "-o") {
      outputfilename = argv[++opt];
    } else if (std::string(argv[opt]) == "-t") {
      treename = argv[++opt];
    } else if (std::string(argv[opt]) == "-b") {
      branchname = argv[++opt];
    } else if (std::string(argv[opt]) == "-f") {
      isdouble = false;
    } else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }
}

int main(int argc, char const *argv[]) {
  handleOpts(argc, argv);

  TChain ch(treename.c_str());
  for (size_t i = 0; i < inputdescriptors.size(); ++i) {
    ch.Add(inputdescriptors[i].c_str());
    std::cout << "Added: " << inputdescriptors[i] << " to input file list."
              << std::endl;
  }

  TFile *outfile = new TFile(outputfilename.c_str(), "RECREATE");
  std::cout << "Writing to " << outputfilename << std::endl;
  TTree *outtree = ch.CloneTree(0, "");
  outtree->SetDirectory(outfile);

  size_t nents = ch.GetEntries();
  double ntrees = ch.GetNtrees();

  if (isdouble) {
    double fScaleFactor;
    ch.SetBranchAddress(branchname.c_str(), &fScaleFactor);
    std::cout << "recalculating " << branchname << "(double) for " << ntrees
              << " input trees." << std::endl;

    for (size_t ent_it = 0; ent_it < nents; ++ent_it) {
      if (ent_it && !(ent_it % (ent_it/100))) {
        std::cout << "Processed " << ent_it << "/" << nents << std::endl;
      }

      ch.GetEntry(ent_it);

      fScaleFactor /= ntrees;

      outtree->Fill();
    }
  } else {
    float fScaleFactor;
    ch.SetBranchAddress(branchname.c_str(), &fScaleFactor);
    std::cout << "recalculating " << branchname << "(float) for " << ntrees
              << " input trees." << std::endl;

    for (size_t ent_it = 0; ent_it < nents; ++ent_it) {
      if (ent_it && !(ent_it % 10000)) {
        std::cout << "Processed " << ent_it << "/" << nents << std::endl;
      }

      ch.GetEntry(ent_it);

      fScaleFactor /= ntrees;

      outtree->Fill();
    }
  }

  outfile->Write();
  outfile->Close();
}