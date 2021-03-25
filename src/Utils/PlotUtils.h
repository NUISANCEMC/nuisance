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

#ifndef PLOTUTILS_H_SEEN
#define PLOTUTILS_H_SEEN

#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TMatrixD.h"
#include "TProfile.h"
#include "TSystem.h"
#include "TVectorD.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// C Includes
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

// ROOT includes
#include <TChain.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <THStack.h>
#include <TKey.h>
#include <TLegend.h>
#include <TList.h>
#include <TLorentzVector.h>
#include <TObjArray.h>
#include <TROOT.h>
#include <TRandom3.h>
#include <TTree.h>
#include <ctime>
#include "TGraphErrors.h"
#include "TH2Poly.h"
#include "TMatrixDSym.h"

// Fit includes
#include "FitEvent.h"
#include "FitLogger.h"
#include "GeneralUtils.h"
#include "StatUtils.h"

/*!
 *  \addtogroup Utils
 *  @{
 */

//! Functions to handle different TH1s and TFiles
namespace PlotUtils {

/*!
  MISC Functions
*/

//! Check the root file has an object containing the given substring in its name
bool CheckObjectWithName(TFile* inFile, std::string substring);

//! Get object in a TFile that has  a matching substring in its name
std::string GetObjectWithName(TFile* inFile, std::string substring);

/*!
  Interaction Mode Histogram Handling
*/

//! Fill the Histogram Array with 61 new histograms for interaction channels.
void CreateNeutModeArray(TH1* hist, TH1* neutarray[]);

//! Call Fill on the relevent Mode 1D Histogram
void FillNeutModeArray(TH1D* hist[], int mode, double xval,
                       double weight = 1.0);

//! Call Fill on the relevant Mode 2D Histogram
void FillNeutModeArray(TH2D* hist[], int mode, double xval, double yval,
                       double weight = 1.0);

//! Given two arrays of histograms for the NEUT interaction mode, hist1 = hist1
//! + scale*hist2
void AddNeutModeArray(TH1D* hist1[], TH1D* hist2[], double scaling = 1.0);

//! Generate a legend for the THStack
TLegend GenerateStackLegend(THStack stack, int xlow, int ylow, int xhigh,
                            int yhigh);

//! Turn the array of TH1 histograms into a stack of Modes
THStack GetNeutModeStack(std::string title, TH1* ModeStack[], int option);

//! Get a slice in Y of a TH2D
TH1D* GetSliceY(TH2D*, int);
//! Get a slice in X of a TH2D
TH1D* GetSliceX(TH2D*, int);

//! Reset each histogram in the mode array
void ResetNeutModeArray(TH1* hist[]);

//! Scale each histogram in the mode array by a single scaling factor, option
//! can be used to define "width" scaling.
void ScaleNeutModeArray(TH1* hist[], double factor, std::string option = "");

//! Free up the memory used by each of the 61 mode histograms
void DeleteNeutModeArray(TH1* neutarray[]);

/*!
  Handling Functions
*/

//! Divide by the flux histogram for Enu Histograms
void DivideByFlux(TH1D* fMCHist, TH1D* fFluxHist);

TH1D* InterpolateFineHistogram(TH1D* hist, int res, std::string opt);

//! Flux unfolded scaling, like DivideByFlux but uses interpolation.
void FluxUnfoldedScaling(TH1D* plot, TH1D* flux, TH1D* events = NULL,
                         double scalefactor = 1.0, int nevents = 1);

//! Flux unfolded scaling for 2D histograms
void FluxUnfoldedScaling(TH2D* plot, TH1D* flux, TH1D* events = NULL,
                         double scalefactor = 1.0);

//! Fill a 2D Histogram from a text file
void Set2DHistFromText(std::string dataFile, TH2* hist, double norm,
                       bool skipbins = false);

//! Fill a 2D Poly Histogram from a text file
void Set2PolyHistFromText(std::string dataFile, TH2Poly* hist, double norm,
                          bool skipbins = false);

//! Fill a 1D Histogram from a text file
TH1D* GetTH1DFromFile(std::string dataFile, std::string title = "",
                      std::string fPlotTitles = "", std::string alt_name = "");

TH1* GetTH1FromRootFile(std::string file, std::string name);

std::vector<TH1*> GetTH1sFromRootFile(std::string const& descriptor);

//! Grab a 1D Histrogram from a ROOT File
TH1D* GetTH1DFromRootFile(std::string file, std::string name);

//! Grab a 2D Histrogram from a ROOT File
TH2D* GetTH2DFromRootFile(std::string file, std::string name);

//! Grab a 2D Histrogram from a ROOT File

template <typename TH>
inline TH* GetTH2FromRootFile(std::string file, std::string name){
  if (name.empty()) {
    std::vector<std::string> tempfile = GeneralUtils::ParseToStr(file, ";");
    file = tempfile[0];
    name = tempfile[1];
  }

  TFile *rootHistFile = new TFile(file.c_str(), "READ");
  TH *tempHist = dynamic_cast<TH *>(rootHistFile->Get(name.c_str())->Clone());

  if(!tempHist){
    NUIS_ABORT("Failed to read " << name << " from " << file);
  }

  tempHist->SetDirectory(nullptr);

  rootHistFile->Close();
  delete rootHistFile;

  return tempHist;
}


//! Get a TGraph from a ROOT file
TGraph* GetTGraphFromRootFile(std::string file, std::string name);

//! Grab a 2D Histrogram from a ROOT File
TH2D* GetTH2DFromTextFile(std::string data, std::string binx, std::string biny);

std::vector<double> GetArrayFromTextFile(std::string file);
std::vector<std::vector<double> > Get2DArrayFromTextFile(std::string file);

//! Scale mc to match data considering empty and masked bins
void ScaleToData(TH1D* data, TH1D* mc, TH1I* mask);

//! Apply bin masking (More cosmetic)
void MaskBins(TH1D* hist, TH1I* mask);

//! Apply bin masking (More cosmetic)
void MaskBins(TH2D* hist, TH2I* mask);

//! Get the data MC ratio considering empty and masked bins
double GetDataMCRatio(TH1D* data, TH1D* mc, TH1I* mask = NULL);

/*!
  Formatting Plot Utils
*/

//! Create new ratio plot. hist3 = hist1/hist2
TH1D* GetRatioPlot(TH1D* hist1, TH1D* hist2, TH1D* new_hist=NULL);

//! Create new plot of hist2 normalised to hist1. hist2 = hist1 *
//! (Integral(hist1)/Integral(hist2))
TH1D* GetRenormalisedPlot(TH1D* hist1, TH1D* hist2);

//! Normalise histogram to area of unity.
TH1D* GetShapePlot(TH1D* hist1);

//! Normalise hist1 and hist2 to unity, before creating a new plot of their
//! ratio.
TH1D* GetShapeRatio(TH1D* hist1, TH1D* hist2);

//! Create a covariance histogram from a TMatrixDSym and add titles given.
TH2D* GetCovarPlot(TMatrixDSym* cov, std::string name, std::string title);

//! Wrapper function to create full covariance plot
TH2D* GetFullCovarPlot(TMatrixDSym* cov, std::string name);

//! Wrapper function to create inverted covariance plot
TH2D* GetInvCovarPlot(TMatrixDSym* cov, std::string name);

//! Wrapper function to create decomposed covariance plot
TH2D* GetDecompCovarPlot(TMatrixDSym* cov, std::string name);

//! Given a covariance histogram, divide by errors to form a correlation plot.
TH2D* GetCorrelationPlot(TH2D* cov, std::string name);

//! Given a covariance histogram, calculate the decomposed matrix to form a
//! decomposed plot.
TH2D* GetDecompPlot(TH2D* cov, std::string name);

//! Given two 1D histograms create a 2D histogram which uses their bin edges to
//! define both axes.
TH2D* MergeIntoTH2D(TH1D* xhist, TH1D* yhist, std::string zname = "");

//! Given a 1D Histogram, set any empty bins in Data to empty bins in MC
void MatchEmptyBins(TH1D* data, TH1D* mc);

//! Given a 2D Histogram, set any empty bins in Data to empty bins in MC
void MatchEmptyBins(TH2D* data, TH2D* mc);

//! Return a projection of a 2D Histogram onto X accounting for bin masking
TH1D* GetProjectionX(TH2D* hist, TH2I* mask);

//! Return a projection of a 2D Histogram onto Y accounting for bin masking
TH1D* GetProjectionY(TH2D* hist, TH2I* mask);

//! Return a histogram with a restricted range
TH1D* RestrictHistRange(TH1D* inHist, double minVal, double maxVal);
}

/*! @} */
#endif
