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

#include "Measurement2D.h"
#include "TDecompChol.h"

//********************************************************************
Measurement2D::Measurement2D(void) {
  //********************************************************************

  covar = NULL;
  fDecomp = NULL;
  fFullCovar = NULL;

  fMCHist = NULL;
  fMCFine = NULL;
  fDataHist = NULL;

  fMCHist_X = NULL;
  fMCHist_Y = NULL;
  fDataHist_X = NULL;
  fDataHist_Y = NULL;

  fMaskHist = NULL;
  fMapHist = NULL;
  fDataOrig = NULL;
  fDataTrue = NULL;
  fMCWeighted = NULL;

  fResidualHist = NULL;
  fChi2LessBinHist = NULL;

  fDefaultTypes = "FIX/FULL/CHI2";
  fAllowedTypes =
      "FIX,FREE,SHAPE/FULL,DIAG/CHI2/NORM/ENUCORR/Q2CORR/ENU1D/FITPROJX/"
      "FITPROJY";

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
  fIsEnu = false;

  // XSec Scalings
  fScaleFactor = -1.0;
  fCurrentNorm = 1.0;

  // Histograms
  fDataHist = NULL;
  fDataTrue = NULL;

  fMCHist = NULL;
  fMCFine = NULL;
  fMCWeighted = NULL;

  fMaskHist = NULL;

  // Covar
  covar = NULL;
  fFullCovar = NULL;

  fCovar = NULL;
  fInvert = NULL;
  fDecomp = NULL;

  // Fake Data
  fFakeDataInput = "";
  fFakeDataFile = NULL;

  // Options
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
  fIsWriting = false;

  // Inputs
  fInput = NULL;
  fRW = NULL;

  // Extra Histograms
  fMCHist_Modes = NULL;
}

//********************************************************************
Measurement2D::~Measurement2D(void) {
  //********************************************************************

  if (fDataHist)
    delete fDataHist;
  if (fDataTrue)
    delete fDataTrue;
  if (fMCHist)
    delete fMCHist;
  if (fMCFine)
    delete fMCFine;
  if (fMCWeighted)
    delete fMCWeighted;
  if (fMaskHist)
    delete fMaskHist;
  if (covar)
    delete covar;
  if (fFullCovar)
    delete fFullCovar;
  if (fCovar)
    delete fCovar;
  if (fInvert)
    delete fInvert;
  if (fDecomp)
    delete fDecomp;

  delete fResidualHist;
  delete fChi2LessBinHist;
}

//********************************************************************
void Measurement2D::FinaliseSampleSettings() {
  //********************************************************************

  MeasurementBase::FinaliseSampleSettings();

  // Setup naming + renaming
  fName = fSettings.GetName();
  fSettings.SetS("originalname", fName);
  if (fSettings.Has("rename")) {
    fName = fSettings.GetS("rename");
    fSettings.SetS("name", fName);
  }

  // Setup all other options
  NUIS_LOG(SAM, "Finalising Sample Settings: " << fName);

  if ((fSettings.GetS("originalname").find("Evt") != std::string::npos)) {
    fIsRawEvents = true;
    NUIS_LOG(SAM,
             "Found event rate measurement but using poisson likelihoods.");
  }

  if (fSettings.GetS("originalname").find("Enu") != std::string::npos) {
    fIsEnu1D = true;
    NUIS_LOG(SAM, "::" << fName << "::");
    NUIS_LOG(SAM,
             "Found XSec Enu measurement, applying flux integrated scaling, "
                 << "not flux averaged!");
  }

  if (fIsEnu1D && fIsRawEvents) {
    NUIS_ERR(FTL, "Found 2D Enu XSec distribution AND fIsRawEvents, is this "
                  "really correct?!");
    NUIS_ERR(FTL, "Check experiment constructor for " << fName
                                                      << " and correct this!");
    NUIS_ABORT("I live in " << __FILE__ << ":" << __LINE__);
  }

  if (!fRW)
    fRW = FitBase::GetRW();
  if (!fInput)
    SetupInputs(fSettings.GetS("input"));

  // Setup options
  SetFitOptions(fDefaultTypes);          // defaults
  SetFitOptions(fSettings.GetS("type")); // user specified

  EnuMin = GeneralUtils::StrToDbl(fSettings.GetS("enu_min"));
  EnuMax = GeneralUtils::StrToDbl(fSettings.GetS("enu_max"));
}

void Measurement2D::CreateDataHistogram(int dimx, double *binx, int dimy,
                                        double *biny) {
  if (fDataHist)
    delete fDataHist;

  NUIS_LOG(SAM, "Creating Data Histogram dim : " << dimx << " " << dimy);

  fDataHist = new TH2D((fSettings.GetName() + "_data").c_str(),
                       (fSettings.GetFullTitles()).c_str(), dimx - 1, binx,
                       dimy - 1, biny);
}

void Measurement2D::SetDataFromTextFile(std::string data, std::string binx,
                                        std::string biny) {
  // Get the data hist
  fDataHist = PlotUtils::GetTH2DFromTextFile(data, binx, biny);
  // Set the name properly
  fDataHist->SetName((fSettings.GetName() + "_data").c_str());
  fDataHist->SetTitle(fSettings.GetFullTitles().c_str());
}

void Measurement2D::SetDataFromRootFile(std::string datfile,
                                        std::string histname) {
  NUIS_LOG(SAM, "Reading data from root file: " << datfile << ";" << histname);
  fDataHist = PlotUtils::GetTH2DFromRootFile(datfile, histname);
  fDataHist->SetNameTitle((fSettings.GetName() + "_data").c_str(),
                          (fSettings.GetFullTitles()).c_str());
}

void Measurement2D::SetDataValuesFromTextFile(std::string datfile, TH2D *hist) {

  NUIS_LOG(SAM, "Setting data values from text file");
  if (!hist)
    hist = fDataHist;

  // Read TH2D From textfile
  TH2D *valhist = (TH2D *)hist->Clone();
  valhist->Reset();
  PlotUtils::Set2DHistFromText(datfile, valhist, 1.0, true);

  NUIS_LOG(SAM, " -> Filling values from read hist.");
  for (int i = 0; i < valhist->GetNbinsX(); i++) {
    for (int j = 0; j < valhist->GetNbinsY(); j++) {
      hist->SetBinContent(i + 1, j + 1, valhist->GetBinContent(i + 1, j + 1));
    }
  }
  NUIS_LOG(SAM, " --> Done");
}

void Measurement2D::SetDataErrorsFromTextFile(std::string datfile, TH2D *hist) {
  NUIS_LOG(SAM, "Setting data errors from text file");

  if (!hist)
    hist = fDataHist;

  // Read TH2D From textfile
  TH2D *valhist = (TH2D *)hist->Clone();
  valhist->Reset();
  PlotUtils::Set2DHistFromText(datfile, valhist, 1.0);

  // Fill Errors
  NUIS_LOG(SAM, " -> Filling errors from read hist.");

  for (int i = 0; i < valhist->GetNbinsX(); i++) {
    for (int j = 0; j < valhist->GetNbinsY(); j++) {
      hist->SetBinError(i + 1, j + 1, valhist->GetBinContent(i + 1, j + 1));
    }
  }
  NUIS_LOG(SAM, " --> Done");
}

void Measurement2D::SetMapValuesFromText(std::string dataFile) {

  TH2D *hist = fDataHist;
  std::vector<double> edgex;
  std::vector<double> edgey;

  for (int i = 0; i <= hist->GetNbinsX(); i++)
    edgex.push_back(hist->GetXaxis()->GetBinLowEdge(i + 1));
  for (int i = 0; i <= hist->GetNbinsY(); i++)
    edgey.push_back(hist->GetYaxis()->GetBinLowEdge(i + 1));

  fMapHist = new TH2I((fName + "_map").c_str(), (fName + fPlotTitles).c_str(),
                      edgex.size() - 1, &edgex[0], edgey.size() - 1, &edgey[0]);

  NUIS_LOG(SAM, "Reading map from: " << dataFile);
  PlotUtils::Set2DHistFromText(dataFile, fMapHist, 1.0);
}

//********************************************************************
void Measurement2D::SetPoissonErrors() {
  //********************************************************************

  if (!fDataHist) {
    NUIS_ERR(FTL, "Need a data hist to setup possion errors! ");
    NUIS_ABORT("Setup Data First!");
  }

  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i + 1, sqrt(fDataHist->GetBinContent(i + 1)));
  }
}

