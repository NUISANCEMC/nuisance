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

#include "Measurement1D.h"

/*
   Constructor/destructor Functions
*/
//********************************************************************
Measurement1D::Measurement1D() {
  //********************************************************************
  fCurrentNorm = 1.0;
  fMCHist = NULL;
  fDataHist = NULL;
  fMCFine = NULL;
  fMCWeighted = NULL;
  fMaskHist = NULL;
  this->covar = NULL;
  fFullCovar = NULL;
  fDecomp = NULL;

  this->fakeDataFile = "";
  fFluxHist = NULL;
  fEventHist = NULL;
  fXSecHist = NULL;

  fDefaultTypes = "FIX/FULL/CHI2";
  fAllowedTypes =
      "FIX,FREE,SHAPE/FULL,DIAG/CHI2/NORM/ENUCORR/Q2CORR/ENU1D/MASK";

  fIsFix = false;
  fIsShape = false;
  fIsFree = false;
  fIsDiag = false;
  fIsFull = false;
  fAddNormPen = false;
  fIsMask = false;
  fIsChi2SVD = false;
  fIsRawEvents = false;
  fIsDifXSec = false;
  fIsEnu1D = false;
}

//********************************************************************
Measurement1D::~Measurement1D() {
  //********************************************************************
}

//********************************************************************
void Measurement1D::Init() {
  //********************************************************************
}

/*
  Setup Functions
  -- All these are used only at the start of the Measurement
*/
//********************************************************************
void Measurement1D::SetupMeasurement(std::string inputfile, std::string type,
                                     FitWeight* rw, std::string fkdt) {
  //********************************************************************

  // Reset everything to NULL
  Init();

  // Check if name contains Evt, indicating that it is a raw number of events
  // measurements and should thus be treated as once
  fIsRawEvents = false;
  if ((fName.find("Evt") != std::string::npos) && fIsRawEvents == false) {
    fIsRawEvents = true;
    LOG(SAM) << "Found event rate measurement but fIsRawEvents == false!"
             << std::endl;
    LOG(SAM) << "Overriding this and setting fIsRawEvents == true!"
             << std::endl;
  }

  fIsEnu1D = false;
  if (fName.find("XSec_1DEnu") != std::string::npos) {
    fIsEnu1D = true;
    LOG(SAM) << "::" << fName << "::" << std::endl;
    LOG(SAM) << "Found XSec Enu measurement, applying flux integrated scaling, "
                "not flux averaged!"
             << std::endl;
  }

  if (fIsEnu1D && fIsRawEvents) {
    LOG(SAM) << "Found 1D Enu XSec distribution AND fIsRawEvents, is this "
                "really correct?!"
             << std::endl;
    LOG(SAM) << "Check experiment constructor for " << fName
             << " and correct this!" << std::endl;
    LOG(SAM) << "I live in " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }

  fRW = rw;

  this->SetupInputs(inputfile);

  // Set Default Options
  SetFitOptions(fDefaultTypes);

  // Set Passed Options
  SetFitOptions(type);

  // Still adding support for flat flux inputs
  //  // Set Enu Flux Scaling
  //  if (isFlatFluxFolding) this->Input()->ApplyFluxFolding(
  //  this->defaultFluxHist );
}

//********************************************************************
void Measurement1D::SetupDefaultHist() {
  //********************************************************************

  // Setup fMCHist
  fMCHist = (TH1D*)fDataHist->Clone();
  fMCHist->SetNameTitle((fName + "_MC").c_str(),
                        (fName + "_MC" + fPlotTitles).c_str());

  // Setup fMCFine
  Int_t nBins = fMCHist->GetNbinsX();
  fMCFine = new TH1D(
      (fName + "_MC_FINE").c_str(), (fName + "_MC_FINE" + fPlotTitles).c_str(),
      nBins * 6, fMCHist->GetBinLowEdge(1), fMCHist->GetBinLowEdge(nBins + 1));

  fMCStat = (TH1D*)fMCHist->Clone();
  fMCStat->Reset();

  fMCHist->Reset();
  fMCFine->Reset();

  // Setup the NEUT Mode Array
  PlotUtils::CreateNeutModeArray((TH1D*)fMCHist, (TH1**)fMCHist_PDG);
  PlotUtils::ResetNeutModeArray((TH1**)fMCHist_PDG);

  // Setup bin masks using sample name
  if (fIsMask) {
    std::string maskloc = FitPar::Config().GetParDIR(fName + ".mask");
    if (maskloc.empty()) {
      maskloc = FitPar::GetDataBase() + "/masks/" + fName + ".mask";
    }

    SetBinMask(maskloc);
  }

  return;
}

