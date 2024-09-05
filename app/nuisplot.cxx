#include "GeneralUtils.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TPad.h"
#include "TStyle.h"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> inputfilenames;
std::string outputfilename = "";
std::vector<std::string> titles;
std::string xtitle = "", ytitle = "", ztitle = "";
bool xtitleset = false, ytitleset = false, ztitleset = false;
std::vector<std::string> MCTags = {};

std::string DataTag = "Data";
bool saychi2 = false;

std::string samplename = "";
std::string mchistname = "";
std::string datahistname = "";

std::vector<std::pair<std::string, std::string> > slicelist;

std::vector<int> NXBinsToRemove;
std::vector<int> NYBinsToRemove;

bool variable_yscale = false;

bool split = false;
bool splity = false;

bool dump_to_numpcsv = false;

void SayUsage(char const *argv[]) {
  std::cout
      << "[USAGE]: " << argv[0] << "\n"
      << "\t-i|--input <comp.root> [<f2.root> ...] : Input file\n\n"
      << "\t-o|--output <outfilename.[png|pdf]>    : Output file\n\n"
      << "\t-m|--mc-tag <tag> [<tag2> ...]         : Legend tag(s) for the MC\n"
      << "\t                                         series plotted from one\n"
      << "\t                                         or more files passed to\n"
      << "\t                                         --input\n\n"
      << "\t-d|--data-tag <tag>                    : Legend tag for data.\n"
      << "\t                                         Data histograms will\n"
      << "\t                                         always be read from the\n"
      << "\t                                         first file passed to \n"
      << "\t                                         --input.\n\n"
      << "\t-t|--title <plot title> [<title> ...]  : Plot title(s)\n\n"
      << "\t-s|--sample <name[:mcname[:dataname]]> : Sample name with\n"
      << "\t                                         optional specifications \n"
      << "\t                                         for the MCand data\n"
      << "\t                                         histogram names.\n\n"
      << "\t-x|--xaxis-title <title string>        : x axis title\n\n"
      << "\t-y|--yaxis-title <title string>        : y axis title\n\n"
      << "\t-z|--zaxis-title <title string>        : z axis title\n\n"
      << "\t--bins-nodisplay <X|Y> <N>             : Do not plot the <N>\n"
      << "\t                                         highest X or Y bins\n\n"
      << "\t--split2D <X|Y>                        : Split a 2D histogram\n"
      << "\t                                         into multiple 1D\n"
      << "\t                                         histograms for display.\n"
      << "\t                                         Slice along X or Y \n"
      << "\t                                         depending on argument.\n\n"
      << "\t--slice-list <mcname:dataname> [<m:d>] ... \n"
      << "\t                                       : Pass a list of histogram\n"
      << "\t                                         names to plot. These can\n"
      << "\t                                         be used in cases where a\n"
      << "\t                                         multi-dimensional\n"
      << "\t                                         distribution is already\n"
      << "\t                                         split into 1D panes in\n"
      << "\t                                         the comparisons file.\n\n"
      << "\t--variable-yscale                      : For split histograms, \n"
      << "\t                                         set the yaxis range per\n"
      << "\t                                         split instead of using\n"
      << "\t                                         the global maximum.\n\n"
      << "\t--chi2                                 : Output the chi2\n\n"
      << "\t--np-csv                               : Dump plots to "
      << "\t                                         numpy-parseable csv "
      << "\t                                         strings instead of "
      << "\t                                         plotting them."
      << "\t-?|--help                              : Print this message\n\n"
      << std::endl;
}

