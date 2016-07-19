#include "EventManager.h"

EventManager* EventManager::m_evtmgrInstance = NULL;

EventManager& EventManager::Get(void){
  if (!m_evtmgrInstance){
    m_evtmgrInstance = new EventManager;
  }
  return *m_evtmgrInstance;
}


  
FitWeight* EventManager::GetRW(){
  return fRW;
};

InputHandler* EventManager::GetInput(int infile) {
  return finputs[infile];
};


FitEvent* EventManager::GetEvent(int infile, int i){

  finputs[infile]->ReadEvent(i);
  FitEvent* evtpt = finputs[infile]->GetEventPointer();
    
  if (!frwneeded[infile][i]){

    evtpt->Weight = calc_rw[infile][i];

  } else {

    evtpt->RWWeight = fRW->CalcWeight(evtpt);
    evtpt->Weight = evtpt->RWWeight*evtpt->InputWeight;
    
    calc_rw[infile][i] = evtpt->Weight;
    frwneeded[infile][i] = false;
  }
  
  return evtpt;
}


double EventManager::GetEventWeight(int infile, int i){
  
  if (!frwneeded[infile][i]) {
    return calc_rw[infile][i];
  }
  
  finputs[infile]->GetTreeEntry(i);
  FitEvent* evtpt = finputs[infile]->GetEventPointer();
  double Weight = fRW->CalcWeight(evtpt) * evtpt->InputWeight;
  
  calc_rw[infile][i] = Weight;
  frwneeded[infile][i] = false;
  
  return Weight;
}
  
void EventManager::AddInput(std::string handle, std::string infile){

  if (finputs.find(GetInputID(infile)) != finputs.end()){
    std::cout<<"Event manager already contains this input. skipping"<<std::endl;
    return;
  } else {
    std::cout<<"Adding input "<<infile<<std::endl;
  }

  int id = GetInputID(infile);
  
  finputs[id] = new InputHandler(handle, infile);
  frwneeded[id] = std::vector<bool>(finputs[id]->GetNEvents(),true);
  calc_rw[id] = std::vector<double>(finputs[id]->GetNEvents(),0.0);
}

void EventManager::ResetWeightFlags(){
  for (std::map<int, InputHandler*>::iterator iter = finputs.begin();
       iter != finputs.end(); iter++){
    int id = iter->first;
    frwneeded[id].clear();
    frwneeded[id]= std::vector<bool>(finputs[id]->GetNEvents(),true);
  }
}
  
EventManager::EventManager(){
  fRW = new FitWeight("FitWeight");
  finputs.clear();
};

EventManager::~EventManager(){
  delete fRW;
  finputs.clear();
};



int EventManager::GetInputID(std::string infile){

  if (fid.find(infile) == fid.end()){
    return fid.size();
  }
  
  return fid[infile];
}
