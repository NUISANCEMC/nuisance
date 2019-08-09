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
#include "utility/HistogramUtility.hxx"
#include "utility/KinematicUtility.hxx"
#include "utility/PDGCodeUtility.hxx"
#include "utility/PhaseSpaceRestriction.hxx"
#include "utility/StringUtility.hxx"
#include "utility/UnitsUtility.hxx"

#include "utility/experimental/VirginiaQEArchiveUtility.hxx"

#include "TCanvas.h"
#include "TLatex.h"

using namespace nuis::event;
using namespace nuis::utility;
using namespace nuis::utility::VQE;

class VirginiaQEArchive : public SimpleDataComparison_1D {

public:
  NEW_NUIS_EXCEPT(invalid_dataset_specifier);
  NEW_NUIS_EXCEPT(ambiguous_dataset_specifier);

  std::vector<std::unique_ptr<TGraph>> fDataGraphs;
  std::vector<SimpleParticlePhaseSpaceRestriction> fAngleCuts;
  std::vector<decltype(fPrediction)> fPredictions;
  std::vector<VirginiaQEArchiveInterface::Dataset> fDatasets;
  size_t fNDataSets;
  std::string fPrint;

  SimpleParticlePhaseSpaceRestriction fEin_ps;

  VirginiaQEArchive() : SimpleDataComparison_1D("VirginiaQEArchive") {
    ReadGlobalConfigDefaults();
  }

  std::string GetDocumentation() { return ""; }
  fhicl::ParameterSet GetExampleConfiguration() {
    fhicl::ParameterSet exps =
        SimpleDataComparison_1D::GetExampleConfiguration();

    exps.put<std::string>("TargetZ", "<TargetZ, e.g. 6>");
    exps.put<std::string>("EIn_GeV", "<e.g. 1.2>");
    exps.put<std::string>("Angle_Deg", "<e.g. 32>");
    exps.put<std::string>("Author", "!Optional: Filter by author name");
    exps.put<std::string>("Year", "!Optional: Filter by year");
    exps.put<std::string>("require_unique",
                          "!Optional: require only a single dataset match.");

    return exps;
  }

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    fhicl::ParameterSet inst_paramset = instance_sample_configuration;

    size_t TargetZ = inst_paramset.get<size_t>("TargetZ");
    double EIn_GeV = inst_paramset.get<double>("EIn_GeV");
    double Angle_Deg = inst_paramset.get<double>("Angle_Deg", -1);

    fPrint = inst_paramset.get<std::string>("print", "");

    // For ease of use we may want to template the filename
    std::string file_template = inst_paramset.get<std::string>("file");

    file_template = str_replace(file_template, "%E",
                                inst_paramset.get<std::string>("EIn_GeV"));

    if (Angle_Deg != -1) {
      file_template = str_replace(file_template, "%A",
                                  inst_paramset.get<std::string>("Angle_Deg"));
    }
    inst_paramset.put_or_replace<std::string>("file", file_template);

    std::string Author = inst_paramset.get<std::string>("Author", "");
    size_t Year = inst_paramset.get<size_t>("Year", 0);

    auto const &veqe = VirginiaQEArchiveInterface::Get();

    bool require_unique = inst_paramset.get<bool>("require_unique", false);

    auto const &datasetids =
        veqe.GetMatchingDatasets(TargetZ, EIn_GeV, Angle_Deg, Author, Year);

    fNDataSets = datasetids.size();

    if (!fNDataSets) {
      invalid_dataset_specifier err;
      err << "When searching VirginiaQE archive with: {"
             " TargetZ: "
          << TargetZ << " EIn_GeV: " << EIn_GeV;
      if (Angle_Deg > 0) {
        err << " Angle_Deg: " << Angle_Deg;
      }
      if (Author.size()) {
        err << " Author: \"" << Author << "\"";
      }
      if (Year) {
        err << " Year: " << Year;
      }
      err << "}, matched " << fNDataSets
          << " datasets, but required at least 1.";
      throw err;
    }

    if (require_unique && (fNDataSets != 1)) {
      ambiguous_dataset_specifier err;
      err << "When searching VirginiaQE archive with: {"
             " TargetZ: "
          << TargetZ << " EIn_GeV: " << EIn_GeV;
      if (Angle_Deg > 0) {
        err << " Angle_Deg: " << Angle_Deg;
      }
      if (Author.size()) {
        err << " Author: \"" << Author << "\"";
      }
      if (Year) {
        err << " Year: " << Year;
      }
      err << " }, matched " << fNDataSets
          << " datasets, but required exactly 1 match because "
             "\"require_unique\" was set.";
      throw err;
    }

    auto const &dataset = veqe.GetDataset(datasetids.front());

    double Angle_Bite_Deg = inst_paramset.get<double>("Angle_Bite_Deg", 1);

    // If we have multiple matching datasets
    for (size_t id : datasetids) {
      auto const &ds = veqe.GetDataset(id);
      fDatasets.push_back(ds);

      fDataGraphs.push_back(std::move(BuildTGraph(ds.Datapoints)));
      fDataGraphs.back()->SetName(ds.Citation.c_str());

      fAngleCuts.push_back(SimpleParticlePhaseSpaceRestriction::Theta_deg(
          Angle_Deg - 0.5 * Angle_Bite_Deg - 1E-5,
          Angle_Deg + 0.5 * Angle_Bite_Deg + 1E-5));

      if (inst_paramset.has_key("prediction_hist")) {
        fPredictions.push_back(BuildHistFromFHiCL<TH1D>(
            inst_paramset.get<fhicl::ParameterSet>("prediction_hist")));
      } else { // Use a default binning
        fPredictions.push_back(std::make_unique<TH1D>(
            (std::string("MCPrediction_") + ds.Citation).c_str(),
            ";#nu (GeV); Count", 100, 0, EIn_GeV));
      }
      fPredictions.back()->SetDirectory(nullptr);

      // It wants a prediction
      if (!fPrediction) {
        fPrediction = Clone(fPredictions.back());
      }
    }