//********************************************************************
void Measurement1D::SetFitOptions(std::string opt) {
  //********************************************************************

  // Do nothing if default given
  if (opt == "DEFAULT") return;

  // CHECK Conflicting Fit Options
  std::vector<std::string> fit_option_allow =
      GeneralUtils::ParseToStr(fAllowedTypes, "/");
  for (UInt_t i = 0; i < fit_option_allow.size(); i++) {
    std::vector<std::string> fit_option_section =
        GeneralUtils::ParseToStr(fit_option_allow.at(i), ",");
    bool found_option = false;

    for (UInt_t j = 0; j < fit_option_section.size(); j++) {
      std::string av_opt = fit_option_section.at(j);

      if (!found_option and opt.find(av_opt) != std::string::npos) {
        found_option = true;

      } else if (found_option and opt.find(av_opt) != std::string::npos) {
        ERR(FTL) << "ERROR: Conflicting fit options provided: " << opt
                 << std::endl;
        ERR(FTL) << "Conflicting group = " << fit_option_section.at(i)
                 << std::endl;
        ERR(FTL) << "You should only supply one of these options in card file."
                 << std::endl;
        exit(-1);
      }
    }
  }

  // Check all options are allowed
  std::vector<std::string> fit_options_input = GeneralUtils::ParseToStr(opt, "/");
  for (UInt_t i = 0; i < fit_options_input.size(); i++) {
    if (fAllowedTypes.find(fit_options_input.at(i)) == std::string::npos) {
      ERR(FTL) << "ERROR: Fit Option '" << fit_options_input.at(i)
               << "' Provided is not allowed for this measurement."
               << std::endl;
      ERR(FTL) << "Fit Options should be provided as a '/' seperated list "
                  "(e.g. FREE/DIAG/NORM)"
               << std::endl;
      ERR(FTL) << "Available options for " << fName << " are '" << fAllowedTypes
               << "'" << std::endl;

      exit(-1);
    }
  }

  // Set TYPE
  fFitType = opt;

  // FIX,SHAPE,FREE
  if (opt.find("FIX") != std::string::npos) {
    fIsFree = fIsShape = false;
    fIsFix = true;
  } else if (opt.find("SHAPE") != std::string::npos) {
    fIsFree = fIsFix = false;
    fIsShape = true;
  } else if (opt.find("FREE") != std::string::npos) {
    fIsFix = fIsShape = false;
    fIsFree = true;
  }

  // DIAG,FULL (or default to full)
  if (opt.find("DIAG") != std::string::npos) {
    fIsDiag = true;
    fIsFull = false;
  } else if (opt.find("FULL") != std::string::npos) {
    fIsDiag = false;
    fIsFull = true;
  }

  // CHI2/LL (OTHERS?)
  if (opt.find("LOG") != std::string::npos)
    fIsChi2 = false;
  else
    fIsChi2 = true;

  // EXTRAS
  if (opt.find("RAW") != std::string::npos) fIsRawEvents = true;
  if (opt.find("DIF") != std::string::npos) fIsDifXSec = true;
  if (opt.find("ENU1D") != std::string::npos) fIsEnu1D = true;
  if (opt.find("NORM") != std::string::npos) fAddNormPen = true;
  if (opt.find("MASK") != std::string::npos) fIsMask = true;

  return;
};

//********************************************************************
void Measurement1D::SetDataValues(std::string dataFile) {
  //********************************************************************

  // Override this function if the input file isn't in a suitable format
  LOG(SAM) << "Reading data from: " << dataFile.c_str() << std::endl;
  fDataHist =
      PlotUtils::GetTH1DFromFile(dataFile, (fName + "_data"), fPlotTitles);
  fDataTrue = (TH1D*)fDataHist->Clone();

  // Number of data points is number of bins
  fNDataPointsX = fDataHist->GetXaxis()->GetNbins();

  return;
};

//********************************************************************
void Measurement1D::SetDataFromDatabase(std::string inhistfile,
                                        std::string histname) {
  //********************************************************************

  LOG(SAM) << "Filling histogram from " << inhistfile << "->" << histname
           << std::endl;
  fDataHist = PlotUtils::GetTH1DFromRootFile(
      (GeneralUtils::GetTopLevelDir() + "/data/" + inhistfile), histname);
  fDataHist->SetNameTitle((fName + "_data").c_str(), (fName + "_data").c_str());

  return;
};

//********************************************************************
void Measurement1D::SetDataFromFile(std::string inhistfile,
                                    std::string histname) {
  //********************************************************************

  LOG(SAM) << "Filling histogram from " << inhistfile << "->" << histname
           << std::endl;
  fDataHist = PlotUtils::GetTH1DFromRootFile((inhistfile), histname);
  fDataHist->SetNameTitle((fName + "_data").c_str(), (fName + "_data").c_str());

  return;
};

//********************************************************************
void Measurement1D::SetCovarMatrix(std::string covarFile) {
  //********************************************************************

  // Covariance function, only really used when reading in the MB Covariances.

  TFile* tempFile = new TFile(covarFile.c_str(), "READ");

  TH2D* covarPlot = new TH2D();
  //  TH2D* decmpPlot = new TH2D();
  TH2D* covarInvPlot = new TH2D();
  TH2D* fFullCovarPlot = new TH2D();
  std::string covName = "";
  std::string covOption = FitPar::Config().GetParS("thrown_covariance");

  if (fIsShape || fIsFree) covName = "shp_";
  if (fIsDiag)
    covName += "diag";
  else
    covName += "full";

  covarPlot = (TH2D*)tempFile->Get((covName + "cov").c_str());
  covarInvPlot = (TH2D*)tempFile->Get((covName + "covinv").c_str());

  if (!covOption.compare("SUB"))
    fFullCovarPlot = (TH2D*)tempFile->Get((covName + "cov").c_str());
  else if (!covOption.compare("FULL"))
    fFullCovarPlot = (TH2D*)tempFile->Get("fullcov");
  else
    ERR(WRN) << "Incorrect thrown_covariance option in parameters."
             << std::endl;

  int dim = int(fDataHist->GetNbinsX());  //-this->masked->Integral());
  int covdim = int(fDataHist->GetNbinsX());

  this->covar = new TMatrixDSym(dim);
  fFullCovar = new TMatrixDSym(dim);
  fDecomp = new TMatrixDSym(dim);

  int row, column = 0;
  row = 0;
  column = 0;
  for (Int_t i = 0; i < covdim; i++) {
    // if (this->masked->GetBinContent(i+1) > 0) continue;

    for (Int_t j = 0; j < covdim; j++) {
      //   if (this->masked->GetBinContent(j+1) > 0) continue;

      (*this->covar)(row, column) = covarPlot->GetBinContent(i + 1, j + 1);
      (*fFullCovar)(row, column) = fFullCovarPlot->GetBinContent(i + 1, j + 1);

      column++;
    }
    column = 0;
    row++;
  }

  // Set bin errors on data
  if (!fIsDiag) {
    StatUtils::SetDataErrorFromCov(fDataHist, fFullCovar);
  }

  // Get Deteriminant and inverse matrix
  fCovDet = this->covar->Determinant();

  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");

  return;
};

