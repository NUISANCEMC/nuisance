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

#ifndef ElectronScattering_DurhamData_H_SEEN
#define ElectronScattering_DurhamData_H_SEEN

#include "Measurement1D.h"
#include "TH3D.h"

//********************************************************************
class ElectronScattering_DurhamData : public Measurement1D {
//********************************************************************

public:

  ElectronScattering_DurhamData(nuiskey samplekey);
  virtual ~ElectronScattering_DurhamData() {};

  void FillEventVariables(FitEvent *event);
  void FillHistograms();
  bool isSignal(FitEvent *event);
  void ScaleEvents(); // Converts TH3D to TH1D
  void ResetAll();
  void ApplyNormScale(double norm);
  void Write(std::string drawOpts);
void SetDataFromName(std::string name);
  int GetNDOF();
  double GetLikelihood();
  void SetFitOptions(std::string opt);
  // MeasurementVariableBox* CreateBox() {return new MeasurementVariableBox1D();};
  // ElectronVariableBox* GetBox() { return static_cast<ElectronVariableBox*>(MeasurementBase::GetBox()); };

  TH1D* GetMCHistogram(void);
  TH1D* GetDataHistogram(void);

private:

  TH3D* fMCScan_Q0vsThetavsE;
  TH2D* fMCScan_Q0vsTheta;
  TH2D* fMCScan_Q0vsE;
  // TH1D* fMCHist;
  // TH1D* fDataHist;
  TGraphErrors* fDataGraph;

  double fXLowLim, fXHighLim, fYLowLim, fYHighLim, fZLowLim, fZHighLim;
  double fYCenter, fZCenter;
};

#endif
