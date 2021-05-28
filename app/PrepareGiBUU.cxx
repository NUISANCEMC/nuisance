#include "FitLogger.h"
#include "PlotUtils.h"
#include "StatUtils.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include <stdio.h>
#include <stdlib.h>

std::string fInputFiles = "";
std::string fOutputFile = "";
std::string fFluxFile   = "";

void PrintOptions();
void ParseOptions(int argc, char *argv[]);
void CreateRateHistogram(std::string inputList, std::string flux,
                         std::string output);
TH1D* MakeFluxHistFromDatFile(std::string inputDatFile);

int main(int argc, char *argv[]) {

  SETVERBOSITY(FitPar::Config().GetParI("VERBOSITY"));
  SETERRVERBOSITY(FitPar::Config().GetParI("ERROR"));

  ParseOptions(argc, argv);
  NUIS_LOG(FIT, "Running PrepareGiBUU");
  CreateRateHistogram(fInputFiles, fFluxFile, fOutputFile);
};

TH1D* MakeFluxHistFromDatFile(std::string inputDatFile){
  
  std::vector<double> bin_cent;
  std::vector<double> bin_vals;

  // Loop over lines in the file
  std::string line;
  std::ifstream in_file(inputDatFile.c_str(), std::ifstream::in);
  
  while (std::getline(in_file >> std::ws, line, '\n')) {

    // Skip any lines that start with a #
    if (line.at(0) == '#') continue;
    
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, "\t");

    // Append the bin centers and bin values
    bin_cent.push_back(entries[0]);
    bin_vals.push_back(entries[1]);
  }

  if (bin_cent.size() < 2 || bin_vals.size() < 2){
    NUIS_ABORT(inputDatFile << " is not a well formed file!");
  }
  
  // Check that the histogram has equal binning (currently required)
  double bin_stride = bin_cent[1] - bin_cent[0];

  // Check stride uniformity
  for (uint s = 0; s < bin_cent.size()-1; ++s){
    double this_stride = bin_cent[s+1] - bin_cent[s];
    if (fabs(this_stride-bin_stride) > 1E-8){
      NUIS_ABORT(inputDatFile << " does not have regular binning!");
    }
  }

  // Get bin edges
  std::vector<double> bin_edges;
  for (uint s = 0; s < bin_cent.size(); ++s){
    bin_edges.push_back(bin_cent[s] - bin_stride/2.);
  }
  bin_edges.push_back(bin_cent[bin_cent.size()-1]+bin_stride/2.);

  // Finally, make the histogram
  TH1D* flux = new TH1D("flux", "flux; E_{#nu} (GeV)", bin_cent.size(), &bin_edges[0]);
  for (uint x = 0; x < bin_cent.size(); ++x){
    flux -> SetBinContent(x+1, bin_vals[x]);
  }

  return flux;
}


