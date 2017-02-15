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

nuisconfig::nuisconfig(){

  // Initial Setup
  std::string filename = GeneralUtils::GetTopLevelDir() + "/parameters/config.xml"; 
  std::cout << "[NUIS]: Loading DEFAULT config from : " << filename;
  
  // Create XML Engine
  fXML = new TXMLEngine;
  
  // Load in documents
  fXMLDocs.clear();
  fXMLDocs.push_back( fXML->ParseFile(filename.c_str()) );
  
  // Setup Main XML Node
  fMainNode = fXML->DocGetRootElement( fXMLDocs[0] );
  
  std::cout << "  ->  DONE. " << std::endl;


}

nuisconfig::~nuisconfig(){
  
  // Free all xml docs
  //  for (int i = 0; i < fXMLDocs.size(); i++){
  //    fXML->FreeDoc( fXMLDocs.at(i) );
  //  }
  
  // Remove XMLDocs
  //fXMLDocs.clear();

  // Delete Engine
  //delete fXML;

}

void nuisconfig::LoadConfig(std::string filename, std::string state=""){
  
  // Add new file to xml docs list
  fXMLDocs.push_back( fXML->ParseFile( filename.c_str() ) );

  // Get New Doc ROOT
  int nxml = fXMLDocs.size();
  XMLNodePointer_t newdocroot = fXML->DocGetRootElement( fXMLDocs[nxml-1] );

  // Loop over children and add
  XMLNodePointer_t child = fXML->GetChild( newdocroot );
  while( child!=0 ){ 
    // Add additional state flag if given
    if (!state.empty()){
      fXML->NewAttr(child, 0,"state",state.c_str());
    }

    // Add this child to the main config list
    fXML->AddChild( fMainNode, child );
    
    // Get Next Child
    child = fXML->GetNext(child);
  }
}

XMLNodePointer_t nuisconfig::CreateNode(std::string name){
  return fXML->NewChild(fMainNode, 0, name.c_str());
}

void nuisconfig::WriteConfig(std::string outputname){
  // Create a New XML Doc
  XMLDocPointer_t newxmldoc = fXML->NewDoc();
  fXML->DocSetRootElement(newxmldoc, fMainNode);

  // Save document to file                                                                                                                                                                           
  fXML->SaveDoc(newxmldoc, outputname.c_str());
}

std::string nuisconfig::ConfS(const std::string name){  
  std::string temp = "";
  
  // Loop over children and look for name
  XMLNodePointer_t child = fXML->GetChild(fMainNode);
  while (child!=0) {

    // Select only config parameters
    if (std::string(fXML->GetNodeName(child)) == "config"){
      
      // Loop over config attributes and search for name
      XMLAttrPointer_t attr = fXML->GetFirstAttr(child);
      while ( attr!=0 ){

	// Save name value
	if (std::string(fXML->GetAttrName(attr)) == name.c_str()){
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

bool nuisconfig::ConfB(const std::string name){
  std::string pars = ConfS(name);
  return GeneralUtils::StrToBool(pars);
}

int nuisconfig::ConfI(const std::string name){
  std::string pars = ConfS(name);
  return GeneralUtils::StrToInt(pars);
}

double nuisconfig::ConfD(const std::string name){
  std::string pars = ConfS(name);
  return GeneralUtils::StrToDbl(pars);
}

std::vector<XMLNodePointer_t> nuisconfig::GetNodes(const std::string type){
  std::vector<XMLNodePointer_t> nodelist;

  /// Loop over all children
  XMLNodePointer_t child = fXML->GetChild(fMainNode);
  while (child!=0) {

    /// Get nodes for given type (if type empty return all)
    if (std::string(fXML->GetNodeName(child)) == type.c_str() or type.empty()){
      nodelist.push_back(child);
    }

    // Next child
    child = fXML->GetNext(child);
  }
  
  // return list
  return nodelist;
}


/// Get String from a given node                                                                                           
std::string nuisconfig::GetS(XMLNodePointer_t node, std::string name){

  // If node empty return empty
  if (node == 0) return "";

  // Get Attribute from child with name
  XMLAttrPointer_t attr = fXML->GetFirstAttr(node);
  std::string temp = "";

  // Check if its a search or exact (should probs just add wildcards...)
  bool exact = true;
  if (name.size() > 0){
    if (name[0] == '*' and name[name.size()-1] == '*'){
      exact = false;
    }
  }
    
  // Loop over all attributes
  while ( attr!=0 ){
    
    // Find value of correct name
    if (exact){
      if (std::string(fXML->GetAttrName(attr)) == name.c_str()){
	temp = fXML->GetAttrValue(attr);
      }
    } else {
      
    }

    // Next Attribute
    attr = fXML->GetNextAttr(attr);
  }

  return temp;
}

/// Get Bools from a given node                                                                                            
bool nuisconfig::GetB(XMLNodePointer_t node, std::string name){
  std::string tempattr = GetS(node, name);
  return GeneralUtils::StrToBool(tempattr);
}

/// Get int from given node                                                                                                
int nuisconfig::GetI(XMLNodePointer_t node, std::string name){
  std::string tempattr = GetS(node, name);
  return GeneralUtils::StrToInt(tempattr);
}

/// Get double from given node                                                                                             
double nuisconfig::GetD(XMLNodePointer_t node, std::string name){
  std::string tempattr = GetS(node, name);
  return GeneralUtils::StrToDbl(tempattr);
}

std::vector<std::string> nuisconfig::GetVS(XMLNodePointer_t node, std::string name, const char* del){
  std::string tempattr = GetS(node, name);
  return GeneralUtils::ParseToStr(tempattr,del);
}

std::vector<int> nuisconfig::GetVI(XMLNodePointer_t node, std::string name, const char* del){
  std::string tempattr = GetS(node, name);
  return GeneralUtils::ParseToInt(tempattr,del);
}

std::vector<double> nuisconfig::GetVD(XMLNodePointer_t node, std::string name, const char* del){
  std::string tempattr = GetS(node, name);
  return GeneralUtils::ParseToDbl(tempattr,del);
}


namespace Config {
  nuisconfig& Get() { return nuisconfig::GetConfig(); };
}

nuisconfig* nuisconfig::m_nuisconfigInstance = NULL;
nuisconfig& nuisconfig::GetConfig(void) {
  if (!m_nuisconfigInstance)  m_nuisconfigInstance = new nuisconfig;
  return *m_nuisconfigInstance;
};

