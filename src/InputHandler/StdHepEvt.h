#ifndef __STDHEPEVT_SEEN__
#define __STDHEPEVT_SEEN__
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

#include "TChain.h"
#include "TLorentzVector.h"

struct StdHepReader {
 public:
  const static int kStdHepIdxPx = 0;
  const static int kStdHepIdxPy = 1;
  const static int kStdHepIdxPz = 2;
  const static int kStdHepIdxE = 3;
  const static int kStdHepNPmax = 100;

  StdHepReader();

  ///\brief The number of StdHep particles in this event.
  Int_t StdHepN;

  ///\brief The PDG codes of particles in this event.
  ///
  /// This is determined from the GiBUU particle number by
  /// GiBUUUtils::GiBUUToPDG.
  ///\warning This is not a one-to-one mapping, e.g. resonances are not uniquely
  /// determined by the GiBUU scheme.
  Int_t StdHepPdg[kStdHepNPmax];  //[StdHepN]

  ///\brief The StdHep Status of particles in this event.
  ///
  /// Status Codes in use:
  /// - -1: Initial state real particle.
  /// - 1: Final state real particle.
  Int_t StdHepStatus[kStdHepNPmax];  //[StdHepN]

  ///\brief Four momentum for particles in this event.
  Double_t StdHepP4[kStdHepNPmax][4];

  bool SetBranchAddresses(TChain*);
};

struct GiBUUStdHepReader : public StdHepReader {
  GiBUUStdHepReader() : StdHepReader(){};

  ///\brief GiBUU interaction code
  ///
  /// See https://gibuu.hepforge.org/trac/wiki/LesHouches for details
  Int_t GiBUUReactionCode;

  ///\brief NEUT equivalent reaction code.
  /// CC:
  /// * 1 : QE
  /// * 2 : 2p2h
  /// * 10 : Single pion background (non-resonant)
  /// * 11 : Delta++ ( -11 : Delta- for nubar)
  /// * 12 : Delta+ (-12 : Delta0 for nubar)
  /// * 21 : Multi pion production
  /// * 26 : DIS
  /// * 4 : Higher resonance, charge: -1
  /// * 5 : Higher resonance, charge: 0
  /// * 6 : Higher resonance, charge: +1
  /// * 7 : Higher resonance, charge: +2
  ///
  /// NC:
  /// * 30 : Single pion background (non-resonant)
  /// * 31 : Delta0
  /// * 32 : Delta+
  /// * 41 : Multi pion production
  /// * 42 : 2p2h
  /// * 46 : DIS
  /// * 47 : Higher resonance, charge: -1
  /// * 48 : Higher resonance, charge: 0
  /// * 49 : Higher resonance, charge: +1
  /// * 50 : Higher resonance, charge: +2
  /// * 51 : NCEL proton-target
  /// * 52 : NCEL neutron-target
  ///
  Int_t GiBUU2NeutCode;
  ///\brief The total XSec weighting that should be applied to this event.
  Double_t EvtWght;

  ///\brief Weighting which takes account of multiple input numu species.
  ///
  /// Defined such that W_numu + W_numubar = 1
  Double_t SpeciesWght_numu;
  ///\brief Weighting which takes account of multiple input nue species.
  ///
  /// Defined such that W_nue + W_nuebar = 1
  Double_t SpeciesWght_nue;
  ///\brief Weighting which takes account of multiple input neutrino species.
  ///
  /// Defined such that \Sum_species W_species = 1
  Double_t SpeciesWght;

  bool SetBranchAddresses(TChain*);
};

std::string WriteGiBUUEvent(GiBUUStdHepReader const& gi);
#endif