//********************************************************************
void Measurement2D::SetCovarFromDiagonal(TH2D *data) {
  //********************************************************************

  if (!data and fDataHist) {
    data = fDataHist;
  }

  if (data) {
    NUIS_LOG(SAM, "Setting diagonal covariance for: " << data->GetName());
    fFullCovar = StatUtils::MakeDiagonalCovarMatrix(data);
    covar = StatUtils::GetInvert(fFullCovar);
    fDecomp = StatUtils::GetDecomp(fFullCovar);
  } else {
    NUIS_ABORT("No data input provided to set diagonal covar from!");
  }

  // if (!fIsDiag) {
  //   ERR(FTL) << "SetCovarMatrixFromDiag called for measurement "
  //            << "that is not set as diagonal." << std::endl;
  //   throw;
  // }
}

//********************************************************************
void Measurement2D::SetCovarFromTextFile(std::string covfile, int dim) {
  //********************************************************************

  if (dim == -1) {
    dim = this->GetNDOF();
  }

  NUIS_LOG(SAM, "Reading covariance from text file: " << covfile << " " << dim);
  fFullCovar = StatUtils::GetCovarFromTextFile(covfile, dim);
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}

//********************************************************************
void Measurement2D::SetCovarFromRootFile(std::string covfile,
                                         std::string histname) {
  //********************************************************************

  NUIS_LOG(SAM,
           "Reading covariance from text file: " << covfile << ";" << histname);
  fFullCovar = StatUtils::GetCovarFromRootFile(covfile, histname);
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}

//********************************************************************
void Measurement2D::SetCovarInvertFromTextFile(std::string covfile, int dim) {
  //********************************************************************

  if (dim == -1) {
    dim = this->GetNDOF();
  }

  NUIS_LOG(SAM, "Reading inverted covariance from text file: " << covfile);
  covar = StatUtils::GetCovarFromTextFile(covfile, dim);
  fFullCovar = StatUtils::GetInvert(covar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}

//********************************************************************
void Measurement2D::SetCovarInvertFromRootFile(std::string covfile,
                                               std::string histname) {
  //********************************************************************

  NUIS_LOG(SAM, "Reading inverted covariance from text file: " << covfile << ";"
                                                               << histname);
  covar = StatUtils::GetCovarFromRootFile(covfile, histname);
  fFullCovar = StatUtils::GetInvert(covar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}

//********************************************************************
void Measurement2D::SetCorrelationFromTextFile(std::string covfile, int dim) {
  //********************************************************************

  if (dim == -1)
    dim = this->GetNDOF();
  NUIS_LOG(SAM, "Reading data correlations from text file: " << covfile << ";"
                                                             << dim);
  TMatrixDSym *correlation = StatUtils::GetCovarFromTextFile(covfile, dim);

  if (!fDataHist) {
    NUIS_ABORT("Trying to set correlations from text file but there is no "
               "data to build it from. \n"
               << "In constructor make sure data is set before "
                  "SetCorrelationFromTextFile is called. \n");
  }

  // Fill covar from data errors and correlations
  fFullCovar = new TMatrixDSym(dim);
  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    for (int j = 0; j < fDataHist->GetNbinsX(); j++) {
      (*fFullCovar)(i, j) = (*correlation)(i, j) *
                            fDataHist->GetBinError(i + 1) *
                            fDataHist->GetBinError(j + 1) * 1.E76;
    }
  }

  // Fill other covars.
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  delete correlation;
}

//********************************************************************
void Measurement2D::SetCorrelationFromRootFile(std::string covfile,
                                               std::string histname) {
  //********************************************************************

  NUIS_LOG(SAM, "Reading data correlations from text file: " << covfile << ";"
                                                             << histname);
  TMatrixDSym *correlation = StatUtils::GetCovarFromRootFile(covfile, histname);

  if (!fDataHist) {
    NUIS_ABORT("Trying to set correlations from text file but there is no "
               "data to build it from. \n"
               << "In constructor make sure data is set before "
                  "SetCorrelationFromTextFile is called. \n");
  }

  // Fill covar from data errors and correlations
  fFullCovar = new TMatrixDSym(fDataHist->GetNbinsX());
  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    for (int j = 0; j < fDataHist->GetNbinsX(); j++) {
      (*fFullCovar)(i, j) = (*correlation)(i, j) *
                            fDataHist->GetBinError(i + 1) *
                            fDataHist->GetBinError(j + 1) * 1.E76;
    }
  }

  // Fill other covars.
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  delete correlation;
}

//********************************************************************
void Measurement2D::SetCholDecompFromTextFile(std::string covfile, int dim) {
  //********************************************************************

  if (dim == -1) {
    dim = this->GetNDOF();
  }

  NUIS_LOG(SAM, "Reading cholesky from text file: " << covfile << " " << dim);
  TMatrixD *temp = StatUtils::GetMatrixFromTextFile(covfile, dim, dim);

  TMatrixD *trans = (TMatrixD *)temp->Clone();
  trans->T();
  (*trans) *= (*temp);

  fFullCovar = new TMatrixDSym(dim, trans->GetMatrixArray(), "");
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  delete temp;
  delete trans;
}

//********************************************************************
void Measurement2D::SetCholDecompFromRootFile(std::string covfile,
                                              std::string histname) {
  //********************************************************************

  NUIS_LOG(SAM, "Reading cholesky decomp from root file: " << covfile << ";"
                                                           << histname);
  TMatrixD *temp = StatUtils::GetMatrixFromRootFile(covfile, histname);

  TMatrixD *trans = (TMatrixD *)temp->Clone();
  trans->T();
  (*trans) *= (*temp);

  fFullCovar = new TMatrixDSym(temp->GetNrows(), trans->GetMatrixArray(), "");
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  delete temp;
  delete trans;
}

void Measurement2D::SetShapeCovar() {

  // Return if this is missing any pre-requisites
  if (!fFullCovar)
    return;
  if (!fDataHist)
    return;

  // Also return if it's bloody stupid under the circumstances
  if (fIsDiag)
    return;

  fShapeCovar =
      StatUtils::ExtractShapeOnlyCovar(fFullCovar, fDataHist, fMapHist);
  return;
}

//********************************************************************
void Measurement2D::ScaleData(double scale) {
  //********************************************************************
  fDataHist->Scale(scale);
}

//********************************************************************
void Measurement2D::ScaleDataErrors(double scale) {
  //********************************************************************
  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    for (int j = 0; j < fDataHist->GetNbinsY(); j++) {
      fDataHist->SetBinError(i + 1, j + 1,
                             fDataHist->GetBinError(i + 1, j + 1) * scale);
    }
  }
}

//********************************************************************
void Measurement2D::ScaleCovar(double scale) {
  //********************************************************************
  (*fFullCovar) *= scale;
  (*covar) *= 1.0 / scale;
  (*fDecomp) *= sqrt(scale);
}

//********************************************************************
void Measurement2D::SetBinMask(std::string maskfile) {
  //********************************************************************

  if (!fIsMask)
    return;
  NUIS_LOG(SAM, "Reading bin mask from file: " << maskfile);

  // Create a mask histogram with dim of data
  int nbinsx = fDataHist->GetNbinsX();
  int nbinxy = fDataHist->GetNbinsY();
  fMaskHist = new TH2I((fSettings.GetName() + "_BINMASK").c_str(),
                       (fSettings.GetName() + "_BINMASK; Bin; Mask?").c_str(),
                       nbinsx, 0, nbinsx, nbinxy, 0, nbinxy);
  std::string line;
  std::ifstream mask(maskfile.c_str(), std::ifstream::in);

  if (!mask.is_open()) {
    NUIS_LOG(FTL, " Cannot find mask file.");
    throw;
  }

  while (std::getline(mask >> std::ws, line, '\n')) {
    std::vector<int> entries = GeneralUtils::ParseToInt(line, " ");

    // Skip lines with poorly formatted lines
    if (entries.size() < 2) {
      NUIS_LOG(WRN,
               "Measurement2D::SetBinMask(), couldn't parse line: " << line);
      continue;
    }

    // The first index should be the bin number, the second should be the mask
    // value.
    int val = 0;
    if (entries[2] > 0)
      val = 1;
    fMaskHist->SetBinContent(entries[0], entries[1], val);
  }

  // Apply masking by setting masked data bins to zero
  PlotUtils::MaskBins(fDataHist, fMaskHist);

  return;
}

