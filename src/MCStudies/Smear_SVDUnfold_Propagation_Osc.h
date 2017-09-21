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

#ifndef Smear_SVDUnfold_Propagation_Osc_H_SEEN
#define Smear_SVDUnfold_Propagation_Osc_H_SEEN

#include "Measurement1D.h"
/// Class for building simple oscillation analyses.
///
/// An example tag which runs a FHC beam and performs a numudisp and nue app
/// measurement might look like:
///
/// <sample name="Smear_SVDUnfold_Propagation_Osc" rename="FHC"
/// input="HISTO:NDObs.root[NDRate|NDSmearing]" DetectorVolume=""
/// DetectorDensity="" TruncateStart="0" TruncateUpTo="10">
///   <NDObs ObsInput="NDObs.root[NDRate_nue|NDSmearing_nue]" NuPDG="12" />
///   <FDObs ObsInput="FDObs.root[FDRate_numu|FDSmearing_numu]"
///   FitRegion_min="0"
///   FitRegion_max="7" OscillateToPDG="14" FDNDRatio="<1/1200*1200 [km^2]>"
///   DetectorVolume="" DetectorDensity=""/>
///   <FDObs ObsInput="FDObs.root[FDRate_nue|FDSmearing_nue]" FitRegion_min="0"
///   FitRegion_max="5" OscillateToPDG="12" FDNDRatio="<1/1200*1200 [km^2]>"
///   DetectorVolume="" DetectorDensity=""/>
/// </sample>
class Smear_SVDUnfold_Propagation_Osc : public Measurement1D {
  struct FDSample {
    TH1D *FDDataHist;

    std::vector<TH1D *> FD_Propagated_Spectrum_Hist_NDSamples;
    std::vector<TH1D *> NDFD_Corrected_Spectrum_Hist_NDSamples;

    TH1D *FD_Propagated_Spectrum_Hist;
    TH1D *NDFD_Corrected_Spectrum_Hist;
    TH1D *FD_Smeared_Spectrum_Hist;

    TH2D *SpectrumToFDSmearingMatrix_TH2;
    TMatrixD SpectrumToFDSmearingMatrix;
    Int_t OscillateToPDG;

    double FitRegion_Min;
    double FitRegion_Max;
    std::map<int,double> FDNDRatios;
    double FDNDMassRatio;

    std::pair<double, double> DetectorInfo;
  };

  struct NDSample {
    TH1D *NDDataHist;
    Int_t TruncateStart;
    Int_t TruncateUpTo;

    TH1D *ND_Unfolded_Spectrum_Hist;
    TH2D *NDToSpectrumSmearingMatrix;

    TMatrixD NDToSpectrumResponseMatrix;

    Int_t NuPDG;

    double XSecToEvRateScale;
  };

 public:
  Smear_SVDUnfold_Propagation_Osc(nuiskey samplekey);

  virtual ~Smear_SVDUnfold_Propagation_Osc(){};

  void FillEventVariables(FitEvent *event);
  bool isSignal(FitEvent *event);

  void UnfoldToNDETrueSpectrum(size_t);
  void ConvertEventRates(void);
  void Write(std::string drawOpt);

  void ReadExtraConfig(nuiskey &samplekey);
  void SetupChi2Hists();
  void UpdateChi2Hists();

  // ----- Near detector things
  void AddNDInputs(nuiskey &samplekey);
  void SetupNDInputs();
  std::vector<NDSample> NDSamples;

  std::pair<double, double> NDetectorInfo;
  double FitRegion_Min;
  double FitRegion_Max;

  // ----- Far detector things
  Int_t NFDAnalysisBins;
  Int_t GetFDSampleNAnalysisBins(size_t fds_it);
  void AddFDTarget(nuiskey &nk);
  void PropagateFDSample(size_t fds_it);
  void FinaliseFDSamples();
  std::vector<FDSample> FDSamples;

  double ScalePOT;

  // ----- Other config
  bool UseRateErrors;
};

#endif
