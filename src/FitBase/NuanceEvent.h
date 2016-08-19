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

#ifndef NUANCE_H_SEEN
#define NUANCE_H_SEEN

#ifdef __NUANCE_ENABLED__
/*!
 *  \addtogroup FitBase
 *  @{
 */

class NuanceEvent {
 public:

  bool cc;
  bool bound;
  int neutrino;
  int target;
  float iniQ;
  float finQ;
  int lepton0;
  float polar;
  int channel;
  float qsq;
  float w;
  float x;
  float y;

  float p_neutrino[4];
  float p_targ[5];
  float vertex[4];
  float start[4];
  float depth;
  float flux;

  int n_leptons;
  
  float p_ltot[5];
  int lepton[200];
  float p_lepton[5][200];
  
  int n_hadrons;
  float p_htot[5];
  int hadron[200];
  float p_hadron[5][200];
};
#endif
#endif
