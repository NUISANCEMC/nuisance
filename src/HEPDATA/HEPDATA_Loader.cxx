// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
 *    This file is part of NUISANCE.
 *
 *    NUISANCE is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    NUISANCE is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
#include "HEPDATA_Loader.h"
#include "nuiscling.h"
#include "yaml-cpp/yaml.h"

using namespace YAML;

struct HepDataDocument {
  std::string name;
  std::string data_file;
  std::string description;
};

namespace YAML {
template <> struct convert<HepDataDocument> {

  static bool decode(const Node &node, HepDataDocument &rhs) {

    rhs.name.clear();
    rhs.data_file.clear();
    rhs.description.clear();

    // Return empty if no node
    if (!node)
      return 1;
    if (!node["name"])
      return 1;

    if (node["name"])
      rhs.name = node["name"].as<std::string>();
    if (node["data_file"])
      rhs.data_file = node["data_file"].as<std::string>();
    // if (node["description"]) rhs.description =
    // node["description"].as<std::string>();

    return 1;
  };
};
}; // namespace YAML

struct HepDataVariables {
  std::vector<double> values;
  std::vector<double> low;
  std::vector<double> high;
  std::vector<double> center;
  std::vector<double> edges;
  std::map<std::string, std::string> qualifiers;
  bool valid = 0;
  std::string name;
  std::string units;
  std::string title;
  int n = 0;
};

namespace YAML {
template <> struct convert<HepDataVariables> {

  static bool decode(const Node &node, HepDataVariables &rhs) {

    rhs.values.clear();
    rhs.low.clear();
    rhs.high.clear();
    rhs.center.clear();
    rhs.edges.clear();
    rhs.valid = 0;

    // Return empty if no node
    if (!node)
      return 1;

    rhs.name = node["header"]["name"].as<std::string>();
    rhs.title = rhs.name;
    if (node["units"]) {
      rhs.units = node["header"]["units"].as<std::string>();
      rhs.title += "[" + rhs.units + "]";
    }

    YAML::Node xvalues = node["values"];
    rhs.n = xvalues.size();

    if (rhs.n == 0)
      return 1;

    rhs.values.resize(xvalues.size());
    rhs.low.resize(xvalues.size());
    rhs.high.resize(xvalues.size());
    rhs.center.resize(xvalues.size());
    rhs.edges.resize(xvalues.size() + 1);

    double NULL_ENTRY = -999;
    for (int i = 0; i < xvalues.size(); i++) {

      rhs.values[i] =
          xvalues[i]["value"] ? xvalues[i]["value"].as<double>() : NULL_ENTRY;
      rhs.low[i] =
          xvalues[i]["low"] ? xvalues[i]["low"].as<double>() : NULL_ENTRY;
      rhs.high[i] =
          xvalues[i]["high"] ? xvalues[i]["high"].as<double>() : NULL_ENTRY;
      rhs.center[i] =
          xvalues[i]["center"] ? xvalues[i]["center"].as<double>() : NULL_ENTRY;
      rhs.edges[i] =
          xvalues[i]["low"] ? xvalues[i]["low"].as<double>() : NULL_ENTRY;
    }
    int il = xvalues.size() - 1;
    rhs.edges[il + 1] =
        xvalues[il]["high"] ? xvalues[il]["high"].as<double>() : NULL_ENTRY;

    if (node["qualifiers"]) {
      YAML::Node node_qualifiers = node["qualifiers"];
      for (int i = 0; i < node_qualifiers.size(); i++) {
        if (node_qualifiers[i]["name"] && node_qualifiers[i]["value"]) {
          std::string name = node_qualifiers[i]["name"].as<std::string>();
          std::string value = node_qualifiers[i]["value"].as<std::string>();
          rhs.qualifiers[name] = value;
        }
      }
    }

    rhs.valid = 1;
    return 1;
  };
};
}; // namespace YAML

// // Loop through and find correct data table
//   std::string datafile = "";
//   std::string dataname = "";
//   for (std::size_t i = 0; i < docs.size(); i++) {
//     YAML::Node doc = docs[i];
//     curdoc = doc;
//     for (YAML::const_iterator it = doc.begin(); it != doc.end(); ++it) {
//       std::string key = it->first.as<std::string>(); // <- key
//       if (!key.compare("name")) {
//         std::string value = it->second.as<std::string>(); // <- key
//         if (!value.compare(table)) {
//           curdoc = Clone(doc);
//           datafile = doc["data_file"].as<std::string>();
//           dataname = doc["name"].as<std::string>();
//           break;
//         }
//       }
//     }
//   }

