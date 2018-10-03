// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#include "NuisConfig.h"

#include "FitLogger.h"
#include "GeneralUtils.h"
#include "TXMLEngine.h"

namespace Config {

nuisconfig &Get() { return nuisconfig::GetConfig(); };
std::string GetPar(std::string const &name) { return Get().GetConfig(name); }
bool HasPar(std::string const &name) { return Get().HasConfig(name); }
std::string GetParS(std::string const &name) { return Get().GetConfigS(name); }
int GetParI(std::string const &name) { return Get().GetConfigI(name); }
bool GetParB(std::string const &name) { return Get().GetConfigB(name); }
float GetParF(std::string const &name) { return Get().GetConfigF(name); }
double GetParD(std::string const &name) { return Get().GetConfigD(name); }
void SetPar(std::string const &name, std::string const &val) {
  Get().SetConfig(name, val);
}
void SetPar(std::string const &name, char const *val) {
  Get().SetConfig(name, val);
}
void SetPar(std::string const &name, bool val) { Get().SetConfig(name, val); }
void SetPar(std::string const &name, int val) { Get().SetConfig(name, val); }
void SetPar(std::string const &name, float val) { Get().SetConfig(name, val); }
void SetPar(std::string const &name, double val) { Get().SetConfig(name, val); }
}

namespace FitPar {
std::string GetDataBase() { return GeneralUtils::GetTopLevelDir() + "/data/"; };
nuisconfig &Config() { return Config::Get(); };
}

nuisconfig *nuisconfig::m_nuisconfigInstance = NULL;
nuisconfig &nuisconfig::GetConfig(void) {
  if (!m_nuisconfigInstance) m_nuisconfigInstance = new nuisconfig;
  return *m_nuisconfigInstance;
};

// Main Class Definition
nuisconfig::nuisconfig() {
  // Load default Parameters
  std::string filename =
      (GeneralUtils::GetTopLevelDir() + "/parameters/config.xml");
  std::cout << "[ NUISANCE ]: Loading DEFAULT settings from : " << filename;

  // Create XML Engine
  fXML = new TXMLEngine;
  fXML->SetSkipComments(true);

  // Load in documents
  fXMLDocs.push_back(fXML->ParseFile(filename.c_str(), 1000000));
  if (!fXMLDocs[0]) {
    THROW("Cannot Read Parameters File!");
  }

  // Setup Main XML Node to be the first file read
  fMainNode = fXML->DocGetRootElement(fXMLDocs[0]);

  // Print result
  std::cout << " -> DONE." << std::endl;
}

nuisconfig::~nuisconfig() {
  // Should really delete XML objects here but we don't
}

void nuisconfig::LoadSettings(std::string const &filename,
                              std::string const &state) {
  // Open file and see if its XML
  std::cout << "[ NUISANCE ]: Trying to parse file : " << filename;
  StopTalking();
  XMLDocPointer_t readdoc = fXML->ParseFile(filename.c_str(), 1000000);
  StartTalking();

  // If it is parse it as a nice XML config file
  if (readdoc) {
    std::cout << " -> Found XML file." << std::endl;
    LoadXMLSettings(filename, state);

    // Otherwise its an old simple card file
  } else {
    std::cout << " -> Assuming its a simple card file." << std::endl;
    LoadCardSettings(filename, state);
  }
}

