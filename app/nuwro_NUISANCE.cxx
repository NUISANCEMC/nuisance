#ifdef __NUWRO_ENABLED__
#include "ComparisonRoutines.h"
#include "ParserUtils.h"
#include "TargetUtils.h"

// All possible inputs
std::string gOptEnergyDef;
std::vector<double> gOptEnergyRange;
int gOptNumberEvents = -1;
int gOptNumberTestEvents = 5E6;
std::string gOptGeneratorList = "Default";
std::string gOptCrossSections =
    "Default";  // If default this will look in
                // $NUISANCE/data/nuwro/default_params.txt
int gOptSeed = time(NULL);
std::string gOptTargetDef = "";
std::string gOptFluxDef = "";
std::string gOptOutputFile = "";
int gOptRunNumber = -1;

void GetCommandLineArgs(int argc, char** argv);
void PrintSyntax(void);

std::string GetDynamicModes(std::string list) {
  LOG(FIT) << "Using " << list << " to define interaction modes." << std::endl;
  std::map<std::string, int> modes;

  if (!list.compare("Default")) {
    modes["dyn_qel_cc"] = 1;  // Quasi elastic charged current
    modes["dyn_qel_nc"] = 1;  // Quasi elastic neutral current
    modes["dyn_res_cc"] = 1;  // Resonant charged current
    modes["dyn_res_nc"] = 1;  // Resonant neutral current
    modes["dyn_dis_cc"] = 1;  // Deep inelastic charged current
    modes["dyn_dis_nc"] = 1;  // Deep inelastic neutral current
    modes["dyn_coh_cc"] = 1;  // Coherent charged current
    modes["dyn_coh_nc"] = 1;  // Coherent neutral current
    modes["dyn_mec_cc"] = 0;  // Meson exchange charged current
    modes["dyn_mec_nc"] = 0;  // Meson exchange neutral current

  } else if (!list.compare("DefaultFree")) {
    modes["dyn_qel_cc"] = 1;  // Quasi elastic charged current
    modes["dyn_qel_nc"] = 1;  // Quasi elastic neutral current
    modes["dyn_res_cc"] = 1;  // Resonant charged current
    modes["dyn_res_nc"] = 1;  // Resonant neutral current
    modes["dyn_dis_cc"] = 1;  // Deep inelastic charged current
    modes["dyn_dis_nc"] = 1;  // Deep inelastic neutral current
    modes["dyn_coh_cc"] = 0;  // Coherent charged current
    modes["dyn_coh_nc"] = 0;  // Coherent neutral current
    modes["dyn_mec_cc"] = 0;  // Meson exchange charged current
    modes["dyn_mec_nc"] = 0;  // Meson exchange neutral current

  } else if (!list.compare("Default+MEC")) {
    modes["dyn_qel_cc"] = 1;  // Quasi elastic charged current
    modes["dyn_qel_nc"] = 1;  // Quasi elastic neutral current
    modes["dyn_res_cc"] = 1;  // Resonant charged current
    modes["dyn_res_nc"] = 1;  // Resonant neutral current
    modes["dyn_dis_cc"] = 1;  // Deep inelastic charged current
    modes["dyn_dis_nc"] = 1;  // Deep inelastic neutral current
    modes["dyn_coh_cc"] = 1;  // Coherent charged current
    modes["dyn_coh_nc"] = 1;  // Coherent neutral current
    modes["dyn_mec_cc"] = 1;  // Meson exchange charged current
    modes["dyn_mec_nc"] = 1;  // Meson exchange neutral current

  } else {
    THROW("Event generator list " << list << " not found!");
  }

  std::string modestring = "";
  for (std::map<std::string, int>::iterator iter = modes.begin();
       iter != modes.end(); iter++) {
    std::cout << " -> " << iter->first << " : " << iter->second << std::endl;
    modestring += " -p  \"" + iter->first + "=" +
                  GeneralUtils::IntToStr(iter->second) + "\"";
  }
  return modestring;
}

