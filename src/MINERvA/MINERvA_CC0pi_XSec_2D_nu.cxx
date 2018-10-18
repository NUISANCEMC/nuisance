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
#include "MINERvA_CC0pi_XSec_2D_nu.h"

//********************************************************************
void MINERvA_CC0pi_XSec_2D_nu::SetupDataSettings() {
//********************************************************************
  // Set Distribution
  // See header file for enum and some descriptions
  std::string name = fSettings.GetS("name");
  // Have two distributions as of summer 2018
  if      (!name.compare("MINERvA_CC0pi_XSec_2Dptpz_nu"))   fDist = kPtPz;
  else if (!name.compare("MINERvA_CC0pi_XSec_2DptQ2_nu"))   fDist = kPtQ2;

  // Define what files to use from the dist
  std::string datafile = "";
  std::string corrfile = "";
  std::string titles = "";
  std::string distdescript = "";
  std::string histname = "";

  switch (fDist) {
    case (kPtPz):
      //datafile = "MINERvA/CC0pi_2D/MINERvA_LE_CCQELike_pzmu.root";
      //corrfile = "MINERvA/CC0pi_2D/MINERvA_LE_CCQELike_pzmu.root";
      datafile = "MINERvA/CC0pi_2D/cov_ptpl_2D_qelike.root";
      corrfile = "MINERvA/CC0pi_2D/cov_ptpl_2D_qelike.root";
      titles    = "MINERvA CC0#pi #nu_{#mu} p_{t} p_{z};p_{z} (GeV);p_{t} (GeV);d^{2}#sigma/dP_{t}dP_{z} (cm^{2}/GeV^{2}/nucleon)";
      distdescript = "MINERvA_CC0pi_XSec_2Dptpz_nu sample";
      //histname = "h_pzmu_ptmu_data_nobck_unfold_effcor_cross_section_CV_WithErr";
      histname = "pt_pl_cross_section";
      break;
    case (kPtQ2):
      datafile = "MINERvA/CC0pi_2D/pt_q2_data.root";
      corrfile = "MINERvA/CC0pi_2D/pt_q2_cov.root";
      titles    = "MINERvA CC0#pi #nu_{#mu} p_{t} Q^{2}_{QE};p_t{z} (GeV);Q^{2}_{QE} (GeV^{2});d^{2}#sigma/dP_{t}dQ^{2}_{QE} (cm^{2}/GeV^{3}/nucleon)";
      distdescript = "MINERvA_CC0pi_XSec_2DptQ2_nu sample";
      histname = "h_q2_ptmu_data_nobck_unfold_effcor_cross_section_CV_WithErr";
      break;
    default:
      THROW("Unknown Analysis Distribution : " << fDist);
  }

  fSettings.SetTitle(  GeneralUtils::ParseToStr(titles,";")[0] );
  fSettings.SetXTitle( GeneralUtils::ParseToStr(titles,";")[1] );
  fSettings.SetYTitle( GeneralUtils::ParseToStr(titles,";")[2] );
  fSettings.SetYTitle( GeneralUtils::ParseToStr(titles,";")[3] );

  // Sample overview ---------------------------------------------------
  std::string descrip = distdescript + "\n"\
                        "Target: CH \n" \
                        "Flux: MINERvA Low Energy FHC numu  \n" \
                        "Signal: CC-0pi \n";
  fSettings.SetDescription(descrip);

  // The input ROOT file
  fSettings.SetDataInput(  FitPar::GetDataBase() + datafile);
  // The covariance matrix ROOT file
  //if (fDist == kPtPz) {
  fSettings.SetCovarInput( FitPar::GetDataBase() + corrfile);
  //} else {
    //ERR(WRN) << " no covariance matrix available for PtQ2" << std::endl;
    //ERR(WRN) << " ask Dan Ruterbories nicely and he may provide one" << std::endl;
  //}

  // Set the data file
  SetDataValues(fSettings.GetDataInput(), histname);
}
//********************************************************************
MINERvA_CC0pi_XSec_2D_nu::MINERvA_CC0pi_XSec_2D_nu(nuiskey samplekey) {
//********************************************************************

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/FULL,DIAG/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");

  SetupDataSettings();
  FinaliseSampleSettings();

  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) / this->TotalIntegratedFlux();

  // Data is __NOT__ bin width normalised, so override the default
  //fIsNoWidth = true;

  // Set the mapping values and the covariance matrix files
  //SetMapValuesFromText( fSettings.GetMapInput() );
  // Also have to make our own covariance matrix to exclude the under and overflow
  //if (fDist == kPtPz) {
  //TMatrixDSym* tempmat = StatUtils::GetCovarFromRootFile(fSettings.GetCovarInput(), "TMatrixDBase");
  //TMatrixDSym* tempmat = StatUtils::GetCovarFromRootFile(fSettings.GetCovarInput(), "CovMatrix");
  TMatrixDSym* tempmat = StatUtils::GetCovarFromRootFile(fSettings.GetCovarInput(), "TotalCovariance");
  fFullCovar = tempmat;
  // Decomposition is stable for entries that aren't E-xxx
  double ScalingFactor = 1E38*1E38;
  (*fFullCovar) *= ScalingFactor;

  //} else {
    //SetCovarFromDiagonal();
  //}

  /*
  // Now we cut out every first and last to exclude under and overflow
  fFullCovar = new TMatrixDSym(fDataHist->GetXaxis()->GetNbins()*fDataHist->GetYaxis()->GetNbins());

  // Count the real covariance matrix x and y
  int xcounter = 0;

  int xbins = fDataHist->GetXaxis()->GetNbins();
  int ybins = fDataHist->GetYaxis()->GetNbins();
  // Loop over the x bins (underflow adds one, overflow adds one)
  for (int i = 0; i < (xbins+2)*(ybins+2); ++i) {
    // Skip the under and overflow
    if (i < (ybins+2) || i % (ybins+1) == 0 || ((i+1)%(ybins+1) == 0) || i > (ybins+2)*(xbins+1)) {
      //std::cout << "Skipping ibin " << i << std::endl;
      continue;
    }

    // The ycounter
    int ycounter = 0;
    // For one bin of pT we have pZ^2 bins
    for (int j = 0; j < (xbins+2)*(ybins+2); ++j) {

      // Skip the under and overflow
      if (j < (ybins+2) || j % (ybins+1) == 0 || ((j+1)%(ybins+1) == 0) || j > (ybins+2)*(xbins+1)) {
        //std::cout << "Skipping jbin " << j << std::endl;
        continue;
      }

      (*fFullCovar)(xcounter, ycounter) = (*tempmat)(i, j);
      //std::cout << xcounter << ", " << ycounter << " === " << i << ", " << j << std::endl;
      ycounter++;
    }
    // Check dimensions
    if (ycounter != xbins*ybins) {
      std::cerr << "Counted " << ycounter << " y bins in cov matrix" << std::endl;
      std::cerr << "Whereas there should be " << xbins*ybins << std::endl;
    }
    xcounter++;
  }
  // Check dimensions
  if (xcounter != xbins*ybins) {
    std::cerr << "Counted " << xcounter << " x bins in cov matrix" << std::endl;
    std::cerr << "Whereas there should be " << xbins*ybins << std::endl;
  }
  // Delete the temporary
  delete tempmat;
  */

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

  // Use a TH2D version of the covariance to be able to use the global bin numbering scheme
  /*
  covar_th2d = new TH2D((fSettings.Title()+"_th2").c_str(), (fSettings.Title()+"_th2").c_str(), covar->GetNrows(), 0, covar->GetNrows(), covar->GetNcols(), 0, covar->GetNcols());
  for (int i = 0; i < covar_th2d->GetXaxis()->GetNbins(); ++i) {
    for (int j = 0; j < covar_th2d->GetYaxis()->GetNbins(); ++j) {
      covar_th2d->SetBinContent(i+1, j+1, (*covar)(i,j));
    }
  }
  */
  //std::cout << "covar is " << covar_th2d->GetXaxis()->GetNbins() << " x " << covar_th2d->GetYaxis()->GetNbins() << " = " << covar_th2d->GetXaxis()->GetNbins()*covar_th2d->GetYaxis()->GetNbins() << std::endl;
  //std::cout << "data is " << fDataHist->GetXaxis()->GetNbins() << " x " << fDataHist->GetYaxis()->GetNbins() << " = " << fDataHist->GetXaxis()->GetNbins()*fDataHist->GetYaxis()->GetNbins() << std::endl;

  // Let's make our own mapping histogram
  // The covariance matrix is dominant in Pt and sub-dominant in Pz and includes all bins with under and overflow
  // So we have 13x12 data/MC bins, and including the under/overflow we have 15x14=210 covariance bins
  // i.e. need to cut out the first and last bins of covariance matrix
  // Mapping histogram will have same dimensions as the data
  /*
     fMapHist = (TH2I*)(fDataHist->Clone());
     fMapHist->Reset();
     std::string MapTitle = std::string(fDataHist->GetName())+"_MAP";
     fMapHist->SetNameTitle(MapTitle.c_str(), MapTitle.c_str());
     int counter = 1;
     for (int i = 0; i <= fDataHist->GetXaxis()->GetNbins()+1; ++i) {
     for (int j = 0; j <= fDataHist->GetYaxis()->GetNbins()+1; ++j) {
     if (i == 0 || i == fDataHist->GetXaxis()->GetNbins()+1 || j == 0 || j == fDataHist->GetYaxis()->GetNbins()+1) {
     fMapHist->SetBinContent(i+1, j+1, 0);
     } else {
     fMapHist->SetBinContent(i+1, j+1, counter);
     counter++;
     }
     std::cout << fMapHist->GetBinContent(i+1, j+1) << "    " << fDataHist->GetBinContent(i+1, j+1) << std::endl;
     }
     std::cout << std::endl;
     }
     */

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_CC0pi_XSec_2D_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************
  // Checking to see if there is a Muon
  if (event->NumFSParticle(13) == 0) return;

  // Get the muon kinematics
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  Double_t px = Pmu.X()/1000;
  Double_t py = Pmu.Y()/1000;
  Double_t pt = sqrt(px*px+py*py);

  // y-axis is transverse momentum for both measurements
  fYVar = pt;

  // Now we set the x-axis
  switch (fDist) {
    case kPtPz:
      {
        // Don't want to assume the event generators all have neutrino coming along z
        // pz is muon momentum projected onto the neutrino direction
        Double_t pz = Pmu.Vect().Dot(Pnu.Vect()*(1.0/Pnu.Vect().Mag()))/1000.;
        // Set Hist Variables
        fXVar = pz;
        break;
      }
    case kPtQ2:
      {
        // 34 MeV binding energy and neutrino mode (true)
        double q2qe = FitUtils::Q2QErec(Pmu, Pnu, 34.0, true);
        fXVar = q2qe;
        break;
      }
    default:
      THROW("DIST NOT FOUND : " << fDist);
      break;
  }

};

