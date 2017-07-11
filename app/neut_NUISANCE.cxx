#ifdef __NEUT_ENABLED__
#include "ComparisonRoutines.h"
#include "ParserUtils.h"

#ifdef WINDOWS
#include <direct.h>
    #define GetCurrentDir _getcwd
#else
#include <unistd.h>
    #define GetCurrentDir getcwd
#endif

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

std::string GETCWD(){
  char cCurrentPath[FILENAME_MAX];
  if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))){
    THROW("CANT FIND CURRENT DIRECTORY!");
  }
  std::string curdir = std::string(cCurrentPath);
  return curdir;
}

std::string ExpandPath(std::string name){

  // Get Current
  char cCurrentPath[FILENAME_MAX];
  if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))){
    THROW("CANT FIND CURRENT DIRECTORY!");
  }
  std::string curdir = std::string(cCurrentPath);


  // If first entry is not / then add the current working directory
  if (!name.empty() and name.at(0) != '/'){
    name = curdir + "/" + name;
  }
  return name;
}

std::string GetBaseName(std::string name){
  std::vector<std::string> splitfile = GeneralUtils::ParseToStr(name,"/");
  std::string filename = "";
  if (splitfile.size() == 1){
    filename = splitfile[0];
  } else if (splitfile.size() > 1){
    filename = splitfile[splitfile.size()-1];
  } else {
    THROW("Cannot split filename: " << name);
  }
  return filename;
}

