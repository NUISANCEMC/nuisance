//Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
  Authors: Adrian Orea (v1 2017)
           Clarence Wret (v2 2018)
*/

#include "MINERvA_SignalDef.h"
#include "MINERvA_CC0pi_XSec_1D_2018_nu.h"

//********************************************************************
void MINERvA_CC0pi_XSec_1D_2018_nu::SetupDataSettings() {
//********************************************************************
  // Set Distribution
  // See header file for enum and some descriptions
  std::string name = fSettings.GetS("name");
  // Have two distributions as of summer 2018
  if (!name.compare("MINERvA_CC0pi_XSec_1Dpt_nu"))          fDist = kPt;
  else if (!name.compare("MINERvA_CC0pi_XSec_1Dpz_nu"))     fDist = kPz;
  else if (!name.compare("MINERvA_CC0pi_XSec_1DQ2QE_nu"))   fDist = kQ2QE;
  else if (!name.compare("MINERvA_CC0pi_XSec_1DEnuQE_nu"))  fDist = kEnuQE;

  // Define what files to use from the dist
  std::string datafile = "";
  std::string corrfile = "";
  std::string titles = "";
  std::string distdescript = "";
  std::string histname = "";

  switch (fDist) {
    case (kPt):
      datafile = "MINERvA/CC0pi_1D/FixedBinWidthPub/cov_ptmu_qelike.root";
      corrfile = "MINERvA/CC0pi_1D/FixedBinWidthPub/cov_ptmu_qelike.root";
      titles    = "MINERvA CC0#pi #nu_{#mu} p_{t};p_{t} (GeV);d#sigma/dP_{t} (cm^{2}/GeV/nucleon)";
      distdescript = "MINERvA_CC0pi_XSec_1Dpt_nu sample";
      histname = "ptmu_cross_section";
      break;
    case (kPz):
      datafile = "MINERvA/CC0pi_1D/FixedBinWidthPub/cov_pzmu_qelike.root";
      corrfile = "MINERvA/CC0pi_1D/FixedBinWidthPub/cov_pzmu_qelike.root";
      titles    = "MINERvA CC0#pi #nu_{#mu} p_{z};p_{z} (GeV);d#sigma/dP_{z} (cm^{2}/GeV/nucleon)";
      distdescript = "MINERvA_CC0pi_XSec_1Dpz_nu sample";
      histname = "pzmu_cross_section";
      break;
    case (kQ2QE):
      datafile = "MINERvA/CC0pi_1D/FixedBinWidthPub/cov_q2qe_qelike.root";
      corrfile = "MINERvA/CC0pi_1D/FixedBinWidthPub/cov_q2qe_qelike.root";
      titles    = "MINERvA CC0#pi #nu_{#mu} Q^{2}_{QE};Q^{2}_{QE} (GeV^{2});d#sigma/dQ^{2}_{QE} cm^{2}/GeV^{2}/nucleon)";
      distdescript = "MINERvA_CC0pi_XSec_1DQ2QE_nu sample";
      histname = "q2qe_cross_section";
      break;
    case (kEnuQE):
      datafile = "MINERvA/CC0pi_1D/FixedBinWidthPub/cov_enuqe_qelike.root";
      corrfile = "MINERvA/CC0pi_1D/FixedBinWidthPub/cov_enuqe_qelike.root";
      titles    = "MINERvA CC0#pi #nu_{#mu} E_{#nu}^{QE}; E_{#nu}^{QE} (GeV);d#sigma/dE_{#nu}^{QE} (cm^{2}/GeV/nucleon)";
      distdescript = "MINERvA_CC0pi_XSec_1DEnuQE_nu sample";
      histname = "enuqe_cross_section";
      break;
    default:
      THROW("Unknown Analysis Distribution : " << fDist);
  }

  fSettings.SetTitle(  GeneralUtils::ParseToStr(titles,";")[0] );
  fSettings.SetXTitle( GeneralUtils::ParseToStr(titles,";")[1] );
  fSettings.SetYTitle( GeneralUtils::ParseToStr(titles,";")[2] );

  // Sample overview ---------------------------------------------------
  std::string descrip = distdescript + "\n"\
                        "Target: CH \n" \
                        "Flux: MINERvA Low Energy FHC numu  \n" \
                        "Signal: CC-0pi \n";
  fSettings.SetDescription(descrip);

  // The input ROOT file
  fSettings.SetDataInput(  FitPar::GetDataBase() + datafile);
  fSettings.SetCovarInput( FitPar::GetDataBase() + corrfile);

  // Set the data file
  SetDataFromRootFile(fSettings.GetDataInput(), histname);
};

