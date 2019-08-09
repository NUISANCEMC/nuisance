#include "config/GlobalConfiguration.hxx"

#include "utility/experimental/VirginiaQEArchiveUtility.hxx"

#include "utility/PDGCodeUtility.hxx"

#include "fhiclcpp/ParameterSet.h"

#include <iostream>

using namespace nuis::utility;
using namespace nuis::utility::VQE;
int main() {
  nuis::config::EnsureConfigurationRead("nuis.global.config.fcl");

  auto const &veqe = VirginiaQEArchiveInterface::Get();

  fhicl::ParameterSet ps;

  ps.put<bool>("shape_only", false);
  ps.put<bool>("flux_unfolded", false);

  std::vector<fhicl::ParameterSet> sub_samples;

  for (size_t it = 0; it < veqe.GetNDatasets(); ++it) {

    auto const &ds = veqe.GetDataset(it);

    fhicl::ParameterSet ss;
    ss.put<size_t>("TargetZ", GetZ(ds.TargetPDG));
    ss.put<double>("EIn_GeV", ds.EIn_GeV);
    ss.put<double>("Angle_Deg", ds.Angle_Deg);
    ss.put<std::string>("Author", ds.Author);
    ss.put<size_t>("Year", ds.Year);

    sub_samples.push_back(ss);
  }

  ps.put<std::vector<fhicl::ParameterSet>>("sub_samples", sub_samples);

  std::cout << "global.sample_configuration.VirginiaQEArchive: { "
            << ps.to_indented_string() << " }" << std::endl;
}
