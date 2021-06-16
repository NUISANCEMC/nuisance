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

#include "FNAL_CC1ppip_XSec_1DEnu_nu.h"

//********************************************************************
FNAL_CC1ppip_XSec_1DEnu_nu::FNAL_CC1ppip_XSec_1DEnu_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "FNAL_CC1ppip_XSec_1DEnu_nu sample. \n" \
                        "Target: D2 \n" \
                        "Flux:  \n" \
                        "Signal:  \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("#sigma(E_{#nu}) (cm^{2}/proton)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG", "FIX/DIAG");
  fSettings.SetEnuRange(10.0, 100.0);
  fSettings.DefineAllowedTargets("D,H");

  // plot information
  fSettings.SetTitle("FNAL_CC1ppip_XSec_1DEnu_nu");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "FNAL/CC1pip_on_p/fnal78-numu-p-to-mu-p-piplus-lowW_edges.txt");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor for shape
  fScaleFactor =  GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents)*(2./1.);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

}


void FNAL_CC1ppip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(211) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double Enu     = -1.0;

  if (hadMass < 1400) Enu = Pnu.E()/1.E3;
  fXVar = Enu;

  return;
}

bool FNAL_CC1ppip_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}

