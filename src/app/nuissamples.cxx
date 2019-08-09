#include "config/GlobalConfiguration.hxx"

#include "input/IInputHandler.hxx"

#include "plugins/Instantiate.hxx"

#include "samples/IDataComparison.hxx"

#include "utility/StringUtility.hxx"

#include <fstream>
#include <iostream>
#include <regex>
#include <string>

std::string name_search_term = ".*";
bool strict_name_regex = false;

std::string target_search_term = ".*";
bool strict_target_regex = false;

std::string flux_search_term = ".*";
bool strict_flux_regex = false;

std::string signal_search_term = ".*";
bool strict_signal_regex = false;

std::string year_search_term = ".*";

std::string config_out_filename = "";

bool NameOnly = false;

void SayUsage(char const *argv[]) {
  std::cout
      << "[USAGE]: " << argv[0]
      << "\n"
         "\t-n,-N,-t,-T,-y, -s, -S, -f, -F <search regex> : Filters known \n"
         "\t           IDataComparisons by the search term \n"
         "\t           (-n: Name, -t: Target, -y: Year, -s: Signal, -f Flux).\n"
         "\t             Capitalized versions filter on exact match.\n"
         "\t-o <output_file>              : Dump example sample configuration "
         "file for matching \n"
         "\t                                samples.\n"
         "\t--name-only                   : Only write out matching sample "
         "names. \n"
         "\t                                (Still applies all search terms)\n"
      << std::endl;
}

void handleOpts(int argc, char const *argv[]) {
  int opt = 1;
  while (opt < argc) {
    if ((std::string(argv[opt]) == "-?") ||
        (std::string(argv[opt]) == "--help")) {
      SayUsage(argv);
      exit(0);
    } else if (std::string(argv[opt]) == "-n") {
      name_search_term = argv[++opt];
    } else if (std::string(argv[opt]) == "-N") {
      strict_name_regex = true;
      name_search_term = argv[++opt];
    } else if (std::string(argv[opt]) == "-t") {
      target_search_term = argv[++opt];
    } else if (std::string(argv[opt]) == "-T") {
      strict_target_regex = true;
      target_search_term = argv[++opt];
    } else if (std::string(argv[opt]) == "-s") {
      signal_search_term = argv[++opt];
    } else if (std::string(argv[opt]) == "-S") {
      strict_signal_regex = true;
      signal_search_term = argv[++opt];
    } else if (std::string(argv[opt]) == "-f") {
      flux_search_term = argv[++opt];
    } else if (std::string(argv[opt]) == "-F") {
      strict_flux_regex = true;
      flux_search_term = argv[++opt];
    } else if (std::string(argv[opt]) == "-y") {
      year_search_term = argv[++opt];
    } else if (std::string(argv[opt]) == "-o") {
      config_out_filename = argv[++opt];
    } else if (std::string(argv[opt]) == "--name-only") {
      NameOnly = true;
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

  std::regex rpattern_name(strict_name_regex
                               ? name_search_term
                               : std::string(".*") + name_search_term + ".*");
  std::regex rpattern_target(
      strict_target_regex ? target_search_term
                          : std::string(".*") + target_search_term + ".*");
  std::regex rpattern_flux(strict_flux_regex
                               ? flux_search_term
                               : std::string(".*") + flux_search_term + ".*");
  std::regex rpattern_signal(
      strict_signal_regex ? signal_search_term
                          : std::string(".*") + signal_search_term + ".*");

  std::regex rpattern_year(std::string(".*") + year_search_term + ".*");

  std::vector<fhicl::ParameterSet> example_sample_configs;

  for (std::string const &comparison_set_key :
       nuis::config::GetDocument()
           .get<fhicl::ParameterSet>("data_comparisons")
           .get_names()) {
    for (std::string const &sample_name :
         nuis::config::GetDocument().get<std::vector<std::string>>(
             std::string("data_comparisons.") + comparison_set_key)) {

      if (!std::regex_match(sample_name, rpattern_name)) {
        continue;
      }

      fhicl::ParameterSet sample_global_config =
          nuis::config::GetDocument().get<fhicl::ParameterSet>(
              std::string("global.sample_configuration.") + sample_name,
              fhicl::ParameterSet{});

      std::vector<nuis::plugins::plugin_traits<IDataComparison>::unique_ptr_t>
          samples;

      // If a single sample can produce multiple comparisons this can be used to
      // report them as separate samples in the nuissamples list
      if (sample_global_config.has_key("sub_samples")) {
        for (fhicl::ParameterSet const &ss_ps :
             sample_global_config.get<std::vector<fhicl::ParameterSet>>(
                 "sub_samples")) {
          samples.push_back(
              nuis::plugins::Instantiate<IDataComparison>(sample_name));
          samples.back()->Initialize(ss_ps);
        }
      } else {
        samples.push_back(
            nuis::plugins::Instantiate<IDataComparison>(sample_name));
      }

      for (auto &sample : samples) {
        if (!std::regex_match(sample->GetTargetMaterial(), rpattern_target)) {
          continue;
        }

        if (!std::regex_match(sample->GetFluxDescription(), rpattern_flux)) {
          continue;
        }

        if (!std::regex_match(sample->GetSignalDescription(),
                              rpattern_signal)) {
          continue;
        }

        if (!std::regex_match(sample->GetYear(), rpattern_year)) {
          continue;
        }

        std::cout << sample->Name() << std::endl;
        if (!NameOnly) {
          std::cout << "\tJournal: " << sample->GetJournalReference()
                    << std::endl;
          std::cout << "\tDOI: " << sample->GetDOI() << std::endl;
          std::cout << "\tYear: " << sample->GetYear() << std::endl;
          std::cout << "\tTarget: " << sample->GetTargetMaterial() << std::endl;
          std::cout << "\tFlux: " << sample->GetFluxDescription() << std::endl;
          std::cout << "\tSignal: " << sample->GetSignalDescription()
                    << std::endl;
          std::cout << "\tDocs: \n"
                    << nuis::utility::indent_apply_width(
                           sample->GetDocumentation(), 10)
                    << std::endl;
          std::cout
              << "\tExample_Config: {\n"
              << nuis::utility::indent_apply_width(
                     sample->GetExampleConfiguration().to_indented_string(), 12)
              << "\n\t}\n"
              << std::endl;
        }

        example_sample_configs.push_back(sample->GetExampleConfiguration());
      }
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
