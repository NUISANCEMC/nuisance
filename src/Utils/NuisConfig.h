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
#include "TXMLEngine.h"
#include "GeneralUtils.h"
#include <algorithm>
#include <map>

// New NUISANCE Config Class
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
  void LoadConfig(std::string filename, std::string state);

  /// Save the config to file
  void WriteConfig(std::string filename);

  /// Request a string config key
  std::string ConfS(const std::string name);

  /// Get Config Bool
  bool ConfB(const std::string name);

  /// Get Config Int
  int ConfI(const std::string name);

  /// Get Config Double
  double ConfD(const std::string name);

  /// Node Functions
  void CheckCallCount(std::string name);

  // Get List of nodes
  std::vector<XMLNodePointer_t> GetNodes(std::string type="");

  /// Get String from a given node
  std::string GetS(XMLNodePointer_t node, std::string name);

  /// Get Bools from a given node
  bool GetB(XMLNodePointer_t node, std::string name);

  /// Get int from given node
  int GetI(XMLNodePointer_t node, std::string name);

  /// Get double from given node
  double GetD(XMLNodePointer_t node, std::string name);
  
  // Add Children to root node
  XMLNodePointer_t CreateNode(std::string name);

  // Add attribute to node
  void AddS(XMLNodePointer_t node, std::string name, std::string val);
  void AddB(XMLNodePointer_t node, std::string name, bool val);
  void AddI(XMLNodePointer_t node, std::string name, int val);
  void AddD(XMLNodePointer_t node, std::string name, double val);

  void SetS(XMLNodePointer_t node, std::string name, std::string val);
  void SetB(XMLNodePointer_t node, std::string name, bool val);
  void SetI(XMLNodePointer_t node, std::string name, int val);
  void SetD(XMLNodePointer_t node, std::string name, double val);

  void ChangeS(XMLNodePointer_t node, std::string name, std::string val);
  void ChangeB(XMLNodePointer_t node, std::string name, bool val);
  void ChangeI(XMLNodePointer_t node, std::string name, int val);
  void ChangeD(XMLNodePointer_t node, std::string name, double val);


  // Check has element
  bool Has(XMLNodePointer_t node, std::string name);

  // Reconfigure (sorts overrides, logger, etc)
  void Reconfigure(){};

  /// OLD Wrapper Functions for GetParI,etc
  inline std::string GetParS(std::string name){ return ConfS(name); };
  inline int GetParI(std::string name){ return ConfI(name); };
  inline double GetParD(std::string name){ return ConfD(name); };
  inline bool GetParB(std::string name){ return ConfB(name); };

  // Get Vectors
  std::vector<std::string> GetVS(XMLNodePointer_t node, std::string name, const char* del);
  std::vector<int> GetVI(XMLNodePointer_t node, std::string name, const char* del);
  std::vector<double> GetVD(XMLNodePointer_t node, std::string name, const char* del);

  void RemoveEmptyNodes();
  void RemoveIdenticalNodes();
  bool MatchingNodes(XMLNodePointer_t node1, XMLNodePointer_t node2);
  void PrintNode(XMLNodePointer_t node);
  void RemoveNode(XMLNodePointer_t node);

  std::string GetTag(std::string name);
  void ExpandAllTags();

 private:

  XMLNodePointer_t fMainNode; ///< Main XML Parent Node
  TXMLEngine* fXML; ///< ROOT XML Engine
  std::vector<XMLDocPointer_t> fXMLDocs; ///< List of all XML document inputs

  int fCurrentTime; ///< Unix time for inefficiency checking
  std::map<std::string, int> fConfigCallCount; ///< To check for inefficiency.
  std::map<std::string, bool> fConfigCallWarning; ///< Only print warning once.

protected:
  static nuisconfig* m_nuisconfigInstance;
};

// Get Function for Singleton
namespace Config {
  nuisconfig& Get();
}


/*! @} */
#endif


