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
#ifndef GENIEINPUTHANDLER_H
#define GENIEINPUTHANDLER_H
/*!
 *  \addtogroup InputHandler
 *  @{
 */
#ifdef __GENIE_ENABLED__
#include "InputHandler.h"
#include "InputUtils.h"
#include "PlotUtils.h"

#ifdef GENIE_PRE_R3
#include "GHEP/GHepParticle.h"
#include "PDG/PDGUtils.h"
#include "GHEP/GHepUtils.h"
#include "Conventions/Units.h"
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepRecord.h"
#include "Ntuple/NtpMCEventRecord.h"
#else
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/GHEP/GHepUtils.h"
#include "Framework/Conventions/Units.h"
#include "Framework/EventGen/EventRecord.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/Ntuple/NtpMCEventRecord.h"
#endif

using namespace genie;

/// GENIE Generator Container to save extra particle status codes.
class GENIEGeneratorInfo : public GeneratorInfoBase {
public:
	GENIEGeneratorInfo() {};
	virtual ~GENIEGeneratorInfo();

	/// Assigns information to branches
	void AddBranchesToTree(TTree* tn);

	/// Setup reading information from branches
	void SetBranchesFromTree(TTree* tn);

	/// Allocate any dynamic arrays for a new particle stack size
	void AllocateParticleStack(int stacksize);

	/// Clear any dynamic arrays
	void DeallocateParticleStack();

	/// Read extra genie information from the event
	void FillGeneratorInfo(NtpMCEventRecord* ntpl);

	/// Reset extra information to default/empty values
	void Reset();

	int  kMaxParticles; ///< Number of particles in stack
	int* fGenieParticlePDGs; ///< GENIE Particle PDGs (example)
};

/// Main GENIE InputHandler
class GENIEInputHandler : public InputHandlerBase {
public:

	/// Standard constructor given a name and input files
	GENIEInputHandler(std::string const& handle, std::string const& rawinputs);
	virtual ~GENIEInputHandler();

	/// Create a TTree Cache to speed up file read
	void CreateCache();

	/// Remove TTree Cache to save memory
	void RemoveCache();
	
	/// Returns a NUISANCE format event from the GENIE TTree. If !lightweight
	/// then CalcNUISANCEKinematics() is called to convert the GENIE event into
	/// a standard NUISANCE format.
	FitEvent* GetNuisanceEvent(const UInt_t entry, const bool lightweight = false);

	/// Converts GENIE event into standard NUISANCE FitEvent by looping over all
	/// particles in the event and adding them to stack in fNUISANCEEvent.
	void CalcNUISANCEKinematics();

	/// Placeholder for GENIE related event printing.
	void Print();

	/// Converts GENIE particle status codes into NUISANCE status codes.
	int GetGENIEParticleStatus(genie::GHepParticle* part, int mode = 0);

	/// Converts GENIE event reaction codes into NUISANCE reaction codes.
	int ConvertGENIEReactionCode(GHepRecord* gheprec);

	GHepRecord* fGenieGHep;         ///< Pointer to actual event record
	NtpMCEventRecord* fGenieNtpl;   ///< Ntpl Wrapper Class

	TChain* fGENIETree;             ///< Main GENIE Event TTree
	bool fSaveExtra;    ///< Flag to save Extra GENIE info into Nuisance Event
	GENIEGeneratorInfo* fGenieInfo; ///< Extra GENIE Generator Info Writer

        bool fNOvAWeights; ///< Flag to save nova weights or not

        // Extra weights from Jeremy for NOvA weights
        double MAQEw;
        double NonResw;
        double RPAQEw;
        double RPARESw;
        double MECw;
        double DISw;
        double NOVAw;
};
/*! @} */
#endif
#endif