void HandleOpts(int argc, char const *argv[]) {
  int opt = 1;
  while (opt < argc) {
    if (std::string(argv[opt]) == "-?" || std::string(argv[opt]) == "--help") {
      SayUsage(argv);
      exit(0);
    } else if ((std::string(argv[opt]) == "-i") ||
               (std::string(argv[opt]) == "--input")) {

      while (((opt + 1) != argc) && (argv[opt + 1][0] != '-')) {
        inputfilenames.push_back(argv[++opt]);
        std::cout << "[OPT]: Reading from " << inputfilenames.back()
                  << std::endl;
      }
    } else if ((std::string(argv[opt]) == "-o") ||
               (std::string(argv[opt]) == "--output")) {
      outputfilename = argv[++opt];
      std::cout << "[OPT]: Writing plot to " << outputfilename << std::endl;
    } else if ((std::string(argv[opt]) == "-t") ||
               (std::string(argv[opt]) == "--title")) {
      while (((opt + 1) != argc) && (argv[opt + 1][0] != '-')) {
        titles.push_back(argv[++opt]);
        std::cout << "[OPT]: Using " << titles.back() << " as plot title"
                  << std::endl;
      }
    } else if ((std::string(argv[opt]) == "-m") ||
               (std::string(argv[opt]) == "--mc-tag")) {

      while (((opt + 1) != argc) && (argv[opt + 1][0] != '-')) {
        MCTags.push_back(argv[++opt]);
        std::cout << "[OPT]: Using " << MCTags.back() << " as MC series name"
                  << std::endl;
      }

    } else if ((std::string(argv[opt]) == "-d") ||
               (std::string(argv[opt]) == "--data-tag")) {
      DataTag = argv[++opt];
      std::cout << "[OPT]: Using " << DataTag << " as Data series name"
                << std::endl;
    } else if ((std::string(argv[opt]) == "-s") ||
               (std::string(argv[opt]) == "--sample")) {
      auto splits = GeneralUtils::ParseToStr(argv[++opt], ":");

      if (!splits.size()) {
        std::cout << "[ERROR]: Could not parse argument\"" << argv[opt - 1]
                  << "\" to sample and hist names." << std::endl;
        abort();
      }

      samplename = splits[0];

      if (splits.size() < 2) {
        mchistname = samplename + "_MC";
        datahistname = samplename + "_data";
      } else if (splits.size() < 3) {
        mchistname = splits[1];
        datahistname = samplename + "_data";
      } else {
        mchistname = splits[1];
        datahistname = splits[2];
      }

      std::cout << "[OPT]: Plotting sample: " << samplename << ", MC("
                << mchistname << "), Data(" << datahistname << ")" << std::endl;

    } else if ((std::string(argv[opt]) == "-x") ||
               (std::string(argv[opt]) == "--xaxis-title")) {
      xtitle = argv[++opt];
      xtitleset = true;
      std::cout << "[OPT]: Using " << xtitle << " as X axis title" << std::endl;

    } else if ((std::string(argv[opt]) == "-y") ||
               (std::string(argv[opt]) == "--yaxis-title")) {
      ytitle = argv[++opt];
      ytitleset = true;
      std::cout << "[OPT]: Using " << ytitle << " as Y axis title" << std::endl;

    } else if ((std::string(argv[opt]) == "-z") ||
               (std::string(argv[opt]) == "--zaxis-title")) {
      ztitle = argv[++opt];
      ztitleset = true;
      std::cout << "[OPT]: Using " << ztitle << " as Z axis title" << std::endl;

    } else if (std::string(argv[opt]) == "--bins-nodisplay") {

      std::string XorY = argv[++opt];
      if (XorY[0] == 'X') {
        auto splits = GeneralUtils::ParseToStr(argv[++opt], ":");
        for (auto const &sp : splits) {
          NXBinsToRemove.push_back(std::strtol(sp.c_str(), nullptr, 10));
        }
      } else if (XorY[0] == 'Y') {
        auto splits = GeneralUtils::ParseToStr(argv[++opt], ":");
        for (auto const &sp : splits) {
          NYBinsToRemove.push_back(std::strtol(sp.c_str(), nullptr, 10));
        }
      } else {
        std::cout << "[ERROR]: --bins-nodisplay expected to be passed a first "
                     "argument of X or Y"
                  << std::endl;
        SayUsage(argv);
        exit(1);
      }

    } else if (std::string(argv[opt]) == "--split2D") {
      split = true;

      std::string XorY = argv[++opt];
      if (XorY[0] == 'X') {
        splity = false;
      } else if (XorY[0] == 'Y') {
        splity = true;
      } else {
        std::cout << "[ERROR]: --split2D expected to be passed an "
                     "argument of X or Y"
                  << std::endl;
        SayUsage(argv);
        exit(1);
      }
    } else if (std::string(argv[opt]) == "--slice-list") {
      while (((opt + 1) != argc) && (argv[opt + 1][0] != '-')) {
        auto splits = GeneralUtils::ParseToStr(argv[++opt], ":");

        if (splits.size() != 2) {
          std::cout << "[ERROR]: Could not parse argument\"" << argv[opt - 1]
                    << "\" to split hist names." << std::endl;
          abort();
        }

        slicelist.emplace_back(splits[0], splits[1]);
      }
    } else if (std::string(argv[opt]) == "--variable-yscale") {
      variable_yscale = true;
    } else if (std::string(argv[opt]) == "--chi2") {
      saychi2 = true;
    } else if (std::string(argv[opt]) == "--np-csv") {
      dump_to_numpcsv = true;
    } else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }
}

