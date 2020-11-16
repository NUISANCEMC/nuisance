#include "event1.h"
#include <stdio.h>
#include <stdlib.h>
// Hopefully we don't need these as they're included above.
// #include "params_all.h"
// #include "params.h"
#include "FitLogger.h"
#include "PlotUtils.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

void printInputCommands(char *argv[]) {
  std::cout << "[USAGE]: " << argv[0]
            << " [-h] [-f] [-F <FluxRootFile>,<FluxHistName>[,PDG[,speciesFraction]] [-o output.root] "
               "inputfile.root [file2.root ...]"
            << std::endl
            << "\t-h : Print this message." << std::endl
            << "\t-f : Pass -f argument to '$ hadd' invocation." << std::endl
            << "\t-F : Read input flux from input descriptor." << std::endl
            << "\t-o : Write full output to a new file." << std::endl
            << std::endl;
};
void CreateRateHistograms(std::string inputs, bool force_out);
void HaddNuwroFiles(std::vector<std::string> &inputs, bool force_out);

bool outputNewFile = false;
std::string ofile = "";
bool haveFluxInputs = false;

struct FluxInputBlob {
  FluxInputBlob(std::string _File, std::string _Hist, int _PDG,
                double _Fraction)
      : File(_File), Hist(_Hist), PDG(_PDG), Fraction(_Fraction) {}
  std::string File;
  std::string Hist;
  int PDG;
  double Fraction;
};

std::vector<FluxInputBlob> FluxInputs;

bool haddedFiles = false;

TH1D *F2D(TH1F *f) {
  Double_t *bins = new Double_t[f->GetXaxis()->GetNbins() + 1];
  for (Int_t bi_it = 0; bi_it < f->GetXaxis()->GetNbins(); ++bi_it) {
    bins[bi_it] = f->GetXaxis()->GetBinLowEdge(bi_it + 1);
  }
  bins[f->GetXaxis()->GetNbins()] =
      f->GetXaxis()->GetBinUpEdge(f->GetXaxis()->GetNbins());

  TH1D *d = new TH1D((std::string(f->GetName()) + "_f").c_str(), f->GetTitle(),
                     f->GetXaxis()->GetNbins(), bins);
  std::cout << "Converted TH1F with " << f->GetXaxis()->GetXbins()
            << " bins : " << std::endl;
  for (Int_t bi_it = 0; bi_it < f->GetXaxis()->GetNbins() + 2; ++bi_it) {
    d->SetBinContent(bi_it, f->GetBinContent(bi_it));
    d->SetBinError(bi_it, f->GetBinError(bi_it));
    std::cout << "\tF " << f->GetXaxis()->GetBinLowEdge(bi_it) << "--[" << bi_it
              << "]--" << f->GetXaxis()->GetBinUpEdge(bi_it) << ": "
              << f->GetBinContent(bi_it) << " D "
              << d->GetXaxis()->GetBinLowEdge(bi_it) << "--[" << bi_it << "]--"
              << d->GetXaxis()->GetBinUpEdge(bi_it) << ": "
              << d->GetBinContent(bi_it) << std::endl;
  }
  delete bins;
  return d;
}
TH1D *GetTH1DFromFile(std::string const &rootFile,
                      std::string const &histName) {
  TFile *inpFile = new TFile(rootFile.c_str(), "READ");
  if (!inpFile || !inpFile->IsOpen()) {
    NUIS_ABORT("Cannot open input root file: " << rootFile
                                               << " to read input histo.");
  }

  TH1D *histD = dynamic_cast<TH1D *>(inpFile->Get(histName.c_str()));
  if (!histD) {
    TH1F *histF = dynamic_cast<TH1F *>(inpFile->Get(histName.c_str()));
    if (!histF) {
      NUIS_ABORT("Cannot find TH1D/F: " << histName << " in root file: "
                                        << rootFile << ".");
    }
    histD = F2D(histF);
  } else {
    histD = static_cast<TH1D *>(histD->Clone());
  }
  histD->SetDirectory(NULL);
  inpFile->Close();
  return histD;
}

