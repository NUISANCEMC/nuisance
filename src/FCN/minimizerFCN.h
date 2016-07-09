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

#ifndef _MINIMIZER_FCN_H_
#define _MINIMIZER_FCN_H_
/*!                                                                                                                                                                                                   
 *  \addtogroup FCN                                                                                                                                                                                 
 *  @{                                                                                                                                                                                                
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <list>

// ROOT headers
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include <TMatrixDSym.h>
#include "TGraphErrors.h"
#include <TVectorD.h>
#include <TMath.h>

// Minuit headers
#include "FitUtils.h"
#include "TVirtualFitter.h"
#include "Minuit2/FCNBase.h"
#include "TFitterMinuit.h"

// External fitter headers
#include "MeasurementBase.h"
#include "parameter_pulls.h"
#include "SampleList.h"

#include "Math/IFunction.h"
#define GetCurrentDir getcwd

using namespace FitUtils;

//! Main FCN Class which ROOT's minimizer function needs to evaulate the chi2 at each stage of the fit.
class minimizerFCN  
{
  
 private: 
    
  // Reweighting Engine
  FitWeight *f_rw;

  TFile *out;
  std::string card;
  std::string fakeData;
  std::string drawingOptions;

  TMatrixDSym* FullCovar;

  bool saveHists;
  bool useFullCovar;

  // Append the experiments to include in the fit to this list
  std::list<MeasurementBase*> fChain;
  
  void LoadSamples(std::string cardFile);
  void LoadDrawOpts(std::string cardFile);

 public:
  
  
 minimizerFCN(std::string cardfile, std::string fakeDataFile, TFile *outfile, FitWeight *a_rw = NULL, bool save = 0, bool minimizing=0) : f_rw(a_rw)
    {
      out       = outfile;
      FitPar::Config().out = outfile;
      card      = cardfile;
      fakeData  = fakeDataFile;
      saveHists = save;
      LoadParameters(card);
      LoadSamples(card);
      LoadDrawOpts(card);

      useFullCovar = false; // default
      this->current_iteration = 0;
      filledMC = false;
      randGen = new TRandom3();
      randGen->SetSeed(time(NULL));
    };
  
  ~minimizerFCN() {
    for (std::list<MeasurementBase*>::iterator iter = fChain.begin(); iter != fChain.end(); iter++){
      MeasurementBase* exp = *iter;
      delete exp;
    }
  };

  void SetOutName(std::string name){this->outname = name;}
  void Write();

  double operator() (const std::vector<double> & x) const{
    
    double* x_array = new double[x.size()];
    return this->DoEval(x_array);
  };
  
   
  double operator() (const double *x) const{
    return this->DoEval(x);
  };
  

  unsigned int NDim() {return this->GetNDOF();};
  
  
  /* ROOT::Math::IBaseFunctionMultiDim* Clone(){return this;}; */
  double Up() const { return 1.; }
  
  double DoEval(const double *x) const;
  void LoadParameters(std::string cardfile);

  int GetNDOF();
  void ReconfigureAllEvents() const;
  void ReconfigureSamples(bool fullconfig = false) const;
  
  double GetLikelihood() const;
  void CreateFullCovarMatrix();

  void SetFakeData(std::string fakeOpt);
  TH1D* GetXSecPlot(std::string type);
  void ThrowSamples();
  
  std::string outname;
  std::vector<std::string> sampleNames;
  std::vector<std::string> sampleTypes;
  std::vector<std::string> sampleFiles;
  
  std::vector<std::string>         parameterNames;
  mutable std::vector<double> parameterValues;
  mutable std::vector<double> sampleNorms;
  mutable std::vector<double> currentFitVals;
  mutable bool dialChanged;


  void ScanSignalEvents(std::string outputName="");
  void ReconfigureSignal();
  void SetSignalTree(TFile* varFile);

  mutable bool SerialEval;
  TRandom3* randGen;

  mutable UInt_t current_iteration;
  mutable  bool filledMC;

};

/*! @} */
#endif // _FIT_FCN_H_
