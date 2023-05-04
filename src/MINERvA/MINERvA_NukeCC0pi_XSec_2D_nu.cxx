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
  For more info on sample see data/MINERvA/NukeCC0pi/README.txt
  or Phys. Rev. Lett. 130, 161801.
  Based on MINERvA_CC0pi_XSec_2D_nu.cxx
*/

#include "MINERvA_NukeCC0pi_XSec_2D_nu.h"
#include "MINERvA_SignalDef.h"

//********************************************************************
void MINERvA_NukeCC0pi_XSec_2D_nu::_SetupDataSettings(measurement measurement) {
  //********************************************************************

  // Define what files to use from the dist
  std::string datafile = "";
  std::string corrfile = "";
  std::string titles = "";
  std::string distdescript = "";
  std::string histname = "";

  datafile = "MINERvA/NukeCC0pi_2D/nuke_cc0pi_2d.root";
  corrfile = "MINERvA/NukeCC0pi_2D/nuke_cc0pi_2d.root";
  titles = "MINERvA CC0#pi #nu_{#mu} p_{T} p_{||};p_{||} (GeV);p_{T} "
           "(GeV);d^{2}#sigma/dP_{T}dP_{||} (cm^{2}/GeV^{2}/nucleon)";
           
  switch (measurement) {
    case ch: distdescript = "MINERvA_NukeCC0pi_CH_XSec_2D_nu sample"; 
      histname = "absolute_xsec_ch"; covar_name = "absolute_xsec_ch_covariance"; break;
    case c: distdescript = "MINERvA_NukeCC0pi_C_XSec_2D_nu sample"; 
      histname = "absolute_xsec_carbon"; covar_name = "absolute_xsec_carbon_covariance"; break;
    case h2o: distdescript = "MINERvA_NukeCC0pi_H2O_XSec_2D_nu sample"; 
      histname = "absolute_xsec_water"; covar_name = "absolute_xsec_water_covariance"; break;
    case fe: distdescript = "MINERvA_NukeCC0pi_Fe_XSec_2D_nu sample"; 
      histname = "absolute_xsec_iron"; covar_name = "absolute_xsec_iron_covariance"; break;
    case pb: distdescript = "MINERvA_NukeCC0pi_Pb_XSec_2D_nu sample"; 
      histname = "absolute_xsec_lead"; covar_name = "absolute_xsec_lead_covariance"; break;
    case ch_c_flux: distdescript = "MINERvA_NukeCC0pi_CH_C_Flux_XSec_2D_nu sample"; 
      histname = "absolute_xsec_ch_carbon_flux"; covar_name = "absolute_xsec_ch_carbon_flux_covariance"; break;
    case ch_h2o_flux: distdescript = "MINERvA_NukeCC0pi_CH_H2O_Flux_XSec_2D_nu sample"; 
      histname = "absolute_xsec_ch_water_flux"; covar_name = "absolute_xsec_ch_water_flux_covariance"; break;
    case ch_fe_flux: distdescript = "MINERvA_NukeCC0pi_CH_Fe_Flux_XSec_2D_nu sample"; 
      histname = "absolute_xsec_ch_iron_flux"; covar_name = "absolute_xsec_ch_iron_flux_covariance"; break;
    case ch_pb_flux: distdescript = "MINERvA_NukeCC0pi_CH_Pb_Flux_XSec_2D_nu sample"; 
      histname = "absolute_xsec_ch_lead_flux"; covar_name = "absolute_xsec_ch_lead_flux_covariance"; break;
  }

  fSettings.SetTitle(GeneralUtils::ParseToStr(titles, ";")[0]);
  fSettings.SetXTitle(GeneralUtils::ParseToStr(titles, ";")[1]);
  fSettings.SetYTitle(GeneralUtils::ParseToStr(titles, ";")[2]);
  fSettings.SetZTitle(GeneralUtils::ParseToStr(titles, ";")[3]);

  // Sample overview ---------------------------------------------------
  std::string descrip = distdescript + "\n"
                                       "Target: CH \n"
                                       "Flux: MINERvA Med Energy FHC numu  \n"
                                       "Signal: CC-0pi \n";
  fSettings.SetDescription(descrip);

  // The input ROOT file
  fSettings.SetDataInput(FitPar::GetDataBase() + datafile);
  fSettings.SetCovarInput(FitPar::GetDataBase() + corrfile);

  // Set the data file
  SetDataValues(fSettings.GetDataInput(), histname);
}