//*******************************
int main(int argc, char *argv[]) {
  //*******************************

  // If No Arguments print commands
  if (argc == 1) {
    printInputCommands(argv);
    return 0;
  }

  int verbocount = 0;
  int errorcount = 0;
  verbocount += Config::GetParI("VERBOSITY");
  errorcount += Config::GetParI("ERROR");
  bool trace = Config::GetParB("TRACE");
  std::cout << "[ NUISANCE ]: Setting VERBOSITY=" << verbocount << std::endl;
  std::cout << "[ NUISANCE ]: Setting ERROR=" << errorcount << std::endl;
  SETVERBOSITY(verbocount);
  SETTRACE(trace);

  std::vector<std::string> inputfiles;
  bool force_output = false;

  // Get Inputs
  for (int i = 1; i < argc; ++i) {
    if (!std::strcmp(argv[i], "-h")) {
      printInputCommands(argv);
      return 0;
    } else if (!std::strcmp(argv[i], "-f")) {
      force_output = true;
    } else if (!std::strcmp(argv[i], "-o")) {
      outputNewFile = true;
      ofile = argv[++i];
    } else if (!std::strcmp(argv[i], "-F")) {
      std::string inpLine = argv[++i];
      std::vector<std::string> fluxInputDescriptor =
          GeneralUtils::ParseToStr(inpLine, ",");
      if ((fluxInputDescriptor.size() != 2) &&
          (fluxInputDescriptor.size() != 3) &&
          (fluxInputDescriptor.size() != 4)) {
        NUIS_ABORT("Received -F argument with option: \""
                   << inpLine
                   << "\", was expecting "
                      "<FluxRootFile>,<FluxHistName>[,PDG[,speciesFraction]].");
      }
      haveFluxInputs = true;
      FluxInputs.push_back(
          FluxInputBlob(fluxInputDescriptor[0], fluxInputDescriptor[1],
                        (fluxInputDescriptor.size() > 2)
                            ? GeneralUtils::StrToInt(fluxInputDescriptor[2])
                            : 14,
                        (fluxInputDescriptor.size() > 3)
                            ? GeneralUtils::StrToDbl(fluxInputDescriptor[3])
                            : 1));

      if (!FluxInputs.back().File.length() ||
          !FluxInputs.back().Hist.length()) {
        NUIS_ABORT("Received -F argument with option: \""
                   << inpLine
                   << "\", was expecting "
                      "<FluxRootFile>,<FluxHistName>[,PDG[,speciesFraction]].");
      }
    } else {
      inputfiles.push_back(std::string(argv[i]));
    }
  }

  // If one input file just create flux histograms
  if (inputfiles.size() > (UInt_t)1) {
    HaddNuwroFiles(inputfiles, force_output);
  } else if (inputfiles.size() < (UInt_t)1) {
    printInputCommands(argv);
  }

  CreateRateHistograms(inputfiles[0], force_output);

  NUIS_LOG(FIT, "Finished NUWRO Prep.");
};

