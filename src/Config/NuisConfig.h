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
#ifndef NUISCONFIG_H_SEEN
#define NUISCONFIG_H_SEEN

#include <algorithm>
#include <map>

#include "TFile.h"
#include "TXMLEngine.h"

/// NUISANCE Global Settings Class
class nuisconfig {
 public:
  /// Singleton Get Function
  static nuisconfig &GetConfig(void);

  /// Constructor \n
  /// Loads defaults from $NUISANCE/parameters/config.xml
  nuisconfig();

  /// Desctructor \n Frees XML Docs
  virtual ~nuisconfig();

  /// Adds a new configuration list
  void LoadSettings(std::string const &filename, std::string const &state);

  /// Adds a new config from new xml file format
  void LoadXMLSettings(std::string const &filename,
                       std::string const &state = "");

  /// Adds a new config from old card file format
  void LoadCardSettings(std::string const &filename, std::string const &state);

  /// Save the config to file
  void WriteSettings(std::string const &filename);

  void PrintXML(XMLNodePointer_t node, int indent = 0);

  XMLNodePointer_t CreateNode(std::string const &name);
  XMLNodePointer_t CreateNode(XMLNodePointer_t node, std::string const &name);

  void RemoveEmptyNodes();
  void RemoveIdenticalNodes();

  bool MatchingNodes(XMLNodePointer_t node1, XMLNodePointer_t node2);
  void PrintNode(XMLNodePointer_t node);
  void RemoveNode(XMLNodePointer_t node);

  void FinaliseSettings(std::string const &name);

  XMLNodePointer_t CreateSampleNodeFromLine(std::string const &line);
  XMLNodePointer_t CreateParameterNodeFromLine(std::string const &line);
  XMLNodePointer_t CreatePullNodeFromLine(std::string const &line);
  XMLNodePointer_t CreateOldConfigNodeFromLine(std::string const &line);

  // Get List of child nodes of nuisance element
  std::vector<XMLNodePointer_t> GetNodes(std::string const &type = "");
  std::vector<XMLNodePointer_t> GetNodes(XMLNodePointer_t node,
                                         std::string const &type = "");
  XMLNodePointer_t GetNode(std::string const &type = "");
  XMLNodePointer_t GetNode(XMLNodePointer_t node, std::string const &type = "");

  std::vector<std::string> GetAllKeysForNode(XMLNodePointer_t node);

  std::string GetElementName(XMLNodePointer_t node) {
    return fXML->GetNodeName(node);
  }

  /// Check node has key name
  bool Has(XMLNodePointer_t node, std::string const &name);

  /// Add attribute to node
  void Set(XMLNodePointer_t node, std::string const &name,
           std::string const &val);
  void Set(XMLNodePointer_t node, std::string const &name,
           char const *val);
  void Set(XMLNodePointer_t node, std::string const &name, bool val);
  void Set(XMLNodePointer_t node, std::string const &name, int val);
  void Set(XMLNodePointer_t node, std::string const &name, float val);
  void Set(XMLNodePointer_t node, std::string const &name, double val);

  void SetS(XMLNodePointer_t node, std::string const &name,
            std::string const &val);
  void SetB(XMLNodePointer_t node, std::string const &name, bool val);
  void SetI(XMLNodePointer_t node, std::string const &name, int val);
  void SetF(XMLNodePointer_t node, std::string const &name, float val);
  void SetD(XMLNodePointer_t node, std::string const &name, double val);

  /// Get String from a given node
  std::string Get(XMLNodePointer_t node, std::string const &name);
  std::string GetS(XMLNodePointer_t node, std::string const &name);
  bool GetB(XMLNodePointer_t node, std::string const &name);
  int GetI(XMLNodePointer_t node, std::string const &name);
  float GetF(XMLNodePointer_t node, std::string const &name);
  double GetD(XMLNodePointer_t node, std::string const &name);

  // Get values parsed into a vector
  std::vector<std::string> GetVS(XMLNodePointer_t node, std::string const &name,
                                 const char *del);
  std::vector<int> GetVI(XMLNodePointer_t node, std::string const &name,
                         const char *del);
  std::vector<double> GetVD(XMLNodePointer_t node, std::string const &name,
                            const char *del);

  /// Set an already set config value to something else
  void OverrideConfig(std::string const &conf);

  /// Return the node of a given config parameter
  XMLNodePointer_t GetConfigNode(std::string const &name);

  void SetConfig(std::string const &name, std::string const &val);
  void SetConfig(std::string const &name, char const *val);
  void SetConfig(std::string const &name, bool val);
  void SetConfig(std::string const &name, int val);
  void SetConfig(std::string const &name, float val);
  void SetConfig(std::string const &name, double val);

  void SetParS(std::string const &name, std::string const &val);
  void SetParB(std::string const &name, bool val);
  void SetParI(std::string const &name, int val);
  void SetParD(std::string const &name, double val);

  std::string GetConfig(std::string const &name);
  bool HasConfig(std::string const &name);
  std::string GetConfigS(std::string const &name);
  bool GetConfigB(std::string const &name);
  int GetConfigI(std::string const &name);
  float GetConfigF(std::string const &name);
  double GetConfigD(std::string const &name);

  std::string GetPar(std::string const &name) { return GetConfig(name); };
  std::string GetParS(std::string const &name) { return GetConfigS(name); };
  bool GetParB(std::string const &name) { return GetConfigB(name); };
  int GetParI(std::string const &name) { return GetConfigI(name); };
  float GetParF(std::string const &name) { return GetConfigF(name); };
  double GetParD(std::string const &name) { return GetConfigD(name); };

  std::string GetParDIR(std::string const &parName);

  TFile *out;

 private:
  XMLNodePointer_t fMainNode;             ///< Main XML Parent Node
  TXMLEngine *fXML;                       ///< ROOT XML Engine
  std::vector<XMLDocPointer_t> fXMLDocs;  ///< List of all XML document inputs

 protected:
  static nuisconfig *m_nuisconfigInstance;
};

namespace Config {
nuisconfig &Get();

std::string GetPar(std::string const &name);
bool HasPar(std::string const &name);
std::string GetParS(std::string const &name);
bool GetParB(std::string const &name);
int GetParI(std::string const &name);
float GetParF(std::string const &name);
double GetParD(std::string const &name);

void SetPar(std::string const &name, std::string const &val);
void SetPar(std::string const &name, char const *val);
void SetPar(std::string const &name, bool val);
void SetPar(std::string const &name, int val);
void SetPar(std::string const &name, float val);
void SetPar(std::string const &name, double val);
}

namespace FitPar {
nuisconfig &Config();
std::string GetDataBase();
}

/*! @} */
#endif
