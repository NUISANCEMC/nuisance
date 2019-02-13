// Copyright 2018 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

//********************************************************************

#include "samples/SimpleDataComparison.hxx"

#include "utility/EventTopologyUtility.hxx"
#include "utility/FullEventUtility.hxx"
#include "utility/KinematicUtility.hxx"
#include "utility/PDGCodeUtility.hxx"

using namespace nuis::event;
using namespace nuis::utility;

class T2K_CC0Pi_H2O_xsec_2Dpcthetamu_numubar
    : public SimpleDataComparison_2DPoly {

  std::unique_ptr<TH1> fPrediction_pmu;
  std::unique_ptr<TH1> fPrediction_ctheta;

  std::unique_ptr<TH2> fPrediction_fine;
  std::unique_ptr<HistType> fPrediction_unscale;

public:
  T2K_CC0Pi_H2O_xsec_2Dpcthetamu_numubar() { ReadGlobalConfigDefaults(); }

  std::string GetDocumentation() { return ""; }
  fhicl::ParameterSet GetExampleConfiguration() {
    fhicl::ParameterSet exps =
        SimpleDataComparison_2DPoly::GetExampleConfiguration();

    return exps;
  }

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    SetData(
        GetDataDir() +
        "nuA/Nuclear/T2K/CC0Pi/H2O_xsec_2Dpmuthetamu_numubar.root;datapoly");

    fPrediction_fine = std::unique_ptr<TH2D>(
        new TH2D("fPrediction_fine",
                 ";#it{p}_{#mu} "
                 "(MeV/#it{c});cos(#theta_{#mu});d^{2}#sigma/"
                 "d#it{p}_{#mu}dcos(#theta_{#mu}) (cm^{2} MeV^{-1} A^{-1})",
                 100, 0, 3000, 50, 0.84, 1));

    fPrediction_pmu =
        std::unique_ptr<TH1D>(new TH1D("fPrediction_pmu",
                                       ";#it{p}_{#mu} "
                                       "(MeV/#it{c});d#sigma/"
                                       "d#it{p}_{#mu} (cm^{2} MeV^{-1} A^{-1})",
                                       100, 0, 3000));
    fPrediction_ctheta =
        std::unique_ptr<TH1D>(new TH1D("fPrediction_ctheta",
                                       ";cos(#theta_{#mu});d#sigma/"
                                       "ddcos(#theta_{#mu}) (cm^{2} A^{-1})",
                                       50, 0.84, 1));

    SimpleDataComparison_2DPoly::Initialize(instance_sample_configuration);

    fPrediction_unscale =
        nuis::utility::Clone(fPrediction, false, "Prediction_unscale");

    // Signal selection function
    IsSigFunc = [](FullEvent const &fev) -> bool {
      if (!IsCC0Pi(fev)) {
        return false;
      }

      Particle ISNumuBar = GetHMISParticle(fev, {pdgcodes::kNuMuBar});
      if (!ISNumuBar) {
        return false;
      }

      Particle FSMuPlus = GetHMFSParticle(fev, {pdgcodes::kMuPlus});
      if (!FSMuPlus) {
        return false;
      }

      if (FSMuPlus.CosTheta() < 0.84) {
        return false;
      }

      return true;
    };
    // 1D Projection function
    CompProjFunc = [](FullEvent const &fev) -> std::array<double, 2> {
      Particle FSMuPlus = GetHMFSParticle(fev, {pdgcodes::kMuPlus});
      return {FSMuPlus.P(), FSMuPlus.CosTheta()};
    };

    ProcessExtraFunc = [&](FullEvent const &fev, bool isSel, double weight) {
      if (isSel) {
        Particle FSMuPlus = GetHMFSParticle(fev, {pdgcodes::kMuPlus});
        TH_Helper<TH2>::Fill(fPrediction_fine,
                             {FSMuPlus.P(), FSMuPlus.CosTheta()}, weight);
        TH_Help::Fill(fPrediction_unscale, {FSMuPlus.P(), FSMuPlus.CosTheta()});
      }

      Particle ISNumuBar = GetHMISParticle(fev, {pdgcodes::kNuMuBar});
      if (!ISNumuBar) {
        return;
      }

      Particle FSMuPlus = GetHMFSParticle(fev, {pdgcodes::kMuPlus});
      if (!FSMuPlus) {
        return;
      }

      TH_Helper<TH1>::Fill(fPrediction_pmu, {FSMuPlus.P()}, weight);
      TH_Helper<TH1>::Fill(fPrediction_ctheta, {FSMuPlus.CosTheta()}, weight);
    };
  }

  std::string Name() { return "T2K_CC0Pi_H2O_xsec_2Dpcthetamu_numubar"; }

  void Write() {
    SimpleDataComparison_2DPoly::Write();
    nuis::persistency::WriteToOutputFile<TH2>(
        fPrediction_fine, "Prediction_fine", write_directory);
    nuis::persistency::WriteToOutputFile<TH1>(
        fPrediction_pmu, "fPrediction_pmu", write_directory);
    nuis::persistency::WriteToOutputFile<TH1>(
        fPrediction_ctheta, "fPrediction_ctheta", write_directory);

    nuis::persistency::WriteToOutputFile<HistType>(
        fPrediction_unscale, "Prediction_unscale", write_directory);

    static std::vector<std::vector<nuis::utility::PolyBinSpecifier>> const
        binning = {
            {// Slice_0
             YPolyBinSpec(450, 0.85), YPolyBinSpec(450, 0.95)},
            {// Slice_1
             YPolyBinSpec(550, 0.86), YPolyBinSpec(550, 0.93),
             YPolyBinSpec(550, 0.97)},
            {// Slice_2
             YPolyBinSpec(700, 0.89), YPolyBinSpec(700, 0.94),
             YPolyBinSpec(700, 0.98)},
            {// Slice_3
             YPolyBinSpec(900, 0.91), YPolyBinSpec(900, 0.95),
             YPolyBinSpec(900, 0.98)},
            {// Slice_4
             YPolyBinSpec(1200, 0.92), YPolyBinSpec(1200, 0.96),
             YPolyBinSpec(1200, 0.98)},
            {// Slice_4
             YPolyBinSpec(1600, 0.93), YPolyBinSpec(1600, 0.97),
             YPolyBinSpec(1600, 0.99)},
            {// Slice_5
             YPolyBinSpec(2500, 0.96), YPolyBinSpec(2500, 0.99)},
        };

    for (auto &slice : GetTH2PolySlices(fData, binning)) {
      nuis::persistency::WriteToOutputFile<TH1>(slice, slice->GetName(),
                                                write_directory);
    }
    for (auto &slice : GetTH2PolySlices(fPrediction_xsec, binning)) {
      nuis::persistency::WriteToOutputFile<TH1>(slice, slice->GetName(),
                                                write_directory);
    }
  }
};

DECLARE_PLUGIN(IDataComparison, T2K_CC0Pi_H2O_xsec_2Dpcthetamu_numubar);
DECLARE_PLUGIN(IEventProcessor, T2K_CC0Pi_H2O_xsec_2Dpcthetamu_numubar);