std::string GetDynamicModes(std::string list, bool neutrino){
  LOG(FIT) << "Using " << list << " to define interaction modes for Neutrino=" << neutrino << std::endl;

  std::map<std::string, int> modes;
  std::vector<std::string> ids;
  
  // Create vector of ids for the print out and future reference
  /*
    C
C  nu                     nub
C  1:     CC Q.E.         CC Q.E.( Free )
C  2-4:   CC 1pi          CC 1pi 
C  5:     CC DIS 1320     CC DIS 1.3 < W < 2.0 
C  6-9:   NC 1pi          NC 1pi 
C  10:    NC DIS 1320     NC DIS 1.3 < W < 2.0 
C  11:    NC els          CC Q.E.( Bound )
C  12:    NC els          NC els 
C  13:    NC els          NC els 
C  14:    coherent        NC els 
C  15:    coherent        coherent
C  16:    CC eta          coherent
C  17     NC eta          CC eta 
C  18:    NC eta          NC eta 
C  19:    CC K            NC eta 
C  20     NC K            CC K
C  21:    NC K            NC K
C  22:    N/A             NC K
C  23:    CC DIS          CC DIS (W > 2.0)
C  24:    NC DIS          NC DIS (W > 2.0)
C  25:    CC 1 gamma      CC 1 gamma
C  26:    NC 1 gamma      NC 1 gamma
C  27:    NC 1 gamma      NC 1 gamma
C  28:    2p2h            2p2h

   */
  ids.push_back("crsmode_CCQE" ); 
  ids.push_back("crsmode_CC2P2H" );
  ids.push_back("crsmode_CC1pi");
  ids.push_back("crsmode_CCDIS_lowW" );
  ids.push_back("crsmode_NC1pi");
  ids.push_back("crsmode_NCDIS_lowW" );
  ids.push_back("crsmode_NCEL");
  ids.push_back("crsmode_CCCOH");
  ids.push_back("crsmode_NCCOH");
  ids.push_back("crsmode_CCETA");
  ids.push_back("crsmode_NCETA");
  ids.push_back("crsmode_CCKAON");
  ids.push_back("crsmode_NCKAON");
  ids.push_back("crsmode_CCDIS_highW");
  ids.push_back("crsmode_NCDIS_highW");
  ids.push_back("crsmode_CCGAMMA");
  ids.push_back("crsmode_NCGAMMA");
  
  // Now define possible models
  if (!list.compare("Default")){ // Everything but MEC

    modes["crsmode_CCQE"] = 1;
    modes["crsmode_CC2P2H"] = 0;
    modes["crsmode_CC1pi"] = 1;
    modes["crsmode_CCDIS_lowW"] = 1;
    modes["crsmode_CCCOH"] = 1;
    modes["crsmode_CCETA"] = 1;
    modes["crsmode_CCKAON"] = 1;
    modes["crsmode_CCDIS_highW"] = 1;
    modes["crsmode_CCGAMMA"] = 1;
    modes["crsmode_NC1pi"] = 1;
    modes["crsmode_NCDIS_lowW"] = 1;
    modes["crsmode_NCEL"] = 1;
    modes["crsmode_NCCOH"] = 1;
    modes["crsmode_NCETA"] = 1;
    modes["crsmode_NCKAON"] = 1;
    modes["crsmode_NCDIS_highW"] = 1;
    modes["crsmode_NCGAMMA"] = 1;

  } else if (!list.compare("Default+MEC")){

    modes["crsmode_CCQE"] = 1;
    modes["crsmode_CC2P2H"] = 1;
    modes["crsmode_CC1pi"] = 1;
    modes["crsmode_CCDIS_lowW"] = 1;
    modes["crsmode_CCCOH"] = 1;
    modes["crsmode_CCETA"] = 1;
    modes["crsmode_CCKAON"] = 1;
    modes["crsmode_CCDIS_highW"] = 1;
    modes["crsmode_CCGAMMA"] = 1;
    modes["crsmode_NC1pi"] = 1;
    modes["crsmode_NCDIS_lowW"] = 1;
    modes["crsmode_NCEL"] = 1;
    modes["crsmode_NCCOH"] = 1;
    modes["crsmode_NCETA"] = 1;
    modes["crsmode_NCKAON"] = 1;
    modes["crsmode_NCDIS_highW"] = 1;
    modes["crsmode_NCGAMMA"] = 1;

  } else {
    THROW("Event generator list " << list << " not found!");
  }


  // Now we actually have to make the conversion because NEUTS modes organisation are a mess.
  /*
    C
C  nu                     nub
C  1:     CC Q.E.         CC Q.E.( Free )
C  2-4:   CC 1pi          CC 1pi 
C  5:     CC DIS 1320     CC DIS 1.3 < W < 2.0 
C  6-9:   NC 1pi          NC 1pi 
C  10:    NC DIS 1320     NC DIS 1.3 < W < 2.0 
C  11:    NC els          CC Q.E.( Bound )
C  12:    NC els          NC els 
C  13:    NC els          NC els 
C  14:    coherent        NC els 
C  15:    coherent        coherent
C  16:    CC eta          coherent
C  17     NC eta          CC eta 
C  18:    NC eta          NC eta 
C  19:    CC K            NC eta 
C  20     NC K            CC K
C  21:    NC K            NC K
C  22:    N/A             NC K
C  23:    CC DIS          CC DIS (W > 2.0)
C  24:    NC DIS          NC DIS (W > 2.0)
C  25:    CC 1 gamma      CC 1 gamma
C  26,27: NC 1 gamma      NC 1 gamma
   */
																													   
  std::string modestring_neutrino = "NEUT-CRS   ";
  std::string modestring_antineutrino = "NEUT-CRSB   ";

  // Neutrino First
  if (neutrino){

    // Fill empty NEUT-CRSB
    for (size_t i = 0; i < 27; i++){
      modestring_antineutrino += " 0";
    }

    modestring_neutrino += (modes["crsmode_CCQE"]?  " 1" : " 0");
    modestring_neutrino += (modes["crsmode_CC1pi"]? " 1 1 1" : " 0 0 0");
    modestring_neutrino += (modes["crsmode_CCDIS_lowW"]?  " 1" : " 0");
    modestring_neutrino += (modes["crsmode_NC1pi"]?  " 1 1 1 1" : " 0 0 0 0");
    modestring_neutrino += (modes["crsmode_NCDIS_lowW"]?  " 1" : " 0");
    modestring_neutrino += (modes["crsmode_NCEL"]? " 1 1 1" : " 0 0 0");
    modestring_neutrino += (modes["crsmode_CCCOH"]? " 1" : " 0");
    modestring_neutrino += (modes["crsmode_NCCOH"]? " 1" : " 0");
    modestring_neutrino += (modes["crsmode_CCETA"]? " 1" : " 0");
    modestring_neutrino += (modes["crsmode_NCETA"]? " 1 1" : " 0 0");
    modestring_neutrino += (modes["crsmode_CCKAON"]? " 1" : " 0");
    modestring_neutrino += (modes["crsmode_NCKAON"]? " 1 1" : " 0 0");
    modestring_neutrino += " 1"; // /NA
    modestring_neutrino += (modes["crsmode_CCDIS_highW"]? " 1" : " 0");
    modestring_neutrino += (modes["crsmode_NCDIS_highW"]? " 1" : " 0");
    modestring_neutrino += (modes["crsmode_CCGAMMA"]? " 1" : " 0");
    modestring_neutrino += (modes["crsmode_NCGAMMA"]? " 1" : " 0");
    modestring_neutrino += (modes["crsmode_CC2P2H"]? " 1" : " 0");

  } else {
    
    // Fill Empty NEUT CRS
    for (size_t i = 0; i < 27; i++){
      modestring_neutrino += " 0";
    }

    modestring_antineutrino += (modes["crsmode_CCQE"]?  " 1" : " 0");
    modestring_antineutrino += (modes["crsmode_CC1pi"]? " 1 1 1" : " 0 0 0");
    modestring_antineutrino += (modes["crsmode_CCDIS_lowW"]?  " 1" : " 0");
    modestring_antineutrino += (modes["crsmode_NC1pi"]?  " 1 1 1 1" : " 0 0 0 0");
    modestring_antineutrino += (modes["crsmode_NCDIS_lowW"]?  " 1" : " 0");
    modestring_antineutrino += (modes["crsmode_CCQE"]?  " 1" : " 0");
    modestring_antineutrino += (modes["crsmode_NCEL"]? " 1 1 1" : " 0 0 0");
    modestring_antineutrino += (modes["crsmode_CCCOH"]? " 1" : " 0");
    modestring_antineutrino += (modes["crsmode_NCCOH"]? " 1" : " 0");
    modestring_antineutrino += (modes["crsmode_CCETA"]? " 1" : " 0");
    modestring_antineutrino += (modes["crsmode_NCETA"]? " 1 1" : " 0 0");
    modestring_antineutrino += (modes["crsmode_CCKAON"]? " 1" : " 0");
    modestring_antineutrino += (modes["crsmode_NCKAON"]? " 1 1" : " 0 0");
    modestring_antineutrino += (modes["crsmode_CCDIS_highW"]? " 1" : " 0");
    modestring_antineutrino+= (modes["crsmode_NCDIS_highW"]? " 1" : " 0");
    modestring_antineutrino+= (modes["crsmode_CCGAMMA"]? " 1" : " 0");
    modestring_antineutrino+= (modes["crsmode_NCGAMMA"]? " 1" : " 0");
    modestring_antineutrino+= (modes["crsmode_CC2P2H"]? " 1" : " 0");

  }

  return "NEUT-MODE -1 \n" + modestring_neutrino + "\n" + modestring_antineutrino;
}

