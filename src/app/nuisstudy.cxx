#include "config/GlobalConfiguration.hxx"

#include "input/IInputHandler.hxx"

#include "event/MinimalEvent.hxx"

#include "samples/IEventProcessor.hxx"

#include "plugins/Instantiate.hxx"

#include "exception/exception.hxx"

#include "persistency/ROOTOutput.hxx"

#include "fhiclcpp/make_ParameterSet.h"

#include <string>

NEW_NUIS_EXCEPT(invalid_cli_arguments);

int main(int argc, char const *argv[]) {
  nuis::config::EnsureConfigurationRead("nuis.global.config.fcl");
  nuis::config::EnsureConfigurationRead("nuis.datacomparisons.fcl");

  if (argc != 2 || (std::string(argv[1]) == "-?") ||
      (std::string(argv[1]) == "--help")) {
    throw invalid_cli_arguments()
        << "[ERROR]: Expected to be passed a single FHiCL file name or "
           "absolute or relative path. N.B. Files in the local directory must "
           "be fully qualified like \"$ "
        << argv[0] << " ./myconf.fcl\".";
  }

  nuis::config::EnsureConfigurationRead(argv[1]);

  size_t NMax = nuis::config::GetDocument().get<size_t>(
      "nmax", std::numeric_limits<size_t>::max());

  for (fhicl::ParameterSet const &samp_config :
       nuis::config::GetDocument().get<std::vector<fhicl::ParameterSet>>(
           "samples")) {

    std::cout << "[INFO]: Reading sample: "
              << samp_config.get<std::string>("name") << std::endl;

    nuis::plugins::plugin_traits<IEventProcessor>::unique_ptr_t sample =
        nuis::plugins::Instantiate<IEventProcessor>(
            samp_config.get<std::string>("name"));

    sample->Initialize(samp_config);
    sample->ProcessSample(NMax);
    sample->Write();
  }

  nuis::persistency::CloseOpenTFiles();
}