//********************************************************************
MINERvA_NukeCC0pi_XSec_2D_nu::MINERvA_NukeCC0pi_XSec_2D_nu(nuiskey samplekey) {
  //********************************************************************

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/FULL,DIAG/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");

  SetupDataSettings();
  FinaliseSampleSettings();

  fScaleFactor =
      (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) /
      this->TotalIntegratedFlux();

  TMatrixDSym *tempmat = StatUtils::GetCovarFromRootFile(
      fSettings.GetCovarInput(), covar_name);
  fFullCovar = tempmat;
  // Decomposition is stable for entries that aren't E-xxx
  double ScalingFactor = 1E38 * 1E38;
  (*fFullCovar) *= ScalingFactor;

  // Just check that the data error and covariance are the same
  for (int i = 0; i < fFullCovar->GetNrows(); ++i) {
    for (int j = 0; j < fFullCovar->GetNcols(); ++j) {
      // Get the global bin
      int xbin1, ybin1, zbin1;
      fDataHist->GetBinXYZ(i, xbin1, ybin1, zbin1);
      double xlo1 = fDataHist->GetXaxis()->GetBinLowEdge(xbin1);
      double xhi1 = fDataHist->GetXaxis()->GetBinLowEdge(xbin1 + 1);
      double ylo1 = fDataHist->GetYaxis()->GetBinLowEdge(ybin1);
      double yhi1 = fDataHist->GetYaxis()->GetBinLowEdge(ybin1 + 1);
      if (xlo1 < fDataHist->GetXaxis()->GetBinLowEdge(1) ||
          ylo1 < fDataHist->GetYaxis()->GetBinLowEdge(1) ||
          xhi1 > fDataHist->GetXaxis()->GetBinLowEdge(
                     fDataHist->GetXaxis()->GetNbins() + 1) ||
          yhi1 > fDataHist->GetYaxis()->GetBinLowEdge(
                     fDataHist->GetYaxis()->GetNbins() + 1))
        continue;
      double data_error = fDataHist->GetBinError(xbin1, ybin1);
      double cov_error = sqrt((*fFullCovar)(i, i) / ScalingFactor);
      if (fabs(data_error - cov_error) > 1E-5) {
        std::cerr << "Error on data is different to that of covariance"
                  << std::endl;
        NUIS_ERR(FTL, "Data error: " << data_error);
        NUIS_ERR(FTL, "Cov error: " << cov_error);
        NUIS_ERR(FTL, "Data/Cov: " << data_error / cov_error);
        NUIS_ERR(FTL, "Data-Cov: " << data_error - cov_error);
        NUIS_ERR(FTL, "For x: " << xlo1 << "-" << xhi1);
        NUIS_ABORT("For y: " << ylo1 << "-" << yhi1);
      }
    }
  }

  // Now can make the inverted covariance
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  // Now scale back
  (*fFullCovar) *= 1.0 / ScalingFactor;
  (*fDecomp) *= ScalingFactor;
  //Don't scale this back as GetLikelihood expects it to look like this
  // (*covar) *= ScalingFactor;

  fMapHist = new TH2I("MINERvA_NukeCC0pi_XSec_2D_nu_maphist", "",
                      fDataHist->GetNbinsX(), 0, fDataHist->GetNbinsX(),
                      fDataHist->GetNbinsY(), 0, fDataHist->GetNbinsY());

  int nbinsx = fDataHist->GetNbinsX();
  int nbinsy = fDataHist->GetNbinsY();
  Int_t Nbins = nbinsx * nbinsy;

  // Loop over the covariance matrix bins
  for (int i = 0; i < Nbins; ++i) {
    int xbin = (i % nbinsx) + 1;
    int ybin = (i / nbinsx) + 1;

    fMapHist->SetBinContent(xbin, ybin, i+1);
  }

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_NukeCC0pi_XSec_2D_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************
  // Checking to see if there is a Muon
  if (event->NumFSParticle(13) == 0)
    return;

  // Get the muon kinematics
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  Double_t px = Pmu.X() / 1000;
  Double_t py = Pmu.Y() / 1000;
  Double_t pt = sqrt(px * px + py * py);

  // y-axis is transverse momentum for both measurements
  fYVar = pt;

  // Don't want to assume the event generators all have neutrino coming along
  // z pz is muon momentum projected onto the neutrino direction
  Double_t pz = Pmu.Vect().Dot(Pnu.Vect() * (1.0 / Pnu.Vect().Mag())) / 1000.;
  // Set Hist Variables
  fXVar = pz;
};

//********************************************************************
bool MINERvA_NukeCC0pi_XSec_2D_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isNukeCC0pi_MINERvAPTPZ(event, EnuMin, EnuMax);
};