std::map<std::string, std::string> MakeNewFluxFile(std::string flux, std::string out){
  LOG(FIT) << "Using " << flux << " to define NEUT beam." << std::endl;

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
    else if (pdg == 16) fluxmap["beam_inputroot_tau"]    = name;
    else if (pdg ==-16) fluxmap["beam_inputroot_taub"]   = name;
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

  return fluxmap;
}

std::string GetFluxDefinition( std::string fluxfile, std::string fluxhist, std::string fluxid ){

  // Get base name of flux file as its being copied to NEUT Run Directory
  std::vector<std::string> splitfluxfile = GeneralUtils::ParseToStr(fluxfile,"/");
  std::string filename = "";
  if (splitfluxfile.size() == 1){
    filename = splitfluxfile[0];
  } else if (splitfluxfile.size() > 1){
    filename = splitfluxfile[splitfluxfile.size()-1];
  } else {
    THROW("NO FILENAME FOR FLUX DEFINITION FOUND!");
  }
  
  // Build string
  std::string fluxparams = "";
  fluxparams += "EVCT-FILENM \'" + filename + "\' \n";
  fluxparams += "EVCT-HISTNM \'" + fluxhist + "\' \n";
  fluxparams += "EVCT-INMEV 0 \n";
  fluxparams += "EVCT-MPV 3 \n";

  // Set PDG Code
  if (!fluxid.compare("nue"))         fluxparams += "EVCT-IDPT   12";
  else if (!fluxid.compare("nueb"))   fluxparams += "EVCT-IDPT  -12";
  else if (!fluxid.compare("numu"))   fluxparams += "EVCT-IDPT   14";
  else if (!fluxid.compare("numub"))  fluxparams += "EVCT-IDPT  -14";
  else if (!fluxid.compare("nutau"))  fluxparams += "EVCT-IDPT   16";
  else if (!fluxid.compare("nutaub")) fluxparams += "EVCT-IDPT  -16";
  else {
    THROW("UNKNOWN FLUX ID GIVEN!");
  }

  return fluxparams;
}

