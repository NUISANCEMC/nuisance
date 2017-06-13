#include <stdio.h>
#include <stdlib.h>
#include "event1.h"
#include "params_all.h"
#include "params.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "PlotUtils.h"
#include "FitLogger.h"

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
  
  LOG(FIT) << "Finished NUWRO Prep." << std::endl;
};

//*******************************
void CreateRateHistograms(std::string inputs, bool force_out){
//*******************************

  // Open root file
  TFile* inRootFile = new TFile(inputs.c_str(), "UPDATE");
  TTree* nuwrotree = (TTree*) inRootFile->Get("treeout");
  
  // Get Flux Histogram
  event* evt = NULL;
  nuwrotree->SetBranchAddress("e",&evt);
  nuwrotree->GetEntry(0);

  int fluxtype = evt->par.beam_type;

  std::map<int, TH1D*> fluxlist;
  std::map<int, TH1D*> eventlist;
  std::vector<int> allpdg;
  std::map<int, int> nevtlist;
  std::map<int, double> intxseclist;

  // Did the input file have a mono-energetic flux?
  bool isMono = false;

  nevtlist[0] = 0.0;
  intxseclist[0] = 0.0;
  
  allpdg.push_back(0);

  LOG(FIT)<<"Nuwro fluxtype = "<<fluxtype<<std::endl;
  if (fluxtype == 0){
    
    std::string fluxstring = evt->par.beam_energy;
    std::vector<double> fluxvals = GeneralUtils::ParseToDbl(fluxstring, " ");

    int pdg = evt->par.beam_particle;
    double Elow  = double(fluxvals[0])/1000.0;
    double Ehigh = double(fluxvals[1])/1000.0;
    TH1D* fluxplot = NULL;
    
    if (Elow > Ehigh) isMono = true;

    // For files produced with a flux distribution
    if (!isMono) {
      
      LOG(FIT) << "Adding new nuwro flux "
	       << "pdg: " << pdg
	       << " Elow: " << Elow
	       << " Ehigh: " << Ehigh
	       << std::endl;
      
      fluxplot = new TH1D("fluxplot","fluxplot", fluxvals.size()-4, Elow, Ehigh);
      for (uint j = 2; j < fluxvals.size(); j++){
	LOG(DEB) << j <<" "<<fluxvals[j]<<endl;
	fluxplot->SetBinContent(j-1, fluxvals[j]);
      }
    } else { // For monoenergetic fluxes
      LOG(FIT) << "Adding mono-energetic nuwro flux "
               << "pdg: " << pdg
               << " E: " << Elow
               << std::endl;

      fluxplot = new TH1D("fluxplot", "fluxplot", 100, 0, Elow*2);
      fluxplot->SetBinContent(fluxplot->FindBin(Elow), 1);
    }
  
    // Setup total flux
    fluxlist[0] = (TH1D*) fluxplot->Clone();
    fluxlist[0]->SetNameTitle("FluxHist",
			      "FluxHist");
    
    // Prep empty total events
    eventlist[0] = (TH1D*) fluxplot->Clone();
    eventlist[0]->SetNameTitle("EvtHist",
			       "EvtHist");
    eventlist[0]->Reset();
    
    fluxplot->SetNameTitle(Form("nuwro_pdg%i_Flux",pdg),
			   Form("nuwro_pdg%i_Flux",pdg));
    
    TH1D* eventplot = (TH1D*) fluxplot->Clone();
    eventplot->SetNameTitle(Form("nuwro_pdg%i_Evt",pdg),
			    Form("nuwro_pdg%i_Evt",pdg));
    eventplot->Reset();
    
    fluxlist[pdg]  = fluxplot;
    eventlist[pdg] = eventplot;
    nevtlist[pdg] = 0;
    intxseclist[pdg] = 0.0;
    allpdg.push_back(pdg);

  } else if (fluxtype == 1){

    std::string fluxstring = evt->par.beam_content;
    
    std::vector<std::string> fluxlines = GeneralUtils::ParseToStr(fluxstring, "\n");
    for (uint  i = 0; i < fluxlines.size(); i++){
      
      std::vector<double> fluxvals = GeneralUtils::ParseToDbl(fluxlines[i], " ");

      int pdg = int(fluxvals[0]);
      double pctg = double(fluxvals[1])/100.0;
      double Elow  = double(fluxvals[2])/1000.0;
      double Ehigh = double(fluxvals[3])/1000.0;

      LOG(FIT) << "Adding new nuwro flux "
	       << "pdg: " << pdg
	       << " pctg: " << pctg
	       << " Elow: " << Elow
	       << " Ehigh: " << Ehigh
	       << std::endl;
      
      TH1D* fluxplot = new TH1D("fluxplot","fluxplot", fluxvals.size()-4, Elow, Ehigh);
      for (uint j = 4; j < fluxvals.size(); j++){
	fluxplot->SetBinContent(j+1, fluxvals[j]);
      }
   
      // Sort total flux plot
      if (!fluxlist[0]){
	
	// Setup total flux
	fluxlist[0] = (TH1D*) fluxplot->Clone();
	fluxlist[0]->SetNameTitle("FluxHist",
				  "FluxHist");
	
	// Prep empty total events
	eventlist[0] = (TH1D*) fluxplot->Clone();
	eventlist[0]->SetNameTitle("EvtHist",
				   "EvtHist");
	eventlist[0]->Reset();
	
      } else {
	
	// Add up each new plot
	fluxlist[0]->Add(fluxplot);
      }
      
      fluxplot->SetNameTitle(Form("nuwro_pdg%i_pct%f_Flux",pdg,pctg),
			     Form("nuwro_pdg%i_pct%f_Flux",pdg,pctg));
      
      TH1D* eventplot = (TH1D*) fluxplot->Clone();
      eventplot->SetNameTitle(Form("nuwro_pdg%i_pct%f_Evt",pdg,pctg),
			      Form("nuwro_pdg%i_pct%f_Evt",pdg,pctg));
      eventplot->Reset();

      fluxlist[pdg]  = fluxplot;
      eventlist[pdg] = eventplot;
      nevtlist[pdg] = 0;
      intxseclist[pdg] = 0.0;
      allpdg.push_back(pdg);
      
    }
  }   

  // Start main event loop to fill plots
  int nevents = nuwrotree->GetEntries();
  double Enu = 0.0;
  double TotXSec = 0.0;
  //double totaleventmode = 0.0;
  //double totalevents = 0.0;
  int pdg = 0;
  int countwidth = nevents/50.0;
  
  for (int i = 0; i < nevents; i++){
    nuwrotree->GetEntry(i);

    // Get Variables
    Enu = evt->in[0].E() / 1000.0;
    TotXSec = evt->weight;
    pdg = evt->in[0].pdg;

    eventlist[0]->Fill(Enu);
    eventlist[pdg]->Fill(Enu);
    
    nevtlist[0] += 1;
    nevtlist[pdg] += 1;

    intxseclist[0]   += TotXSec;
    intxseclist[pdg] += TotXSec;

    if (i % countwidth == 0)
      LOG(FIT) << "Processed " << i <<" events "
	       << " (" << int(i*100.0/nevents) << "%)"
	       << " : E, W, PDG = " 
	       << Enu << ", " << TotXSec << ", "
	       << pdg << std::endl;
	    
  }

  TH1D* zeroevents = (TH1D*) eventlist[0]->Clone();
  
  // Loop over eventlist
  for (uint i = 0; i < allpdg.size(); i++){

    int pdg = allpdg[i];
    double AvgXSec = intxseclist[0] * 1E38 / double(nevtlist[0]);

    LOG(FIT) << pdg << " Avg XSec = " << AvgXSec << endl;
    LOG(FIT) << pdg << " nevents = " << double(nevtlist[pdg]) << endl;
    
    if (!isMono){
      // Convert events to PDF
      eventlist[pdg] -> Scale(1.0 / zeroevents->Integral("width"));
      
      // Multiply by total predicted event rate
      eventlist[pdg] -> Scale( fluxlist[0]->Integral("width") * AvgXSec );
    } else {
      // If a mono-energetic flux was used, width should not be used
      // The output is (now) forced to be flux = 1, evtrt = xsec (in 1E38 * nb cm^2)
      eventlist[pdg] -> Scale(1.0 / zeroevents->Integral());
      eventlist[pdg] -> Scale( fluxlist[0]->Integral()*AvgXSec );
    }

    // Save everything
    fluxlist[pdg]  -> Write("",TObject::kOverwrite);
    eventlist[pdg] -> Write("",TObject::kOverwrite);    
  }

  // Tidy up
  inRootFile->Close();
  fluxlist.clear();
  eventlist.clear();

  // Exit Program
  return;
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
  LOG(FIT) <<" Running HADD from ExtFit_PrepareNuwro: "<<cmd<<std::endl;

  // Start HADD
  system(cmd.c_str());
  
  // Return name of output file
  inputs.clear();
  inputs.push_back( outputname );
  return;
}
  

