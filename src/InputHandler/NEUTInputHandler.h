#ifndef NEUTINPUTHANDLER_H
#define NEUTINPUTHANDLER_H

#ifdef __NEUT_ENABLED__
#include "InputHandler.h"
#include "TargetUtils.h"

#include "neutvect.h"


/// NEUT Generator Container to save extra particle status codes.
class NEUTGeneratorInfo : public GeneratorInfoBase {
public:
	NEUTGeneratorInfo() {};
	virtual ~NEUTGeneratorInfo();

	/// Assigns information to branches
	void AddBranchesToTree(TTree* tn);

	/// Setup reading information from branches
	void SetBranchesFromTree(TTree* tn);

	/// Allocate any dynamic arrays for a new particle stack size
	void AllocateParticleStack(int stacksize);

	/// Clear any dynamic arrays
	void DeallocateParticleStack();

	/// Read extra NEUT information from the event
	void FillGeneratorInfo(NeutVect* nevent);

	/// Reset extra information to default/empty values
	void Reset();

	int  kMaxParticles; ///< Number of particles in stack
	int* fNEUTParticleStatusCode; ///<NEUT Particle Status Flags
	int* fNEUTParticleAliveCode; ///< NEUT Alive Code (0 dead, 1 final state)
	int fNEUTParticleN; ///< Number of particles
};

/// NEUT Input Convertor to read in NeutVects and convert to FitEvents
class NEUTInputHandler : public InputHandlerBase {
public:

	/// Main constructor. Can read in single or joint inputs.
	NEUTInputHandler(std::string const& handle, std::string const& rawinputs);
	~NEUTInputHandler();

	/// Returns NUISANCE Format event from entry in fNEUTTree
	FitEvent* GetNuisanceEvent(const UInt_t entry, const bool lightweight);

	/// Create a TTree Cache to speed up file read
	void CreateCache();

	/// Remove TTree Cache to save memory
	void RemoveCache();

	/// Convert NEUT particle status codes to NUISANCE format status
	int GetNeutParticleStatus(NeutPart* part);

	/// eads fNeutVect and fills into fNUISANCEEvent.
	void CalcNUISANCEKinematics();

	double GetQ2(FitEvent* event); // Gets Q2 for Mott's rewegiht

	bool fSaveExtra; ///< Save Extra NEUT information in to fNeutInfo
	TChain* fNEUTTree; ///< TTree for reading neut vectors.
	NeutVect* fNeutVect;  ///< Neut vector format event.
	NEUTGeneratorInfo* fNeutInfo; ///< NEUT Generator Info container.

	bool fReweightQ4; // Flag to apply the 1/Q2**2 in the case of electron scattering using Jordan's file

};

namespace NEUTUtils {
#ifdef __NEUT_ENABLED__
void FillNeutCommons(NeutVect* nvect);
#endif
}

#endif
#endif
