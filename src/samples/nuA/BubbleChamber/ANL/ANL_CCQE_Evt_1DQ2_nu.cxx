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

#include "utility/FullEventUtility.hxx"
#include "utility/KinematicUtility.hxx"
#include "utility/PDGCodeUtility.hxx"

using namespace nuis::event;
using namespace nuis::utility;

class ANL_CCQE_Evt_1DQ2_nu : public SimpleDataComparison_1D {

public:
  NEW_NUIS_EXCEPT(invalid_publication_specifier);

  enum Publication { kPRL31, kPRD16, kPRD26 };
  Publication Pub;
  std::string Pub_str;
  bool UseD2Corr;
  std::unique_ptr<HistType> fD2CorrHist;
  std::unique_ptr<HistType> fPrediction_Uncorr;

  ANL_CCQE_Evt_1DQ2_nu()
      : SimpleDataComparison_1D("ANL_CCQE_Evt_1DQ2_nu"), Pub(kPRD26),
        Pub_str(""), UseD2Corr(false), fD2CorrHist(nullptr) {
    ReadGlobalConfigDefaults();
  }

  std::string GetDocumentation() {
    return "Can specify \"publication: <PUB>\", where <PUB> is one of [ PRL31, "
           "PRD16, PRD26 ] to clarify a publication for comparison. Defaults "
           "to PRD26.\n"
           "Can enable deuterium Q2 correction by specifying "
           "\"use_D2_correction: true\"";
  }
  fhicl::ParameterSet GetExampleConfiguration() {
    fhicl::ParameterSet exps =
        SimpleDataComparison_1D::GetExampleConfiguration();

    exps.put<std::string>("publication", "PRD26");
    exps.put<bool>("use_D2_correction", false);

    return exps;
  }

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    std::string publication =
        instance_sample_configuration.get<std::string>("publication", "PRD26");
    if (publication == "PRL31") {
      Pub = kPRL31;
    } else if (publication == "PRD16") {
      Pub = kPRD16;
    } else if (publication == "PRD26") {
      Pub = kPRD26;
    } else {
      throw invalid_publication_specifier()
          << "[ERROR]: Found unexpected publication specifier "
          << std::quoted(publication)
          << ". Expected one of [ PRL31, PRD16, PRD26 ]";
    }

    switch (Pub) {
    case kPRL31: {
      Pub_str = "PRL31_844";
      fEnergyCut = std::pair<double, double>{0, 3E3};
      IEventProcessor_INFO(
          "Sample " << Name() << " specialized for publication: " << Pub_str);
      break;
    }
    case kPRD16: {
      Pub_str = "PRD16_3103";
      fEnergyCut = std::pair<double, double>{0, 6E3};
      IEventProcessor_INFO(
          "Sample " << Name() << " specialized for publication: " << Pub_str);
      break;
    }
    case kPRD26: {
      Pub_str = "PRD26_537";
      fEnergyCut = std::pair<double, double>{0, 6E3};
      IEventProcessor_INFO(
          "Sample " << Name() << " specialized for publication: " << Pub_str);
      break;
    }
    }

    fhicl::ParameterSet const &global_sample_configuration =
        nuis::config::GetDocument().get<fhicl::ParameterSet>(
            std::string("global.sample_configuration.") + Name(),
            fhicl::ParameterSet());

    SetData(GetDataDir() + "nuA/BubbleChamber/ANL/CCQE/ANL_CCQE_Data_" +
            Pub_str + ".root;ANL_1DQ2_Data");

    SimpleDataComparison_1D::Initialize(instance_sample_configuration);

    UseD2Corr = instance_sample_configuration.get<bool>(
        "use_D2_correction",
        global_sample_configuration.get<bool>("use_D2_correction", false));

    if (UseD2Corr) {
      fD2CorrHist = nuis::utility::GetHistogram<HistType>(
          GetDataDir() + "nuA/BubbleChamber/ANL/CCQE/"
                         "ANL_CCQE_Data_PRL31_844.root;ANL_1DQ2_Correction");
      fPrediction_Uncorr = Clone(fPrediction, true);
    }

    // Signal selection function
    IsSigFunc = [&](FullEvent const &fev) -> bool {
      if (fev.mode != Channel_t::kCCQE) {
        return false;
      }

      Particle ISNumu = GetHMISNeutralLepton(fev);

      if (!ISNumu) {
        return false;
      }

      if (ISNumu.pdg != pdgcodes::kNuMu) {
        return false;
      }

      if (!fEnergyCut.IsInRange(ISNumu.P4.E())) {
        return false;
      }

      double Q2 = GetNeutrinoQ2QERec(fev, 0);
      if (Q2 <= 0) {
        return false;
      }

      return true;
    };
    // 1D Projection function
    CompProjFunc = [](FullEvent const &fev) -> std::array<double, 1> {
      return {GetNeutrinoQ2QERec(fev, 0)};
    };
  }

  // Used to apply D2 correction if requested
  virtual void FillProjection(std::array<double, 1> const &proj,
                              double event_weight) {

    if (UseD2Corr) {
      TH_Help::Fill(fPrediction_Uncorr, proj, event_weight);
      event_weight *= fD2CorrHist->Interpolate(proj[0]);
    }
    TH_Help::Fill(fPrediction, proj, event_weight);
  }

  void FinalizeComparison() {
    SimpleDataComparison_1D::FinalizeComparison();
    if (UseD2Corr) {
      fPrediction_Uncorr->Scale(1.0, "width");
    }
  }

  void Write() {
    SimpleDataComparison_1D::Write();
    if (UseD2Corr) {
      nuis::persistency::WriteToOutputFile<HistType>(
          fPrediction_Uncorr, "Prediction_Uncorr", fWrite_directory);
    }
  }
};

DECLARE_PLUGIN(IDataComparison, ANL_CCQE_Evt_1DQ2_nu);
DECLARE_PLUGIN(IEventProcessor, ANL_CCQE_Evt_1DQ2_nu);
