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
// #include "boost/histogram.hpp"
// using YAML;
// using namespace boost::histogram;

#include "ANL_CCQE_HEPDATA.h"

#include "nuiscling.h"

using namespace YAML;
// void fillsettings_from_yaml(SampleSettings& setting){

// Convert YAML

//   // All these need to come from YAML.
//   fSettings.SetDescription("Test Description");
//   fSettings.SetXTitle("Q^{2} (GeV^{2})");
//   fSettings.SetYTitle("Number of events");
//   fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG/Q2CORR/MASK");
//   fSettings.SetEnuRange(0.0, 6.0);
//   fSettings.DefineAllowedTargets("D,H");
//   fSettings.SetTitle("ANL #nu_mu CCQE");
//   fSettings.DefineAllowedSpecies("numu");

//   fSettings.SetDataInput(  FitPar::GetDataBase() +
//   "ANL/ANL_CCQE_Data_PRL31_844.root;ANL_1DQ2_Data" );
//   fSettings.SetEnuRange(0.0, 3.0);

// }

// void GetHistogramFromYAMLFile(fSettings.GetDataInput());

struct HepDataSubmission {
  std::string test;
};

struct HepDataVariables {
  std::vector<double> values;
  std::vector<double> low;
  std::vector<double> high;
  std::vector<double> center;
  std::vector<double> edges;
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
    rhs.units = node["header"]["units"].as<std::string>();
    rhs.title = rhs.name + "[" + rhs.units + "]";

    YAML::Node xvalues = node["values"];

    rhs.n = xvalues.size();
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

    rhs.valid = 1;
    return 1;
  };
};
}; // namespace YAML

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
  return "NO!!";
};

double QueryErrorFromPoint(YAML::Node val) {

  // if ( val["errors"] )
  // yerrors.push_back( yvalues[i]["errors"] );
  return 0.0;
}

//********************************************************************
ANL_CCQE_HEPDATA::ANL_CCQE_HEPDATA(nuiskey samplekey) {
  //********************************************************************

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);

  std::string filename = samplekey.GetS("release") + "/submission.yaml";
  std::string table = samplekey.GetS("table");
  std::string NUIS_SNIPPET_DIR =
      std::string(getenv("NUISANCE")) + "/data/HEPDataSnippets/";
  std::string snippet_file = NUIS_SNIPPET_DIR + samplekey.GetS("snippet_file");
  YAML::Node doc = YAML::LoadFile(filename);

  std::cout << "Loading from file " << filename << std::endl;
  std::vector<YAML::Node> docs = YAML::LoadAllFromFile(filename);

  YAML::Node curdoc;

  NUIS_LOG(SAM, "BUILDING CCQE HEPDATA");

  // Loop through and find correct data table
  std::string datafile = "";
  std::string dataname = "";
  for (std::size_t i = 0; i < docs.size(); i++) {
    YAML::Node doc = docs[i];
    curdoc = doc;
    for (YAML::const_iterator it = doc.begin(); it != doc.end(); ++it) {
      std::string key = it->first.as<std::string>(); // <- key
      if (!key.compare("name")) {
        std::string value = it->second.as<std::string>(); // <- key
        if (!value.compare(table)) {
          curdoc = Clone(doc);
          datafile = doc["data_file"].as<std::string>();
          dataname = doc["name"].as<std::string>();
          break;
        }
      }
    }
  }

  // Now load release
  std::cout << "DATA FILE " << datafile << std::endl;
  std::cout << "NAME " << dataname << std::endl;
  doc = YAML::LoadFile(samplekey.GetS("release") + "/" + datafile);

  // Grab dependent independent variables
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG/Q2CORR/MASK");

  double enumin;
  double enumax;
  std::string targets;
  std::string species;
  YAML::Node qualifiers = doc["dependent_variables"][0]["qualifiers"];
  for (int i = 0; i < qualifiers.size(); i++) {
    std::string name = qualifiers[i]["name"].as<std::string>();
    std::string value = qualifiers[i]["value"].as<std::string>();
    fSettings.SetDefault(name, value);
  }

  YAML::Node indep_var = doc["independent_variables"];
  HepDataVariables dimension1 = indep_var[0].as<HepDataVariables>();
  HepDataVariables dimension2 = indep_var[1].as<HepDataVariables>();
  HepDataVariables dimension3 = indep_var[2].as<HepDataVariables>();

  YAML::Node dep_var = doc["dependent_variables"];
  HepDataVariables entries = dep_var[0].as<HepDataVariables>();

  std::string title = dataname + ";" + dimension1.title + ";" + entries.title;

  for (int i = 0; i < dimension1.n + 1; i++) {
    std::cout << "BIN1 " << i << " " << dimension1.edges[i] << " "
              << dimension1.edges.size() << std::endl;
  }
  if (dimension1.valid)
    fDataHist = new TH1D(dataname.c_str(), title.c_str(), dimension1.n,
                         &dimension1.edges[0]);

  std::cout << "[INFO]: Parsing snippet file: " << snippet_file << std::endl;
  if (!nuiscling::Get().LoadFile(snippet_file)) {
    NUIS_ABORT("Failed to process snippet file.");
  }

  if (dimension1.valid) {
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

  if (!fSettings.Has("Filter")) {
    std::cout << "Known Qualifiers: " << std::endl;
    for (int i = 0; i < qualifiers.size(); i++) {
      std::cout << "\t" << qualifiers[i]["name"].as<std::string>() << ": "
                << qualifiers[i]["value"].as<std::string>() << std::endl;
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

  // Non dynamic NUISANCE CRAP
  fSettings.SetDescription(dataname);
  fSettings.SetXTitle(dimension1.title);
  fSettings.SetYTitle(entries.title);
  fSettings.SetTitle(title);

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor for shape
  fScaleFactor = 1.0;

  // Plot Setup -------------------------------------------------------
  // SetDataFromRootFile( fSettings.GetDataInput() );

  // Hard Coded YAML File Histogram Setting
  // fDataHist = GetHistogramFromYAMLFile(fSettings.GetDataInput());

  // SetPoissonErrors();   - Should not be needed
  // SetCovarFromDiagonal(); - Should not be needed

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
}

//********************************************************************
void ANL_CCQE_HEPDATA::FillEventVariables(FitEvent *event) {
  //********************************************************************
  fXVar = projection_funcs[0](event);
};

//********************************************************************
bool ANL_CCQE_HEPDATA::isSignal(FitEvent *event) {
  //********************************************************************
  return filter_func(event);
};

//********************************************************************
void ANL_CCQE_HEPDATA::FillHistograms() {
  //********************************************************************

  // Should not be needed in the future.
  Measurement1D::FillHistograms();
}

//********************************************************************
void ANL_CCQE_HEPDATA::ScaleEvents() {
  //********************************************************************

  // Should not be needed in the future.
  Measurement1D::ScaleEvents();
}
