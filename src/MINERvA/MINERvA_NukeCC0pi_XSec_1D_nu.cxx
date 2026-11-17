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
  Author: Jeffrey Kleykamp
  Please see https://inspirehep.net/literature/2902126
  Based on MINERvA_NukeCC0pi_XSec_2D_nu.cxx.
*/

#include "MINERvA_NukeCC0pi_XSec_1D_nu.h"
#include "MINERvA_SignalDef.h"
#include "TFile.h"

namespace {

constexpr double PROTON_MASS = 0.93827208;  // GeV
constexpr double MUON_MASS = 0.10565837;    // GeV
constexpr double NEUTRON_MASS = 0.93956541; // GeV

// Nuclear masses (GeV), computed from atomic mass minus atomic electron mass.
constexpr double MASS_C12 = 11.174863;
constexpr double MASS_O16 = 14.895080;
constexpr double MASS_FE56 = 52.076550;
constexpr double MASS_PB208 = 193.647388;

// Binding energy (GeV), standard GENIE removal-energy values per target.
constexpr double BE_C12 = 0.025;
constexpr double BE_O16 = 0.027;
constexpr double BE_FE56 = 0.036;
constexpr double BE_PB208 = 0.044;

double TargetMass(measurement_1d measurement) {
  switch (measurement) {
  case target1d_h2o:
  case target1d_ch_h2o_flux:
    return MASS_O16;
  case target1d_fe:
  case target1d_ch_fe_flux:
    return MASS_FE56;
  case target1d_pb:
  case target1d_ch_pb_flux:
    return MASS_PB208;
  default:
    return MASS_C12;
  }
}

double BindingEnergy(measurement_1d measurement) {
  switch (measurement) {
  case target1d_h2o:
  case target1d_ch_h2o_flux:
    return BE_O16;
  case target1d_fe:
  case target1d_ch_fe_flux:
    return BE_FE56;
  case target1d_pb:
  case target1d_ch_pb_flux:
    return BE_PB208;
  default:
    return BE_C12;
  }
}

void compute_stvs(const TVector3 &p3mu, const TVector3 &p3p,
                   const TVector3 &p3nu, double target_mass,
                   double binding_energy, double &delta_pT,
                   double &delta_phiT, double &delta_alphaT, double &delta_pL,
                   double &pn, double &delta_pTx, double &delta_pTy) {

  TVector3 zhat = p3nu.Unit();

  double pmu_L = p3mu.Dot(zhat);
  double pp_L = p3p.Dot(zhat);
  TVector3 pmuT = p3mu - pmu_L * zhat;
  TVector3 ppT = p3p - pp_L * zhat;
  TVector3 delta_pT_vec = pmuT + ppT;

  delta_pT = delta_pT_vec.Mag();

  delta_phiT = std::acos(-pmuT.Dot(ppT) / (pmuT.Mag() * ppT.Mag()));

  delta_alphaT = std::acos(-pmuT.Dot(delta_pT_vec) /
                            (pmuT.Mag() * delta_pT_vec.Mag()));

  double Emu = std::sqrt(MUON_MASS * MUON_MASS + p3mu.Mag2());
  double Ep = std::sqrt(PROTON_MASS * PROTON_MASS + p3p.Mag2());
  double R = target_mass + pmu_L + pp_L - Emu - Ep;
  double mf = target_mass - NEUTRON_MASS + binding_energy;
  delta_pL = 0.5 * R - (mf * mf + delta_pT * delta_pT) / (2. * R);

  pn = std::sqrt(delta_pL * delta_pL + delta_pT * delta_pT);

  TVector3 xTUnit = zhat.Cross(p3mu).Unit();
  TVector3 yTUnit = (-pmuT).Unit();
  delta_pTx = xTUnit.Dot(delta_pT_vec);
  delta_pTy = yTUnit.Dot(delta_pT_vec);
}

} // namespace