std::string PrintNodeType(YAML::Node mynode) {

  switch (mynode.Type()) {
  case NodeType::Null:
    return "NULL";
  case NodeType::Scalar:
    return "Scaler";
  case NodeType::Sequence:
    return "Sequence";
  case NodeType::Map:
    return "Map";
  case NodeType::Undefined:
    return "Undef";
  }

  return "UNKNOWN";
};

//********************************************************************
HEPDATA_Loader::HEPDATA_Loader(nuiskey samplekey) {
  //********************************************************************

  // Setup common settings
  std::cout << "[INFO]: Loading Sample Settings" << std::endl;
  fSettings = LoadSampleSettings(samplekey);
  std::string release = samplekey.GetS("release");
  std::string filename = release + "/submission.yaml";
  std::string table = samplekey.GetS("table");
  std::string snippet_file = samplekey.GetS("snippet_file");
  std::string covarkey = samplekey.Has("covar") ? samplekey.GetS("covar") : "";

  // Convert HEPDATA YAML submission to map of documents
  std::cout << "[INFO]: Loading YAML Docs for " << filename << std::endl;
  std::vector<YAML::Node> yaml_docs = YAML::LoadAllFromFile(filename);
  std::map<std::string, HepDataDocument> hepdata_docs;
  for (std::size_t i = 0; i < yaml_docs.size(); i++) {
    HepDataDocument cur_hepdatadoc = yaml_docs[i].as<HepDataDocument>();
    hepdata_docs[cur_hepdatadoc.name] = cur_hepdatadoc;
  }

  // Check requested name is inside.
  if (hepdata_docs.find(table) == hepdata_docs.end()) {
    std::cerr << "HepData Table not found : " << table << std::endl;
    std::cerr << " - [ Available Tables ]" << std::endl;
    for (auto const &it : hepdata_docs) {
      std::cerr << " - " << (it.first) << std::endl;
    }
  }

  // Now get individual table info
  std::cout << "[INFO]: Parsing YAML Table for " << table << std::endl;
  HepDataDocument measurement_doc = hepdata_docs[table];
  std::string datafile = measurement_doc.data_file;
  std::string dataname = measurement_doc.name;
  fSettings.SetTitle(measurement_doc.name);
  fSettings.SetDescription(measurement_doc.description);

  YAML::Node doc = YAML::LoadFile(samplekey.GetS("release") + "/" + datafile);
  YAML::Node indep_var = doc["independent_variables"];
  HepDataVariables dimension1 = indep_var[0].as<HepDataVariables>();
  HepDataVariables dimension2 = indep_var[1].as<HepDataVariables>();
  HepDataVariables dimension3 = indep_var[2].as<HepDataVariables>();

  YAML::Node dep_var = doc["dependent_variables"];
  HepDataVariables entries = dep_var[0].as<HepDataVariables>();

  // Fill fSettings from Y Qualifiers for this table
  std::cout << "[INFO]: Populating Settings from Qualifiers" << std::endl;
  YAML::Node qualifiers = doc["dependent_variables"][0]["qualifiers"];
  for (auto const &q : entries.qualifiers) {
    std::cout << " -- " << q.first << " = " << q.second << std::endl;
    fSettings.SetDefault(q.first, q.second);
  }

  // YAML::Node qualifiers = doc["dependent_variables"][0]["qualifiers"];
  // for (int i = 0; i < qualifiers.size(); i++) {
  //   std::string name = qualifiers[i]["name"].as<std::string>();
  //   std::string value = qualifiers[i]["value"].as<std::string>();
  //   fSettings.SetDefault(name, value);
  // }

  // Now do nuicling black magic - oooOOOoooooOOOOOoooh
  std::cout << "[INFO]: Parsing snippet file: " << snippet_file << std::endl;
  if (!nuiscling::Get().LoadFile(snippet_file)) {
    NUIS_ABORT("Failed to process snippet file.");
  }

  // Define Signal Filter
  if (!fSettings.Has("Filter")) {
    std::cout << "Known Qualifiers: " << std::endl;
    for (auto const &q : entries.qualifiers) {
      std::cout << " -- " << q.first << " = " << q.second << std::endl;
    }

    NUIS_ABORT("Could not find qualifier named: Filter for filter function "
               "definition.");
  }

  filter_func = nuiscling::Get().GetFilterFunction(fSettings.GetS("Filter"));
  if (!filter_func) {
    NUIS_ABORT("Failed to find required function "
               << fSettings.GetS("Filter")
               << " after interpreting snippet_file: " << snippet_file)
  }

  // Determine dimension of class
  fHEPDATA_NDIM =
      (int)dimension1.valid + (int)dimension2.valid + (int)dimension3.valid;
  if (fHEPDATA_NDIM == 1) {
    // Measurement 1D uses normal nuisance classes
    fDataHist = new TH1D(
        dataname.c_str(),
        (dataname + ";" + dimension1.title + ";" + entries.title).c_str(),
        dimension1.n, &dimension1.edges[0]);

    for (int i = 0; i < entries.values.size(); i++) {
      fDataHist->SetBinContent(i + 1, entries.values[i]);
    }
    fSettings.SetXTitle(dimension1.title);
    fSettings.SetYTitle(entries.title);

  } else if (fHEPDATA_NDIM == 2) {

    // dimension1.n still gives total number of bins across full 2D space
    fDataHist =
        new TH1D(dataname.c_str(), (dataname + ";BIN;" + entries.title).c_str(),
                 dimension1.n, 0.0, (double)dimension1.n);

    for (int i = 0; i < entries.values.size(); i++) {
      fDataHist->SetBinContent(i + 1, entries.values[i]);
    }
    fSettings.SetXTitle(dimension1.title);
    fSettings.SetYTitle(entries.title);

    // Now create a seperate TH2Poly to play with
    fDataHist_TH2Poly = new TH2Poly();

    for (int i = 0; i < entries.values.size(); i++) {
      double xlow = dimension1.low[i];
      double xhigh = dimension1.high[i];
      double ylow = dimension2.low[i];
      double yhigh = dimension2.high[i];
      double vals = entries.values[i];

      fDataHist_TH2Poly->AddBin(xlow, ylow, xhigh, yhigh);
      fDataHist_TH2Poly->SetBinContent(i, fDataHist->GetBinContent(i + 1));
    }
    fDataHist_TH2Poly->GetXaxis()->SetTitle(dimension1.name.c_str());
    fDataHist_TH2Poly->GetYaxis()->SetTitle(dimension2.name.c_str());
    fDataHist_TH2Poly->GetZaxis()->SetTitle(entries.name.c_str());

    fMCHist_TH2Poly = (TH2Poly *)fDataHist_TH2Poly->Clone("fMCHist_TH2Poly");
  } else if (fHEPDATA_NDIM == 3) {

    fDataHist =
        new TH1D(dataname.c_str(), (dataname + ";BIN;" + entries.title).c_str(),
                 dimension1.n, 0.0, (double)dimension1.n);

    for (int i = 0; i < entries.values.size(); i++) {
      fDataHist->SetBinContent(i + 1, entries.values[i]);
    }

    fSettings.SetXTitle(dimension1.title);
    fSettings.SetYTitle(entries.title);

    std::vector<double> XBins = {
        dimension1.low[0],
    };
    std::vector<double> YBins = {
        dimension2.low[0],
    };
    std::vector<double> ZBins = {
        dimension3.low[0],
    };

    // assume contiguous/monotonically increasing
    for (int i = 0; i < entries.values.size(); i++) {

      bool bin_added = false;

      if (dimension1.high[i] >
          XBins.back()) { // while we are monotonically increasing
        XBins.push_back(dimension1.high[i]);
        bin_added = true;
        std::cout << "Added X(" << dimension1.name
                  << ") bin: " << XBins[XBins.size() - 2] << " -- "
                  << XBins.back() << std::endl;
      }

      if (dimension2.high[i] >
          YBins.back()) { // while we are monotonically increasing
        YBins.push_back(dimension2.high[i]);
        bin_added = true;
        std::cout << "Added Y(" << dimension2.name
                  << ") bin: " << YBins[YBins.size() - 2] << " -- "
                  << YBins.back() << std::endl;
      }

      if (dimension3.high[i] >
          ZBins.back()) { // while we are monotonically increasing
        ZBins.push_back(dimension3.high[i]);
        bin_added = true;
        std::cout << "Added Z(" << dimension3.name
                  << ") bin: " << ZBins[ZBins.size() - 2] << " -- "
                  << ZBins.back() << std::endl;
      }
    }

    int NXBins = int(XBins.size() - 1);
    int NYBins = int(YBins.size() - 1);
    int NZBins = int(ZBins.size() - 1);

    fDataHist_TH3 = new TH3D(
        (dataname + "_3D").c_str(),
        (";" + dimension1.name + ";" + dimension2.name + ";" + dimension3.name)
            .c_str(),
        NXBins, XBins.data(), NYBins, YBins.data(), NZBins, ZBins.data());

    fMCHist_TH3 = (TH3 *)fDataHist_TH3->Clone("fMCHist_TH3");

    int gbin = 0;
    for (int k = 0; k < NZBins; k++) {
      for (int j = 0; j < NYBins; j++) {
        for (int i = 0; i < NXBins; i++) {
          if (GBins.size() < NXBins) {
            GBins.resize(NXBins);
          }
          if (GBins[i].size() < NYBins) {
            GBins[i].resize(NYBins);
          }
          if (GBins[i][j].size() < NZBins) {
            GBins[i][j].resize(NZBins);
          }
          fDataHist_TH3->SetBinContent(i + 1, j + 1, k + 1,
                                       entries.values[gbin]);
          GBins[i][j][k] = gbin++;
        }
      }
    }

    if (gbin != entries.values.size()) {
      NUIS_ABORT("Built " << gbin << " global bins, but only have "
                          << entries.values.size() << " HEPData data points.");
    }

  } else {
    NUIS_ABORT("Cannot currently process tables with "
               << fHEPDATA_NDIM << " independent variables.");
  }

  if (fHEPDATA_NDIM >= 1) {
    if (!fSettings.Has(dimension1.name)) {
      std::cout << "Known Qualifiers: " << std::endl;
      for (int i = 0; i < qualifiers.size(); i++) {
        std::cout << "\t" << qualifiers[i]["name"].as<std::string>() << ": "
                  << qualifiers[i]["value"].as<std::string>() << std::endl;
      }
      NUIS_ABORT("Could not find qualifier named: "
                 << dimension1.name << " for projection function definition.");
    }

    auto proj_func =
        nuiscling::Get().GetProjectionFunction(fSettings.GetS(dimension1.name));
    if (!proj_func) {
      NUIS_ABORT("Failed to find required function "
                 << fSettings.GetS(dimension1.name)
                 << " after interpreting snippet_file: " << snippet_file)
    }
    projection_funcs.push_back(proj_func);
  }

  if (fHEPDATA_NDIM >= 2) {
    if (!fSettings.Has(dimension2.name)) {
      std::cout << "Known Qualifiers: " << std::endl;
      for (int i = 0; i < qualifiers.size(); i++) {
        std::cout << "\t" << qualifiers[i]["name"].as<std::string>() << ": "
                  << qualifiers[i]["value"].as<std::string>() << std::endl;
      }
      NUIS_ABORT("Could not find qualifier named: "
                 << dimension2.name << " for projection function definition.");
    }

    auto proj_func =
        nuiscling::Get().GetProjectionFunction(fSettings.GetS(dimension2.name));
    if (!proj_func) {
      NUIS_ABORT("Failed to find required function "
                 << fSettings.GetS(dimension2.name)
                 << " after interpreting snippet_file: " << snippet_file)
    }
    projection_funcs.push_back(proj_func);
  }

  if (fHEPDATA_NDIM >= 3) {
    if (!fSettings.Has(dimension3.name)) {
      std::cout << "Known Qualifiers: " << std::endl;
      for (int i = 0; i < qualifiers.size(); i++) {
        std::cout << "\t" << qualifiers[i]["name"].as<std::string>() << ": "
                  << qualifiers[i]["value"].as<std::string>() << std::endl;
      }
      NUIS_ABORT("Could not find qualifier named: "
                 << dimension3.name << " for projection function definition.");
    }

    auto proj_func =
        nuiscling::Get().GetProjectionFunction(fSettings.GetS(dimension3.name));
    if (!proj_func) {
      NUIS_ABORT("Failed to find required function "
                 << fSettings.GetS(dimension3.name)
                 << " after interpreting snippet_file: " << snippet_file)
    }
    projection_funcs.push_back(proj_func);
  }

  // Covariance Setup -----------
  if (!covarkey.empty()) {

    YAML::Node covar_doc = YAML::LoadFile(covarkey);

    HepDataVariables covar_vals =
        covar_doc["dependent_variables"][0].as<HepDataVariables>();
    HepDataVariables covar_dim1 =
        covar_doc["independent_variables"][0].as<HepDataVariables>();
    HepDataVariables covar_dim2 =
        covar_doc["independent_variables"][1].as<HepDataVariables>();

    // Covariance is assumed to be square, so matrix size is sqrt that.
    fFullCovar = new TMatrixDSym(sqrt(covar_vals.n));
    for (int i = 0; i < covar_vals.n; i++) {
      int bin1 = covar_dim1.values[i];
      int bin2 = covar_dim2.values[i];
      double cov = covar_vals.values[i];
      (*fFullCovar)(bin2, bin1) = cov;
    }
    std::cout << "[INFO] Loaded a full covariance size : " << covar_vals.n
              << std::endl;
  }

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor is bodged based on dependent units
  fScaleFactor = GetEventHistogram()->Integral("width") / (double(fNEvents));

  if (!entries.units.compare("/GeV")) {
    fScaleFactor = GetEventHistogram()->Integral("width") /
                   (double(fNEvents) * TotalIntegratedFlux("width"));
  }

  if (!entries.units.compare("10^{-38}")) {
    fScaleFactor *= 1E38;
  }

  // Plot Setup -------------------------------------------------------
  // Hard Coded YAML File Histogram Setting
  if (!fSettings.GetS("error_flag").compare("Possion")) {
    SetPoissonErrors();
    SetCovarFromDiagonal();
  }

  if (!fSettings.GetS("error_flag").compare("Uncorrelated")) {
    SetCovarFromDiagonal();
  }

  // Final setup  ---------------------------------------------------
  FinaliseSampleSettings();
  FinaliseMeasurement();
}

