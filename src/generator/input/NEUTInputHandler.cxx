#include "generator/input/NEUTInputHandler.hxx"

#include "utility/ROOTUtility.hxx"
#include "utility/PDGCodeUtility.hxx"
#include "utility/InteractionChannelUtility.hxx"

#include "generator/utility/NEUTUtility.hxx"

#include "fhiclcpp/ParameterSet.h"

using namespace nuis::event;
using namespace nuis::utility;

NEUTInputHandler::NEUTInputHandler() : fInputTree(nullptr) {}
NEUTInputHandler::NEUTInputHandler(NEUTInputHandler &&other)
    : fInputTree(std::move(other.fInputTree)),
      fReaderEvent(std::move(other.fReaderEvent)) {}

void NEUTInputHandler::Initialize(fhicl::ParameterSet const &ps) {

  fInputTree = CheckGetTTree(ps.get<std::string>("file"), "neuttree");

  fReaderEvent.fNeutVect = nullptr;
  fInputTree->tree->SetBranchAddress("vectorbranch", &fReaderEvent.fNeutVect);
}

MinimalEvent const &NEUTInputHandler::GetMinimalEvent(ev_index_t idx) const {
  if (idx >= GetNEvents()) {
    throw IInputHandler::invalid_entry()
        << "[ERROR]: Attempted to get entry " << idx
        << " from an InputHandler with only " << GetNEvents();
  }
  fInputTree->tree->GetEntry(idx);

  fReaderEvent.mode = IntToChannel(fReaderEvent.fNeutVect->Mode);

  size_t NPart = fReaderEvent.fNeutVect->Npart();
  for (size_t part_it = 0; part_it < NPart; part_it++) {
    NeutPart const &part = (*fReaderEvent.fNeutVect->PartInfo(part_it));
    if ((part.fIsAlive == false) && (part.fStatus == -1) &&
        IsNeutralLepton(part.fPID)) {
      fReaderEvent.probe_E = part.fP.T();
      fReaderEvent.probe_pdg = part.fPID;
      break;
    }
  }

  return fReaderEvent;
}

FullEvent const &NEUTInputHandler::GetFullEvent(ev_index_t idx) const {
  (void)GetMinimalEvent(idx);

  fReaderEvent.ClearParticleStack();

  if (fReaderEvent.fNeutVect->Ibound) {
    fReaderEvent.target_pdg = MakeNuclearPDG(fReaderEvent.fNeutVect->TargetA,
                                             fReaderEvent.fNeutVect->TargetZ);
  } else {
    fReaderEvent.target_pdg = MakeNuclearPDG(1, 1);
  }

  size_t NPart = fReaderEvent.fNeutVect->Npart();
  bool FoundIntermediateStateParticle = false;
  for (size_t part_it = 0; part_it < NPart; part_it++) {
    NeutPart const &part = (*fReaderEvent.fNeutVect->PartInfo(part_it));

    Particle nuis_part;
    nuis_part.pdg = part.fPID;
    nuis_part.P4 = part.fP;

    Particle::Status_t state = GetNeutParticleStatus(part, fReaderEvent.mode);
    size_t state_int = static_cast<size_t>(state);

    if ((!FoundIntermediateStateParticle) &&
        (state == Particle::Status_t::kIntermediate)) {
      FoundIntermediateStateParticle = true;
    }

    // Add status == 0 particles as pre-FSI particles until we find an
    // intermediate state particle
    if (!IsCoh(fReaderEvent.mode) && (part_it > 1) && (state_int == 0) &&
        (!FoundIntermediateStateParticle)) {
      fReaderEvent
          .ParticleStack[static_cast<size_t>(
              Particle::Status_t::kPrimaryFinalState)]
          .particles.push_back(nuis_part);
    }

    // Intermediate particles should be pushed onto the primary final state
    // stack for NEUT
    if (state == Particle::Status_t::kIntermediate) {
      state_int = static_cast<size_t>(Particle::Status_t::kPrimaryFinalState);
    }

    fReaderEvent.ParticleStack[state_int].particles.push_back(nuis_part);
  }

  return fReaderEvent;
}

size_t NEUTInputHandler::GetNEvents() const {
  return fInputTree->tree->GetEntries();
}

DECLARE_PLUGIN(IInputHandler, NEUTInputHandler);
