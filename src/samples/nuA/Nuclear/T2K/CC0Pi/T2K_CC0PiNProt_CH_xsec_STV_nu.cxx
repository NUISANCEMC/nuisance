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

class T2K_CC0PiNProt_CH_xsec_STV_nu : public MultiDataComparison {

public:
  T2K_CC0PiNProt_CH_xsec_STV_nu()
      : MultiDataComparison("T2K_CC0PiNProt_CH_xsec_STV_nu") {
    ReadGlobalConfigDefaults();
  }

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    // Perform any per-sample configuration in the base class
    MultiDataComparison::Initialize(instance_sample_configuration);

    std::unique_ptr<SimpleDataComparison_1D> DPT(
        new SimpleDataComparison_1D("T2K_CC0PiNProt_CH_xsec_STV_nu:dpt"));
    DPT->SetData(
        GetDataDir() +
        "nuA/Nuclear/T2K/CC0Pi/NProt_CH_xsec_STV_nu/dptResults.root;Result");
    DPT->Initialize(
        instance_sample_configuration.get<fhicl::ParameterSet>("dpt", {}));
    DPT->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {GetDeltaPT_CC0PiN(fev).Mag() * 1E-3};
    };

    std::unique_ptr<SimpleDataComparison_1D> DAT(
        new SimpleDataComparison_1D("T2K_CC0PiNProt_CH_xsec_STV_nu:dat"));
    DAT->SetData(
        GetDataDir() +
        "nuA/Nuclear/T2K/CC0Pi/NProt_CH_xsec_STV_nu/datResults.root;Result");
    DAT->Initialize(
        instance_sample_configuration.get<fhicl::ParameterSet>("dat", {}));
    DAT->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {GetDeltaAlphaT_CC0PiN(fev)};
    };

    std::unique_ptr<SimpleDataComparison_1D> DPhiT(
        new SimpleDataComparison_1D("T2K_CC0PiNProt_CH_xsec_STV_nu:dphit"));
    DPhiT->SetData(
        GetDataDir() +
        "nuA/Nuclear/T2K/CC0Pi/NProt_CH_xsec_STV_nu/dphitResults.root;Result");
    DPhiT->Initialize(
        instance_sample_configuration.get<fhicl::ParameterSet>("dphit", {}));
    DPhiT->CompProjFunc = [](FullEvent const &fev)
        -> std::array<double, SimpleDataComparison_1D::NDim> {
      return {GetDeltaPhiT_CC0PiN(fev)};
    };

    std::string wd_stub = write_directory.size() ? write_directory + "/" : "";
    DPT->write_directory = wd_stub + "dpt";
    DAT->write_directory = wd_stub + "dat";
    DPhiT->write_directory = wd_stub + "dphit";

    Comparisons.emplace_back("dpt", std::move(DPT));
    Comparisons.emplace_back("dat", std::move(DAT));
    Comparisons.emplace_back("dphit", std::move(DPhiT));

    //! Define your event signal here.
    IsSigFunc = [](FullEvent const &fev) -> bool {
      // Is numuCC
      if (GetNParticles(fev, {pdgcodes::kMu}) != 1) {
        return false;
      }
      return t2k::IsCC0Pi_STV(fev);
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
DECLARE_PLUGIN(IDataComparison, T2K_CC0PiNProt_CH_xsec_STV_nu);
DECLARE_PLUGIN(IEventProcessor, T2K_CC0PiNProt_CH_xsec_STV_nu);