std::string GetTargetDefinition(std::string target){

  LOG(FIT) << "Defining NuWro Target from : " << target << std::endl;

  // Target is given as either a single PDG, or a combo with the total number of nucleons
  std::vector<std::string> trgts = GeneralUtils::ParseToStr(target,",");
  std::string targetstring = "";

  // NEUT only lets us pass C and CH type inputs.

  // Single Target
  if (trgts.size() == 1){

    int PDG = GeneralUtils::StrToInt(trgts[0]);
    int Z = TargetUtils::GetTargetZFromPDG(PDG);
    int N = TargetUtils::GetTargetAFromPDG(PDG) - Z;
    targetstring += "NEUT-NUMBNDP " + GeneralUtils::IntToStr(Z) + "\n";
    targetstring += "NEUT-NUMBNDN " + GeneralUtils::IntToStr(N) + "\n";
    targetstring += "NEUT-NUMFREP 0\n";
    targetstring += "NEUT-NUMATOM " + GeneralUtils::IntToStr(Z+N) + "\n";

  // Combined target
  } else if (trgts.size() == 3){
    
    int NUCLEONS = GeneralUtils::StrToInt(trgts[0]);
    std::string target1 = trgts[1];
    std::string target2 = trgts[2];

    // Parse target strings
    string::size_type open_bracket  = target1.find("[");
    string::size_type close_bracket = target1.find("]");
    string::size_type ibeg = 0;
    string::size_type iend = open_bracket;
    string::size_type jbeg = open_bracket+1;
    string::size_type jend = close_bracket-1;
    int    PDG1 = atoi(target1.substr(ibeg,iend).c_str());
    double W1   = atof(target1.substr(jbeg,jend).c_str());

    open_bracket  = target2.find("[");
    close_bracket = target2.find("]");
    ibeg = 0;
    iend = open_bracket;
    jbeg = open_bracket+1;
    jend = close_bracket-1;
    int    PDG2 = atoi(target2.substr(ibeg,iend).c_str());
    double W2   = atof(target2.substr(jbeg,jend).c_str());
    
    // Can only have H as a secondary target!
    if (PDG1 != 1000010010 && PDG2 != 1000010010){
      THROW("NEUT Doesn't support composite targets apart fromn Target+H. E.g. CH");
    }

    // Switch so H is PDG2 if given
    if (PDG1 == 1000010010 && PDG2 != 1000010010){
      PDG1 = PDG2;
      PDG2 = 1000010010;

      double temp1 = W1;
      W1 = W2;
      W2 = temp1;
    }

    // Now build string
    int Z = TargetUtils::GetTargetZFromPDG(PDG1);
    int N = TargetUtils::GetTargetAFromPDG(PDG1) - Z;
    int NHydrogen = int(W2 * double(NUCLEONS));
    if (double(W2*double(NUCLEONS)) - (double(NHydrogen)) > 0.5){
      NHydrogen++; // awkward rounding bug fix
    }
    targetstring += "NEUT-NUMBNDP " + GeneralUtils::IntToStr(Z)         + "\n";
    targetstring += "NEUT-NUMBNDN " + GeneralUtils::IntToStr(N)         + "\n";
    targetstring += "NEUT-NUMFREP " + GeneralUtils::IntToStr(NHydrogen) + "\n";
    targetstring += "NEUT-NUMATOM " + GeneralUtils::IntToStr(Z+N)       + "\n";

  } else {
    THROW("NEUT only supports single targets or ones with a secondary H!");
  }

  return targetstring;
}

