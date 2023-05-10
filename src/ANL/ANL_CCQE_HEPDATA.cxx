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
  
//   fSettings.SetDataInput(  FitPar::GetDataBase() + "ANL/ANL_CCQE_Data_PRL31_844.root;ANL_1DQ2_Data" );
//   fSettings.SetEnuRange(0.0, 3.0);

// }

// void GetHistogramFromYAMLFile(fSettings.GetDataInput());


std::string PrintNodeType(YAML::Node mynode){
  
  switch (mynode.Type()) {
    case NodeType::Null: return "NULL";
    case NodeType::Scalar: return "Scaler";
    case NodeType::Sequence: return "Sequence";
    case NodeType::Map: return "Map";
    case NodeType::Undefined: return "Undef";
  }
  return "NO!!";

};

//********************************************************************
ANL_CCQE_HEPDATA::ANL_CCQE_HEPDATA(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  // // Descrip should go into 1DQ2
  // std::string descrip = "ANL CCQ2 Event Rate 1DQ2 nu sample. \n" \
  //                       "Target: D2 \n" \
  //                       "Flux:  \n" \
  //                       "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);

  std::string filename = samplekey.GetS("release") + "/submission.yaml";
  std::string table = samplekey.GetS("table");
  YAML::Node doc = YAML::LoadFile(filename); 

  std::cout << "Loading from file " << filename << std::endl;
  std::vector<YAML::Node> docs = YAML::LoadAllFromFile(filename);
 
  YAML::Node curdoc;

  NUIS_LOG(SAM, "BUILDING CCQE HEPDATA");

  // Loop through and find correct data table
  std::string datafile = "";
  std::string dataname = "";
  for (std::size_t i=0;i<docs.size();i++) {
    YAML::Node doc = docs[i];
    curdoc = doc;
    for(YAML::const_iterator it=doc.begin();it != doc.end();++it) {
      std::string key = it->first.as<std::string>();       // <- key
      if (!key.compare("name")){
        std::string value = it->second.as<std::string>();       // <- key
        if (!value.compare(table)){
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
  YAML::Node ind = doc["independent_variables"][0];
  YAML::Node dep = doc["dependent_variables"][0];

  std::cout << "Getting independent " << doc["independent_variables"].size() << " " <<  doc["independent_variables"][0]["header"]["name"].as<std::string>() << std::endl;
  std::cout << doc["dependent_variables"]["values"][0] << std::endl;

  fSettings.SetDescription(dataname);

  std::cout << "XTITLE " << doc["independent_variables"][0]["header"]["name"].as<std::string>() << std::endl;
  std::cout << "YTITLE " << doc["dependent_variables"][0]["header"]["name"].as<std::string>() << std::endl;
  fSettings.SetXTitle(doc["independent_variables"][0]["header"]["name"].as<std::string>());
  fSettings.SetYTitle(doc["dependent_variables"][0]["header"]["name"].as<std::string>());
  fSettings.SetTitle(dataname);

  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG/Q2CORR/MASK");

  double enumin;
  double enumax;
  std::string targets;
  std::string species;
  YAML::Node qualifiers = doc["dependent_variables"][0]["qualifiers"];
  for (int i = 0; i < qualifiers.size(); i++){
    std::string name = qualifiers[i]["name"].as<std::string>();
    std::string value = qualifiers[i]["value"].as<std::string>();
    std::cout << "QUALIFIER SETTING : " << name << " " << value << std::endl;
    fSettings.SetDefault(name, value);
    // if (!name.compare("Enu_min")) enumin = qualifiers[i]["value"].as<double>();
    // if (!name.compare("Enu_max")) enumax = qualifiers[i]["value"].as<double>();
    // if (!name.compare("AllowedSpecies")) species = qualifiers[i]["value"].as<std::string>();
    // if (!name.compare("AllowedTargets")) targets = qualifiers[i]["value"].as<std::string>();
  }

  fSettings.SetEnuRange( enumin, enumax);
  fSettings.DefineAllowedTargets(targets);
  fSettings.DefineAllowedSpecies(species);


  YAML::Node xvalues = doc["independent_variables"][0]["values"];
  YAML::Node yvalues = doc["dependent_variables"][0]["values"];

  std::vector<double> edges;
  std::vector<double> yentries;
  std::vector<double> xcentres;
  for (int i = 0; i < xvalues.size(); i++){
    edges.push_back( xvalues[i]["low"].as<double>());
    yentries.push_back( yvalues[i]["value"].as<double>());
    xcentres.push_back( (xvalues[i]["low"].as<double>() + xvalues[i]["high"].as<double>())/2);
  }
  edges.push_back( xvalues[xvalues.size()-1]["high"].as<double>() );

  for (int i = 0; i < edges.size()-1; i++){
    std::cout << "XVALUES " << edges[i] << " " << yentries[i] <<  std::endl;
  }

  std::string xtitle = doc["independent_variables"][0]["header"]["name"].as<std::string>() + "[" + doc["dependent_variables"][0]["header"]["units"].as<std::string>() + "]";
  std::string ytitle = doc["dependent_variables"][0]["header"]["name"].as<std::string>() + "[" + doc["dependent_variables"][0]["header"]["units"].as<std::string>() + "]";
  std::string title = dataname + ";" + xtitle + ";" + ytitle;
  fDataHist = new TH1D( dataname.c_str(), title.c_str(), edges.size()-1, &edges[0] );

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor for shape
  fScaleFactor = 1.0;

  // Plot Setup -------------------------------------------------------
  //SetDataFromRootFile( fSettings.GetDataInput() );
  

  // Hard Coded YAML File Histogram Setting
  // fDataHist = GetHistogramFromYAMLFile(fSettings.GetDataInput());


  // SetPoissonErrors();   - Should not be needed
  // SetCovarFromDiagonal(); - Should not be needed


  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

}

//********************************************************************
void ANL_CCQE_HEPDATA::FillEventVariables(FitEvent * event) {
//********************************************************************

  // Eventual Selection of fXVar, fYVar....
  if (event->NumFSParticle(13) == 0)
    return;

  // Fill histogram with reconstructed Q2 Distribution
  fXVar = -999.9;
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  fXVar = FitUtils::Q2QErec(Pmu, cos(ThetaMu), 0., true);

  GetQ2Box()->fQ2 = fXVar;
  return;
};

//********************************************************************
bool ANL_CCQE_HEPDATA::isSignal(FitEvent * event) {
//********************************************************************

  if (!SignalDef::isCCQE(event, 14, EnuMin, EnuMax)) return false;

  return true;
};

//********************************************************************
void ANL_CCQE_HEPDATA::FillHistograms() {
//********************************************************************

  Measurement1D::FillHistograms();

}

//********************************************************************
void ANL_CCQE_HEPDATA::ScaleEvents() {
//********************************************************************

  Measurement1D::ScaleEvents();

}
