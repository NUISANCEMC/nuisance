#ifndef NUISKEY_H
#define NUISKEY_H

#include <algorithm>
#include <map>

#include "TFile.h"
#include "TXMLEngine.h"

class nuiskey {
 public:
  nuiskey(){};

  nuiskey(XMLNodePointer_t node) { fNode = node; };
  nuiskey(std::string const &name);

  void Print();

  ~nuiskey(){};

  std::string GetS(std::string const &name);
  int GetI(std::string const &name);
  double GetD(std::string const &name);
  bool GetB(std::string const &name);

  std::vector<std::string> GetVS(std::string const &name, const char *del);
  std::vector<int> GetVI(std::string const &name, const char *del);
  std::vector<double> GetVD(std::string const &name, const char *del);

  void SetS(std::string const &name, std::string const &newval);
  void SetI(std::string const &name, int newval);
  void SetD(std::string const &name, double newval);
  void SetB(std::string const &name, bool newval);

  void Set(std::string const &name, std::string const &newval);
  void Set(std::string const &name, char const *newval);
  void Set(std::string const &name, int newval);
  void Set(std::string const &name, double newval);
  void Set(std::string const &name, bool newval);

  bool Has(std::string const &name);

  std::string GetElementName();

  std::vector<std::string> GetAllKeys();

  std::vector<nuiskey> GetListOfChildNodes(std::string const &filter = "");

  XMLNodePointer_t fNode;  ///< XML Node in Config::Get().fXML for this key
};

namespace Config {

// Return a vector of keys for use
std::vector<nuiskey> QueryKeys(std::string const &name,
                               std::string const &test1 = "");
nuiskey QueryFirstKey(std::string const &name, std::string const &test1 = "");
nuiskey QueryLastKey(std::string const &name, std::string const &test1 = "");

nuiskey CreateKey(std::string const &name);
}
#endif
