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

#include "MicroBooNE_CC1Mu2p_XSec_1D_nu.h"
#include "MicroBooNE_SignalDef.h"
#include "TH2D.h"

//********************************************************************
MicroBooNE_CC1Mu2p_XSec_1D_nu::MicroBooNE_CC1Mu2p_XSec_1D_nu(nuiskey samplekey) {
//********************************************************************
  fSettings = LoadSampleSettings(samplekey);
  std::string name = fSettings.GetS("name");
  std::string objSuffix;

  if (!name.compare("MicroBooNE_CC1Mu2p_XSec_1DDeltaPT_nu")) {
    fDist = kDeltaPT;
    objSuffix = "delta_PT";
    fSettings.SetXTitle("#deltaP_{T}^{true} (GeV)");
    fSettings.SetYTitle("d#sigma/d#deltaP_{T}^{true} (cm^{2}/(GeV/c)/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1Mu2p_XSec_1DOpening_Angle_Protons_Lab_nu")) {
    fDist = kOpening_Angle_Protons_Lab;
    objSuffix = "opening_angle_protons_lab";
    fSettings.SetXTitle("cos#gamma_{#vec{p}_{L},#vec{p}_{R}}^{true}");
    fSettings.SetYTitle("d#sigma/dcos#gamma_{#vec{p}_{L},#vec{p}_{R}}^{true} (cm^{2}/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1Mu2p_XSec_1DOpening_Angle_Mu_Both_nu")) {
    fDist = kOpening_Angle_Mu_Both;
    objSuffix = "opening_angle_mu_both";
    fSettings.SetXTitle("cos#gamma_{#vec{p}_{#mu},#vec{p}_{sum}}^{true} (GeV)");
    fSettings.SetYTitle("d#sigma/dcos#gamma_{#vec{p}_{#mu},#vec{p}_{sum}}^{true} (cm^{2}/^{40}Ar)");
  }
  else {
    assert(false);
  }

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n" \
                        "Target: Ar\n" \
                        "Flux: BNB FHC numu\n" \
                        "Signal: CC1Mu2p\n";

  fSettings.SetDescription(descrip);
  fSettings.SetTitle(name);
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 6.8);
  fSettings.DefineAllowedTargets("Ar");
  fSettings.DefineAllowedSpecies("numu");
  FinaliseSampleSettings();

  // Load data ---------------------------------------------------------
  std::string inputFile = FitPar::GetDataBase() + "/MicroBooNE/CC1Mu2p/CC2p_data_MC_cov_dataRelease.root";
  SetDataFromRootFile(inputFile, "h_total_systematic_uncertainty_" + objSuffix);
  ScaleData(1E-38);

  // ScaleFactor for DiffXSec/cm2/Nucleus // Already multiplied by the Ar mass number
  fScaleFactor = GetEventHistogram()->Integral("width") / fNEvents * 1E-38 / TotalIntegratedFlux() * 40;

  SetCovarFromRootFile(inputFile, "h_2D_total_covariance_matrix_" + objSuffix);

  // Final setup ------------------------------------------------------
  FinaliseMeasurement();
};


