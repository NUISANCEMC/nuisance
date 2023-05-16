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
#include "yaml-cpp/yaml.h"
#include "HEPDATA_Loader.h"
#include "nuiscling.h"

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
    if (!node) return 1;
    if (!node["name"]) return 1;

    if (node["name"]) rhs.name = node["name"].as<std::string>();
    if (node["data_file"]) rhs.data_file = node["data_file"].as<std::string>();
    // if (node["description"]) rhs.description = node["description"].as<std::string>();

    return 1;
  };
};
};

struct HepDataVariables {
  std::vector<double> values;
  std::vector<double> low;
  std::vector<double> high;
  std::vector<double> center;
  std::vector<double> edges;
  std::map<std::string,std::string> qualifiers;
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

    if (node["qualifiers"]){
      YAML::Node node_qualifiers = node["qualifiers"];
      for (int i = 0; i < node_qualifiers.size(); i++) {
        if (node_qualifiers[i]["name"] && node_qualifiers[i]["value"]){
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
  if (hepdata_docs.find(table) == hepdata_docs.end()){
    std::cerr << "HepData Table not found : " << table << std::endl;
    std::cerr << " - [ Available Tables ]" << std::endl;
    for (auto const& it : hepdata_docs){
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
  for (auto const& q : entries.qualifiers){
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
    for (auto const& q : entries.qualifiers){
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
  fHEPDATA_NDIM = (int)dimension1.valid + (int)dimension2.valid + (int)dimension3.valid;
  if (fHEPDATA_NDIM == 1){
    // Measurement 1D uses normal nuisance classes
    fDataHist = new TH1D(dataname.c_str(), 
      (dataname+";"+dimension1.title+";"+entries.title).c_str(), 
      dimension1.n,
      &dimension1.edges[0]
    );

    for (int i = 0; i < entries.values.size(); i++) {
      fDataHist->SetBinContent(i + 1, entries.values[i]);
    }
    fSettings.SetXTitle(dimension1.title);
    fSettings.SetYTitle(entries.title);

  } else if (fHEPDATA_NDIM == 2){

    // dimension1.n still gives total number of bins across full 2D space
    fDataHist = new TH1D(dataname.c_str(), 
      (dataname+";BIN;"+entries.title).c_str(), 
      dimension1.n,
      0.0,
      (double)dimension1.n
    );

    for (int i = 0; i < entries.values.size(); i++) {
      fDataHist->SetBinContent(i+1, entries.values[i]);
    }
    fSettings.SetXTitle(dimension1.title);
    fSettings.SetYTitle(entries.title);

    // Now create a seperate TH2Poly to play with
    fDataHist_TH2Poly = new TH2Poly();

    for (int i = 0; i < entries.values.size(); i++) {

      double xlow  = dimension1.low[i];
      double xhigh = dimension1.high[i];
      double ylow  = dimension2.low[i];
      double yhigh = dimension2.high[i];
      double vals  = entries.values[i];

      fDataHist_TH2Poly->AddBin(xlow,ylow,xhigh,yhigh);
      fDataHist_TH2Poly->SetBinContent(i, fDataHist->GetBinContent(i+1));
  
    }
    fDataHist_TH2Poly->GetXaxis()->SetTitle( dimension1.name.c_str() );
    fDataHist_TH2Poly->GetYaxis()->SetTitle( dimension2.name.c_str() );
    fDataHist_TH2Poly->GetZaxis()->SetTitle( entries.name.c_str() );

    fMCHist_TH2Poly = (TH2Poly*) fDataHist_TH2Poly;
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
                 << dimension1.name << " for projection function definition.");
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


  // Covariance Setup -----------
  if (!covarkey.empty()){

    YAML::Node covar_doc = YAML::LoadFile(covarkey);

    HepDataVariables covar_vals = covar_doc["dependent_variables"][0].as<HepDataVariables>();
    HepDataVariables covar_dim1 = covar_doc["independent_variables"][0].as<HepDataVariables>();
    HepDataVariables covar_dim2 = covar_doc["independent_variables"][1].as<HepDataVariables>();

    // Covariance is assumed to be square, so matrix size is sqrt that.
    fFullCovar = new TMatrixDSym(sqrt(covar_vals.n));
    for (int i = 0; i < covar_vals.n; i++){
      int bin1 = covar_dim1.values[i];
      int bin2 = covar_dim2.values[i];
      double cov = covar_vals.values[i];
      (*fFullCovar)(bin2,bin1) = cov;
    }
    std::cout << "[INFO] Loaded a full covariance size : " << covar_vals.n << std::endl;

  }



  // Scaling Setup ---------------------------------------------------
  // ScaleFactor is bodged based on dependent units
  fScaleFactor = GetEventHistogram()->Integral("width") / (double(fNEvents));

  if (!entries.units.compare("/GeV")){
    fScaleFactor = GetEventHistogram()->Integral("width") / (double(fNEvents) * TotalIntegratedFlux("width"));
  }

  if (!entries.units.compare("10^{-38}")){
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
  if (fHEPDATA_NDIM == 1){ fXVar = projection_funcs[0](event); }
  else if (fHEPDATA_NDIM == 2){
    double x = projection_funcs[0](event);
    double y = projection_funcs[1](event);
    fXVar = fDataHist_TH2Poly->FindBin(x,y) + 0.5; // Convert to a bin index
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

//********************************************************************
void HEPDATA_Loader::ScaleEvents() {
//********************************************************************
  // Should not be needed in the future.
  Measurement1D::ScaleEvents();

  if (fHEPDATA_NDIM == 2){
    // If 2D need to scale by width in the THPoly
    for (int i = 0; i < fMCHist->GetNbinsX(); i++){
      fMCHist_TH2Poly->SetBinContent(i+1, fMCHist->GetBinContent(i+1));
    }
    fMCHist_TH2Poly->Scale(fScaleFactor,"width");
    for (int i = 0; i < fMCHist->GetNbinsX(); i++){
      fMCHist->SetBinContent(i+1, fMCHist_TH2Poly->GetBinContent(i+1));
    }
  }

}


void HEPDATA_Loader::Write(std::string drawOpt){
  Measurement1D::Write(drawOpt);
  if (fHEPDATA_NDIM == 2){
    fDataHist_TH2Poly->Write();
    fMCHist_TH2Poly->Write();
  }
}