void nuisconfig::LoadXMLSettings(std::string const &filename,
                                 std::string const &state) {
  std::cout << "[ NUISANCE ]: Loading XML settings from : " << filename
            << std::endl;

  // Add new file to xml docs list
  fXMLDocs.push_back(fXML->ParseFile(filename.c_str(), 1000000));

  if (!fXMLDocs.back()) {
    THROW("Failed to read: " << filename);
  }

  // Loop over children and add
  XMLNodePointer_t child =
      fXML->GetChild(fXML->DocGetRootElement(fXMLDocs.back()));

  // // Here we manually load all the children from the card file into our root
  // node
  if (!child) {
    THROW("CANNOT Find child inside settings file!");
  }

  while (child) {
    // SPECIAL CONFIG CASE
    // If its a config node, then remove previous attributes, overriding old
    // value
    if (!std::string(fXML->GetNodeName(child)).compare("config")) {
      // Loop over attribues
      XMLAttrPointer_t attr1 = fXML->GetFirstAttr(child);
      while (attr1) {
        // If a valid attribute name is given then compare
        if (!GetConfigS(fXML->GetAttrName(attr1)).empty()) {
          // Get full list of present configs
          std::vector<XMLNodePointer_t> confignodes = GetNodes("config");

          // Loop over present configs and compare
          for (size_t i = 0; i < confignodes.size(); i++) {
            // If we already have this config, free the old attribute
            if (fXML->HasAttr(confignodes[i], fXML->GetAttrName(attr1))) {
              fXML->FreeAttr(confignodes[i], fXML->GetAttrName(attr1));
              break;
            }
          }
        }

        // Move onto next config attribute
        attr1 = fXML->GetNextAttr(attr1);
      }
    }

    TString nodeStr;

    fXML->SaveSingleNode(child, &nodeStr);

    XMLNodePointer_t copyNode = fXML->ReadSingleNode(nodeStr.Data());

    // std::cout << "copying node..." << std::endl;
    // PrintXML(copyNode);

    // Add this child to the main config list
    fXML->AddChild(fMainNode, copyNode);

    // std::cout << "Done, was it added?" << std::endl;
    // PrintXML(fMainNode);

    // Get Next Child
    child = fXML->GetNext(child);
  }
  std::cout << " -> DONE." << std::endl;
}

void nuisconfig::LoadCardSettings(std::string const &filename,
                                  std::string const &state) {
  std::cout << "[ NUISANCE ]: Loading simple config from : " << filename;

  // Build XML Config from the card file by parsing each line
  std::vector<std::string> cardlines =
      GeneralUtils::ParseFileToStr(filename, "\n");
  int linecount = 0;

  // Loop over all input lines
  for (std::vector<std::string>::iterator iter = cardlines.begin();
       iter != cardlines.end(); iter++) {
    std::string line = (*iter);
    linecount++;

    // Skip Comments
    if (line.empty()) continue;
    if (line.c_str()[0] == '#') continue;

    // Parse whitespace
    std::vector<std::string> strvct = GeneralUtils::ParseToStr(line, " ");
    if (strvct.empty()) continue;

    // Get Identifier
    std::string id = strvct[0];

    // Build backwards compatible xml configs

    // Sample structure
    if (!id.compare("sample")) {
      CreateSampleNodeFromLine(line);
    }

    // Any parameter structure
    if (id.find("_parameter") != std::string::npos) {
      CreateParameterNodeFromLine(line);
    }

    // Any covar structure
    if (!id.compare("covar") || !id.compare("pull") || !id.compare("throw")) {
      CreatePullNodeFromLine(line);
    }

    // Any config structure
    if (!id.compare("config")) {
      CreateOldConfigNodeFromLine(line);
    }
  }
  std::cout << " -> DONE." << std::endl;
}

XMLNodePointer_t nuisconfig::CreateSampleNodeFromLine(std::string const &line) {
  // Create new node entry
  XMLNodePointer_t samplenode = CreateNode("sample");

  // Parse line
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(line, " ");

  // Add line elements to the node
  // name input type norm
  if (strvct.size() > 1) Set(samplenode, "name", strvct[1]);
  if (strvct.size() > 2) Set(samplenode, "input", strvct[2]);
  if (strvct.size() > 3) Set(samplenode, "type", strvct[3]);
  if (strvct.size() > 4) Set(samplenode, "norm", strvct[4]);

  return samplenode;
}

XMLNodePointer_t nuisconfig::CreateParameterNodeFromLine(
    std::string const &line) {
  // Create new node entry
  XMLNodePointer_t parnode = CreateNode("parameter");

  // Parse line
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(line, " ");

  // Add line elements to the node
  // type name nominal [low] [high] [step] state
  if (strvct.size() > 0) Set(parnode, "type", strvct[0]);
  if (strvct.size() > 1) Set(parnode, "name", strvct[1]);
  if (strvct.size() > 2) Set(parnode, "nominal", strvct[2]);

  // If free structure
  if (strvct.size() == 7) {
    Set(parnode, "low", strvct[3]);
    Set(parnode, "high", strvct[4]);
    Set(parnode, "step", strvct[5]);
    Set(parnode, "state", strvct[6]);

    // Fixed param structure
  } else if (strvct.size() == 3) {
    Set(parnode, "state", "FIX");
  } else if (strvct.size() == 4) {
    Set(parnode, "state", strvct[3]);
  }

  return parnode;
}

