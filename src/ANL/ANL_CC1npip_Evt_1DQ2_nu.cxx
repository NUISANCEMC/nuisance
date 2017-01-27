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

#include "ANL_CC1npip_Evt_1DQ2_nu.h"

// The constructor
// User can specify W < 1.4 or no W cut in std::string type (W14 or NOW, default reverts to NOW)
//********************************************************************
ANL_CC1npip_Evt_1DQ2_nu::ANL_CC1npip_Evt_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
//********************************************************************

  fName = "ANL_CC1npip_Evt_1DQ2_nu";
  fPlotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); Number of events";
  EnuMin = 0;
  EnuMax = 1.5;
  fIsDiag = true;
  fIsRawEvents = true;
  fDefaultTypes="EVT/SHAPE/DIAG";
  fAllowedTypes = "EVT/SHAPE/DIAG/W14/NOW";

  // User can specify W < 1.4 or no W cut
  if (type.find("W14") != std::string::npos) {
    HadCut = 1.4;
  } else {
    HadCut = 10.0;
  }
    
  std::string DataLocation = GeneralUtils::GetTopLevelDir()+"/data/ANL/CC1pip_on_n/";
  if (HadCut == 1.4) {
    DataLocation += "ANL_CC1pip_on_n_noEvents_Q2_W14GeV_firstQ2rem.txt";
  } else {
    DataLocation += "ANL_CC1pip_on_n_noEvents_Q2_noWcut_firstQ2rem.txt";
  }

  // Get rid of the slashes in the type
  if (!type.empty() && type != "DEFAULT") {
    std::string temp_type = type;
    std::replace(temp_type.begin(), temp_type.end(), '/', '_');
    fName += "_"+temp_type;
  }

  SetupMeasurement(inputfile, type, rw, fakeDataFile);

  SetDataValues(DataLocation);
  SetupDefaultHist();

  // Set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  // Setup Covariance
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  fScaleFactor = (GetEventHistogram()->Integral()/double(fNEvents));
};


//********************************************************************
void ANL_CC1npip_Evt_1DQ2_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

  if (event->NumFSParticle(2112) == 0 || event->NumFSParticle(211) == 0 || event->NumFSParticle(13) == 0) {
    return;
  }

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pn   = event->GetHMFSParticle(2112)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pn, Ppip);
  double q2CCpip;

  // ANL has a M(pi, p) < 1.4 GeV cut imposed (also no cut measurement but not useful for delta tuning)
  if (hadMass < HadCut*1000.) {
    q2CCpip = -1.0*(Pnu-Pmu).Mag2()/1.E6;
  } else {
    q2CCpip = -1.0;
  }

  fXVar = q2CCpip;

  return;
};


//********************************************************************
bool ANL_CC1npip_Evt_1DQ2_nu::isSignal(FitEvent *event) {
//********************************************************************

  return SignalDef::isCC1pi3Prong(event, 14, 211, 2112, EnuMin, EnuMax);
}