std::string GetFluxDefinition(std::string flux, std::string out) {
  LOG(FIT) << "Using " << flux << " to define NuWro beam." << std::endl;

  // By default the flux is type 6 with a root file
  std::vector<std::string> fluxargs = GeneralUtils::ParseToStr(flux, ",");
  if (fluxargs.size() < 2) {
    THROW(
        "Expected flux in the format: file.root,hist_name1[pdg1],... : "
        "reveived : "
        << flux);
  }

  // Build Map
  std::map<std::string, std::string> fluxmap;
  fluxmap["beam_type"] = "6";
  fluxmap["beam_inputroot"] = fluxargs[0];
  fluxmap["beam_inputroot_nue"] = "";
  fluxmap["beam_inputroot_nueb"] = "";
  fluxmap["beam_inputroot_numu"] = "";
  fluxmap["beam_inputroot_numub"] = "";
  fluxmap["beam_inputroot_nutau"] = "";
  fluxmap["beam_inputroot_nutaub"] = "";

  // Split by beam bdgs
  for (int i = 1; i < fluxargs.size(); i++) {
    std::string histdef = fluxargs[i];
    string::size_type open_bracket = histdef.find("[");
    string::size_type close_bracket = histdef.find("]");
    string::size_type ibeg = 0;
    string::size_type iend = open_bracket;
    string::size_type jbeg = open_bracket + 1;
    string::size_type jend = close_bracket - 1;
    std::string name = std::string(histdef.substr(ibeg, iend).c_str());
    int pdg = atoi(histdef.substr(jbeg, jend).c_str());

    if (pdg == 12)
      fluxmap["beam_inputroot_nue"] = name;
    else if (pdg == -12)
      fluxmap["beam_inputroot_nueb"] = name;
    else if (pdg == 14)
      fluxmap["beam_inputroot_numu"] = name;
    else if (pdg == -14)
      fluxmap["beam_inputroot_numub"] = name;
    else if (pdg == 16)
      fluxmap["beam_inputroot_nutau"] = name;
    else if (pdg == -16)
      fluxmap["beam_inputroot_nutaub"] = name;
  }

  // Now create a new flux file matching the output file
  std::cout << " -> Moving flux from '" + fluxmap["beam_inputroot"] +
                   "' to current directory to keep everything organised."
            << std::endl;
  TFile* fluxread = new TFile(fluxmap["beam_inputroot"].c_str(), "READ");
  TFile* fluxwrite = new TFile((out + ".flux.root").c_str(), "RECREATE");

  for (std::map<std::string, std::string>::iterator iter = fluxmap.begin();
       iter != fluxmap.end(); iter++) {
    TH1* temp = (TH1*)fluxread->Get(iter->second.c_str());
    if (!temp) continue;
    TH1D* cuthist = (TH1D*)temp->Clone();

    // Restrict energy range if required
    if (gOptEnergyRange.size() == 2) {
      for (int i = 0; i < cuthist->GetNbinsX(); i++) {
        if (cuthist->GetXaxis()->GetBinCenter(i + 1) < gOptEnergyRange[0] or
            cuthist->GetXaxis()->GetBinCenter(i + 1) > gOptEnergyRange[1]) {
          cuthist->SetBinContent(i + 1, 0.0);
        }
      }
    }

    // Check Flux
    if (cuthist->Integral() <= 0.0) {
      THROW("Flux histogram " << iter->second << " has integral <= 0.0");
    }

    // Save
    fluxwrite->cd();
    cuthist->Write();
  }
  std::cout << " ->-> Saved to : " << (out + ".flux.root") << std::endl;
  fluxmap["beam_inputroot"] = (out + ".flux.root");
  fluxwrite->Close();

  // Return a parameters string
  std::string fluxstring = "";
  for (std::map<std::string, std::string>::iterator iter = fluxmap.begin();
       iter != fluxmap.end(); iter++) {
    std::cout << " -> " << iter->first << " : " << iter->second << std::endl;
    fluxstring += " -p  \"" + iter->first + "=" + iter->second + "\"";
  }
  return fluxstring;
}

