#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include "InputHandler.h"
#include "FitWeight.h"

class EventManager {
 public:
  static EventManager& Get(void);
  
  FitWeight* GetRW();
  InputHandler* GetInput(int id);
  FitEvent* GetEvent(int id, int i);
  double GetEventWeight(int id, int i);
  void AddInput(std::string handle, std::string infile);
  void ResetWeightFlags();
  bool GetRWFlag(int id);
  void SetRWFlag(int id, bool flag);
  int GetInputID(std::string infile);
  
 protected:
  EventManager();  
  ~EventManager();

  static EventManager* m_evtmgrInstance;
  
  FitWeight* fRW;
  std::map< std::string, int > fid;
  std::map< int, InputHandler* > finputs;
  std::map< int, std::vector< bool > > frwneeded;
  std::map< int, std::vector< double > > calc_rw;
  
};


namespace FitBase {
 
  EventManager& EvtManager(){ return EventManager::Get(); };
  FitWeight* GetRW(){ return EvtManager().GetRW(); };
  int GetInputID(std::string infile){ return EvtManager().GetInputID(infile); };
  InputHandler* GetInput(int infile){ return EvtManager().GetInput(infile); };
  void AddInput(std::string handle, std::string infile){ return EvtManager().AddInput(handle, infile); };
  
  
}
#endif
