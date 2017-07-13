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

#ifndef OfficialNIWGPlots_H_SEEN
#define OfficialNIWGPlots_H_SEEN
#include "Measurement1D.h"
namespace MCStudies {


class OfficialNIWGStack : public StackBase {
public:

  /// Main constructor listing true mode categories.
  OfficialNIWGStack(std::string name, std::string title, TH1* hist);

  /// List to convert Modes to Index.
  /// Should be kept in sync with constructor.
  int ConvertModeToIndex(FitEvent* evt);
  /// Extracts Mode from FitEvent and fills
  void Fill(FitEvent* evt, double x, double y = 1.0, double z = 1.0, double weight = 1.0);


};

  class OfficialPionStack : public StackBase {
  public:

    /// Main constructor listing true mode categories.                                                                                                                                                                                        
    OfficialPionStack(std::string name, std::string title, TH1* hist);

    /// List to convert Modes to Index.                                                                                                                                                                                                       
    /// Should be kept in sync with constructor.                                                                                                                                                                                              
    int ConvertModeToIndex(FitEvent* evt);
    /// Extracts Mode from FitEvent and fills                                                                                                                                                                                                 
    void Fill(FitEvent* evt, double x, double y = 1.0, double z = 1.0, double weight = 1.0);


  };



};


//********************************************************************
class OfficialNIWGPlots : public Measurement1D {
//********************************************************************

public:

  OfficialNIWGPlots(nuiskey samplekey);
  virtual ~OfficialNIWGPlots() {};

  //! Grab info from event
  void FillEventVariables(FitEvent *event);

  void ScaleEvents();
  void ResetAll();

  //! Define this samples signal
  bool isSignal(FitEvent *nvect);

  //! Write Files
  void Write(std::string drawOpt);

private:
  double fScaleFactorDifXSec;
  double fScaleFactorEnuXSec;

  TH1D* fHist_Enu;
  MCStudies::OfficialNIWGStack* fHist_Enu_Modes;
  MCStudies::OfficialPionStack* fHist_Enu_Pions;

  TH1D* fHist_EnuRates;
  MCStudies::OfficialNIWGStack* fHist_EnuRates_Modes;
  MCStudies::OfficialPionStack* fHist_EnuRates_Pions;

  TH1D* fHist_Q2;
  MCStudies::OfficialNIWGStack* fHist_Q2_Modes;
  MCStudies::OfficialPionStack* fHist_Q2_Pions;

  TH1D* fHist_Pmu;
  MCStudies::OfficialNIWGStack* fHist_Pmu_Modes;
  MCStudies::OfficialPionStack* fHist_Pmu_Pions;

  TH1D* fHist_Cosmu;
  MCStudies::OfficialNIWGStack* fHist_Cosmu_Modes;
  MCStudies::OfficialPionStack* fHist_Cosmu_Pions;


};





#endif
