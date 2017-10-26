#include <stdio.h>
#include <stdlib.h>
#include "FitLogger.h"
#include "PlotUtils.h"
#include "StatUtils.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

// If you don't have NEUT enabled, you shouldn't compile this...
#include "neutpart.h"
#include "neutvect.h"

std::string fInputFiles = "";
std::string fOutputFile = "";
std::string fFluxFile = "";
bool fFluxInGeV = false;
bool fIsMonoEFlux = false;
double fMonoEEnergy = 0xdeadbeef;

void PrintOptions();
void ParseOptions(int argc, char* argv[]);
void AddMonoRateHistogram(std::string inputList, double MonoE,
                          std::string output);
void CreateRateHistogram(std::string inputList, std::string flux,
                         std::string output);

//*******************************
int main(int argc, char* argv[]) {
  //*******************************

  LOG_VERB(FitPar::Config().GetParI("VERBOSITY"));
  ERR_VERB(FitPar::Config().GetParI("ERROR"));

  ParseOptions(argc, argv);
  LOG(FIT) << "Running PrepareNEUT" << std::endl;
  if (fIsMonoEFlux) {
    AddMonoRateHistogram(fInputFiles, fMonoEEnergy, fOutputFile);
  } else {
    CreateRateHistogram(fInputFiles, fFluxFile, fOutputFile);
  }
};

void AddMonoRateHistogram(std::string inputList, double MonoE,
                          std::string output) {
  // Need to allow for more than one file... will do soon
  TChain* tn = new TChain("neuttree");

  std::vector<std::string> inputs = GeneralUtils::ParseToStr(inputList, ",");
  for (std::vector<std::string>::iterator it = inputs.begin();
       it != inputs.end(); ++it) {
    LOG(FIT) << "Adding " << *it << " to the output" << std::endl;
    tn->AddFile((*it).c_str());
  }

  if (inputs.size() > 1 && output.empty()) {
    ERR(FTL) << "You must provide a new output file name if you want to have "
                "more than 1 input file!"
             << std::endl;
    throw;
  }

  int nevts = tn->GetEntries();

  if (!nevts) {
    ERR(FTL) << "Either the input file is not from NEUT, or it's empty..."
             << std::endl;
    throw;
  }

  NeutVect* fNeutVect = NULL;
  tn->SetBranchAddress("vectorbranch", &fNeutVect);

  TH1D* fluxHist = new TH1D("flux", "flux", 1000, 0, fFluxInGeV ? 10 : 10000);
  fluxHist->Fill(MonoE);
  fluxHist->Scale(1, "width");
  // Make Event Hist
  TH1D* xsecHist = (TH1D*)fluxHist->Clone();
  xsecHist->Reset();

  // Make a total cross section hist for shits and giggles
  TH1D* entryHist = (TH1D*)xsecHist->Clone();

  double MeanE = 0;
  for (int i = 0; i < nevts; ++i) {
    tn->GetEntry(i);
    NeutPart* part = fNeutVect->PartInfo(0);
    double E = part->fP.E();
    double xsec = fNeutVect->Totcrs;

    // Unit conversion
    if (fFluxInGeV) E *= 1E-3;

    xsecHist->Fill(E, xsec);
    entryHist->Fill(E);
    MeanE += E;

    if (i % (nevts / 20) == 0) {
      LOG(FIT) << "Processed " << i << "/" << nevts << " NEUT events."
               << std::endl;
    }
  }
  MeanE /= double(nevts);
  LOG(FIT) << "Processed all events" << std::endl;

  xsecHist->Divide(entryHist);

  // This will be the evtrt histogram
  TH1D* evtHist = (TH1D*)xsecHist->Clone();
  evtHist->Multiply(fluxHist);

  // Check whether the overflow is empty. If not, advise that either the wrong
  // flux histogram or units were used...
  // If the events were generated with a limited range of the flux histogram,
  // this may be benign
  if (evtHist->Integral(0, -1) != evtHist->Integral() ||
      evtHist->Integral(0, -1) == 0) {
    ERR(WRN) << "The input file and flux histogram provided do not match... "
             << std::endl;
    ERR(WRN) << "Are the units correct (MeanE = " << MeanE
             << ", FluxHistoUpperLim: "
             << fluxHist->GetXaxis()->GetBinUpEdge(1000)
             << ")? Did you provide the correct flux file?" << std::endl;
    ERR(WRN) << "Use output with caution..." << std::endl;
  }

  // Pick where the output should go
  TFile* outFile = NULL;
  if (!output.empty()) {
    LOG(FIT) << "Saving histograms in " << output << std::endl;
    outFile = new TFile(output.c_str(), "RECREATE");
  } else {
    LOG(FIT) << "Saving histograms in " << inputs[0] << std::endl;
    outFile = new TFile(inputs[0].c_str(), "UPDATE");
  }
  outFile->cd();

  std::string xsec_name = "xsec_PrepareNeut";
  std::string flux_name = "flux_PrepareNeut";
  std::string rate_name = "evtrt_PrepareNeut";

  if (output.empty()) {
    // Check whether we should overwrite existing histograms
    std::string input_xsec = PlotUtils::GetObjectWithName(outFile, "xsec");
    std::string input_flux = PlotUtils::GetObjectWithName(outFile, "flux");
    std::string input_rate = PlotUtils::GetObjectWithName(outFile, "evtrt");

    if (!input_xsec.empty()) {
      LOG(FIT) << "Updating histogram: " << input_xsec << std::endl;
      xsec_name = input_xsec;
    }
    if (!input_flux.empty()) {
      LOG(FIT) << "Updating histogram: " << input_flux << std::endl;
      flux_name = input_flux;
    }
    if (!input_rate.empty()) {
      LOG(FIT) << "Updating histogram: " << input_rate << std::endl;
      rate_name = input_rate;
    }

  } else {
    LOG(FIT) << "Cloning neuttree into output file." << std::endl;
    StopTalking();
    TTree* newtree = (TTree*)tn->CloneTree(-1, "fast");
    StartTalking();
    newtree->Write();
  }

  xsecHist->Write(xsec_name.c_str(), TObject::kOverwrite);
  fluxHist->Write(flux_name.c_str(), TObject::kOverwrite);
  evtHist->Write(rate_name.c_str(), TObject::kOverwrite);

  outFile->Close();
}

