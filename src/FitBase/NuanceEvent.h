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

#ifndef NUANCE_H_SEEN
#define NUANCE_H_SEEN

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
  double iniQ;
  double finQ;
  int lepton0;
  double polar;
  int channel;
  double qsq;
  double w;
  double x;
  double y;

  double p_neutrino[4];
  double p_targ[5];
  double vertex[4];
  double start[4];
  double depth;
  double flux;

  int n_leptons;
  
  double p_ltot[5];
  int lepton[200];
  double p_lepton[5][200];
  
  int n_hadrons;
  double p_htot[5];
  int hadron[200];
  double p_hadron[5][200];
};
#endif
