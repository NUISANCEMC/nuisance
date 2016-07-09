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

#ifndef PARAMETER_PULLS_H_SEEN
#define PARAMETER_PULLS_H_SEEN

/*!                                                                                                                                                                                                   
 *  \addtogroup FitBase                                                                                                                                                                               
 *  @{                                                                                                                                                                                                
 */

#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>

// ROOT includes
#include <TROOT.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TMatrixDSym.h>
#include <TDecompSVD.h>
#include <TVectorD.h>

// Fit Includes
#include "Measurement1D.h"
#include "FitUtils.h"
#include "FitLogger.h"
using namespace std;

//****************************************
//! Used to produce gaussian penalty terms in the fit.
class parameter_pulls : public Measurement1D {
//****************************************
  
public:

  //! Constructor
  //! Parameter pulls class is used to produce gaussian penalty terms in the fit given a covariance and list of central values.
  //!
  //! Pulls can be named in the sample card using parameter_pulls_ID. (e.g. parameter_pulls_CCQEfit).
  //! Results from a previous can be automatically used.
  parameter_pulls(std::string name, std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  
  //! Default destructor
  virtual ~parameter_pulls() {};
  
  // Neut event functions

  //! Signal definition is just set to true as this doesn't handle MC
  bool isSignal(FitEvent *nvect){return true;};

  //! Reconfigure function reads in current RW engine dials and sets their value to MC
  void Reconfigure(double norm, bool fullconfig=false);

  //! Renormalise doesn't do anything for pulls class
  void Renormalise(double norm);

  //! Chi2 returns the difference between current dial values and original
  //! values multiplied by the covariance.
  double GetChi2();

  // Functions to deal with the covariance matrix

  //! Return the inverted covariance matrix
  TH2D GetCovarMatrix(){ return TH2D(*covar);};

  //! Get the covariance matrix
  TH2D GetFullCovarMatrix();

  //! Set the data values from a file
  void SetDataValues(std::string dataFile);

  //! Save the histograms
  void Write(std::string drawOpt);

  //! Throw the dial values using the current covariance. Useful for parameter throws.
  void ThrowCovariance();

 private:
  std::string dialoption;

  std::string inFile;   //!< input file for the covariance
  std::map<std::string,int> binMap; //!< map between dial values and bins in dataHist
  TVectorD centralValues; //!< Vector of central values
};

/*! @} */  
#endif