bool MicroBooNE_CC1Mu2p_XSec_1D_nu::isSignal(FitEvent* event) {
  
  // apapadop
  /*static int counter  = 0;
  static double old_mom = 0.;
  double new_mom = event->GetHMFSParticle(13)->fP.Vect().Mag();
  if (new_mom < 100 || new_mom > 1200) { return false; }
  bool signal = SignalDef::MicroBooNE::isCC1Mu2p(event, EnuMin, EnuMax);
  
    if (old_mom != new_mom) {  

      old_mom = new_mom; 

      if (signal) {

	int ProtonCounter = 0;
	std::vector<int> ProtonIndices = event->GetAllFSProtonIndices();
	std::vector<int> SignalProtonIndices;

	for (int i = 0; i < (int)(ProtonIndices.size()); i++) {

	  double mom = event->GetParticleMom( ProtonIndices.at(i) );
	  if (mom > 300 && mom < 1000) {

	    ProtonCounter++;
	    SignalProtonIndices.push_back( ProtonIndices.at(i) );

	  }

	}

	if (ProtonCounter != 2) { return false; }

	TVector3 vpmu = event->GetHMFSParticle(13)->fP.Vect();
	TVector3 vplead = event->GetParticleP3(SignalProtonIndices.at(0) );
	TVector3 vprecoil = event->GetParticleP3( SignalProtonIndices.at(1) );

	if ( vplead.Mag() < vprecoil.Mag() ) {

	  TVector3 temp = vplead;
	  vplead = vprecoil;
	  vprecoil = temp;

	}

	TVector3 vSumP = vplead + vprecoil;
	TVector3 vSumAll = vpmu + vplead + vprecoil;

	double DeltaPT = vSumAll.Pt() / 1000.; // GeV/c                                                                                                                                                         
	double CosThetaP_Lab = cos( vplead.Angle(vprecoil) );
	double CosThetaMu_Both = cos( vpmu.Angle(vSumP) );

	cout << "event = " << counter << ", mu mom = " << new_mom/1000. << ", p lead mom = " << vplead.Mag()/1000. << ", p recoil mom = " << vprecoil.Mag()/1000. << std::endl;
	cout << "      DeltaPT = " << DeltaPT  << ", opening_angle_protons_lab = " << CosThetaP_Lab << ", opening_angle_mu_both = " << CosThetaMu_Both << endl;
      }

      ++counter;

    }
    // end of apapadop
    */
  return SignalDef::MicroBooNE::isCC1Mu2p(event, EnuMin, EnuMax);
};


void MicroBooNE_CC1Mu2p_XSec_1D_nu::FillEventVariables(FitEvent* event) {

  if (event->NumFSParticle(13) == 0) return;
  if (event->NumFSParticle(2212) < 2) return;

  if ( !(SignalDef::MicroBooNE::isCC1Mu2p(event, EnuMin, EnuMax) ) ) { return; }

  TVector3 vpmu = event->GetHMFSParticle(13)->fP.Vect();
  if (vpmu.Mag() < 100 || vpmu.Mag() > 1200) { return; }

  int ProtonCounter = 0;
  std::vector<int> ProtonIndices = event->GetAllFSProtonIndices();
  std::vector<int> SignalProtonIndices;

  for (int i = 0; i < (int)(ProtonIndices.size()); i++) {

    double mom = event->GetParticleMom( ProtonIndices.at(i) );
    if (mom > 300 && mom < 1000) { 
      
      ProtonCounter++; 
      SignalProtonIndices.push_back( ProtonIndices.at(i) );

    }

  }

  if (ProtonCounter != 2) { return; }

  TVector3 vplead = event->GetParticleP3(SignalProtonIndices.at(0) );
  TVector3 vprecoil = event->GetParticleP3( SignalProtonIndices.at(1) );
  
  if ( vplead.Mag() < vprecoil.Mag() ) {  

    TVector3 temp = vplead;
    vplead = vprecoil;
    vprecoil = temp;

  }

  TVector3 vSumP = vplead + vprecoil;
  TVector3 vSumAll = vpmu + vplead + vprecoil;

  double DeltaPT = vSumAll.Pt() / 1000.; // GeV/c
  double CosThetaP_Lab = cos( vplead.Angle(vprecoil) );
  double CosThetaMu_Both = cos( vpmu.Angle(vSumP) );

  if (fDist == kDeltaPT) {
    fXVar = DeltaPT;
  }
  else if (fDist == kOpening_Angle_Protons_Lab) {
    fXVar = CosThetaP_Lab;
  }
  else if (fDist == kOpening_Angle_Mu_Both) {
    fXVar = CosThetaMu_Both;
  }
}