//********************************************************************
MINERvA_CC0pi_XSec_1D_2018_nu::MINERvA_CC0pi_XSec_1D_2018_nu(nuiskey samplekey) {
//********************************************************************

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/FULL,DIAG/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");

  SetupDataSettings();
  FinaliseSampleSettings();

  // If EnuQE distribution we apply the bin by bin flux integrated scaling (so don't divide scalefactor by integrated flux yet)
  if (fDist == kEnuQE) {
    fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.));
  } else {
    fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) / this->TotalIntegratedFlux();
  }

  TMatrixDSym* tempmat = StatUtils::GetCovarFromRootFile(fSettings.GetCovarInput(), "TotalCovariance");
  // Scale up up up!
  (*tempmat) *= 1E38*1E38;
  fFullCovar = tempmat;
  // Decomposition is stable for entries that aren't E-xxx
  double ScalingFactor = 1E38;
  (*fFullCovar) *= ScalingFactor;

  // Just check that the data error and covariance are the same
  for (int i = 0; i < fFullCovar->GetNrows(); ++i) {
    for (int j = 0; j < fFullCovar->GetNcols(); ++j) {
      // Get the global bin
      int xbin1, ybin1, zbin1;
      fDataHist->GetBinXYZ(i, xbin1, ybin1, zbin1);
      double xlo1 = fDataHist->GetXaxis()->GetBinLowEdge(xbin1);
      double xhi1 = fDataHist->GetXaxis()->GetBinLowEdge(xbin1+1);
      double ylo1 = fDataHist->GetYaxis()->GetBinLowEdge(ybin1);
      double yhi1 = fDataHist->GetYaxis()->GetBinLowEdge(ybin1+1);
      if (xlo1 < fDataHist->GetXaxis()->GetBinLowEdge(1) ||
          ylo1 < fDataHist->GetYaxis()->GetBinLowEdge(1) ||
          xhi1 > fDataHist->GetXaxis()->GetBinLowEdge(fDataHist->GetXaxis()->GetNbins()+1) ||
          yhi1 > fDataHist->GetYaxis()->GetBinLowEdge(fDataHist->GetYaxis()->GetNbins()+1)) continue;
      double data_error = fDataHist->GetBinError(xbin1, ybin1);
      double cov_error = sqrt((*fFullCovar)(i,i)/ScalingFactor);
      if (fabs(data_error - cov_error) > 1E-5) {
        std::cerr << "Error on data is different to that of covariance" << std::endl;
        ERR(FTL) << "Data error: " << data_error << std::endl;
        ERR(FTL) << "Cov error: " << cov_error << std::endl;
        ERR(FTL) << "Data/Cov: " << data_error/cov_error << std::endl;
        ERR(FTL) << "Data-Cov: " << data_error-cov_error << std::endl;
        ERR(FTL) << "For x: " << xlo1 << "-" << xhi1 << std::endl;
        ERR(FTL) << "For y: " << ylo1 << "-" << yhi1 << std::endl;
        throw;
      }
    }
  }

  // Now can make the inverted covariance
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  // Now scale back
  (*fFullCovar) *= 1.0/ScalingFactor;
  (*covar) *= ScalingFactor;
  (*fDecomp) *= ScalingFactor;

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_CC0pi_XSec_1D_2018_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************
  // Checking to see if there is a Muon
  if (event->NumFSParticle(13) == 0) return;

  // Get the muon kinematics
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  // Now we set the x-axis
  switch (fDist) {
    case (kPt):
      {
        Double_t px = Pmu.X()/1000.;
        Double_t py = Pmu.Y()/1000.;
        Double_t pt = sqrt(px*px+py*py);
        fXVar = pt;
        break;
      }
    case (kPz):
      {
        Double_t pz = Pmu.Vect().Dot(Pnu.Vect()*(1.0/Pnu.Vect().Mag()))/1000.;
        fXVar = pz;
        break;
      }
    case (kQ2QE):
      {
        Double_t q2qe = FitUtils::Q2QErec(Pmu, Pnu, 34.);
        fXVar = q2qe;
        break;
      }
    case (kEnuQE):
      {
        Double_t enuqe = FitUtils::EnuQErec(Pmu, cos(Pnu.Vect().Angle(Pmu.Vect())), 34.);
        fXVar = enuqe;
        break;
      }
    default:
      THROW("DIST NOT FOUND : " << fDist);
      break;
  }

  return;
};

//********************************************************************
bool MINERvA_CC0pi_XSec_1D_2018_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC0pi_MINERvAPTPZ(event, 14, EnuMin, EnuMax);
};