//*******************************
void CreateRateHistogram(std::string inputList, std::string flux,
                         std::string output) {
  //*******************************

  // Need to allow for more than one file... will do soon
  TChain* tn = new TChain("neuttree");

  std::vector<std::string> inputs = GeneralUtils::ParseToStr(inputList, ",");
  for (std::vector<std::string>::iterator it = inputs.begin();
       it != inputs.end(); ++it) {
    LOG(FIT) << "Adding " << *it << " to the output" << std::endl;
    tn->AddFile((*it).c_str());
  }

  if (inputs.size() > 1 && output.empty()) {
    ERR(FTL) << "You must provide a new output file name if you want to have "
                "more than 1 input file!"
             << std::endl;
    throw;
  }

  int nevts = tn->GetEntries();

  if (!nevts) {
    ERR(FTL) << "Either the input file is not from NEUT, or it's empty..."
             << std::endl;
    throw;
  }

  NeutVect* fNeutVect = NULL;
  tn->SetBranchAddress("vectorbranch", &fNeutVect);

  // Get Flux Hist
  std::vector<std::string> fluxvect = GeneralUtils::ParseToStr(flux, ",");
  TH1D* fluxHist = NULL;
  if (fluxvect.size() > 1) {
    TFile* fluxfile = new TFile(fluxvect[0].c_str(), "READ");
    fluxHist = (TH1D*)fluxfile->Get(fluxvect[1].c_str());
    fluxHist->SetDirectory(0);
  } else {
    ERR(FTL) << "NO FLUX SPECIFIED" << std::endl;
    throw;
  }

  // Decide what type of flux was given
  if (fFluxInGeV)
    LOG(FIT) << "Assuming flux histogram is in GeV" << std::endl;
  else
    LOG(FIT) << "Assuming flux histogram is in MeV" << std::endl;

  // Make Event Hist
  TH1D* xsecHist = (TH1D*)fluxHist->Clone();
  xsecHist->Reset();

  // Make a total cross section hist for shits and giggles
  TH1D* entryHist = (TH1D*)xsecHist->Clone();

  for (int i = 0; i < nevts; ++i) {
    tn->GetEntry(i);
    NeutPart* part = fNeutVect->PartInfo(0);
    double E = part->fP.E();
    double xsec = fNeutVect->Totcrs;

    // Unit conversion
    if (fFluxInGeV) E *= 1E-3;

    xsecHist->Fill(E, xsec);
    entryHist->Fill(E);

    if (i % (nevts / 20) == 0) {
      LOG(FIT) << "Processed " << i << "/" << nevts << " NEUT events."
               << std::endl;
    }
  }
  LOG(FIT) << "Processed all events" << std::endl;

  xsecHist->Divide(entryHist);

  // This will be the evtrt histogram
  TH1D* evtHist = NULL;

  // If the integral of xsecHist is 0 the input file used a really old version
  // of NEUT without Totcrs
  if (!xsecHist->Integral(0, -1)) {
    ERR(WRN) << "Old NEUT input file: events will not be correctly normalized"
             << std::endl;
    evtHist = (TH1D*)entryHist->Clone();

    if (evtHist->Integral() != 0)
      evtHist->Scale(fluxHist->Integral() / float(evtHist->Integral()));
  } else {
    evtHist = (TH1D*)xsecHist->Clone();
    evtHist->Multiply(fluxHist);
  }

  // Check whether the overflow is empty. If not, advise that either the wrong
  // flux histogram or units were used...
  // If the events were generated with a limited range of the flux histogram,
  // this may be benign
  if (evtHist->Integral(0, -1) != evtHist->Integral() ||
      evtHist->Integral(0, -1) == 0) {
    ERR(WRN) << "The input file and flux histogram provided do not match... "
             << std::endl;
    ERR(WRN) << "Are the units correct? Did you provide the correct flux file?"
             << std::endl;
    ERR(WRN) << "Use output with caution..." << std::endl;
  }

  // Pick where the output should go
  TFile* outFile = NULL;
  if (!output.empty()) {
    LOG(FIT) << "Saving histograms in " << output << std::endl;
    outFile = new TFile(output.c_str(), "RECREATE");
  } else {
    LOG(FIT) << "Saving histograms in " << inputs[0] << std::endl;
    outFile = new TFile(inputs[0].c_str(), "UPDATE");
  }
  outFile->cd();

  std::string xsec_name = "xsec_PrepareNeut";
  std::string flux_name = "flux_PrepareNeut";
  std::string rate_name = "evtrt_PrepareNeut";

  if (output.empty()) {
    // Check whether we should overwrite existing histograms
    std::string input_xsec = PlotUtils::GetObjectWithName(outFile, "xsec");
    std::string input_flux = PlotUtils::GetObjectWithName(outFile, "flux");
    std::string input_rate = PlotUtils::GetObjectWithName(outFile, "evtrt");

    if (!input_xsec.empty()) {
      LOG(FIT) << "Updating histogram: " << input_xsec << std::endl;
      xsec_name = input_xsec;
    }
    if (!input_flux.empty()) {
      LOG(FIT) << "Updating histogram: " << input_flux << std::endl;
      flux_name = input_flux;
    }
    if (!input_rate.empty()) {
      LOG(FIT) << "Updating histogram: " << input_rate << std::endl;
      rate_name = input_rate;
    }

  } else {
    LOG(FIT) << "Cloning neuttree into output file." << std::endl;
    StopTalking();
    TTree* newtree = (TTree*)tn->CloneTree(-1, "fast");
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
  std::cout << "PrepareNEUT NUISANCE app. " << std::endl
            << "Produces or recalculates evtrt and flux histograms necessary "
               "for NUISANCE normalization."
            << std::endl;
  std::cout << "PrepareNEUT: " << std::endl;
  std::cout << "    [-h,-help,--h,--help]" << std::endl;
  std::cout << "    -i inputfile1.root,inputfile2.root,inputfile3.root,..."
            << std::endl;
  std::cout << "          Takes any number of files, but assumes all are "
               "produced with a single flux"
            << std::endl;
  std::cout << "    -f flux_root_file.root,flux_hist_name" << std::endl;
  std::cout << "          Path to root file containing the flux histogram used "
               "when generating the NEUT files"
            << std::endl;
  std::cout << "    [-o outputfile.root] " << std::endl;
  std::cout
      << "          If an output file is not given, the input file will be used"
      << std::endl;
  std::cout << "          If more than one input file is given, an output file "
               "must be given"
            << std::endl;
  std::cout << "    [-G]" << std::endl;
  std::cout << "          Flux is assumed to be in MeV. This switch indicates "
               "the input flux is in GeV"
            << std::endl;
  std::cout << "    [-m E_nu]" << std::endl;
  std::cout << "          Used to add dummy flux and evt rate histograms to "
               "mono-energetic vectors. Adheres to the -G flag."
            << std::endl;
}

void ParseOptions(int argc, char* argv[]) {
  bool flagopt = false;

  // If No Arguments print commands
  for (int i = 1; i < argc; ++i) {
    if (!std::strcmp(argv[i], "-h")) {
      flagopt = true;
      break;
    } else if (!std::strcmp(argv[i], "-G")) {
      fFluxInGeV = true;
      continue;
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
      } else if (!std::strcmp(argv[i], "-m")) {
        fIsMonoEFlux = true;
        fMonoEEnergy = GeneralUtils::StrToDbl(argv[i + 1]);
        ++i;
      } else {
        ERR(FTL) << "ERROR: unknown command line option given! - '" << argv[i]
                 << " " << argv[i + 1] << "'" << std::endl;
        PrintOptions();
        break;
      }
    }
  }
  if (fInputFiles == "" && !flagopt) {
    ERR(FTL) << "No input file(s) specified!" << std::endl;
    flagopt = true;
  }

  if (fFluxFile == "" && (!flagopt) && (!fIsMonoEFlux)) {
    ERR(FTL) << "No flux input specified!" << std::endl;
    flagopt = true;
  }

  if (argc < 1 || flagopt) {
    PrintOptions();
    exit(-1);
  }

  return;
}