XMLNodePointer_t nuisconfig::CreatePullNodeFromLine(std::string const &line) {
  // Create new node entry
  XMLNodePointer_t parnode = CreateNode("covar");

  // Parse line
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(line, " ");

  // Add line elements to the node
  // name input type
  if (strvct.size() > 1) Set(parnode, "name", strvct[1]);
  if (strvct.size() > 2) Set(parnode, "input", strvct[2]);
  if (strvct.size() > 3) Set(parnode, "type", strvct[3]);

  return parnode;
}

XMLNodePointer_t nuisconfig::CreateOldConfigNodeFromLine(
    std::string const &line) {
  // Create new node entry
  XMLNodePointer_t confignode = CreateNode("config");

  // Parse line
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(line, " ");

  // Add line elements to the node
  // name value
  if (strvct.size() > 2) Set(confignode, strvct[1], strvct[2]);

  return confignode;
}

void nuisconfig::FinaliseSettings(std::string const &name) {
  std::cout << "[ NUISANCE ]: Finalising run settings";

  WriteSettings(name);

  // Save full config to file
  RemoveEmptyNodes();
  RemoveIdenticalNodes();

  std::cout << " -> DONE." << std::endl;
}

void nuisconfig::WriteSettings(std::string const &outputname) {
  // Create a New XML Doc
  XMLDocPointer_t newxmldoc = fXML->NewDoc();
  fXML->DocSetRootElement(newxmldoc, fMainNode);

  // Save document to file
  if (GetConfigB("SaveParsedXMLFile")) {
    fXML->SaveDoc(newxmldoc, outputname.c_str());
  }
}

void nuisconfig::PrintXML(XMLNodePointer_t node, int indent) {
  if (!node) {
    node = fMainNode;
  }

  std::stringstream ss("");
  for (int i = 0; i < indent; ++i) {
    ss << " ";
  }

  std::cout << ss.str() << "<" << fXML->GetNodeName(node) << std::flush;

  XMLAttrPointer_t attr = fXML->GetFirstAttr(node);
  while (attr) {
    std::cout << " " << fXML->GetAttrName(attr) << "=\""
              << fXML->GetAttrValue(attr) << "\"" << std::flush;
    attr = fXML->GetNextAttr(attr);
  }
  if (!fXML->GetChild(node)) {
    std::cout << " />" << std::endl;
    return;
  }
  std::cout << " >" << std::endl;

  XMLNodePointer_t child = fXML->GetChild(node);
  while (child) {
    PrintXML(child, indent + 1);
    child = fXML->GetNext(child);
  }

  std::cout << ss.str() << "</" << fXML->GetNodeName(node) << ">" << std::endl;
}

XMLNodePointer_t nuisconfig::CreateNode(std::string const &name) {
  return fXML->NewChild(fMainNode, 0, name.c_str());
}

XMLNodePointer_t nuisconfig::CreateNode(XMLNodePointer_t node,
                                        std::string const &name) {
  return fXML->NewChild(node, 0, name.c_str());
}

XMLNodePointer_t nuisconfig::GetNode(XMLNodePointer_t node,
                                     std::string const &type) {
  /// Loop over all children
  XMLNodePointer_t child = fXML->GetChild(node);
  while (child != 0) {
    /// Get nodes for given type (if type empty return all)
    if (std::string(fXML->GetNodeName(child)) == type.c_str() or type.empty()) {
      return child;
    }

    // Next child
    child = fXML->GetNext(child);
  }

  // Child not found
  return 0;
}

void nuisconfig::RemoveEmptyNodes() {
  std::vector<XMLNodePointer_t> nodelist = Config::Get().GetNodes();
  for (size_t i = 0; i < nodelist.size(); i++) {
    if (fXML->IsEmptyNode(nodelist[i])) {
      std::cout << "Removing empty node: " << fXML->GetNodeName(nodelist[i])
                << ", with child ?" << bool(fXML->GetChild(nodelist[i]))
                << std::endl;
      RemoveNode(nodelist[i]);
    }
  }
}

