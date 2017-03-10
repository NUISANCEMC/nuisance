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
#include "FitParameters.h"

nuisconfig::nuisconfig() {

  // Initial Setup
  std::string filename = GeneralUtils::GetTopLevelDir() + "/parameters/config.xml";
  LOG(FIT) << "Loading DEFAULT config from : " << filename;
  // Create XML Engine
  fXML = new TXMLEngine;

  // Load in documents
  fXMLDocs.clear();
  fXML->SetSkipComments(true);
  fXMLDocs.push_back( fXML->ParseFile(filename.c_str()) );
  fXML->SetSkipComments(false);

  // Setup Main XML Node
  fMainNode = fXML->DocGetRootElement( fXMLDocs[0] );
  //RemoveIdenticalNodes();

  std::cout << " -> DONE." << std::endl;


}

nuisconfig::~nuisconfig() {

  // Free all xml docs
  //  for (int i = 0; i < fXMLDocs.size(); i++){
  //    fXML->FreeDoc( fXMLDocs.at(i) );
  //  }

  // Remove XMLDocs
  //fXMLDocs.clear();

  // Delete Engine
  //delete fXML;

}

void nuisconfig::OverrideConfig(std::string conf) {
  std::vector<std::string> opts = GeneralUtils::ParseToStr(conf, "=");
  SetConfS(opts[0], opts[1]);
}


