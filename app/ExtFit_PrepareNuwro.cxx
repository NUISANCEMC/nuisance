#include <stdio.h>
#include <stdlib.h>
#include "event1.h"
#include "params_all.h"
#include "params.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "PlotUtils.h"

void printInputCommands(){ return; };
void CreateRateHistograms(std::string inputs, bool force_out);
void HaddNuwroFiles(std::vector<std::string>& inputs, bool force_out);

//*******************************
int main(int argc, char* argv[]){
//*******************************
  
  // If No Arguments print commands
  if (argc == 1) printInputCommands();
  std::vector<std::string> inputfiles;
  bool force_output = false;
  
  // Get Inputs
  for (int i = 1; i< argc; ++i){
    if (!std::strcmp(argv[i], "-h")) printInputCommands();
    else if (!std::strcmp(argv[i], "-f")) force_output = true;
    else {
      inputfiles.push_back( std::string(argv[i]) );
    }
  }
  
  // If one input file just create flux histograms
  if (inputfiles.size() > (UInt_t)1){
    HaddNuwroFiles(inputfiles, force_output);
  } else if (inputfiles.size() <(UInt_t) 1){
    printInputCommands();
  }

  CreateRateHistograms(inputfiles[0], force_output);
  
  std::cout << "Finished NUWRO Prep." << std::endl;
};

//*******************************
void CreateRateHistograms(std::string inputs, bool force_out){
//*******************************

  // Open root file
  TFile* inRootFile = new TFile(inputs.c_str(), "READ");
  TTree* nuwrotree = (TTree*) inRootFile->Get("treeout");
  
  // Get Flux Histogram
  event* evt = NULL;
  nuwrotree->SetBranchAddress("e",&evt);
  nuwrotree->GetEntry(0);

  int fluxtype = evt->par.beam_type;

  std::cout<<"Nuwro fluxtype = "<<fluxtype<<std::endl;
  if (fluxtype == 0){
    
    std::string fluxstring = evt->par.beam_energy;
    std::vector<double> fluxvals = PlotUtils::FillVectorDFromString(fluxstring, " ");

  } else if (fluxtype == 1){

    std::string fluxstring = evt->par.beam_content;
    cout<<"flux content = "<<fluxstring<<std::endl;

    std::vector<std::string> fluxlines = PlotUtils::FillVectorSFromString(fluxstring, "\n");
    for (int  i = 0; i < fluxlines.size(); i++){
      std::cout<<"Entry "<<i<<" = "<<fluxlines[i]<<std::endl;

      std::vector<double> fluxvals = PlotUtils::FillVectorDFromString(fluxlines[i], " ");

      for (int j = 0; j < fluxvals.size(); j++){
	std::cout<<" fluxVals "<<j<<" = "<<fluxvals[j]<<std::endl;
      }
      
    }
    
  }

    
  

  
  // Read all events and get total xsec histogram
  
}


//*******************************  
void HaddNuwroFiles(std::vector<std::string>& inputs, bool force_out){
//*******************************

  // Get output file name
  std::string outputname = inputs[0];

  // Make command line string
  std::string cmd = "hadd ";
  if (force_out) cmd += "-f ";
  for (UInt_t i = 0; i < inputs.size(); i++){    
    cmd += inputs[i] + " ";
  }
  std::cout<<" Running HADD from ExtFit_PrepareNuwro: "<<cmd<<std::endl;

  // Start HADD
  system(cmd.c_str());
  
  // Return name of output file
  inputs.clear();
  inputs.push_back( outputname );
  return;
}
  

