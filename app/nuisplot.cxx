#include "GeneralUtils.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TPad.h"

#include <sstream>
#include <string>

std::string inputfilename = "";
std::string outputfilename = "";
std::string title = "", xtitle = "", ytitle = "", ztitle = "";
bool titleset = false, xtitleset = false, ytitleset = false, ztitleset = false;
std::string MCTag = "MC", DataTag = "Data";
bool saychi2 = false;

std::string samplename = "";
std::string mchistname = "";
std::string datahistname = "";

void SayUsage(char const *argv[]) {
  std::cout
      << "[USAGE]: " << argv[0] << "\n"
      << "\t-i|--input <comparisons.root>        : Input file\n"
      << "\t-o|--output <outfilename.[png|pdf]>  : Output file\n"
      << "\t-m|--mc-tag <tag>                    : legend tag for MC series\n"
      << "\t-d|--data-tag <tag>                  : legend tag for data series\n"
      << "\t-t|--title <plot title>              : plot title\n"
      << "\t-s|--sample name[:mcname[:dataname]] : Sample name with optional "
         "specifications for the MC and data histogram names.\n"
      << "\t-x|--xaxis-title <title string>      : x axis title\n"
      << "\t-y|--yaxis-title <title string>      : y axis title\n"
      << "\t-z|--zaxis-title <title string>      : z axis title\n"
      << "\t--chi2                               : Output the chi2\n"
      << "\t-?|--help                            : Print this message\n"
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
      inputfilename = argv[++opt];
      std::cout << "[OPT]: Reading from " << inputfilename << std::endl;
    } else if ((std::string(argv[opt]) == "-o") ||
               (std::string(argv[opt]) == "--output")) {
      outputfilename = argv[++opt];
      std::cout << "[OPT]: Writing plot to " << outputfilename << std::endl;
    } else if ((std::string(argv[opt]) == "-t") ||
               (std::string(argv[opt]) == "--title")) {
      title = argv[++opt];
      titleset = true;
      std::cout << "[OPT]: Using " << title << " as plot title" << std::endl;
    } else if ((std::string(argv[opt]) == "-m") ||
               (std::string(argv[opt]) == "--mc-tag")) {
      MCTag = argv[++opt];
      std::cout << "[OPT]: Using " << MCTag << " as MC series name"
                << std::endl;
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

    } else if (std::string(argv[opt]) == "--chi2") {
      saychi2 = true;
    } else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }
}

int main(int argc, char const *argv[]) {

  HandleOpts(argc, argv);

  if (!samplename.length()) {
    std::cout << "[ERROR]: --sample option was not passed." << std::endl;
    SayUsage(argv);
    return 1;
  }

  if (!inputfilename.length()) {
    std::cout << "[ERROR]: --input option was not passed." << std::endl;
    SayUsage(argv);
    return 1;
  }

  if (!saychi2 && !outputfilename.size()) {
    std::cout << "[ERROR]: Must pass one of --output or --chi2." << std::endl;
    SayUsage(argv);
    return 1;
  }

  TFile f(inputfilename.c_str(), "READ");

  if (!f.IsOpen()) {
    std::cout << "[ERROR]: Failed to open input file: \"" << inputfilename
              << "\"" << std::endl;
    return 2;
  }

  TH1 *MC = nullptr;
  TH1 *Data = nullptr;

  f.GetObject(mchistname.c_str(), MC);
  f.GetObject(datahistname.c_str(), Data);

  if (!MC) {
    std::cout << "[ERROR]: Failed to get MC histogram : \"" << mchistname
              << "\" from file: \"" << inputfilename << "\"" << std::endl;
    return 2;
  }

  if (!Data) {
    std::cout << "[ERROR]: Failed to get Data histogram : \"" << datahistname
              << "\" from file: \"" << inputfilename << "\"" << std::endl;
    return 2;
  }

  std::string chi2 = MC->GetTitle();

  if (saychi2) {
    std::cout << "Chi2: " << chi2 << std::endl;
  }

  if (outputfilename.size()) {

    TCanvas c1("c1", "", 1200, 1200);

    if (titleset) {
      MC->SetTitle(title.c_str());
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

    if (ytitleset) {
      MC->GetYaxis()->SetTitle(ytitle.c_str());
    }

    MC->GetYaxis()->SetTitleSize(0.05);
    MC->GetYaxis()->SetLabelSize(0.05);
    MC->GetYaxis()->SetTitleOffset(1);
    MC->GetYaxis()->SetLabelOffset(0.01);

    double max_bin_value = std::max(MC->GetMaximum(), Data->GetMaximum());

    int ndims = MC->GetDimension();
    if (ndims == 1) {

      TPad *toppad = new TPad("toppad", "", 0, 0.35, 1, 0.85);
      toppad->SetBottomMargin(0.15);
      toppad->AppendPad();

      TPad *bottompad = new TPad("bottompad", "", 0, 0, 1, 0.35);
      bottompad->SetBottomMargin(0.175);
      bottompad->AppendPad();

      toppad->cd();

      MC->GetYaxis()->SetRangeUser(0, max_bin_value * 1.1);

      MC->SetLineColor(kRed);
      MC->SetLineWidth(2);
      MC->SetLineStyle(1);

      Data->SetLineColor(kBlack);
      Data->SetLineWidth(2);
      Data->SetLineStyle(1);
      Data->SetMarkerColor(kBlack);
      Data->SetMarkerSize(0.5);
      Data->SetMarkerStyle(20);

      MC->DrawClone("HIST");
      Data->DrawClone("SAME E1");

      bottompad->cd();

      MC->Divide(Data);

      MC->GetYaxis()->SetRangeUser(0,  2);
      std::stringstream ss("");
      ss << MCTag << "/" << DataTag;
      MC->GetYaxis()->SetTitle(ss.str().c_str());

      MC->GetXaxis()->SetTitleSize(0.05 * (0.5 / 0.35));
      MC->GetXaxis()->SetLabelSize(0.05 * (0.5 / 0.35));
      MC->GetXaxis()->SetTitleOffset(1);
      MC->GetXaxis()->SetLabelOffset(0.01);

      MC->GetYaxis()->SetTitleSize(0.05 * (0.5 / 0.35));
      MC->GetYaxis()->SetLabelSize(0.05 * (0.5 / 0.35));
      MC->GetYaxis()->SetTitleOffset(0.5);
      MC->GetYaxis()->SetLabelOffset(0.01);

      MC->DrawClone("HIST");

      c1.cd();

      TLegend *leg = new TLegend(0.1, 0.8, 0.9, 1);
      leg->SetBorderSize(0);

      ss.str("");
      ss << MCTag << ": #chi^{2} = " << chi2 << std::endl;

      leg->AddEntry(MC, ss.str().c_str(), "l");
      leg->AddEntry(Data, DataTag.c_str(), "lp");
      leg->Draw();

    } else if (ndims == 2) {
      if (ztitleset) {
        MC->GetZaxis()->SetTitle(ztitle.c_str());
      }
      MC->GetZaxis()->SetRangeUser(0, max_bin_value * 1.1);
    }

    c1.Print(outputfilename.c_str());
  }
}