//********************************************************************
void MINERvA_NukeCC0pi_XSec_1D_nu::_SetupDataSettings(measurement_1d measurement,
                                                       kinematic_1d kinematic) {
  //********************************************************************

  fMeasurement = measurement;
  fKinematic = kinematic;

  std::string datafile = "MINERvA/NukeCC0pi_1D/tki_release.root";
  std::string corrfile = "MINERvA/NukeCC0pi_1D/tki_release.root";

  std::string kinname = "";
  std::string kintitle = "";
  std::string kinsymbol = "";
  std::string kinunit = "";

  switch (kinematic) {
  case kin1d_dpt:
  case kin1d_dpt_fine:
    kinname = "dpt";
    kintitle = "#deltap_{T} (GeV/c)";
    kinsymbol = "#deltap_{T}";
    kinunit = "GeV/c";
    break;
  case kin1d_dptx:
    kinname = "dptx";
    kintitle = "#deltap_{Tx} (GeV/c)";
    kinsymbol = "#deltap_{Tx}";
    kinunit = "GeV/c";
    break;
  case kin1d_dpty:
    kinname = "dpty";
    kintitle = "#deltap_{Ty} (GeV/c)";
    kinsymbol = "#deltap_{Ty}";
    kinunit = "GeV/c";
    break;
  case kin1d_pl:
    kinname = "pl";
    kintitle = "p_{L} (GeV/c)";
    kinsymbol = "p_{L}";
    kinunit = "GeV/c";
    break;
  case kin1d_pn:
    kinname = "pn";
    kintitle = "p_{n} (GeV/c)";
    kinsymbol = "p_{n}";
    kinunit = "GeV/c";
    break;
  case kin1d_alpha:
    kinname = "alpha";
    kintitle = "#delta#alpha_{T} (deg)";
    kinsymbol = "#delta#alpha_{T}";
    kinunit = "deg";
    break;
  case kin1d_phi:
    kinname = "phi";
    kintitle = "#delta#phi_{T} (deg)";
    kinsymbol = "#delta#phi_{T}";
    kinunit = "deg";
    break;
  case kin1d_muon_p:
    kinname = "muon_p";
    kintitle = "p_{#mu} (GeV/c)";
    kinsymbol = "p_{#mu}";
    kinunit = "GeV/c";
    break;
  case kin1d_muon_pt:
    kinname = "muon_pt";
    kintitle = "p_{T#mu} (GeV/c)";
    kinsymbol = "p_{T#mu}";
    kinunit = "GeV/c";
    break;
  case kin1d_muon_theta:
    kinname = "muon_theta";
    kintitle = "#theta_{#mu} (deg)";
    kinsymbol = "#theta_{#mu}";
    kinunit = "deg";
    break;
  case kin1d_proton_p:
    kinname = "proton_p";
    kintitle = "p_{p} (GeV/c)";
    kinsymbol = "p_{p}";
    kinunit = "GeV/c";
    break;
  case kin1d_proton_pt:
    kinname = "proton_pt";
    kintitle = "p_{Tp} (GeV/c)";
    kinsymbol = "p_{Tp}";
    kinunit = "GeV/c";
    break;
  case kin1d_proton_theta:
    kinname = "proton_theta";
    kintitle = "#theta_{p} (deg)";
    kinsymbol = "#theta_{p}";
    kinunit = "deg";
    break;
  }

  std::string distdescript = "";
  std::string histname = "";
  std::string target = "";
  std::string allowedtargets = "";

  switch (measurement) {
  case target1d_ch:
    distdescript = "MINERvA_NukeCC0pi_CH_XSec_1D_nu sample";
    histname = "absolute_xsec_" + kinname + "_ch";
    target = "CH";
    allowedtargets = "C,H";
    break;
  case target1d_carbon:
    distdescript = "MINERvA_NukeCC0pi_C_XSec_1D_nu sample";
    histname = "absolute_xsec_" + kinname + "_carbon";
    target = "C";
    allowedtargets = "C";
    break;
  case target1d_h2o:
    distdescript = "MINERvA_NukeCC0pi_H2O_XSec_1D_nu sample";
    histname = "absolute_xsec_" + kinname + "_water";
    target = "H2O";
    allowedtargets = "O,H";
    break;
  case target1d_fe:
    distdescript = "MINERvA_NukeCC0pi_Fe_XSec_1D_nu sample";
    histname = "absolute_xsec_" + kinname + "_iron";
    target = "Fe";
    allowedtargets = "Fe";
    break;
  case target1d_pb:
    distdescript = "MINERvA_NukeCC0pi_Pb_XSec_1D_nu sample";
    histname = "absolute_xsec_" + kinname + "_lead";
    target = "Pb";
    allowedtargets = "Pb";
    break;
  case target1d_ch_c_flux:
    distdescript = "MINERvA_NukeCC0pi_CH_C_Flux_XSec_1D_nu sample";
    histname = "absolute_xsec_" + kinname + "_ch_carbon_flux";
    target = "CH";
    allowedtargets = "C,H";
    break;
  case target1d_ch_h2o_flux:
    distdescript = "MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_1D_nu sample";
    histname = "absolute_xsec_" + kinname + "_ch_water_flux";
    target = "CH";
    allowedtargets = "C,H";
    break;
  case target1d_ch_fe_flux:
    distdescript = "MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_1D_nu sample";
    histname = "absolute_xsec_" + kinname + "_ch_iron_flux";
    target = "CH";
    allowedtargets = "C,H";
    break;
  case target1d_ch_pb_flux:
    distdescript = "MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_1D_nu sample";
    histname = "absolute_xsec_" + kinname + "_ch_lead_flux";
    target = "CH";
    allowedtargets = "C,H";
    break;
  }

  covar_name = histname + "_covariance";

  fSettings.SetTitle("MINERvA CC0#pi #nu_{#mu} " + kinname);
  fSettings.SetXTitle(kintitle);
  fSettings.SetYTitle("d#sigma/d" + kinsymbol + " (cm^{2}/" + kinunit +
                       "/nucleon)");

  std::string descrip = distdescript +
                         "\n"
                         "Target: " +
                         target +
                         " \n"
                         "Flux: MINERvA Med Energy FHC numu  \n"
                         "Signal: CC-0pi-Np \n";
  fSettings.SetDescription(descrip);

  fSettings.SetDataInput(FitPar::GetDataBase() + datafile);
  fSettings.SetCovarInput(FitPar::GetDataBase() + corrfile);
  fSettings.DefineAllowedTargets(allowedtargets);

  TFile *datarootfile = TFile::Open(fSettings.GetDataInput().c_str(), "READ");
  TH1D *datahist = (TH1D *)datarootfile->Get(histname.c_str());
  if (!datahist)
    NUIS_ABORT("Could not find histogram " << histname << " in "
                                            << fSettings.GetDataInput());
  fDataHist = (TH1D *)datahist->Clone((fSettings.GetName() + "_data").c_str());
  fDataHist->SetDirectory(0);
  datarootfile->Close();
  delete datarootfile;
}