std::string GetEventAndSeedDefinition(int nevents, int seed){
  
  std::string eventdef = "";
  eventdef += "EVCT-NEVT " + GeneralUtils::IntToStr(nevents)  + "\n";

  LOG(FIT) << "Event Definition: " << std::endl;
  std::cout << " -> EVCT-NEVT  : " << nevents << std::endl;

  return eventdef;
}




//____________________________________________________________________________
int main(int argc, char ** argv)
{
  LOG(FIT) << "==== RUNNING nuwro_nuisance Event Generator =====" << std::endl;
  GetCommandLineArgs(argc,argv);
  std::string neutroot = std::string(getenv("NEUT_ROOT")) + "/src/neutsmpl/";

  // Calculate the dynamic modes definition
  bool neutrino = true;
  std::string dynparamsdef = GetDynamicModes(gOptGeneratorList, neutrino);

  // Read Target string
  std::string targetparamsdef = GetTargetDefinition(gOptTargetDef);



  //____________________________

  // NEUT doesn't let us do combined flux inputs so have to loop over each flux.
  std::map<std::string,std::string> newfluxdef = MakeNewFluxFile(gOptFluxDef,gOptOutputFile);
  
  // Copy this file to the NEUT working directory
  LOG(FIT) << "Copying flux to NEUT working directory" << std::endl;
  system(("cp -v " + newfluxdef["beam_inputroot"] + " " + neutroot + "/").c_str());
  TFile* fluxrootfile = new TFile( newfluxdef["beam_inputroot"].c_str(), "READ");

  // Setup possible beams and get relative fractions
  std::vector<std::string> possiblefluxids;
  std::vector<double> fluxfractions;
  possiblefluxids.push_back("nue");
  possiblefluxids.push_back("nueb");
  possiblefluxids.push_back("numu");
  possiblefluxids.push_back("numub");
  possiblefluxids.push_back("tau");
  possiblefluxids.push_back("taub");

  // Total up integrals
  double totintflux = 0.0;
  for (size_t i = 0; i < possiblefluxids.size(); i++){    
    if (newfluxdef["beam_inputroot_" + possiblefluxids[i]].empty()){
      fluxfractions.push_back(0.0);
    } else {

      TH1D* fluxhist = (TH1D*) fluxrootfile->Get( newfluxdef["beam_inputroot_" + possiblefluxids[i]].c_str() );
      if (!fluxhist){
	THROW("FLUX HIST : " << newfluxdef["beam_inputroot_" + possiblefluxids[i]] << " not found!");
      }
      fluxfractions.push_back( fluxhist->Integral() );
      totintflux += fluxhist->Integral();
    }
  }
  fluxrootfile->Close();

  
  // Now loop over and actually generate jobs!
  for (size_t i = 0; i < possiblefluxids.size(); i++){
    if (fluxfractions[i] == 0.0) continue;

    // Get number of events for this subbeam

    int nevents = int( double(gOptNumberEvents) * fluxfractions[i] / totintflux );
    std::cout << "NEVENTS = " << gOptNumberEvents << " " << fluxfractions[i] <<" " << totintflux << " " << nevents << std::endl;
    std::string eventparamsdef = GetEventAndSeedDefinition(nevents,
							   gOptSeed);

    std::string fluxparamsdef = GetFluxDefinition( newfluxdef["beam_inputroot"],
						   newfluxdef["beam_inputroot_" + possiblefluxids[i]],
						   possiblefluxids[i] );
						   

    LOG(FIT) << "==== Generating CardFiles NEUT! ===" << std::endl;
    std::cout << dynparamsdef << std::endl;
    std::cout << targetparamsdef << std::endl;
    std::cout << eventparamsdef << std::endl;
    std::cout << fluxparamsdef << std::endl;

    // Create card file
    std::ifstream incardfile;
    std::ofstream outcardfile;
    std::string line;
    incardfile.open( gOptCrossSections.c_str(), ios::in );
    outcardfile.open( (gOptOutputFile + "." + possiblefluxids[i] + ".par").c_str(), ios::out );

    // Copy base card file
    if (incardfile.is_open()){
      while( getline (incardfile, line) ){
	outcardfile << line << '\n';
      }
    } else {
      THROW( "Cannot find card file : " << gOptCrossSections );
    }

    // Now copy our strings
    outcardfile << eventparamsdef<< '\n';
    outcardfile << dynparamsdef << '\n';
    outcardfile << targetparamsdef<< '\n';
    outcardfile << fluxparamsdef<< '\n';

    // Close card and keep name for future use.
    outcardfile.close();
  
  }

  LOG(FIT) << "GENERATING" << std::endl;
  for (size_t i = 0; i < possiblefluxids.size(); i++){
    if (fluxfractions[i] == 0.0) continue;
    int nevents = int( double(gOptNumberEvents) * fluxfractions[i] / totintflux );
    if (nevents <= 0) continue;
    
    std::string cardfile   =  ExpandPath(gOptOutputFile + "." + possiblefluxids[i] + ".par");
    std::string outputfile =  ExpandPath(gOptOutputFile + "." + possiblefluxids[i] + ".root");
    std::string basecardfile = GetBaseName(cardfile);
    std::string baseoutputfile = GetBaseName(outputfile);

    std::cout << "CARDFILE = " << cardfile << " : " << basecardfile << std::endl;
    std::cout << "OUTPUTFILE = " << outputfile << " : " << baseoutputfile << std::endl;

    system(("cp " + cardfile + " " + neutroot).c_str());

    std::string cwd = GETCWD();
    chdir(neutroot.c_str());

    int attempts = 0;
    while(true){

      // Break if too many attempts
      attempts++;
      if (attempts > 20) continue;

      // Actually run neutroot2
      system(("./neutroot2 " + basecardfile + " " + baseoutputfile).c_str());

      // Check the output is valid, sometimes NEUT aborts mid run.
      TFile* f = new TFile(baseoutputfile.c_str(),"READ");
      if (!f or f->IsZombie()) continue;
      
      // Check neutttree is there and filled correctly.
      TTree* tn = (TTree*) f->Get("neuttree");
      if (!tn) continue;
      if (tn->GetEntries() < nevents * 0.9) continue;

      break;
    }

    // Move the finished file back and clean this directory of card files
    system(("echo mv " + baseoutputfile + " " + outputfile).c_str());
    system(("echo rm " + basecardfile).c_str());
    chdir(cwd.c_str());

  }

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
