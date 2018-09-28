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

#ifndef MINERVA_CC0PI_XSEC_2D_NU_H_SEEN
#define MINERVA_CC0PI_XSEC_2D_NU_H_SEEN

#include "Measurement2D.h"

//********************************************************************  
class MINERvA_CC0pi_XSec_2D_nu : public Measurement2D {
//********************************************************************  

 public:

  // Constructor
 MINERvA_CC0pi_XSec_2D_nu(nuiskey samplekey);

  // Destructor
  virtual ~MINERvA_CC0pi_XSec_2D_nu() {

    // Remove all the content histograms                     *
    //    for (int i = 0; i < 9; i++)  
      // delete this->fMCHist_content[i];   

    // delete everything
    /* delete difHist; */
    /* delete evtsignalHist; */
    /* delete fluxsignalHist; */				
    /* delete fMapHist; */
    /* delete status; */
    /* delete PDGHistogram; */
    
    /* // Delete MODE histograms */
    /* for (int i = 0; i < 60; i++) */
    /*   delete fMCHist_PDG[i]; */			     

    return;
  };

  // Required functions
  bool isSignal(FitEvent *nvect);
  void FillEventVariables(FitEvent *event);
  
 protected:
  // Converted covariance matrix to provide global binning method in GetLikelihood
  TH2D* covar_th2d;
  double GetLikelihood();

  // Set up settings based on distribution
  void SetupDataSettings();

 private:
  // The distribution privates
  int fDist;
  enum Distribution {
    // Pt Pz
    kPtPz,
    // Pt Q2
    kPtQ2
  };
  
};
  
#endif
