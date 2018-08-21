#include "config/GlobalConfiguration.hxx"

#include "input/IInputHandler.hxx"

#include "plugins/Instantiate.hxx"

#include "samples/IDataComparison.hxx"

#include <iostream>
#include <string>

int main() {
  nuis::config::EnsureConfigurationRead("nuis.global.config.fcl");
  nuis::config::EnsureConfigurationRead("nuis.datacomparisons.fcl");

  for (std::string const &comparison_set_key :
       nuis::config::GetDocument()
           .get<fhicl::ParameterSet>("data_comparisons")
           .get_names()) {
    for (std::string const &sample_name :
         nuis::config::GetDocument().get<std::vector<std::string>>(
             std::string("data_comparisons.") + comparison_set_key)) {

      nuis::plugins::plugin_traits<IDataComparison>::unique_ptr_t sample =
          nuis::plugins::Instantiate<IDataComparison>(sample_name);
      
      std::cout << sample->Name() << std::endl;
      std::cout << "\tJournal: " << sample->GetJournalReference() << std::endl;
      std::cout << "\tTarget: " << sample->GetTargetMaterial() << std::endl;
      std::cout << "\tFlux: " << sample->GetFluxDescription() << std::endl;
      std::cout << "\tSignal: " << sample->GetSignalDescription() << std::endl;
      std::cout << "\tDocs: " << sample->GetDocumentation() << std::endl;

    }
  }
}