//********************************************************************
void Measurement2D::FinaliseMeasurement() {
  //********************************************************************

  NUIS_LOG(SAM, "Finalising Measurement: " << fName);
  if (fSettings.GetB("onlymc")) {
    if (fDataHist)
      delete fDataHist;
    fDataHist = new TH2D("empty_data", "empty_data", 1, 0.0, 1.0, 1, 0.0, 1.0);
  }
  // Make sure data is setup
  if (!fDataHist) {
    NUIS_ABORT("No data has been setup inside " << fName << " constructor!");
  }

  // Make sure covariances are setup
  if (!fFullCovar) {
    fIsDiag = true;
    SetCovarFromDiagonal(fDataHist);
  } else if (fIsDiag) { // Have covariance but also set Diag
    NUIS_LOG(SAM, "Have full covariance for sample "
                      << GetName()
                      << " but only using diagonal elements for likelihood");
    size_t nbins = fFullCovar->GetNcols();
    for (int i = 0; i < nbins; ++i) {
      for (int j = 0; j < nbins; ++j) {
        if (i != j) {
          (*fFullCovar)[i][j] = 0;
        }
      }
    }
    delete covar;
    covar = NULL;
    delete fDecomp;
    fDecomp = NULL;
  }

  if (!covar) {
    covar = StatUtils::GetInvert(fFullCovar);
  }

  if (!fDecomp) {
    fDecomp = StatUtils::GetDecomp(fFullCovar);
  }

  // If shape only, set covar and fDecomp using the shape-only matrix (if set)
  if (fIsShape && fShapeCovar && FitPar::Config().GetParB("UseShapeCovar")) {
    if (covar)
      delete covar;
    covar = StatUtils::GetInvert(fShapeCovar);
    if (fDecomp)
      delete fDecomp;
    fDecomp = StatUtils::GetDecomp(fFullCovar);

    fUseShapeNormDecomp = FitPar::Config().GetParB("UseShapeNormDecomp");
    if (fUseShapeNormDecomp) {
      fNormError = 0;

      // From https://arxiv.org/pdf/2003.00088.pdf
      for (int i = 0; i < fFullCovar->GetNcols(); ++i) {
        for (int j = 0; j < fFullCovar->GetNcols(); ++j) {
          fNormError += (*fFullCovar)[i][j];
        }
      }

      NUIS_LOG(SAM, "Sample: " << fName
                               << ", using shape/norm decomp with norm error: "
                               << fNormError);
    }
  }

  // Setup fMCHist from data
  fMCHist = (TH2D *)fDataHist->Clone();
  fMCHist->SetNameTitle((fSettings.GetName() + "_MC").c_str(),
                        (fSettings.GetFullTitles()).c_str());
  fMCHist->Reset();

  // Setup fMCFine
  fMCFine = new TH2D(
      "mcfine", "mcfine", fDataHist->GetNbinsX() * 6,
      fMCHist->GetXaxis()->GetBinLowEdge(1),
      fMCHist->GetXaxis()->GetBinLowEdge(fDataHist->GetNbinsX() + 1),
      fDataHist->GetNbinsY() * 6, fMCHist->GetYaxis()->GetBinLowEdge(1),
      fMCHist->GetYaxis()->GetBinLowEdge(fDataHist->GetNbinsY() + 1));

  fMCFine->SetNameTitle((fSettings.GetName() + "_MC_FINE").c_str(),
                        (fSettings.GetFullTitles()).c_str());
  fMCFine->Reset();

  // Setup MC Stat
  fMCStat = (TH2D *)fMCHist->Clone();
  fMCStat->Reset();

  // Search drawopts for possible types to include by default
  std::string drawopts = FitPar::Config().GetParS("drawopts");
  if (drawopts.find("MODES") != std::string::npos) {
    fMCHist_Modes = new TrueModeStack((fSettings.GetName() + "_MODES").c_str(),
                                      ("True Channels"), fMCHist);
    SetAutoProcessTH1(fMCHist_Modes);
  }

  if (fSettings.Has("maskfile") && fSettings.Has("maskhist")) {
    fMaskHist = PlotUtils::GetTH2FromRootFile<TH2I>(fSettings.GetS("maskfile"),
                                                    fSettings.GetS("maskhist"));
    fIsMask = bool(fMaskHist);
    NUIS_LOG(SAM, "Loaded mask histogram: " << fSettings.GetS("maskhist")
                                            << " from "
                                            << fSettings.GetS("maskfile"));
  } else if (fIsMask) { // Setup bin masks using sample name

    std::string curname = fName;
    std::string origname = fSettings.GetS("originalname");

    // Check rename.mask
    std::string maskloc = FitPar::Config().GetParDIR(curname + ".mask");

    // Check origname.mask
    if (maskloc.empty())
      maskloc = FitPar::Config().GetParDIR(origname + ".mask");

    // Check database
    if (maskloc.empty()) {
      maskloc = FitPar::GetDataBase() + "/masks/" + origname + ".mask";
    }

    // Setup Bin Mask
    SetBinMask(maskloc);
  }

  if (fScaleFactor < 0) {
    NUIS_ERR(FTL, "I found a negative fScaleFactor in " << __FILE__ << ":"
                                                        << __LINE__);
    NUIS_ERR(FTL, "fScaleFactor = " << fScaleFactor);
    NUIS_ABORT("EXITING");
  }

  if (fAddNormPen) {
    if (!fUseShapeNormDecomp) {
      fNormError = fSettings.GetNormError();
    }
    if (fNormError <= 0.0) {
      NUIS_ERR(FTL, "Norm error for class " << fName << " is 0.0!");
      NUIS_ABORT("If you want to use it please add fNormError=VAL");
    }
  }

  // Create and fill Weighted Histogram
  if (!fMCWeighted) {

    fMCWeighted = (TH2D *)fMCHist->Clone();
    fMCWeighted->SetNameTitle((fName + "_MCWGHTS").c_str(),
                              (fName + "_MCWGHTS" + fPlotTitles).c_str());
    fMCWeighted->GetYaxis()->SetTitle("Weighted Events");
  }

  if (!fMapHist)
    fMapHist = StatUtils::GenerateMap(fDataHist);
}

