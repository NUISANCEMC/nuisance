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

#include "BEBC_CC1npim_XSec_1DEnu_antinu.h"



//********************************************************************
BEBC_CC1npim_XSec_1DEnu_antinu::BEBC_CC1npim_XSec_1DEnu_antinu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "BEBC_CC1npim_XSec_1DEnu_antinu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("#sigma(E_{#nu}) (cm^{2}/neutron)");
  fSettings.SetAllowedTypes("FIX/FREE,SHAPE/DIAG", "FIX/DIAG");
  fSettings.SetEnuRange(5.0, 200.0);
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("BEBC_CC1npim_XSec_1DEnu_antinu");
  fSettings.DefineAllowedSpecies("numub");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/BEBC/theses/BEBC_theses_ANU_CC1pi-_nFin_W14.txt" );

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents)*(2./1.);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void BEBC_CC1npim_XSec_1DEnu_antinu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2112) == 0 ||
      event->NumFSParticle(-211) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pn   = event->GetHMFSParticle(2112)->fP;
  TLorentzVector Ppim = event->GetHMFSParticle(-211)->fP;

  double hadMass = FitUtils::MpPi(Pn, Ppim);
  double Enu     = -1.0;

  if (hadMass < 1400) Enu = Pnu.E()/1.E3;
  fXVar = Enu;

  return;
};




bool BEBC_CC1npim_XSec_1DEnu_antinu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, -14, -211, 2112, EnuMin, EnuMax);
}


