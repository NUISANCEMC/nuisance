#ifndef GIBUUINPUTHANDLER_H
#define GIBUUINPUTHANDLER_H
#ifdef GiBUU_ENABLED
/*!
 *  \addtogroup InputHandler
 *  @{
 */
#include "InputHandler.h"
#include "PlotUtils.h"
#include "StdHepEvt.h"

/// GIBUU Generator Container to save extra particle status codes.
class GIBUUGeneratorInfo : public GeneratorInfoBase {
public:
	GIBUUGeneratorInfo() {};
	virtual ~GIBUUGeneratorInfo();

	/// Assigns information to branches
	void AddBranchesToTree(TTree* tn);

	/// Setup reading information from branches
	void SetBranchesFromTree(TTree* tn);

	/// Allocate any dynamic arrays for a new particle stack size
	void AllocateParticleStack(int stacksize);

	/// Clear any dynamic arrays
	void DeallocateParticleStack();

	/// Read extra GIBUU information from the event
	void FillGeneratorInfo(GiBUUStdHepReader* nevent);

	/// Reset extra information to default/empty values
	void Reset();

	// int  kMaxParticles; ///< Number of particles in stack
	// int* fNEUTParticleStatusCode; ///<GIBUU Particle Status Flags
	// int* fNEUTParticleAliveCode; ///< GIBUU Alive Code (0 dead, 1 final state)
	// int fNEUTParticleN; ///< Number of particles
};

/// Convert GiBUU Code to NUISANCE Event Code
int GetGIBUUParticleStatus(int status, int pdg);

/// GiBUU Handler to read in Luke's formatted GiBUU events
class GIBUUInputHandler : public InputHandlerBase {
public:

	/// Standard constructor given name and inputs
	GIBUUInputHandler(std::string const& handle, std::string const& rawinputs);
	~GIBUUInputHandler() {};

	/// Returns NUISANCE Format Event from GiReader
	FitEvent* GetNuisanceEvent(const UInt_t entry, const bool lightweight = false);

	/// Override to handle the fact that GiBUU already averages over nevents.
	void SetupJointInputs();

	/// Fill NUISANCE Particle Stack
	void CalcNUISANCEKinematics();

	/// Print event information
	void Print();

private:
	GiBUUStdHepReader* fGiReader; ///< GiBUU Event Reader
	TChain* fGIBUUTree;           ///< GiBUU Event Tree
};
#endif
#endif
