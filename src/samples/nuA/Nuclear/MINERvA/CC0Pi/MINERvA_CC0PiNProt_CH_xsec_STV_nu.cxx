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
#include "utility/experimental/MINERvAUtility.hxx"

using namespace nuis::event;
using namespace nuis::utility;

class MINERvA_CC0PiNProt_CH_xsec_STV_nu : public MultiDataComparison {

public:
  MINERvA_CC0PiNProt_CH_xsec_STV_nu()
      : MultiDataComparison("MINERvA_CC0PiNProt_CH_xsec_STV_nu") {
    ReadGlobalConfigDefaults();
  }

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    // Perform any per-sample configuration in the base class
    MultiDataComparison::Initialize(instance_sample_configuration);

    std::unique_ptr<SimpleDataComparison_1D> DPT(
        new SimpleDataComparison_1D("MINERvA_CC0PiNProt_CH_xsec_STV_nu:dpt"));
    DPT->SetData(GetDataDir() +
                 "nuA/Nuclear/MINERvA/CC0Pi/NProt_CH_xsec_STV_nu/"
                 "MINERvA_1805.05486.root;dpt");
    DPT->Initialize(
        instance_sample_configuration.get<fhicl::ParameterSet>("dpt", {}));
    DPT->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {mnv::GetDeltaPT_CC0PiN_mnv(fev).Mag() * 1E-3};
    };

    std::unique_ptr<SimpleDataComparison_1D> DAT(new SimpleDataComparison_1D(
        "MINERvA_CC0PiNProt_CH_xsec_STV_nu:dalphat"));
    DAT->SetData(GetDataDir() +
                 "nuA/Nuclear/MINERvA/CC0Pi/NProt_CH_xsec_STV_nu/"
                 "MINERvA_1805.05486.root;dalphat");
    DAT->Initialize(
        instance_sample_configuration.get<fhicl::ParameterSet>("dat", {}));
    DAT->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {mnv::GetDeltaAlphaT_CC0PiN_mnv(fev)};
    };

    std::unique_ptr<SimpleDataComparison_1D> DPhiT(
        new SimpleDataComparison_1D("MINERvA_CC0PiNProt_CH_xsec_STV_nu:dphit"));
    DPhiT->SetData(GetDataDir() +
                   "nuA/Nuclear/MINERvA/CC0Pi/NProt_CH_xsec_STV_nu/"
                   "MINERvA_1805.05486.root;dphit");
    DPhiT->Initialize(
        instance_sample_configuration.get<fhicl::ParameterSet>("dphit", {}));
    DPhiT->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {mnv::GetDeltaPhiT_CC0PiN_mnv(fev)};
    };

    std::unique_ptr<SimpleDataComparison_1D> MuMom(new SimpleDataComparison_1D(
        "MINERvA_CC0PiNProt_CH_xsec_STV_nu:muonmomentum"));
    MuMom->SetData(GetDataDir() +
                   "nuA/Nuclear/MINERvA/CC0Pi/NProt_CH_xsec_STV_nu/"
                   "MINERvA_1805.05486.root;muonmomentum");
    MuMom->Initialize(instance_sample_configuration.get<fhicl::ParameterSet>(
        "muonmomentum", {}));
    MuMom->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {GetHMFSChargedLepton(fev).P() * 1E-3};
    };
    std::unique_ptr<SimpleDataComparison_1D> MuTheta(
        new SimpleDataComparison_1D(
            "MINERvA_CC0PiNProt_CH_xsec_STV_nu:muontheta"));
    MuTheta->SetData(GetDataDir() +
                     "nuA/Nuclear/MINERvA/CC0Pi/NProt_CH_xsec_STV_nu/"
                     "MINERvA_1805.05486.root;muontheta");
    MuTheta->Initialize(instance_sample_configuration.get<fhicl::ParameterSet>(
        "muontheta", {}));
    MuTheta->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {GetHMFSChargedLepton(fev).Theta_deg()};
    };

    std::unique_ptr<SimpleDataComparison_1D> ProtonMom(
        new SimpleDataComparison_1D(
            "MINERvA_CC0PiNProt_CH_xsec_STV_nu:protonmomentum"));
    ProtonMom->SetData(GetDataDir() +
                       "nuA/Nuclear/MINERvA/CC0Pi/NProt_CH_xsec_STV_nu/"
                       "MINERvA_1805.05486.root;protonmomentum");
    ProtonMom->Initialize(
        instance_sample_configuration.get<fhicl::ParameterSet>("protonmomentum",
                                                               {}));
    ProtonMom->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {GetHMFSProtonInPhaseSpace(fev, mnv::CC0PiNProt_ProtonPS).P() *
              1E-3};
    };
    std::unique_ptr<SimpleDataComparison_1D> ProtonTheta(
        new SimpleDataComparison_1D(
            "MINERvA_CC0PiNProt_CH_xsec_STV_nu:protontheta"));
    ProtonTheta->SetData(GetDataDir() +
                         "nuA/Nuclear/MINERvA/CC0Pi/NProt_CH_xsec_STV_nu/"
                         "MINERvA_1805.05486.root;protontheta");
    ProtonTheta->Initialize(
        instance_sample_configuration.get<fhicl::ParameterSet>("protontheta",
                                                               {}));
    ProtonTheta->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {
          GetHMFSProtonInPhaseSpace(fev, mnv::CC0PiNProt_ProtonPS).Theta_deg()};
    };

    std::unique_ptr<SimpleDataComparison_1D> NeutronMomReco(
        new SimpleDataComparison_1D(
            "MINERvA_CC0PiNProt_CH_xsec_STV_nu:neutronmomentum"));
    NeutronMomReco->SetData(GetDataDir() +
                            "nuA/Nuclear/MINERvA/CC0Pi/NProt_CH_xsec_STV_nu/"
                            "MINERvA_1805.05486.root;neutronmomentum");
    NeutronMomReco->Initialize(
        instance_sample_configuration.get<fhicl::ParameterSet>(
            "neutronmomentum", {}));
    NeutronMomReco->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {mnv::GetNeutronMomentumReco_CC0PiN_mnv(fev)*1E-3};
    };

    std::string wd_stub = fWrite_directory.size() ? fWrite_directory + "/" : "";
    DPT->fWrite_directory = wd_stub + "dpt";
    DAT->fWrite_directory = wd_stub + "dat";
    DPhiT->fWrite_directory = wd_stub + "dphit";
    MuMom->fWrite_directory = wd_stub + "muonmomentum";
    MuTheta->fWrite_directory = wd_stub + "muontheta";
    ProtonMom->fWrite_directory = wd_stub + "protonmomentum";
    ProtonTheta->fWrite_directory = wd_stub + "protontheta";
    NeutronMomReco->fWrite_directory = wd_stub + "neutronmomentum";

    Comparisons.emplace_back("dpt", std::move(DPT));
    Comparisons.emplace_back("dat", std::move(DAT));
    Comparisons.emplace_back("dphit", std::move(DPhiT));
    Comparisons.emplace_back("muonmomentum", std::move(MuMom));
    Comparisons.emplace_back("muontheta", std::move(MuTheta));
    Comparisons.emplace_back("protonmomentum", std::move(ProtonMom));
    Comparisons.emplace_back("protontheta", std::move(ProtonTheta));
    Comparisons.emplace_back("neutronmomentum", std::move(NeutronMomReco));

    //! Define your event signal here.
    IsSigFunc = [](FullEvent const &fev) -> bool {
      // Is numuCC
      if (GetNParticles(fev, {pdgcodes::kMu}) != 1) {
        return false;
      }
      return mnv::IsCC0PiNp_STV(fev);
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
DECLARE_PLUGIN(IDataComparison, MINERvA_CC0PiNProt_CH_xsec_STV_nu);
DECLARE_PLUGIN(IEventProcessor, MINERvA_CC0PiNProt_CH_xsec_STV_nu);