std::vector<double> Chi2s;
int NBins;
std::vector<std::vector<TH1 *> > MCHists;
std::vector<TH1 *> DataHists;
double MaxBinValue;

double GetMaximumBinPlusError(TH1 *h) {
  int maxbin = h->GetMaximumBin();
  return h->GetBinContent(maxbin) + h->GetBinError(maxbin);
}

std::vector<TH1 *> Split(TH1 *in) {
  if (in->GetDimension() != 2) {
    std::cout << "[ERROR]: Attempting to split a TH" << in->GetDimension()
              << ", can only apply to TH2s." << std::endl;
    abort();
  }

  TH2 *in2 = static_cast<TH2 *>(in);
  std::vector<TH1 *> rtnvect;
  if (splity) {
    // Can only lop off entire bins in the direction that we're slicing
    int NBinsToRemove = NYBinsToRemove.size() ? NYBinsToRemove[0] : 0;
    for (int i = 0; i < in2->GetYaxis()->GetNbins() - NBinsToRemove; ++i) {
      std::stringstream ss("");
      ss << in2->GetName() << "_px" << (i + 1);
      rtnvect.push_back(in2->ProjectionX(ss.str().c_str(), i + 1, i + 1, "e"));
      ss.str("");

      ss << in2->GetYaxis()->GetBinLowEdge(i + 1) << " < "
         << in2->GetYaxis()->GetTitle() << " < "
         << in2->GetYaxis()->GetBinUpEdge(i + 1);

      rtnvect.back()->SetTitle(ss.str().c_str());

      if (ztitleset) {
        rtnvect.back()->GetYaxis()->SetTitle(ztitle.c_str());
      }
    }
  } else {
    // Can only lop off entire bins in the direction that we're slicing
    int NBinsToRemove = NXBinsToRemove.size() ? NXBinsToRemove[0] : 0;
    for (int i = 0; i < in2->GetXaxis()->GetNbins() - NBinsToRemove; ++i) {
      std::stringstream ss("");
      ss << in2->GetName() << "_py" << (i + 1);
      rtnvect.push_back(in2->ProjectionY(ss.str().c_str(), i + 1, i + 1, "e"));
      ss.str("");

      ss << in2->GetXaxis()->GetBinLowEdge(i + 1) << " < "
         << in2->GetXaxis()->GetTitle() << " < "
         << in2->GetXaxis()->GetBinUpEdge(i + 1);

      rtnvect.back()->SetTitle(ss.str().c_str());

      if (ztitleset) {
        rtnvect.back()->GetYaxis()->SetTitle(ztitle.c_str());
      }
    }
  }
  return rtnvect;
}