//********************************************************************
bool MINERvA_CC0pi_XSec_2D_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC0pi_MINERvAPTPZ(event, 14, EnuMin, EnuMax);
};

//********************************************************************
// Custom likelihood calculator because binning of covariance matrix
double MINERvA_CC0pi_XSec_2D_nu::GetLikelihood() {
  //********************************************************************

  // The calculated chi2
  double chi2 = 0.0;

  // STRICTLY DEBUGGING WITH DAN
  /*
     std::vector<std::string> Names;
  //Names.push_back(std::string(std::getenv("NUISANCE"))+"/build/app/DanMC/Genie_MC.root");
  //Names.push_back(std::string(std::getenv("NUISANCE"))+"/build/app/DanMC/Genie2p2hrpa_MC.root");
  //Names.push_back(std::string(std::getenv("NUISANCE"))+"/build/app/DanMC/MnvGENIE_MC.root");
  Names.push_back(std::string(std::getenv("NUISANCE"))+"/data/MINERvA/CC0pi_2D/MINERvA_LE_CCQELike_pzmu.root");

  // Hack hack hack hack
  double scaleF = 0.0;

  for (size_t a = 0; a < Names.size(); ++a) {
  std::cout << Names[a] << std::endl;
  //TFile *file = new TFile((std::string(std::getenv("NUISANCE"))+"/build/app/DanMC/Genie_MC.root").c_str(), "OPEN");
  //TFile *file = new TFile((std::string(std::getenv("NUISANCE"))+"/build/app/DanMC/Genie2p2hrpa_MC.root").c_str(), "OPEN");
  //TFile *file = new TFile((std::string(std::getenv("NUISANCE"))+"/build/app/DanMC/MnvGENIE_MC.root").c_str(), "OPEN");
  TFile *file = new TFile(Names[a].c_str(), "OPEN");
  //TH2D *mc = (TH2D*)(file->Get("h_pzmu_ptmu_mc_nobck_unfold_effcor_cross_section_CV_WithStatErr")->Clone());
  TH2D *mc = (TH2D*)(file->Get("h_pzmu_ptmu_mc_nobck_unfold_effcor_cross_section_CV_WithErr")->Clone());
  fMCHist = mc;
  //fMCHist->Scale(1., "width");
  //fDataHist->Scale(1., "width");
  */

  // Support shape comparisons
  double scaleF = fDataHist->Integral() / fMCHist->Integral();
  if (fIsShape) {
    fMCHist->Scale(scaleF);
    fMCFine->Scale(scaleF);
    //PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG, scaleF);
  }

  // Even though this chi2 calculation looks ugly it is _EXACTLY_ what MINERvA used for their measurement
  // Can be prettified in due time but for now keep
  bool chi2_use_overflow_err = false;
  //const int lowBin  = chi2_use_overflow_err?0:1; // Either they both use underflow, or neither of them does

  int nbinsx=fMCHist->GetNbinsX();
  int nbinsy=fMCHist->GetNbinsY();
  //const int highBinX = nbinsx;
  //const int highBinY = nbinsy;
  Int_t Nbins = nbinsx*nbinsy;

  /*
  TMatrixD covMatrixTmp = (*fFullCovar);

  TMatrixD covMatrix(Nbins, Nbins);
  const double scaling = 1e80;//ROOT can't seem to handle small entries with lots of zeros? Suggested scaling the histogram and then rescaling the inverted matrix
  for( int i = 0; i != Nbins; ++i ) {
    for( int j = 0; j != Nbins; ++j ) {
      // I think this is right... it checks out on a small sample
      int old_i_bin = (i/nbinsx + 1)* (nbinsx +2) +1 + i%nbinsx;
      int old_j_bin = (j/nbinsx + 1)* (nbinsx +2) +1 + j%nbinsx;
      covMatrix[i][j] = covMatrixTmp[old_i_bin][old_j_bin]*scaling;
      //std::cout << Nbins*Nbins << " (" << Nbins << "*" << Nbins << ")" << std::endl;
      //std::cout << i << ", " << j << " = " << old_i_bin << " " << old_j_bin << std::endl;
    }
    //throw;
  }
  TDecompSVD error(covMatrix);
  TMatrixD errorMatrix(covMatrix);
  if( ! error.Invert( errorMatrix ) ) {
    std::cout << "Cannot invert total covariance matrix. You could use statistical errors only for Chi2 calculation. But it isn't implemented yet" << std::endl;
  }

  covMatrix *= 1/scaling;
  errorMatrix *= scaling;
  */

  /*
  for( int i = 0; i != Nbins; ++i ) {
    //int hist_i_bin = chi2_use_overflow_err?i:((i/nbinsx + 1)* (nbinsx +2) +1 + i%nbinsx); // Translate to the histogram bin, if we aren't using the overflow errors, meaning the covariance matrix is smaller than the histogram
    //int hist_i_bin = ((i/nbinsx + 1)* (nbinsx) +1 + i%nbinsx); // Translate to the histogram bin, if we aren't using the overflow errors, meaning the covariance matrix is smaller than the histogram
    //int hist_i_bin = ((i/nbinsx)*(nbinsx) + i%nbinsx); // Translate to the histogram bin, if we aren't using the overflow errors, meaning the covariance matrix is smaller than the histogram
    int hist_i_bin = ((i/nbinsx)*nbinsy) + i%nbinsx + 1;
    const Double_t x_data_i = fDataHist->GetBinContent(hist_i_bin);
    const Double_t x_mc_i   = fMCHist->GetBinContent(hist_i_bin);
    if (i > 1) throw;
    //const Double_t x_data_i = fDataHist->GetBinContent(i);
    //const Double_t x_mc_i   = fMCHist->GetBinContent(i);
    for( int j = 0; j != Nbins; ++j ) {
      // Each element of the inverted covariance matrix corresponds to a pair of data and MC
      //int hist_j_bin = chi2_use_overflow_err?j:((j/nbinsx + 1)* (nbinsx +2) +1 + j%nbinsx);
      //int hist_j_bin = ((j/nbinsy)*(nbinsx) + j%nbinsx);
      int hist_j_bin = (j%nbinsx)+1;

      const Double_t x_data_j = fDataHist->GetBinContent(hist_j_bin);
      const Double_t x_mc_j   = fMCHist->GetBinContent(hist_j_bin);
      //const Double_t x_data_j = fDataHist->GetBinContent(j);
      //const Double_t x_mc_j   = fMCHist->GetBinContent(j);
      //const double chi2_ij = (x_data_i - x_mc_i) * errorMatrix[i][j] * (x_data_j - x_mc_j);
      const double chi2_ij = (x_data_i - x_mc_i) * (*covar)[i][j] * (x_data_j - x_mc_j);
      //std::cout << x_data_i << "\t" << x_mc_i << "\t" << i << "\t" << errorMatrix[i][j] << "\t" << j << "\t" << x_data_j << "\t" << x_mc_j << "\t" << chi2_ij << std::endl;
      std::cout << "hist i = " << hist_i_bin << " hist j = " << hist_j_bin << std::endl;
      std::cout << "i = " << i << " j = " << j << std::endl;
      std::cout << chi2 << " += " << chi2_ij << " for " << i << " " << j << std::endl;
      std::cout << fDataHist->GetXaxis()->GetBinLowEdge(hist_i_bin+1) << " " << fDataHist->GetXaxis()->GetBinLowEdge(hist_i_bin+2) << std::endl;
      std::cout << fDataHist->GetYaxis()->GetBinLowEdge(hist_j_bin+1) << " " << fDataHist->GetYaxis()->GetBinLowEdge(hist_j_bin+2) << std::endl;
      std::cout << fMCHist->GetXaxis()->GetBinLowEdge(hist_i_bin+1) << " " << fMCHist->GetXaxis()->GetBinLowEdge(hist_i_bin+2) << std::endl;
      std::cout << fMCHist->GetYaxis()->GetBinLowEdge(hist_j_bin+1) << " " << fMCHist->GetYaxis()->GetBinLowEdge(hist_j_bin+2) << std::endl;
      std::cout << "==" << std::endl;

      chi2 += chi2_ij;
    }
  }
  //std::cout << Names[a] << " chi2 " << chi2 << std::endl;
  */

  // Loop over the covariance matrix bins
  for (int i = 0; i < Nbins; ++i) {
    int xbin = i%nbinsx+1;
    int ybin = i/nbinsx+1;
    double datax = fDataHist->GetBinContent(xbin,ybin);
    double mcx = fMCHist->GetBinContent(xbin,ybin);
    //if (i > 0) throw;
    for (int j = 0; j < Nbins; ++j) {
      int xbin2 = j%nbinsx+1;
      int ybin2 = j/nbinsx+1;

      double datay = fDataHist->GetBinContent(xbin2,ybin2);
      double mcy = fMCHist->GetBinContent(xbin2,ybin2);

      double chi2_xy = (datax-mcx)*(*covar)(i,j)*(datay-mcy);
      chi2 += chi2_xy;

      /*
      std::cout << "xbin = " << xbin << " ybin = " << ybin << std::endl;
      std::cout << "xbin2 = " << xbin2 << " ybin2 = " << ybin2 << std::endl;
      std::cout << "i = " << i << " j = " << j << std::endl;

      std::cout << "i in cov: " << fDataHist->GetXaxis()->GetBinLowEdge(xbin) << "-" << fDataHist->GetXaxis()->GetBinLowEdge(xbin+1) << ", " << fDataHist->GetYaxis()->GetBinLowEdge(ybin) << "-" << fDataHist->GetYaxis()->GetBinLowEdge(ybin+1) << std::endl;

      std::cout << "j in cov: " << fDataHist->GetXaxis()->GetBinLowEdge(xbin2) << "-" << fDataHist->GetXaxis()->GetBinLowEdge(xbin2+1) << ", " << fDataHist->GetYaxis()->GetBinLowEdge(ybin2) << "-" << fDataHist->GetYaxis()->GetBinLowEdge(ybin2+1) << std::endl;

      std::cout << "chi2: " << chi2 << " += " << chi2_xy << " for " << i << " " << j << std::endl;
      std::cout << "***" << std::endl;
      */

    }
  }

  /*
   * CWRET CALC
  // Calculate the test-statistic
  for (int i = 0; i < covar_th2d->GetXaxis()->GetNbins()+1; ++i) {
  // Get the global bin for x
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

  // Get the data
  double data1 = fDataHist->GetBinContent(i);
  // Get the MC
  double mc1 = fMCHist->GetBinContent(i);

  for (int j = 0; j < covar_th2d->GetYaxis()->GetNbins()+1; ++j) {

  int xbin2, ybin2, zbin2;
  fDataHist->GetBinXYZ(j, xbin2, ybin2, zbin2);
  double xlo2 = fDataHist->GetXaxis()->GetBinLowEdge(xbin2);
  double xhi2 = fDataHist->GetXaxis()->GetBinLowEdge(xbin2+1);
  double ylo2 = fDataHist->GetYaxis()->GetBinLowEdge(ybin2);
  double yhi2 = fDataHist->GetYaxis()->GetBinLowEdge(ybin2+1);

  if (xlo2 < fDataHist->GetXaxis()->GetBinLowEdge(1) ||
  ylo2 < fDataHist->GetYaxis()->GetBinLowEdge(1) ||
  xhi2 > fDataHist->GetXaxis()->GetBinLowEdge(fDataHist->GetXaxis()->GetNbins()+1) ||
  yhi2 > fDataHist->GetYaxis()->GetBinLowEdge(fDataHist->GetYaxis()->GetNbins()+1)) continue;


  //std::cout << "Correlating: (" << xlo1 << "-" << xhi1 << "," << ylo1 << "-" << yhi1 << ") with (" << xlo2 << "-" << xhi2 << "," << ylo2 << "-" << yhi2 << ")" << std::endl;

  // Get the data
  double data2 = fDataHist->GetBinContent(j);
  // Get the MC
  double mc2 = fMCHist->GetBinContent(j);
  //std::cout << data1 << " " << mc1 << std::endl;
  //std::cout << data2 << " " << mc2 << std::endl;
  //std::cout << std::endl;

  // Get the inverse covariance matrix entry
  double coventry = covar_th2d->GetBinContent(i, j);

  //std::cout << fDataHist->GetXaxis()->GetBinLowEdge(i+1) << " - " << fDataHist->GetXaxis()->GetBinLowEdge(i+2) << ", " << fDataHist->GetYaxis()->GetBinLowEdge(j+1) << " - " << fDataHist->GetYaxis()->GetBinLowEdge(j+2) << " = " << coventry << " (global = " << global << ")" << std::endl;

  chi2 += (data1-mc1)*coventry*(data2-mc2);
  }
  }
  */
  //}

  // Normalisation penalty term if included
  if (fAddNormPen) {
    chi2 +=
      (1 - (fCurrentNorm)) * (1 - (fCurrentNorm)) / (fNormError * fNormError);
    LOG(REC) << "Norm penalty = "
      << (1 - (fCurrentNorm)) * (1 - (fCurrentNorm)) /
      (fNormError * fNormError)
      << std::endl;
  }

  // Adjust the shape back to where it was.
  if (fIsShape and !FitPar::Config().GetParB("saveshapescaling")) {
    fMCHist->Scale(1. / scaleF);
    fMCFine->Scale(1. / scaleF);
  }

  fLikelihood = chi2;

  return chi2;
};
