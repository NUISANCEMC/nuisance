#include "config/GlobalConfiguration.hxx"

#ifdef USE_GENIE
#include "generator/input/GENIEInputHandler.hxx"
#endif
#ifdef USE_NEUT
#include "generator/input/NEUTInputHandler.hxx"
#endif
#include "input/InputManager.hxx"

#include "event/MinimalEvent.hxx"

#include "exception/exception.hxx"

#include "utility/StringUtility.hxx"

#include "fhiclcpp/make_ParameterSet.h"
#include "string_parsers/from_string.hxx"

#include <string>

NEW_NUIS_EXCEPT(invalid_cli_arguments);

void SayUsage(char const *argv[]) {
  std::cout
      << "[USAGE]: " << argv[0]
      << "\n"
         "\t-c <config.fcl>             : FHiCL file containing study "
         "configuration. \n"
         "\t-s <sample name>            : FHiCL key of a single sample "
         "to run from the -c argument. \n"
         "\t-n <NTries>                 : Number of events toload. \n"
#ifdef USE_GENIE
         "\t--genie                     : Just use GENIEInputHandler directly "
#endif
#ifdef USE_NEUT
         "\t--neut                      : Just use NEUTInputHandler directly "
#endif
         "\t-G                          : Just use GetEntry directly "
         "rather than the GetMinimalEvent interface."
      << std::endl;
}

std::string fhicl_file = "";
std::string named_sample = "";
size_t NTries = 0;
bool GetEntry = false;

enum EGen {
  kManager,
#ifdef USE_NEUT
  kNEUT,
#endif
#ifdef USE_GENIE
  kGENIE,
#endif
};
EGen WhichGen = kManager;

void handleOpts(int argc, char const *argv[]) {
  int opt = 1;
  while (opt < argc) {
    if ((std::string(argv[opt]) == "-?") ||
        (std::string(argv[opt]) == "--help")) {
      SayUsage(argv);
      exit(0);
    } else if (std::string(argv[opt]) == "-c") {
      fhicl_file = argv[++opt];
    } else if (std::string(argv[opt]) == "-s") {
      named_sample = argv[++opt];
    } else if (std::string(argv[opt]) == "-n") {
      NTries = fhicl::string_parsers::str2T<size_t>(argv[++opt]);
    } else if (std::string(argv[opt]) == "-G") {
      GetEntry = true;
    }
#ifdef USE_GENIE
    else if (std::string(argv[opt]) == "--genie") {
      WhichGen = kGENIE;
    }
#endif
#ifdef USE_NEUT
    else if (std::string(argv[opt]) == "--neut") {
      WhichGen = kNEUT;
    }
#endif
    else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }
}

int main(int argc, char const *argv[]) {
  nuis::config::EnsureConfigurationRead("nuis.global.config.fcl");
  nuis::config::EnsureConfigurationRead("nuis.datacomparisons.fcl");

  handleOpts(argc, argv);

  if (!fhicl_file.size()) {
    SayUsage(argv);
    throw invalid_cli_arguments();
  }

  nuis::config::EnsureConfigurationRead(fhicl_file);

  fhicl::ParameterSet inps =
      nuis::config::GetDocument().get<fhicl::ParameterSet>(named_sample);
  IInputHandler const *IH;

  switch (WhichGen) {
  case kManager: {
    auto id = nuis::input::InputManager::Get().EnsureInputLoaded(inps);
    IH = &nuis::input::InputManager::Get().GetInputHandler(id);
    break;
  }
#ifdef USE_NEUT
  case kNEUT: {
    IInputHandler *IIH = new NEUTInputHandler();
    IIH->Initialize(inps);
    IH = IIH;
    break;
  }
#endif
#ifdef USE_GENIE
  case kGENIE: {
    IInputHandler *IIH = new GENIEInputHandler();
    IIH->Initialize(inps);
    IH = IIH;
    break;
  }
#endif
  default: {
    std::cout << "[ERROR]: Invalid input handler type." << std::endl;
    return 1;
  }
  }

  size_t ShoutEvery = NTries / 100;

  std::cout << "[INFO]: Read " << 0 << "/" << NTries << " GENIE events."
            << std::flush;
  for (size_t t_it = 0; t_it < NTries; ++t_it) {
    if (ShoutEvery && !(t_it % ShoutEvery)) {
      std::cout << "\r[INFO]: Read " << t_it << "/" << NTries
                << " GENIE events." << std::flush;
    }
    if (GetEntry) {
      IH->GetEntry(0);
    } else {
      IH->GetMinimalEvent(0);
    }
  }
  std::cout << std::endl;
}