//********************************************************************
// Sets the covariance matrix from a provided file in a text format
// scale is a multiplicative pre-factor to apply in the case where the covariance is given in some unit (e.g. 1E-38)
void Measurement1D::SetCovarMatrixFromText(std::string covarFile, int dim, double scale) {
//********************************************************************

  // Make a counter to track the line number
  int row = 0;

  std::string line;
  std::ifstream covarread(covarFile.c_str(), ifstream::in);

  this->covar = new TMatrixDSym(dim);
  fFullCovar = new TMatrixDSym(dim);
  if (covarread.is_open())
    LOG(SAM) << "Reading covariance matrix from file: " << covarFile
             << std::endl;
  else
    ERR(FTL) << "Covariance matrix provided is incorrect: " << covarFile
             << std::endl;

  // Loop over the lines in the file
  while (std::getline(covarread >> std::ws, line, '\n')) {
    int column = 0;

    // Loop over entries and insert them into matrix
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
	 iter != entries.end(); iter++){

      (*covar)(row, column) = *iter;
      (*fFullCovar)(row, column) = *iter;

      column++;
    }

    row++;
  }

  // Scale the actualy covariance matrix by some multiplicative factor
  (*fFullCovar) *= scale;

  // Robust matrix inversion method
  TDecompSVD LU = TDecompSVD(*this->covar);
  // THIS IS ACTUALLY THE INVERSE COVARIANCE MATRIXA AAAAARGH
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");

  // Now need to multiply by the scaling factor
  // If the covariance
  (*this->covar) *= 1./(scale);

  return;
};

//********************************************************************
void Measurement1D::SetCovarMatrixFromCorrText(std::string corrFile, int dim){
//********************************************************************

  // Make a counter to track the line number
  int row = 0;

  std::string line;
  std::ifstream corr(corrFile.c_str(),ifstream::in);

  this->covar = new TMatrixDSym(dim);
  this->fFullCovar = new TMatrixDSym(dim);
  if(corr.is_open()) LOG(SAM) << "Reading and converting correlation matrix from file: " << corrFile << std::endl;
  else {
    ERR(FTL) <<"Correlation matrix provided is incorrect: "<<corrFile<<std::endl;
    exit(-1);
  }

  // MINERvA CC1pip needs slightly different method
  // Only half the covariance matrix is given, so fill up the other half
  if (fName.find("MINERvA_CC1pip") != std::string::npos || (fName.find("MINERvA_CCNpip") != std::string::npos && fName.find("2016") == std::string::npos)) {

    LOG(SAM) << "Treating MINERvA CC1pi+ differently" << std::endl;

    // Get a new line from the covariance
    while (std::getline(corr >> std::ws, line, '\n')) {
      int column = 0;

      while (column < dim) {

        if (row > column) {

          (*this->covar)(row,column) = (*this->covar)(column,row);
          column++;

        } else {

	  std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
	  for (std::vector<double>::iterator iter = entries.begin();
	       iter != entries.end(); iter++){

            double val = (*iter)*(this->fDataHist->GetBinError(row+1)*1E38*this->fDataHist->GetBinError(column+1)*1E38); // need in these units to do Cholesky
            if (val == 0) {
              ERR(FTL) << "Found a zero value in the covariance matrix, assuming this is an error!" << std::endl;
              exit(-1);
            }
            (*this->covar)(row, column) = val;
            (*this->fFullCovar)(row, column) = val;
            column++;

          }
        }
      }
      row++;
    }

    // Robust matrix inversion method
    TDecompChol a = TDecompChol(*this->covar);
    this->covar = new TMatrixDSym(dim, a.Invert().GetMatrixArray(), "");

// End special treatment for MINERvA CC1pi+ 2015
// Now do the general case where we have the full matrix
  } else {

    while (std::getline(corr >> std::ws, line, '\n')) {
      int column = 0;

      // Loop over entries and insert them into matrix
      // Multiply by the errors to get the covariance, rather than the correlation matrix
      std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
      for (std::vector<double>::iterator iter = entries.begin();
	   iter != entries.end(); iter++){

        double val = (*iter) * this->fDataHist->GetBinError(row+1)*1E38*this->fDataHist->GetBinError(column+1)*1E38;
        if (val == 0) {
          ERR(FTL) << "Found a zero value in the covariance matrix, assuming this is an error!" << std::endl;
          exit(-1);
        }

        (*this->covar)(row, column) = val;
        (*this->fFullCovar)(row, column) = val;

        column++;
      }

      row++;
    }

    // Robust matrix inversion method
    TDecompSVD LU = TDecompSVD(*this->covar);
    this->covar = new TMatrixDSym(dim, LU .Invert().GetMatrixArray(), "");

  }

  return;
};

//********************************************************************
void Measurement1D::SetSmearingMatrix(std::string smearfile, int truedim, int recodim){
//********************************************************************

  // The smearing matrix describes the migration from true bins (rows) to reco
  // bins (columns)
  // Counter over the true bins!
  int row = 0;

  std::string line;
  std::ifstream smear(smearfile.c_str(), ifstream::in);

  // Note that the smearing matrix may be rectangular.
  fSmearMatrix = new TMatrixD(truedim, recodim);

  if (smear.is_open())
    LOG(SAM) << "Reading smearing matrix from file: " << smearfile << std::endl;
  else
    ERR(FTL) << "Smearing matrix provided is incorrect: " << smearfile
             << std::endl;

  while (std::getline(smear >> std::ws, line, '\n')) {
    int column = 0;

    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
	 iter != entries.end(); iter++){
      (*fSmearMatrix)(row, column) = (*iter) / 100.;  // Convert to fraction from
                                                  // percentage (this may not be
                                                  // general enough)
      column++;
    }
    row++;
  }
  return;
}

