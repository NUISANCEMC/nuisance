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

#ifndef MINERVA_CC1PIP_XSEC_1D_2017UPDATE_NU_H_SEEN
#define MINERVA_CC1PIP_XSEC_1D_2017UPDATE_NU_H_SEEN

#include "Measurement1D.h"

/*
  MINERvA 2017 update to the CC1pi+/- cross section measurement from 2014

  It is somewhat confusing, but this "2017Update" version was never published by MINERvA in a journal (although it was intended to), and contained important updates and fixes to the 2014 measurement, including a flux constrained by neutrino-electron scattering data. So *please use this data*; it is recommended by MINERvA.

  It is also the only data there is from MINERvA on CC1pi+/- in muon related kinematics; the first publication (Publication 3 below) only had Tpi and thpi.

  See the data release page here: https://minerva.fnal.gov/pion-data-release-page/
  I'm reproducing the text from that website here, in case it goes offline. I'll put my own comments in hard brackets [].
  {
  Publication 1: “Cross sections for neutrino and antineutrino induced pion production on hydrocarbon in the few-GeV region using MINERvA”, Phys. Rev. D 94, 052005 (2016)
  Publication 2: “Single neutral pion production by charged current antinu interactions on plastic scintillator at Enu ∼ 4 GeV”, Phys.Lett. B749 (2015) 130-136
  Publication 3: “Charged Pion Production in nu_mu Interactions on Hydrocarbon at ⟨E_nu⟩= 4.0 GeV” Phys. Rev. D 92, 092008 (2015)

  These measurements all use the low energy NUMI beams (average neutrino energy of 3.5-4.0 GeV), running in both neutrino and antineutrino configurations.

  Publication 1 above has both pion and muon kinematics with derived quantities.
  Publications 2 and 3 above emphasized pion kinematics. [Additionally, 3
  Publication 1 extends publications 2 and 3, also supercedes data from publication 3. 
  [VERY importantly, publication 1 only publishes a CCNpi+/- cross section, not a CC1pi+/-. So it supercedes some data from publication 3 (the CCNpi+/- data with W < 1.8 GeV/c2), but NOT ALL]

  Data files (including the original charged and neutral pion production results updated with the published flux) can be found at the following location: latex file of cross sections as function of both muon and pion kinematics
  The new data superceding data from publication 3 are provided at this this link (pdf file) 
  A new publication with these new data is in preparation, please reference Publication 3 until the new reference is available.
  Each file contains cross sections with statistical and systematic errors, a table of components of the systematic error, and covariance matrices. These are given for:

  muon energy
  muon polar angle
  pion kinetic energy
  pion polar angle
  momentum transfer squared
  Neutrino energy
  }
*/

class MINERvA_CC1pip_XSec_1D_2017Update : public Measurement1D {
public:
  MINERvA_CC1pip_XSec_1D_2017Update(nuiskey samplekey);
  virtual ~MINERvA_CC1pip_XSec_1D_2017Update() {};

  void SetupDataSettings();
  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);
  
  private:
  int fDist;
  
  enum DataDistribution {
    kTpi,
    kth,
    kpmu,
    kthmu,
    kQ2,
    kEnu
  } CC1pip_DataDistributions;

};

#endif
