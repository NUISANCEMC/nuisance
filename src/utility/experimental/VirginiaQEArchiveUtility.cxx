#include "utility/experimental/VirginiaQEArchiveUtility.hxx"

#include "utility/FileSystemUtility.hxx"
#include "utility/PDGCodeUtility.hxx"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"

#include <cmath>

namespace nuis {
namespace utility {
namespace VQE {

VirginiaQEArchiveInterface *VirginiaQEArchiveInterface::_global_inst = nullptr;

VirginiaQEArchiveInterface::VirginiaQEArchiveInterface() {
  std::string const &dd = GetDataDir();
  Populate(std::vector<std::string>{dd + "eA/VirginiaArchive/12C.fcl",
                                    dd + "eA/VirginiaArchive/16O.fcl"});
}

void VirginiaQEArchiveInterface::Populate(
    std::vector<std::string> fcl_databases) {
  std::cout << "[INFO]: Populating Virginia QE archive database..."
            << std::endl;
  for (std::string const &dbname : fcl_databases) {
    auto const &db = fhicl::make_ParameterSet(dbname);
    size_t ctr = 0;
    for (auto const &ds_ps :
         db.get<std::vector<fhicl::ParameterSet>>("datasets")) {
      Dataset ds;
      ds.Author = ds_ps.get<std::string>("Author");
      ds.Year = ds_ps.get<size_t>("Year");
      ds.Citation = ds_ps.get<std::string>("Citation");
      auto const &ZA = ds_ps.get<std::array<size_t, 2>>("Target");
      ds.TargetPDG = MakeNuclearPDG(ZA[1], ZA[0]);
      ds.EIn_GeV = ds_ps.get<double>("EIn");
      ds.Angle_Deg = ds_ps.get<double>("AngOut");
      ds.Datapoints = ds_ps.get<std::vector<std::array<double, 3>>>("Data");
      data.push_back(ds);
    }
  }
  std::cout << "[INFO]: Done!" << std::endl;
}

VirginiaQEArchiveInterface &VirginiaQEArchiveInterface::Get() {
  if (!_global_inst) {
    _global_inst = new VirginiaQEArchiveInterface();
  }
  return *_global_inst;
}

std::vector<VirginiaQEArchiveInterface::DsId_t>
VirginiaQEArchiveInterface::GetMatchingDatasets_ZE(size_t Z,
                                                   double E_GeV) const {
  size_t NDatasets = data.size();
  std::vector<DsId_t> rtn;

  for (DsId_t it = 0; it < NDatasets; ++it) {
    Dataset const &ds = data[it];
    if (GetZ(ds.TargetPDG) != Z) {
      continue;
    }
    if (fabs(ds.EIn_GeV - E_GeV) > 1E-5) {
      continue;
    }
    rtn.push_back(it);
  }

  return rtn;
}
std::vector<VirginiaQEArchiveInterface::DsId_t>
VirginiaQEArchiveInterface::GetMatchingDatasets(size_t Z, double E_GeV,
                                                double Theta_deg,
                                                std::string Author,
                                                size_t Year) const {

  std::vector<DsId_t> match = GetMatchingDatasets_ZE(Z, E_GeV);

  size_t NDatasets = match.size();
  std::vector<DsId_t> rtn;

  for (size_t it = 0; it < NDatasets; ++it) {
    Dataset const &ds = data[match[it]];

    if ((ds.Angle_Deg > 0) && (fabs(ds.Angle_Deg - Theta_deg) > 1E-5)) {
      continue;
    }

    if (Author.size() && (ds.Author != Author)) {
      continue;
    }
    if (Year && (ds.Year != Year)) {
      continue;
    }
    rtn.push_back(match[it]);
  }

  return rtn;
}

} // namespace VQE
} // namespace utility
} // namespace nuis
