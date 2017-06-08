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

/**
 * Radecky et al. Phys Rev D, 3rd series, volume 25, number 5, 1 March 1982, p 1161-1173
*/

#include "ANL_CC1ppip_Evt_1DQ2_nu.h"

// The constructor
ANL_CC1ppip_Evt_1DQ2_nu::ANL_CC1ppip_Evt_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {

  fName = "ANL_CC1ppip_Evt_1DQ2_nu";
  fPlotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); Number of events";
  EnuMin = 0;
  EnuMax = 6.0;
  fIsDiag = true;
  fIsRawEvents = true;
  fDefaultTypes="EVT/SHAPE/DIAG";
  fAllowedTypes="EVT/SHAPE/DIAG/W14/NOW";

  // User can specify W < 1.4 or no W cut
  if (type.find("W14") != std::string::npos) {
    HadCut = 1.4;
  } else {
    HadCut = 10.0;
  }
    
  std::string DataLocation = GeneralUtils::GetTopLevelDir()+"/data/ANL/CC1pip_on_p/";
  if (HadCut == 1.4) {
    DataLocation += "ANL_CC1pip_on_p_noEvents_Q2_W14GeV_rebin_firstQ2rem.txt";
  } else {
    //DataLocation += "ANL_CC1pip_on_p_noEvents_Q2_noW_firstQ2rem.txt";
    DataLocation += "ANL_CC1pip_on_p_noEvents_Q2_noW_HighQ2Gone.txt";
  }
  // Get rid of the slashes in the type
  if (!type.empty() && type != "DEFAULT") {
    std::string temp_type = type;
    std::replace(temp_type.begin(), temp_type.end(), '/', '_');
    fName += "_"+temp_type;
  }

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  SetDataValues(DataLocation);
  SetupDefaultHist();

  // Set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  fScaleFactor = GetEventHistogram()->Integral("width")/(fNEvents+0.)*16./8.;
};


void ANL_CC1ppip_Evt_1DQ2_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 || event->NumFSParticle(211) == 0 || event->NumFSParticle(13) == 0) {
    return;
  }

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double q2CCpip = -1.0;

  // ANL has a M(pi, p) < 1.4 GeV cut imposed or no W cut
  if (hadMass < HadCut*1000.) {
    q2CCpip = -1*(Pnu-Pmu).Mag2()/1.E6;
  }

  fXVar = q2CCpip;

  return;
};

bool ANL_CC1ppip_Evt_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}

/*
void ANL_CC1ppip_Evt_1DQ2_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void ANL_CC1ppip_Evt_1DQ2_nu::ScaleEvents() {

  PlotUtils::FluxUnfoldedScaling(fMCHist, GetFluxHistogram());
  PlotUtils::FluxUnfoldedScaling(fMCFine, GetFluxHistogram());

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fScaleFactor);

  return;
}
*/
