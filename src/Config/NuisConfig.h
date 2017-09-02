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
#ifndef NUISCONFIG_H_SEEN
#define NUISCONFIG_H_SEEN

#include <algorithm>
#include <map>

#include "TXMLEngine.h"
#include "TFile.h"

/// NUISANCE Global Settings Class
class nuisconfig {
public:

  /// Singleton Get Function
  static nuisconfig& GetConfig(void);

  /// Constructor \n
  /// Loads defaults from $NUISANCE/parameters/config.xml
  nuisconfig();

  /// Desctructor \n Frees XML Docs
  virtual ~nuisconfig();

  /// Adds a new configuration list
  void LoadSettings(std::string filename, std::string state);

  /// Adds a new config from new xml file format
  void LoadXMLSettings(std::string filename, std::string state);

  /// Adds a new config from old card file format
  void LoadCardSettings(std::string filename, std::string state);

  /// Save the config to file
  void WriteSettings(std::string filename);

  XMLNodePointer_t CreateNode(std::string name);
  XMLNodePointer_t CreateNode(XMLNodePointer_t node, std::string name);

  void RemoveEmptyNodes();
  void RemoveIdenticalNodes();

  bool MatchingNodes(XMLNodePointer_t node1, XMLNodePointer_t node2);
  void PrintNode(XMLNodePointer_t node);
  void RemoveNode(XMLNodePointer_t node);

  void FinaliseSettings(std::string name);

  XMLNodePointer_t CreateSampleNodeFromLine(const std::string line);
  XMLNodePointer_t CreateParameterNodeFromLine(const std::string line);
  XMLNodePointer_t CreatePullNodeFromLine(const std::string line);
  XMLNodePointer_t CreateOldConfigNodeFromLine(const std::string line);

  // Get List of child nodes of nuisance element
  std::vector<XMLNodePointer_t> GetNodes(std::string type = "");
  std::vector<XMLNodePointer_t> GetNodes(XMLNodePointer_t node, std::string type = "");
  XMLNodePointer_t GetNode(std::string type = "");
  XMLNodePointer_t GetNode(XMLNodePointer_t node, std::string type = "");

  std::vector<std::string> GetAllKeysForNode(XMLNodePointer_t node);

  std::string GetElementName(XMLNodePointer_t node) { return fXML->GetNodeName(node); }

  /// Check node has key name
  bool Has(XMLNodePointer_t node, std::string name);

  /// Add attribute to node
  void Set(XMLNodePointer_t node, std::string name, std::string val);
  void Set(XMLNodePointer_t node, std::string name, bool val);
  void Set(XMLNodePointer_t node, std::string name, int val);
  void Set(XMLNodePointer_t node, std::string name, float val);
  void Set(XMLNodePointer_t node, std::string name, double val);

  void SetS(XMLNodePointer_t node, std::string name, std::string val);
  void SetB(XMLNodePointer_t node, std::string name, bool val);
  void SetI(XMLNodePointer_t node, std::string name, int val);
  void SetF(XMLNodePointer_t node, std::string name, float val);
  void SetD(XMLNodePointer_t node, std::string name, double val);

  /// Get String from a given node
  std::string Get(XMLNodePointer_t node, std::string name);
  std::string GetS(XMLNodePointer_t node, std::string name);
  bool        GetB(XMLNodePointer_t node, std::string name);
  int         GetI(XMLNodePointer_t node, std::string name);
  float       GetF(XMLNodePointer_t node, std::string name);
  double      GetD(XMLNodePointer_t node, std::string name);

  // Get values paresed into a vector
  std::vector<std::string> GetVS(XMLNodePointer_t node, std::string name,
                                 const char* del);
  std::vector<int> GetVI(XMLNodePointer_t node, std::string name,
                         const char* del);
  std::vector<double> GetVD(XMLNodePointer_t node, std::string name,
                            const char* del);


  /// Set an already set config value to something else
  void OverrideConfig(std::string conf);

  /// Return the node of a given config parameter
  XMLNodePointer_t GetConfigNode(std::string name);

  void SetConfig(std::string name, std::string val);
  void SetConfig(std::string name, bool val);
  void SetConfig(std::string name, int val);
  void SetConfig(std::string name, float val);
  void SetConfig(std::string name, double val);

  void SetParS(std::string name, std::string val);
  void SetParB(std::string name, bool val);
  void SetParI(std::string name, int val);
  void SetParD(std::string name, double val);

  std::string GetConfig(const std::string name);
  std::string GetConfigS(const std::string name);
  bool        GetConfigB(const std::string name);
  int         GetConfigI(const std::string name);
  float       GetConfigF(const std::string name);
  double      GetConfigD(const std::string name);

  std::string GetPar (const std::string name) { return GetConfig(name);  };
  std::string GetParS(const std::string name) { return GetConfigS(name); };
  bool        GetParB(const std::string name) { return GetConfigB(name); };
  int         GetParI(const std::string name) { return GetConfigI(name); };
  float       GetParF(const std::string name) { return GetConfigF(name); };
  double      GetParD(const std::string name) { return GetConfigD(name); };

  std::string GetParDIR(std::string parName);

  TFile* out;

private:

  XMLNodePointer_t fMainNode;             ///< Main XML Parent Node
  TXMLEngine* fXML;                       ///< ROOT XML Engine
  std::vector<XMLDocPointer_t> fXMLDocs;  ///< List of all XML document inputs

protected:

  static nuisconfig* m_nuisconfigInstance;
};

namespace Config {
nuisconfig& Get();

std::string GetPar(std::string name);
std::string GetParS(std::string name);
bool        GetParB(std::string name);
int         GetParI(std::string name);
float       GetParF(std::string name);
double      GetParD(std::string name);

void SetPar(std::string name, std::string val);
void SetPar(std::string name, bool val);
void SetPar(std::string name, int val);
void SetPar(std::string name, float val);
void SetPar(std::string name, double val);

}

namespace FitPar {
nuisconfig& Config();
std::string GetDataBase();
}

/*! @} */
#endif