void ReadHists() {

  int ctr = 0;
  for (auto const &fname : inputfilenames) {
    TFile *f = TFile::Open(fname.c_str(), "READ");

    if (!f->IsOpen()) {
      std::cout << "[ERROR]: Failed to open input file: \"" << fname << "\""
                << std::endl;
      abort();
    }

    TH1 *Data = nullptr;

    if (!DataHists.size()) { // Only read in the first set of data
      f->GetObject(datahistname.c_str(), Data);

      if (!Data) {
        std::cout << "[ERROR]: Failed to get Data histogram : \""
                  << datahistname << "\" from file: \"" << fname << "\""
                  << std::endl;
        abort();
      }

      Data->SetDirectory(nullptr);
      MaxBinValue = GetMaximumBinPlusError(Data);

      if (slicelist.size()) {
        // Read the split hists
        for (auto const &snamepair : slicelist) {
          TH1 *Dataslice = nullptr;
          f->GetObject(snamepair.second.c_str(), Dataslice);
          if (!Dataslice) {
            std::cout << "[ERROR]: Failed to get Data slice histogram : \""
                      << snamepair.second << "\" from file: \"" << fname << "\""
                      << std::endl;
            abort();
          }
          Dataslice->SetDirectory(nullptr);
          DataHists.push_back(Dataslice);
        }
      } else {

        if (split) {
          DataHists = Split(Data);
        } else {
          DataHists.push_back({
              Data,
          });
        }
      }
    }

    TH1 *MC = nullptr;

    f->GetObject(mchistname.c_str(), MC);

    if (!MC) {
      std::cout << "[ERROR]: Failed to get MC histogram : \"" << mchistname
                << "\" from file: \"" << fname << "\"" << std::endl;
      abort();
    }
    MC->SetDirectory(nullptr);
    Chi2s.push_back(std::strtod(MC->GetTitle(), nullptr));

    if (MC->GetDimension() == 1) {
      NBins = MC->GetNbinsX();
    } else if (MC->GetDimension() == 2) {
      TH2 *MC2 = static_cast<TH2 *>(MC);
      NBins = MC2->GetNbinsX() * MC2->GetNbinsY();
    } else {
      NBins = 0;
    }

    if (saychi2) {
      std::ofstream of((samplename + "_" + MCTags[ctr] + ".chi2").c_str());
      of << Chi2s.back() << "/" << NBins << std::endl;
    }
    MaxBinValue = std::max(GetMaximumBinPlusError(MC), MaxBinValue);

    if (slicelist.size()) {
      MCHists.emplace_back();
      // Read the split hists
      for (auto const &snamepair : slicelist) {
        TH1 *MCslice = nullptr;
        f->GetObject(snamepair.first.c_str(), MCslice);
        if (!MCslice) {
          std::cout << "[ERROR]: Failed to get MC slice histogram : \""
                    << snamepair.second << "\" from file: \"" << fname << "\""
                    << std::endl;
          abort();
        }
        MCslice->SetDirectory(nullptr);
        MCHists.back().push_back(MCslice);
      }
    } else {
      if (split) {
        MCHists.push_back(Split(MC));
      } else {
        MCHists.push_back({
            MC,
        });
      }
    }

    ctr++;
  }
}