//********************************************************************
void Measurement2D::SetFitOptions(std::string opt) {
  //********************************************************************

  // Do nothing if default given
  if (opt == "DEFAULT")
    return;

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
        NUIS_ABORT(
            "ERROR: Conflicting fit options provided: "
            << opt << std::endl
            << "Conflicting group = " << fit_option_section.at(i) << std::endl
            << "You should only supply one of these options in card file.");
      }
    }
  }

  // Check all options are allowed
  std::vector<std::string> fit_options_input =
      GeneralUtils::ParseToStr(opt, "/");
  for (UInt_t i = 0; i < fit_options_input.size(); i++) {
    if (fAllowedTypes.find(fit_options_input.at(i)) == std::string::npos) {
      NUIS_ERR(FTL, "ERROR: Fit Option '"
                        << fit_options_input.at(i)
                        << "' Provided is not allowed for this measurement.");
      NUIS_ERR(FTL, "Fit Options should be provided as a '/' seperated list "
                    "(e.g. FREE/DIAG/NORM)");
      NUIS_ABORT("Available options for " << fName << " are '" << fAllowedTypes
                                          << "'");
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
  if (opt.find("LOG") != std::string::npos) {
    fIsChi2 = false;

    NUIS_ERR(FTL, "No other LIKELIHOODS properly supported!");
    NUIS_ABORT("Try to use a chi2!");

  } else {
    fIsChi2 = true;
  }

  // EXTRAS
  if (opt.find("RAW") != std::string::npos)
    fIsRawEvents = true;
  if (opt.find("DIF") != std::string::npos)
    fIsDifXSec = true;
  if (opt.find("ENU1D") != std::string::npos)
    fIsEnu1D = true;
  if (opt.find("NORM") != std::string::npos)
    fAddNormPen = true;
  if (opt.find("MASK") != std::string::npos)
    fIsMask = true;

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
  if (opt.find("RAW") != std::string::npos)
    fIsRawEvents = true;
  if (opt.find("DIF") != std::string::npos)
    fIsDifXSec = true;
  if (opt.find("ENU1D") != std::string::npos)
    fIsEnu = true;
  if (opt.find("NORM") != std::string::npos)
    fAddNormPen = true;
  if (opt.find("MASK") != std::string::npos)
    fIsMask = true;

  fIsProjFitX = (opt.find("FITPROJX") != std::string::npos);
  fIsProjFitY = (opt.find("FITPROJY") != std::string::npos);

  return;
};

/*
   Reconfigure LOOP
*/
//********************************************************************
void Measurement2D::ResetAll() {
  //********************************************************************

  fMCHist->Reset();
  fMCFine->Reset();
  fMCStat->Reset();

  return;
};

//********************************************************************
void Measurement2D::FillHistograms() {
  //********************************************************************

  if (Signal) {
    fMCHist->Fill(fXVar, fYVar, Weight);
    fMCFine->Fill(fXVar, fYVar, Weight);
    fMCStat->Fill(fXVar, fYVar, 1.0);

    if (fMCHist_Modes)
      fMCHist_Modes->Fill(Mode, fXVar, fYVar, Weight);
  }

  return;
};

//********************************************************************
void Measurement2D::ScaleEvents() {
  //********************************************************************

  // Fill MCWeighted;
  // for (int i = 0; i < fMCHist->GetNbinsX(); i++) {
  // fMCWeighted->SetBinContent(i + 1, fMCHist->GetBinContent(i + 1));
  // fMCWeighted->SetBinError(i + 1,   fMCHist->GetBinError(i + 1));
  // }

  // Setup Stat ratios for MC and MC Fine
  double *statratio = new double[fMCHist->GetNbinsX()];
  for (int i = 0; i < fMCHist->GetNbinsX(); i++) {
    if (fMCHist->GetBinContent(i + 1) != 0) {
      statratio[i] =
          fMCHist->GetBinError(i + 1) / fMCHist->GetBinContent(i + 1);
    } else {
      statratio[i] = 0.0;
    }
  }

  double *statratiofine = new double[fMCFine->GetNbinsX()];
  for (int i = 0; i < fMCFine->GetNbinsX(); i++) {
    if (fMCFine->GetBinContent(i + 1) != 0) {
      statratiofine[i] =
          fMCFine->GetBinError(i + 1) / fMCFine->GetBinContent(i + 1);
    } else {
      statratiofine[i] = 0.0;
    }
  }

  // Scaling for raw event rates
  if (fIsRawEvents) {
    double datamcratio = fDataHist->Integral() / fMCHist->Integral();

    fMCHist->Scale(datamcratio);
    fMCFine->Scale(datamcratio);

    if (fMCHist_Modes)
      fMCHist_Modes->Scale(datamcratio);

    // Scaling for XSec as function of Enu
  } else if (fIsEnu1D) {

    PlotUtils::FluxUnfoldedScaling(fMCHist, GetFluxHistogram(),
                                   GetEventHistogram(), fScaleFactor);

    PlotUtils::FluxUnfoldedScaling(fMCFine, GetFluxHistogram(),
                                   GetEventHistogram(), fScaleFactor);

    // if (fMCHist_Modes) {
    // PlotUtils::FluxUnfoldedScaling(fMCHist_Modes, GetFluxHistogram(),
    // GetEventHistogram(), fScaleFactor,
    // fNEvents);
    // }

    // Any other differential scaling
  } else {
    fMCHist->Scale(fScaleFactor, "width");
    fMCFine->Scale(fScaleFactor, "width");

    // if (fMCHist_Modes) fMCHist_Modes->Scale(fScaleFactor, "width");
  }

  // Proper error scaling - ROOT Freaks out with xsec weights sometimes
  for (int i = 0; i < fMCStat->GetNbinsX(); i++) {
    fMCHist->SetBinError(i + 1, fMCHist->GetBinContent(i + 1) * statratio[i]);
  }

  for (int i = 0; i < fMCFine->GetNbinsX(); i++) {
    fMCFine->SetBinError(i + 1,
                         fMCFine->GetBinContent(i + 1) * statratiofine[i]);
  }

  // Clean up
  delete statratio;
  delete statratiofine;

  return;
};

//********************************************************************
void Measurement2D::ApplyNormScale(double norm) {
  //********************************************************************

  fCurrentNorm = norm;

  fMCHist->Scale(1.0 / norm);
  fMCFine->Scale(1.0 / norm);

  return;
};

/*
   Statistic Functions - Outsources to StatUtils
*/

//********************************************************************
int Measurement2D::GetNDOF() {
  //********************************************************************

  // Just incase it has gone...
  if (!fDataHist)
    return -1;

  int nDOF = 0;

  // If datahist has no errors make sure we don't include those bins as they are
  // not data points
  for (int xBin = 0; xBin < fDataHist->GetNbinsX(); ++xBin) {
    for (int yBin = 0; yBin < fDataHist->GetNbinsY(); ++yBin) {
      if (fDataHist->GetBinError(xBin + 1, yBin + 1) != 0)
        ++nDOF;
    }
  }

  // Account for possible bin masking
  int nMasked = 0;
  if (fMaskHist and fIsMask)
    if (fMaskHist->Integral() > 0)
      for (int xBin = 0; xBin < fMaskHist->GetNbinsX() + 1; ++xBin)
        for (int yBin = 0; yBin < fMaskHist->GetNbinsY() + 1; ++yBin)
          if (fMaskHist->GetBinContent(xBin, yBin) > 0.5)
            ++nMasked;

  // Take away those masked DOF
  if (fIsMask) {
    nDOF -= nMasked;
  }

  return nDOF;
}

//********************************************************************
double Measurement2D::GetLikelihood() {
  //********************************************************************

  // If this is for a ratio, there is no data histogram to compare to!
  if (fNoData || !fDataHist)
    return 0.;

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

  //  if (fIsProjFitX or fIsProjFitY) return GetProjectedChi2();

  // Scale up the results to match each other (Not using width might be
  // inconsistent with Meas1D)
  double scaleF = fDataHist->Integral() / fMCHist->Integral();
  if (fIsShape) {
    fMCHist->Scale(scaleF);
    fMCFine->Scale(scaleF);
    // PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG, scaleF);
  }

  if (!fMapHist) {
    fMapHist = StatUtils::GenerateMap(fDataHist);
  }

  // Get the chi2 from either covar or diagonals
  double chi2 = 0.0;

  if (fIsChi2) {
    if (fIsDiag) {
      chi2 =
          StatUtils::GetChi2FromDiag(fDataHist, fMCHist, fMapHist, fMaskHist);
    } else {
      chi2 = StatUtils::GetChi2FromCov(fDataHist, fMCHist, covar, fMapHist,
                                       fMaskHist,
                                       fIsWriting ? fResidualHist : NULL);
      if (fChi2LessBinHist && fIsWriting) {
        NUIS_LOG(SAM, "Building n-1 chi2 contribution plot for " << GetName());
        for (int xi = 0; xi < fDataHist->GetNbinsX(); ++xi) {
          for (int yi = 0; yi < fDataHist->GetNbinsY(); ++yi) {
            TH2I *binmask =
                fMaskHist
                    ? static_cast<TH2I *>(fMaskHist->Clone("mask"))
                    : new TH2I("mask", "", fDataHist->GetNbinsX(), 0,
                               fDataHist->GetNbinsX(), fDataHist->GetNbinsY(),
                               0, fDataHist->GetNbinsY());
            binmask->SetDirectory(NULL);

            binmask->SetBinContent(xi + 1, yi + 1, 1);
            fChi2LessBinHist->SetBinContent(
                xi + 1, yi + 1,
                StatUtils::GetChi2FromCov(fDataHist, fMCHist, covar, fMapHist,
                                          binmask));
            delete binmask;
          }
        }
      }
    }
  }

  // Add a normal penalty term
  if (fAddNormPen) {
    if (fUseShapeNormDecomp) { // if shape norm, then add the norm penalty from
                               // https://arxiv.org/pdf/2003.00088.pdf

      TH2 *masked_data = StatUtils::ApplyHistogramMasking(fDataHist, fMaskHist);
      TH2 *masked_mc = StatUtils::ApplyHistogramMasking(fMCHist, fMaskHist);
      masked_mc->Scale(scaleF);

      NUIS_LOG(REC, "ShapeNormDecomp: mcinteg: "
                           << masked_mc->Integral() * 1E38
                           << ", datainteg: " << masked_data->Integral() * 1E38
                           << ", normerror: " << fNormError);

      double normpen =
          std::pow((masked_data->Integral() - masked_mc->Integral()) * 1E38,
                   2) /
          fNormError;

      masked_data->SetDirectory(NULL);
      delete masked_data;
      masked_mc->SetDirectory(NULL);
      delete masked_mc;

      NUIS_LOG(REC, "Using Shape/Norm decomposition: Norm penalty "
                        << normpen << " on shape penalty of " << chi2);
      chi2 += normpen;

    } else {

      chi2 += (1 - (fCurrentNorm)) * (1 - (fCurrentNorm)) /
              (fNormError * fNormError);
      NUIS_LOG(SAM, "Norm penalty = " << (1 - (fCurrentNorm)) *
                                             (1 - (fCurrentNorm)) /
                                             (fNormError * fNormError));
    }
  }

  // Adjust the shape back to where it was.
  if (fIsShape and !FitPar::Config().GetParB("saveshapescaling")) {
    fMCHist->Scale(1. / scaleF);
    fMCFine->Scale(1. / scaleF);
  }

  fLikelihood = chi2;

  return chi2;
}

/*
  Fake Data Functions
*/
//********************************************************************
void Measurement2D::SetFakeDataValues(std::string fakeOption) {
  //********************************************************************

  // Setup original/datatrue
  TH2D *tempdata = (TH2D *)fDataHist->Clone();

  if (!fIsFakeData) {
    fIsFakeData = true;

    // Make a copy of the original data histogram.
    if (!fDataOrig)
      fDataOrig = (TH2D *)fDataHist->Clone((fName + "_data_original").c_str());

  } else {
    ResetFakeData();
  }

  // Setup Inputs
  fFakeDataInput = fakeOption;
  NUIS_LOG(SAM, "Setting fake data from : " << fFakeDataInput);

  // From MC
  if (fFakeDataInput.compare("MC") == 0) {
    fDataHist = (TH2D *)fMCHist->Clone((fName + "_MC").c_str());

    // Fake File
  } else {
    if (!fFakeDataFile)
      fFakeDataFile = new TFile(fFakeDataInput.c_str(), "READ");
    fDataHist = (TH2D *)fFakeDataFile->Get((fName + "_MC").c_str());
  }

  // Setup Data Hist
  fDataHist->SetNameTitle((fName + "_FAKE").c_str(),
                          (fName + fPlotTitles).c_str());

  // Replace Data True
  if (fDataTrue)
    delete fDataTrue;
  fDataTrue = (TH2D *)fDataHist->Clone();
  fDataTrue->SetNameTitle((fName + "_FAKE_TRUE").c_str(),
                          (fName + fPlotTitles).c_str());

  // Make a new covariance for fake data hist.
  int nbins = fDataHist->GetNbinsX() * fDataHist->GetNbinsY();
  double alpha_i = 0.0;
  double alpha_j = 0.0;

  for (int i = 0; i < nbins; i++) {
    for (int j = 0; j < nbins; j++) {
      if (tempdata->GetBinContent(i + 1) && tempdata->GetBinContent(j + 1)) {
        alpha_i =
            fDataHist->GetBinContent(i + 1) / tempdata->GetBinContent(i + 1);
        alpha_j =
            fDataHist->GetBinContent(j + 1) / tempdata->GetBinContent(j + 1);
      } else {
        alpha_i = 0.0;
        alpha_j = 0.0;
      }

      (*fFullCovar)(i, j) = alpha_i * alpha_j * (*fFullCovar)(i, j);
    }
  }

  // Setup Covariances
  if (covar)
    delete covar;
  covar = StatUtils::GetInvert(fFullCovar);

  if (fDecomp)
    delete fDecomp;
  fDecomp = StatUtils::GetInvert(fFullCovar);

  delete tempdata;

  return;
};

//********************************************************************
void Measurement2D::ResetFakeData() {
  //********************************************************************

  if (fIsFakeData) {
    if (fDataHist)
      delete fDataHist;
    fDataHist =
        (TH2D *)fDataTrue->Clone((fSettings.GetName() + "_FKDAT").c_str());
  }
}

//********************************************************************
void Measurement2D::ResetData() {
  //********************************************************************

  if (fIsFakeData) {
    if (fDataHist)
      delete fDataHist;
    fDataHist =
        (TH2D *)fDataOrig->Clone((fSettings.GetName() + "_data").c_str());
  }

  fIsFakeData = false;
}

//********************************************************************
void Measurement2D::ThrowCovariance() {
  //********************************************************************

  // Take a fDecomposition and use it to throw the current dataset.
  // Requires fDataTrue also be set incase used repeatedly.

  if (fDataHist)
    delete fDataHist;
  fDataHist = StatUtils::ThrowHistogram(fDataTrue, fFullCovar);

  return;
};

//********************************************************************
void Measurement2D::ThrowDataToy() {
  //********************************************************************
  if (!fDataTrue)
    fDataTrue = (TH2D *)fDataHist->Clone();
  if (fMCHist)
    delete fMCHist;
  fMCHist = StatUtils::ThrowHistogram(fDataTrue, fFullCovar);
}

/*
   Access Functions
*/

//********************************************************************
TH2D *Measurement2D::GetMCHistogram() {
  //********************************************************************

  if (!fMCHist)
    return fMCHist;

  std::ostringstream chi2;
  chi2 << std::setprecision(5) << this->GetLikelihood();

  int linecolor = kRed;
  int linestyle = 1;
  int linewidth = 1;

  int fillcolor = 0;
  int fillstyle = 1001;

  if (fSettings.Has("linecolor"))
    linecolor = fSettings.GetI("linecolor");
  if (fSettings.Has("linestyle"))
    linestyle = fSettings.GetI("linestyle");
  if (fSettings.Has("linewidth"))
    linewidth = fSettings.GetI("linewidth");

  if (fSettings.Has("fillcolor"))
    fillcolor = fSettings.GetI("fillcolor");
  if (fSettings.Has("fillstyle"))
    fillstyle = fSettings.GetI("fillstyle");

  fMCHist->SetTitle(chi2.str().c_str());

  fMCHist->SetLineColor(linecolor);
  fMCHist->SetLineStyle(linestyle);
  fMCHist->SetLineWidth(linewidth);

  fMCHist->SetFillColor(fillcolor);
  fMCHist->SetFillStyle(fillstyle);

  return fMCHist;
};

//********************************************************************
TH2D *Measurement2D::GetDataHistogram() {
  //********************************************************************

  if (!fDataHist)
    return fDataHist;

  int datacolor = kBlack;
  int datastyle = 1;
  int datawidth = 1;

  if (fSettings.Has("datacolor"))
    datacolor = fSettings.GetI("datacolor");
  if (fSettings.Has("datastyle"))
    datastyle = fSettings.GetI("datastyle");
  if (fSettings.Has("datawidth"))
    datawidth = fSettings.GetI("datawidth");

  fDataHist->SetLineColor(datacolor);
  fDataHist->SetLineWidth(datawidth);
  fDataHist->SetMarkerStyle(datastyle);

  return fDataHist;
};

/*
   Write Functions
*/

// Save all the histograms at once
//********************************************************************
void Measurement2D::Write(std::string drawOpt) {
  //********************************************************************

  // Get Draw Options
  drawOpt = FitPar::Config().GetParS("drawopts");

  // Write Settigns
  if (drawOpt.find("SETTINGS") != std::string::npos) {
    fSettings.Set("#chi^{2}", fLikelihood);
    fSettings.Set("NDOF", this->GetNDOF());
    fSettings.Set("#chi^{2}/NDOF", fLikelihood / this->GetNDOF());
    fSettings.Write();
  }

  // // Likelihood residual plots
  // if (drawOpt.find("RESIDUAL") != std::string::npos) {
  // WriteResidualPlots();
  //}

  // // RATIO
  // if (drawOpt.find("CANVMC") != std::string::npos) {
  //   TCanvas* c1 = WriteMCCanvas(fDataHist, fMCHist);
  //   c1->Write();
  //   delete c1;
  // }

  // // PDG
  // if (drawOpt.find("CANVPDG") != std::string::npos && fMCHist_Modes) {
  //   TCanvas* c2 = WritePDGCanvas(fDataHist, fMCHist, fMCHist_Modes);
  //   c2->Write();
  //   delete c2;
  // }

  /// 2D VERSION
  // If null pointer return
  if (!fMCHist and !fDataHist) {
    NUIS_LOG(SAM, fName << "Incomplete histogram set!");
    return;
  }

  //  Config::Get().out->cd();

  // Get Draw Options
  drawOpt = FitPar::Config().GetParS("drawopts");
  bool drawData = (drawOpt.find("DATA") != std::string::npos);
  bool drawNormal = (drawOpt.find("MC") != std::string::npos);
  bool drawEvents = (drawOpt.find("EVT") != std::string::npos);
  bool drawFine = (drawOpt.find("FINE") != std::string::npos);
  bool drawRatio = (drawOpt.find("RATIO") != std::string::npos);
  // bool drawModes = (drawOpt.find("MODES") != std::string::npos);
  bool drawShape = (drawOpt.find("SHAPE") != std::string::npos);
  bool residual = (drawOpt.find("RESIDUAL") != std::string::npos);
  bool drawMatrix = (drawOpt.find("MATRIX") != std::string::npos);
  bool drawFlux = (drawOpt.find("FLUX") != std::string::npos);
  bool drawMask = (drawOpt.find("MASK") != std::string::npos);
  bool drawMap = (drawOpt.find("MAP") != std::string::npos);
  bool drawProj = (drawOpt.find("PROJ") != std::string::npos);
  // bool drawCanvPDG = (drawOpt.find("CANVPDG") != std::string::npos);
  bool drawCov = (drawOpt.find("COV") != std::string::npos);
  bool drawSliceMC = (drawOpt.find("CANVSLICEMC") != std::string::npos);
  bool drawWeighted =
      (drawOpt.find("WEIGHTS") != std::string::npos && fMCWeighted);

  if (FitPar::Config().GetParB("EventManager")) {
    drawFlux = false;
    drawEvents = false;
  }

  // Save standard plots
  if (drawData) {
    GetDataList().at(0)->Write();
    // Generate a simple map
    if (!fMapHist)
      fMapHist = StatUtils::GenerateMap(fDataHist);
    // Convert to 1D Lists
    TH1D *data_1D = StatUtils::MapToTH1D(fDataHist, fMapHist);
    data_1D->Write();
    delete data_1D;
  }
  if (drawNormal) {
    GetMCList().at(0)->Write();
    if (!fMapHist)
      fMapHist = StatUtils::GenerateMap(fDataHist);
    TH1D *mc_1D = StatUtils::MapToTH1D(fMCHist, fMapHist);
    mc_1D->SetLineColor(kRed);
    mc_1D->Write();
    delete mc_1D;
  }

  if (fIsChi2 && !fIsDiag) {
    fResidualHist = (TH2D *)fMCHist->Clone((fName + "_RESIDUAL").c_str());
    fResidualHist->GetYaxis()->SetTitle("#Delta#chi^{2}");
    fResidualHist->Reset();

    fChi2LessBinHist =
        (TH2D *)fMCHist->Clone((fName + "_Chi2NMinusOne").c_str());
    fChi2LessBinHist->GetYaxis()->SetTitle("Total #chi^{2} without bin_{i}");
    fChi2LessBinHist->Reset();

    fIsWriting = true;
    (void)GetLikelihood();
    fIsWriting = false;

    fResidualHist->Write((fName + "_RESIDUAL").c_str());
    fChi2LessBinHist->Write((fName + "_Chi2NMinusOne").c_str());

    if (fMapHist) {
      TH1D *ResidualHist_1D = StatUtils::MapToTH1D(fResidualHist, fMapHist);
      TH1D *Chi2LessBinHist_1D =
          StatUtils::MapToTH1D(fChi2LessBinHist, fMapHist);
      ResidualHist_1D->Write((fName + "_RESIDUAL_1D").c_str());
      Chi2LessBinHist_1D->Write((fName + "_Chi2NMinusOne_1D").c_str());
    }
  }

  // Write Weighted Histogram
  if (drawWeighted)
    fMCWeighted->Write();

  if (drawCov) {
    TH2D(*fFullCovar).Write((fName + "_COV").c_str());
  }

  if (drawOpt.find("INVCOV") != std::string::npos) {
    TH2D(*covar).Write((fName + "_INVCOV").c_str());
  }

  // Save only mc and data if splines
  if (fEventType == 4 or fEventType == 3) {
    return;
  }

  // Draw Extra plots
  if (drawFine)
    this->GetFineList().at(0)->Write();
  if (drawFlux and GetFluxHistogram()) {
    GetFluxHistogram()->Write();
  }
  if (drawEvents and GetEventHistogram()) {
    GetEventHistogram()->Write();
  }
  if (fIsMask and drawMask) {
    fMaskHist->Write((fName + "_MSK").c_str()); //< save mask
    TH1I *mask_1D = StatUtils::MapToMask(fMaskHist, fMapHist);
    if (mask_1D) {
      mask_1D->Write();

      TMatrixDSym *calc_cov =
          StatUtils::ApplyInvertedMatrixMasking(covar, mask_1D);
      TH1D *data_1D = StatUtils::MapToTH1D(fDataHist, fMapHist);
      TH1D *mc_1D = StatUtils::MapToTH1D(fMCHist, fMapHist);
      TH1D *calc_data = StatUtils::ApplyHistogramMasking(data_1D, mask_1D);
      TH1D *calc_mc = StatUtils::ApplyHistogramMasking(mc_1D, mask_1D);

      TH2D *bin_cov = new TH2D(*calc_cov);

      bin_cov->Write();
      calc_data->Write();
      calc_mc->Write();

      delete mask_1D;
      delete calc_cov;
      delete calc_data;
      delete calc_mc;
      delete bin_cov;
      delete data_1D;
      delete mc_1D;
    }
  }
  if (drawMap)
    fMapHist->Write((fName + "_MAP").c_str()); //< save map

  // // Save neut stack
  // if (drawModes) {
  //   THStack combo_fMCHist_PDG = PlotUtils::GetNeutModeStack(
  //                                 (fName + "_MC_PDG").c_str(),
  //                                 (TH1**)fMCHist_PDG, 0);
  //   combo_fMCHist_PDG.Write();
  // }

  // Save Matrix plots
  if (drawMatrix and fFullCovar and covar and fDecomp) {
    TH2D cov = TH2D((*fFullCovar));
    cov.SetNameTitle((fName + "_cov").c_str(),
                     (fName + "_cov;Bins; Bins;").c_str());
    cov.Write();

    TH2D covinv = TH2D((*this->covar));
    covinv.SetNameTitle((fName + "_covinv").c_str(),
                        (fName + "_cov;Bins; Bins;").c_str());
    covinv.Write();

    TH2D covdec = TH2D((*fDecomp));
    covdec.SetNameTitle((fName + "_covdec").c_str(),
                        (fName + "_cov;Bins; Bins;").c_str());
    covdec.Write();
  }

  // Save ratio plots if required
  if (drawRatio) {
    // Needed for error bars
    for (int i = 0; i < fMCHist->GetNbinsX() * fMCHist->GetNbinsY(); i++)
      fMCHist->SetBinError(i + 1, 0.0);

    fDataHist->GetSumw2();
    fMCHist->GetSumw2();

    // Create Ratio Histograms
    TH2D *dataRatio = (TH2D *)fDataHist->Clone((fName + "_data_RATIO").c_str());
    TH2D *mcRatio = (TH2D *)fMCHist->Clone((fName + "_MC_RATIO").c_str());

    mcRatio->Divide(fMCHist);
    dataRatio->Divide(fMCHist);

    // Cancel bin errors on MC
    for (int i = 0; i < mcRatio->GetNbinsX() * mcRatio->GetNbinsY(); i++) {
      mcRatio->SetBinError(i + 1, fMCHist->GetBinError(i + 1) /
                                      fMCHist->GetBinContent(i + 1));
    }

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
    TH2D *mcShape = (TH2D *)fMCHist->Clone((fName + "_MC_SHAPE").c_str());

    double shapeScale = 1.0;
    if (fIsRawEvents) {
      shapeScale = fDataHist->Integral() / fMCHist->Integral();
    } else {
      shapeScale = fDataHist->Integral("width") / fMCHist->Integral("width");
    }

    mcShape->Scale(shapeScale);

    mcShape->SetLineWidth(3);
    mcShape->SetLineStyle(7); // dashes

    mcShape->Write();

    // Save shape ratios
    if (drawRatio) {
      // Needed for error bars
      mcShape->GetSumw2();

      // Create shape ratio histograms
      TH2D *mcShapeRatio =
          (TH2D *)mcShape->Clone((fName + "_MC_SHAPE_RATIO").c_str());
      TH2D *dataShapeRatio =
          (TH2D *)fDataHist->Clone((fName + "_data_SHAPE_RATIO").c_str());

      // Divide the histograms
      mcShapeRatio->Divide(mcShape);
      dataShapeRatio->Divide(mcShape);

      // Colour the shape ratio plots
      mcShapeRatio->SetLineWidth(3);
      mcShapeRatio->SetLineStyle(7); // dashes

      mcShapeRatio->Write();
      dataShapeRatio->Write();

      delete mcShapeRatio;
      delete dataShapeRatio;
    }

    delete mcShape;
  }

  // Save residual calculations of what contributed to the chi2 values.
  if (residual) {
  }

  if (fIsProjFitX || fIsProjFitY || drawProj) {
    // If not already made, make the projections
    if (!fMCHist_X) {
      PlotUtils::MatchEmptyBins(fDataHist, fMCHist);

      fMCHist_X = PlotUtils::GetProjectionX(fMCHist, fMaskHist);
      fMCHist_Y = PlotUtils::GetProjectionY(fMCHist, fMaskHist);

      fDataHist_X = PlotUtils::GetProjectionX(fDataHist, fMaskHist);
      fDataHist_Y = PlotUtils::GetProjectionY(fDataHist, fMaskHist);

      // This is not the correct way of doing it
      // double chi2X = StatUtils::GetChi2FromDiag(fDataHist_X, fMCHist_X);
      // double chi2Y = StatUtils::GetChi2FromDiag(fDataHist_Y, fMCHist_Y);

      // fMCHist_X->SetTitle(Form("%f", chi2X));
      // fMCHist_Y->SetTitle(Form("%f", chi2Y));
    }

    // Save the histograms
    fDataHist_X->Write();
    fMCHist_X->Write();

    fDataHist_Y->Write();
    fMCHist_Y->Write();
  }

  if (drawSliceMC) {
    TCanvas *c1 = new TCanvas((fName + "_MC_CANV_Y").c_str(),
                              (fName + "_MC_CANV_Y").c_str(), 1024, 1024);

    c1->Divide(2, int(fDataHist->GetNbinsY() / 3. + 1));
    TH2D *mcShape = (TH2D *)fMCHist->Clone((fName + "_MC_SHAPE").c_str());
    double shapeScale =
        fDataHist->Integral("width") / fMCHist->Integral("width");
    mcShape->Scale(shapeScale);
    mcShape->SetLineStyle(7);

    c1->cd(1);
    TLegend *leg = new TLegend(0.0, 0.0, 1.0, 1.0);
    leg->AddEntry(fDataHist, (fName + " Data").c_str(), "lep");
    leg->AddEntry(fMCHist, (fName + " MC").c_str(), "l");
    leg->AddEntry(mcShape, (fName + " Shape").c_str(), "l");
    leg->SetLineColor(0);
    leg->SetLineStyle(0);
    leg->SetFillColor(0);
    leg->SetLineStyle(0);
    leg->Draw("SAME");

    // Make Y slices
    for (int i = 1; i < fDataHist->GetNbinsY() + 1; i++) {
      c1->cd(i + 1);
      TH1D *fDataHist_SliceY = PlotUtils::GetSliceY(fDataHist, i);
      fDataHist_SliceY->Draw("E1");

      TH1D *fMCHist_SliceY = PlotUtils::GetSliceY(fMCHist, i);
      fMCHist_SliceY->Draw("SAME");

      TH1D *mcShape_SliceY = PlotUtils::GetSliceY(mcShape, i);
      mcShape_SliceY->Draw("SAME");
      mcShape_SliceY->SetLineStyle(mcShape->GetLineStyle());
    }
    c1->Write();
    delete c1;
    delete leg;

    TCanvas *c2 = new TCanvas((fName + "_MC_CANV_X").c_str(),
                              (fName + "_MC_CANV_X").c_str(), 1024, 1024);

    c2->Divide(2, int(fDataHist->GetNbinsX() / 3. + 1));
    mcShape = (TH2D *)fMCHist->Clone((fName + "_MC_SHAPE").c_str());
    shapeScale = fDataHist->Integral("width") / fMCHist->Integral("width");
    mcShape->Scale(shapeScale);
    mcShape->SetLineStyle(7);

    c2->cd(1);
    TLegend *leg2 = new TLegend(0.0, 0.0, 1.0, 1.0);
    leg2->AddEntry(fDataHist, (fName + " Data").c_str(), "lep");
    leg2->AddEntry(fMCHist, (fName + " MC").c_str(), "l");
    leg2->AddEntry(mcShape, (fName + " Shape").c_str(), "l");
    leg2->SetLineColor(0);
    leg2->SetLineStyle(0);
    leg2->SetFillColor(0);
    leg2->SetLineStyle(0);
    leg2->Draw("SAME");

    // Make Y slices
    for (int i = 1; i < fDataHist->GetNbinsX() + 1; i++) {
      c2->cd(i + 1);
      TH1D *fDataHist_SliceX = PlotUtils::GetSliceX(fDataHist, i);
      fDataHist_SliceX->Draw("E1");

      TH1D *fMCHist_SliceX = PlotUtils::GetSliceX(fMCHist, i);
      fMCHist_SliceX->Draw("SAME");

      TH1D *mcShape_SliceX = PlotUtils::GetSliceX(mcShape, i);
      mcShape_SliceX->Draw("SAME");
      mcShape_SliceX->SetLineStyle(mcShape->GetLineStyle());
    }

    c2->Write();
    delete c2;
    delete leg2;
  }

  // Write Extra Histograms
  AutoWriteExtraTH1();
  WriteExtraHistograms();

  // Returning
  NUIS_LOG(SAM, "Written Histograms: " << fName);
  return;
}

/*
   Setup Functions
   */
//********************************************************************
void Measurement2D::SetupMeasurement(std::string inputfile, std::string type,
                                     FitWeight *rw, std::string fkdt) {
  //********************************************************************

  // Check if name contains Evt, indicating that it is a raw number of events
  // measurements and should thus be treated as once
  fIsRawEvents = false;
  if ((fName.find("Evt") != std::string::npos) && fIsRawEvents == false) {
    fIsRawEvents = true;
    NUIS_LOG(SAM, "Found event rate measurement but fIsRawEvents == false!");
    NUIS_LOG(SAM, "Overriding this and setting fIsRawEvents == true!");
  }

  fIsEnu = false;
  if ((fName.find("XSec") != std::string::npos) &&
      (fName.find("Enu") != std::string::npos)) {
    fIsEnu = true;
    NUIS_LOG(SAM, "::" << fName << "::");
    NUIS_LOG(SAM,
             "Found XSec Enu measurement, applying flux integrated scaling, "
             "not flux averaged!");
    if (FitPar::Config().GetParB("EventManager")) {
      NUIS_ERR(FTL, "Enu Measurements do not yet work with the Event Manager!");
      NUIS_ERR(FTL, "If you want decent flux unfolded results please run in "
                    "series mode (-q EventManager=0)");
      sleep(2);
      throw;
    }
  }

  if (fIsEnu && fIsRawEvents) {
    NUIS_ERR(FTL, "Found 1D Enu XSec distribution AND fIsRawEvents, is this "
                  "really correct?!");
    NUIS_ERR(FTL, "Check experiment constructor for " << fName
                                                      << " and correct this!");
    NUIS_ABORT("I live in " << __FILE__ << ":" << __LINE__);
  }

  // Reset everything to NULL
  fRW = rw;

  // Setting up 2D Inputs
  this->SetupInputs(inputfile);

  // Set Default Options
  SetFitOptions(fDefaultTypes);

  // Set Passed Options
  SetFitOptions(type);
}

//********************************************************************
void Measurement2D::SetupDefaultHist() {
  //********************************************************************

  // Setup fMCHist
  fMCHist = (TH2D *)fDataHist->Clone();
  fMCHist->SetNameTitle((fName + "_MC").c_str(),
                        (fName + "_MC" + fPlotTitles).c_str());

  // Setup fMCFine
  Int_t nBinsX = fMCHist->GetNbinsX();
  Int_t nBinsY = fMCHist->GetNbinsY();
  fMCFine = new TH2D((fName + "_MC_FINE").c_str(),
                     (fName + "_MC_FINE" + fPlotTitles).c_str(), nBinsX * 3,
                     fMCHist->GetXaxis()->GetBinLowEdge(1),
                     fMCHist->GetXaxis()->GetBinLowEdge(nBinsX + 1), nBinsY * 3,
                     fMCHist->GetYaxis()->GetBinLowEdge(1),
                     fMCHist->GetYaxis()->GetBinLowEdge(nBinsY + 1));

  // Setup MC Stat
  fMCStat = (TH2D *)fMCHist->Clone();
  fMCStat->Reset();

  // Setup the NEUT Mode Array
  // PlotUtils::CreateNeutModeArray(fMCHist, (TH1**)fMCHist_PDG);

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
void Measurement2D::SetDataValues(std::string dataFile, std::string TH2Dname) {
  //********************************************************************

  if (dataFile.find(".root") == std::string::npos) {
    NUIS_ERR(FTL, "Error! " << dataFile << " is not a .root file");
    NUIS_ERR(FTL, "Currently only .root file reading is supported (MiniBooNE "
                  "CC1pi+ 2D), but implementing .txt should be dirt easy");
    NUIS_ABORT("See me at " << __FILE__ << ":" << __LINE__);

  } else {
    TFile *inFile = new TFile(dataFile.c_str(), "READ");
    fDataHist = (TH2D *)(inFile->Get(TH2Dname.c_str())->Clone());
    fDataHist->SetDirectory(0);

    fDataHist->SetNameTitle((fName + "_data").c_str(),
                            (fName + "_MC" + fPlotTitles).c_str());

    delete inFile;
  }

  return;
}

//********************************************************************
void Measurement2D::SetDataValues(std::string dataFile, double dataNorm,
                                  std::string errorFile, double errorNorm) {
  //********************************************************************

  // Make a counter to track the line number
  int yBin = 0;

  std::string line;
  std::ifstream data(dataFile.c_str(), std::ifstream::in);

  fDataHist = new TH2D((fName + "_data").c_str(), (fName + fPlotTitles).c_str(),
                       fNDataPointsX - 1, fXBins, fNDataPointsY - 1, fYBins);

  if (data.is_open()) {
    NUIS_LOG(SAM, "Reading data from: " << dataFile.c_str());
  }

  while (std::getline(data >> std::ws, line, '\n')) {
    int xBin = 0;

    // Loop over entries and insert them into the histogram
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++) {
      fDataHist->SetBinContent(xBin + 1, yBin + 1, (*iter) * dataNorm);
      xBin++;
    }
    yBin++;
  }

  yBin = 0;
  std::ifstream error(errorFile.c_str(), std::ifstream::in);

  if (error.is_open()) {
    NUIS_LOG(SAM, "Reading errors from: " << errorFile.c_str());
  }

  while (std::getline(error >> std::ws, line, '\n')) {
    int xBin = 0;

    // Loop over entries and insert them into the histogram
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++) {
      fDataHist->SetBinError(xBin + 1, yBin + 1, (*iter) * errorNorm);
      xBin++;
    }
    yBin++;
  }

  return;
};