    // Set the properties for the relevant dataset
    fJournalReference = dataset.Citation;
    fYear = std::to_string(dataset.Year);
    fTargetMaterial = std::string("Z = ") +
                      std::to_string(GetZ(dataset.TargetPDG)) +
                      ", A = " + std::to_string(GetA(dataset.TargetPDG));
    fFluxDescription = std::to_string(dataset.EIn_GeV) + " GeV";
    fSignalDescription = "Theta e\' = " + std::to_string(dataset.Angle_Deg);

    fWrite_directory =
        "VirginiaQEArchive_E=" + std::to_string(dataset.EIn_GeV) + "_A" +
        std::to_string(dataset.Angle_Deg);

    SimpleDataComparison_1D::Initialize(inst_paramset);

    bool force_electron_probe =
        inst_paramset.get<bool>("force_electron_probe", true);
    bool force_nu_probe_CC = inst_paramset.get<bool>("force_nu_probe_CC", true);
    bool force_nu_probe_NC = inst_paramset.get<bool>("force_nu_probe_NC", true);

    fEin_ps = SimpleParticlePhaseSpaceRestriction::Energy(
        (EIn_GeV / kGeV) - 1E-5, (EIn_GeV / kGeV) + 1E-5);

    IsSigFunc = [=](FullEvent const &fev) -> bool {
      Particle ISProbe;

      if (force_electron_probe) {
        ISProbe = GetHMISChargedLepton(fev);
        if (!ISProbe) {
          return false;
        }
        if (ISProbe.pdg != pdgcodes::kElectron) {
          return false;
        }
      } else if (force_nu_probe_CC || force_nu_probe_NC) {
        ISProbe = GetHMISNeutralLepton(fev);
        if (!ISProbe) {
          return false;
        }
      } else {
        ISProbe = GetHMISLepton(fev);
        if (!ISProbe) {
          return false;
        }
      }

      Particle FSLepton;
      if (force_electron_probe) {
        FSLepton = GetHMFSChargedLepton(fev);
        if (!FSLepton) {
          return false;
        }
        if (FSLepton.pdg != pdgcodes::kElectron) {
          return false;
        }
      } else if (force_nu_probe_CC) {
        FSLepton = GetHMFSChargedLepton(fev);
        if (!FSLepton) {
          return false;
        }
        if (FSLepton.pdg != GetChargedLeptonPDG(ISProbe.pdg)) {
          return false;
        }
      } else if (force_nu_probe_NC) {
        FSLepton = GetHMFSNeutralLepton(fev);
        if (!FSLepton) {
          return false;
        }
        if (FSLepton.pdg != ISProbe.pdg) {
          return false;
        }
      } else {
        FSLepton = GetHMFSLepton(fev);
        if (!FSLepton) {
          return false;
        }
      }

      if (!fEin_ps.Inside(ISProbe)) {
        return false;
      }

      return true;
    };

    // Dummy function as we want to use the ProcessExtra to fill local histos
    // keyed by angle.
    CompProjFunc = [](FullEvent const &fev) -> std::array<double, 1> {
      return {0};
    };

    // Here is where the magic happens
    ProcessExtraFunc = [&](FullEvent const &fev, bool isSig, double weight) {
      if (!isSig) {
        return;
      }

      Particle ISProbe = GetHMISLepton(fev);
      Particle FSLepton = GetHMFSLepton(fev);

      for (size_t ds_it = 0; ds_it < fNDataSets; ++ds_it) {
        if (fAngleCuts[ds_it].Inside(FSLepton)) {
          TH_Help::Fill(fPredictions[ds_it],
                        {(ISProbe.E() - FSLepton.E()) * kGeV}, weight);
        }
      }
    };
  }

  void FinalizeComparison() {}

  void Write() {

    bool doprint = fPrint.size();
    std::string print_name = fPrint;
    bool print_open =
        fPrint.size() && fInstanceConfig.get<bool>("print_open", false);
    bool print_closed =
        fPrint.size() && fInstanceConfig.get<bool>("print_closed", false);

    TCanvas c1;
    if (print_open) {
      c1.Print((fPrint + "[").c_str());
    }

    for (size_t i = 0; i < fNDataSets; ++i) {
      auto &g = fDataGraphs[i];
      auto &h = fPredictions[i];

      nuis::persistency::WriteToOutputFile(g, g->GetName(), fWrite_directory);

      PeakScale(g, 1);

      nuis::persistency::WriteToOutputFile(
          g, std::string(g->GetName()) + "_peaknorm", fWrite_directory);

      nuis::persistency::WriteToOutputFile(h, h->GetName(), fWrite_directory);

      PeakScale(h, 1);

      nuis::persistency::WriteToOutputFile(
          h, std::string(h->GetName()) + "_peaknorm", fWrite_directory);

      if (doprint) {
        h->Draw("CHIST");
        g->Draw("PL");

        TLatex lat;
        lat.DrawLatexNDC(0.3, 0.95,
                         (fDatasets[i].Citation +
                          " E_{in} = " + std::to_string(fDatasets[i].EIn_GeV) +
                          ", Theta = " + std::to_string(fDatasets[i].Angle_Deg))
                             .c_str());

        c1.Print(print_name.c_str());
      }
    }
    if (print_closed) {
      c1.Print((fPrint + "]").c_str());
    }
  }
};

DECLARE_PLUGIN(IDataComparison, VirginiaQEArchive);
DECLARE_PLUGIN(IEventProcessor, VirginiaQEArchive);