void CreateRateHistogram(std::string inputList, std::string flux,
                         std::string output) {

  TChain *tn = new TChain("RootTuple");
  double E, xsec;
  tn->SetBranchAddress("lepIn_E", &E);
  tn->SetBranchAddress("weight", &xsec);

  std::vector<std::string> inputs = GeneralUtils::ParseToStr(inputList, ",");
  for (std::vector<std::string>::iterator it = inputs.begin();
       it != inputs.end(); ++it) {
    NUIS_LOG(FIT, "Adding " << *it << " to the output");
    tn->AddFile((*it).c_str());
  }

  if (inputs.size() > 1 && output.empty()) {
    NUIS_ABORT("You must provide a new output file name if you want to have "
           "more than 1 input file!");
  }

  int nevts = tn->GetEntries();

  if (!nevts) {
    NUIS_ABORT("Either the input file is not from GiBUU, or it's empty...");
  }

  // Get flux hist
  std::vector<std::string> fluxvect = GeneralUtils::ParseToStr(flux, ",");
  TH1D *fluxHist = NULL;
  if (fluxvect.size() > 1) {
    TFile *fluxfile = new TFile(fluxvect[0].c_str(), "READ");
    fluxHist = (TH1D *)fluxfile->Get(fluxvect[1].c_str());
    fluxHist->SetDirectory(0);
  } else if (flux.find(".dat") != std::string::npos){
    fluxHist = MakeFluxHistFromDatFile(flux);
  } else {
    NUIS_ABORT("NO FLUX SPECIFIED");
  }

  // Make Event Hist
  TH1D *xsecHist = (TH1D *)fluxHist->Clone();
  xsecHist->Reset();

  // Make a total cross section hist for shits and giggles
  TH1D *entryHist = (TH1D *)xsecHist->Clone();

  for (int i = 0; i < nevts; ++i) {
    tn->GetEntry(i);
    xsecHist->Fill(E, xsec);
    entryHist->Fill(E);

    if (i % (nevts / 10) == 0) {
      NUIS_LOG(FIT, "Processed " << i << "/" << nevts << " GiBUU events."
                             << "(Enu = " << E << ", xsec = " << xsec << ") ");
    }
  }
  NUIS_LOG(FIT, "Processed all events");

  xsecHist->Scale(1, "width");
  // xsecHist->Divide(entryHist);

  // This will be the evtrt histogram
  TH1D *evtHist = (TH1D*)xsecHist->Clone();
  evtHist->Multiply(fluxHist);

  // Check whether the overflow is empty. If not, advise that either the wrong
  // flux histogram or units were used...
  // If the events were generated with a limited range of the flux histogram,
  // this may be benign
  if (evtHist->Integral(0, -1) != evtHist->Integral() ||
      evtHist->Integral(0, -1) == 0) {
    NUIS_ERR(WRN, "The input file("
                    << evtHist->Integral(0, -1)
                    << ") and flux histogram provided do not match... ");
    NUIS_ERR(WRN,
           "Are the units correct? Did you provide the correct flux file?");
    NUIS_ERR(WRN, "Use output with caution...");
  }

  // Pick where the output should go
  TFile *outFile = NULL;
  if (!output.empty()) {
    NUIS_LOG(FIT, "Saving histograms in " << output);
    outFile = new TFile(output.c_str(), "RECREATE");
  } else {
    NUIS_LOG(FIT, "Saving histograms in " << inputs[0]);
    outFile = new TFile(inputs[0].c_str(), "UPDATE");
  }
  outFile->cd();

  std::string xsec_name = "xsec_PrepareGiBUU";
  std::string flux_name = "flux_PrepareGiBUU";
  std::string rate_name = "evtrt_PrepareGiBUU";

  if (output.empty()) {
    // Check whether we should overwrite existing histograms
    std::string input_xsec = PlotUtils::GetObjectWithName(outFile, "xsec");
    std::string input_flux = PlotUtils::GetObjectWithName(outFile, "flux");
    std::string input_rate = PlotUtils::GetObjectWithName(outFile, "evtrt");

    if (!input_xsec.empty()) {
      NUIS_LOG(FIT, "Updating histogram: " << input_xsec);
      xsec_name = input_xsec;
    }
    if (!input_flux.empty()) {
      NUIS_LOG(FIT, "Updating histogram: " << input_flux);
      flux_name = input_flux;
    }
    if (!input_rate.empty()) {
      NUIS_LOG(FIT, "Updating histogram: " << input_rate);
      rate_name = input_rate;
    }

  } else {
    NUIS_LOG(FIT, "Cloning event tree into output file.");
    StopTalking();
    TTree *newtree = (TTree *)tn->CloneTree(-1, "fast");
    StartTalking();
    newtree->Write();
  }

  xsecHist->Write(xsec_name.c_str(), TObject::kOverwrite);
  fluxHist->Write(flux_name.c_str(), TObject::kOverwrite);
  evtHist->Write(rate_name.c_str(), TObject::kOverwrite);
  outFile->Close();

  return;
}

void PrintOptions() {
  std::cout << "PrepareGiBUU NUISANCE app. " << std::endl
            << "Produces or recalculates evtrt and flux histograms necessary "
               "for NUISANCE normalization."
            << std::endl;
  std::cout << "PrepareGiBUU: " << std::endl;
  std::cout << "    [-h,-help,--h,--help]" << std::endl;
  std::cout << "    -i inputfile1.root,inputfile2.root,inputfile3.root,..."
            << std::endl;
  std::cout << "          Takes any number of files, but assumes all are "
               "produced with a single flux"
            << std::endl;
  std::cout << "    -f flux_root_file.root,flux_hist_name" << std::endl;
  std::cout << "          Path to ROOT or .dat file containing the flux used "
               "when generating the GiBUU files"
            << std::endl;
  std::cout << "    [-o outputfile.root] " << std::endl;
  std::cout
      << "          If an output file is not given, the input file will be used"
      << std::endl;
  std::cout << "          If more than one input file is given, an output file "
               "must be given"
            << std::endl;
}

void ParseOptions(int argc, char *argv[]) {
  bool flagopt = false;

  // If No Arguments print commands
  for (int i = 1; i < argc; ++i) {
    if (!std::strcmp(argv[i], "-h")) {
      flagopt = true;
      break;
    }
    if (i + 1 != argc) {
      // Cardfile
      if (!std::strcmp(argv[i], "-h")) {
        flagopt = true;
        break;
      } else if (!std::strcmp(argv[i], "-i")) {
        fInputFiles = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-o")) {
        fOutputFile = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-f")) {
        fFluxFile = argv[i + 1];
        ++i;
      } else {
        NUIS_ERR(FTL, "ERROR: unknown command line option given! - '"
                        << argv[i] << " " << argv[i + 1] << "'");
        PrintOptions();
        break;
      }
    }
  }
  if (fInputFiles == "" && !flagopt) {
    NUIS_ERR(FTL, "No input file(s) specified!");
    flagopt = true;
  }

  if (fFluxFile == "" && (!flagopt)) {
    NUIS_ERR(FTL, "No flux input specified!");
    flagopt = true;
  }

  if (argc < 1 || flagopt) {
    PrintOptions();
    exit(-1);
  }

  return;
}
