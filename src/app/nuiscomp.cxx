#include "config/GlobalConfiguration.hxx"

#include "input/InputManager.hxx"

#include "event/MinimalEvent.hxx"

#include "samples/IDataComparison.hxx"

#include "plugins/Instantiate.hxx"

#include "exception/exception.hxx"

#include "persistency/ROOTOutput.hxx"

#include "variation/WeightManager.hxx"

#include "utility/StringUtility.hxx"

#include "fhiclcpp/make_ParameterSet.h"

#include <string>

NEW_NUIS_EXCEPT(invalid_cli_arguments);

void SayUsage(char const *argv[]) {
  std::cout << "[USAGE]: " << argv[0]
            << "\n"
               "\t-c <config.fcl>             : FHiCL file containing study "
               "configuration. \n"
               "\t-s <sample name>            : FHiCL key of a single sample "
               "to run from the -c argument. \n"
               "\t-o <out.file[:mode]>        : Default output stream name "
               "override, if unspecified mode will be 'CREATE'. \n"
            << std::endl;
}

std::string fhicl_file = "";
std::string named_sample = "";
std::string default_stream_override = "";

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
    } else if (std::string(argv[opt]) == "-o") {
      default_stream_override = argv[++opt];
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
  nuis::config::EnsureConfigurationRead("nuis.datacomparisons.fcl");

  handleOpts(argc, argv);

  if (!fhicl_file.size()) {
    SayUsage(argv);
    throw invalid_cli_arguments();
  }

  nuis::config::EnsureConfigurationRead(fhicl_file);

  if (default_stream_override.size()) {
    std::vector<std::string> split =
        nuis::utility::split(default_stream_override, ":");
    std::string open_mode = (split.size() > 1) ? split[1] : "CREATE";

    nuis::persistency::NewStream("default", split[0], open_mode);
  }

  fhicl::ParameterSet const &global_config = nuis::config::GetDocument();
  size_t NMax =
      global_config.get<size_t>("nmax", std::numeric_limits<size_t>::max());

  std::vector<fhicl::ParameterSet> samples;
  if (named_sample.size()) {
    samples.push_back(global_config.get<fhicl::ParameterSet>(named_sample));
  } else {
    samples = global_config.get<std::vector<fhicl::ParameterSet>>("samples");
  }

  if (global_config.has_key("weight_engines")) {
    for (fhicl::ParameterSet const &we_ps :
         global_config.get<std::vector<fhicl::ParameterSet>>(
             "weight_engines")) {
      nuis::variation::WeightManager::Get().EnsureWeightProviderLoaded(we_ps);
    }
    nuis::variation::WeightManager::Get().ReconfigureWeightEngines();
  }

  for (fhicl::ParameterSet const &samp_config : samples) {

    std::cout << "[INFO]: Reading sample: "
              << samp_config.get<std::string>("name") << std::endl;

    nuis::plugins::plugin_traits<IDataComparison>::unique_ptr_t sample =
        nuis::plugins::Instantiate<IDataComparison>(
            samp_config.get<std::string>("name"));

    sample->Initialize(samp_config);
    sample->ProcessSample(NMax);
    sample->Write();

    // Ensures no re-use of samples but cleans up the memory.
    nuis::input::InputManager::Get().Clear();
  }

  nuis::persistency::CloseOpenTFiles();
}
