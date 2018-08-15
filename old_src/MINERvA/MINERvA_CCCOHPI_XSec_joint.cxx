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

#include "MINERvA_SignalDef.h"
#include "MINERvA_CCCOHPI_XSec_joint.h"
#include "MINERvA_CCCOHPI_XSec_1DEpi_nu.h"
#include "MINERvA_CCCOHPI_XSec_1DEpi_antinu.h"
#include "MINERvA_CCCOHPI_XSec_1Dth_nu.h"
#include "MINERvA_CCCOHPI_XSec_1Dth_antinu.h"
#include "MINERvA_CCCOHPI_XSec_1DEnu_nu.h"
#include "MINERvA_CCCOHPI_XSec_1DEnu_antinu.h"
#include "MINERvA_CCCOHPI_XSec_1DQ2_nu.h"
#include "MINERvA_CCCOHPI_XSec_1DQ2_antinu.h"

//********************************************************************
void MINERvA_CCCOHPI_XSec_joint::SetupDataSettings(){
//********************************************************************

  // Set Distribution
  std::string name = fSettings.GetS("name");
  if      (!name.compare("MINERvA_CCCOHPI_XSec_1DEpi_joint"))  fDist = kEpi;
  else if (!name.compare("MINERvA_CCCOHPI_XSec_1Dth_joint"))   fDist = kth;
  else if (!name.compare("MINERvA_CCCOHPI_XSec_1DQ2_joint"))   fDist = kQ2;
  else if (!name.compare("MINERvA_CCCOHPI_XSec_1DEnu_joint"))  fDist = kEnu;
  
  // Define what files to use from the dist
  std::string datafile = "";
  std::string covarfile = "";
  std::string titles = "";
  std::string distdescript = "";
  switch(fDist){
    
  case (kEpi):
    datafile    = "Epi_joint_data.csv";
    covarfile   = "Epi_joint_cov.csv";
    titles      = "CC-Coherent #pi;E_{#pi} Bins;d#sigma/dE_{#pi} (cm^{2}/GeV/C12)";
    break;

  case (kth):
    datafile  = "Thpi_joint_data.csv";
    covarfile = "Thpi_joint_cov.csv";
    titles    = "CC-Coherent #pi;#theta_{#pi} Bins;d#sigma/d#theta_{#pi} (cm^{2}/C12)";
    break;

  case (kQ2):
    datafile  = "Q2_joint_data.csv";
    covarfile = "Q2_joint_cov.csv";
    titles    ="CC-Coherent #pi;Q^{2} Bins;d#sigma/dQ^{2} (cm^{2}/GeV^{2}/C12)";
    break;

  case (kEnu):
    datafile  = "Enu_joint_data.csv";
    covarfile = "Enu_joint_cov.csv";
    titles    ="CC-Coherent #pi;E_{#nu} Bins;#sigma(E_{#nu}) (cm^{2}/C12";
    break;
    
  default:
    THROW("Unknown Analysis Distribution : " << fDist);
  }
  
  // Now setup each data distribution and description.
  std::string descrip =  distdescript +			     \
    "Target: CH \n"							\
    "Flux: 2 Files, MINERvA FHC numu;MINERvA FHC numubar \n"		\
    "Signal: Any event with 1 electron, any nucleons, and no other FS particles \n";
  
  fSettings.SetDescription(descrip);
  fSettings.SetDataInput(  GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCcoh/" + datafile  );
  fSettings.SetCovarInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCcoh/" + covarfile );
  fSettings.SetTitle(  GeneralUtils::ParseToStr(titles,";")[0] );
  fSettings.SetXTitle( GeneralUtils::ParseToStr(titles,";")[1] );
  fSettings.SetYTitle( GeneralUtils::ParseToStr(titles,";")[2] );
  
  return;
}

//********************************************************************
MINERvA_CCCOHPI_XSec_joint::MINERvA_CCCOHPI_XSec_joint(nuiskey samplekey) {
//********************************************************************

  // Define Sample Settings common to all data distributions
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 20.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");
  SetupDataSettings();
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  fScaleFactor = 0.0;
  
  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromMultipleTextFiles(fSettings.GetCovarInput());

  // Setup Sub Measurements -------------------------------------------------------
  SetupSubMeasurements();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void MINERvA_CCCOHPI_XSec_joint::SetupSubMeasurements(){
//********************************************************************
  
  // Get parsed input files
  if (fSubInFiles.size() != 2) {
    ERR(FTL) << "MINERvA Joint requires input files in format: nu;antinu" << std::endl;
  }
  std::string inFileNeutrino         = fSubInFiles.at(0);
  std::string inFileAntineutrino     = fSubInFiles.at(1);

  // Create some config keys
  nuiskey nukey = Config::CreateKey("sample");
  nukey.SetS("input", inFileNeutrino);
  nukey.SetS("type", fSettings.GetS("type"));
  
  nuiskey antinukey = Config::CreateKey("sample");
  antinukey.SetS("input", inFileAntineutrino);
  antinukey.SetS("type", fSettings.GetS("type"));

  // Create samples for given DIST
  switch(fDist){

  case kEpi:
    nukey.SetS("name","MINERvA_CCCOHPI_XSec_1DEpi_nu");
    MIN_nu = new MINERvA_CCCOHPI_XSec_1DEpi_nu(nukey);
    antinukey.SetS("name","MINERvA_CCCOHPI_XSec_1DEpi_antinu");
    MIN_anu = new MINERvA_CCCOHPI_XSec_1DEpi_antinu(antinukey);
    break;

  case kth:
    nukey.SetS("name","MINERvA_CCCOHPI_XSec_1Dth_nu");
    MIN_nu = new MINERvA_CCCOHPI_XSec_1Dth_nu(nukey);
    antinukey.SetS("name","MINERvA_CCCOHPI_XSec_1Dth_antinu");
    MIN_anu = new MINERvA_CCCOHPI_XSec_1Dth_antinu(antinukey);
    break;

  case kEnu:
    nukey.SetS("name","MINERvA_CCCOHPI_XSec_1DEnu_nu");
    MIN_nu = new MINERvA_CCCOHPI_XSec_1DEnu_nu(nukey);
    antinukey.SetS("name","MINERvA_CCCOHPI_XSec_1DEnu_antinu");
    MIN_anu = new MINERvA_CCCOHPI_XSec_1DEnu_antinu(antinukey);
    break;

  case kQ2:
    nukey.SetS("name","MINERvA_CCCOHPI_XSec_1DQ2_nu");
    MIN_nu = new MINERvA_CCCOHPI_XSec_1DQ2_nu(nukey);
    antinukey.SetS("name","MINERvA_CCCOHPI_XSec_1DQ2_antinu");
    MIN_anu = new MINERvA_CCCOHPI_XSec_1DQ2_antinu(antinukey);
    break;
  }
 
  // Add to chain for processing
  fSubChain.clear();
  fSubChain.push_back(MIN_anu);
  fSubChain.push_back(MIN_nu);
}

//********************************************************************
void MINERvA_CCCOHPI_XSec_joint::MakePlots() {
//********************************************************************

  TH1D* MIN_nu_mc  = (TH1D*) MIN_nu->GetMCHistogram();
  TH1D* MIN_anu_mc = (TH1D*) MIN_anu->GetMCHistogram();
  int count = 0;
  for (int i = 0; i < MIN_nu_mc->GetNbinsX(); i++){
    fMCHist->SetBinContent( count+1, MIN_nu_mc->GetBinContent(i+1) );
    fMCHist->SetBinError(   count+1, MIN_nu_mc->GetBinError(i+1) );
    fMCHist->GetXaxis()->SetBinLabel(count+1, GeneralUtils::IntToStr(count+1).c_str());
    count++;
  }
  for (int i = 0; i < MIN_anu_mc->GetNbinsX(); i++){
    fMCHist->SetBinContent(count+1, MIN_anu_mc->GetBinContent(i+1) );
    fMCHist->SetBinError(count+1,   MIN_anu_mc->GetBinError(i+1) );
    fMCHist->GetXaxis()->SetBinLabel(count+1, GeneralUtils::IntToStr(count+1).c_str());
    count++;
  }
 
  return;
}



