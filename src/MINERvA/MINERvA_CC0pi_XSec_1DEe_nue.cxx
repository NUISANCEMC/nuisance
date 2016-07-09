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

#include "MINERvA_CC0pi_XSec_1DEe_nue.h"

//******************************************************************** 
MINERvA_CC0pi_XSec_1DEe_nue::MINERvA_CC0pi_XSec_1DEe_nue(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//******************************************************************** 

  // Measurement Details
  measurementName = "MINERvA_CC0pi_1DEe_nue";
  plotTitles = "; E_{e} (GeV); d#sigma/dE_{e} (cm^{2}/GeV)";
  EnuMin =  1.5;
  EnuMax = 10.0;
  isRatio = false;
  isSummed = true;
  saveSubMeas = false;
  normError = 0.101;
  JointMeas1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);
  
  // Setup Data Plots
  std::string datafile = std::string(std::getenv("NIWG_DATA"))+"/MINERvA/MINERvA_CC0pi_nue_Data_ARX1509_05729.root";

  this->SetDataFromFile(datafile, "Data_1DEe");
  this->SetCovarFromDataFile(datafile, "Covar_1DEe");

  // Setup MC
  SetupDefaultHist();

  // Setup Sub Samples that fill this hist
  std::string neutrinosample = subInFiles.at(0);
  std::string antineutrinosample = subInFiles.at(1);
  
  // Setup Samples
  nue_sample    = new MINERvA_CC0pi_XSec_1DFill_nue(neutrinosample, rw, type, fakeDataFile, true, 0);
  nuebar_sample = new MINERvA_CC0pi_XSec_1DFill_nue(antineutrinosample, rw, type, fakeDataFile, false, 0);

  // Add to sub list
  subChain.clear();
  subChain.push_back(nue_sample);
  subChain.push_back(nuebar_sample);
  
  // Get Flux and Event Hist
  this->fluxHist = GetCombinedFlux();
  this->eventHist = GetCombinedEventRate();
  
};

