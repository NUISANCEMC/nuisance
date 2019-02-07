#include "config/GlobalConfiguration.hxx"

#include "input/IInputHandler.hxx"

#include "plugins/Instantiate.hxx"

#include "samples/IDataComparison.hxx"

#include "utility/StringUtility.hxx"

#include <fstream>
#include <iostream>
#include <regex>
#include <string>

std::string search_term = ".*";
bool fStrictRegex = false;
std::string config_out_filename = "";

void SayUsage(char const *argv[]) {
  std::cout << "[USAGE]: " << argv[0]
            << "\n"
               "\t-s <search regex> : Used to filter known IDataComparisons. "
               "\n\t\tWild cards are added to either side of the search term.\n"
               "\t-S <search regex> : Used to filter known IDataComparisons. "
               "\n\t\tThe exact passed term is used.\n"
               "\t-o <output_file>  : Dump example sample configuration file\n"
               "\t\tfor matching samples.\n"
            << std::endl;
}

void handleOpts(int argc, char const *argv[]) {
  int opt = 1;
  while (opt < argc) {
    if ((std::string(argv[opt]) == "-?") ||
        (std::string(argv[opt]) == "--help")) {
      SayUsage(argv);
      exit(0);
    } else if (std::string(argv[opt]) == "-s") {
      search_term = argv[++opt];
    } else if (std::string(argv[opt]) == "-S") {
      fStrictRegex = true;
      search_term = argv[++opt];
    } else if (std::string(argv[opt]) == "-o") {
      config_out_filename = argv[++opt];
    } else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }
}

NEW_NUIS_EXCEPT(invalid_output_file);

int main(int argc, char const *argv[]) {
  nuis::config::EnsureConfigurationRead("nuis.global.config.fcl");
  nuis::config::EnsureConfigurationRead("nuis.datacomparisons.fcl");

  handleOpts(argc, argv);

  std::regex rpattern(fStrictRegex ? search_term
                                   : std::string(".*") + search_term + ".*");

  std::vector<fhicl::ParameterSet> example_sample_configs;

  for (std::string const &comparison_set_key :
       nuis::config::GetDocument()
           .get<fhicl::ParameterSet>("data_comparisons")
           .get_names()) {
    for (std::string const &sample_name :
         nuis::config::GetDocument().get<std::vector<std::string>>(
             std::string("data_comparisons.") + comparison_set_key)) {

      if (!std::regex_match(sample_name, rpattern)) {
        continue;
      }

      nuis::plugins::plugin_traits<IDataComparison>::unique_ptr_t sample =
          nuis::plugins::Instantiate<IDataComparison>(sample_name);

      std::cout << sample->Name() << std::endl;
      std::cout << "\tJournal: " << sample->GetJournalReference() << std::endl;
      std::cout << "\tTarget: " << sample->GetTargetMaterial() << std::endl;
      std::cout << "\tFlux: " << sample->GetFluxDescription() << std::endl;
      std::cout << "\tSignal: " << sample->GetSignalDescription() << std::endl;
      std::cout << "\tDocs: \n"
                << nuis::utility::indent_apply_width(sample->GetDocumentation(),
                                                     10)
                << std::endl;
      std::cout << "\tExample_Config: {\n"
                << nuis::utility::indent_apply_width(
                       sample->GetExampleConfiguration().to_indented_string(),
                       12)
                << "\n\t}\n"
                << std::endl;
      ;
      example_sample_configs.push_back(sample->GetExampleConfiguration());
    }
  }

  if (config_out_filename.length()) {
    std::ofstream out_file(config_out_filename);

    if (!out_file) {
      throw invalid_output_file() << "[ERROR]: Failed to open output file: "
                                  << std::quoted(config_out_filename);
    }

    fhicl::ParameterSet example_config;
    example_config.put("samples", example_sample_configs);
    out_file << example_config.to_indented_string();
  }
}
