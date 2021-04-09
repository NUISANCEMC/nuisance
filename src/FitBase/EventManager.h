// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include "InputHandler.h"
#include "FitWeight.h"
#include "InputUtils.h"
#include "InputFactory.h"
// This class is meant to manage one input file for many distributions
class EventManager {
 public:
  static EventManager& Get(void);

  FitWeight* GetRW();
  void SetRW(FitWeight* rw);

  InputHandlerBase* GetInput(int id);
  FitEvent* GetEvent(int id, int i);
  double GetEventWeight(int id, int i);
  InputHandlerBase* AddInput(std::string handle, std::string infile);
  void ResetWeightFlags();
  int GetInputID(std::string infile);

  std::map< int, InputHandlerBase* > GetInputs();

 protected:
  EventManager();
  ~EventManager();

  static EventManager* m_evtmgrInstance;

  FitWeight* fRW;
  std::map< std::string, int > fid;
  std::map< int, InputHandlerBase* > finputs;
  std::map< int, std::vector< bool > > frwneeded;
  std::map< int, std::vector< double > > calc_rw;

};


namespace FitBase {

  inline
  EventManager& EvtManager(){ return EventManager::Get(); };
  inline
  FitWeight* GetRW(){ return EvtManager().GetRW(); };
  inline
  void SetRW(FitWeight* rw){ EvtManager().SetRW(rw); };
  inline
  int GetInputID(std::string infile){ return EvtManager().GetInputID(infile); };
  inline
  InputHandlerBase* GetInput(int infile){ return EvtManager().GetInput(infile); };
  inline
  InputHandlerBase* AddInput(std::string handle, std::string infile){ return EvtManager().AddInput(handle, infile); };


}
#endif
