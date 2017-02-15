#include "NuisKey.h"

std::string nuiskey::GetS(std::string name){ 
  return Config::Get().GetS(fNode,name); 
};

int         nuiskey::GetI(std::string name){ 
  return Config::Get().GetI(fNode,name); 
};

double      nuiskey::GetD(std::string name){ 
  return Config::Get().GetD(fNode,name); 
};

bool        nuiskey::GetB(std::string name){ 
  return Config::Get().GetB(fNode,name); 
};

std::vector<std::string> nuiskey::GetVS(std::string name, const char* del){
  return Config::Get().GetVS(fNode,name,del);
};

std::vector<int>         nuiskey::GetVI(std::string name, const char* del){
  return Config::Get().GetVI(fNode,name,del);
};

std::vector<double>      nuiskey::GetVD(std::string name, const char* del){
  return Config::Get().GetVD(fNode,name,del);
};


std::vector<nuiskey> Config::QueryKeys(const std::string type, const std::string test1){

  // Get Vector of nodes     
  std::vector<XMLNodePointer_t> nodelist = Config::Get().GetNodes(type);

  // Convert List into a key list for easier access  
  std::vector<nuiskey> keylist;
  for (std::vector<XMLNodePointer_t>::const_iterator iter = nodelist.begin();
       iter != nodelist.end(); iter++){

    // Create new key
    nuiskey newkey = nuiskey(*iter);

    // Add test1
    if (!test1.empty()){
      std::vector<std::string> testvect = GeneralUtils::ParseToStr(test1,"=");
      if (testvect.size() < 2) continue;
      if (newkey.GetS(testvect[0]) != testvect[1]) continue;
    }

    // Save node as nuiskey  
    keylist.push_back( newkey );
  }

  // Return list of keys     
  return keylist;
}

nuiskey Config::QueryLastKey(const std::string type, const std::string test1){
  // Loop over all for now because I'm lazy...
  std::vector<nuiskey> allkeys = Config::QueryKeys(type,test1);
  if (allkeys.size() < 1) return nuiskey();
  else return allkeys[allkeys.size()-1];
}


nuiskey Config::QueryFirstKey(const std::string type, const std::string test1){
  // Loop over all for now because I'm lazy...          
  std::vector<nuiskey> allkeys = Config::QueryKeys(type,test1);
  if (allkeys.size() < 1) return nuiskey();
  else return allkeys[allkeys.size()-1];
}





nuiskey Config::CreateKey(const std::string name){
  return nuiskey(Config::Get().CreateNode(name));
}