std::string GetTargetDefinition(std::string target) {
  LOG(FIT) << "Defining NuWro Target from : " << target << std::endl;

  // Target is given as either a single PDG, or a combo with the total number of
  // nucleons
  std::vector<std::string> trgts = GeneralUtils::ParseToStr(target, ",");
  std::string targetstring = "";

  // Single Target
  if (trgts.size() == 1) {
    int PDG = GeneralUtils::StrToInt(trgts[0]);
    int Z = TargetUtils::GetTargetZFromPDG(PDG);
    int N = TargetUtils::GetTargetAFromPDG(PDG) - Z;
    int TOTAL = 1;
    targetstring += (" -p \"target_content=" + GeneralUtils::IntToStr(Z) + " " +
                     GeneralUtils::IntToStr(N) + " " +
                     GeneralUtils::IntToStr(TOTAL) + "x" + "\"");

    // Combined target
  } else if (trgts.size() > 1) {
    int NUCLEONS = GeneralUtils::StrToInt(trgts[0]);

    // Loop over all targets
    for (size_t i = 1; i < trgts.size(); i++) {
      // Extra PDG and W
      std::string tgtdef = trgts[i];
      string::size_type open_bracket = tgtdef.find("[");
      string::size_type close_bracket = tgtdef.find("]");
      string::size_type ibeg = 0;
      string::size_type iend = open_bracket;
      string::size_type jbeg = open_bracket + 1;
      string::size_type jend = close_bracket - 1;
      int PDG = atoi(tgtdef.substr(ibeg, iend).c_str());
      double W = atof(tgtdef.substr(jbeg, jend).c_str());

      // extract Z N
      int Z = TargetUtils::GetTargetZFromPDG(PDG);
      int A = TargetUtils::GetTargetAFromPDG(PDG);
      int N = TargetUtils::GetTargetAFromPDG(PDG) - Z;
      std::cout << "Target " << PDG << " Z" << Z << " N" << N << std::endl;

      // extract weight
      int TOTAL = round(double(NUCLEONS) * W / A);

      if (i == 1) {
        targetstring += (" -p \"target_content=" + GeneralUtils::IntToStr(Z) +
                         " " + GeneralUtils::IntToStr(N) + " " +
                         GeneralUtils::IntToStr(TOTAL) + "x" + "\"");
      } else {
        targetstring += (" -p \"target_content+=" + GeneralUtils::IntToStr(Z) +
                         " " + GeneralUtils::IntToStr(N) + " " +
                         GeneralUtils::IntToStr(TOTAL) + "x" + "\"");
      }
    }

    // No target given!
  } else {
    THROW("No target given : " << target);
  }

  std::cout << " -> " << targetstring << std::endl;
  return targetstring;
}

std::string GetEventAndSeedDefinition(int nevents, int ntestevents, int seed) {
  std::string eventdef = "";
  eventdef +=
      " -p \"number_of_events=" + GeneralUtils::IntToStr(nevents) + "\"";
  eventdef += " -p \"number_of_test_events=" +
              GeneralUtils::IntToStr(ntestevents) + "\"";
  eventdef += " -p \"random_seed=" + GeneralUtils::IntToStr(seed) + "\"";

  LOG(FIT) << "Event Definition: " << std::endl;
  std::cout << " -> number_of_events      : " << nevents << std::endl;
  std::cout << " -> number_of_test_events : " << ntestevents << std::endl;
  std::cout << " -> seed    : " << seed << std::endl;

  return eventdef;
}

//____________________________________________________________________________
int main(int argc, char** argv) {
  LOG(FIT) << "==== RUNNING nuwro_nuisance Event Generator =====" << std::endl;
  GetCommandLineArgs(argc, argv);

  // Calculate the dynamic modes definition
  std::string dynparamsdef = GetDynamicModes(gOptGeneratorList);

  // Get Flux and Target definition
  std::string fluxparamsdef = GetFluxDefinition(gOptFluxDef, gOptOutputFile);
  std::string targetparamsdef = GetTargetDefinition(gOptTargetDef);

  // Get Run Definition
  std::string eventparamsdef = GetEventAndSeedDefinition(
      gOptNumberEvents, gOptNumberTestEvents, gOptSeed);

  // Run NuWro Externally!
  LOG(FIT) << "==== Actually running nuwro! ===" << std::endl;
  std::string nuwrocommand = "nuwro";
  nuwrocommand += " -i " + gOptCrossSections;
  nuwrocommand += " -o " + gOptOutputFile;
  nuwrocommand += " " + fluxparamsdef;
  nuwrocommand += " " + dynparamsdef;
  nuwrocommand += " " + eventparamsdef;
  nuwrocommand += " " + targetparamsdef;
  std::cout << nuwrocommand << std::endl;
  sleep(10);
  system((nuwrocommand).c_str());

  return 0;
}

