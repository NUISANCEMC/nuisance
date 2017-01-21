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
 * Kitagaki et al. Phys Rev D, Volume 34, Number 9, 1 November 1986, p2254-2565
 * K. Furuno et al. NuInt02 proceedings, (supposedly published in Nucl. Phys. B but I never found it), Retreieved from KEK preprints. 
 * KEK Preprint 2003-48, RCNS-03-01, September 2003
*/

#include "BNL_CC1ppip_Evt_1DQ2_nu.h"

// The constructor
BNL_CC1ppip_Evt_1DQ2_nu::BNL_CC1ppip_Evt_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {

  fName = "BNL_CC1ppip_Evt_1DQ2_nu";
  fPlotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); Number of events";
  EnuMin = 0.5;
  EnuMax = 6.;
  fIsDiag = true;
  fIsRawEvents = true;
  fDefaultTypes="EVT/SHAPE/DIAG";
  fAllowedTypes = "EVT/SHAPE/DIAG/W14/NOW";

  // Look if user has specified a W cut
  if (type.find("W14") != std::string::npos) {
    fName += "_W14";
    HadCut = 1.4;
  } else {
    fName += "_noW";
    HadCut = 10.0;
    EnuMin = 0.0;
    EnuMax = 10.0;
  }

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);


  std::string DataLocation = GeneralUtils::GetTopLevelDir()+"/data/BNL/CC1pip_on_p/";

  // If W < 1.4 GeV cut
  if (HadCut == 1.4) {
    DataLocation += "BNL_CC1pip_on_p_noEvents_q2_w14_enu05to6_finebin_firstQ2gone.txt";
  // If W < 2.0 GeV
  } else {
    // No Enu cuts on full W space
    DataLocation += "BNL_CC1pip_on_p_noEvents_q2_noWcut_bin.txt";
  }

  SetDataValues(DataLocation);
  SetupDefaultHist();

  // Set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = GetEventHistogram()->Integral("width")/(fNEvents+0.)*16./8.;
};


void BNL_CC1ppip_Evt_1DQ2_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 || event->NumFSParticle(211) == 0 || event->NumFSParticle(13) == 0) {
    return;
  }

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double q2CCpip = -1.0;

  // BNL has a M(pi, p) < 1.4 GeV cut and no W cut. 
  // This should be specified by user in "type" field
  // Reverts to 10 GeV (essentially no W cut!)
  if (hadMass < HadCut*1000.) {
    q2CCpip = -1*(Pnu-Pmu).Mag2()/1.E6;
  }

  fXVar = q2CCpip;

  return;
};


bool BNL_CC1ppip_Evt_1DQ2_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212,EnuMin,EnuMax);
}

/*
void BNL_CC1ppip_Evt_1DQ2_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void BNL_CC1ppip_Evt_1DQ2_nu::ScaleEvents() {

  PlotUtils::FluxUnfoldedScaling(fMCHist, GetFluxHistogram());
  PlotUtils::FluxUnfoldedScaling(fMCFine, GetFluxHistogram());

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fScaleFactor);

  return;
}
*/
