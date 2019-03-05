#include "generator/input/NEUTInputHandler.hxx"

#include "persistency/ROOTOutput.hxx"

#include "utility/HistogramUtility.hxx"
#include "utility/InteractionChannelUtility.hxx"
#include "utility/PDGCodeUtility.hxx"

#include "generator/utility/NEUTUtility.hxx"

#include "neutpart.h"
#include "neutvect.h"

#include "fhiclcpp/ParameterSet.h"

using namespace nuis;
using namespace nuis::event;
using namespace nuis::utility;
using namespace nuis::neuttools;

NEUTInputHandler::NEUTInputHandler() : fInputTreeFile() {}
NEUTInputHandler::NEUTInputHandler(NEUTInputHandler &&other)
    : fInputTreeFile(std::move(other.fInputTreeFile)),
      fReaderEvent(std::move(other.fReaderEvent)) {}

void NEUTInputHandler::Initialize(fhicl::ParameterSet const &ps) {

  fInputTreeFile = CheckGetTTree(ps.get<std::string>("file"), "neuttree");

  fNeutVect = nullptr;
  fInputTreeFile.tree->SetBranchAddress("vectorbranch", &fNeutVect);

  fKeepIntermediates = ps.get<bool>("keep_intermediates", false);

  std::string flux_name = ps.get<std::string>("flux_hist_name", "flux_numu");
  std::string evtrt_name = ps.get<std::string>("evtrt_hist_name", "evtrt_numu");
  fXSecRescaleFactor = ps.get<double>("xsec_rescale_factor",1);

  std::string override_flux_file =
      ps.get<std::string>("override_flux_file", "");

  if (override_flux_file.size()) {
    fFlux = GetHistogramFromROOTFile<TH1>(override_flux_file, flux_name);
    RebuildEventRate(!ps.get<bool>("flux_hist_in_MeV", false));
  } else {
    fFlux =
        GetHistogramFromROOTFile<TH1>(fInputTreeFile.file, flux_name, false);
  }

  if (fFlux) {
    if (!fEvtrt) {
      fEvtrt = GetHistogramFromROOTFile<TH1>(fInputTreeFile.file, evtrt_name);
    }

    fFileWeight = fEvtrt->Integral() * 1.0E-38 /
                  (fFlux->Integral() * double(GetNEvents()));
    std::cout << "[INFO]: Average NEUT XSecWeight = " << fFileWeight << ""
              << std::endl;
  } else {
    std::cout
        << "[INFO]: Input NEUT file doesn't have flux information, assuming "
           "mono energetic and calculating average cross-section..."
        << std::endl;
    fFileWeight = GetMonoEXSecWeight() / double(GetNEvents());
    std::cout << "[INFO]: Done (Average NEUT XSecWeight = " << fFileWeight
              << ")!" << std::endl;
  }

  fFileWeight *= fXSecRescaleFactor;
}

NEW_NUIS_EXCEPT(non_mono_energetic_input_file);
double NEUTInputHandler::GetMonoEXSecWeight() {
  double xsec = 0;
  double count = 0;
  double E_first = std::numeric_limits<double>::max();
  size_t NEvents = GetNEvents();
  size_t ShoutEvery = NEvents / 100;
  std::cout << "[INFO]: Read " << 0 << "/" << NEvents << " NEUT events."
            << std::flush;

  for (size_t nev_it = 0; nev_it < NEvents; ++nev_it) {
    if (ShoutEvery && !(nev_it % ShoutEvery)) {
      std::cout << "\r[INFO]: Read " << nev_it << "/" << NEvents
                << " NEUT events." << std::flush;
    }

    fInputTreeFile.tree->GetEntry(nev_it);
    xsec += fNeutVect->Totcrs;
    count++;
    if (E_first == std::numeric_limits<double>::max()) {
      NeutPart *part = fNeutVect->PartInfo(0);
      E_first = part->fP.E();
    } else {
      NeutPart *part = fNeutVect->PartInfo(0);
      if (E_first != part->fP.E()) {
        throw non_mono_energetic_input_file()
            << "[ERROR]: When calculating NEUT xsec weight for a mono "
               "energetic beam, found different energies: "
            << E_first << " and " << part->fP.E()
            << ". Cannot continue with this input file.";
      }
    }
  }
  std::cout << "\r[INFO]: Read " << NEvents << "/" << NEvents << " NEUT events."
            << std::endl;

  return (xsec / count) * 1E-38;
}