//********************************************************************
void Measurement2D::SetDataValuesFromText(std::string dataFile,
                                          double dataNorm) {
  //********************************************************************

  fDataHist = new TH2D((fName + "_data").c_str(), (fName + fPlotTitles).c_str(),
                       fNDataPointsX - 1, fXBins, fNDataPointsY - 1, fYBins);

  NUIS_LOG(SAM, "Reading data from: " << dataFile);
  PlotUtils::Set2DHistFromText(dataFile, fDataHist, dataNorm, true);

  return;
};

//********************************************************************
void Measurement2D::SetCovarMatrix(std::string covarFile) {
  //********************************************************************

  // Used to read a covariance matrix from a root file
  TFile *tempFile = new TFile(covarFile.c_str(), "READ");

  // Make plots that we want
  TH2D *covarPlot = new TH2D();
  TH2D *fFullCovarPlot = new TH2D();

  // Get covariance options for fake data studies
  std::string covName = "";
  std::string covOption = FitPar::Config().GetParS("throw_covariance");

  // Which matrix to get?
  if (fIsShape || fIsFree)
    covName = "shp_";
  if (fIsDiag)
    covName += "diag";
  else
    covName += "full";

  covarPlot = (TH2D *)tempFile->Get((covName + "cov").c_str());

  // Throw either the sub matrix or the full matrix
  if (!covOption.compare("SUB"))
    fFullCovarPlot = (TH2D *)tempFile->Get((covName + "cov").c_str());
  else if (!covOption.compare("FULL"))
    fFullCovarPlot = (TH2D *)tempFile->Get("fullcov");
  else {
    NUIS_ERR(WRN, " Incorrect thrown_covariance option in parameters.");
  }

  // Bin masking?
  int dim = int(fDataHist->GetNbinsX()); //-this->masked->Integral());
  int covdim = int(fDataHist->GetNbinsX());

  // Make new covars
  this->covar = new TMatrixDSym(dim);
  fFullCovar = new TMatrixDSym(dim);
  fDecomp = new TMatrixDSym(dim);

  // Full covariance values
  int row, column = 0;
  row = 0;
  column = 0;
  for (Int_t i = 0; i < covdim; i++) {
    // masking can be dodgy
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
    for (Int_t i = 0; i < fDataHist->GetNbinsX(); i++) {
      fDataHist->SetBinError(
          i + 1, sqrt((covarPlot->GetBinContent(i + 1, i + 1))) * 1E-38);
    }
  }

  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");

  tempFile->Close();
  delete tempFile;

  return;
};

