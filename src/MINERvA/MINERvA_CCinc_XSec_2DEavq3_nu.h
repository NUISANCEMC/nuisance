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

#ifndef MINERVA_XSEC_CCINC_2DEAVQ3_H_SEEN
#define MINERVA_XSEC_CCINC_2DEAVQ3_H_SEEN

#include "Measurement2D.h"

//********************************************************************  
class MINERvA_CCinc_XSec_2DEavq3_nu : public Measurement2D {
//********************************************************************  

 public:

  // Constructor
 MINERvA_CCinc_XSec_2DEavq3_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);

  // Destructor
  virtual ~MINERvA_CCinc_XSec_2DEavq3_nu() {

    // Remove all the content histograms
    //    for (int i = 0; i < 9; i++)  
      // delete this->mcHist_content[i];   

    // delete everything
    /* delete difHist; */
    /* delete evtsignalHist; */
    /* delete fluxsignalHist; */
    /* delete mapHist; */
    /* delete status; */
    /* delete PDGHistogram; */
    
    /* // Delete MODE histograms */
    /* for (int i = 0; i < 60; i++) */
    /*   delete mcHist_PDG[i]; */

    return;
  };

  // Required functions
  bool isSignal(FitEvent *nvect);
  void FillEventVariables(FitEvent *event);
  
 protected:
  
  // Cuts
  bool hadroncut;
  bool useq3true;
  bool splitMEC_PN_NN;
};
  
#endif
