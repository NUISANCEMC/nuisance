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

#include "MINERvA_CCinc_XSec_1Dx_nu.h"

//******************************************************************** 
MINERvA_CCinc_XSec_1Dx_nu::MINERvA_CCinc_XSec_1Dx_nu(std::string name, std::string inputfile, FitWeight *rw, std::string type, 
						     std::string fakeDataFile){
//******************************************************************** 

  // Measurement Details                                                                                                           
  measurementName = name;
  plotTitles = "; Reconstructed Bjorken x; d#sigma/dx (cm^{2}/nucleon)";
  EnuMin = 2.;
  EnuMax = 20.;
  target = "";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  if      (name.find("C12")   != std::string::npos) target =   "C12";
  else if (name.find("Fe56")  != std::string::npos) target =  "Fe56";
  else if (name.find("Pb208") != std::string::npos) target = "Pb208";
  if      (name.find("DEN")   != std::string::npos) target =   "CH";
  if (target == "") ERR(WRN) << "target " << target << " was not found!" << std::endl;

  // Setup the Data Plots
  std::string basedir = FitPar::GetDataBase()+"/MINERvA/CCinc/";
  std::string smearfilename  = "CCinc_"+target+"_x_smear.csv";
  int nbins = 6;
  double bins[7] = {0, 0.1, 0.3, 0.7, 0.9, 1.1, 1.5};

  // Note that only the ratio is given, so I can't do this->SetDataValues or this->SetCovarMatrix
  this->dataHist = new TH1D(name.c_str(),(name+plotTitles).c_str(),nbins,bins);

  // Setup Default MC Histograms
  this->SetupDefaultHist();

  // The smearing matrix is rectangular
  this->SetSmearingMatrix(basedir + smearfilename, nbins, nbins+1);

  // Set Scale Factor (EventHist/nucleons) so I don't need to know what the target is here
  this->scaleFactor = (this->eventHist->Integral("width")*1E-38/(nevents+0.))/this->TotalIntegratedFlux(); // NEUT
  
};

//********************************************************************
void MINERvA_CCinc_XSec_1Dx_nu::FillEventVariables(FitEvent *event){
//********************************************************************
  
  Enu  = (event->PartInfo(0))->fP.E()/1000.0;

  // Get the relevant signal information
  for (int j = 0; j < event->Npart(); ++j){

    if ((event->PartInfo(j))->fPID != 13) continue;

    ThetaMu     = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());
    TLorentzVector q = ((event->PartInfo(0))->fP - (event->PartInfo(j))->fP);
    double q0   = q.E()/1000.0;
    double Emu  = ((event->PartInfo(j))->fP.E())/1000.0;
    Enu_rec     = Emu + q0;
    double Q2   = 4*Enu_rec*Emu*sin(ThetaMu/2)*sin(ThetaMu/2);
    bjork_x     = Q2/2./q0/0.938918; // Average nucleon mass is 0.938918 GeV
    break;
  }

  this->X_VAR   = bjork_x;
  return;
}



//********************************************************************
bool MINERvA_CCinc_XSec_1Dx_nu::isSignal(FitEvent *event){
//*******************************************************************

  // Throw away NC events
  if (event->Mode > 30) return false;

  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Restrict the phase space to theta < 17 degrees
  if (ThetaMu > 0.296706) return false;

  // restrict energy range
  if (Enu < this->EnuMin || Enu > this->EnuMax) return false;
  if (Enu_rec < this->EnuMin || Enu_rec > this->EnuMax) return false;

  return true;
};

//********************************************************************
void MINERvA_CCinc_XSec_1Dx_nu::ScaleEvents(){
//********************************************************************
  
  this->dataHist = (TH1D*)this->GetMCList().at(0)->Clone();
  this->dataHist->SetNameTitle((this->measurementName+"_unsmear").c_str(), (this->measurementName+"_unsmear"+this->plotTitles).c_str());
  this->ApplySmearingMatrix();

  // Get rid of this because it causes odd behaviour
  //Measurement1D::ScaleEvents();

  this->mcHist->Scale(this->scaleFactor, "width");

  // Proper error scaling - ROOT Freaks out with xsec weights sometimes
  for(int i=0; i<this->mcStat->GetNbinsX();i++) {
    
    if (this->mcStat->GetBinContent(i+1) != 0)
      this->mcHist->SetBinError(i+1, this->mcHist->GetBinContent(i+1) * this->mcStat->GetBinError(i+1) / this->mcStat->GetBinContent(i+1) );
    else this->mcHist->SetBinError(i+1, this->mcHist->Integral());
  }

}
