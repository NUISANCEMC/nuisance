#ifndef NEUTINPUTHANDLER_H
#define NEUTINPUTHANDLER_H

#ifdef __NEUT_ENABLED__
#include "InputHandler2.h"
#include "TargetUtils.h"
#include "neutpart.h"
#include "neutvect.h"
#include "PlotUtils.h"


/// NEUT Generator Container to save extra particle status codes.
class NEUTGeneratorInfo : public GeneratorInfoBase {
public:

  NEUTGeneratorInfo(){};
  ~NEUTGeneratorInfo(){};

  /// Sets up tree branches for saving to NUISANCE format event trees.
  void AddBranchesToTree(TTree* tn);

  /// Resets particle stack. Called inside FitEvent::Reset();
  void Reset();

  /// Max Particle List.
  const static UInt_t kMaxParticles = 400;

  /// NEUT Particle Status Flags
  int fNEUT_ParticleStatusCode[kMaxParticles];

  /// NEUT Alive Code (0 dead, 1 final state)
  int fNEUT_ParticleAliveCode[kMaxParticles];
};

/// NEUT Input Convertor to read in NeutVects and convert to FitEvents
class NEUTInputHandler : public InputHandlerBase {
public:

	/// Main constructor. Can read in single or joint inputs.
	NEUTInputHandler(std::string const& handle, std::string const& rawinputs);
	~NEUTInputHandler(){};

	/// Returns NUISANCE Format event from entry in fNEUTTree
	FitEvent* GetNuisanceEvent(const UInt_t entry);

	/// Returns BaseEvent (just NeutVect pointer) from entry in fNEUTTree
	BaseFitEvt* GetBaseEvent(const UInt_t entry);

	/// Convert NEUT particle status codes to NUISANCE format status
	int GetNeutParticleStatus(NeutPart* part);

	/// eads fNeutVect and fills into fNUISANCEEvent.
	void CalcNUISANCEKinematics();

	/// Calculates weight if using joint inputs
	double GetInputWeight(const UInt_t entry);

	bool fSaveExtra; ///< Save Extra NEUT information in to fNeutInfo
	TChain* fNEUTTree; ///< TTree for reading neut vectors.
	NeutVect* fNeutVect;  ///< Neut vector format event.
	NEUTGeneratorInfo* fNeutInfo; ///< NEUT Generator Info container.
};
#endif
#endif