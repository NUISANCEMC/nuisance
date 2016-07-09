// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef PLOTUTILS_H_SEEN
#define PLOTUTILS_H_SEEN

// C Includes
#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <iostream>
#include <unistd.h>

// ROOT includes
#include <TROOT.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TFile.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include <TList.h>
#include <TKey.h>
#include <THStack.h>
#include <TLegend.h>
#include <TObjArray.h>
#include <TRandom3.h>
#include<ctime>
#include "TH2Poly.h"
#include "TGraphErrors.h"
#include "TMatrixDSym.h"

// Fit includes
#include "FitEvent.h"
#include "FitParameters.h"
#include "FitLogger.h"
#include "StatUtils.h"


/*!
 *  \addtogroup Utils
 *  @{
 */

//! Functions to handle different TH1s and TFiles
namespace PlotUtils{

  /*!
    MISC Functions
  */
  
  //! Check the root file has an object containing the given substring in its name
  bool CheckObjectWithName(TFile *inFile, std::string substring);
  
  //! Get object in a TFile that has  a matching substring in its name
  std::string GetObjectWithName(TFile *inFile, std::string substring);

  //! Parse a string into a vector of doubles given a delimiter "del"
  std::vector<double> FillVectorDFromString(std::string str, const char* del);

  //! Parse a string into a vector of strings given a delimiter "del"  
  std::vector<std::string> FillVectorSFromString(std::string str, const char* del);

  /*!
    Interaction Mode Histogram Handling
  */

  //! Fill the Histogram Array with 61 new histograms for interaction channels.
  void CreateNeutModeArray(TH1* hist, TH1* neutarray[]);

  //! Call Fill on the relevent Mode 1D Histogram
  void FillNeutModeArray(TH1D* hist[], int mode, double xval, double weight = 1.0);

  //! Call Fill on the relevant Mode 2D Histogram 
  void FillNeutModeArray(TH2D* hist[], int mode, double xval, double yval, double weight = 1.0);

  //! Given two arrays of histograms for the NEUT interaction mode, hist1 = hist1 + scale*hist2
  void AddNeutModeArray(TH1D* hist1[], TH1D* hist2[], double scaling = 1.0);
  
  //! Generate a legend for the THStack
  TLegend GenerateStackLegend(THStack stack, int xlow, int ylow, int xhigh, int yhigh);

  //! Turn the array of TH1 histograms into a stack of Modes
  THStack GetNeutModeStack(std::string title,TH1* ModeStack[], int option);

  //! Reset each histogram in the mode array
  void ResetNeutModeArray(TH1* hist[]);

  //! Scale each histogram in the mode array by a single scaling factor, option can be used to define "width" scaling.
  void ScaleNeutModeArray(TH1* hist[], double factor, std::string option = "");
  
  //! Free up the memory used by each of the 61 mode histograms
  void DeleteNeutModeArray(TH1* neutarray[]);


  /*!
    Handling Functions
  */

  //! Fill a mask histogram from a text file
  TH2D* SetMaskHist(std::string type, TH2D* data);

  //! Divide by the flux histogram for Enu Histograms
  void DivideByFlux(TH1D* mcHist, TH1D* fluxHist);

  //! Flux unfolded scaling, like DivideByFlux but uses interpolation.
  void FluxUnfoldedScaling(TH1D* plot, TH1D* flux);

  //! Flux unfolded scaling for 2D histograms
  void FluxUnfoldedScaling(TH2D* plot, TH1D* flux, int axis = 0);
  
  //! Fill a 2D Histogram from a text file
  void Set2DHistFromText(std::string dataFile, TH2* hist, double norm, bool skipbins=false);

  //! Fill a 2D Poly Histogram from a text file
  void Set2PolyHistFromText(std::string dataFile, TH2Poly* hist, double norm, bool skipbins=false);

  //! Fill a 1D Histogram from a text file
  TH1D* GetTH1DFromFile(std::string dataFile, std::string title, std::string plotTitles="", std::string alt_name="");

  //! Grab a 1D Histrogram from a ROOT File
  TH1D* GetTH1DFromRootFile(std::string file, std::string name);

  //! Scale mc to match data considering empty and masked bins
  void ScaleToData(TH1D* data, TH1D* mc, TH1I* mask);

  //! Apply bin masking (More cosmetic)
  void MaskBins(TH1D* hist, TH1I* mask);

  //! Apply bin masking (More cosmetic)                                                                            
  void MaskBins(TH2D* hist, TH2I* mask);

  //! Get the data MC ratio considering empty and masked bins
  double GetDataMCRatio(TH1D* data, TH1D* mc, TH1I* mask=NULL);

  
  /*!
    Formatting Plot Utils
  */

  //! Create new ratio plot. hist3 = hist1/hist2
  TH1D* GetRatioPlot(TH1D* hist1,   TH1D* hist2);

  //! Create new plot of hist2 normalised to hist1. hist2 = hist1 * (Integral(hist1)/Integral(hist2))
  TH1D* GetRenormalisedPlot(TH1D* hist1,   TH1D* hist2);

  //! Normalise histogram to area of unity.
  TH1D* GetShapePlot(TH1D* hist1);

  //! Normalise hist1 and hist2 to unity, before creating a new plot of their ratio.
  TH1D* GetShapeRatio(TH1D* hist1,   TH1D* hist2);

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

  //! Given a covariance histogram, calculate the decomposed matrix to form a decomposed plot.
  TH2D* GetDecompPlot(TH2D* cov, std::string name);

  //! Given two 1D histograms create a 2D histogram which uses their bin edges to define both axes.
  TH2D* MergeIntoTH2D(TH1D* xhist, TH1D* yhist, std::string zname="");

  //! Given a 1D Histogram, set any empty bins in Data to empty bins in MC
  void MatchEmptyBins(TH1D* data, TH1D* mc);
  
  //! Given a 2D Histogram, set any empty bins in Data to empty bins in MC  
  void MatchEmptyBins(TH2D* data, TH2D* mc);

  //! Return a projection of a 2D Histogram onto X accounting for bin masking 
  TH1D* GetProjectionX(TH2D* hist, TH2I* mask);
  
  //! Return a projection of a 2D Histogram onto Y accounting for bin masking
  TH1D* GetProjectionY(TH2D* hist, TH2I* mask);
  
}

/*! @} */
#endif