//********************************************************************
void Measurement2D::SetCovarMatrixFromText(std::string covarFile, int dim) {
  //********************************************************************

  // Make a counter to track the line number
  int row = 0;

  std::string line;
  std::ifstream covar(covarFile.c_str(), std::ifstream::in);

  this->covar = new TMatrixDSym(dim);
  fFullCovar = new TMatrixDSym(dim);
  if (covar.is_open()) {
    NUIS_LOG(SAM, "Reading covariance matrix from file: " << covarFile);
  }
  while (std::getline(covar >> std::ws, line, '\n')) {
    int column = 0;

    // Loop over entries and insert them into matrix
    // Multiply by the errors to get the covariance, rather than the correlation
    // matrix
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++) {
      double val = (*iter) * fDataHist->GetBinError(row + 1) * 1E38 *
                   fDataHist->GetBinError(column + 1) * 1E38;
      (*this->covar)(row, column) = val;
      (*fFullCovar)(row, column) = val;

      column++;
    }

    row++;
  }

  // Robust matrix inversion method
  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");

  return;
};

//********************************************************************
void Measurement2D::SetCovarMatrixFromChol(std::string covarFile, int dim) {
  //********************************************************************

  // Make a counter to track the line number
  int row = 0;

  std::string line;
  std::ifstream covarread(covarFile.c_str(), std::ifstream::in);

  TMatrixD *newcov = new TMatrixD(dim, dim);

  if (covarread.is_open()) {
    NUIS_LOG(SAM, "Reading covariance matrix from file: " << covarFile);
  }
  while (std::getline(covarread >> std::ws, line, '\n')) {
    int column = 0;

    // Loop over entries and insert them into matrix
    // Multiply by the errors to get the covariance, rather than the correlation
    // matrix
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++) {
      (*newcov)(row, column) = *iter;
      column++;
    }

    row++;
  }
  covarread.close();

  // Form full covariance
  TMatrixD *trans = (TMatrixD *)(newcov)->Clone();
  trans->T();
  (*trans) *= (*newcov);

  fFullCovar = new TMatrixDSym(dim, trans->GetMatrixArray(), "");

  delete newcov;
  delete trans;

  // Robust matrix inversion method
  TDecompChol LU = TDecompChol(*this->fFullCovar);
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");

  return;
};

// //********************************************************************
// void Measurement2D::SetMapValuesFromText(std::string dataFile) {
// //********************************************************************

//   fMapHist = new TH2I((fName + "_map").c_str(), (fName +
//   fPlotTitles).c_str(),
//                       fNDataPointsX - 1, fXBins, fNDataPointsY - 1, fYBins);

//   LOG(SAM) << "Reading map from: " << dataFile << std::endl;
//   PlotUtils::Set2DHistFromText(dataFile, fMapHist, 1.0);

//   return;
// };
