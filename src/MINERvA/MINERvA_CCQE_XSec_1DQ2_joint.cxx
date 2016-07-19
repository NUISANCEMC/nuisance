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

#include "MINERvA_CCQE_XSec_1DQ2_joint.h"

//********************************************************************  
MINERvA_CCQE_XSec_1DQ2_joint::MINERvA_CCQE_XSec_1DQ2_joint(std::string name, std::string inputfiles, FitWeight *rw, std::string  type, std::string fakeDataFile){
//********************************************************************  

  // Setup The Measurement
  measurementName = name;
  nBins = 16;
  plotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  isFluxFix      = name.find("_newflux") != std::string::npos;
  fullphasespace = name.find("_20deg")   == std::string::npos;
  isRatio = false;
  isSummed = false;
  saveSubMeas = true;
  SetupMeasurement(inputfiles, type, rw, fakeDataFile);

  // Get parsed input files
  if (subInFiles.size() != 2) ERR(FTL) << "MINERvA Joint requires input files in format: antinu;nu"<<std::endl;
  std::string inFileAntineutrino = subInFiles.at(0);
  std::string inFileNeutrino     = subInFiles.at(1);
    
  // Push classes back into list for processing loop
  subChain.push_back(MIN_anu);
  subChain.push_back(MIN_nu);

  // Setup the Data input                          
  std::string basedir = FitPar::GetDataBase()+"/MINERvA/";
  std::string datafilename  = "";
  std::string covarfilename = "";
  std::string neutrinoclass = "";
  std::string antineutrinoclass = "";

  // Full Phase Space                       
  if (fullphasespace){

    if (isFluxFix){
      if (isShape) isShape = false;
      datafilename  = "Q2QE_joint_data_fluxfix.txt";
      covarfilename = "Q2QE_joint_covar_fluxfix.txt";
      neutrinoclass = "MINERvA_CCQE_XSec_1DQ2_nu_newflux";
      antineutrinoclass = "MINERvA_CCQE_XSec_1DQ2_antinu_newflux";
      
    } else {
      if (isShape){
        datafilename  = "Q2QE_joint_dataa_SHAPE-extracted.txt";
        covarfilename = "Q2QE_joint_covara_SHAPE-extracted.txt";
      } else {
        datafilename  = "Q2QE_joint_data.txt";
        covarfilename = "Q2QE_joint_covar.txt";
      }
      neutrinoclass = "MINERvA_CCQE_XSec_1DQ2_nu";
      antineutrinoclass = "MINERvA_CCQE_XSec_1DQ2_antinu";
    }

  // Restricted Phase Space                                            
  } else {

    if (isFluxFix){
      if (isShape) isShape = false;
      datafilename  = "20deg_Q2QE_joint_data_fluxfix.txt";
      covarfilename = "20deg_Q2QE_joint_covar_fluxfix.txt";
      neutrinoclass = "MINERvA_CCQE_XSec_1DQ2_nu_20deg_newflux";
      antineutrinoclass = "MINERvA_CCQE_XSec_1DQ2_antinu_20deg_newflux";

    } else {
      if (isShape){
        datafilename  = "20deg_Q2QE_joint_dataa_SHAPE-extracted.txt";
        covarfilename = "20deg_Q2QE_joint_covara_SHAPE-extracted.txt";
      } else {
        datafilename  = "20deg_Q2QE_joint_data.txt";
        covarfilename = "20deg_Q2QE_joint_covar.txt";
      }
      neutrinoclass = "MINERvA_CCQE_XSec_1DQ2_nu_20deg";
      antineutrinoclass = "MINERvA_CCQE_XSec_1DQ2_antinu_20deg";
    }
  }

  // Setup Data
  this->SetDataValues( basedir + datafilename );
  this->SetCovarMatrixFromText( basedir + covarfilename, nBins);

  // Setup Experiments
  MIN_anu = new MINERvA_CCQE_XSec_1DQ2_antinu(antineutrinoclass, inFileAntineutrino, rw, type, fakeDataFile);
  MIN_nu  = new MINERvA_CCQE_XSec_1DQ2_nu    (neutrinoclass,     inFileNeutrino,     rw, type, fakeDataFile);
 
  // Add to chain for processing
  this->subChain.clear();
  this->subChain.push_back(MIN_anu);
  this->subChain.push_back(MIN_nu);

  this->fluxHist = GetCombinedFlux();
  this->eventHist = GetCombinedEventRate();

  // Setup Default MC Hists
  SetupDefaultHist();


};

//********************************************************************  
void MINERvA_CCQE_XSec_1DQ2_joint::MakePlots(){
//********************************************************************  

  UInt_t sample = 0;
  for (std::vector<MeasurementBase*>::const_iterator expIter = subChain.begin(); expIter != subChain.end(); expIter++){
    MeasurementBase* exp = static_cast<MeasurementBase*>(*expIter);
    
    if (sample == 0){

      MIN_anu = static_cast<MINERvA_CCQE_XSec_1DQ2_antinu*>(exp);
      TH1D* MIN_anu_mc = (TH1D*) MIN_anu->GetMCList().at(0);
      for (int i = 0; i < 8; i++){
	mcHist->SetBinContent(i+1, MIN_anu_mc->GetBinContent(i+1));
	mcHist->SetBinError(i+1, MIN_anu_mc->GetBinError(i+1));
      }
    } else if (sample == 1){

      MIN_nu = static_cast<MINERvA_CCQE_XSec_1DQ2_nu*>(exp);
      TH1D* MIN_nu_mc = (TH1D*) MIN_nu->GetMCList().at(0);
      for (int i = 0; i < 8; i++){
	mcHist->SetBinContent(i+1+8, MIN_nu_mc->GetBinContent(i+1));
      }

    } else break;
    sample++;
  }

  return;
}



