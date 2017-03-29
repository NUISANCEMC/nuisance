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
                        "Target: CH \n" \
                        "Flux: MINERvA Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 electron, any nucleons, and no other FS particles \n";

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
      covarfilename = "Q2QE_numubar_covar_fluxfix.txt";

    } else {
      if (fIsShape) {
        datafilename  = "Q2QE_numubar_data_SHAPE-extracted.txt";
        covarfilename = "Q2QE_numubar_covar_SHAPE-extracted.txt";
      } else {
        datafilename  = "Q2QE_numubar_data.txt";
        covarfilename = "Q2QE_numubar_covar.txt";
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
      covarfilename = "20deg_Q2QE_numubar_covar_fluxfix.txt";

    } else {
      if (fIsShape) {
        datafilename  = "20deg_Q2QE_numubar_data_SHAPE-extracted.txt";
        covarfilename = "20deg_Q2QE_numubar_covar_SHAPE-extracted.txt";
      } else {
        datafilename  = "20deg_Q2QE_numubar_data.txt";
        covarfilename = "20deg_Q2QE_numubar_covar.txt";
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

  if (!isFluxFix or !fullphasespace) {
    SetCorrelationFromTextFile( fSettings.GetCovarInput() );
    ScaleCovar(1E76);
  } else {
    SetCovarFromTextFile( fSettings.GetCovarInput() );
  }

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
  return;
}



//********************************************************************
bool MINERvA_CCQE_XSec_1DQ2_antinu::isSignal(FitEvent *event) {
//*******************************************************************
  return SignalDef::isCCQEnumubar_MINERvA(event, EnuMin, EnuMax, fullphasespace);
}
