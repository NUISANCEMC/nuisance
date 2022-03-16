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
#ifndef NUANCEINPUTHANDLER_H
#define NUANCEINPUTHANDLER_H
#ifdef NUANCE_ENABLED
/*!
 *  \addtogroup InputHandler
 *  @{
 */
#include "InputHandler.h"
#include "PlotUtils.h"

/// NUANCE Generator Container to save extra particle status codes.
class NUANCEGeneratorInfo : public GeneratorInfoBase {
public:
	NUANCEGeneratorInfo() {};
	virtual ~NUANCEGeneratorInfo();

	/// Assigns information to branches
	void AddBranchesToTree(TTree* tn);

	/// Setup reading information from branches
	void SetBranchesFromTree(TTree* tn);

	/// Allocate any dynamic arrays for a new particle stack size
	void AllocateParticleStack(int stacksize);

	/// Clear any dynamic arrays
	void DeallocateParticleStack();

	/// Read extra NUANCE information from the event
	void FillGeneratorInfo(NuanceEvent* nevent);

	/// Reset extra information to default/empty values
	void Reset();

	// int  kMaxParticles; ///< Number of particles in stack
	// int* fNEUTParticleStatusCode; ///<NEUT Particle Status Flags
	// int* fNEUTParticleAliveCode; ///< NEUT Alive Code (0 dead, 1 final state)
	// int fNEUTParticleN; ///< Number of particles
};

/// Handler to read NUANCE TTrees directly into NUISANCE Format
class NUANCEInputHandler : public InputHandlerBase {
public:

	/// Standard  constructor given name and input file list
	NUANCEInputHandler(std::string const& handle, std::string const& rawinputs);
	~NUANCEInputHandler();

	/// Returns NUISANCE format event by reading fNUANCETree
	FitEvent* GetNuisanceEvent(const UInt_t entry, const bool lightweight = false);

	/// Converts events from the current TTree entry into NUISANCE event.
	void CalcNUISANCEKinematics();

	/// Create a TTree Cache to speed up file read
	void CreateCache();

	/// Remove TTree Cache to save memory
	void RemoveCache();

	/// Print event information
	void Print();

	/// Converts Nuance Interaction Mode to NUISANCE codes
	int ConvertNuanceMode(NuanceEvent * evt);

	bool fSaveExtra; ///< Save Extra NUANCE information in to fNuanceInfo
	NUANCEGeneratorInfo fNuanceInfo; ///< Generator Info Saver
	NuanceEvent* fNuanceEvent; ///< NUANCE Reader
	TChain* fNUANCETree;       ///< Pointer to TTree in file
};
#endif
/*! @} */
#endif