//********************************************************************
void Measurement1D::SetCovarFromDataFile(std::string covarFile,
                                         std::string covName) {
  //********************************************************************

  LOG(SAM) << "Getting covariance from " << covarFile << "->" << covName
           << std::endl;

  TFile* tempFile = new TFile(covarFile.c_str(), "READ");
  TH2D* covPlot = (TH2D*)tempFile->Get(covName.c_str());
  covPlot->SetDirectory(0);

  int dim = covPlot->GetNbinsX();
  fFullCovar = new TMatrixDSym(dim);

  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      (*fFullCovar)(i, j) = covPlot->GetBinContent(i + 1, j + 1);
    }
  }

  this->covar = (TMatrixDSym*)fFullCovar->Clone();
  fDecomp = (TMatrixDSym*)fFullCovar->Clone();

  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");

  TDecompChol LUChol = TDecompChol(*fDecomp);
  LUChol.Decompose();
  fDecomp = new TMatrixDSym(dim, LU.GetU().GetMatrixArray(), "");

  return;
};

//********************************************************************
void Measurement1D::SetBinMask(std::string maskFile) {
  //********************************************************************

  // Create a mask histogram.
  int nbins = fDataHist->GetNbinsX();
  fMaskHist =
      new TH1I((fName + "_fMaskHist").c_str(),
               (fName + "_fMaskHist; Bin; Mask?").c_str(), nbins, 0, nbins);
  std::string line;
  std::ifstream mask(maskFile.c_str(), ifstream::in);

  if (mask.is_open())
    LOG(SAM) << "Reading bin mask from file: " << maskFile << std::endl;
  else
    LOG(FTL) << " Cannot find mask file." << std::endl;

  while (std::getline(mask >> std::ws, line, '\n')) {

    std::vector<int> entries = GeneralUtils::ParseToInt(line, " ");

    // Skip lines with poorly formatted lines
    if (entries.size() < 2) {
      LOG(WRN) << "Measurement1D::SetBinMask(), couldn't parse line: " << line << std::endl;
      continue;
    }

    // The first index should be the bin number, the second should be the mask value.
    fMaskHist->SetBinContent(entries[0], entries[1]);
  }
  
  // Set masked data bins to zero
  PlotUtils::MaskBins(fDataHist, fMaskHist);

  return;
}

/*
  XSec Functions
*/

//********************************************************************
void Measurement1D::SetFluxHistogram(std::string fluxFile, int minE, int maxE,
                                     double fluxNorm) {
  //********************************************************************

  // Note this expects the flux bins to be given in terms of MeV
  LOG(SAM) << "Reading flux from file: " << fluxFile << std::endl;

  TGraph f(fluxFile.c_str(), "%lg %lg");

  fFluxHist =
      new TH1D((fName + "_flux").c_str(), (fName + "; E_{#nu} (GeV)").c_str(),
               f.GetN() - 1, minE, maxE);

  Double_t* yVal = f.GetY();

  for (int i = 0; i < fFluxHist->GetNbinsX(); ++i)
    fFluxHist->SetBinContent(i + 1, yVal[i] * fluxNorm);
};

//********************************************************************
double Measurement1D::TotalIntegratedFlux(std::string intOpt, double low,
                                          double high) {
  //********************************************************************

  if (GetInput()->GetType() == kGiBUU) {
    return 1.0;
  }

  // Set Energy Limits
  if (low == -9999.9) low = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;

  int minBin = fFluxHist->GetXaxis()->FindBin(low);
  int maxBin = fFluxHist->GetXaxis()->FindBin(high);

  // Get integral over custom range
  double integral = fFluxHist->Integral(minBin, maxBin + 1, intOpt.c_str());

  return integral;
};

/*
  Reconfigure LOOP
*/

//********************************************************************
void Measurement1D::ResetAll() {
  //********************************************************************

  // Simple function to reset the mc Histograms incase that is all that is
  // needed.

  // Clear histograms
  fMCHist->Reset();
  fMCFine->Reset();
  fMCStat->Reset();

  PlotUtils::ResetNeutModeArray((TH1**)fMCHist_PDG);

  return;
};

//********************************************************************
void Measurement1D::FillHistograms() {
  //********************************************************************

  if (Signal) {
    fMCHist->Fill(fXVar, Weight);
    fMCFine->Fill(fXVar, Weight);
    fMCStat->Fill(fXVar, 1.0);

    PlotUtils::FillNeutModeArray(fMCHist_PDG, Mode, fXVar, Weight);
  }

  return;
};