//********************************************************************
void HEPDATA_Loader::FillEventVariables(FitEvent *event) {
  //********************************************************************

  // Hack for 1D to 2D.
  if (fHEPDATA_NDIM == 1) {
    fXVar = projection_funcs[0](event);
  } else if (fHEPDATA_NDIM == 2) {
    double x = projection_funcs[0](event);
    double y = projection_funcs[1](event);
    fXVar = fDataHist_TH2Poly->FindBin(x, y) + 0.5; // Convert to a bin index
  } else if (fHEPDATA_NDIM == 3) {
    double x = projection_funcs[0](event);
    double y = projection_funcs[1](event);
    double z = projection_funcs[2](event);

    int xb = fDataHist_TH3->GetXaxis()->FindFixBin(x);
    int yb = fDataHist_TH3->GetYaxis()->FindFixBin(y);
    int zb = fDataHist_TH3->GetZaxis()->FindFixBin(z);

    // std::cout << "Projection: " << fDataHist_TH3->GetXaxis()->GetTitle() <<
    // ": "
    //           << x << ", " << fDataHist_TH3->GetYaxis()->GetTitle() << ": "
    //           << y
    //           << ", " << fDataHist_TH3->GetZaxis()->GetTitle() << ": " << z
    //           << " => {" << xb << ", " << yb << ", " << zb << " }" <<
    //           std::endl;

    if ((xb == 0) || (yb == 0) || (zb == 0)) {
      fXVar = -1;
    } else if ((xb == (fDataHist_TH3->GetXaxis()->GetNbins() + 1)) ||
               (yb == (fDataHist_TH3->GetYaxis()->GetNbins() + 1)) ||
               (zb == (fDataHist_TH3->GetZaxis()->GetNbins() + 1))) {
      fXVar = fDataHist->GetXaxis()->GetNbins() + 1;
    } else {
      fXVar = GBins[xb - 1][yb - 1][zb - 1] + 0.5; // Convert to a bin index
    }
  }
};

