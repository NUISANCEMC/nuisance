#include "config/GlobalConfiguration.hxx"

#include "input/IInputHandler.hxx"

#include "event/MinimalEvent.hxx"

#include "samples/IEventProcessor.hxx"

#include "plugins/Instantiate.hxx"

#include "persistency/ROOTOutput.hxx"

#include "exception/exception.hxx"

#include "variation/WeightManager.hxx"

#include "fhiclcpp/make_ParameterSet.h"
#include "string_parsers/from_string.hxx"

#include <string>

NEW_NUIS_EXCEPT(invalid_cli_arguments);

size_t NMax = std::numeric_limits<size_t>::max();
std::string input_file = "";
std::string input_type = "";
std::string default_stream_override = "";
std::string variation_config_file = "";

void SayUsage(char const *argv[]) {
  std::cout << "[USAGE]: " << argv[0]
            << "\n"
               "\t-i <input_file>             : Input file passed to named "
               "IInputHandler instance \n"
               "\t-H <input_handler>          : Name of IInputHandler subclass "
               "capable of reading NUISANCE events from the argument of -i.\n"
               "\t-n <NMax>                   : Maximum number of events to "
               "read. Will read entire input file by default.\n"
               "\t-o <out.file[:mode]>        : Default output stream name "
               "override, if unspecified mode will be 'CREATE'. \n"
               "\t--variation-config <vc.fcl> : Read weight engine state from "
               "input fhicl file. \n"
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
    } else if (std::string(argv[opt]) == "-o") {
      default_stream_override = argv[++opt];
    } else if (std::string(argv[opt]) == "--variation-config") {
      variation_config_file = argv[++opt];
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

  if (default_stream_override.size()) {
    std::vector<std::string> split =
        nuis::utility::split(default_stream_override, ":");
    std::string open_mode = (split.size() > 1) ? split[1] : "CREATE";

    nuis::persistency::NewStream("default", split[0], open_mode);
  }

  fhicl::ParameterSet const &global_config = nuis::config::GetDocument();

  if (variation_config_file.size()) {
    nuis::config::EnsureConfigurationRead(variation_config_file);

    if (global_config.has_key("weight_engines")) {
      for (fhicl::ParameterSet const &we_ps :
           global_config.get<std::vector<fhicl::ParameterSet>>(
               "weight_engines")) {
        nuis::variation::WeightManager::Get().EnsureWeightProviderLoaded(we_ps);
      }
      nuis::variation::WeightManager::Get().ReconfigureWeightEngines();
    }
  }

  fhicl::ParameterSet sample_config;

  sample_config.put("input_type", input_type);
  sample_config.put("file", input_file);

  nuis::plugins::plugin_traits<IEventProcessor>::unique_ptr_t
      VerboseEventSummary = nuis::plugins::Instantiate<IEventProcessor>(
          "EventSummary_ECTJune2019");

  VerboseEventSummary->Initialize(sample_config);
  VerboseEventSummary->ProcessSample(NMax);

  nuis::persistency::CloseOpenTFiles();
}
