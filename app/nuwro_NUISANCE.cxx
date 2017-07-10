#ifdef __NUWRO_ENABLED__
#include "ComparisonRoutines.h"
#include "ParserUtils.h"

// All possible inputs
std::string gOptEnergyDef;
std::vector<double> gOptEnergyRange;
int gOptNumberEvents = -1;
int gOptNumberTestEvents = 5E6;
std::string gOptGeneratorList = "Default";
std::string gOptCrossSections = "Default"; // If default this will look in $NUISANCE/data/nuwro/default_params.txt
int gOptSeed = time(NULL);
std::string gOptTargetDef = "";
std::string gOptFluxDef = "";
std::string gOptOutputFile = "";
int gOptRunNumber = -1;

void GetCommandLineArgs (int argc, char ** argv);
void PrintSyntax        (void);

string          ConvertTargetIDs        (string);
string          ConvertFluxIDs          (string);
void            ListTargetIDs(void);
void            ListFluxIDs(void);

std::string GetDynamicModes(std::string list){

  LOG(FIT) << "Using " << list << " to define interaction modes." << std::endl;
  std::map<std::string, int> modes;

  if (!list.compare("Default")){
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

  } else if (!list.compare("DefaultFree")){
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

  } else if (!list.compare("Default+MEC")){
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
  for(std::map<std::string, int>::iterator iter = modes.begin(); 
      iter != modes.end(); iter++){
    std::cout << " -> " << iter->first << " : " << iter->second << std::endl;
    modestring += " -p  \"" + iter->first + "=" + GeneralUtils::IntToStr(iter->second) + "\"";
  }
  return modestring;
}

std::string GetFluxDefinition(std::string flux, std::string out){
  LOG(FIT) << "Using " << flux << " to define NuWro beam." << std::endl;

  // By default the flux is type 6 with a root file
  std::vector<std::string> fluxargs = GeneralUtils::ParseToStr(flux,",");
  if (fluxargs.size() < 2){
    THROW("Expected flux in the format: file.root,hist_name1[pdg1],... : reveived : " << flux);
  }

  // Build Map
  std::map<std::string, std::string> fluxmap;
  fluxmap["beam_type"] = "6";
  fluxmap["beam_inputroot"] = fluxargs[0];
  fluxmap["beam_inputroot_nue"]    = "";
  fluxmap["beam_inputroot_nueb"]   = "";
  fluxmap["beam_inputroot_numu"]   = "";
  fluxmap["beam_inputroot_numub"]  = "";
  fluxmap["beam_inputroot_nutau"]  = "";
  fluxmap["beam_inputroot_nutaub"] = "";

  // Split by beam bdgs
  for (int i = 1; i < fluxargs.size(); i++){
    std::string histdef = fluxargs[i];
    string::size_type open_bracket  = histdef.find("[");
    string::size_type close_bracket = histdef.find("]");
    string::size_type ibeg = 0;
    string::size_type iend = open_bracket;
    string::size_type jbeg = open_bracket+1;
    string::size_type jend = close_bracket-1;
    std::string name = std::string(histdef.substr(ibeg,iend).c_str());
    int pdg = atoi(histdef.substr(jbeg,jend).c_str());

    if      (pdg == 12) fluxmap["beam_inputroot_nue"]    = name;
    else if (pdg ==-12) fluxmap["beam_inputroot_nueb"]   = name;
    else if (pdg == 14) fluxmap["beam_inputroot_numu"]   = name;
    else if (pdg ==-14) fluxmap["beam_inputroot_numub"]  = name;
    else if (pdg == 16) fluxmap["beam_inputroot_nutau"]  = name;
    else if (pdg ==-16) fluxmap["beam_inputroot_nutaub"] = name;
  }
  
  // Now create a new flux file matching the output file
  std::cout << " -> Moving flux from '" + fluxmap["beam_inputroot"] + "' to current directory to keep everything organised." << std::endl;
  TFile* fluxread = new TFile(fluxmap["beam_inputroot"].c_str(),"READ");
  TFile* fluxwrite = new TFile((out + ".flux.root").c_str(),"RECREATE");

  for(std::map<std::string, std::string>::iterator iter = fluxmap.begin();
      iter != fluxmap.end(); iter++){
    TH1* temp = (TH1*)fluxread->Get(iter->second.c_str());
    if (!temp) continue;
    TH1D* cuthist = (TH1D*)temp->Clone();

    // Restrict energy range if required
    if (gOptEnergyRange.size() == 2){
      for (int i = 0; i < cuthist->GetNbinsX(); i++){
	if (cuthist->GetXaxis()->GetBinCenter(i+1) < gOptEnergyRange[0] or 
	    cuthist->GetXaxis()->GetBinCenter(i+1) > gOptEnergyRange[1]){
	  cuthist->SetBinContent(i+1, 0.0);
	}
      }
    }

    // Check Flux
    if (cuthist->Integral() <= 0.0){
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
  for(std::map<std::string, std::string>::iterator iter = fluxmap.begin();
      iter != fluxmap.end(); iter++){
    std::cout << " -> " << iter->first << " : " << iter->second << std::endl;
    fluxstring += " -p  \"" + iter->first + "=" + iter->second + "\"";
  }
  return fluxstring;
}

std::string GetTargetDefinition(std::string target){

  LOG(FIT) << "Defining NuWro Target from : " << target << std::endl;

  // Target is given as either a single PDG, or a combo with the total number of nucleons
  std::vector<std::string> trgts = GeneralUtils::ParseToStr(target,",");
  std::string targetstring = "";

  // Single Target
  if (trgts.size() == 1){

    int PDG = GeneralUtils::StrToInt(trgts[0]);
    int Z = TargetUtils::GetTargetZFromPDG(PDG);
    int N = TargetUtils::GetTargetAFromPDG(PDG) - Z;
    int TOTAL = 1;
    targetstring += (" -p \"target_content=" 
		     + GeneralUtils::IntToStr(Z) + " "
		     + GeneralUtils::IntToStr(N) + " "
		     + GeneralUtils::IntToStr(TOTAL) + "x"
		     + "\"");

  // Combined target
  } else if (trgts.size() > 1){
    
    int NUCLEONS = GeneralUtils::StrToInt(trgts[0]);

    // Loop over all targets
    for (size_t i = 1; i < trgts.size(); i++){

      // Extra PDG and W
      std::string tgtdef = trgts[i];
      string::size_type open_bracket  = tgtdef.find("[");
      string::size_type close_bracket = tgtdef.find("]");
      string::size_type ibeg = 0;
      string::size_type iend = open_bracket;
      string::size_type jbeg = open_bracket+1;
      string::size_type jend = close_bracket-1;
      int    PDG = atoi(tgtdef.substr(ibeg,iend).c_str());
      double W   = atof(tgtdef.substr(jbeg,jend).c_str());

      // extract Z N
      int Z = TargetUtils::GetTargetZFromPDG(PDG);
      int N = TargetUtils::GetTargetAFromPDG(PDG) - Z;
      std::cout << "Target " << PDG << " Z" << Z << " N" << N << std::endl;

      // extract weight
      int TOTAL = int(double(NUCLEONS)*W);

      if (i == 1){
	targetstring += (" -p \"target_content="
			 + GeneralUtils::IntToStr(Z) + " "
			 + GeneralUtils::IntToStr(N) + " "
			 + GeneralUtils::IntToStr(TOTAL) + "x"
			 + "\"");
      } else {
	targetstring += (" -p \"target_content+="
			 + GeneralUtils::IntToStr(Z) + " "
			 + GeneralUtils::IntToStr(N) + " "
			 + GeneralUtils::IntToStr(TOTAL) + "x"
			 + "\"");
      }      
    }

  // No target given!
  } else {
    THROW("No target given : " << target);
  }

  std::cout << " -> " << targetstring << std::endl;
  return targetstring;
}

std::string GetEventAndSeedDefinition(int nevents, int ntestevents, int seed){
  
  std::string eventdef = "";
  eventdef += " -p \"number_of_events=" + GeneralUtils::IntToStr(nevents) + "\"";
  eventdef += " -p \"number_of_test_events=" + GeneralUtils::IntToStr(ntestevents) + "\"";
  eventdef += " -p \"random_seed=" + GeneralUtils::IntToStr(seed) + "\"";

  LOG(FIT) << "Event Definition: " << std::endl;
  std::cout << " -> number_of_events      : " << nevents << std::endl;
  std::cout << " -> number_of_test_events : " << ntestevents << std::endl;
  std::cout << " -> seed    : " << seed << std::endl;

  return eventdef;
}




//____________________________________________________________________________
int main(int argc, char ** argv)
{
  LOG(FIT) << "==== RUNNING nuwro_nuisance Event Generator =====" << std::endl;
  GetCommandLineArgs(argc,argv);
  
  // Calculate the dynamic modes definition
  std::string dynparamsdef = GetDynamicModes(gOptGeneratorList);

  // Get Flux and Target definition
  std::string fluxparamsdef = GetFluxDefinition(gOptFluxDef, gOptOutputFile);
  std::string targetparamsdef = GetTargetDefinition(gOptTargetDef);

  // Get Run Definition
  std::string eventparamsdef = GetEventAndSeedDefinition(gOptNumberEvents,
							 gOptNumberTestEvents,
							 gOptSeed);

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

///____________________________________________________________________________
void ListTargetIDs(){

  // Keep in sync with ConvertTargetIDs
  LOG(FIT) << "Possible Target IDs: \n"
			 << "\n H  : " << ConvertTargetIDs("H")
                         << "\n C  : " << ConvertTargetIDs("C")
			 << "\n CH  : " << ConvertTargetIDs("CH")
			 << "\n CH2 : " << ConvertTargetIDs("CH2")
			 << "\n H2O : " << ConvertTargetIDs("H2O")
			 << "\n Fe  : " << ConvertTargetIDs("Fe")
			 << "\n Pb  : " << ConvertTargetIDs("Pb")
			 << "\n D2  : " << ConvertTargetIDs("D2")
			 << "\n D2-free : " << ConvertTargetIDs("D2-free");
}


//____________________________________________________________________________
string ConvertTargetIDs(string id){

  if (!id.compare("H")) return "1000010010";
  else if  (!id.compare("C")) return "1000060120";
  else if  (!id.compare("CH"))  return "13,1000060120[0.9231],1000010010[0.0769]";
  else if  (!id.compare("CH2")) return "14,1000060120[0.8571],1000010010[0.1429]";
  else if  (!id.compare("H2O")) return "18,1000080160[0.8888],1000010010[0.1111]";
  else if  (!id.compare("Fe"))  return "1000260560";
  else if  (!id.compare("Pb"))  return "1000822070";
  else if  (!id.compare("D2"))  return "1000010020";
  else if  (!id.compare("D2-free")) return "2,1000010010[0.5],1000000010[0.5]";
  else return "";

};

///____________________________________________________________________________
void ListFluxIDs(){

  // Keep in sync with ConvertTargetIDs
  LOG(FIT) << "Possible Flux IDs: \n"
                         << "\n MINERvA_fhc_numu  : " << ConvertFluxIDs("MINERvA_fhc_numu")
			 << "\n MINERvA_fhc_numunumubar  : " << ConvertFluxIDs("MINERvA_fhc_numunumubar")
                         << "\n MINERvA_fhc_nue  : " << ConvertFluxIDs("MINERvA_fhc_nue")
                         << "\n MINERvA_fhc_nuenuebar  : " << ConvertFluxIDs("MINERvA_fhc_nuenuebar")
			 << "\n MINERvA_fhc_all  : " << ConvertFluxIDs("MINERvA_fhc_all")

			 << "\n MINERvA_rhc_numubar  : " << ConvertFluxIDs("MINERvA_rhc_numubar")
			 << "\n MINERvA_rhc_numubarnumu  : " << ConvertFluxIDs("MINERvA_rhc_numubarnumu")
			 << "\n MINERvA_rhc_nuebar  : " << ConvertFluxIDs("MINERvA_rhc_nuebar")
			 << "\n MINERvA_rhc_nuebarnue  : " << ConvertFluxIDs("MINERvA_rhc_nuebarnue")
			 << "\n MINERvA_rhc_all  : " << ConvertFluxIDs("MINERvA_rhc_all")

			 << "\n ANL_fhc_numu : " << ConvertFluxIDs("ANL_fhc_numu")
			 << "\n BNL_fhc_numu : " << ConvertFluxIDs("BNL_fhc_numu")
			 << "\n BNL_fhc_numu_ALT1986 : " << ConvertFluxIDs("BNL_fhc_numu_ALT1986")
			 << "\n BNL_fhc_numu_ALT1981 : " << ConvertFluxIDs("BNL_fhc_numu_ALT1981")
			 << "\n BEBC_fhc_numu : " << ConvertFluxIDs("BEBC_fhc_numu")
			 << "\n FNAL_fhc_numu : " << ConvertFluxIDs("FNAL_fhc_numu")
			 << "\n FNAL_rhc_numub : " << ConvertFluxIDs("FNAL_rhc_numub")
			 << "\n GGM_fhc_numu : " << ConvertFluxIDs("GGM_fhc_numu");
    
}


//____________________________________________________________________________
string ConvertFluxIDs(string id){

  char * const var = getenv("NUISANCE");
  if (!var) {
    std::cout << "Cannot find top level directory! Set the NUISANCE environmental variable" << std::endl;
    exit(-1);
  }
  string topnuisancedir = string(var);
  string fluxfolder = topnuisancedir + "/data/flux/";
  string inputs = "";

  if (!id.compare("MINERvA_fhc_numu")) inputs="minerva_flux.root,numu_fhc[14]";
  else if (!id.compare("MINERvA_fhc_numunumubar")) inputs="minerva_flux.root,numu_fhc[14],numubar_fhc[-14]";
  else if (!id.compare("MINERvA_fhc_numu")) inputs="minerva_flux.root,nue_fhc[12]";
  else if (!id.compare("MINERvA_fhc_nuenuebar")) inputs="minerva_flux.root,nue_fhc[12],nuebar_fhc[-12]";
  else if (!id.compare("MINERvA_fhc_all")) inputs="minerva_flux.root,numu_fhc[14],numubar_fhc[-14],nue_fhc[12],nuebar_fhc[-12]";

  else if (!id.compare("MINERvA_rhc_numubar")) inputs="minerva_flux.root,numubar_rhc[-14]";
  else if (!id.compare("MINERvA_rhc_numubarnumu")) inputs="minerva_flux.root,numubar_rhc[-14],numu_rhc[14]";
  else if (!id.compare("MINERvA_rhc_nuebar")) inputs="minerva_flux.root,nuebar_rhc[-12]";
  else if (!id.compare("MINERvA_rhc_nuebarnue")) inputs="minerva_flux.root,nuebar_rhc[-12],nue_rhc[12]";
  else if (!id.compare("MINERvA_rhc_all")) inputs="minerva_flux.root,numu_rhc[14],numubar_rhc[-14],nue_rhc[12],nuebar_rhc[-12]";

  else if (!id.compare("ANL_fhc_numu"))         inputs="ANL_1977_2horn_rescan.root,numu_flux[14]";
  else if (!id.compare("BNL_fhc_numu"))         inputs="BNL_NuInt02_rescan.root,numu_flux[14]";
  else if (!id.compare("BNL_fhc_numu_ALT1986")) inputs="BNL_1986_flux-ALTERNATIVE.root,numu_flux[14]";
  else if (!id.compare("BNL_fhc_numu_ALT1981")) inputs="BNL_CCQE_1981_rescan-ALTERNATIVE.root,numu_flux[14]";

  else if (!id.compare("BEBC_fhc_numu"))   inputs="BEBC_Wachsmuth_numubar_table.root,numu_flux[14]";
  else if (!id.compare("FNAL_fhc_numu"))   inputs="FNAL_CCinc_1982_nu_MCadj.root,numu_flux[14]";
  else if (!id.compare("FNAL_rhc_numub"))  inputs="FNAL_coh_1993_anu.root,numu_flux[-14]";
  else if (!id.compare("GGM_fhc_numu"))    inputs="GGM_nu_flux_1979_rescan.root,numu_flux[14]";
  else return "";

  return fluxfolder + inputs;

};

//____________________________________________________________________________
void GetCommandLineArgs(int argc, char ** argv)
{

  // Check for -h flag.
  for (int i = 0; i < argc; i++){
    if (!std::string(argv[i]).compare("-h")) PrintSyntax();
  }

  // Format is nuwro -r run_number -n n events 
  std::vector<std::string> args = GeneralUtils::LoadCharToVectStr(argc, argv);
  ParserUtils::ParseArgument(args, "-n", gOptNumberEvents, false);
  if (gOptNumberEvents == -1){
    THROW( "No event count passed to nuwro_NUISANCE!");
  }

  // Flux/Energy Specs
  ParserUtils::ParseArgument(args, "-e", gOptEnergyDef, false);
  gOptEnergyRange = GeneralUtils::ParseToDbl(gOptEnergyDef,",");

  ParserUtils::ParseArgument(args, "-f", gOptFluxDef, false);
  if (gOptFluxDef.empty() and gOptEnergyRange.size() < 1){
    THROW("No flux or energy range given to nuwro_nuisance!");

  } else if (gOptFluxDef.empty() and gOptEnergyRange.size() == 1){
    // Fixed energy, make sure -p is given
    THROW("nuwro_NUISANCE cannot yet do fixed energy!");
  
  } else if (gOptFluxDef.empty() and gOptEnergyRange.size() == 2){
    // Uniform energy range
    THROW("nuwro_NUISANCE cannot yet do a uniform energy range!");

  } else if (!gOptFluxDef.empty()){
    // Try to convert the flux definition if possible.
    std::string convflux = ConvertFluxIDs(gOptFluxDef);
    if (!convflux.empty()) gOptFluxDef = convflux;

  } else {
    THROW("Unknown flux energy range combination!");
  }

  ParserUtils::ParseArgument(args, "-t", gOptTargetDef, false);
  if (gOptTargetDef.empty()){
    THROW("No Target passed to nuwro_nuisance! use the '-t' argument.");
  } else {
    std::string convtarget = ConvertTargetIDs(gOptTargetDef);
    if (!convtarget.empty()) gOptTargetDef = convtarget;
  }

  ParserUtils::ParseArgument(args, "-r", gOptRunNumber, false);
  ParserUtils::ParseArgument(args, "-o", gOptOutputFile, false);
  if (gOptOutputFile.empty()){
    if (gOptRunNumber == -1) gOptRunNumber = 1;
    LOG(FIT) << "No output file given! Saving file to : nuwrogen." << gOptRunNumber << ".event.root" << std::endl;
    gOptOutputFile = "nuwrogen." + GeneralUtils::IntToStr(gOptRunNumber) + ".event.root";
  } else {
    // if no run number given leave as is, else add run number.
    if (gOptRunNumber != -1){
      gOptOutputFile += "." + GeneralUtils::IntToStr(gOptRunNumber) + ".root";
    } else {
      gOptRunNumber = 0;
    }
  }

  ParserUtils::ParseArgument(args, "--cross-section", gOptCrossSections, false);
  if (!gOptCrossSections.compare("Default")){
    LOG(FIT) << "No Parameters File passed. Using default NuWro one." << std::endl;
    char * const var = getenv("NUISANCE");
    if (!var) {
      std::cout << "Cannot find top level directory! Set the NUISANCE environmental variable" << std::endl;
      exit(-1);
    }  
    std::string topnuisancedir = string(var);
    gOptCrossSections = topnuisancedir + "/nuwro/default_params.txt";
  }
  
  ParserUtils::ParseArgument(args, "--event-generator-list", gOptGeneratorList, false);
  ParserUtils::ParseArgument(args, "--seed", gOptSeed, false);
  ParserUtils::ParseArgument(args, "--test-events", gOptNumberTestEvents, false);

  // Final Check and output
  if (args.size() > 0){
    PrintSyntax();
    ParserUtils::CheckBadArguments(args);
  }

  LOG(FIT) << "Generating NuWro Events with the following properties:"  << std::endl
	   << " -> Energy      : " << gOptEnergyDef << " (" << gOptEnergyRange.size() << ")" << std::endl
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
void PrintSyntax(void)
{
  LOG(FIT) 
    << "\n\n" << "Syntax:" << "\n"
    << "\n      gevgen [-h]"
    << "\n              [-r run#]"
    << "\n               -n nev"
    << "\n               -e energy (or energy range) "
    << "\n               -p neutrino_pdg"
    << "\n               -t target_pdg "
    << "\n              [-f flux_description]"
    << "\n              [-w]"
    << "\n              [--seed random_number_seed]"
    << "\n              [--cross-sections xml_file]"
    << "\n              [--event-generator-list list_name]"
    << "\n              [--message-thresholds xml_file]"
    << "\n              [--unphysical-event-mask mask]"
    << "\n              [--event-record-print-level level]"
    << "\n              [--mc-job-status-refresh-rate  rate]"
    << "\n              [--cache-file root_file]"
    << "\n\n" ;
  ListTargetIDs();
  ListFluxIDs();
  exit(0);
}
//____________________________________________________________________________
#endif