void Plot1D(std::vector<TH1 *> MCs, TH1 *Data) {

  TCanvas c1("c1", "", 1200, 1200);

  TPad *toppad = new TPad("toppad", "", 0, 0.35, 1, 0.85);
  toppad->SetBottomMargin(0.15);
  toppad->SetLeftMargin(0.15);
  toppad->AppendPad();

  TPad *bottompad = new TPad("bottompad", "", 0, 0, 1, 0.35);
  bottompad->SetBottomMargin(0.175);
  bottompad->SetLeftMargin(0.15);
  bottompad->AppendPad();

  toppad->cd();

  int colorwheel[6] = {kRed, kBlue, kGreen, kMagenta, kOrange, kAzure};

  for (size_t i = 0; i < MCs.size(); ++i) {
    MCs[i]->SetLineColor(colorwheel[i % 6]);
    MCs[i]->SetFillColorAlpha(colorwheel[i % 6], 0.5);
    MCs[i]->SetLineWidth(2);
    MCs[i]->SetLineStyle(1);
    MCs[i]->SetMarkerSize(0);
    MCs[i]->SetMarkerStyle(0);
    MCs[i]->SetMarkerColorAlpha(colorwheel[i % 6], 0);
  }

  Data->SetLineColor(kBlack);
  Data->SetLineWidth(2);
  Data->SetLineStyle(1);
  Data->SetMarkerColor(kBlack);
  Data->SetMarkerSize(0.5);
  Data->SetMarkerStyle(20);

  for (size_t i = 0; i < MCs.size(); ++i) {
    MCs[i]->DrawClone(!i ? "E2" : "E2SAME");
    MCs[i]->SetFillColorAlpha(colorwheel[i % 6], 0);
    MCs[i]->DrawClone("HISTSAME");
  }
  Data->DrawClone("SAME E1");

  bottompad->cd();

  for (size_t i = 0; i < MCs.size(); ++i) {
    MCs[i]->Divide(Data);
  }

  MCs[0]->GetYaxis()->SetRangeUser(0, 2);
  std::stringstream ss("");
  ss << MCTags[0] << "/" << DataTag;
  MCs[0]->GetYaxis()->SetTitle(ss.str().c_str());

  MCs[0]->GetXaxis()->SetTitleSize(0.05 * (0.5 / 0.35));
  MCs[0]->GetXaxis()->SetLabelSize(0.05 * (0.5 / 0.35));
  MCs[0]->GetXaxis()->SetTitleOffset(0.75);
  MCs[0]->GetXaxis()->SetLabelOffset(0.01);

  MCs[0]->GetYaxis()->SetTitleSize(0.05 * (0.5 / 0.35));
  MCs[0]->GetYaxis()->SetLabelSize(0.05 * (0.5 / 0.35));
  MCs[0]->GetYaxis()->SetTitleOffset(0.75);
  MCs[0]->GetYaxis()->SetLabelOffset(0.01);

  for (size_t i = 0; i < MCs.size(); ++i) {
    MCs[i]->DrawClone(!i ? "HIST" : "HISTSAME");
  }

  c1.cd();

  TLegend *leg = new TLegend(0.1, 0.8, 0.9, 1);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  leg->AddEntry(Data, DataTag.c_str(), "lp");

  for (size_t i = 0; i < MCs.size(); ++i) {
    ss.str("");
    ss << MCTags[i] << ": #chi^{2} = " << Chi2s[i] << "/" << NBins << " bins"
       << std::endl;

    leg->AddEntry(MCs[i], ss.str().c_str(), "l");
  }

  leg->Draw();

  c1.Print(outputfilename.c_str());
}

void CSVDump1D(std::vector<TH1 *> MCs, TH1 *Data) {

  std::vector<std::string> colorwheel = {"red",     "blue",   "green",
                                         "magenta", "orange", "azure"};

  for (size_t i = 0; i < MCs.size(); ++i) {
    std::cout << "#" << MCTags[i] << ": #chi^{2} = " << Chi2s[i] << "/" << NBins
              << " bins" << std::endl;
    std::cout << "mc" << std::to_string(i) << R"(_csvstr =\
""")";
    for(int bi = 0; bi < MCs[i]->GetXaxis()->GetNbins(); ++bi){
      std::cout << "  " << MCs[i]->GetXaxis()->GetBinLowEdge(bi + 1) << ", "
                << MCs[i]->GetXaxis()->GetBinUpEdge(bi + 1) << ", "
                << MCs[i]->GetBinContent(bi + 1) << std::endl;
    }
    std::cout << R"(""")" << std::endl<< std::endl;
  }

  std::cout << "#" << DataTag << std::endl;
    std::cout << R"(data_csvstr =\
""")";
    for(int bi = 0; bi < Data->GetXaxis()->GetNbins(); ++bi){
      std::cout << "  " << Data->GetXaxis()->GetBinLowEdge(bi + 1) << ", "
                << Data->GetXaxis()->GetBinUpEdge(bi + 1) << ", "
                << Data->GetBinContent(bi + 1) << ", "
                << Data->GetBinError(bi + 1) << std::endl;
    }
    std::cout << R"(""")" << std::endl<< std::endl;
}

