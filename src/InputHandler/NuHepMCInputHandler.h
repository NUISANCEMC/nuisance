#pragma once

#include "InputHandler.h"

#include "HepMC3/Reader.h"
#include "HepMC3/GenEvent.h"

#include <memory>
#include <string>

/// NEUT Input Convertor to read in NeutVects and convert to FitEvents
class NuHepMCInputHandler : public InputHandlerBase {
public:

	NuHepMCInputHandler(std::string const& handle, std::string const& rawinputs);
	~NuHepMCInputHandler();

	FitEvent* GetNuisanceEvent(const UInt_t entry, bool);

	BaseFitEvt* GetBaseEvent(const UInt_t entry);

	void CalcNUISANCEKinematics();

	int ConvertHepMCStatus();

	double GetInputWeight(const UInt_t entry);

  std::shared_ptr<HepMC3::Reader> fReader;
  std::shared_ptr<HepMC3::GenRunInfo> frun_info;
  UInt_t nextentry;
  HepMC3::GenEvent fHepMC3Evt;
  std::string fFilename;
};