void nuisconfig::RemoveIdenticalNodes() {
  std::vector<XMLNodePointer_t> removed;

  // Loop over all nodes and check for identical nodes
  std::vector<XMLNodePointer_t> nodelist = Config::Get().GetNodes();
  for (size_t i = 0; i < nodelist.size(); i++) {
    for (size_t j = 0; j < nodelist.size(); j++) {
      if (i == j) continue;

      XMLNodePointer_t node1 = nodelist[i];
      XMLNodePointer_t node2 = nodelist[j];

      // Check node already removed.
      if (std::find(removed.begin(), removed.end(), node1) != removed.end()) {
        continue;
      }
      if (std::find(removed.begin(), removed.end(), node2) != removed.end()) {
        continue;
      }

      // Check matching
      if (!MatchingNodes(node1, node2)) {
        continue;
      }

      if (std::string(fXML->GetNodeName(node1)).compare("config") and
          fXML->IsEmptyNode(node1)) {
        // Matching so print out warning
        std::cout << "Matching nodes given! Removing node1!" << std::endl
                  << "Node 1" << std::endl;
        PrintNode(node1);

        std::cout << "Node 2" << std::endl;
        PrintNode(node2);
      }

      // Remove node
      removed.push_back(node1);
    }
  }

  // Now go through and remove this node.
  for (size_t i = 0; i < removed.size(); i++) {
    RemoveNode(removed.at(i));
  }

  return;
}

void nuisconfig::RemoveNode(XMLNodePointer_t node) {
  std::cout << "[INFO]: Removing node: " << fXML->GetNodeName(node)
            << std::endl;
  fXML->FreeAllAttr(node);
  fXML->CleanNode(node);
  fXML->FreeNode(node);
  fXML->UnlinkNode(node);
}

void nuisconfig::PrintNode(XMLNodePointer_t node) {
  // Print Node Name
  std::cout << fXML->GetNodeName(node) << std::endl;

  // Loop and print all attributes
  XMLAttrPointer_t attr = fXML->GetFirstAttr(node);
  while (attr != 0) {
    std::cout << " -> " << fXML->GetAttrName(attr) << " : "
              << fXML->GetAttrValue(attr) << std::endl;
    attr = fXML->GetNextAttr(attr);
  }
}

bool nuisconfig::MatchingNodes(XMLNodePointer_t node1, XMLNodePointer_t node2) {
  bool matching = true;
  XMLAttrPointer_t attr = fXML->GetFirstAttr(node1);
  while (attr != 0) {
    if (GetS(node2, fXML->GetAttrName(attr)) != fXML->GetAttrValue(attr))
      matching = false;
    attr = fXML->GetNextAttr(attr);
  }
  return matching;
}

XMLNodePointer_t nuisconfig::GetNode(std::string const &type) {
  return GetNode(fMainNode, type);
}

std::vector<XMLNodePointer_t> nuisconfig::GetNodes(XMLNodePointer_t node,
                                                   std::string const &type) {
  // Create new vector for nodes
  std::vector<XMLNodePointer_t> nodelist;

  /// Loop over all children
  XMLNodePointer_t child = fXML->GetChild(node);
  while (child != 0) {
    /// Get nodes for given type (if type empty return all)
    if (std::string(fXML->GetNodeName(child)) == type.c_str() or type.empty()) {
      nodelist.push_back(child);
    }

    // Next child
    child = fXML->GetNext(child);
  }

  // return list
  return nodelist;
}

std::vector<XMLNodePointer_t> nuisconfig::GetNodes(std::string const &type) {
  return GetNodes(fMainNode, type);
}

void nuisconfig::Set(XMLNodePointer_t node, std::string const &name,
                     std::string const &val) {
  // Remove and re-add attribute
  if (fXML->HasAttr(node, name.c_str())) {
    fXML->FreeAttr(node, name.c_str());
  }

  fXML->NewAttr(node, 0, name.c_str(), val.c_str());
}
void nuisconfig::Set(XMLNodePointer_t node, std::string const &name,
                     char const *val) {
  Set(node, name, std::string(val));
}

void nuisconfig::Set(XMLNodePointer_t node, std::string const &name, bool val) {
  Set(node, name, GeneralUtils::BoolToStr(val));
}

void nuisconfig::Set(XMLNodePointer_t node, std::string const &name, int val) {
  Set(node, name, GeneralUtils::IntToStr(val));
}

void nuisconfig::Set(XMLNodePointer_t node, std::string const &name,
                     float val) {
  Set(node, name, GeneralUtils::DblToStr(val));
}

void nuisconfig::Set(XMLNodePointer_t node, std::string const &name,
                     double val) {
  Set(node, name, GeneralUtils::DblToStr(val));
}

