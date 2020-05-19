// Copyright 2016 L. Pickering, P. Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
 *    This ile is part of NUISANCE.
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

//********************************************************************
Measurement1D::Measurement1D(void) {
  //********************************************************************

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
  fShapeCovar = NULL;

  fCovar = NULL;
  fInvert = NULL;
  fDecomp = NULL;

  fResidualHist = NULL;
  fChi2LessBinHist = NULL;

  // Fake Data
  fFakeDataInput = "";
  fFakeDataFile = NULL;

  // Options
  fDefaultTypes = "FIX/FULL/CHI2";
  fAllowedTypes =
      "FIX,FREE,SHAPE/FULL,DIAG/CHI2/NORM/ENUCORR/Q2CORR/ENU1D/MASK/NOWIDTH";

  fIsFix = false;
  fIsShape = false;
  fIsFree = false;
  fIsDiag = false;
  fIsFull = false;
  fAddNormPen = false;
  fIsMask = false;
  fIsChi2SVD = false;
  fIsRawEvents = false;
  fIsNoWidth = false;
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
Measurement1D::~Measurement1D(void) {
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
  if (fShapeCovar)
    delete fShapeCovar;
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
void Measurement1D::FinaliseSampleSettings() {
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

  if (fSettings.GetS("originalname").find("XSec_1DEnu") != std::string::npos) {
    fIsEnu1D = true;
    NUIS_LOG(SAM, "::" << fName << "::");
    NUIS_LOG(SAM,
             "Found XSec Enu measurement, applying flux integrated scaling, "
                 << "not flux averaged!");
  }

  if (fIsEnu1D && fIsRawEvents) {
    NUIS_ERR(FTL, "Found 1D Enu XSec distribution AND fIsRawEvents, is this "
                  "really correct?!");
    NUIS_ERR(FTL, "Check experiment constructor for " << fName
                                                      << " and correct this!");
    NUIS_ERR(FTL, "I live in " << __FILE__ << ":" << __LINE__);
    throw;
  }

  if (!fRW)
    fRW = FitBase::GetRW();
  if (!fInput and !fIsJoint)
    SetupInputs(fSettings.GetS("input"));

  // Setup options
  SetFitOptions(fDefaultTypes);          // defaults
  SetFitOptions(fSettings.GetS("type")); // user specified

  EnuMin = GeneralUtils::StrToDbl(fSettings.GetS("enu_min"));
  EnuMax = GeneralUtils::StrToDbl(fSettings.GetS("enu_max"));
}

//********************************************************************
void Measurement1D::CreateDataHistogram(int dimx, double *binx) {
  //********************************************************************

  if (fDataHist)
    delete fDataHist;

  fDataHist = new TH1D((fSettings.GetName() + "_data").c_str(),
                       (fSettings.GetFullTitles()).c_str(), dimx, binx);
}

//********************************************************************
void Measurement1D::SetDataFromTextFile(std::string datafile) {
  //********************************************************************

  NUIS_LOG(SAM, "Reading data from text file: " << datafile);
  fDataHist = PlotUtils::GetTH1DFromFile(
      datafile, fSettings.GetName() + "_data", fSettings.GetFullTitles());
}

//********************************************************************
void Measurement1D::SetDataFromRootFile(std::string datafile,
                                        std::string histname) {
  //********************************************************************

  NUIS_LOG(SAM, "Reading data from root file: " << datafile << ";" << histname);
  fDataHist = PlotUtils::GetTH1DFromRootFile(datafile, histname);
  fDataHist->SetNameTitle((fSettings.GetName() + "_data").c_str(),
                          (fSettings.GetFullTitles()).c_str());

  return;
};

//********************************************************************
void Measurement1D::SetEmptyData() {
  //********************************************************************

  fDataHist = new TH1D("EMPTY_DATA", "EMPTY_DATA", 1, 0.0, 1.0);
}

//********************************************************************
void Measurement1D::SetPoissonErrors() {
  //********************************************************************

  if (!fDataHist) {
    NUIS_ERR(FTL, "Need a data hist to setup possion errors! ");
    NUIS_ERR(FTL, "Setup Data First!");
    throw;
  }

  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i + 1, sqrt(fDataHist->GetBinContent(i + 1)));
  }
}

//********************************************************************
void Measurement1D::SetCovarFromDiagonal(TH1D *data) {
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
  //            << "that is not set as diagonal." );
  //   throw;
  // }
}

//********************************************************************
void Measurement1D::SetCovarFromTextFile(std::string covfile, int dim) {
  //********************************************************************

  if (dim == -1) {
    dim = fDataHist->GetNbinsX();
  }

  NUIS_LOG(SAM, "Reading covariance from text file: " << covfile);
  fFullCovar = StatUtils::GetCovarFromTextFile(covfile, dim);
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}

//********************************************************************
void Measurement1D::SetCovarFromMultipleTextFiles(std::string covfiles,
                                                  int dim) {
  //********************************************************************

  if (dim == -1) {
    dim = fDataHist->GetNbinsX();
  }

  std::vector<std::string> covList = GeneralUtils::ParseToStr(covfiles, ";");

  fFullCovar = new TMatrixDSym(dim);
  for (uint i = 0; i < covList.size(); ++i) {
    NUIS_LOG(SAM, "Reading covariance from text file: " << covList[i]);
    TMatrixDSym *temp_cov = StatUtils::GetCovarFromTextFile(covList[i], dim);
    (*fFullCovar) += (*temp_cov);
    delete temp_cov;
  }
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}

//********************************************************************
void Measurement1D::SetCovarFromRootFile(std::string covfile,
                                         std::string histname) {
  //********************************************************************

  NUIS_LOG(SAM,
           "Reading covariance from text file: " << covfile << ";" << histname);
  fFullCovar = StatUtils::GetCovarFromRootFile(covfile, histname);
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}