//********************************************************************
bool HEPDATA_Loader::isSignal(FitEvent *event) {
  //********************************************************************
  return filter_func(event);
};

//********************************************************************
void HEPDATA_Loader::FillHistograms() {
  //********************************************************************
  // Should not be needed in the future.
  Measurement1D::FillHistograms();
}

std::vector<std::unique_ptr<TH1>> Split2D(TH2 const *h, std::string SplitOn) {
  std::vector<std::unique_ptr<TH1>> rtns;
  if (SplitOn == "X") {
    for (int i = 0; i < h->GetNbinsX(); i++) {
      rtns.emplace_back((TH1D *)h->ProjectionY(
          (std::string(h->GetName()) + "_" + SplitOn + "Slice_" +
           h->GetXaxis()->GetTitle() + "_" + std::to_string(i) + "_" +
           std::to_string(h->GetXaxis()->GetBinLowEdge(i + 1)) + "-" +
           std::to_string(h->GetXaxis()->GetBinUpEdge(i + 1)))
              .c_str(),
          i + 1, i + 1, "e"));
    }
  } else {
    for (int i = 0; i < h->GetNbinsY(); i++) {
      rtns.emplace_back((TH1D *)h->ProjectionX(
          (std::string(h->GetName()) + "_" + SplitOn + "Slice_" +
           h->GetYaxis()->GetTitle() + "_" + std::to_string(i) + "_" +
           std::to_string(h->GetYaxis()->GetBinLowEdge(i + 1)) + "-" +
           std::to_string(h->GetYaxis()->GetBinUpEdge(i + 1)))
              .c_str(),
          i + 1, i + 1, "e"));
    }
  }
  return rtns;
}

