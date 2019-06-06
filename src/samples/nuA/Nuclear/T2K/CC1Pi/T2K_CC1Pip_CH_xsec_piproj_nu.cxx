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

#include "samples/MultiDataComparison.hxx"
#include "samples/SimpleDataComparison.hxx"

#include "utility/EventTopologyUtility.hxx"
#include "utility/FullEventUtility.hxx"
#include "utility/KinematicUtility.hxx"
#include "utility/PDGCodeUtility.hxx"
#include "utility/experimental/T2KUtility.hxx"

using namespace nuis::event;
using namespace nuis::utility;

class T2K_CC1Pip_CH_xsec_piproj_nu : public MultiDataComparison {

public:
  T2K_CC1Pip_CH_xsec_piproj_nu()
      : MultiDataComparison("T2K_CC1Pip_CH_xsec_piproj_nu") {
    ReadGlobalConfigDefaults();
  }

  enum distribution {
    kMomentumPion, kPmuThetamu, kPhi_adler, kQ2, kTheta_adler, kThetapimu, kThetapion
  };

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    // Perform any per-sample configuration in the base class
    MultiDataComparison::Initialize(instance_sample_configuration);

// PmuThetamu.root
// MomentumPion.root       Thetapimu.root
// MomentumPion.txt    Q2.root             Thetapimu.txt
// phi_adler.root      Q2.txt              Thetapion.root
// phi_adler.txt       theta_adler.root    Thetapion.txt

    std::unique_ptr<SimpleDataComparison_1D> MomentumPion(
        new SimpleDataComparison_1D("T2K_CC1Pip_CH_xsec_piproj_nu:MomentumPion"));
    MomentumPion->SetData(
        GetDataDir() +
        "nuA/Nuclear/T2K/CC0Pi/NProt_CH_xsec_STV_nu/MomentumPion.root;Momentum_pion (GeV)");
    MomentumPion->Initialize(
        instance_sample_configuration.get<fhicl::ParameterSet>("MomentumPion", {}));
    MomentumPion->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {GetHMFSParticle(fev,{pdgcodes::kPiPlus}).P() * 1E-3};
    };

    std::string wd_stub = write_directory.size() ? write_directory + "/" : "";
    MomentumPion->write_directory = wd_stub + "MomentumPion";

    Comparisons.emplace_back("MomentumPion", std::move(MomentumPion));

    //! Define your event signal here.
    IsSigFunc = [](FullEvent const &fev) -> bool {
      // Is numuCC
      if (GetNParticles(fev, {pdgcodes::kMu}) != 1) {
        return false;
      }
      return t2k::IsCC1Pip_CH_RecPi(fev);
    };
  }

  //! Here you can write any custom histograms to TTrees that your sample has
  //! been handling.
  void Write() {
    for (auto &comp : Comparisons) {
      comp.second->Write();
    }
  }
};

//! These declarations allow your class to be loaded dynamically by NUISANCE
DECLARE_PLUGIN(IDataComparison, T2K_CC1Pip_CH_xsec_piproj_nu);
DECLARE_PLUGIN(IEventProcessor, T2K_CC1Pip_CH_xsec_piproj_nu);
