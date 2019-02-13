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

class T2K_CC0PiNProt_CH_xsec_STV_nu : public SimpleDataComparison_1D {

  enum STVProjection { kDeltaPT, kDeltaPhiT, kDeltaAlphaT };

public:
  T2K_CC0PiNProt_CH_xsec_STV_nu() { ReadGlobalConfigDefaults(); }

  std::string GetDocumentation() {
    return "Can specify \"projection: <PROJ>\", where <PROJ> is one of [ "
           "DeltaPT, DeltaPhiT, DeltaAlphaT ] to clarify a projection for "
           "comparison. Defaults to DeltaPT.\n";
  }

  fhicl::ParameterSet GetExampleConfiguration() {
    fhicl::ParameterSet exps =
        SimpleDataComparison_1D::GetExampleConfiguration();

    exps.put<std::string>("projection", "DeltaPT");

    return exps;
  }

  NEW_NUIS_EXCEPT(invalid_projection_specifier);

  STVProjection GetProjection(std::string const &pstring) {
    if ((pstring == "DeltaPT") || (pstring == "deltapt") ||
        (pstring == "dpt")) {
      return kDeltaPT;
    } else if ((pstring == "DeltaPhiT") || (pstring == "deltaphit") ||
               (pstring == "dphit")) {
      return kDeltaPhiT;
    } else if ((pstring == "DeltaAlphaT") || (pstring == "deltaalphat") ||
               (pstring == "dat")) {
      return kDeltaAlphaT;
    }
    throw invalid_projection_specifier()
        << "Unknown projection " << std::quoted(pstring)
        << " for comparison: " << std::quoted(Name());
  }

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    //! Set the verbosity of the sample logging macros.
    if (instance_sample_configuration.has_key("verbosity")) {
      SetSampleVerbosity(
          instance_sample_configuration.get<std::string>("verbosity"));
    }

    STVProjection proj =
        GetProjection(instance_sample_configuration.get<std::string>(
            "projection", "DeltaPT"));

    //! This will automatically set the data histogram to be loaded.
    SetData(instance_sample_configuration.get<std::string>("data_specifier"));

    std::string projstr = "";
    switch (proj) {
    case kDeltaPT: {
      projstr = "dpt";
      break;
    }
    case kDeltaPhiT: {
      projstr = "dphit";
      break;
    }
    case kDeltaAlphaT: {
      projstr = "dat";
      break;
    }
    }

    SetData(GetDataDir() + "nuA/Nuclear/T2K/CC0Pi/" + projstr +
            "Results.root;Result");

    // Perform any per-sample configuration in the base class
    SimpleDataComparison_1D::Initialize(instance_sample_configuration);

    //! Define your event signal here.
    IsSigFunc = [](FullEvent const &fev) -> bool {
      //! See src/utility/EventTopologyUtility.hxx for more pre-defined
      //! topological signals.
      if (!IsCC0Pi(fev)) {
        return false;
      }

      //! See src/event/FullEvent.hxx for the full event class definition.
      //! See src/utility/FullEventUtility.hxx for more helper methods for
      //! interacting with the event class.

      //! Get the initial state muon neutrino
      Particle ISNumu = GetHMISParticle(fev, {pdgcodes::kNuMu});
      //! An nuis::event::Particle that return true for !part is invalid and
      //! does not exist on the particle stack. i.e. here, the selection fails
      //! if the event didn't have an initial state numubar.
      if (!ISNumu) {
        return false;
      }

      //! Get the final state muon
      Particle FSMu = GetHMFSParticle(fev, {pdgcodes::kMu});
      if (!FSMu) {
        return false;
      }

      //! Get the highest momentum final state proton
      Particle FSProton = GetHMFSParticle(fev, {pdgcodes::kProton});
      if (!FSProton) {
        return false;
      }

      //! Cut on kinematic properties of the true final state.

      // Muon phase space
      // Pmu > 250 MeV, cos(theta_mu) > -0.6 (Sweet phase space!)
      if ((FSMu.P() < 250) || (cos(ISNumu.P3().Angle(FSMu.P3())) < -0.6)) {
        return false;
      }

      // Proton phase space
      // Pprot > 450 MeV, cos(theta_proton) > 0.4
      if ((FSProton.P() < 450) || (FSProton.P() > 1E3) ||
          (cos(ISNumu.P3().Angle(FSProton.P3())) < 0.4)) {
        return false;
      }

      //! Select the event!
      return true;
    };

    //! 1D Projection function
    //! This function takes selected events and returns an array of size, the
    //! dimensionality of the comparisons (SimpleDataComparison_1D::NDim)
    CompProjFunc = [=](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      switch (proj) {
      case kDeltaPT: {
        return {GetDeltaPT_CC0PiN(fev).Mag() * 1E-3};
      }
      case kDeltaPhiT: {
        return {GetDeltaPhiT_CC0PiN(fev)};
      }
      case kDeltaAlphaT: {
        return {GetDeltaAlphaT_CC0PiN(fev)};
      }
      }
      return {-std::numeric_limits<double>::max()};
    };
  }

  std::string Name() { return "T2K_CC0PiNProt_CH_xsec_STV_nu"; }

  //! Here you can write any custom histograms to TTrees that your sample has
  //! been handling.
  void Write() { SimpleDataComparison_1D::Write(); }
};

//! These declarations allow your class to be loaded dynamically by NUISANCE
DECLARE_PLUGIN(IDataComparison, T2K_CC0PiNProt_CH_xsec_STV_nu);
DECLARE_PLUGIN(IEventProcessor, T2K_CC0PiNProt_CH_xsec_STV_nu);