void Plot2D(std::vector<TH1 *> MC, TH1 *Data) {}

void PlotSingleHist() {

  TH1 *MC = MCHists[0][0];
  TH1 *Data = DataHists[0];

  if (titles.size()) {
    MC->SetTitle(titles[0].c_str());
  } else {
    MC->SetTitle("");
  }

  if (xtitleset) {
    MC->GetXaxis()->SetTitle(xtitle.c_str());
  }

  MC->GetXaxis()->SetTitleSize(0.05);
  MC->GetXaxis()->SetLabelSize(0.05);
  MC->GetXaxis()->SetTitleOffset(1);
  MC->GetXaxis()->SetLabelOffset(0.01);
  MC->GetXaxis()->SetNdivisions(505);

  if (NXBinsToRemove.size()) {
    double lowbinedge = MC->GetXaxis()->GetBinLowEdge(1);
    double upbinedge = MC->GetXaxis()->GetBinUpEdge(MC->GetXaxis()->GetNbins() -
                                                    NXBinsToRemove[0]);

    MC->GetXaxis()->SetRangeUser(lowbinedge, upbinedge);
  }

  if (ytitleset) {
    MC->GetYaxis()->SetTitle(ytitle.c_str());
  }

  MC->GetYaxis()->SetTitleSize(0.05);
  MC->GetYaxis()->SetLabelSize(0.05);
  MC->GetYaxis()->SetTitleOffset(1);
  MC->GetYaxis()->SetLabelOffset(0.01);
  MC->GetYaxis()->SetNdivisions(505);

  if (NYBinsToRemove.size()) {
    double lowbinedge = MC->GetYaxis()->GetBinLowEdge(1);
    double upbinedge = MC->GetYaxis()->GetBinUpEdge(MC->GetYaxis()->GetNbins() -
                                                    NYBinsToRemove[0]);

    MC->GetYaxis()->SetRangeUser(lowbinedge, upbinedge);
  }

  std::vector<TH1 *> MCSingleHists;
  for (auto const &MCHistList : MCHists) {
    MCSingleHists.push_back(MCHistList[0]);
  }

  int ndims = MC->GetDimension();
  if (ndims == 1) {
    MC->GetYaxis()->SetRangeUser(0, MaxBinValue * 1.1);
    Plot1D(MCSingleHists, Data);
  } else if (ndims == 2) {
    if (ztitleset) {
      MC->GetZaxis()->SetTitle(ztitle.c_str());
    }
    MC->GetZaxis()->SetRangeUser(0, MaxBinValue * 1.1);
    Plot2D(MCSingleHists, Data);
  }
}

void CSVSingleHist() {

  TH1 *MC = MCHists[0][0];
  TH1 *Data = DataHists[0];

  if (titles.size()) {
    MC->SetTitle(titles[0].c_str());
  } else {
    MC->SetTitle("");
  }

  if (xtitleset) {
    MC->GetXaxis()->SetTitle(xtitle.c_str());
  }

  MC->GetXaxis()->SetTitleSize(0.05);
  MC->GetXaxis()->SetLabelSize(0.05);
  MC->GetXaxis()->SetTitleOffset(1);
  MC->GetXaxis()->SetLabelOffset(0.01);
  MC->GetXaxis()->SetNdivisions(505);

  if (NXBinsToRemove.size()) {
    double lowbinedge = MC->GetXaxis()->GetBinLowEdge(1);
    double upbinedge = MC->GetXaxis()->GetBinUpEdge(MC->GetXaxis()->GetNbins() -
                                                    NXBinsToRemove[0]);

    MC->GetXaxis()->SetRangeUser(lowbinedge, upbinedge);
  }

  if (ytitleset) {
    MC->GetYaxis()->SetTitle(ytitle.c_str());
  }

  MC->GetYaxis()->SetTitleSize(0.05);
  MC->GetYaxis()->SetLabelSize(0.05);
  MC->GetYaxis()->SetTitleOffset(1);
  MC->GetYaxis()->SetLabelOffset(0.01);
  MC->GetYaxis()->SetNdivisions(505);

  if (NYBinsToRemove.size()) {
    double lowbinedge = MC->GetYaxis()->GetBinLowEdge(1);
    double upbinedge = MC->GetYaxis()->GetBinUpEdge(MC->GetYaxis()->GetNbins() -
                                                    NYBinsToRemove[0]);

    MC->GetYaxis()->SetRangeUser(lowbinedge, upbinedge);
  }

  std::vector<TH1 *> MCSingleHists;
  for (auto const &MCHistList : MCHists) {
    MCSingleHists.push_back(MCHistList[0]);
  }

  int ndims = MC->GetDimension();
  if (ndims == 1) {
    MC->GetYaxis()->SetRangeUser(0, MaxBinValue * 1.1);
    CSVDump1D(MCSingleHists, Data);
  } else if (ndims == 2) {
    NUIS_ABORT("Can currently only dump 1D histograms to CSV.");
  }
}

