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

#include "EventManager.h"

EventManager* EventManager::m_evtmgrInstance = NULL;

EventManager& EventManager::Get(void) {
  if (!m_evtmgrInstance) {
    m_evtmgrInstance = new EventManager;
  }
  return *m_evtmgrInstance;
}

FitWeight* EventManager::GetRW() { return fRW; };

InputHandlerBase* EventManager::GetInput(int infile) { return finputs[infile]; };

FitEvent* EventManager::GetEvent(int infile, int i) {
  /*
  finputs[infile]->ReadEvent(i);
  FitEvent* evtpt = finputs[infile]->GetEventPointer();

  // If we don't need a full reweight
  if (!frwneeded[infile][i]) {
    evtpt->Weight = calc_rw[infile][i];

    // if we do need a full reweight
  } else {
    evtpt->RWWeight = fRW->CalcWeight(evtpt);
    evtpt->Weight = evtpt->RWWeight * evtpt->InputWeight;

    calc_rw[infile][i] = evtpt->Weight;
    frwneeded[infile][i] = false;
  }
*/
  return NULL;
}

double EventManager::GetEventWeight(int infile, int i) {
  /*
  if (!frwneeded[infile][i]) {
    return calc_rw[infile][i];
  }

  finputs[infile]->GetTreeEntry(i);
  FitEvent* evtpt = finputs[infile]->GetEventPointer();
  double Weight = fRW->CalcWeight(evtpt) * evtpt->InputWeight;

  calc_rw[infile][i] = Weight;
  frwneeded[infile][i] = false;
*/
  double Weight = 1.0;
  return Weight;
}

std::map<int, InputHandlerBase*> EventManager::GetInputs() { return finputs; }

InputHandlerBase* EventManager::AddInput(std::string handle, std::string infile) {

  // Expect INPUTTYPE:FileLocation(s)
  std::vector<std::string> file_descriptor =
      GeneralUtils::ParseToStr(infile, ":");
  if (file_descriptor.size() != 2) {
    ERR(FTL) << "File descriptor had no filetype declaration: \"" << infile
             << "\". expected \"FILETYPE:file.root\"" << std::endl;
    throw;
  }
  InputUtils::InputType inpType =
      InputUtils::ParseInputType(file_descriptor[0]);

  int id = GetInputID(file_descriptor[1]);
  if ((uint)id != fid.size()) {
    LOG(SAM) << "Event manager already contains this input."
             << std::endl;
    return finputs[id];
  } else {
    LOG(SAM) << "Adding input " << file_descriptor[1] << std::endl;
  }

  fid[file_descriptor[1]] = id;
  finputs[id] = InputUtils::CreateInputHandler(handle, inpType, file_descriptor[1]);
  frwneeded[id] = std::vector<bool>(finputs[id]->GetNEvents(), true);
  calc_rw[id] = std::vector<double>(finputs[id]->GetNEvents(), 0.0);
  
  return finputs[id];
}

// Reset the weight flags
// Should be called for every succesful event loop
void EventManager::ResetWeightFlags() {
  /*
  // Loop over the inpts
  for (std::map<int, InputHandler*>::iterator iter = finputs.begin();
       iter != finputs.end(); iter++) {
    int id = iter->first;
    frwneeded[id].clear();
    // Reset so that all events need the reweight
    frwneeded[id] = std::vector<bool>(finputs[id]->GetNEvents(), true);
  }
  */
}

EventManager::EventManager() {
  fRW = new FitWeight("FitWeight");
  finputs.clear();
};

EventManager::~EventManager() {
  delete fRW;
  finputs.clear();
};

int EventManager::GetInputID(std::string infile) {
  if (fid.find(infile) == fid.end()) {
    return fid.size();
  }

  return fid[infile];
}

