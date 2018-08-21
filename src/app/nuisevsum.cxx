#include "config/GlobalConfiguration.hxx"

#include "input/IInputHandler.hxx"

#include "event/MinimalEvent.hxx"

#include "samples/ISample.hxx"

#include "plugins/Instantiate.hxx"

#include "exception/exception.hxx"

#include "fhiclcpp/make_ParameterSet.h"
#include "string_parsers/from_string.hxx"

#include <string>

NEW_NUIS_EXCEPT(invalid_cli_arguments);

size_t NMax = std::numeric_limits<size_t>::max();
std::string input_file;
std::string input_type;

void SayUsage(char const *argv[]) {
  std::cout << "[USAGE]: " << argv[0]
            << "\n"
               "\t-i <input_file>             : Input file passed to named "
               "IInputHandler instance \n"
               "\t-H <input_handler>          : Name of IInputHandler subclass "
               "capable of reading NUISANCE events from the argument of -i.\n"
               "\t-n <NMax>                   : Maximum number of events to "
               "read. Will read entire input file by default.\n"
            << std::endl;
}

void handleOpts(int argc, char const *argv[]) {
  int opt = 1;
  while (opt < argc) {
    if ((std::string(argv[opt]) == "-?") ||
        (std::string(argv[opt]) == "--help")) {
      SayUsage(argv);
      exit(0);
    } else if (std::string(argv[opt]) == "-i") {
      input_file = argv[++opt];
    } else if (std::string(argv[opt]) == "-H") {
      input_type = argv[++opt];
    } else if (std::string(argv[opt]) == "-n") {
      NMax = fhicl::string_parsers::str2T<size_t>(argv[++opt]);
    } else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }
}

int main(int argc, char const *argv[]) {
  nuis::config::EnsureConfigurationRead("nuis.global.config.fcl");

  handleOpts(argc, argv);

  if (!input_type.length() || !input_file.length()) {
    SayUsage(argv);
    throw invalid_cli_arguments()
        << "[ERROR]: Require both -i and -H cli options to be passed.";
  }

  fhicl::ParameterSet sample_config;

  sample_config.put("input_type", input_type);
  sample_config.put("file", input_file);

  nuis::plugins::plugin_traits<ISample>::unique_ptr_t VerboseEventSummary =
      nuis::plugins::Instantiate<ISample>("VerboseEventSummary");

  VerboseEventSummary->Initialize(sample_config);
  VerboseEventSummary->ProcessSample(NMax);
}
