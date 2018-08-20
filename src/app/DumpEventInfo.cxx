#include "config/GlobalConfiguration.hxx"

#include "input/IInputHandler.hxx"

#include "event/MinimalEvent.hxx"

#include "samples/ISample.hxx"

#include "plugins/Instantiate.hxx"

#include "exception/exception.hxx"

#include "fhiclcpp/make_ParameterSet.h"

#include <string>

NEW_NUIS_EXCEPT(invalid_cli_arguments);

int main(int argc, char const *argv[]) {
  nuis::config::EnsureConfigurationRead("nuis.global.config.fcl");

  if (argc != 2) {
    throw invalid_cli_arguments()
        << "[ERROR]: Expected to be passed a single FHiCL file name or "
           "absolute or relative path. N.B. Files in the local directory must "
           "be fully qualified like \"$ "
        << argv[0] << " ./myconf.fcl\".";
  }

  fhicl::ParameterSet ps = fhicl::make_ParameterSet(argv[1]);

  size_t NMax = std::numeric_limits<size_t>::max();

  if (ps.has_key("nmax")) {
    NMax = ps.get<size_t>("nmax");
  }

  for (fhicl::ParameterSet const &samp_config :
       ps.get<std::vector<fhicl::ParameterSet>>("samples")) {

    std::cout << "[INFO]: Reading sample: "
              << samp_config.get<std::string>("name") << std::endl;

    nuis::plugins::plugin_traits<ISample>::unique_ptr_t sample =
        nuis::plugins::Instantiate<ISample>(
            samp_config.get<std::string>("name"));

    sample->Initialize(samp_config);
    sample->ProcessSample(NMax);
    sample->Write();
  }
}
