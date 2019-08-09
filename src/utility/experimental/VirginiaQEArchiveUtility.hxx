#pragma once

#include "exception/exception.hxx"

#include <array>
#include <string>
#include <vector>

namespace nuis {
namespace utility {
namespace VQE {
class VirginiaQEArchiveInterface {
public:
  struct Dataset {
    std::string Author;
    size_t Year;
    std::string Citation;
    size_t TargetPDG;
    double EIn_GeV;
    double Angle_Deg;
    std::vector<std::array<double, 3>> Datapoints;
  };

private:
  VirginiaQEArchiveInterface();
  NEW_NUIS_EXCEPT(InvalidVirginiaQEDatasetReq);

  std::vector<Dataset> data;

  void Populate(std::vector<std::string> fcl_databases);

  static VirginiaQEArchiveInterface *_global_inst;

public:
  static VirginiaQEArchiveInterface &Get();

  using DsId_t = size_t;

  std::vector<DsId_t> GetMatchingDatasets_ZE(size_t Z, double E_GeV) const;

  std::vector<DsId_t> GetMatchingDatasets(size_t Z, double E_GeV,
                                          double Theta_deg = -1,
                                          std::string Author = "",
                                          size_t Year = 0) const;

  size_t GetNDatasets() const { return data.size(); }
  Dataset const &GetDataset(DsId_t it) const {
    if (it >= GetNDatasets()) {
      throw InvalidVirginiaQEDatasetReq()
          << "Requested dataset with ID " << it << ", but only have "
          << GetNDatasets() << " datasets.";
    }
    return data[it];
  }
};
} // namespace VQE
} // namespace utility
} // namespace nuis
