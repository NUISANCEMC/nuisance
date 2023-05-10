#include "FitEvent.h"
#include "InputFactory.h"

#include "nuiscling.h"

#include <iostream>
#include <string>
#include <vector>

std::string file_to_read;
std::string filename;

std::string filter_symname;
std::vector<std::string> projection_symnames;
std::vector<std::string> weights_symnames;

void SayUsage(char const *argv[]) {
  std::cout
      << "[USAGE]: " << argv[0] << "\n"
      << "\t-f <sourcefile.cxx> : File to interpret\n"
      << "\t--Filter <symname>  : Symbol to use for filtering\n"
      << "\t--Project <symname> : Symbol to use for projection, can be passed "
         "more than once\n"
      << "\t--Weight <symname>  : Symbol to use for weights, can be passed "
         "more than once\n"
      << "\t-F <file.root>      : input event file\n"
      << std::endl;
}

void handleOpts(int argc, char const *argv[]) {
  int opt = 1;
  while (opt < argc) {
    if (std::string(argv[opt]) == "-?" || std::string(argv[opt]) == "--help") {
      SayUsage(argv);
      exit(0);
    } else if ((opt + 1) < argc) {
      if (std::string(argv[opt]) == "-f") {
        file_to_read = argv[++opt];
      } else if (std::string(argv[opt]) == "--Filter") {
        filter_symname = argv[++opt];
      } else if (std::string(argv[opt]) == "--Project") {
        projection_symnames.push_back(argv[++opt]);
      } else if (std::string(argv[opt]) == "--Weight") {
        weights_symnames.push_back(argv[++opt]);
      } else if (std::string(argv[opt]) == "-F") {
        filename = argv[++opt];
      }
    } else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }
}

typedef bool (*filter_ptrtype)(FitEvent *);

int main(int argc, char const *argv[]) {

  handleOpts(argc, argv);

  std::cout << "[INFO]: Interpreting " << file_to_read << std::endl;
  if (!nuiscling::Get().LoadFile(file_to_read.c_str())) {
    std::cout << "[ERROR]: nuiscling failed interpreting: " << argv[1]
              << std::endl;
    return 1;
  }

  auto filter_func = nuiscling::Get().GetFilterFunction(filter_symname);
  std::vector<nuiscling_ftypes::project> proj_funcs;
  std::vector<std::string> proj_funcnames;
  for (auto &proj_sym_name : projection_symnames) {
    auto proj_func = nuiscling::Get().GetProjectionFunction(proj_sym_name);
    if (proj_func) {
      proj_funcs.push_back(proj_func);
      proj_funcnames.push_back(proj_sym_name);
    }
  }

  TFile *fin = TFile::Open(filename.c_str(), "READ");

  InputHandlerBase *inp = InputUtils::CreateInputHandler(
      "myinphandler", InputUtils::GuessInputTypeFromFile(fin), filename);

  FitEvent *ev = inp->FirstNuisanceEvent();
  size_t e_it = 0;
  while (ev) {
    bool filtered = filter_func(ev);
    if (filtered) {
      std::cout << "Event: " << e_it << " passes filter: " << std::endl;
      for (size_t i = 0; i < proj_funcs.size(); ++i) {
        std::cout << "\t" << proj_funcnames[i] << ": " << proj_funcs[i](ev)
                  << std::endl;
      }
    }
    ev = inp->NextNuisanceEvent();
    e_it++;
  }
}