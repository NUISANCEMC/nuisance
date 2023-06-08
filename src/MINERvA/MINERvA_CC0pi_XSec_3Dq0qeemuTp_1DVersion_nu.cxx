// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "MINERvA_CC0pi_XSec_3Dq0qeemuTp_1DVersion_nu.h"


MINERvA_CC0pi_XSec_3Dq0qeemuTp_1DVersion_nu::MINERvA_CC0pi_XSec_3Dq0qeemuTp_1DVersion_nu(
  nuiskey samplekey )
{

  std::string descrip = "MINERvA_CC0pi_XSec_3Dq0qeemuTp_nu sample\n"
    "Target: CH \n"
    "Flux: MINERvA Medium Energy FHC numu  \n"
    "Signal: CC-0pi \n";

  // Setup common settings
  fSettings = LoadSampleSettings( samplekey );
  fSettings.SetDescription( descrip );
  fSettings.SetXTitle( "GlobalBinIndex" );
  fSettings.SetYTitle( "d^{3}#sigma/dq^{QE}_{0}dE_{#mu}d#SigmaT_{p} (cm^{2}/GeV^{3}/nucleon)" );

  this->SetFitOptions("NOWIDTH");

  fSettings.SetAllowedTypes( "FIX,FREE,SHAPE/FULL,DIAG/MASK", "FIX/FULL" );
  fSettings.SetEnuRange( 0., 100. );
  fSettings.DefineAllowedTargets( "C,H" );
  fSettings.DefineAllowedSpecies( "numu" );

  fSettings.SetTitle( "MINERvA_CC0pi_XSec_3Dq0qeemuTp_1DVersion_nu" );

  fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()
    + "/data/MINERvA/CC0pi_3D/data_q0qeemusumtp_1D.root" );

  fSettings.SetCovarInput( GeneralUtils::GetTopLevelDir()
    + "/data/MINERvA/CC0pi_3D/data_q0qeemusumtp_1D.root" );

  std::string filepath = GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC0pi_3D/data_q0qeemusumtp_1D.root";
  TFile *f_forBinning = new TFile(filepath.c_str());
  hist_forBinning = (TH3D *)f_forBinning->Get("q0qeemusumtp_data_cross_section_with_total_unc");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor for energy-dependent (as opposed to flux-averaged)
  // total cross section (cm^2 / nucleon)

  std::cout << "GetEventHistogram()->Integral(\"width\") = " << GetEventHistogram()->Integral("width") << std::endl;
  std::cout << "fNEvents = " << fNEvents << std::endl;
  std::cout << "this->TotalIntegratedFlux() = " << this->TotalIntegratedFlux() << std::endl;

  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) / this->TotalIntegratedFlux();
  std::cout << "fScaleFactor = " << fScaleFactor << std::endl;

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile( fSettings.GetDataInput(), "q0qeemusumtp_data_cross_section_with_total_unc_1D" );
  SetCovarFromRootFile( fSettings.GetCovarInput(), "TMatrixT<double>" );

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

}

void MINERvA_CC0pi_XSec_3Dq0qeemuTp_1DVersion_nu::FillEventVariables( FitEvent* event ) {

  //********************************************************************
  // Checking to see if there is a Muon
  if (event->NumFSParticle(13) == 0) return;

  // Get the muon kinematics
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  Double_t emu = Pmu.E() / 1000.;
  Double_t pz = Pmu.Vect().Dot(Pnu.Vect() * (1.0 / Pnu.Vect().Mag())) / 1000.;

  static Double_t Ebind = 0.034;
  Double_t q0qe_num = pow(PhysConst::mass_proton,2) - pow(PhysConst::mass_neutron-Ebind,2) - pow(PhysConst::mass_muon,2) + 2.*(emu-pz)*emu;
  Double_t q0qe_den = 2.*(PhysConst::mass_neutron-Ebind) - emu + pz;

  Double_t q0qe = q0qe_num/q0qe_den;

  // y-axis is transverse momentum for both measurements

  // Set Hist Variables

  // Sum up kinetic energy of protons
  double sum = 0.0;
  std::vector<FitParticle *> allFSProtons = event->GetAllFSProton();
  for (const auto& it:allFSProtons){
    sum += it->KE()/1000.;
  }

  //==== Note) fXVar is the bin index for a given (Sum Tp, q_{0}^{QE}, E_mu)
  fXVar = GetBinIndexFromKinamatics(sum, q0qe, emu);
  //std::cout << "sum = " << sum << ", q0qe = " << q0qe << ", emu = " << emu << " -> fXVar = " << fXVar << std::endl;

}

bool MINERvA_CC0pi_XSec_3Dq0qeemuTp_1DVersion_nu::isSignal( FitEvent* event ) {
  return SignalDef::isCC0pi_MINERvAPTPZ(event, 14, EnuMin, EnuMax);
}

int MINERvA_CC0pi_XSec_3Dq0qeemuTp_1DVersion_nu::GetBinIndexFromKinamatics(double sum, double q0qe, double emu){
  return hist_forBinning->FindBin(sum, q0qe, emu);
}
