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

  ANL_CCQE_Evt_1DQ2_nu() : Pub(kPRD26), Pub_str(""), UseD2Corr(false) {
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

  void Initialize(fhicl::ParameterSet const &ps) {

    if (ps.has_key("verbosity")) {
      SetSampleVerbosity(ps.get<std::string>("verbosity"));
    }

    std::string publication = ps.get<std::string>("publication", "PRD26");
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
      EnuRange = std::pair<double, double>{0, 3E3};
      ISAMPLE_INFO("Sample " << Name()
                             << " specialized for publication: " << Pub_str);
      break;
    }
    case kPRD16: {
      Pub_str = "PRD16_3103";
      EnuRange = std::pair<double, double>{0, 6E3};
      ISAMPLE_INFO("Sample " << Name()
                             << " specialized for publication: " << Pub_str);
      break;
    }
    case kPRD26: {
      Pub_str = "PRD26_537";
      EnuRange = std::pair<double, double>{0, 6E3};
      ISAMPLE_INFO("Sample " << Name()
                             << " specialized for publication: " << Pub_str);
      break;
    }
    }

    fhicl::ParameterSet const &global_sample_configuration =
        nuis::config::GetDocument().get<fhicl::ParameterSet>(
            std::string("global.sample_configuration.") + Name(),
            fhicl::ParameterSet());

    UseD2Corr = ps.get<bool>(
        "use_D2_correction",
        global_sample_configuration.get<bool>("use_D2_correction", false));

    SetData(GetDataDir() + "nuA/BubbleChamber/ANL/CCQE/ANL_CCQE_Data_" +
            Pub_str + ".root;ANL_1DQ2_Data");

    SimpleDataComparison_1D::Initialize(ps);

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

      if ((ISNumu.P4.E() < EnuRange.first) ||
          (ISNumu.P4.E() > EnuRange.second)) {
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
      return {{GetNeutrinoQ2QERec(fev, 0)}};
    };
  }

  std::string Name() { return "ANL_CCQE_Evt_1DQ2_nu"; }
};

DECLARE_PLUGIN(IDataComparison, ANL_CCQE_Evt_1DQ2_nu);
DECLARE_PLUGIN(ISample, ANL_CCQE_Evt_1DQ2_nu);