//********************************************************************
MINERvA_NukeCC0pi_XSec_1D_nu::MINERvA_NukeCC0pi_XSec_1D_nu(
    nuiskey samplekey, measurement_1d measurement, kinematic_1d kinematic) {
  //********************************************************************

  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/FULL,DIAG/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedSpecies("numu");

  _SetupDataSettings(measurement, kinematic);
  FinaliseSampleSettings();

  fScaleFactor =
      (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) /
      this->TotalIntegratedFlux();

  TMatrixDSym *tempmat = StatUtils::GetCovarFromRootFile(
      fSettings.GetCovarInput(), covar_name);

  int nbins = fDataHist->GetNbinsX();
  if (tempmat->GetNrows() == nbins + 2) {
    TMatrixDSym *trimmed = new TMatrixDSym(nbins);
    for (int i = 0; i < nbins; ++i)
      for (int j = 0; j < nbins; ++j)
        (*trimmed)(i, j) = (*tempmat)(i + 1, j + 1);
    delete tempmat;
    tempmat = trimmed;
  }

  fFullCovar = tempmat;
  double ScalingFactor = 1E38 * 1E38;
  (*fFullCovar) *= ScalingFactor;

  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  (*fDecomp) *= ScalingFactor;
  // fFullCovar is left at the scaled magnitude: this covariance file's
  // entries are stored 1E76 below the data histogram's variance scale,
  // so scaling back down (as in the 2D file) reintroduces the mismatch.

  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_NukeCC0pi_XSec_1D_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  if (event->NumFSParticle(13) == 0)
    return;
  if (event->NumFSParticle(2212) == 0)
    return;

  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Pp = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  TVector3 p3mu = Pmu.Vect() * (1.0 / 1000.);
  TVector3 p3p = Pp.Vect() * (1.0 / 1000.);
  TVector3 p3nu = Pnu.Vect();

  double delta_pT, delta_phiT, delta_alphaT, delta_pL, pn_val;
  double delta_pTx, delta_pTy;
  compute_stvs(p3mu, p3p, p3nu, TargetMass(fMeasurement),
               BindingEnergy(fMeasurement), delta_pT, delta_phiT,
               delta_alphaT, delta_pL, pn_val, delta_pTx, delta_pTy);

  switch (fKinematic) {
  case kin1d_dpt:
  case kin1d_dpt_fine:
    fXVar = delta_pT;
    break;
  case kin1d_dptx:
    fXVar = delta_pTx;
    break;
  case kin1d_dpty:
    fXVar = delta_pTy;
    break;
  case kin1d_pl:
    fXVar = delta_pL;
    break;
  case kin1d_pn:
    fXVar = pn_val;
    break;
  case kin1d_alpha:
    fXVar = delta_alphaT * 180. / M_PI;
    break;
  case kin1d_phi:
    fXVar = delta_phiT * 180. / M_PI;
    break;
  case kin1d_muon_p:
    fXVar = p3mu.Mag();
    break;
  case kin1d_muon_pt:
    fXVar = (p3mu - p3mu.Dot(p3nu.Unit()) * p3nu.Unit()).Mag();
    break;
  case kin1d_muon_theta:
    fXVar = p3mu.Angle(p3nu) * 180. / M_PI;
    break;
  case kin1d_proton_p:
    fXVar = p3p.Mag();
    break;
  case kin1d_proton_pt:
    fXVar = (p3p - p3p.Dot(p3nu.Unit()) * p3nu.Unit()).Mag();
    break;
  case kin1d_proton_theta:
    fXVar = p3p.Angle(p3nu) * 180. / M_PI;
    break;
  }
};

//********************************************************************
bool MINERvA_NukeCC0pi_XSec_1D_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isNukeCC0piNp_MINERvA_STV(event, EnuMin, EnuMax);
};
