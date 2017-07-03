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

#include "MINERvA_CCQE_XSec_1DQ2_antinu.h"


//********************************************************************
MINERvA_CCQE_XSec_1DQ2_antinu::MINERvA_CCQE_XSec_1DQ2_antinu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CCQE_XSec_1DQ2_antinu sample. \n" \
    "Target: CH \n"						   \
    "Flux: MINERvA Forward Horn Current Numubar \n"		   \
    "Signal: True CCQE/2p2h defined at the vertex level \n";
  
  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{QE} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(1.5, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  isFluxFix      = !fSettings.Found("name", "_oldflux");
  fullphasespace = !fSettings.Found("name", "_20deg");

  // CCQELike plot information
  fSettings.SetTitle("MINERvA_CCQE_XSec_1DQ2_antinu");

  std::string basedir = FitPar::GetDataBase() + "/MINERvA/CCQE/";
  std::string datafilename  = "";
  std::string covarfilename = "";

  // Full Phase Space
  if (fullphasespace) {

    if (isFluxFix) {
      if (fIsShape) {
        ERR(WRN) << "SHAPE likelihood comparison not available for MINERvA "
                 << "datasets with fixed flux information. NUISANCE will scale MC to match "
                 << "data normalization but full covariance will be used. " << std::endl;
      }
      datafilename  = "Q2QE_numubar_data_fluxfix.txt";
      covarfilename = "Q2QE_numubar_covar_fluxfix.txt"; // Correlation Matrix

    } else {
      if (fIsShape) {
        datafilename  = "Q2QE_numubar_data_SHAPE-extracted.txt";
        covarfilename = "Q2QE_numubar_covar_SHAPE-extracted.txt"; // correlation
      } else {
        datafilename  = "Q2QE_numubar_data.txt";
        covarfilename = "Q2QE_numubar_covar.txt"; // Correlation
      }
    }

    // Restricted Phase Space
  } else {
    if (isFluxFix) {
      if (fIsShape) {
        ERR(WRN) << "SHAPE likelihood comparison not available for MINERvA "
                 << "datasets with fixed flux information. NUISANCE will scale MC to match "
                 << "data normalization but full covariance will be used. " << std::endl;
      }
      datafilename  = "20deg_Q2QE_numubar_data_fluxfix.txt";
      covarfilename = "20deg_Q2QE_numubar_covar_fluxfix.txt"; // Correlation

    } else {
      if (fIsShape) {
        datafilename  = "20deg_Q2QE_numubar_data_SHAPE-extracted.txt";
        covarfilename = "20deg_Q2QE_numubar_covar_SHAPE-extracted.txt"; // Correlation
      } else {
        datafilename  = "20deg_Q2QE_numubar_data.txt";
        covarfilename = "20deg_Q2QE_numubar_covar.txt"; // Correlation
      }
    }
  }

  fSettings.SetDataInput(  basedir + datafilename );
  fSettings.SetCovarInput( basedir + covarfilename );
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) * 13. / 7. / TotalIntegratedFlux();

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCorrelationFromTextFile( fSettings.GetCovarInput() );

  fExtra_Eav = new TH1D((fSettings.GetName() + "Eav").c_str(),"Eav",30, 0.0, 1.0);
  fExtra_Eav_MODES = new MINERvAUtils::ModeStack((fSettings.GetName() + "EavMODES").c_str(),"EavMODES", fExtra_Eav);
  //  fExtra_Eav_MODES->SetReverseStack();
  SetAutoProcess(fExtra_Eav);
  SetAutoProcess(fExtra_Eav_MODES);

  fExtra_EavQ2 = new TH2D((fSettings.GetName() + "EavQ2").c_str(),"EavQ2",12,0.0,2.0,50,0.0,1.0);
  fExtra_EavQ2_MODES = new MINERvAUtils::ModeStack((fSettings.GetName() + "EavQ2MODES").c_str(),"EavQ2MODES", fExtra_EavQ2);
  //  fExtra_EavQ2_MODES->SetReverseStack();
  SetAutoProcess(fExtra_EavQ2);
  SetAutoProcess(fExtra_EavQ2_MODES);

  fEavQ2Cut = new TF1((fSettings.GetName() + "f1").c_str(),"0.03 + 0.3*x",0.0,2.0);
  SetAutoProcess(fEavQ2Cut);
  
  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void MINERvA_CCQE_XSec_1DQ2_antinu::FillEventVariables(FitEvent *event) {
//********************************************************************

  if (event->NumFSParticle(-13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(-13)->fP;

  double ThetaMu  = Pnu.Vect().Angle(Pmu.Vect());
  double q2qe     = FitUtils::Q2QErec(Pmu, cos(ThetaMu), 30., true);

  fXVar = q2qe;

  double pm = 938.2720813;
  double pe = pm + 120.0;
  FitParticle* fakeproton = new FitParticle(0.0,0.0,sqrt(pe*pe-pm*pm), pe, 2212, kFinalState);
  double range = MINERvAUtils::RangeInScintillator(fakeproton, 100);
  
  pm = 139.57018;
  pe = pm + 65;
  FitParticle* fakepion = new FitParticle(0.0,0.0,sqrt(pe*pe-pm*pm), pe, 211, kFinalState);
  double pionrange = MINERvAUtils::RangeInScintillator(fakepion, 100);
  
  double Eav = 0.0;
  for (int i = 0; i < event->NParticles(); i++){
    if (event->GetParticleState(i) != kFinalState) continue;
    int pid = event->GetParticlePDG(i);
    double ParticleEav = 0.0;
    if (abs(pid) == 13) continue;
    if (pid != 2112 and pid != 22 and pid != 111){
      ParticleEav = MINERvAUtils::GetEDepositOutsideRangeInScintillator(event->GetParticle(i), range) / 1.E3;
    } else if (pid == 22 or pid == 111){
      ParticleEav = event->GetParticle(i)->fP.E()/1.E3;
    }    
    Eav += ParticleEav;
  }

  fExtra_Eav->Fill(Eav);
  fExtra_Eav_MODES->Fill( event->Mode, Eav );

  fExtra_EavQ2->Fill(q2qe,Eav);
  fExtra_EavQ2_MODES->Fill( event->Mode, q2qe, Eav );

  return;
}



//********************************************************************
bool MINERvA_CCQE_XSec_1DQ2_antinu::isSignal(FitEvent *event) {
//*******************************************************************
  return SignalDef::isCCQEnumubar_MINERvA(event, EnuMin, EnuMax, fullphasespace);
}