void NEUTInputHandler::RebuildEventRate(bool FluxInGeV) {
  auto XSec = Clone(fFlux, true, "xsec");
  auto Entry = Clone(fFlux, true, "entry");

  std::cout << "[INFO]: Rebuilding total cross-section prediction..."
            << std::endl;
  size_t NEvents = GetNEvents();
  size_t ShoutEvery = NEvents / 100;
  std::cout << "[INFO]: Read " << 0 << "/" << NEvents << " NEUT events."
            << std::flush;

  for (size_t nev_it = 0; nev_it < NEvents; ++nev_it) {
    if (ShoutEvery && !(nev_it % ShoutEvery)) {
      std::cout << "\r[INFO]: Read " << nev_it << "/" << NEvents
                << " NEUT events." << std::flush;
    }

    fInputTreeFile.tree->GetEntry(nev_it);

    NeutPart *part = fNeutVect->PartInfo(0);
    double E = part->fP.E() * (FluxInGeV ? 1E-3 : 1);

    XSec->Fill(E, fNeutVect->Totcrs);
    Entry->Fill(E);
  }
  std::cout << std::endl << "[INFO]: Done!" << std::endl;

  // Binned total xsec
  XSec->Divide(Entry.get());

  fEvtrt = Clone(XSec, false, "evtrt");
  // Event rate prediction
  fEvtrt->Multiply(fFlux.get());
}

double
NEUTInputHandler::GetXSecScaleFactor(std::pair<double, double> const &) const {

  throw input_handler_feature_unimplemented()
      << "[ERROR]: Flux cuts not yet implemented for NEUT input handler.";
}

NeutVect *NEUTInputHandler::GetNeutEvent(ev_index_t idx) const {
  if (idx >= GetNEvents()) {
    throw IInputHandler::invalid_entry()
        << "[ERROR]: Attempted to get entry " << idx
        << " from an InputHandler with only " << GetNEvents();
  }
  fInputTreeFile.tree->GetEntry(idx);

  return fNeutVect;
}

MinimalEvent const &NEUTInputHandler::GetMinimalEvent(ev_index_t idx) const {
  if (idx >= GetNEvents()) {
    throw IInputHandler::invalid_entry()
        << "[ERROR]: Attempted to get entry " << idx
        << " from an InputHandler with only " << GetNEvents();
  }
  fInputTreeFile.tree->GetEntry(idx);

  fReaderEvent.mode = IntToChannel(fNeutVect->Mode);

  size_t NPart = fNeutVect->Npart();
  for (size_t part_it = 0; part_it < NPart; part_it++) {
    NeutPart const &part = (*fNeutVect->PartInfo(part_it));
    if ((part.fIsAlive == false) && (part.fStatus == -1) &&
        IsNeutralLepton(part.fPID)) {
      fReaderEvent.probe_E = part.fP.T();
      fReaderEvent.probe_pdg = part.fPID;
      break;
    }
  }

  fReaderEvent.XSecWeight = fFileWeight;

  if (fWeightCache.size() <= idx) {
    fWeightCache.push_back(fReaderEvent.XSecWeight);
  }

  fReaderEvent.fGenEvent = fNeutVect;

  return fReaderEvent;
}

FullEvent const &NEUTInputHandler::GetFullEvent(ev_index_t idx) const {
  (void)GetMinimalEvent(idx);

  fReaderEvent.ClearParticleStack();

  if (fNeutVect->Ibound) {
    fReaderEvent.target_pdg =
        MakeNuclearPDG(fNeutVect->TargetA, fNeutVect->TargetZ);
  } else {
    fReaderEvent.target_pdg = MakeNuclearPDG(1, 1);
  }

  size_t NPart = fNeutVect->Npart();
  size_t NPrimary = fNeutVect->Nprimary();
  for (size_t part_it = 0; part_it < NPart; part_it++) {
    NeutPart const &part = (*fNeutVect->PartInfo(part_it));

    Particle nuis_part;
    nuis_part.pdg = part.fPID;
    nuis_part.P4 = part.fP;

    Particle::Status_t state = GetNeutParticleStatus(part, fReaderEvent.mode);

    if (!fKeepIntermediates && (state == Particle::Status_t::kIntermediate)) {
      continue;
    }

    size_t state_int = static_cast<size_t>(state);

    // Add status == 0 particles as pre-FSI particles until we find an
    // intermediate state particle
    if ((part_it < NPrimary) &&
        (state != Particle::Status_t::kPrimaryInitialState)) {
      fReaderEvent
          .ParticleStack[static_cast<size_t>(
              Particle::Status_t::kPrimaryFinalState)]
          .particles.push_back(nuis_part);
    }

    fReaderEvent.ParticleStack[state_int].particles.push_back(nuis_part);
  }

  return fReaderEvent;
}

double NEUTInputHandler::GetEventWeight(ev_index_t idx) const {
  if (idx > fWeightCache.size()) {
    throw weight_cache_miss()
        << "[ERROR]: Failed to get cached weight for event index: " << idx;
  }
  return fWeightCache[idx];
}

size_t NEUTInputHandler::GetNEvents() const {
  return fInputTreeFile.tree->GetEntries();
}

GeneratorManager::Generator_id_t NEUTInputHandler::GetGeneratorId() const {
  return GeneratorManager::Get().EnsureGeneratorRegistered("NEUT");
}

DECLARE_PLUGIN(IInputHandler, NEUTInputHandler);
