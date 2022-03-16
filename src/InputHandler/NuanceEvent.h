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
#ifndef NUANCE_H_SEEN
#define NUANCE_H_SEEN
/*!
 *  \addtogroup FitBase
 *  @{
 */

#ifdef NUANCE_ENABLED
#include "TTree.h"

/// TTree Reader for Nuance TTree Events
class NuanceEvent {
 public:

  /// Empty constructor
  NuanceEvent(){};

  /// Reads values from the tTree
  void SetBranchAddresses(TTree* tn);

  bool cc;       ///< Is Charged Current
  bool bound;    ///< On Bound Target
  int neutrino;  ///< Neutrino PDG
  int target;    ///< Target PDG
  float iniQ;
  float finQ;
  int lepton0;  
  float polar;
  int channel;
  float qsq;
  float w;
  float x;       ///< Event Bjorken x
  float y;       ///< Event Bjorken y

  float p_neutrino[4]; ///< Neutrino 4-Mom
  float p_targ[5];     ///< Target 5-mom
  float vertex[4];     ///< Vertex Position
  float start[4];      ///< Start Position 
  float depth;
  float flux;

  int n_leptons;       ///< Number of final state leptons
  float p_ltot[5];     ///< Total 5-momentum of leptonic side
  int lepton[200];     ///< Lepton PDGs
  float p_lepton[5][200]; ///< Lepton 5-momentum
  
  int n_hadrons;       ///< Number of final state hadrons
  float p_htot[5];     ///< Total 5-momentum of hadronic side
  int hadron[200];     ///< Hadron PDGs
  float p_hadron[5][200]; ///< Hadron 5-momentum
};
#endif
/*! @} */
#endif