//____________________________________________________________________________
void GetCommandLineArgs(int argc, char** argv) {
  // Check for -h flag.
  for (int i = 0; i < argc; i++) {
    if (!std::string(argv[i]).compare("-h")) PrintSyntax();
  }

  // Format is nuwro -r run_number -n n events
  std::vector<std::string> args = GeneralUtils::LoadCharToVectStr(argc, argv);
  ParserUtils::ParseArgument(args, "-n", gOptNumberEvents, false);
  if (gOptNumberEvents == -1) {
    THROW("No event count passed to nuwro_NUISANCE!");
  }

  // Flux/Energy Specs
  ParserUtils::ParseArgument(args, "-e", gOptEnergyDef, false);
  gOptEnergyRange = GeneralUtils::ParseToDbl(gOptEnergyDef, ",");

  ParserUtils::ParseArgument(args, "-f", gOptFluxDef, false);
  if (gOptFluxDef.empty() and gOptEnergyRange.size() < 1) {
    THROW("No flux or energy range given to nuwro_nuisance!");

  } else if (gOptFluxDef.empty() and gOptEnergyRange.size() == 1) {
    // Fixed energy, make sure -p is given
    THROW("nuwro_NUISANCE cannot yet do fixed energy!");

  } else if (gOptFluxDef.empty() and gOptEnergyRange.size() == 2) {
    // Uniform energy range
    THROW("nuwro_NUISANCE cannot yet do a uniform energy range!");

  } else if (!gOptFluxDef.empty()) {
    // Try to convert the flux definition if possible.
    std::string convflux = BeamUtils::ConvertFluxIDs(gOptFluxDef);
    if (!convflux.empty()) gOptFluxDef = convflux;

  } else {
    THROW("Unknown flux energy range combination!");
  }

  ParserUtils::ParseArgument(args, "-t", gOptTargetDef, false);
  if (gOptTargetDef.empty()) {
    THROW("No Target passed to nuwro_nuisance! use the '-t' argument.");
  } else {
    std::string convtarget = TargetUtils::ConvertTargetIDs(gOptTargetDef);
    if (!convtarget.empty()) gOptTargetDef = convtarget;
  }

  ParserUtils::ParseArgument(args, "-r", gOptRunNumber, false);
  ParserUtils::ParseArgument(args, "-o", gOptOutputFile, false);
  if (gOptOutputFile.empty()) {
    if (gOptRunNumber == -1) gOptRunNumber = 1;
    LOG(FIT) << "No output file given! Saving file to : nuwrogen."
             << gOptRunNumber << ".event.root" << std::endl;
    gOptOutputFile =
        "nuwrogen." + GeneralUtils::IntToStr(gOptRunNumber) + ".event.root";
  } else {
    // if no run number given leave as is, else add run number.
    if (gOptRunNumber != -1) {
      gOptOutputFile += "." + GeneralUtils::IntToStr(gOptRunNumber) + ".root";
    } else {
      gOptRunNumber = 0;
    }
  }

  ParserUtils::ParseArgument(args, "--cross-section", gOptCrossSections, false);
  if (!gOptCrossSections.compare("Default")) {
    LOG(FIT) << "No Parameters File passed. Using default NuWro one."
             << std::endl;
    char* const var = getenv("NUISANCE");
    if (!var) {
      std::cout << "Cannot find top level directory! Set the NUISANCE "
                   "environmental variable"
                << std::endl;
      exit(-1);
    }
    std::string topnuisancedir = string(var);
    gOptCrossSections = topnuisancedir + "/nuwro/Default_params.txt";
  }

  ParserUtils::ParseArgument(args, "--event-generator-list", gOptGeneratorList,
                             false);
  ParserUtils::ParseArgument(args, "--seed", gOptSeed, false);
  ParserUtils::ParseArgument(args, "--test-events", gOptNumberTestEvents,
                             false);

  // Final Check and output
  if (args.size() > 0) {
    PrintSyntax();
    ParserUtils::CheckBadArguments(args);
  }

  LOG(FIT) << "Generating NuWro Events with the following properties:"
           << std::endl
           << " -> Energy      : " << gOptEnergyDef << " ("
           << gOptEnergyRange.size() << ")" << std::endl
           << " -> NEvents     : " << gOptNumberEvents << std::endl
           << " -> NTestEvents : " << gOptNumberTestEvents << std::endl
           << " -> Generators  : " << gOptGeneratorList << std::endl
           << " -> XSecPars    : " << gOptCrossSections << std::endl
           << " -> Seed        : " << gOptSeed << std::endl
           << " -> Target      : " << gOptTargetDef << std::endl
           << " -> Flux        : " << gOptFluxDef << std::endl
           << " -> Output      : " << gOptOutputFile << std::endl
           << " -> Run         : " << gOptRunNumber << std::endl;
  return;
}
//____________________________________________________________________________
void PrintSyntax(void) {
  LOG(FIT) << "\n\n"
           << "Syntax:"
           << "\n"
           << "\n      nuwro_nuisance [-h]"
           << "\n               -n nev"
           << "\n               -f flux_description"
           << "\n               -t target_description"
           << "\n              [ -r run_number ]"
           << "\n              [ -o output_file ]"
           << "\n              [ --cross-section /path/to/params.txt ]"
           << "\n              [ --event-generator-list mode_definition ]"
           << "\n              [ --seed seed_value ]"
           << "\n              [ --test-events ntest ]"
           << "\n \n";

  LOG(FIT)
      << "\n\n Arguments:"
      << "\n"
      << "\n -n nev"
      << "\n    -> Total number of events to generate (e.g. 2500000)"
      << "\n"
      << "\n -f flux_description"
      << "\n    Definition of the flux to be read in from a ROOT file."
      << "\n"
      << "\n    Multiple histograms can be read in from the same file using"
      << "\n    the format '-f file.root,hist1[pdg1],hist2[pdg2]"
      << "\n    e.g. \'-f "
         "./flux/myfluxfile.root,numu_flux[14],numubar_flux[-14]\'"
      << "\n"
      << "\n    When passing in multiple histograms, the nuwro_nuisance will"
      << "\n    generate a single file containing both sets of events with the"
      << "\n    correct ratios for each set."
      << "\n"
      << "\n    A flux can also be given according to any of the flux IDs shown"
      << "\n    at the end of this help message."
      << "\n    e.g. \' -f MINERvA_fhc_numu\' "
      << "\n"
      << "\n -t target_description"
      << "\n    Definition of the target to be used. Multiple targets can be "
         "given."
      << "\n"
      << "\n    To pass a single target just provide the target PDG"
      << "\n    e.g. \' -t 1000060120 \'"
      << "\n"
      << "\n    To pass a combined target provide a list containing the "
         "following"
      << "\n    \' -t TotalNucleons,Target1[Weight1],Target2[Weight2],.. where "
         "the "
      << "\n    TotalNucleons is the total nucleons combined, Target1 is the "
         "PDG "
      << "\n    of the first target, and Weight1 is the fractional weight of "
         "the "
      << "\n    first target."
      << "\n    e.g. \' -t 13,1000060120[0.9231],1000010010[0.0769] \'"
      << "\n"
      << "\n    Target can also be specified by the target IDs given at the "
         "end of"
      << "\n    this help message."
      << "\n    e.g. \' -t CH2 \'"
      << "\n"
      << "\n -r run_number"
      << "\n    run number ID that can be used when generating large samples "
         "in small "
      << "\n    jobs. Must be an integer. When given nuwro_nuisance will "
         "update the "
      << "\n    output file from 'output.root' to 'output.root.run_number.root'"
      << "\n"
      << "\n -o output_file"
      << "\n    Path to the output_file you want to save events to."
      << "\n"
      << "\n    If this is not given but '-r' is then events will be saved to "
      << "\n    the file 'nuwrogen.run_number.events.root'"
      << "\n"
      << "\n    If a run number is given alongside '-o' then events will be "
         "saved "
      << "\n    to 'output.root.run_number.root'"
      << "\n"
      << "\n --cross-section /path/to/params.txt"
      << "\n    Path to the nuwro model definition. If this is not given, then "
         "this "
      << "\n    will default to $NUISANCE/data/nuwro/Default_params.txt"
      << "\n"
      << "\n    Look in $NUISANCE/data/nuwro/Default_params.txt for examples "
         "when "
      << "\n    writing your own card files."
      << "\n"
      << "\n --event-generator-list mode_definition"
      << "\n    Name of modes to run. This sets the dynamic mode settings in "
         "nuwro."
      << "\n    e.g. --event-generator-list Default+MEC"
      << "\n"
      << "\n    Allowed mode_definitions are given at the end of this help "
         "message."
      << "\n"
      << "\n --seed seed_value "
      << "\n    Value to use as the seed. If seed isn't given, time(NULL) is "
         "used."
      << "\n"
      << "\n --test-events ntest "
      << "\n    Sets the number of test events for Nuwro to use. If this "
         "option "
      << "\n    isn't given then we assume 5E6 test events by default."
      << "\n\n";

  std::cout << "-----------------" << std::endl;
  TargetUtils::ListTargetIDs();
  std::cout << "-----------------" << std::endl;
  BeamUtils::ListFluxIDs();
  std::cout << "-----------------" << std::endl;
  LOG(FIT) << "Allowed Mode Definitions:" << std::endl
           << " - Default : Default CC+NC modes, no MEC" << std::endl
           << " - Default+MEC : Default CC+NC modes + 2p2h MEC " << std::endl
           << " - DefaultFree : Default CC+NC modes, no Coherent or MEC "
           << std::endl;
  std::cout << "----------------" << std::endl;

  exit(0);
}
//____________________________________________________________________________
#endif