//********************************************************************
void Measurement1D::ScaleEvents() {
  //********************************************************************

  LOG(REC) << std::setw(20) << " " << fMCHist->Integral() << "/" << fNEvents
           << " events passed selection + binning after reweight" << std::endl;

  // Simple function to scale to xsec result if this is all that is needed.
  // Scale bin errors correctly
  TH1D* tempFine = (TH1D*)fMCFine->Clone();

  // Create Weighted Histogram
  if (fMCWeighted) delete fMCWeighted;
  fMCWeighted = (TH1D*)fMCHist->Clone();
  fMCWeighted->SetNameTitle((fName + "_MC_WGHTS").c_str(),
                            (fName + "_MC_WGHTS" + fPlotTitles).c_str());
  fMCWeighted->GetYaxis()->SetTitle("Weighted Events");

  // Should apply different scaling for:
  // 1D Enu distributions -- need bin by bin flux unfolding (bin by bin flux
  // integration)
  // 1D count distributions -- need shape scaling to data
  // anything else -- flux averages

  LOG(DEB) << "Scaling Factor = " << fScaleFactor << endl;
  LOG(DEB) << "MC Hist = " << fMCHist->Integral() << endl;

  // Scaling for raw event rates
  if (fIsRawEvents) {
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG,
                                  (fDataHist->Integral() / fMCHist->Integral()),
                                  "width");
    fMCHist->Scale(fDataHist->Integral() / fMCHist->Integral());
    fMCFine->Scale(fDataHist->Integral() / fMCFine->Integral());

    // Scaling for XSec as function of Enu
  } else if (fIsEnu1D) {
    PlotUtils::FluxUnfoldedScaling(fMCHist, fFluxHist);
    PlotUtils::FluxUnfoldedScaling(fMCFine, fFluxHist);

    fMCHist->Scale(fScaleFactor);
    fMCFine->Scale(fScaleFactor);
    // Any other differential scaling
  } else {
    fMCHist->Scale(fScaleFactor, "width");
    fMCFine->Scale(fScaleFactor, "width");
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG, fScaleFactor, "width");
  }

  // Proper error scaling - ROOT Freaks out with xsec weights sometimes
  // Scale the MC histogram
  for (int i = 0; i < fMCStat->GetNbinsX(); i++) {
    if (fMCStat->GetBinContent(i + 1) != 0) {
      fMCHist->SetBinError(i + 1, fMCHist->GetBinContent(i + 1) *
                                      fMCStat->GetBinError(i + 1) /
                                      fMCStat->GetBinContent(i + 1));
    } else {
      fMCHist->SetBinError(i + 1, fMCHist->Integral());
    }
  }

  // Scale the fine MC histogram
  for (int i = 0; i < tempFine->GetNbinsX(); i++) {
    if (tempFine->GetBinContent(i + 1) != 0) {
      fMCFine->SetBinError(i + 1, fMCFine->GetBinContent(i + 1) *
                                      tempFine->GetBinError(i + 1) /
                                      tempFine->GetBinContent(i + 1));
    } else {
      fMCFine->SetBinError(i + 1, fMCFine->Integral());
    }
  }

  return;
};

//********************************************************************
void Measurement1D::ApplyNormScale(double norm) {
  //********************************************************************

  fCurrentNorm = norm;

  fMCHist->Scale(1.0 / norm);
  fMCFine->Scale(1.0 / norm);

  PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG, 1.0 / norm);

  return;
};

//********************************************************************
void Measurement1D::ApplySmearingMatrix() {
  //********************************************************************

  if (!fSmearMatrix) {
    ERR(WRN) << fName
             << ": attempted to apply smearing matrix, but none was set"
             << std::endl;
    return;
  }

  TH1D* unsmeared = (TH1D*)fMCHist->Clone();
  TH1D* smeared = (TH1D*)fMCHist->Clone();
  smeared->Reset();

  // Loop over reconstructed bins
  // true = row; reco = column
  for (int rbin = 0; rbin < fSmearMatrix->GetNcols(); ++rbin) {
    // Sum up the constributions from all true bins
    double rBinVal = 0;

    // Loop over true bins
    for (int tbin = 0; tbin < fSmearMatrix->GetNrows(); ++tbin) {
      rBinVal +=
          (*fSmearMatrix)(tbin, rbin) * unsmeared->GetBinContent(tbin + 1);
    }
    smeared->SetBinContent(rbin + 1, rBinVal);
  }
  fMCHist = (TH1D*)smeared->Clone();

  return;
}

/*
  Statistic Functions - Outsources to StatUtils
*/

//********************************************************************
int Measurement1D::GetNDOF() {
  //********************************************************************
  return fDataHist->GetNbinsX();  // - fMaskHist->Integral();
}

//********************************************************************
double Measurement1D::GetLikelihood() {
  //********************************************************************

  double stat = 0.0;

  // Fix weird masking bug
  if (!fIsMask) {
    if (fMaskHist) {
      fMaskHist = NULL;
    }
  } else {
    if (fMaskHist) {
      PlotUtils::MaskBins(fMCHist, fMaskHist);
    }
  }

  // Sort Initial Scaling
  double scaleF = 0.0;
  if (fMCHist->Integral(1, fMCHist->GetNbinsX(), "width")) {
    scaleF = fDataHist->Integral(1, fDataHist->GetNbinsX(), "width") /
             fMCHist->Integral(1, fMCHist->GetNbinsX(), "width");
  }

  if (fIsShape) {
    fMCHist->Scale(scaleF);
    fMCFine->Scale(scaleF);
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG, scaleF);
  }

  // Get Chi2
  if (fIsChi2) {
    if (!fIsDiag) {
      if (!fIsChi2SVD) {
        stat = StatUtils::GetChi2FromCov(fDataHist, fMCHist, covar, fMaskHist);
      } else {
        stat = StatUtils::GetChi2FromSVD(fDataHist, fMCHist, fFullCovar,
                                         fMaskHist);
      }

    } else {
      if (fIsRawEvents) {
        stat = StatUtils::GetChi2FromEventRate(fDataHist, fMCHist, fMaskHist);
      } else {
        stat = StatUtils::GetChi2FromDiag(fDataHist, fMCHist, fMaskHist);
      }
    }
  } else {
    if (!fIsDiag) {
      if (!fIsChi2SVD)
        stat = StatUtils::GetLikelihoodFromCov(fDataHist, fMCHist, covar,
                                               fMaskHist);
      else
        stat = StatUtils::GetLikelihoodFromSVD(fDataHist, fMCHist, fFullCovar,
                                               fMaskHist);
    } else {
      if (fIsRawEvents)
        stat = StatUtils::GetLikelihoodFromEventRate(fDataHist, fMCHist,
                                                     fMaskHist);
      else
        stat = StatUtils::GetLikelihoodFromDiag(fDataHist, fMCHist, fMaskHist);
    }
  }

  // Sort Penalty Terms
  if (fAddNormPen) {
    if (fNormError <= 0.0) {
      ERR(WRN) << "Norm error for class " << fName << " is 0.0!" << endl;
      ERR(WRN) << "Skipping norm penalty." << endl;
    }

    double penalty =
        (1. - fCurrentNorm) * (1. - fCurrentNorm) / (fNormError * fNormError);
    stat += penalty;
  }

  // Return to normal scaling
  if (fIsShape and !FitPar::Config().GetParB("saveshapescaling")) {
    fMCHist->Scale(1. / scaleF);
    fMCFine->Scale(1. / scaleF);
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG, 1.0 / scaleF);
  }

  return stat;
}

