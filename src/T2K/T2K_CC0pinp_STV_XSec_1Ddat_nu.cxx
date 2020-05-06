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

#include "T2K_SignalDef.h"

#include "T2K_CC0pinp_STV_XSec_1Ddat_nu.h"

//********************************************************************
T2K_CC0pinp_STV_XSec_1Ddat_nu::T2K_CC0pinp_STV_XSec_1Ddat_nu(
    nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC0pinp_STV_XSec_1Ddpt_nu sample. \n"
                        "Target: CH \n"
                        "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                        "Signal: CC0piNp (N>=1) with 450 MeV < p_p < 1 GeV \n"
                        "                            p_mu > 250 MeV \n"
                        "                            cth_p >  0.4 \n"
                        "                            cth_mu > -0.6 \n"
                        "https://doi.org/10.1103/PhysRevD.98.032003 \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("#delta#it{#alpha}_{T} (GeV c^{-1})");
  fSettings.SetYTitle(
      "#frac{d#sigma}{d#delta#it{#alpha}_{T}} (cm^{2} nucleon^{-1} rads^{-1})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 50.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("T2K_CC0pinp_STV_XSec_1Ddat_nu");
  // fSettings.SetDataInput(  GeneralUtils::GetTopLevelDir() +
  // "/data/T2K/T2K_CC0pinp_STV_XSec_1Ddat_nu.dat");

  fSettings.SetDataInput(FitPar::GetDataBase() +
                         "/T2K/CC0pi/STV/datResults.root;Result");
  fSettings.SetCovarInput(FitPar::GetDataBase() +
                          "/T2K/CC0pi/STV/datResults.root;Correlation_Matrix");

  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) /
                 (double(fNEvents) * TotalIntegratedFlux("width"));

  // Plot Setup -------------------------------------------------------
  // SetDataFromTextFile( fSettings.GetDataInput() );
  // ScaleData(1E-38);
  // SetCovarFromDiagonal();

  SetDataFromRootFile(fSettings.GetDataInput());
  SetCorrelationFromRootFile(fSettings.GetCovarInput());

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

void T2K_CC0pinp_STV_XSec_1Ddat_nu::FillEventVariables(FitEvent *event) {
  fXVar = FitUtils::Get_STV_dalphat_HMProton(event, 14, true);
  return;
};

//********************************************************************
bool T2K_CC0pinp_STV_XSec_1Ddat_nu::isSignal(FitEvent *event)
//********************************************************************
{
  return SignalDef::isT2K_CC0pi_STV(event, EnuMin, EnuMax);
}