void nuisconfig::SetS(XMLNodePointer_t node, std::string const &name,
                      std::string const &val) {
  Set(node, name, val);
}

void nuisconfig::SetB(XMLNodePointer_t node, std::string const &name,
                      bool val) {
  Set(node, name, GeneralUtils::BoolToStr(val));
}

void nuisconfig::SetI(XMLNodePointer_t node, std::string const &name, int val) {
  Set(node, name, GeneralUtils::IntToStr(val));
}

void nuisconfig::SetF(XMLNodePointer_t node, std::string const &name,
                      float val) {
  Set(node, name, GeneralUtils::DblToStr(val));
}

void nuisconfig::SetD(XMLNodePointer_t node, std::string const &name,
                      double val) {
  Set(node, name, GeneralUtils::DblToStr(val));
}

bool nuisconfig::Has(XMLNodePointer_t node, std::string const &name) {
  // If node empty return empty
  if (node == 0) return false;

  // Search attributes
  XMLAttrPointer_t attr = fXML->GetFirstAttr(node);
  bool found = false;

  // Loop over all attributes
  while (attr != 0) {
    // Find value of correct name
    if (std::string(fXML->GetAttrName(attr)) == name.c_str()) {
      found = true;
      break;
    }

    // Next Attribute
    attr = fXML->GetNextAttr(attr);
  }

  return found;
}

std::string nuisconfig::Get(XMLNodePointer_t node, std::string const &name) {
  // If node empty return empty
  if (node == 0) return "";

  // Get Attribute from child with name
  XMLAttrPointer_t attr = fXML->GetFirstAttr(node);
  std::string temp = "";

  // Loop over all attributes
  while (attr != 0) {
    // If valid match then save
    if (std::string(fXML->GetAttrName(attr)) == name.c_str()) {
      temp = fXML->GetAttrValue(attr);
    }

    // Next Attribute
    attr = fXML->GetNextAttr(attr);
  }

  return temp;
}

std::string nuisconfig::GetS(XMLNodePointer_t node, std::string const &name) {
  return Get(node, name);
}

bool nuisconfig::GetB(XMLNodePointer_t node, std::string const &name) {
  std::string tempattr = Get(node, name);
  return GeneralUtils::StrToBool(tempattr);
}

int nuisconfig::GetI(XMLNodePointer_t node, std::string const &name) {
  std::string tempattr = Get(node, name);
  return GeneralUtils::StrToInt(tempattr);
}

float nuisconfig::GetF(XMLNodePointer_t node, std::string const &name) {
  std::string tempattr = Get(node, name);
  return GeneralUtils::StrToDbl(tempattr);
}

double nuisconfig::GetD(XMLNodePointer_t node, std::string const &name) {
  std::string tempattr = Get(node, name);
  return GeneralUtils::StrToDbl(tempattr);
}

std::vector<std::string> nuisconfig::GetVS(XMLNodePointer_t node,
                                           std::string const &name,
                                           const char *del) {
  std::string tempattr = Get(node, name);
  return GeneralUtils::ParseToStr(tempattr, del);
}

// std::vector<int> nuisconfig::GetVB(XMLNodePointer_t node,
//                                    std::string name,
//                                    const char* del) {
//   std::string tempattr = Get(node, name);
//   return GeneralUtils::ParseToBool(tempattr, del);
// }

std::vector<int> nuisconfig::GetVI(XMLNodePointer_t node,
                                   std::string const &name, const char *del) {
  std::string tempattr = Get(node, name);
  return GeneralUtils::ParseToInt(tempattr, del);
}

// std::vector<int> nuisconfig::GetVF(XMLNodePointer_t node,
//                                    std::string name,
//                                    const char* del) {
//   std::string tempattr = Get(node, name);
//   return GeneralUtils::ParseToDouble(tempattr, del);
// }

std::vector<double> nuisconfig::GetVD(XMLNodePointer_t node,
                                      std::string const &name,
                                      char const *del) {
  std::string tempattr = Get(node, name);
  return GeneralUtils::ParseToDbl(tempattr, del);
}

std::vector<std::string> nuisconfig::GetAllKeysForNode(XMLNodePointer_t node) {
  //bool matching = true;
  XMLAttrPointer_t attr = fXML->GetFirstAttr(node);
  std::vector<std::string> keys;
  while (attr != 0) {
    if (!std::string(fXML->GetAttrName(attr)).empty()) {
      keys.push_back(std::string(fXML->GetAttrName(attr)));
    }
    attr = fXML->GetNextAttr(attr);
  }

  return keys;
}

