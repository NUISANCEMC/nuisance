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

/*
  Author: Anežka Klustová @anezkaklust (a.klustova20@imperial.ac.uk or klustova.a@gmail.com)
  MINERvA antineutrino CC inclusive on nuclear targets (RHC flux)
  Signal: numubar CC inclusive, 1 mu+, 2 < Emu < 20 GeV, thmu < 17 deg
  arXiv:2604.07091
  PRL DOI: https://doi.org/10.1103/n7s9-8btb
  Zenodo DOI: https://doi.org/10.5281/zenodo.24306934
*/

#include "MINERvA_SignalDef.h"
#include "MINERvA_NukeCCinc_XSec_1Dptmu_antinu.h"

//********************************************************************
void MINERvA_NukeCCinc_XSec_1Dptmu_antinu::SetupDataSettings(){
//********************************************************************

  std::string name = fSettings.GetS("name");

  // Have CH, C, Fe, Pb targets, plus tracker CH daisy-reweighted to the
  // C, Fe and Pb target fluxes (CH_<tgt>_Flux).
  // Each sample's flux is a histogram in
  //   data/flux/MINERvA_ME_AntiNuMu_Flux_Nuclear_Targets_Constrained.root
  // Input events must be generated with that histogram.
  struct TargetInfo { std::string name; target tgt; std::string fluxhist; };
  const std::vector<TargetInfo> targets = {
    {"CH_C_Flux",  kCH_C_Flux,  "flux_carbon"},
    {"CH_Fe_Flux", kCH_Fe_Flux, "flux_iron"},
    {"CH_Pb_Flux", kCH_Pb_Flux, "flux_lead"},
    {"CH",         kCH,         "flux_hydrocarbon"},
    {"C",          kC,          "flux_carbon"},
    {"Fe",         kFe,         "flux_iron"},
    {"Pb",         kPb,         "flux_lead"},
  };
  std::string target_str = "";
  std::string flux_hist = "";
  for (size_t i = 0; i < targets.size(); ++i) {
    if (name.find("MINERvA_NukeCCinc_" + targets[i].name + "_XSec_1Dptmu_antinu") != std::string::npos) {
      fTarget = targets[i].tgt;
      target_str = targets[i].name;
      flux_hist = targets[i].fluxhist;
      break;
    }
  }
  if (target_str.empty()) {
    NUIS_ABORT("Could not find appropriate target for " << name);
  }
  const std::string flux_file =
      "data/flux/MINERvA_ME_AntiNuMu_Flux_Nuclear_Targets_Constrained.root";

  std::string distribution_str = "";
  std::string titles = "";
  if (name.find("_XSec_1Dptmu_antinu") != std::string::npos) {
    fDistribution = kPtmu;
    distribution_str = "ptmu";
    titles = "p_{#mu,T} (GeV);d#sigma/dp_{#mu,T} (cm^{2}/nucleon/GeV)";
  }
  else {
    NUIS_ABORT("Could not find appropriate distribution for " << name);
  }

  std::string datafile = "NukeCCinc_" + target_str + "_" + distribution_str + "_antinu";

  std::string descrip = "Target: " + target_str + "\n" +
                        "Flux: MINERvA ME Reverse Horn Current numubar ONLY, " +
                        flux_file + " : " + flux_hist + "\n" +
                        "Signal: numubar CC inclusive, 1 mu+, 2 < Emu < 20 GeV, thmu < 17 deg";
  fSettings.SetDescription(descrip);

  fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/NukeCCinc_antinu/" + datafile + ".txt" );
  fSettings.SetCovarInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/NukeCCinc_antinu/" + datafile + "_cov_total.txt" );
  fSettings.SetXTitle(GeneralUtils::ParseToStr(titles,";")[0]);
  fSettings.SetYTitle(GeneralUtils::ParseToStr(titles,";")[1]);
}

//********************************************************************
MINERvA_NukeCCinc_XSec_1Dptmu_antinu::MINERvA_NukeCCinc_XSec_1Dptmu_antinu(nuiskey samplekey) {
//********************************************************************

  // Define Sample Settings common to all data distributions
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  // Flux integral and signal over 0-100 GeV (no Enu cut in the measurement;
  // the 2 < Emu < 20 GeV cut is in the signal definition)
  fSettings.SetEnuRange(0., 100.0);
  fSettings.DefineAllowedTargets("C,H,Fe,Pb");
  // Flux-averaged xsec: the flux comes from the input file, which must be
  // generated with this sample's histogram from
  // data/flux/MINERvA_ME_AntiNuMu_Flux_Nuclear_Targets_Constrained.root
  // (see SetupDataSettings)
  fSettings.DefineAllowedSpecies("numub");
  SetupDataSettings();

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromTextFile( fSettings.GetCovarInput() );

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_NukeCCinc_XSec_1Dptmu_antinu::FillEventVariables(FitEvent *event) {
//********************************************************************

  fXVar = -999.9;
  if (event->NumISParticle(-14) == 0 || event->NumFSParticle(-13) == 0) {
    return;
  }

  TLorentzVector Pnu = event->GetHMISParticle(-14)->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(-13)->fP;

  // Pz is the neutrino-direction component of muon 3-momentum
  Double_t pz_mu = Pmu.Vect().Dot( Pnu.Vect().Unit() ); // MeV
  TVector3 pt_mu = Pmu.Vect() - pz_mu * Pnu.Vect().Unit(); // MeV

  switch (fDistribution){
    case kPtmu: fXVar = pt_mu.Mag()/1000.; break;
    default: NUIS_ABORT("DIST NOT FOUND : " << fDistribution);
  }
};

//********************************************************************
bool MINERvA_NukeCCinc_XSec_1Dptmu_antinu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isNukeCCinc_antinu_MINERvA(event, EnuMin, EnuMax);
}
