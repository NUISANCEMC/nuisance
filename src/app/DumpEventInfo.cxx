
#include "config/GlobalConfiguration.hxx"

#include "core/IInputHandler.hxx"
#include "core/MinimalEvent.hxx"

#include "samples/ISample.hxx"

#include "plugins/Instantiate.hxx"

#include "fhiclcpp/make_ParameterSet.h"

#include <string>

int main() {
  nuis::config::EnsureConfigurationRead("./nuis.global.config.fcl");

  fhicl::ParameterSet ps = fhicl::make_ParameterSet("./test.fcl");

  for (fhicl::ParameterSet const &samp_config :
       ps.get<std::vector<fhicl::ParameterSet>>("samples")) {

    std::cout << "[INFO]: Reading sample: "
              << samp_config.get<std::string>("name") << std::endl;

    nuis::plugins::plugin_traits<ISample>::unique_ptr_t sample =
        nuis::plugins::Instantiate<ISample>(
            samp_config.get<std::string>("name"));
            

    sample->Initialize(samp_config);
    sample->ProcessSample();
  }
}
