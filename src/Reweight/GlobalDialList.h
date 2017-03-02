#ifndef GLOBAL_DIAL_LIST_H
#define GLOBAL_DIAL_LIST_H
#include "WeightUtils.h"

class GlobalDialList {
 public:

  static GlobalDialList& Get(void);

  /// Constructor \n
  GlobalDialList(){};
  virtual ~GlobalDialList(){};

  int EnumFromNameAndType(std::string name, int type);
  void RegisterDialEnum(std::string name, int type, int nuisenum);


  std::vector<std::string> fAllDialNames;
  std::vector<int> fAllDialTypes;
  std::vector<int> fAllDialEnums;

  std::map<int, std::map<std::string, int> > fTypeEnumCont;

 protected:
  static GlobalDialList* m_diallistInstance;
};

namespace Reweight {
  GlobalDialList& DialList();
}
#endif