XMLNodePointer_t nuisconfig::GetConfigNode(std::string name) {

  // Loop over children and look for name
  XMLNodePointer_t child = fXML->GetChild(fMainNode);
  while (child != 0) {

    // std::cout << "Child = " << fXML->GetNodeName(child) << std::endl;
    // Select only config parameters
    if (!std::string(fXML->GetNodeName(child)).compare("config")) {

      // std::cout << "Found Config " << std::endl;
      // Loop over config attributes and search for name
      XMLAttrPointer_t attr = fXML->GetFirstAttr(child);
      while ( attr != 0 ) {

        // Save name value
        // std::cout << "Setting Temp " << std::string(fXML->GetAttrName(attr)) << " '" << fXML->GetAttrValue(attr) << "' " << std::endl;
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

/// Request a string config key
std::string nuisconfig::SetConfS(const std::string name, std::string val) {
  XMLNodePointer_t node = GetConfigNode(name);
  if (!node) node = CreateNode("config");
  SetS(node, name, val);
}

/// Get nuisconfig::SetConfig Bool
bool nuisconfig::SetConfB(const std::string name, bool val) {
  XMLNodePointer_t node = GetConfigNode(name);
  if (!node) node = CreateNode("config");
  SetB(node, name, val);
}

/// Get nuisconfig::SetConfig Int
int nuisconfig::SetConfI(const std::string name, int val) {
  XMLNodePointer_t node = GetConfigNode(name);
  if (!node) node = CreateNode("config");
  SetI(node, name, val);
}

/// Get nuisconfig::SetConfig Double
double nuisconfig::SetConfD(const std::string name, double val) {
  XMLNodePointer_t node = GetConfigNode(name);
  if (!node) node = CreateNode("config");
  SetD(node, name, val);
}


std::string nuisconfig::ConvertParameterLineToXML(std::string line) {

  // Parse
  std::vector<std::string> parsed = GeneralUtils::ParseToStr(line, " ");

  // Min limits
  if (parsed.size() < 2) {
    ERR(FTL) << " Insufficient parameter options" << std::endl;
    throw;
  }

  // Setup XMLLine
  std::string xmlline = "parameter";

  // Name
  xmlline += " name=\"" + parsed[0] + "\"";

  // Nominal
  xmlline += " nominal=\"" + parsed[1] + "\"";

  // State
  xmlline += " state=\"" + parsed[2] + "\"";

  return "<" + xmlline + "/>";
}

std::string nuisconfig::ConvertSampleLineToXML(std::string line) {

  // Parse
  std::vector<std::string> parsed = GeneralUtils::ParseToStr(line, " ");

  // Min limits
  if (parsed.size() < 2) {
    ERR(FTL) << "Insufficient sample options" << std::endl;
  }

  // Setup XMLLine
  std::string xmlline = "sample";

  // Name
  xmlline += " name=\"" + parsed[1] + "\"";

  // InputFile
  xmlline += " input=\"" + parsed[2] + "\"";

  // If option add it
  if (parsed.size() > 3) {
    xmlline += " state=\"" + parsed[3] + "\"";
  }

  // If norm add it
  if (parsed.size() > 4) {
    xmlline += " norm=\"" + parsed[4] + "\"";
  }

  return "<" + xmlline + "/>";
}


void nuisconfig::AddXMLLine(std::string line) {

  // LOG(FIT) << "Adding XMLLine in nuisconfig " << std::endl;

  // XMLLine
  std::string xmlline = "";

  // If = in it its not an xml
  if (line.find("=") != std::string::npos) {
    xmlline = "<" + line + "/>";

    // Else Convert it to a line
  } else {

    // Parse XMLLine
    std::vector<std::string> parsed = GeneralUtils::ParseToStr(line, " ");
    if (parsed.empty()) return;

    // Convert it to new fomat
    if (!parsed[0].compare("sample")) {
      xmlline = ConvertSampleLineToXML(line);
    } else if (!parsed[0].compare("sample")) {
      xmlline = ConvertParameterLineToXML(line);
    }
  }

  // Ad the line
  LOG(FIT) << "Adding line to config: " << xmlline;
  // fXML->AddRawLine(fMainNode, xmlline.c_str());

  // Make XML Structure
  fXMLDocs.push_back( fXML->ParseString(xmlline.c_str()) );

  int nxml = fXMLDocs.size();
  XMLNodePointer_t newdocroot = fXML->DocGetRootElement( fXMLDocs[nxml - 1] );
  fXML->AddChild( fMainNode, newdocroot );
  /*
  // Loop over children and add
  XMLNodePointer_t child = fXML->GetChild( newdocroot );
  while ( child != 0 ) {
    // Add additional state flag if given
    std::string state = "";
    if (!state.empty()) {
      if (GetS(child, "source").empty()) {
        fXML->NewAttr(child, 0, "source", state.c_str());
      } else {
        // fXML->SetAttr
      }

      // If its a config node, then remove previous attributes, overriding
      if (!std::string(fXML->GetNodeName(child)).compare("config")) {

        // Loop over attribues
        XMLAttrPointer_t attr1 = fXML->GetFirstAttr(child);
        while ( attr1 != 0 ) {
          if (!ConfS(fXML->GetAttrName(attr1)).empty()) {
            std::vector<XMLNodePointer_t> confignodes = GetNodes("config");

            for (size_t i = 0; i < confignodes.size(); i++) {
              if (fXML->HasAttr(confignodes[i], fXML->GetAttrName(attr1))) {
                std::cout << fXML->GetAttrName(attr1) << std::endl;
                fXML->FreeAttr(confignodes[i], fXML->GetAttrName(attr1));
                break;
              }
            }
          }
          attr1 = fXML->GetNextAttr(attr1);
        }
      }
    }

    std::cout << "Adding Child " << std::endl;
    // Add this child to the main config list
    fXML->AddChild( fMainNode, child );

    // Get Next Child
    child = fXML->GetNext(child);
  }
  */
  // std::cout << "Removing Identical Nodes" << std::endl;
  std::cout << " -> DONE." << std::endl;

}

void nuisconfig::FinaliseConfig(std::string name) {

  // Save full config to file
  WriteConfig(name);
  RemoveEmptyNodes();
  RemoveIdenticalNodes();
  LOG(FIT) << "Finished setting up config -> DONE." << std::endl;
}

void nuisconfig::LoadXMLConfig(std::string filename, std::string state = "") {

  LOG(FIT) << "Loading XML config from : " << filename;
  // Add new file to xml docs list
  fXMLDocs.push_back( fXML->ParseFile( filename.c_str() ) );

  // Get New Doc ROOT
  int nxml = fXMLDocs.size();
  XMLNodePointer_t newdocroot = fXML->DocGetRootElement( fXMLDocs[nxml - 1] );


  // Loop over children and add
  XMLNodePointer_t child = fXML->GetChild( newdocroot );
  while ( child != 0 ) {
    // Add additional state flag if given
    if (!state.empty()) {
      if (GetS(child, "source").empty()) {
        fXML->NewAttr(child, 0, "source", state.c_str());
      } else {
        // fXML->SetAttr
      }

      // If its a config node, then remove previous attributes, overriding
      if (!std::string(fXML->GetNodeName(child)).compare("config")) {

        // Loop over attribues
        XMLAttrPointer_t attr1 = fXML->GetFirstAttr(child);
        while ( attr1 != 0 ) {
          if (!ConfS(fXML->GetAttrName(attr1)).empty()) {
            std::vector<XMLNodePointer_t> confignodes = GetNodes("config");

            for (size_t i = 0; i < confignodes.size(); i++) {
              if (fXML->HasAttr(confignodes[i], fXML->GetAttrName(attr1))) {
                std::cout << fXML->GetAttrName(attr1) << std::endl;
                fXML->FreeAttr(confignodes[i], fXML->GetAttrName(attr1));
                break;
              }
            }
          }
          attr1 = fXML->GetNextAttr(attr1);
        }
      }
    }

    // Add this child to the main config list
    fXML->AddChild( fMainNode, child );

    // Get Next Child
    child = fXML->GetNext(child);
  }
  // std::cout << "Removing Identical Nodes" << std::endl;
  if (LOG_LEVEL(FIT)){
    std::cout << " -> DONE." << std::endl;
  }
}


void nuisconfig::LoadConfig(std::string filename, std::string state) {

  // Open file and see if its XML
  LOG(FIT) << "Trying to parse file : " << filename;
  // StopTalking();
  XMLDocPointer_t tempdoc = fXML->ParseFile( filename.c_str() );
  // StartTalking();

  if (tempdoc) {
    std::cout << " -> Found XML file." << std::endl;
    LoadXMLConfig(filename, state);
  } else {
    std::cout << " -> Assuming its a simple card file." << std::endl;
    LoadCardConfig(filename, state);
  }

}

void nuisconfig::LoadCardConfig(std::string filename, std::string state) {

  // Build XML Config from the card file
  std::vector<std::string> cardlines =
    GeneralUtils::ParseFileToStr(filename, "\n");
  int linecount = 0;

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

    // // Build backwards compatible xml configs
    // if (!id.compare("sample")) Config::CreateSampleKeyFromLine(line);

    // if (id.find("_parameter") != std::string::npos)
    //   Config::CreateParameterKeyFromLine(line);

    // if (!id.compare("covar") ||
    //     !id.compare("pull")  ||
    //     !id.compare("throw"))
    //   Config::CreatePullKeyFromLine(line);

    // if (!id.compare("config"))
    //   Config::CreateOldConfigKeyFromLine(line);
  }


  return;

}



XMLNodePointer_t nuisconfig::CreateNode(std::string name) {
  return fXML->NewChild(fMainNode, 0, name.c_str());
}

void nuisconfig::WriteConfig(std::string outputname) {
  // Create a New XML Doc
  XMLDocPointer_t newxmldoc = fXML->NewDoc();
  fXML->DocSetRootElement(newxmldoc, fMainNode);

  // Save document to file
  fXML->SaveDoc(newxmldoc, outputname.c_str());
}


void nuisconfig::CheckCallCount(std::string name) {

  // Add Count Warning Flag so we only warn once...
  if (fConfigCallWarning.find(name) == fConfigCallWarning.end()) {
    fConfigCallWarning[name] = false;
    fConfigCallCount[name] = 0;
  }

  // Check for inefficiency and warn if it happens
  if (abs(time(NULL) - fCurrentTime) > 1) {
    fCurrentTime = time(NULL);

    // Check count since last 10 seconds
    if (!fConfigCallWarning[name] and fConfigCallCount[name] > 100) {
      ERR(WRN) << "Config Parameter " << name
               << " has been requested " << fConfigCallCount[name]
               << " times in the last second." << std::endl;
      ERR(WRN) << "This is very inefficient! Please try to change this." << std::endl;
      fConfigCallWarning[name] = true;
    }

    // Reset counter
    fConfigCallCount[name] = 0;
  }

  // Add to Call Count
  fConfigCallCount[name] += 1;

}

std::string nuisconfig::ConfS(const std::string name) {
  std::string temp = "";

  CheckCallCount(name);

  // Loop over children and look for name
  XMLNodePointer_t child = fXML->GetChild(fMainNode);
  while (child != 0) {

    // std::cout << "Child = " << fXML->GetNodeName(child) << std::endl;
    // Select only config parameters
    if (!std::string(fXML->GetNodeName(child)).compare("config")) {

      // std::cout << "Found Config " << std::endl;
      // Loop over config attributes and search for name
      XMLAttrPointer_t attr = fXML->GetFirstAttr(child);
      while ( attr != 0 ) {

        // Save name value
        // std::cout << "Setting Temp " << std::string(fXML->GetAttrName(attr)) << " '" << fXML->GetAttrValue(attr) << "' " << std::endl;
        if (std::string(fXML->GetAttrName(attr)) == name.c_str()) {
          // std::cout << "Setting Temp " << std::string(fXML->GetAttrName(attr)) << " " << fXML->GetAttrValue(attr) << std::endl;
          temp = fXML->GetAttrValue(attr);
        }

        // Get Next Attribute
        attr = fXML->GetNextAttr(attr);
      }
    }

    // Next Child
    child = fXML->GetNext(child);
  }

  // Return Config Value
  return temp;
}

bool nuisconfig::ConfB(const std::string name) {
  std::string pars = ConfS(name);
  return GeneralUtils::StrToBool(pars);
}

int nuisconfig::ConfI(const std::string name) {
  std::string pars = ConfS(name);
  return GeneralUtils::StrToInt(pars);
}

double nuisconfig::ConfD(const std::string name) {
  std::string pars = ConfS(name);
  return GeneralUtils::StrToDbl(pars);
}

std::vector<XMLNodePointer_t> nuisconfig::GetNodes(const std::string type) {
  std::vector<XMLNodePointer_t> nodelist;

  /// Loop over all children
  XMLNodePointer_t child = fXML->GetChild(fMainNode);
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


/// Get String from a given node
std::string nuisconfig::GetS(XMLNodePointer_t node, std::string name) {

  // If node empty return empty
  if (node == 0) return "";

  // Check request count
  CheckCallCount(name);

  // Get Attribute from child with name
  XMLAttrPointer_t attr = fXML->GetFirstAttr(node);
  std::string temp = "";

  // Check if its a search or exact (should probs just add wildcards...)
  bool exact = true;
  if (name.size() > 0) {
    if (name[0] == '*' and name[name.size() - 1] == '*') {
      exact = false;
    }
  }

  // Loop over all attributes
  while ( attr != 0 ) {

    // Find value of correct name
    if (exact) {
      if (std::string(fXML->GetAttrName(attr)) == name.c_str()) {
        temp = fXML->GetAttrValue(attr);
      }
    } else {

    }

    // Next Attribute
    attr = fXML->GetNextAttr(attr);
  }

  return temp;
}

bool nuisconfig::Has(XMLNodePointer_t node, std::string name) {

  // If node empty return empty
  if (node == 0) return false;

  // Get Attribute from child with name
  XMLAttrPointer_t attr = fXML->GetFirstAttr(node);
  bool found = false;

  // Check if its a search or exact (should probs just add wildcards...)
  bool exact = true;
  if (name.size() > 0) {
    if (name[0] == '*' and name[name.size() - 1] == '*') {
      exact = false;
    }
  }

  // Loop over all attributes
  while ( attr != 0 ) {

    // Find value of correct name
    if (exact) {
      if (std::string(fXML->GetAttrName(attr)) == name.c_str()) {
        found = true;
      }
    } else {

    }

    // Next Attribute
    attr = fXML->GetNextAttr(attr);
  }

  return found;
}

/// Get Bools from a given node
bool nuisconfig::GetB(XMLNodePointer_t node, std::string name) {
  std::string tempattr = GetS(node, name);
  return GeneralUtils::StrToBool(tempattr);
}

/// Get int from given node
int nuisconfig::GetI(XMLNodePointer_t node, std::string name) {
  std::string tempattr = GetS(node, name);
  return GeneralUtils::StrToInt(tempattr);
}

/// Get double from given node
double nuisconfig::GetD(XMLNodePointer_t node, std::string name) {
  std::string tempattr = GetS(node, name);
  return GeneralUtils::StrToDbl(tempattr);
}

std::vector<std::string> nuisconfig::GetVS(XMLNodePointer_t node, std::string name, const char* del) {
  std::string tempattr = GetS(node, name);
  return GeneralUtils::ParseToStr(tempattr, del);
}

std::vector<int> nuisconfig::GetVI(XMLNodePointer_t node, std::string name, const char* del) {
  std::string tempattr = GetS(node, name);
  return GeneralUtils::ParseToInt(tempattr, del);
}

std::vector<double> nuisconfig::GetVD(XMLNodePointer_t node, std::string name, const char* del) {
  std::string tempattr = GetS(node, name);
  return GeneralUtils::ParseToDbl(tempattr, del);
}


namespace Config {
nuisconfig& Get() { return nuisconfig::GetConfig(); };
}

nuisconfig* nuisconfig::m_nuisconfigInstance = NULL;
nuisconfig& nuisconfig::GetConfig(void) {
  if (!m_nuisconfigInstance)  m_nuisconfigInstance = new nuisconfig;
  return *m_nuisconfigInstance;
};


void nuisconfig::AddS(XMLNodePointer_t node, std::string name, std::string val) {
  fXML->NewAttr(node, 0, name.c_str(), val.c_str());
}

void nuisconfig::AddB(XMLNodePointer_t node, std::string name, bool val) {
  AddS(node, name, GeneralUtils::BoolToStr(val));
}

void nuisconfig::AddI(XMLNodePointer_t node, std::string name, int val) {
  AddS(node, name, GeneralUtils::IntToStr(val));
}
void nuisconfig::AddD(XMLNodePointer_t node, std::string name, double val) {
  AddS(node, name, GeneralUtils::DblToStr(val));
}

void nuisconfig::SetS(XMLNodePointer_t node, std::string name, std::string val) {

  // Remove and readd attribute
  if (fXML->HasAttr(node, name.c_str())) {
    fXML->FreeAttr(node, name.c_str());
  }

  AddS(node, name, val);
}

void nuisconfig::SetB(XMLNodePointer_t node, std::string name, bool val) {
  SetS(node, name, GeneralUtils::BoolToStr(val));
}

void nuisconfig::SetI(XMLNodePointer_t node, std::string name, int val) {
  SetS(node, name, GeneralUtils::IntToStr(val));
}
void nuisconfig::SetD(XMLNodePointer_t node, std::string name, double val) {
  SetS(node, name, GeneralUtils::DblToStr(val));
}

void nuisconfig::ChangeS(XMLNodePointer_t node, std::string name, std::string val) {
  if (!fXML->HasAttr(node, name.c_str())) return;
  SetS(node, name, val);
}

void nuisconfig::ChangeB(XMLNodePointer_t node, std::string name, bool val) {
  ChangeS(node, name, GeneralUtils::BoolToStr(val));
}

void nuisconfig::ChangeI(XMLNodePointer_t node, std::string name, int val) {
  ChangeS(node, name, GeneralUtils::IntToStr(val));
}
void nuisconfig::ChangeD(XMLNodePointer_t node, std::string name, double val) {
  ChangeS(node, name, GeneralUtils::DblToStr(val));
}



void nuisconfig::RemoveEmptyNodes() {
  std::vector<XMLNodePointer_t> nodelist = Config::Get().GetNodes();
  for (size_t i = 0; i < nodelist.size(); i++) {
    if (fXML->IsEmptyNode(nodelist[i])) {
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

      XMLNodePointer_t node1 = nodelist[i];
      XMLNodePointer_t node2 = nodelist[j];

      // Check node already removed.
      if (std::find(removed.begin(), removed.end(), node1) != removed.end()) continue;
      if (std::find(removed.begin(), removed.end(), node2) != removed.end()) continue;
      if (i == j) continue;

      // Check matching
      if (!MatchingNodes(node1, node2)) continue;

      if (std::string(fXML->GetNodeName(node1)).compare("config") and fXML->IsEmptyNode(node1)) {

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
    RemoveNode( removed.at(i) );
  }

  return;
}


void nuisconfig::RemoveNode(XMLNodePointer_t node) {
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
  while ( attr != 0 ) {
    std::cout << " -> " << fXML->GetAttrName(attr) << " : " << fXML->GetAttrValue(attr) << std::endl;
    attr = fXML->GetNextAttr(attr);
  }

}

bool nuisconfig::MatchingNodes(XMLNodePointer_t node1, XMLNodePointer_t node2) {

  bool matching = true;
  XMLAttrPointer_t attr = fXML->GetFirstAttr(node1);
  while ( attr != 0 ) {
    if (GetS(node2, fXML->GetAttrName(attr)) != fXML->GetAttrValue(attr)) matching = false;
    attr = fXML->GetNextAttr(attr);
  }
  return matching;
}


std::string nuisconfig::GetTag(std::string name) {

  std::vector<XMLAttrPointer_t> alltags = GetNodes("tag");
  std::string tagval = "";
  bool tagfound = false;

  for (size_t i = 0; i < alltags.size(); i++) {
    if (!Has(alltags[i], name)) continue;
    tagval = GetS(alltags[i], name);
    tagfound = true;
  }

  if (!tagfound) {
    ERR(FTL) << "Cannot find tag " << name << " in global conig!" << std::endl;
    throw;
  }

  return tagval;
};

void nuisconfig::ExpandAllTags() {
  return;
  /*
  // Loop over children and look for name
  XMLNodePointer_t child = fXML->GetChild(fMainNode);
  while (child != 0) {

    // Loop over attributes and search for tags
    XMLAttrPointer_t attr = fXML->GetFirstAttr(child);
    while ( attr != 0 ) {

      // Search attribute value for <>
      std::string attrval = std::string(fXML->GetAttrValue(attr));
      std::string attrname = std::string(fXML->GetAttrName(attr));

      // Search for <>
      while (attrval.find("<") != std::string::npos and
             attrval.find(">") != std::string::npos){

        size_t startdel = 0;
        size_t enddel = 0;
        std::string replacestring = "";

        for (size_t i = 0; i < attrval.size(); i++){

          if (attrval[i] == '<'){
            replacestring = "";
            startdel = i;
          }

          replacestring.push_back(attrval[i]);

          if (attrval[i] == '>'){
            enddel = i;
            break;
          }
        }

        attrval.replace(startdel, replacestring, GetTag(replacestring) );
      }

      fXML->FreeAttr(child, attr);
      attr = fXML->NewAttr(child, attr, attrname.c_str(), attrval.c_str());

      // Get Next Attribute
      attr = fXML->GetNextAttr(attr);
    }
    child = fXML->GetNext(child);
  }
  */



};