//********************************************************************
void Measurement1D::SetCovarInvertFromTextFile(std::string covfile, int dim) {
  //********************************************************************

  if (dim == -1) {
    dim = fDataHist->GetNbinsX();
  }

  NUIS_LOG(SAM, "Reading inverted covariance from text file: " << covfile);
  covar = StatUtils::GetCovarFromTextFile(covfile, dim);
  fFullCovar = StatUtils::GetInvert(covar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}

//********************************************************************
void Measurement1D::SetCovarInvertFromRootFile(std::string covfile,
                                               std::string histname) {
  //********************************************************************

  NUIS_LOG(SAM, "Reading inverted covariance from text file: " << covfile << ";"
                                                               << histname);
  covar = StatUtils::GetCovarFromRootFile(covfile, histname);
  fFullCovar = StatUtils::GetInvert(covar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}

//********************************************************************
void Measurement1D::SetCorrelationFromTextFile(std::string covfile, int dim) {
  //********************************************************************

  if (dim == -1)
    dim = fDataHist->GetNbinsX();
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
void Measurement1D::SetCorrelationFromMultipleTextFiles(std::string corrfiles,
                                                        int dim) {
  //********************************************************************

  if (dim == -1) {
    dim = fDataHist->GetNbinsX();
  }

  std::vector<std::string> corrList = GeneralUtils::ParseToStr(corrfiles, ";");

  fFullCovar = new TMatrixDSym(dim);
  for (uint i = 0; i < corrList.size(); ++i) {
    NUIS_LOG(SAM, "Reading covariance from text file: " << corrList[i]);
    TMatrixDSym *temp_cov = StatUtils::GetCovarFromTextFile(corrList[i], dim);

    for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
      for (int j = 0; j < fDataHist->GetNbinsX(); j++) {
        (*temp_cov)(i, j) = (*temp_cov)(i, j) * fDataHist->GetBinError(i + 1) *
                            fDataHist->GetBinError(j + 1) * 1.E76;
      }
    }

    (*fFullCovar) += (*temp_cov);
    delete temp_cov;
  }
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}

//********************************************************************
void Measurement1D::SetCorrelationFromRootFile(std::string covfile,
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
void Measurement1D::SetCholDecompFromTextFile(std::string covfile, int dim) {
  //********************************************************************

  if (dim == -1) {
    dim = fDataHist->GetNbinsX();
  }

  NUIS_LOG(SAM, "Reading cholesky from text file: " << covfile);
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
void Measurement1D::SetCholDecompFromRootFile(std::string covfile,
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

void Measurement1D::SetShapeCovar() {

  // Return if this is missing any pre-requisites
  if (!fFullCovar)
    return;
  if (!fDataHist)
    return;

  // Also return if it's bloody stupid under the circumstances
  if (fIsDiag)
    return;

  fShapeCovar = StatUtils::ExtractShapeOnlyCovar(fFullCovar, fDataHist);
  return;
}

//********************************************************************
void Measurement1D::ScaleData(double scale) {
  //********************************************************************
  fDataHist->Scale(scale);
}

//********************************************************************
void Measurement1D::ScaleDataErrors(double scale) {
  //********************************************************************
  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    fDataHist->SetBinError(i + 1, fDataHist->GetBinError(i + 1) * scale);
  }
}

//********************************************************************
void Measurement1D::ScaleCovar(double scale) {
  //********************************************************************
  (*fFullCovar) *= scale;
  (*covar) *= 1.0 / scale;
  (*fDecomp) *= sqrt(scale);
}

//********************************************************************
void Measurement1D::SetBinMask(std::string maskfile) {
  //********************************************************************

  if (!fIsMask)
    return;
  NUIS_LOG(SAM, "Reading bin mask from file: " << maskfile);

  // Create a mask histogram with dim of data
  int nbins = fDataHist->GetNbinsX();
  fMaskHist = new TH1I((fSettings.GetName() + "_BINMASK").c_str(),
                       (fSettings.GetName() + "_BINMASK; Bin; Mask?").c_str(),
                       nbins, 0, nbins);
  std::string line;
  std::ifstream mask(maskfile.c_str(), std::ifstream::in);

  if (!mask.is_open()) {
    NUIS_ABORT("Cannot find mask file.");
  }

  while (std::getline(mask >> std::ws, line, '\n')) {
    std::vector<int> entries = GeneralUtils::ParseToInt(line, " ");

    // Skip lines with poorly formatted lines
    if (entries.size() < 2) {
      NUIS_LOG(WRN,
               "Measurement1D::SetBinMask(), couldn't parse line: " << line);
      continue;
    }

    // The first index should be the bin number, the second should be the mask
    // value.
    int val = 0;
    if (entries[1] > 0)
      val = 1;
    fMaskHist->SetBinContent(entries[0], val);
  }

  // Apply masking by setting masked data bins to zero
  PlotUtils::MaskBins(fDataHist, fMaskHist);

  return;
}

//********************************************************************
void Measurement1D::FinaliseMeasurement() {
  //********************************************************************

  NUIS_LOG(SAM, "Finalising Measurement: " << fName);

  if (fSettings.GetB("onlymc")) {
    if (fDataHist)
      delete fDataHist;
    fDataHist = new TH1D("empty_data", "empty_data", 1, 0.0, 1.0);
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

  // Push the diagonals of fFullCovar onto the data histogram
  // Comment this out until the covariance/data scaling is consistent!
  StatUtils::SetDataErrorFromCov(fDataHist, fFullCovar, 1E-38);

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
  fMCHist = (TH1D *)fDataHist->Clone();
  fMCHist->SetNameTitle((fSettings.GetName() + "_MC").c_str(),
                        (fSettings.GetFullTitles()).c_str());
  fMCHist->Reset();

  // Setup fMCFine
  fMCFine = new TH1D("mcfine", "mcfine", fDataHist->GetNbinsX() * 8,
                     fMCHist->GetBinLowEdge(1),
                     fMCHist->GetBinLowEdge(fDataHist->GetNbinsX() + 1));
  fMCFine->SetNameTitle((fSettings.GetName() + "_MC_FINE").c_str(),
                        (fSettings.GetFullTitles()).c_str());
  fMCFine->Reset();

  // Setup MC Stat
  fMCStat = (TH1D *)fMCHist->Clone();
  fMCStat->Reset();

  // Search drawopts for possible types to include by default
  std::string drawopts = FitPar::Config().GetParS("drawopts");
  if (drawopts.find("MODES") != std::string::npos) {
    fMCHist_Modes = new TrueModeStack((fSettings.GetName() + "_MODES").c_str(),
                                      ("True Channels"), fMCHist);
    SetAutoProcessTH1(fMCHist_Modes, kCMD_Reset, kCMD_Norm, kCMD_Write);
  }

  if (fSettings.Has("maskfile") && fSettings.Has("maskhist")) {
    fMaskHist = dynamic_cast<TH1I *>(PlotUtils::GetTH1FromRootFile(
        fSettings.GetS("maskfile"), fSettings.GetS("maskhist")));
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
    NUIS_ERR(FTL, "EXITING");
    throw;
  }

  if (fAddNormPen) {
    if (!fUseShapeNormDecomp) {
      fNormError = fSettings.GetNormError();
    }
    if (fNormError <= 0.0) {
      NUIS_ERR(FTL, "Norm error for class " << fName << " is 0.0!");
      NUIS_ERR(FTL, "If you want to use it please add fNormError=VAL");
      throw;
    }
  }

  // Create and fill Weighted Histogram
  if (!fMCWeighted) {

    fMCWeighted = (TH1D *)fMCHist->Clone();
    fMCWeighted->SetNameTitle((fName + "_MCWGHTS").c_str(),
                              (fName + "_MCWGHTS" + fPlotTitles).c_str());
    fMCWeighted->GetYaxis()->SetTitle("Weighted Events");
  }
}

//********************************************************************
void Measurement1D::SetFitOptions(std::string opt) {
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
      NUIS_ERR(WRN, "ERROR: Fit Option '"
                        << fit_options_input.at(i)
                        << "' Provided is not allowed for this measurement.");
      NUIS_ERR(WRN, "Fit Options should be provided as a '/' seperated list "
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
    NUIS_ERR(FTL, "Try to use a chi2!");
    throw;

  } else {
    fIsChi2 = true;
  }

  // EXTRAS
  if (opt.find("RAW") != std::string::npos)
    fIsRawEvents = true;
  if (opt.find("NOWIDTH") != std::string::npos)
    fIsNoWidth = true;
  if (opt.find("DIF") != std::string::npos)
    fIsDifXSec = true;
  if (opt.find("ENU1D") != std::string::npos)
    fIsEnu1D = true;
  if (opt.find("NORM") != std::string::npos)
    fAddNormPen = true;
  if (opt.find("MASK") != std::string::npos)
    fIsMask = true;

  return;
};

//********************************************************************
void Measurement1D::SetSmearingMatrix(std::string smearfile, int truedim,
                                      int recodim) {
  //********************************************************************

  // The smearing matrix describes the migration from true bins (rows) to reco
  // bins (columns)
  // Counter over the true bins!
  int row = 0;

  std::string line;
  std::ifstream smear(smearfile.c_str(), std::ifstream::in);

  // Note that the smearing matrix may be rectangular.
  fSmearMatrix = new TMatrixD(truedim, recodim);

  if (smear.is_open()) {
    NUIS_LOG(SAM, "Reading smearing matrix from file: " << smearfile);
  } else {
    NUIS_ABORT("Smearing matrix provided is incorrect: " << smearfile);
  }

  while (std::getline(smear >> std::ws, line, '\n')) {
    int column = 0;

    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++) {
      (*fSmearMatrix)(row, column) = (*iter) / 100.; // Convert to fraction from
      // percentage (this may not be
      // general enough)
      column++;
    }
    row++;
  }
  return;
}

//********************************************************************
void Measurement1D::ApplySmearingMatrix() {
  //********************************************************************

  if (!fSmearMatrix) {
    NUIS_ERR(WRN,
             fName << ": attempted to apply smearing matrix, but none was set");
    return;
  }

  TH1D *unsmeared = (TH1D *)fMCHist->Clone();
  TH1D *smeared = (TH1D *)fMCHist->Clone();
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
  fMCHist = (TH1D *)smeared->Clone();

  return;
}

/*
   Reconfigure LOOP
*/
//********************************************************************
void Measurement1D::ResetAll() {
  //********************************************************************

  fMCHist->Reset();
  fMCFine->Reset();
  fMCStat->Reset();

  return;
};

//********************************************************************
void Measurement1D::FillHistograms() {
  //********************************************************************

  if (Signal) {

    NUIS_LOG(DEB, "Fill MCHist: " << fXVar << ", " << Weight);

    fMCHist->Fill(fXVar, Weight);
    fMCFine->Fill(fXVar, Weight);
    fMCStat->Fill(fXVar, 1.0);

    if (fMCHist_Modes)
      fMCHist_Modes->Fill(Mode, fXVar, Weight);
  }

  return;
};

//********************************************************************
void Measurement1D::ScaleEvents() {
  //********************************************************************

  // Fill MCWeighted;
  // for (int i = 0; i < fMCHist->GetNbinsX(); i++) {
  //   fMCWeighted->SetBinContent(i + 1, fMCHist->GetBinContent(i + 1));
  //   fMCWeighted->SetBinError(i + 1,   fMCHist->GetBinError(i + 1));
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
                                   GetEventHistogram(), fScaleFactor, fNEvents);
    PlotUtils::FluxUnfoldedScaling(fMCFine, GetFluxHistogram(),
                                   GetEventHistogram(), fScaleFactor, fNEvents);

    if (fMCHist_Modes) {
      // Loop over the modes
      fMCHist_Modes->FluxUnfold(GetFluxHistogram(), GetEventHistogram(),
                                fScaleFactor, fNEvents);
      // PlotUtils::FluxUnfoldedScaling(fMCHist_Modes, GetFluxHistogram(),
      // GetEventHistogram(), fScaleFactor,
      // fNEvents);
    }

  } else if (fIsNoWidth) {
    fMCHist->Scale(fScaleFactor);
    fMCFine->Scale(fScaleFactor);
    if (fMCHist_Modes)
      fMCHist_Modes->Scale(fScaleFactor);
    // Any other differential scaling
  } else {
    fMCHist->Scale(fScaleFactor, "width");
    fMCFine->Scale(fScaleFactor, "width");

    if (fMCHist_Modes)
      fMCHist_Modes->Scale(fScaleFactor, "width");
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
  delete[] statratio;
  delete[] statratiofine;

  return;
};

//********************************************************************
void Measurement1D::ApplyNormScale(double norm) {
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
int Measurement1D::GetNDOF() {
  //********************************************************************
  int ndof = fDataHist->GetNbinsX();
  if (fMaskHist and fIsMask)
    ndof -= fMaskHist->Integral();
  return ndof;
}

//********************************************************************
double Measurement1D::GetLikelihood() {
  //********************************************************************

  // If this is for a ratio, there is no data histogram to compare to!
  if (fNoData || !fDataHist)
    return 0.;

  // Apply Masking to MC if Required.
  if (fIsMask and fMaskHist) {
    PlotUtils::MaskBins(fMCHist, fMaskHist);
  }

  // Sort Shape Scaling
  double scaleF = 0.0;
  // TODO Include !fIsRawEvents
  if (fIsShape) {
    // Don't renorm based on width if we are using ShapeNormDecomp
    if (fUseShapeNormDecomp) {
      if (fMCHist->Integral(1, fMCHist->GetNbinsX())) {
        scaleF = fDataHist->Integral(1, fDataHist->GetNbinsX()) /
                 fMCHist->Integral(1, fMCHist->GetNbinsX());
        fMCHist->Scale(scaleF);
        fMCFine->Scale(scaleF);
      }
    } else {
      if (fMCHist->Integral(1, fMCHist->GetNbinsX(), "width")) {
        scaleF = fDataHist->Integral(1, fDataHist->GetNbinsX(), "width") /
                 fMCHist->Integral(1, fMCHist->GetNbinsX(), "width");
        fMCHist->Scale(scaleF);
        fMCFine->Scale(scaleF);
      }
    }
  }

  // Likelihood Calculation
  double stat = 0.;
  if (fIsChi2) {

    if (fIsRawEvents) {
      stat = StatUtils::GetChi2FromEventRate(fDataHist, fMCHist, fMaskHist);
    } else if (fIsDiag) {
      stat = StatUtils::GetChi2FromDiag(fDataHist, fMCHist, fMaskHist);
    } else if (!fIsDiag and !fIsRawEvents) {
      stat = StatUtils::GetChi2FromCov(fDataHist, fMCHist, covar, fMaskHist, 1,
                                       1E76, fIsWriting ? fResidualHist : NULL);
      if (fChi2LessBinHist && fIsWriting) {
        for (int xi = 0; xi < fDataHist->GetNbinsX(); ++xi) {
          TH1I *binmask = fMaskHist
                              ? static_cast<TH1I *>(fMaskHist->Clone("mask"))
                              : new TH1I("mask", "", fDataHist->GetNbinsX(), 0,
                                         fDataHist->GetNbinsX());
          binmask->SetDirectory(NULL);
          binmask->SetBinContent(xi + 1, 1);
          fChi2LessBinHist->SetBinContent(
              xi + 1,
              StatUtils::GetChi2FromCov(fDataHist, fMCHist, covar, binmask));
          delete binmask;
        }
      }
    }
  }

  // Sort Penalty Terms
  if (fAddNormPen) {

    if (fUseShapeNormDecomp) { // if shape norm, then add the norm penalty from
                               // https://arxiv.org/pdf/2003.00088.pdf

      TH1 *masked_data = StatUtils::ApplyHistogramMasking(fDataHist, fMaskHist);
      TH1 *masked_mc = StatUtils::ApplyHistogramMasking(fMCHist, fMaskHist);
      masked_mc->Scale(scaleF);

      NUIS_LOG(REC, "Shape Norm Decomp mcinteg: "
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

      NUIS_LOG(SAM, "Using Shape/Norm decomposition: Norm penalty "
                        << normpen << " on shape penalty of " << stat);
      stat += normpen;

    } else {
      double penalty =
          (1. - fCurrentNorm) * (1. - fCurrentNorm) / (fNormError * fNormError);

      stat += penalty;
    }
  }

  // Return to normal scaling
  if (fIsShape) { // and !FitPar::Config().GetParB("saveshapescaling")) {
    fMCHist->Scale(1. / scaleF);
    fMCFine->Scale(1. / scaleF);
  }

  fLikelihood = stat;

  return stat;
}

/*
  Fake Data Functions
*/
//********************************************************************
void Measurement1D::SetFakeDataValues(std::string fakeOption) {
  //********************************************************************

  // Setup original/datatrue
  TH1D *tempdata = (TH1D *)fDataHist->Clone();

  if (!fIsFakeData) {
    fIsFakeData = true;

    // Make a copy of the original data histogram.
    if (!fDataOrig)
      fDataOrig = (TH1D *)fDataHist->Clone((fName + "_data_original").c_str());

  } else {
    ResetFakeData();
  }

  // Setup Inputs
  fFakeDataInput = fakeOption;
  NUIS_LOG(SAM, "Setting fake data from : " << fFakeDataInput);

  // From MC
  if (fFakeDataInput.compare("MC") == 0) {
    fDataHist = (TH1D *)fMCHist->Clone((fName + "_MC").c_str());

    // Fake File
  } else {
    if (!fFakeDataFile)
      fFakeDataFile = new TFile(fFakeDataInput.c_str(), "READ");
    fDataHist = (TH1D *)fFakeDataFile->Get((fName + "_MC").c_str());
  }

  // Setup Data Hist
  fDataHist->SetNameTitle((fName + "_FAKE").c_str(),
                          (fName + fPlotTitles).c_str());

  // Replace Data True
  if (fDataTrue)
    delete fDataTrue;
  fDataTrue = (TH1D *)fDataHist->Clone();
  fDataTrue->SetNameTitle((fName + "_FAKE_TRUE").c_str(),
                          (fName + fPlotTitles).c_str());

  // Make a new covariance for fake data hist.
  int nbins = fDataHist->GetNbinsX();
  double alpha_i = 0.0;
  double alpha_j = 0.0;

  for (int i = 0; i < nbins; i++) {
    for (int j = 0; j < nbins; j++) {
      alpha_i =
          fDataHist->GetBinContent(i + 1) / tempdata->GetBinContent(i + 1);
      alpha_j =
          fDataHist->GetBinContent(j + 1) / tempdata->GetBinContent(j + 1);

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
void Measurement1D::ResetFakeData() {
  //********************************************************************

  if (fIsFakeData) {
    if (fDataHist)
      delete fDataHist;
    fDataHist =
        (TH1D *)fDataTrue->Clone((fSettings.GetName() + "_FKDAT").c_str());
  }
}

//********************************************************************
void Measurement1D::ResetData() {
  //********************************************************************

  if (fIsFakeData) {
    if (fDataHist)
      delete fDataHist;
    fDataHist =
        (TH1D *)fDataOrig->Clone((fSettings.GetName() + "_data").c_str());
  }

  fIsFakeData = false;
}

//********************************************************************
void Measurement1D::ThrowCovariance() {
  //********************************************************************

  // Take a fDecomposition and use it to throw the current dataset.
  // Requires fDataTrue also be set incase used repeatedly.

  if (!fDataTrue)
    fDataTrue = (TH1D *)fDataHist->Clone();
  if (fDataHist)
    delete fDataHist;
  fDataHist = StatUtils::ThrowHistogram(fDataTrue, fFullCovar);

  return;
};

//********************************************************************
void Measurement1D::ThrowDataToy() {
  //********************************************************************
  if (!fDataTrue)
    fDataTrue = (TH1D *)fDataHist->Clone();
  if (fMCHist)
    delete fMCHist;
  fMCHist = StatUtils::ThrowHistogram(fDataTrue, fFullCovar);
}

/*
   Access Functions
*/

//********************************************************************
TH1D *Measurement1D::GetMCHistogram() {
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

  // if (fSettings.Has("linecolor")) linecolor = fSettings.GetI("linecolor");
  // if (fSettings.Has("linestyle")) linestyle = fSettings.GetI("linestyle");
  // if (fSettings.Has("linewidth")) linewidth = fSettings.GetI("linewidth");

  // if (fSettings.Has("fillcolor")) fillcolor = fSettings.GetI("fillcolor");
  // if (fSettings.Has("fillstyle")) fillstyle = fSettings.GetI("fillstyle");

  fMCHist->SetTitle(chi2.str().c_str());

  fMCHist->SetLineColor(linecolor);
  fMCHist->SetLineStyle(linestyle);
  fMCHist->SetLineWidth(linewidth);

  fMCHist->SetFillColor(fillcolor);
  fMCHist->SetFillStyle(fillstyle);

  return fMCHist;
};

//********************************************************************
TH1D *Measurement1D::GetDataHistogram() {
  //********************************************************************

  if (!fDataHist)
    return fDataHist;

  int datacolor = kBlack;
  int datastyle = 1;
  int datawidth = 1;

  // if (fSettings.Has("datacolor")) datacolor = fSettings.GetI("datacolor");
  // if (fSettings.Has("datastyle")) datastyle = fSettings.GetI("datastyle");
  // if (fSettings.Has("datawidth")) datawidth = fSettings.GetI("datawidth");

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
void Measurement1D::Write(std::string drawOpt) {
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

  // Write Data/MC
  if (drawOpt.find("DATA") != std::string::npos)
    GetDataList().at(0)->Write();
  if (drawOpt.find("MC") != std::string::npos) {
    GetMCList().at(0)->Write();
    if ((fEvtRateScaleFactor != 0xdeadbeef) && GetMCList().at(0)) {
      TH1D *PredictedEvtRate = static_cast<TH1D *>(GetMCList().at(0)->Clone());
      PredictedEvtRate->Scale(fEvtRateScaleFactor);
      PredictedEvtRate->GetYaxis()->SetTitle("Predicted event rate");
      PredictedEvtRate->Write();
    }
  }

  // Write Fine Histogram
  if (drawOpt.find("FINE") != std::string::npos)
    GetFineList().at(0)->Write();

  // Write Weighted Histogram
  if (drawOpt.find("WEIGHTS") != std::string::npos && fMCWeighted)
    fMCWeighted->Write();

  // Save Flux/Evt if no event manager
  if (!FitPar::Config().GetParB("EventManager")) {
    if (drawOpt.find("FLUX") != std::string::npos && GetFluxHistogram())
      GetFluxHistogram()->Write();

    if (drawOpt.find("EVT") != std::string::npos && GetEventHistogram())
      GetEventHistogram()->Write();

    if (drawOpt.find("XSEC") != std::string::npos && GetEventHistogram())
      GetXSecHistogram()->Write();
  }

  // Write Mask
  if (fIsMask && (drawOpt.find("MASK") != std::string::npos)) {
    fMaskHist->Write();
  }

  // Write Covariances
  if (drawOpt.find("COV") != std::string::npos && fFullCovar) {
    PlotUtils::GetFullCovarPlot(fFullCovar, fSettings.GetName())->Write();
  }

  if (drawOpt.find("INVCOV") != std::string::npos && covar) {
    PlotUtils::GetInvCovarPlot(covar, fSettings.GetName())->Write();
  }

  if (drawOpt.find("DECOMP") != std::string::npos && fDecomp) {
    PlotUtils::GetDecompCovarPlot(fDecomp, fSettings.GetName())->Write();
  }

  // // Likelihood residual plots
  // if (drawOpt.find("RESIDUAL") != std::string::npos) {
  //   WriteResidualPlots();
  // }

  // Ratio and Shape Plots
  if (drawOpt.find("RATIO") != std::string::npos) {
    WriteRatioPlot();
  }

  if (drawOpt.find("SHAPE") != std::string::npos) {
    WriteShapePlot();
    if (drawOpt.find("RATIO") != std::string::npos)
      WriteShapeRatioPlot();
  }

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

  if (fIsChi2 && !fIsDiag) {
    fResidualHist = (TH1D *)fMCHist->Clone((fName + "_RESIDUAL").c_str());
    fResidualHist->GetYaxis()->SetTitle("#Delta#chi^{2}");
    fResidualHist->Reset();

    fChi2LessBinHist =
        (TH1D *)fMCHist->Clone((fName + "_Chi2NMinusOne").c_str());
    fChi2LessBinHist->GetYaxis()->SetTitle("Total #chi^{2} without bin_{i}");
    fChi2LessBinHist->Reset();

    fIsWriting = true;
    (void)GetLikelihood();
    fIsWriting = false;

    fResidualHist->Write((fName + "_RESIDUAL").c_str());
    fChi2LessBinHist->Write((fName + "_Chi2NMinusOne").c_str());
  }

  // Write Extra Histograms
  AutoWriteExtraTH1();
  WriteExtraHistograms();

  // Returning
  NUIS_LOG(SAM, "Written Histograms: " << fName);
  return;
}

//********************************************************************
void Measurement1D::WriteRatioPlot() {
  //********************************************************************

  // Setup mc data ratios
  TH1D *dataRatio = (TH1D *)fDataHist->Clone((fName + "_data_RATIO").c_str());
  TH1D *mcRatio = (TH1D *)fMCHist->Clone((fName + "_MC_RATIO").c_str());

  // Extra MC Data Ratios
  for (int i = 0; i < mcRatio->GetNbinsX(); i++) {

    dataRatio->SetBinContent(i + 1, fDataHist->GetBinContent(i + 1) /
                                        fMCHist->GetBinContent(i + 1));
    dataRatio->SetBinError(i + 1, fDataHist->GetBinError(i + 1) /
                                      fMCHist->GetBinContent(i + 1));

    mcRatio->SetBinContent(i + 1, fMCHist->GetBinContent(i + 1) /
                                      fMCHist->GetBinContent(i + 1));
    mcRatio->SetBinError(i + 1, fMCHist->GetBinError(i + 1) /
                                    fMCHist->GetBinContent(i + 1));
  }

  // Write ratios
  mcRatio->Write();
  dataRatio->Write();

  delete mcRatio;
  delete dataRatio;
}

//********************************************************************
void Measurement1D::WriteShapePlot() {
  //********************************************************************

  TH1D *mcShape = (TH1D *)fMCHist->Clone((fName + "_MC_SHAPE").c_str());

  TH1D *dataShape = (TH1D *)fDataHist->Clone((fName + "_data_SHAPE").c_str());
  // Don't check error
  if (fShapeCovar)
    StatUtils::SetDataErrorFromCov(dataShape, fShapeCovar, 1E-38, false);

  double shapeScale = 1.0;
  if (fIsRawEvents) {
    shapeScale = fDataHist->Integral() / fMCHist->Integral();
  } else {
    shapeScale = fDataHist->Integral("width") / fMCHist->Integral("width");
  }

  mcShape->Scale(shapeScale);

  std::stringstream ss;
  ss << shapeScale;
  mcShape->SetTitle(ss.str().c_str());

  mcShape->SetLineWidth(3);
  mcShape->SetLineStyle(7);

  mcShape->Write();
  dataShape->Write();

  delete mcShape;
}

//********************************************************************
void Measurement1D::WriteShapeRatioPlot() {
  //********************************************************************

  // Get a mcshape histogram
  TH1D *mcShape = (TH1D *)fMCHist->Clone((fName + "_MC_SHAPE").c_str());

  double shapeScale = 1.0;
  if (fIsRawEvents) {
    shapeScale = fDataHist->Integral() / fMCHist->Integral();
  } else {
    shapeScale = fDataHist->Integral("width") / fMCHist->Integral("width");
  }

  mcShape->Scale(shapeScale);

  // Create shape ratio histograms
  TH1D *mcShapeRatio =
      (TH1D *)mcShape->Clone((fName + "_MC_SHAPE_RATIO").c_str());
  TH1D *dataShapeRatio =
      (TH1D *)fDataHist->Clone((fName + "_data_SHAPE_RATIO").c_str());

  // Divide the histograms
  mcShapeRatio->Divide(mcShape);
  dataShapeRatio->Divide(mcShape);

  // Colour the shape ratio plots
  mcShapeRatio->SetLineWidth(3);
  mcShapeRatio->SetLineStyle(7);

  mcShapeRatio->Write();
  dataShapeRatio->Write();

  delete mcShapeRatio;
  delete dataShapeRatio;
}

//// CRAP TO BE REMOVED

//********************************************************************
void Measurement1D::SetupMeasurement(std::string inputfile, std::string type,
                                     FitWeight *rw, std::string fkdt) {
  //********************************************************************

  nuiskey samplekey = Config::CreateKey("sample");
  samplekey.Set("name", fName);
  samplekey.Set("type", type);
  samplekey.Set("input", inputfile);
  fSettings = LoadSampleSettings(samplekey);

  // Reset everything to NULL
  // Init();

  // Check if name contains Evt, indicating that it is a raw number of events
  // measurements and should thus be treated as once
  fIsRawEvents = false;
  if ((fName.find("Evt") != std::string::npos) && fIsRawEvents == false) {
    fIsRawEvents = true;
    NUIS_LOG(SAM, "Found event rate measurement but fIsRawEvents == false!");
    NUIS_LOG(SAM, "Overriding this and setting fIsRawEvents == true!");
  }

  fIsEnu1D = false;
  if (fName.find("XSec_1DEnu") != std::string::npos) {
    fIsEnu1D = true;
    NUIS_LOG(SAM, "::" << fName << "::");
    NUIS_LOG(SAM,
             "Found XSec Enu measurement, applying flux integrated scaling, "
             "not flux averaged!");
  }

  if (fIsEnu1D && fIsRawEvents) {
    NUIS_ERR(FTL, "Found 1D Enu XSec distribution AND fIsRawEvents, is this "
                  "really correct?!");
    NUIS_ERR(FTL, "Check experiment constructor for " << fName
                                                      << " and correct this!");
    NUIS_ERR(FTL, "I live in " << __FILE__ << ":" << __LINE__);
    throw;
  }

  fRW = rw;

  if (!fInput and !fIsJoint)
    SetupInputs(inputfile);

  // Set Default Options
  SetFitOptions(fDefaultTypes);

  // Set Passed Options
  SetFitOptions(type);

  // Still adding support for flat flux inputs
  //  // Set Enu Flux Scaling
  //  if (isFlatFluxFolding) this->Input()->ApplyFluxFolding(
  //  this->defaultFluxHist );

  // FinaliseMeasurement();
}

//********************************************************************
void Measurement1D::SetupDefaultHist() {
  //********************************************************************

  // Setup fMCHist
  fMCHist = (TH1D *)fDataHist->Clone();
  fMCHist->SetNameTitle((fName + "_MC").c_str(),
                        (fName + "_MC" + fPlotTitles).c_str());

  // Setup fMCFine
  Int_t nBins = fMCHist->GetNbinsX();
  fMCFine = new TH1D(
      (fName + "_MC_FINE").c_str(), (fName + "_MC_FINE" + fPlotTitles).c_str(),
      nBins * 6, fMCHist->GetBinLowEdge(1), fMCHist->GetBinLowEdge(nBins + 1));

  fMCStat = (TH1D *)fMCHist->Clone();
  fMCStat->Reset();

  fMCHist->Reset();
  fMCFine->Reset();

  // Setup the NEUT Mode Array
  PlotUtils::CreateNeutModeArray((TH1D *)fMCHist, (TH1 **)fMCHist_PDG);
  PlotUtils::ResetNeutModeArray((TH1 **)fMCHist_PDG);

  // Setup bin masks using sample name
  if (fIsMask) {
    std::string maskloc = FitPar::Config().GetParDIR(fName + ".mask");
    if (maskloc.empty()) {
      maskloc = FitPar::GetDataBase() + "/masks/" + fName + ".mask";
    }

    SetBinMask(maskloc);
  }

  fMCHist_Modes =
      new TrueModeStack((fName + "_MODES").c_str(), ("True Channels"), fMCHist);
  SetAutoProcessTH1(fMCHist_Modes, kCMD_Reset, kCMD_Norm, kCMD_Write);

  return;
}

//********************************************************************
void Measurement1D::SetDataValues(std::string dataFile) {
  //********************************************************************

  // Override this function if the input file isn't in a suitable format
  NUIS_LOG(SAM, "Reading data from: " << dataFile.c_str());
  fDataHist =
      PlotUtils::GetTH1DFromFile(dataFile, (fName + "_data"), fPlotTitles);
  fDataTrue = (TH1D *)fDataHist->Clone();

  // Number of data points is number of bins
  fNDataPointsX = fDataHist->GetXaxis()->GetNbins();

  return;
};

//********************************************************************
void Measurement1D::SetDataFromDatabase(std::string inhistfile,
                                        std::string histname) {
  //********************************************************************

  NUIS_LOG(SAM, "Filling histogram from " << inhistfile << "->" << histname);
  fDataHist = PlotUtils::GetTH1DFromRootFile(
      (GeneralUtils::GetTopLevelDir() + "/data/" + inhistfile), histname);
  fDataHist->SetNameTitle((fName + "_data").c_str(), (fName + "_data").c_str());

  return;
};

//********************************************************************
void Measurement1D::SetDataFromFile(std::string inhistfile,
                                    std::string histname) {
  //********************************************************************

  NUIS_LOG(SAM, "Filling histogram from " << inhistfile << "->" << histname);
  fDataHist = PlotUtils::GetTH1DFromRootFile((inhistfile), histname);
  fDataHist->SetNameTitle((fName + "_data").c_str(), (fName + "_data").c_str());

  return;
};

//********************************************************************
void Measurement1D::SetCovarMatrix(std::string covarFile) {
  //********************************************************************

  // Covariance function, only really used when reading in the MB Covariances.

  TFile *tempFile = new TFile(covarFile.c_str(), "READ");

  TH2D *covarPlot = new TH2D();
  TH2D *fFullCovarPlot = new TH2D();
  std::string covName = "";
  std::string covOption = FitPar::Config().GetParS("thrown_covariance");

  if (fIsShape || fIsFree)
    covName = "shp_";
  if (fIsDiag)
    covName += "diag";
  else
    covName += "full";

  covarPlot = (TH2D *)tempFile->Get((covName + "cov").c_str());

  if (!covOption.compare("SUB"))
    fFullCovarPlot = (TH2D *)tempFile->Get((covName + "cov").c_str());
  else if (!covOption.compare("FULL"))
    fFullCovarPlot = (TH2D *)tempFile->Get("fullcov");
  else {
    NUIS_ERR(WRN, "Incorrect thrown_covariance option in parameters.");
  }

  int dim = int(fDataHist->GetNbinsX()); //-this->masked->Integral());
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
  // fCovDet = this->covar->Determinant();

  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");

  return;
};

//********************************************************************
// Sets the covariance matrix from a provided file in a text format
// scale is a multiplicative pre-factor to apply in the case where the
// covariance is given in some unit (e.g. 1E-38)
void Measurement1D::SetCovarMatrixFromText(std::string covarFile, int dim,
                                           double scale) {
  //********************************************************************

  // Make a counter to track the line number
  int row = 0;

  std::string line;
  std::ifstream covarread(covarFile.c_str(), std::ifstream::in);

  this->covar = new TMatrixDSym(dim);
  fFullCovar = new TMatrixDSym(dim);
  if (covarread.is_open()) {
    NUIS_LOG(SAM, "Reading covariance matrix from file: " << covarFile);
  } else {
    NUIS_ABORT("Covariance matrix provided is incorrect: " << covarFile);
  }

  // Loop over the lines in the file
  while (std::getline(covarread >> std::ws, line, '\n')) {
    int column = 0;

    // Loop over entries and insert them into matrix
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");

    if (entries.size() <= 1) {
      NUIS_ERR(WRN, "SetCovarMatrixFromText -> Covariance matrix only has <= 1 "
                    "entries on this line: "
                        << row);
    }

    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++) {
      (*covar)(row, column) = *iter;
      (*fFullCovar)(row, column) = *iter;

      column++;
    }

    row++;
  }
  covarread.close();

  // Scale the actualy covariance matrix by some multiplicative factor
  (*fFullCovar) *= scale;

  // Robust matrix inversion method
  TDecompSVD LU = TDecompSVD(*this->covar);
  // THIS IS ACTUALLY THE INVERSE COVARIANCE MATRIXA AAAAARGH
  delete this->covar;
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");

  // Now need to multiply by the scaling factor
  // If the covariance
  (*this->covar) *= 1. / (scale);

  return;
};

//********************************************************************
void Measurement1D::SetCovarMatrixFromCorrText(std::string corrFile, int dim) {
  //********************************************************************

  // Make a counter to track the line number
  int row = 0;

  std::string line;
  std::ifstream corr(corrFile.c_str(), std::ifstream::in);

  this->covar = new TMatrixDSym(dim);
  this->fFullCovar = new TMatrixDSym(dim);
  if (corr.is_open()) {
    NUIS_LOG(SAM, "Reading and converting correlation matrix from file: "
                      << corrFile);
  } else {
    NUIS_ABORT("Correlation matrix provided is incorrect: " << corrFile);
  }

  while (std::getline(corr >> std::ws, line, '\n')) {
    int column = 0;

    // Loop over entries and insert them into matrix
    // Multiply by the errors to get the covariance, rather than the correlation
    // matrix
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++) {
      double val = (*iter) * this->fDataHist->GetBinError(row + 1) * 1E38 *
                   this->fDataHist->GetBinError(column + 1) * 1E38;
      if (val == 0) {
        NUIS_ABORT("Found a zero value in the covariance matrix, assuming "
                   "this is an error!");
      }

      (*this->covar)(row, column) = val;
      (*this->fFullCovar)(row, column) = val;

      column++;
    }

    row++;
  }

  // Robust matrix inversion method
  TDecompSVD LU = TDecompSVD(*this->covar);
  delete this->covar;
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");

  return;
};

//********************************************************************
// FullUnits refers to if we have "real" unscaled units in the covariance
// matrix, e.g. 1E-76. If this is the case we need to scale it so that the chi2
// contribution is correct NUISANCE internally assumes the covariance matrix has
// units of 1E76
void Measurement1D::SetCovarFromDataFile(std::string covarFile,
                                         std::string covName, bool FullUnits) {
  //********************************************************************

  NUIS_LOG(SAM, "Getting covariance from " << covarFile << "->" << covName);

  TFile *tempFile = new TFile(covarFile.c_str(), "READ");
  TH2D *covPlot = (TH2D *)tempFile->Get(covName.c_str());
  covPlot->SetDirectory(0);
  // Scale the covariance matrix if it comes in normal units
  if (FullUnits) {
    covPlot->Scale(1.E76);
  }

  int dim = covPlot->GetNbinsX();
  fFullCovar = new TMatrixDSym(dim);

  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      (*fFullCovar)(i, j) = covPlot->GetBinContent(i + 1, j + 1);
    }
  }

  this->covar = (TMatrixDSym *)fFullCovar->Clone();
  fDecomp = (TMatrixDSym *)fFullCovar->Clone();

  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");

  TDecompChol LUChol = TDecompChol(*fDecomp);
  LUChol.Decompose();
  fDecomp = new TMatrixDSym(dim, LU.GetU().GetMatrixArray(), "");

  return;
};

// //********************************************************************
// void Measurement1D::SetBinMask(std::string maskFile) {
//   //********************************************************************

//   // Create a mask histogram.
//   int nbins = fDataHist->GetNbinsX();
//   fMaskHist =
//     new TH1I((fName + "_fMaskHist").c_str(),
//              (fName + "_fMaskHist; Bin; Mask?").c_str(), nbins, 0, nbins);
//   std::string line;
//   std::ifstream mask(maskFile.c_str(), std::ifstream::in);

//   if (mask.is_open())
//     LOG(SAM) << "Reading bin mask from file: " << maskFile << std::endl;
//   else
//     LOG(FTL) << " Cannot find mask file." << std::endl;

//   while (std::getline(mask >> std::ws, line, '\n')) {
//     std::vector<int> entries = GeneralUtils::ParseToInt(line, " ");

//     // Skip lines with poorly formatted lines
//     if (entries.size() < 2) {
//       LOG(WRN) << "Measurement1D::SetBinMask(), couldn't parse line: " <<
//       line
//                << std::endl;
//       continue;
//     }

//     // The first index should be the bin number, the second should be the
//     mask
//     // value.
//     fMaskHist->SetBinContent(entries[0], entries[1]);
//   }

//   // Set masked data bins to zero
//   PlotUtils::MaskBins(fDataHist, fMaskHist);

//   return;
// }

// //********************************************************************
// void Measurement1D::GetBinContents(std::vector<double>& cont,
//                                    std::vector<double>& err) {
//   //********************************************************************

//   // Return a vector of the main bin contents
//   for (int i = 0; i < fMCHist->GetNbinsX(); i++) {
//     cont.push_back(fMCHist->GetBinContent(i + 1));
//     err.push_back(fMCHist->GetBinError(i + 1));
//   }

//   return;
// };

/*
   XSec Functions
   */

// //********************************************************************
// void Measurement1D::SetFluxHistogram(std::string fluxFile, int minE, int
// maxE,
//     double fluxNorm) {
//   //********************************************************************

//   // Note this expects the flux bins to be given in terms of MeV
//   LOG(SAM) << "Reading flux from file: " << fluxFile << std::endl;

//   TGraph f(fluxFile.c_str(), "%lg %lg");

//   fFluxHist =
//     new TH1D((fName + "_flux").c_str(), (fName + "; E_{#nu} (GeV)").c_str(),
//         f.GetN() - 1, minE, maxE);

//   Double_t* yVal = f.GetY();

//   for (int i = 0; i < fFluxHist->GetNbinsX(); ++i)
//     fFluxHist->SetBinContent(i + 1, yVal[i] * fluxNorm);
// };

// //********************************************************************
// double Measurement1D::TotalIntegratedFlux(std::string intOpt, double low,
//     double high) {
//   //********************************************************************

//   if (fInput->GetType() == kGiBUU) {
//     return 1.0;
//   }

//   // The default case of low = -9999.9 and high = -9999.9
//   if (low == -9999.9) low = this->EnuMin;
//   if (high == -9999.9) high = this->EnuMax;

//   int minBin = fFluxHist->GetXaxis()->FindBin(low);
//   int maxBin = fFluxHist->GetXaxis()->FindBin(high);

//   // Get integral over custom range
//   double integral = fFluxHist->Integral(minBin, maxBin + 1, intOpt.c_str());

//   return integral;
// };
