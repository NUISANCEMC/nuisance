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

nuiskey Config::CreateParameterKeyFromLine(const std::string line){
  nuiskey parameterkey = Config::CreateKey("parameter");

  // Parse
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(line, " ");

  // Add to key
  parameterkey.AddS("type", strvct[0]);
  parameterkey.AddS("name", strvct[1]);
  parameterkey.AddS("nominal", strvct[2]);

  if (strvct.size() == 7){
    parameterkey.AddS("low",strvct[3]);
    parameterkey.AddS("high",strvct[4]);
    parameterkey.AddS("step",strvct[5]);
    parameterkey.AddS("state",strvct[6]);
  } else if (strvct.size() == 3){
    parameterkey.AddS("state","FIX");
  }

  return parameterkey;
}

bool nuiskey::Has(const std::string name){
  return Config::Get().Has(fNode, name);
}

nuiskey Config::CreatePullKeyFromLine(const std::string line){
  nuiskey pullkey = Config::CreateKey("covar");

  // Parse
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(line, " ");
  pullkey.AddS("name", strvct[1]);
  pullkey.AddS("input", strvct[2]);
  pullkey.AddS("type", strvct[3]);
    
  return pullkey;
}


nuiskey Config::CreateOldConfigKeyFromLine(const std::string line){
  nuiskey configkey = Config::CreateKey("config");

  std::vector<std::string> strvct = GeneralUtils::ParseToStr(line, " ");
  configkey.AddS( strvct[1], strvct[2] );

  return configkey;
}

nuiskey Config::CreateSampleKeyFromLine(const std::string line){

  // Create new key
  nuiskey samplekey = Config::CreateKey("sample");

  // Parse
  std::vector<std::string> strvct = GeneralUtils::ParseToStr(line, " ");

  // Get elements
  samplekey.AddS("name"  , strvct[1]);
  samplekey.AddS("input" , strvct[2]);
  if (strvct.size() > 3){
    samplekey.AddS("type", strvct[3]);
  }
  if (strvct.size() > 4){
    samplekey.AddS("norm", strvct[4]);
  }

  return samplekey;
}


nuiskey Config::CreateKey(const std::string name){
  return nuiskey(Config::Get().CreateNode(name));
}

void nuiskey::AddS(std::string name, std::string newval){
  Config::Get().AddS(fNode, name, newval);
}
void nuiskey::AddI(std::string name, int newval){
  Config::Get().AddI(fNode, name, newval);
}
void nuiskey::AddD(std::string name, double newval){
  Config::Get().AddD(fNode, name, newval);
}
void nuiskey::AddB(std::string name, bool newval){
  Config::Get().AddB(fNode, name, newval);
}

void nuiskey::SetS(std::string name, std::string newval){
  Config::Get().SetS(fNode, name, newval);
}
void nuiskey::SetI(std::string name, int newval){
  Config::Get().SetI(fNode, name, newval);
}
void nuiskey::SetD(std::string name, double newval){
  Config::Get().SetD(fNode, name, newval);
}
void nuiskey::SetB(std::string name, bool newval){
  Config::Get().SetB(fNode, name, newval);
}

void nuiskey::ChangeS(std::string name, std::string newval){
  Config::Get().ChangeS(fNode, name, newval);
}
void nuiskey::ChangeI(std::string name, int newval){
  Config::Get().ChangeI(fNode, name, newval);
}
void nuiskey::ChangeD(std::string name, double newval){
  Config::Get().ChangeD(fNode, name, newval);
}
void nuiskey::ChangeB(std::string name, bool newval){
  Config::Get().ChangeB(fNode, name, newval);
}



std::vector<std::string> nuiskey::GetAllKeys(){
  return Config::Get().GetAllKeysForNode(fNode);
}