std::vector<std::unique_ptr<TH2>> Split3D(TH3 const *h, std::string SplitOn) {
  // this is horrible, sorry
  std::vector<std::unique_ptr<TH2>> rtns;
  if (SplitOn == "X") {
    std::string ProjectAxis = "ZY";
    for (int i = 0; i < h->GetNbinsX(); i++) {
      std::unique_ptr<TH2> slice((TH2D *)h->Project3D(ProjectAxis.c_str()));
      slice->SetName((std::string(h->GetName()) + "_" + SplitOn + "Slice_" +
                      h->GetXaxis()->GetTitle() + "_" + std::to_string(i) +
                      "_" +
                      std::to_string(h->GetXaxis()->GetBinLowEdge(i + 1)) +
                      "-" + std::to_string(h->GetXaxis()->GetBinUpEdge(i + 1)))
                         .c_str());
      if (!i) {
        std::cout << "Splitting on: " << SplitOn
                  << ". slice->GetXaxis()->GetTitle(): "
                  << slice->GetXaxis()->GetTitle()
                  << ", slice->GetYaxis()->GetTitle(): "
                  << slice->GetYaxis()->GetTitle() << std::endl;
      }
      for (int j = 0; j < h->GetNbinsY(); j++) {
        for (int k = 0; k < h->GetNbinsZ(); k++) {
          slice->SetBinContent(j + 1, k + 1,
                               h->GetBinContent(i + 1, j + 1, k + 1));
          slice->SetBinError(j + 1, k + 1, h->GetBinError(i + 1, j + 1, k + 1));
        }
      }
      rtns.push_back(std::move(slice));
    }
  } else if (SplitOn == "Y") {
    std::string ProjectAxis = "ZX";
    for (int j = 0; j < h->GetNbinsY(); j++) {
      std::unique_ptr<TH2> slice((TH2D *)h->Project3D(ProjectAxis.c_str()));
      slice->SetName((std::string(h->GetName()) + "_" + SplitOn + "Slice_" +
                      h->GetYaxis()->GetTitle() + "_" + std::to_string(j) +
                      "_" +
                      std::to_string(h->GetYaxis()->GetBinLowEdge(j + 1)) +
                      "-" + std::to_string(h->GetYaxis()->GetBinUpEdge(j + 1)))
                         .c_str());
      if (!j) {
        std::cout << "Splitting on: " << SplitOn
                  << ". slice->GetXaxis()->GetTitle(): "
                  << slice->GetXaxis()->GetTitle()
                  << ", slice->GetYaxis()->GetTitle(): "
                  << slice->GetYaxis()->GetTitle() << std::endl;
      }
      for (int i = 0; i < h->GetNbinsX(); i++) {
        for (int k = 0; k < h->GetNbinsZ(); k++) {
          slice->SetBinContent(i + 1, k + 1,
                               h->GetBinContent(i + 1, j + 1, k + 1));
          slice->SetBinError(j + 1, k + 1, h->GetBinError(i + 1, j + 1, k + 1));
        }
      }
      rtns.push_back(std::move(slice));
    }
  } else {
    std::string ProjectAxis = "YX";
    for (int k = 0; k < h->GetNbinsX(); k++) {
      std::unique_ptr<TH2> slice((TH2D *)h->Project3D(ProjectAxis.c_str()));
      slice->SetName((std::string(h->GetName()) + "_" + SplitOn + "Slice_" +
                      h->GetZaxis()->GetTitle() + "_" + std::to_string(k) +
                      "_" +
                      std::to_string(h->GetZaxis()->GetBinLowEdge(k + 1)) +
                      "-" + std::to_string(h->GetZaxis()->GetBinUpEdge(k + 1)))
                         .c_str());

      if (!k) {
        std::cout << "Splitting on: " << SplitOn
                  << ". slice->GetXaxis()->GetTitle(): "
                  << slice->GetXaxis()->GetTitle()
                  << ", slice->GetYaxis()->GetTitle(): "
                  << slice->GetYaxis()->GetTitle() << std::endl;
      }

      for (int i = 0; i < h->GetNbinsX(); i++) {
        for (int j = 0; j < h->GetNbinsY(); j++) {
          slice->SetBinContent(i + 1, j + 1,
                               h->GetBinContent(i + 1, j + 1, k + 1));
          slice->SetBinError(j + 1, k + 1, h->GetBinError(i + 1, j + 1, k + 1));
        }
      }
      rtns.push_back(std::move(slice));
    }
  }
  return rtns;
}