XMLNodePointer_t nuisconfig::GetConfigNode(std::string const &name) {
  // Loop over children and look for name
  XMLNodePointer_t child = fXML->GetChild(fMainNode);
  while (child != 0) {
    // Select only config parameters
    if (!std::string(fXML->GetNodeName(child)).compare("config")) {
      // Loop over config attributes and search for name
      XMLAttrPointer_t attr = fXML->GetFirstAttr(child);
      while (attr != 0) {
        // Save name value
        if (std::string(fXML->GetAttrName(attr)) == name.c_str()) {
          return child;
        }

        // Get Next Attribute
        attr = fXML->GetNextAttr(attr);
      }
    }

    // Next Child
    child = fXML->GetNext(child);
  }

  return 0;
}

void nuisconfig::SetConfig(std::string const &name, std::string const &val) {
  XMLNodePointer_t node = GetConfigNode(name);
  if (!node) node = CreateNode("config");
  Set(node, name, val);
}
void nuisconfig::SetConfig(std::string const &name, char const *val) {
  SetConfig(name, std::string(val));
}

void nuisconfig::SetConfig(std::string const &name, bool val) {
  XMLNodePointer_t node = GetConfigNode(name);
  if (!node) node = CreateNode("config");
  Set(node, name, val);
}

void nuisconfig::SetConfig(std::string const &name, int val) {
  XMLNodePointer_t node = GetConfigNode(name);
  if (!node) node = CreateNode("config");
  Set(node, name, val);
}

void nuisconfig::SetConfig(std::string const &name, float val) {
  XMLNodePointer_t node = GetConfigNode(name);
  if (!node) node = CreateNode("config");
  Set(node, name, val);
}

void nuisconfig::SetConfig(std::string const &name, double val) {
  XMLNodePointer_t node = GetConfigNode(name);
  if (!node) node = CreateNode("config");
  Set(node, name, val);
}

void nuisconfig::OverrideConfig(std::string const &conf) {
  std::vector<std::string> opts = GeneralUtils::ParseToStr(conf, "=");
  SetConfig(opts[0], opts[1]);
}

std::string nuisconfig::GetConfig(std::string const &name) {
  XMLNodePointer_t node = GetConfigNode(name);
  if (!node) return "";

  XMLAttrPointer_t attr = fXML->GetFirstAttr(node);
  std::string temp = "";

  // Loop config attributes
  while (attr != 0) {
    // Find match
    if (std::string(fXML->GetAttrName(attr)) == name.c_str()) {
      temp = fXML->GetAttrValue(attr);
    }

    // Get Next Attribute
    attr = fXML->GetNextAttr(attr);
  }
  return temp;
}

bool nuisconfig::HasConfig(std::string const &name) {
  return bool(GetConfigNode(name));
}

std::string nuisconfig::GetConfigS(std::string const &name) {
  return GetConfig(name);
}

bool nuisconfig::GetConfigB(std::string const &name) {
  std::string pars = GetConfig(name);
  return GeneralUtils::StrToBool(pars);
}

int nuisconfig::GetConfigI(std::string const &name) {
  std::string pars = GetConfig(name);
  return GeneralUtils::StrToInt(pars);
}

float nuisconfig::GetConfigF(std::string const &name) {
  std::string pars = GetConfig(name);
  return GeneralUtils::StrToDbl(pars);
}

double nuisconfig::GetConfigD(std::string const &name) {
  std::string pars = GetConfig(name);
  return GeneralUtils::StrToDbl(pars);
}

std::string nuisconfig::GetParDIR(std::string const &parName) {
  std::string outstr = this->GetParS(parName);

  // Make replacements in the string
  const int nfiletypes = 2;
  const std::string filetypes[nfiletypes] = {"@data", "@nuisance"};
  std::string filerepl[nfiletypes] = {FitPar::GetDataBase(),
                                      FitPar::GetDataBase() + "/../"};

  for (int i = 0; i < nfiletypes; i++) {
    std::string findstring = filetypes[i];
    std::string replstring = filerepl[i];
    if (outstr.find(findstring) != std::string::npos) {
      outstr.replace(outstr.find(findstring), findstring.size(), filerepl[i]);
      break;
    }
  }

  return outstr;
};
