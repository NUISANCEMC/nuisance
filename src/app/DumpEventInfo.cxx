
#include "config/GlobalConfiguration.hxx"

#include "core/IInputHandler.hxx"
#include "core/MinimalEvent.hxx"

#include "plugins/Instantiate.hxx"

#include "fhiclcpp/make_ParameterSet.h"

#include <string>

int main() {
  nuis::config::EnsureConfigurationRead("./nuis.global.config.fcl");

  fhicl::ParameterSet ps = fhicl::make_ParameterSet("./test.fcl");

  nuis::plugins::plugin_traits<IInputHandler>::unique_ptr_t IH =
      nuis::plugins::Instantiate<IInputHandler>(
          ps.get<std::string>("sample.InputHandler"));

  IH->Initialize(ps.get<fhicl::ParameterSet>("sample"));

  for(IInputHandler::ev_index_t ev_it = 0; ev_it < IH->GetNEvents(); ++ev_it){
    nuis::core::MinimalEvent const &ev = IH->GetMinimalEvent(ev_it);
  }
}