//********************************************************************
void HEPDATA_Loader::ScaleEvents() {
  //********************************************************************
  // Should not be needed in the future.
  Measurement1D::ScaleEvents();

  if (fHEPDATA_NDIM == 2) {
    // If 2D need to scale by width in the THPoly
    for (int i = 0; i < fMCHist->GetNbinsX(); i++) {
      fMCHist_TH2Poly->SetBinContent(i + 1, fMCHist->GetBinContent(i + 1));
    }
    fMCHist_TH2Poly->Scale(fScaleFactor, "width");
    for (int i = 0; i < fMCHist->GetNbinsX(); i++) {
      fMCHist->SetBinContent(i + 1, fMCHist_TH2Poly->GetBinContent(i + 1));
    }
  } else if (fHEPDATA_NDIM == 3) {

    for (int k = 0; k < fMCHist_TH3->GetZaxis()->GetNbins(); k++) {
      for (int j = 0; j < fMCHist_TH3->GetYaxis()->GetNbins(); j++) {
        for (int i = 0; i < fMCHist_TH3->GetXaxis()->GetNbins(); i++) {
          int gbin = GBins[i][j][k] + 1;
          fMCHist_TH3->SetBinContent(i + 1, j + 1, k + 1,
                                     fMCHist->GetBinContent(gbin));
        }
      }
    }
    fMCHist_TH3->Scale(fScaleFactor, "width");
    for (int k = 0; k < fMCHist_TH3->GetZaxis()->GetNbins(); k++) {
      for (int j = 0; j < fMCHist_TH3->GetYaxis()->GetNbins(); j++) {
        for (int i = 0; i < fMCHist_TH3->GetXaxis()->GetNbins(); i++) {
          int gbin = GBins[i][j][k] + 1;
          fMCHist->SetBinContent(
              gbin, fMCHist_TH3->GetBinContent(i + 1, j + 1, k + 1));
        }
      }
    }
  }
}

