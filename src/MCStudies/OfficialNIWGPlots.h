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

  TH1D* fHist_NuMu_Enu;
  MCStudies::OfficialNIWGStack* fHist_NuMu_Enu_Modes;
  MCStudies::OfficialPionStack* fHist_NuMu_Enu_Pions;

  TH1D* fHist_NuMu_EnuRates;
  MCStudies::OfficialNIWGStack* fHist_NuMu_EnuRates_Modes;
  MCStudies::OfficialPionStack* fHist_NuMu_EnuRates_Pions;

  TH1D* fHist_NuMu_Q2;
  MCStudies::OfficialNIWGStack* fHist_NuMu_Q2_Modes;
  MCStudies::OfficialPionStack* fHist_NuMu_Q2_Pions;

  TH1D* fHist_NuMu_Pmu;
  MCStudies::OfficialNIWGStack* fHist_NuMu_Pmu_Modes;
  MCStudies::OfficialPionStack* fHist_NuMu_Pmu_Pions;

  TH1D* fHist_NuMu_Cosmu;
  MCStudies::OfficialNIWGStack* fHist_NuMu_Cosmu_Modes;
  MCStudies::OfficialPionStack* fHist_NuMu_Cosmu_Pions;

  TH1D* fHist_NuMuBar_Enu;
  MCStudies::OfficialNIWGStack* fHist_NuMuBar_Enu_Modes;
  MCStudies::OfficialPionStack* fHist_NuMuBar_Enu_Pions;

  TH1D* fHist_NuMuBar_EnuRates;
  MCStudies::OfficialNIWGStack* fHist_NuMuBar_EnuRates_Modes;
  MCStudies::OfficialPionStack* fHist_NuMuBar_EnuRates_Pions;

  TH1D* fHist_NuMuBar_Q2;
  MCStudies::OfficialNIWGStack* fHist_NuMuBar_Q2_Modes;
  MCStudies::OfficialPionStack* fHist_NuMuBar_Q2_Pions;

  TH1D* fHist_NuMuBar_Pmu;
  MCStudies::OfficialNIWGStack* fHist_NuMuBar_Pmu_Modes;
  MCStudies::OfficialPionStack* fHist_NuMuBar_Pmu_Pions;

  TH1D* fHist_NuMuBar_Cosmu;
  MCStudies::OfficialNIWGStack* fHist_NuMuBar_Cosmu_Modes;
  MCStudies::OfficialPionStack* fHist_NuMuBar_Cosmu_Pions;

  TH1D* fHist_Nue_Enu;
  MCStudies::OfficialNIWGStack* fHist_Nue_Enu_Modes;
  MCStudies::OfficialPionStack* fHist_Nue_Enu_Pions;

  TH1D* fHist_Nue_EnuRates;
  MCStudies::OfficialNIWGStack* fHist_Nue_EnuRates_Modes;
  MCStudies::OfficialPionStack* fHist_Nue_EnuRates_Pions;

  TH1D* fHist_Nue_Q2;
  MCStudies::OfficialNIWGStack* fHist_Nue_Q2_Modes;
  MCStudies::OfficialPionStack* fHist_Nue_Q2_Pions;

  TH1D* fHist_Nue_Pmu;
  MCStudies::OfficialNIWGStack* fHist_Nue_Pmu_Modes;
  MCStudies::OfficialPionStack* fHist_Nue_Pmu_Pions;

  TH1D* fHist_Nue_Cosmu;
  MCStudies::OfficialNIWGStack* fHist_Nue_Cosmu_Modes;
  MCStudies::OfficialPionStack* fHist_Nue_Cosmu_Pions;

  TH1D* fHist_NueBar_Enu;
  MCStudies::OfficialNIWGStack* fHist_NueBar_Enu_Modes;
  MCStudies::OfficialPionStack* fHist_NueBar_Enu_Pions;

  TH1D* fHist_NueBar_EnuRates;
  MCStudies::OfficialNIWGStack* fHist_NueBar_EnuRates_Modes;
  MCStudies::OfficialPionStack* fHist_NueBar_EnuRates_Pions;

  TH1D* fHist_NueBar_Q2;
  MCStudies::OfficialNIWGStack* fHist_NueBar_Q2_Modes;
  MCStudies::OfficialPionStack* fHist_NueBar_Q2_Pions;

  TH1D* fHist_NueBar_Pmu;
  MCStudies::OfficialNIWGStack* fHist_NueBar_Pmu_Modes;
  MCStudies::OfficialPionStack* fHist_NueBar_Pmu_Pions;

  TH1D* fHist_NueBar_Cosmu;
  MCStudies::OfficialNIWGStack* fHist_NueBar_Cosmu_Modes;
  MCStudies::OfficialPionStack* fHist_NueBar_Cosmu_Pions;


};





#endif