//********************************************************************
void Measurement1D::SetFakeDataValues(std::string fakeOption) {
  //********************************************************************

  // Reset things
  if (fIsFakeData) {
    this->ResetFakeData();
  } else {
    fIsFakeData = true;
  }

  // Make a copy of the original data histogram.
  if (!(fDataOrig))
    fDataOrig = (TH1D*)fDataHist->Clone((fName + "_data_original").c_str());

  TH1D* tempData = (TH1D*)fDataHist->Clone();
  TFile* fake = new TFile();

  if (fakeOption.compare("MC") == 0) {
    LOG(SAM) << "Setting fake data from MC " << std::endl;
    fDataHist = (TH1D*)fMCHist->Clone((fName + "_MC").c_str());
    if (fMCHist->Integral() == 0.0)
      ERR(WRN) << fName << ": Invalid histogram" << std::endl;
  } else {
    fake = new TFile(fakeOption.c_str());
    fDataHist = (TH1D*)fake->Get((fName + "_MC").c_str());
  }

  fDataHist->SetNameTitle((fName + "_FAKE").c_str(),
                          (fName + fPlotTitles).c_str());

  fDataTrue = (TH1D*)fDataHist->Clone();
  fDataTrue->SetNameTitle((fName + "_FAKE_TRUE").c_str(),
                          (fName + fPlotTitles).c_str());

  int nbins = fDataHist->GetNbinsX();
  double alpha_i = 0.0;
  double alpha_j = 0.0;

  for (int i = 0; i < nbins; i++) {
    for (int j = 0; j < nbins; j++) {
      alpha_i =
          fDataHist->GetBinContent(i + 1) / tempData->GetBinContent(i + 1);
      alpha_j =
          fDataHist->GetBinContent(j + 1) / tempData->GetBinContent(j + 1);

      (*this->covar)(i, j) = (1.0 / (alpha_i * alpha_j)) * (*this->covar)(i, j);
      (*fFullCovar)(i, j) = alpha_i * alpha_j * (*fFullCovar)(i, j);
    }
  }

  (this->covar) = (TMatrixDSym*)fFullCovar->Clone();
  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(nbins, LU.Invert().GetMatrixArray(), "");

  return;
};

//********************************************************************
void Measurement1D::ResetFakeData() {
  //********************************************************************

  if (fIsFakeData)
    if (fDataHist) delete fDataHist;
  fDataHist = (TH1D*)fDataTrue->Clone((fName + "_FKDAT").c_str());

  return;
}

//********************************************************************
void Measurement1D::ResetData() {
  //********************************************************************

  if (fIsFakeData)
    if (fDataHist) delete fDataHist;
  fDataHist = (TH1D*)fDataTrue->Clone((fName + "_Data").c_str());

  fIsFakeData = false;
}

//********************************************************************
void Measurement1D::ThrowCovariance() {
  //********************************************************************

  // Take a fDecomposition and use it to throw the current dataset.
  // Requires fDataTrue also be set incase used repeatedly.

  delete fDataHist;
  fDataHist = StatUtils::ThrowHistogram(fDataTrue, fFullCovar);

  return;
};

/*
  Access Functions
*/

//********************************************************************
std::vector<TH1*> Measurement1D::GetMCList() {
  //********************************************************************

  // If this isn't a NULL pointer, make the plot pretty!
  if (!fMCHist) return std::vector<TH1*>(1, fMCHist);

  std::ostringstream chi2;
  chi2 << std::setprecision(5) << this->GetLikelihood();

  int plotcolor = kRed;
  if (FitPar::Config().GetParI("linecolour") > 0) {
    plotcolor = FitPar::Config().GetParI("linecolour");
  }

  int plotstyle = 1;
  if (FitPar::Config().GetParI("linestyle") > 0) {
    plotstyle = FitPar::Config().GetParI("linestyle");
  }

  int plotfillstyle = 0;
  if (FitPar::Config().GetParI("fillstyle") > 0) {
    plotfillstyle = FitPar::Config().GetParI("fillstyle");
  }

  fMCHist->SetTitle(chi2.str().c_str());
  fMCHist->SetLineWidth(3);
  fMCHist->SetLineColor(plotcolor);
  fMCHist->SetFillColor(plotcolor);
  fMCHist->SetLineStyle(plotstyle);
  fMCHist->SetFillStyle(plotfillstyle);

  return std::vector<TH1*>(1, fMCHist);
};

//********************************************************************
std::vector<TH1*> Measurement1D::GetDataList() {
  //********************************************************************
  // If this isn't a NULL pointer, make the plot pretty!

  if (!fDataHist) return std::vector<TH1*>(1, fDataHist);

  fDataHist->SetLineWidth(2);
  fDataHist->SetMarkerStyle(8);

  fDataHist->SetLineColor(kBlack);

  return std::vector<TH1*>(1, fDataHist);
};

//********************************************************************
void Measurement1D::GetBinContents(std::vector<double>& cont,
                                   std::vector<double>& err) {
  //********************************************************************

  // Return a vector of the main bin contents
  for (int i = 0; i < fMCHist->GetNbinsX(); i++) {
    cont.push_back(fMCHist->GetBinContent(i + 1));
    err.push_back(fMCHist->GetBinError(i + 1));
  }

  return;
};

