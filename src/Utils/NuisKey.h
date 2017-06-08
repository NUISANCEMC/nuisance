#ifndef NUISKEY_H
#define NUISKEY_H

#include "NuisConfig.h"
#include <vector>

class nuiskey{
 public:

  nuiskey(){
  };

  nuiskey(XMLNodePointer_t node){
    fNode = node;
  };

  ~nuiskey(){};

  std::string GetS(std::string name);
  int    GetI(std::string name);
  double GetD(std::string name);
  bool   GetB(std::string name);

  std::vector<std::string> GetVS(std::string name, const char* del);
  std::vector<int>         GetVI(std::string name, const char* del);
  std::vector<double>      GetVD(std::string name, const char* del);

  void SetS(std::string name, std::string newval);
  void SetI(std::string name, int newval);
  void SetD(std::string name, double newval);
  void SetB(std::string name, bool newval);

  void ChangeS(std::string name, std::string newval);
  void ChangeI(std::string name, int newval);
  void ChangeD(std::string name, double newval);
  void ChangeB(std::string name, bool newval);

  void AddS(std::string name, std::string newval);
  void AddI(std::string name, int newval);
  void AddD(std::string name, double newval);
  void AddB(std::string name, bool newval);

  bool Has(std::string name);

  XMLNodePointer_t fNode; ///< XML Node in Config::Get().fXML for this key                                                   
};

namespace Config {

  // Return a vector of keys for use
  std::vector<nuiskey> QueryKeys(const std::string name, const std::string test1="");
  nuiskey QueryFirstKey(const std::string name, const std::string test1="");
  nuiskey QueryLastKey(const std::string name, const std::string test1="");

  // Create a new global config entry
  nuiskey CreateKey(const std::string name);


  // void ConvertAndLoadCardToXMLFormat(const std::string cardfile);

  // Backwards compatible sample key options
  nuiskey CreateSampleKeyFromLine(const std::string line);
  nuiskey CreateParameterKeyFromLine(const std::string line);
  nuiskey CreatePullKeyFromLine(const std::string line);
  nuiskey CreateOldConfigKeyFromLine(const std::string line);
  
}
#endif
