#ifndef NUISKEY_H
#define NUISKEY_H

#include <vector>

#include "NuisConfig.h"

class nuiskey {
 public:
  nuiskey(){};

  nuiskey(XMLNodePointer_t node) { fNode = node; };
  nuiskey(std::string name);

  ~nuiskey(){};

  std::string GetS(std::string name);
  int GetI(std::string name);
  double GetD(std::string name);
  bool GetB(std::string name);

  std::vector<std::string> GetVS(std::string name, const char* del);
  std::vector<int> GetVI(std::string name, const char* del);
  std::vector<double> GetVD(std::string name, const char* del);

  void SetS(std::string name, std::string newval);
  void SetI(std::string name, int newval);
  void SetD(std::string name, double newval);
  void SetB(std::string name, bool newval);

  void Set(std::string name, std::string newval);
  void Set(std::string name, int newval);
  void Set(std::string name, double newval);
  void Set(std::string name, bool newval);

  bool Has(std::string name);

  std::string GetElementName();

  std::vector<std::string> GetAllKeys();

  std::vector<nuiskey> GetListOfChildNodes(std::string const& filter = "");

  XMLNodePointer_t fNode;  ///< XML Node in Config::Get().fXML for this key
};

namespace Config {

// Return a vector of keys for use
std::vector<nuiskey> QueryKeys(const std::string name,
                               const std::string test1 = "");
nuiskey QueryFirstKey(const std::string name, const std::string test1 = "");
nuiskey QueryLastKey(const std::string name, const std::string test1 = "");

// Create a new global config entry
nuiskey CreateKey(const std::string name);

// void ConvertAndLoadCardToXMLFormat(const std::string cardfile);

// Backwards compatible sample key options
}
#endif