//********************************************************************
std::vector<double> Measurement1D::GetXSec(std::string option) {
  //********************************************************************

  std::vector<double> vals;
  vals.push_back(0.0);
  vals.push_back(0.0);

  bool getMC = !option.compare("MC");
  bool getDT = !option.compare("DATA");

  for (int i = 0; i < fMCHist->GetNbinsX(); i++) {
    if (fDataHist->GetBinContent(i + 1) == 0.0 and
        fDataHist->GetBinError(i + 1) == 0.0)
      continue;

    if (getMC) {
      vals[0] += fMCHist->GetBinContent(i + 1) *
                 fMCHist->GetXaxis()->GetBinWidth(i + 1);
      vals[1] += fMCHist->GetBinError(i + 1) * fMCHist->GetBinError(i + 1) *
                 fMCHist->GetXaxis()->GetBinWidth(i + 1) *
                 fMCHist->GetXaxis()->GetBinWidth(i + 1);

    } else if (getDT) {
      vals[0] += fDataHist->GetBinContent(i + 1) *
                 fDataHist->GetXaxis()->GetBinWidth(i + 1);
      vals[1] += fDataHist->GetBinError(i + 1) * fDataHist->GetBinError(i + 1) *
                 fDataHist->GetXaxis()->GetBinWidth(i + 1) *
                 fDataHist->GetXaxis()->GetBinWidth(i + 1);
    }
  }

  // If not diag Get the total error from the covariance
  if (!fIsDiag and !fIsRawEvents and getDT and fFullCovar) {
    vals[1] = 0.0;
    for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
      for (int j = 0; j < fDataHist->GetNbinsX(); j++) {
        vals[1] += (*fFullCovar)(i, j);
      }
    }
    vals[1] = sqrt(vals[1]) * 1E-38;
  }

  return vals;
}

/*
  Write Functions
*/