//*******************************
void CreateRateHistograms(std::string inputs, bool force_out) {
  //*******************************

  // Open root file
  TFile *outRootFile = 0;
  TTree *nuwrotree = 0;

  if (!haddedFiles &&
      outputNewFile) { // we need to make the new file and clone the tree.
    TFile *inpFile = new TFile(inputs.c_str(), "READ");
    if (!inpFile || !inpFile->IsOpen()) {
      NUIS_ABORT("Cannot open input root file: " << inputs);
    }
    TTree *inpTree = dynamic_cast<TTree *>(inpFile->Get("treeout"));
    if (!inpTree) {
      NUIS_ABORT("Cannot find TTree \"treeout\" in input root file: "
                 << inputs.c_str());
    }

    outRootFile = new TFile(ofile.c_str(), force_out ? "RECREATE" : "CREATE");
    if (!outRootFile || !outRootFile->IsOpen()) {
      NUIS_ABORT("Couldn't open root file: "
                 << ofile << " for writing, does it already exist?");
    }

    nuwrotree = inpTree->CloneTree(-1, "fast");
    nuwrotree->SetDirectory(outRootFile);
    nuwrotree->Write(nuwrotree->GetName());
  } else {
    outRootFile = new TFile(inputs.c_str(), "UPDATE");
    if (!outRootFile || !outRootFile->IsOpen()) {
      NUIS_ABORT("Cannot open input root file: " << inputs);
    }
    nuwrotree = dynamic_cast<TTree *>(outRootFile->Get("treeout"));
    if (!nuwrotree) {
      NUIS_ABORT("Cannot find TTree \"treeout\" in input root file: "
                 << inputs.c_str());
    }
  }

  // Get Flux Histogram
  event *evt = new event();
  nuwrotree->SetBranchAddress("e", &evt);
  nuwrotree->GetEntry(0);

  int fluxtype = evt->par.beam_type;

  std::map<int, TH1D *> fluxlist;
  std::map<int, TH1D *> eventlist;
  std::vector<int> allpdg;
  std::map<int, int> nevtlist;
  std::map<int, double> intxseclist;

  // Did the input file have a mono-energetic flux?
  bool isMono = false;

  nevtlist[0] = 0.0;
  intxseclist[0] = 0.0;

  allpdg.push_back(0);

  NUIS_LOG(FIT, "Nuwro fluxtype = " << fluxtype);
  if (haveFluxInputs) {
    double totalFraction = 0;
    for (size_t flux_it = 0; flux_it < FluxInputs.size(); ++flux_it) {
      FluxInputBlob &fb = FluxInputs[flux_it];

      int pdg = fb.PDG;

      TH1D *fluxHist = GetTH1DFromFile(fb.File, fb.Hist);

      double pctg = fb.Fraction;
      totalFraction += pctg;
      double Elow = fluxHist->GetXaxis()->GetBinLowEdge(1);
      double Ehigh = fluxHist->GetXaxis()->GetBinLowEdge(
          fluxHist->GetXaxis()->GetNbins() + 1);

      NUIS_LOG(FIT, "Adding new nuwro flux "
                        << "pdg: " << pdg << " pctg: " << pctg
                        << " Elow: " << Elow << " Ehigh: " << Ehigh);

      // Sort total flux plot
      if (!fluxlist[0]) {
        // Setup total flux
        fluxlist[0] = (TH1D *)fluxHist->Clone();
        fluxlist[0]->SetNameTitle("FluxHist", "FluxHist");

        // Prep empty total events
        eventlist[0] = (TH1D *)fluxHist->Clone();
        eventlist[0]->SetNameTitle("EvtHist", "EvtHist");
        eventlist[0]->Reset();

      } else {
        // Add up each new plot
        fluxlist[0]->Add(fluxHist);
      }

      fluxHist->SetNameTitle(Form("nuwro_pdg%i_pct%f_Flux", pdg, pctg),
                             Form("nuwro_pdg%i_pct%f_Flux", pdg, pctg));

      TH1D *eventplot = (TH1D *)fluxHist->Clone();
      eventplot->SetNameTitle(Form("nuwro_pdg%i_pct%f_Evt", pdg, pctg),
                              Form("nuwro_pdg%i_pct%f_Evt", pdg, pctg));
      eventplot->Reset();

      fluxlist[pdg] = (TH1D *)fluxHist->Clone();
      eventlist[pdg] = eventplot;
      nevtlist[pdg] = 0;
      intxseclist[pdg] = 0.0;
      allpdg.push_back(pdg);
      delete fluxHist;
    }
    if (fabs(totalFraction - 1) > 1E-5) {
      NUIS_ABORT("Total species fraction for input flux histos = "
                 << totalFraction << ", expected to sum to 1.");
    }
  } else if (fluxtype == 0) {
    std::string fluxstring = evt->par.beam_energy;
    std::vector<double> fluxvals = GeneralUtils::ParseToDbl(fluxstring, " ");

    int pdg = evt->par.beam_particle;
    double Elow = double(fluxvals[0]) / 1000.0;
    double Ehigh = double(fluxvals[1]) / 1000.0;
    TH1D *fluxplot = NULL;

    if (Elow > Ehigh)
      isMono = true;

    // For files produced with a flux distribution
    if (!isMono) {
      NUIS_LOG(FIT, "Adding new nuwro flux "
                        << "pdg: " << pdg << " Elow: " << Elow
                        << " Ehigh: " << Ehigh);

      fluxplot =
          new TH1D("fluxplot", "fluxplot", fluxvals.size() - 4, Elow, Ehigh);
      for (uint j = 2; j < fluxvals.size(); j++) {
        NUIS_LOG(DEB, j << " " << fluxvals[j]);
        fluxplot->SetBinContent(j - 1, fluxvals[j]);
      }
    } else { // For monoenergetic fluxes
      NUIS_LOG(FIT, "Adding mono-energetic nuwro flux "
                        << "pdg: " << pdg << " E: " << Elow);

      fluxplot = new TH1D("fluxplot", "fluxplot", 100, 0, Elow * 2);
      fluxplot->SetBinContent(fluxplot->FindBin(Elow), 1);
    }

    // Setup total flux
    fluxlist[0] = (TH1D *)fluxplot->Clone();
    fluxlist[0]->SetNameTitle("FluxHist", "FluxHist");

    // Prep empty total events
    eventlist[0] = (TH1D *)fluxplot->Clone();
    eventlist[0]->SetNameTitle("EvtHist", "EvtHist");
    eventlist[0]->Reset();

    fluxplot->SetNameTitle(Form("nuwro_pdg%i_Flux", pdg),
                           Form("nuwro_pdg%i_Flux", pdg));

    TH1D *eventplot = (TH1D *)fluxplot->Clone();
    eventplot->SetNameTitle(Form("nuwro_pdg%i_Evt", pdg),
                            Form("nuwro_pdg%i_Evt", pdg));
    eventplot->Reset();

    fluxlist[pdg] = fluxplot;
    eventlist[pdg] = eventplot;
    nevtlist[pdg] = 0;
    intxseclist[pdg] = 0.0;
    allpdg.push_back(pdg);

  } else if (fluxtype == 1) {
    std::string fluxstring = evt->par.beam_content;

    std::vector<std::string> fluxlines =
        GeneralUtils::ParseToStr(fluxstring, "\n");
    for (uint i = 0; i < fluxlines.size(); i++) {
      std::vector<double> fluxvals =
          GeneralUtils::ParseToDbl(fluxlines[i], " ");

      int pdg = int(fluxvals[0]);
      double pctg = double(fluxvals[1]) / 100.0;
      double Elow = double(fluxvals[2]) / 1000.0;
      double Ehigh = double(fluxvals[3]) / 1000.0;

      NUIS_LOG(FIT, "Adding new nuwro flux "
                        << "pdg: " << pdg << " pctg: " << pctg
                        << " Elow: " << Elow << " Ehigh: " << Ehigh);

      TH1D *fluxplot =
          new TH1D("fluxplot", "fluxplot", fluxvals.size() - 4, Elow, Ehigh);
      for (uint j = 4; j < fluxvals.size(); j++) {
        fluxplot->SetBinContent(j + 1, fluxvals[j]);
      }

      // Sort total flux plot
      if (!fluxlist[0]) {
        // Setup total flux
        fluxlist[0] = (TH1D *)fluxplot->Clone();
        fluxlist[0]->SetNameTitle("FluxHist", "FluxHist");

        // Prep empty total events
        eventlist[0] = (TH1D *)fluxplot->Clone();
        eventlist[0]->SetNameTitle("EvtHist", "EvtHist");
        eventlist[0]->Reset();

      } else {
        // Add up each new plot
        fluxlist[0]->Add(fluxplot);
      }

      fluxplot->SetNameTitle(Form("nuwro_pdg%i_pct%f_Flux", pdg, pctg),
                             Form("nuwro_pdg%i_pct%f_Flux", pdg, pctg));

      TH1D *eventplot = (TH1D *)fluxplot->Clone();
      eventplot->SetNameTitle(Form("nuwro_pdg%i_pct%f_Evt", pdg, pctg),
                              Form("nuwro_pdg%i_pct%f_Evt", pdg, pctg));
      eventplot->Reset();

      fluxlist[pdg] = fluxplot;
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
  // double totaleventmode = 0.0;
  // double totalevents = 0.0;
  int pdg = 0;
  int countwidth = nevents / 50.0;
  countwidth = countwidth ? countwidth : 1;

  for (int i = 0; i < nevents; i++) {
    nuwrotree->GetEntry(i);

    // Get Variables
    Enu = evt->in[0].t / 1000.0;
    TotXSec = evt->weight;
    pdg = evt->in[0].pdg;

    eventlist[0]->Fill(Enu);
    eventlist[pdg]->Fill(Enu);

    nevtlist[0] += 1;
    nevtlist[pdg] += 1;

    intxseclist[0] += TotXSec;
    intxseclist[pdg] += TotXSec;

    if (i % countwidth == 0) {
      NUIS_LOG(FIT, "Processed " << i << " events "
                                 << " (" << int(i * 100.0 / nevents) << "%)"
                                 << " : E, W, PDG = " << Enu << ", " << TotXSec
                                 << ", " << pdg)
    }
  }

  TH1D *zeroevents = (TH1D *)eventlist[0]->Clone();

  outRootFile->cd();
  // Loop over eventlist
  for (uint i = 0; i < allpdg.size(); i++) {
    int pdg = allpdg[i];
    double AvgXSec = intxseclist[0] * 1E38 / double(nevtlist[0]);

    NUIS_LOG(FIT, pdg << " Avg XSec = " << AvgXSec);
    NUIS_LOG(FIT, pdg << " nevents = " << double(nevtlist[pdg]));

    if (!isMono) {
      // Convert events to PDF
      eventlist[pdg]->Scale(1.0 / zeroevents->Integral("width"));

      // Multiply by total predicted event rate
      eventlist[pdg]->Scale(fluxlist[0]->Integral("width") * AvgXSec);
    } else {
      // If a mono-energetic flux was used, width should not be used
      // The output is (now) forced to be flux = 1, evtrt = xsec (in 1E38 * nb
      // cm^2)
      eventlist[pdg]->Scale(1.0 / zeroevents->Integral());
      eventlist[pdg]->Scale(fluxlist[0]->Integral() * AvgXSec);
    }

    // Save everything
    fluxlist[pdg]->Write("", TObject::kOverwrite);
    eventlist[pdg]->Write("", TObject::kOverwrite);
  }

  // Tidy up
  outRootFile->Close();
  fluxlist.clear();
  eventlist.clear();

  // Exit Program
  return;
}

//*******************************
void HaddNuwroFiles(std::vector<std::string> &inputs, bool force_out) {
  //*******************************

  // Get output file name
  std::string outputname = inputs[0];

  // Make command line string
  std::string cmd = "hadd ";
  if (outputNewFile) {
    cmd += ofile + " ";
    outputname = ofile;
  } else if (force_out) {
    cmd += "-f ";
  }
  for (UInt_t i = 0; i < inputs.size(); i++) {
    cmd += inputs[i] + " ";
  }
  NUIS_LOG(FIT, " Running HADD from PrepareNuwro: " << cmd);

  // Start HADD
  system(cmd.c_str());

  // Return name of output file
  inputs.clear();
  inputs.push_back(outputname);
  haddedFiles = true;
  return;
}
