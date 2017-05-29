#ifndef NuWroINPUTHANDLER_H
#define NuWroINPUTHANDLER_H
#ifdef __NUWRO_ENABLED__
#include "GeneratorUtils.h"
#include "InputHandler2.h"
#include "PlotUtils.h"

/// NuWro Generator Container to save extra particle status codes.
class NuWroGeneratorInfo : public GeneratorInfoBase {
public:

  NuWroGeneratorInfo(){};
  ~NuWroGeneratorInfo(){};

  /// Sets up tree branches for saving to NUISANCE format event trees.
  void AddBranchesToTree(TTree* tn);

  /// Resets particle stack. Called inside FitEvent::Reset();
  void Reset();

  /// Max Particle List.
  const static UInt_t kMaxParticles = 400;

};

class NuWroInputHandler : public InputHandlerBase {
public:

	/// Constructor. Can handle single and joint inputs.
	NuWroInputHandler(std::string const& handle, std::string const& rawinputs);
	~NuWroInputHandler(){};

	/// Returns filled NUISANCEEvent for given entry.
	FitEvent* GetNuisanceEvent(const UInt_t entry);

	/// Returns NUISANCE Base Event for given entry with pointer to fNuWroEvent
	BaseFitEvt* GetBaseEvent(const UInt_t entry);

	/// Fills fNUISANCEEvent from fNuWroEvent
	void CalcNUISANCEKinematics();

	/// Returns input weight for joint inputs.
	double GetInputWeight(int entry);

	/// (PLACEHOLDER) Automatically creates nuwro flux/event histograms that 
	/// nuisance needs to normalise events.
	void ProcessNuWroInputFlux(const std::string file);
	
	bool fSaveExtra; ///< Save Extra NuWro info into Nuisance Event
	TChain* fNuWroTree; ///< TTree for reading NuWro event vectors
	
#ifdef __NUWRO_ENABLED__
    event* fNuWroEvent;  ///< Pointer to NuWro Format Events 
    /// Calculates a True Interaction code for NuWro events 
	int ConvertNuwroMode (event * e); 

	/// Adds a new particle to NUISANCE stack for given NuWro particle
  	void AddNuWroParticle(FitEvent* evt, const particle& p, int state);
#endif

  	bool save_extra; ///< Save Extra NuWro info into Nuisance Event
  	NuWroGeneratorInfo *fNuWroInfo; /// Extra Generator Info

};
#endif
#endif