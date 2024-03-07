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
#ifndef FITEVENT_INPUTHANDLER_H
#define FITEVENT_INPUTHANDLER_H
/*!
 *  \addtogroup InputHandler
 *  @{
 */
#include "InputHandler.h"
#include "FitEvent.h"
#include "PlotUtils.h"

/// Class to read in NUISANCE FitEvents that have been saved to tree
class GenericVectorsInputHandler : public InputHandlerBase {
public:

	/// Standard constructor given name and inputs
	GenericVectorsInputHandler(std::string const& handle, std::string const& rawinputs);
	virtual ~GenericVectorsInputHandler();

	/// Create a TTree Cache to speed up file read
	void CreateCache();

	/// Remove TTree Cache to save memory
	void RemoveCache();

	/// Returns NUISANCE FitEvent from the TTree. If lightweight does nothing.
	FitEvent* GetNuisanceEvent(const UInt_t entry, const bool lightweight=false);

	/// Alongside InputWeight also returns any saved RWWeights
	double GetInputWeight(int entry);

	/// Print out event information
	void Print();

	TChain* fFitEventTree; ///< TTree from FitEvent file.

	// int fReadNParticles;
	// double fReadParticleMom[400][4];
	// UInt_t fReadParticleState[400];
	// int fReadParticlePDG[400];


	// Save outgoing particle vectors
  int nfsp;
  static const int kMAX = 200;
  float px_fsp[kMAX];
  float py_fsp[kMAX];
  float pz_fsp[kMAX];
  float E_fsp[kMAX];
  int pdg_fsp[kMAX];

  // Save incoming particle info
  int ninitp;
  float px_init[kMAX];
  float py_init[kMAX];
  float pz_init[kMAX];
  float E_init[kMAX];
  int pdg_init[kMAX];

  // Save pre-FSI particle info
  int nvertp;
  float px_vert[kMAX];
  float py_vert[kMAX];
  float pz_vert[kMAX];
  float E_vert[kMAX];
  int pdg_vert[kMAX];

  // Basic event info
  float FlatTreeWeight;
  float FlatTreeInputWeight;
  float FlatTreeRWWeight;

};
/*! @} */
#endif