// Save all the histograms at once
//********************************************************************
void Measurement1D::Write(std::string drawOpt) {
  //********************************************************************

  // If null pointer return
  if (!fMCHist and !fDataHist) {
    LOG(SAM) << fName << "Incomplete histogram set!" << std::endl;
    return;
  }
  FitPar::Config().out->cd();

  // Get Draw Options
  drawOpt = FitPar::Config().GetParS("drawopts");
  bool drawData = (drawOpt.find("DATA") != std::string::npos);
  bool drawNormal = (drawOpt.find("MC") != std::string::npos);
  bool drawEvents = (drawOpt.find("EVT") != std::string::npos);
  bool drawFine = (drawOpt.find("FINE") != std::string::npos);
  bool drawRatio = (drawOpt.find("RATIO") != std::string::npos);
  bool drawModes = (drawOpt.find("MODES") != std::string::npos);
  bool drawShape = (drawOpt.find("SHAPE") != std::string::npos);
  //  bool residual = (drawOpt.find("RESIDUAL") != std::string::npos);
  //  bool drawMatrix = (drawOpt.find("MATRIX") != std::string::npos);
  bool drawXSec = (drawOpt.find("XSEC") != std::string::npos);
  bool drawFlux = (drawOpt.find("FLUX") != std::string::npos);
  bool drawMask = (drawOpt.find("MASK") != std::string::npos);
  bool drawCov = (drawOpt.find("COV") != std::string::npos);
  bool drawInvCov = (drawOpt.find("INVCOV") != std::string::npos);
  bool drawDecomp = (drawOpt.find("DECOMP") != std::string::npos);
  bool drawCanvPDG = (drawOpt.find("CANVPDG") != std::string::npos);
  bool drawCanvMC = (drawOpt.find("CANVMC") != std::string::npos);

  bool drawWeighted = (drawOpt.find("WGHT") != std::string::npos);

  if (FitPar::Config().GetParB("EventManager")){
    drawFlux = false;
    drawXSec = false;
    drawEvents = false;
  }

  // Save standard plots
  if (drawData) this->GetDataList().at(0)->Write();
  if (drawNormal) this->GetMCList().at(0)->Write();

  // Save only mc and data if splines
  if (fEventType == 4 or fEventType == 3) {
    return;
  }

  // Draw Extra plots
  if (drawFine) this->GetFineList().at(0)->Write();

  if (fIsMask and drawMask)
    fMaskHist->Write((fName + "_MSK").c_str());  //< save mask

  if (drawFlux) fFluxHist->Write();
  if (drawXSec) fXSecHist->Write();
  if (drawEvents) fEventHist->Write();

  if (fIsMask and drawMask and fMaskHist) {
    fMaskHist->Write((fName + "_MSK").c_str());  //< save mask
  }

  // Save neut stack
  if (drawModes) {
    //    LOG(SAM) << "Writing MC Hist PDG" << std::endl;
    THStack combo_fMCHist_PDG = PlotUtils::GetNeutModeStack(
        (fName + "_MC_PDG").c_str(), (TH1**)fMCHist_PDG, 0);
    combo_fMCHist_PDG.Write();
  }

  // Save Matrix plots
  if (!fIsRawEvents and !fIsDiag) {
    if (drawCov and fFullCovar) {
      TH2D cov = TH2D((*fFullCovar));
      cov.SetNameTitle((fName + "_cov").c_str(),
                       (fName + "_cov;Bins; Bins;").c_str());
      cov.Write();
    }
  }

  if (fIsMask && drawMask && fMaskHist) {
    fMaskHist->Write((this->fName + "_MSK").c_str());  //< save mask
  }

  // Save neut stack
  if (drawModes) {
    // LOG(SAM) << "Writing MC Hist PDG"<<std::endl;
    THStack combo_mcHist_PDG = PlotUtils::GetNeutModeStack(
        (this->fName + "_MC_PDG").c_str(), (TH1**)this->fMCHist_PDG, 0);
    combo_mcHist_PDG.Write();
  }

  // Save Matrix plots
  if (!fIsRawEvents && !fIsDiag && fFullCovar) {
    if (drawCov && fFullCovar) {
      TH2D cov = TH2D((*this->fFullCovar));
      cov.SetNameTitle((this->fName + "_cov").c_str(),
                       (this->fName + "_cov;Bins; Bins;").c_str());
      cov.Write();
    }

    if (drawInvCov && covar) {
      TH2D covinv = TH2D((*this->covar));
      covinv.SetNameTitle((fName + "_covinv").c_str(),
                          (fName + "_cov;Bins; Bins;").c_str());
      covinv.Write();
    }

    if (drawDecomp and fDecomp) {
      TH2D covdec = TH2D((*fDecomp));
      covdec.SetNameTitle((fName + "_covdec").c_str(),
                          (fName + "_cov;Bins; Bins;").c_str());
      covdec.Write();
    }
  }

  // Save ratio plots if required
  if (drawRatio) {
    // Needed for error bars
    for (int i = 0; i < fMCHist->GetNbinsX() * fMCHist->GetNbinsY(); i++)
      fMCHist->SetBinError(i + 1, 0.0);

    fDataHist->GetSumw2();
    fMCHist->GetSumw2();

    // Create Ratio Histograms
    TH1D* dataRatio = (TH1D*)fDataHist->Clone((fName + "_data_RATIO").c_str());
    TH1D* mcRatio = (TH1D*)fMCHist->Clone((fName + "_MC_RATIO").c_str());

    mcRatio->Divide(fMCHist);
    dataRatio->Divide(fMCHist);

    // Cancel bin errors on MC
    for (int i = 0; i < mcRatio->GetNbinsX(); i++)
      mcRatio->SetBinError(
          i + 1, fMCHist->GetBinError(i + 1) / fMCHist->GetBinContent(i + 1));

    mcRatio->SetMinimum(0);
    mcRatio->SetMaximum(2);
    dataRatio->SetMinimum(0);
    dataRatio->SetMaximum(2);

    mcRatio->Write();
    dataRatio->Write();

    delete mcRatio;
    delete dataRatio;
  }

  // Save Shape Plots if required
  if (drawShape) {
    // Create Shape Histogram
    TH1D* mcShape = (TH1D*)fMCHist->Clone((fName + "_MC_SHAPE").c_str());

    double shapeScale =
        fDataHist->Integral("width") / fMCHist->Integral("width");
    mcShape->Scale(shapeScale);

    std::stringstream ss;
    ss << shapeScale;
    mcShape->SetTitle(ss.str().c_str());

    mcShape->SetLineWidth(3);
    mcShape->SetLineStyle(7);  // dashes

    mcShape->Write();

    // Save shape ratios
    if (drawRatio) {
      // Needed for error bars
      mcShape->GetSumw2();

      // Create shape ratio histograms
      TH1D* mcShapeRatio =
          (TH1D*)mcShape->Clone((fName + "_MC_SHAPE_RATIO").c_str());
      TH1D* dataShapeRatio =
          (TH1D*)fDataHist->Clone((fName + "_data_SHAPE_RATIO").c_str());

      // Divide the histograms
      mcShapeRatio->Divide(mcShape);
      dataShapeRatio->Divide(mcShape);

      // Colour the shape ratio plots
      mcShapeRatio->SetLineWidth(3);
      mcShapeRatio->SetLineStyle(7);  // dashes

      mcShapeRatio->Write();
      dataShapeRatio->Write();

      delete mcShapeRatio;
      delete dataShapeRatio;
    }

    delete mcShape;
  }

  // Make a pretty PDG Canvas
  if (drawCanvPDG) {
    TCanvas* c1 = new TCanvas((fName + "_PDG_CANV").c_str(),
                              (fName + "_PDG_CANV").c_str(), 800, 600);

    fDataHist->Draw("E1");
    fMCHist->Draw("HIST SAME");

    THStack combo_fMCHist_PDG = PlotUtils::GetNeutModeStack(
        (fName + "_MC_PDG").c_str(), (TH1**)fMCHist_PDG, 0);
    combo_fMCHist_PDG.Draw("HIST SAME");
    TLegend leg =
        PlotUtils::GenerateStackLegend(combo_fMCHist_PDG, 0.6, 0.6, 0.9, 0.9);
    fDataHist->Draw("E1 SAME");

    //    leg.Draw("SAME");
    c1->Write();
    delete c1;
  }

  if (drawCanvMC) {
    TCanvas* c1 = new TCanvas((fName + "_MC_CANV").c_str(),
                              (fName + "_MC_CANV").c_str(), 800, 600);
    c1->cd();

    fDataHist->Draw("E1");
    fMCHist->Draw("SAME HIST");

    TH1D* mcShape = (TH1D*)fMCHist->Clone((fName + "_MC_SHAPE").c_str());
    double shapeScale =
        fDataHist->Integral("width") / fMCHist->Integral("width");

    mcShape->Scale(shapeScale);
    mcShape->SetLineStyle(7);

    mcShape->Draw("SAME HIST");

    TLegend* leg = new TLegend(0.6, 0.6, 0.9, 0.9);
    leg->AddEntry(fDataHist, (fName + " Data").c_str(), "ep");
    leg->AddEntry(fMCHist, (fName + " MC").c_str(), "l");
    leg->AddEntry(mcShape, (fName + " Shape").c_str(), "l");

    leg->Draw("SAME");

    c1->Write();
    delete c1;
  }

  if (drawWeighted) {
    fMCWeighted->Write();
  }

  // Returning
  LOG(SAM) << "Written Histograms: " << fName << std::endl;
  return;
};

// ********************************************
// Returns the NEUT mode stack
THStack Measurement1D::GetModeStack() {
  // ********************************************
  THStack combo_hist = PlotUtils::GetNeutModeStack((fName + "_MC_PDG").c_str(),
                                                   (TH1**)fMCHist_PDG, 0);
  return combo_hist;
}
