#include "generator/input/NEUTInputHandler.hxx"

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

  fFlux = GetHistogramFromROOTFile<TH1>(fInputTreeFile.file, "flux_numu");
  fEvtrt = GetHistogramFromROOTFile<TH1>(fInputTreeFile.file, "evtrt_numu");

  fFileWeight = fEvtrt->Integral() / (fFlux->Integral() * double(GetNEvents()));
}

double
NEUTInputHandler::GetXSecScaleFactor(std::pair<double, double> const &) const {

  throw input_handler_feature_unimplemented()
      << "[ERROR]: Flux cuts not yet implemented for NEUT input handler.";
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

size_t NEUTInputHandler::GetNEvents() const {
  return fInputTreeFile.tree->GetEntries();
}

GeneratorManager::Generator_id_t NEUTInputHandler::GetGeneratorId(){
  return GeneratorManager::Get().EnsureGeneratorRegistered("NEUT");
}

DECLARE_PLUGIN(IInputHandler, NEUTInputHandler);
