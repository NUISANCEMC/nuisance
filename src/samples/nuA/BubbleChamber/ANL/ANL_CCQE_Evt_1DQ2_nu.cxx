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

#include "samples/IDataComparison.hxx"

#include "event/FullEvent.hxx"

#include "input/InputManager.hxx"

#include "persistency/ROOTOutput.hxx"

#include "utility/FileSystemUtility.hxx"
#include "utility/FullEventUtility.hxx"
#include "utility/KinematicUtility.hxx"
#include "utility/PDGCodeUtility.hxx"
#include "utility/ROOTUtility.hxx"

#include "TH1D.h"

using namespace nuis::event;
using namespace nuis::input;
using namespace nuis::utility;
using namespace nuis::persistency;

class ANL_CCQE_Evt_1DQ2_nu : public IDataComparison {

public:
  NEW_NUIS_EXCEPT(invalid_publication_specifier);

  enum Publication { kPRL31, kPRD16, kPRD26 };
  Publication Pub;
  std::string Pub_str;
  bool UseD2Corr;

  InputManager::Input_id_t fIH_id;
  std::string write_directory;

  std::unique_ptr<TH1D> fData;
  std::unique_ptr<TH1D> fPrediction;

  std::pair<double, double> EnuRange;

  ANL_CCQE_Evt_1DQ2_nu()
      : Pub(kPRD26), Pub_str(""), UseD2Corr(false),
        fIH_id(std::numeric_limits<InputManager::Input_id_t>::max()),
        write_directory("ANL_CCQE_Evt_1DQ2_nu"), fData(nullptr),
        fPrediction(nullptr) {}

  std::string GetJournalReference() {
    return "PRL 31 844 / PRD 16 3103 / PRD 26 537";
  }
  std::string GetTargetMaterial() { return "D2"; }
  std::string GetFluxDescription() { return "ANL Muon Neutrino"; }
  std::string GetSignalDescription() { return "True CCQE"; }
  std::string GetDocumentation() {
    return "Can specify \"publication: <PUB>\", where <PUB> is one of [ PRL31, "
           "PRD16, PRD26 ] to clarify a publication for comparison. Defaults "
           "to PRD26.\n"
           "Can enable deuterium Q2 correction by specifying "
           "\"use_D2_correction: true\"";
  }
  fhicl::ParameterSet GetExampleConfiguration() {
    fhicl::ParameterSet exps;

    exps.put<std::string>("name", "ANL_CCQE_Evt_1DQ2_nu");
    exps.put<std::string>("input_type", "Generator");
    exps.put<std::string>("file", "ANL_Events.root");
    exps.put<std::string>("write_directory", "ANL_CCQE_Evt_1DQ2_nu_Generator");
    exps.put<std::string>("publication", "PRD26");
    exps.put<bool>("use_D2_correction", true);

    fhicl::ParameterSet fd;
    fd.put<std::string>("file", "ANL_fake_data.root");
    fd.put<std::string>("histogram_name", "fake_data");

    exps.put("fake_data", fd);

    return exps;
  }

  void Initialize(fhicl::ParameterSet const &ps) {

    if (!ps.has_key("publication")) {
      std::string publication = ps.get<std::string>("publication");
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
    }

    switch (Pub) {
    case kPRL31: {
      Pub_str = "PRL31_844";
      EnuRange = std::pair<double, double>{0.0, 3.0};
      break;
    }
    case kPRD16: {
      Pub_str = "PRD16_3103";
      EnuRange = std::pair<double, double>{0.0, 6.0};
      break;
    }
    case kPRD26: {
      Pub_str = "PRD26_537";
      EnuRange = std::pair<double, double>{0.0, 6.0};
      break;
    }
    }

    if (ps.has_key("use_D2_correction")) {
      UseD2Corr = ps.get<bool>("use_D2_correction");
    }

    if (ps.has_key("use_D2_correction")) {
      UseD2Corr = ps.get<bool>("use_D2_correction");
    }

    if (ps.has_key("write_directory")) {
      write_directory = ps.get<std::string>("write_directory");
    }

    fIH_id = InputManager::Get().EnsureInputLoaded(ps);

    if (ps.has_key("fake_data")) {
      fhicl::ParameterSet const &fd = ps.get<fhicl::ParameterSet>("fake_data");
      fData = GetHistogramFromROOTFile<TH1D>(
          fd.get<std::string>("file"), fd.get<std::string>("histogram_name"));
    } else {
      fData = GetHistogramFromROOTFile<TH1D>(
          GetDataDir() + "nuA/BubbleChamber/ANL/ANL_CCQE_Data_" + Pub_str +
              ".root",
          "ANL_1DQ2_Data");
    }
    fPrediction = CloneHistogram(fData, true);
  }

  std::vector<bool> fIsSignal;
  std::vector<double> fQ2;

  void ProcessEvent(FullEvent const &fev) {
    fQ2.push_back(GetNeutrinoQ2QERec(fev));
  }

  bool IsSignal(FullEvent const &fev) {

    if (fev.mode != Channel_t::kCCQE) {
      return false;
    }

    Particle ISNumu = GetISNeutralLepton(fev);

    if (!ISNumu) {
      return false;
    }

    if (ISNumu.pdg != pdgcodes::kNuMu) {
      return false;
    }

    if ((ISNumu.P4.E() < EnuRange.first) || (ISNumu.P4.E() > EnuRange.second)) {
      return false;
    }

    double Q2 = GetNeutrinoQ2QERec(fev);
    if (Q2 <= 0) {
      return false;
    }

    return true;
  }

  void ProcessSample(size_t nmax) {
    if (fIH_id == std::numeric_limits<InputManager::Input_id_t>::max()) {
      throw uninitialized_ISample();
    }
    IInputHandler const &IH = InputManager::Get().GetInputHandler(fIH_id);

    size_t NEvsToProcess = std::min(nmax, IH.GetNEvents());
    IInputHandler::ev_index_t ev_idx = 0;
    size_t NSigEvents = 0;

    bool DetermineSignalEvents = !fIsSignal.size();

    while (ev_idx < NEvsToProcess) {
      if (DetermineSignalEvents) {
        FullEvent const &fev = IH.GetFullEvent(ev_idx);
        bool is_sig = IsSignal(fev);
        fIsSignal.push_back(is_sig);
        if (is_sig) {
          ProcessEvent(fev);
        }
      }

      if (fIsSignal[ev_idx]) {
        fPrediction->Fill(fQ2[NSigEvents++], IH.GetEventWeight(ev_idx));
      }

      ev_idx++;
    }
  }
  void Write() {

    WriteToOutputFile<TH1D>(fData.get(), "Data", write_directory);
    WriteToOutputFile<TH1D>(fPrediction.get(), "Prediction", write_directory);
  }
  std::string Name() { return "ANL_CCQE_Evt_1DQ2_nu"; }

  double GetGOF() { return 0; /*CalcChi2(fData, fPrediction);*/ }
};

DECLARE_PLUGIN(IDataComparison, ANL_CCQE_Evt_1DQ2_nu);
