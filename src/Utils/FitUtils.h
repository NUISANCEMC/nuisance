// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef FITUTILS_H_SEEN
#define FITUTILS_H_SEEN

// C Includes
#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <iostream>
#include <unistd.h>
#include <ctime>

// ROOT includes
#include <TROOT.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TFile.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include <TList.h>
#include <TKey.h>
#include <THStack.h>
#include <TLegend.h>
#include <TObjArray.h>
#include <TRandom3.h>
#include "TH2Poly.h"
#include "TGraph.h"

// Fit  includes
#include "FitParameters.h"

/*!
 *  \addtogroup Utils
 *  @{
 */

//! Functions needed by individual samples for calculating kinematic quantities.
namespace FitUtils{

  /*
    MISC
  */

  //! Function to produce a histogram indicating the fraction of events which have q3 < qCut
  TH2D* CalculateQ3Cut(std::string inFile, TH2D *data, double qCut, int nuPDG, double eMin, double eMax);

  /*
    MISC Event
  */

  //! Returns kinetic energy of particle
  double T(TLorentzVector part);

  //! Returns momentum of particle
  double p(TLorentzVector part);

  //! Returns angle between particles (_NOT_ cosine!)
  double th(TLorentzVector part, TLorentzVector part2);

  //! Hadronic mass reconstruction
  double Wrec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppi);


  /*
    CCQE MiniBooNE/MINERvA
  */
  //! Function to calculate the reconstructed Q^{2}_{QE}
  double Q2QErec(TLorentzVector pmu, double costh, double binding, bool neutrino = true);

  //! Function returns the reconstructed E_{nu} values
  double EnuQErec(TLorentzVector pmu, double costh, double binding, bool neutrino = true);


  /*
    CCQE1p MINERvA
  */
  //! Reconstruct Q2QE given just the maximum energy proton.
  double ProtonQ2QErec(double pE, double binding);

  /*
    CCpi0 MiniBooNE
  */
   //! Reconstruct Enu from CCpi0 vectors and binding energy
  double EnuCC1pi0rec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppi0);

  //! Reconstruct Q2 from CCpi0 vectors and binding energy
  double Q2CC1pi0rec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppi0);

  /*
    CCpi+ MiniBooNE
  */

  //! returns reconstructed Enu a la MiniBooNE CCpi+
  double EnuCC1piprec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppip);

  //! returns reconstructed Enu assumming resonance interaction where intermediate resonance was a Delta
  double EnuCC1piprecDelta(TLorentzVector pnu, TLorentzVector pmu);
  
  //! returns reconstructed Q^2 a la MiniBooNE CCpi+
  double Q2CC1piprec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppip);

  /*
    nucleon single pion
  */
  double MpPi(TLorentzVector pp, TLorentzVector ppi);

}

/*! @} */
#endif