void HEPDATA_Loader::Write(std::string drawOpt) {
  Measurement1D::Write(drawOpt);
  if (fHEPDATA_NDIM == 2) {
    fDataHist_TH2Poly->Write();
    fMCHist_TH2Poly->Write();
  } else if (fHEPDATA_NDIM == 3) {
    fDataHist_TH3->Write();
    fMCHist_TH3->Write();

    if (fSettings.Has("SplitOn")) {
      auto MCHists =
          Split3D(fMCHist_TH3, fSettings.GetS("SplitOn").substr(0, 1));
      for (auto &h : MCHists) {
        h->Write();
        h->SetDirectory(nullptr);
        auto slice_slices =
            Split2D(h.get(), fSettings.GetS("SplitOn").substr(1, 2));
        for (auto &hs : slice_slices) {
          hs->Write();
          hs->SetDirectory(nullptr);
        }
      }
      auto DataHists =
          Split3D(fDataHist_TH3, fSettings.GetS("SplitOn").substr(0, 1));
      for (auto &h : DataHists) {
        h->Write();
        h->SetDirectory(nullptr);
        auto slice_slices =
            Split2D(h.get(), fSettings.GetS("SplitOn").substr(1, 2));
        for (auto &hs : slice_slices) {
          hs->Write();
          hs->SetDirectory(nullptr);
        }
      }
    }
  }
}