void PlotSlices() {

  // How many panes do we need
  size_t nslices = DataHists.size();

  std::vector<int> NBinsToRemove =
      (splity || !split) ? NXBinsToRemove : NYBinsToRemove;
  if (NBinsToRemove.size()) {
    while (NBinsToRemove.size() < nslices) {
      NBinsToRemove.push_back(NBinsToRemove.back());
    }
  }

  // don't get more than 3 wide
  size_t nx = nslices > 16 ? 4 : std::floor(std::sqrt(double(nslices)));
  size_t ny = std::ceil(double(nslices) / double(nx));

  std::cout << "[INFO]: Build " << nx << "x" << ny << " pads for " << nslices
            << std::endl;

  if ((nx * ny) < nslices) {
    std::cout << "[ERROR]: With " << nslices
              << " total, we calculated that we need nx = " << nx
              << ", ny = " << ny << ", total = " << (nx * ny) << " panes."
              << std::endl;
  }

  double ybuffer = 0;

  if ((nx * ny) ==
      nslices) { // if we have a full house, put the legend at the top
    ybuffer = 0.15;
  }

  double padxwidth = 1.0 / double(nx);
  double padywidth = (1.0 - ybuffer) / double(ny);

  TCanvas c1("c1", "", nx * 400, (ny * 400) * (1 + ybuffer));

  std::stringstream ss("");
  for (size_t i = 0; i < nslices; ++i) {
    int ix = i % nx;
    int iy = i / nx;

    double px1 = ix * padxwidth;
    double px2 = (ix + 1) * padxwidth;
    double py1 = (1.0 - ybuffer) - ((iy + 1) * padywidth);
    double py2 = (1.0 - ybuffer) - (iy * padywidth);

    std::cout << "[INFO]: Slice " << i << ", ix = " << ix << ", iy = " << iy
              << " Pad = (" << px1 << ", " << py1 << ", " << px2 << ", " << py2
              << ")" << std::endl;

    ss.str("");
    ss << "p" << i;
    TPad *p = new TPad(ss.str().c_str(), "", px1, py1, px2, py2);
    p->SetBottomMargin(0.15);
    p->SetLeftMargin(0.15);
    p->AppendPad();
    p->cd();

    int colorwheel[6] = {kRed, kBlue, kGreen, kMagenta, kOrange, kAzure};

    if (NBinsToRemove.size()) {
      double lowbinedge = MCHists[0][i]->GetXaxis()->GetBinLowEdge(1);
      double upbinedge = MCHists[0][i]->GetXaxis()->GetBinUpEdge(
          MCHists[0][i]->GetXaxis()->GetNbins() - NBinsToRemove[i]);

      MCHists[0][i]->GetXaxis()->SetRangeUser(lowbinedge, upbinedge);
    }

    if (!variable_yscale) {
      MCHists[0][i]->GetYaxis()->SetRangeUser(0, MaxBinValue);
    } else {
      double localMaxBinValue = GetMaximumBinPlusError(DataHists[i]);
      for (size_t si = 0; si < MCHists.size(); ++si) {
        localMaxBinValue =
            std::max(localMaxBinValue, GetMaximumBinPlusError(MCHists[si][i]));
      }
      MCHists[0][i]->GetYaxis()->SetRangeUser(0, localMaxBinValue);
    }

    MCHists[0][i]->GetXaxis()->SetNdivisions(505);
    MCHists[0][i]->GetYaxis()->SetNdivisions(505);

    if (titles.size()) {
      MCHists[0][i]->SetTitle(
          ((i >= titles.size()) ? titles.back() : titles[i]).c_str());
    }

    for (size_t si = 0; si < MCHists.size(); ++si) {
      MCHists[si][i]->SetLineColor(colorwheel[si % 6]);
      MCHists[si][i]->SetFillColorAlpha(colorwheel[si % 6], 0.5);
      MCHists[si][i]->SetLineWidth(2);
      MCHists[si][i]->SetLineStyle(1);
      MCHists[si][i]->SetMarkerSize(0);
      MCHists[si][i]->SetMarkerStyle(0);
      MCHists[si][i]->SetMarkerColorAlpha(colorwheel[i % 6], 0);
    }

    DataHists[i]->SetLineColor(kBlack);
    DataHists[i]->SetLineWidth(2);
    DataHists[i]->SetLineStyle(1);
    DataHists[i]->SetMarkerColor(kBlack);
    DataHists[i]->SetMarkerSize(0.5);
    DataHists[i]->SetMarkerStyle(20);

    for (size_t si = 0; si < MCHists.size(); ++si) {
      MCHists[si][i]->DrawClone(!si ? "E2" : "E2SAME");
      MCHists[si][i]->SetFillColorAlpha(colorwheel[si % 6], 0);
      MCHists[si][i]->DrawClone("HISTSAME");
    }
    DataHists[i]->DrawClone("SAME E1");

    c1.cd();
  }

  TLegend *leg = nullptr;

  if (ybuffer > 0) {
    leg = new TLegend(0.1, 0.85, 0.9, 1);
    leg->SetTextSize(0.04);
  } else { // Put the legend in the place of the last pane(s)
    leg = new TLegend((nslices % nx) * padxwidth, 1 - (ny * padywidth),
                      nx * padxwidth, 1 - ((ny - 1) * padywidth));
    leg->SetTextSize(0.02 * float((nx * ny) - nslices));
  }

  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(DataHists[0], DataTag.c_str(), "lp");

  for (size_t i = 0; i < MCHists.size(); ++i) {
    ss.str("");
    ss << MCTags[i] << ": #chi^{2} = " << Chi2s[i] << "/" << NBins << " bins"
       << std::endl;

    leg->AddEntry(MCHists[i][0], ss.str().c_str(), "l");
  }

  leg->Draw();

  c1.Print(outputfilename.c_str());
}

int main(int argc, char const *argv[]) {

  gStyle->SetOptStat(false);

  HandleOpts(argc, argv);

  if (!samplename.length()) {
    std::cout << "[ERROR]: --sample option was not passed." << std::endl;
    SayUsage(argv);
    return 1;
  }

  if (!inputfilenames.size()) {
    std::cout << "[ERROR]: --input option was not passed." << std::endl;
    SayUsage(argv);
    return 1;
  }

  if (!outputfilename.size()) {
    outputfilename = samplename + ".pdf";
  }

  while (MCTags.size() < inputfilenames.size()) {
    MCTags.push_back("MC");
  }

  ReadHists();
  if (DataHists.size() > 1) {
    if (dump_to_numpcsv) {
      NUIS_ABORT("Cannot yet dump sliced histograms to numpy csv.");
    }
    PlotSlices();
  } else {
    if (dump_to_numpcsv) {
      CSVSingleHist();
    } else {
      PlotSingleHist();
    }
  }
}