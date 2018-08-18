#include "NEUTInputHandler.hxx"

#include "core/FullEvent.hxx"

#include "utility/ROOTUtility.hxx"

#include "generator/NEUTUtility.hxx"

#include "fhiclcpp/ParameterSet.h"

using namespace nuis::core;
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
  for (size_t part_it = 0; part_it < NPart; part_it++) {
    NeutPart const &part = (*fReaderEvent.fNeutVect->PartInfo(part_it));

    nuis::core::Particle nuis_part;
    nuis_part.pdg = part.fPID;
    nuis_part.status = GetNeutParticleStatus(part, fReaderEvent.mode);
    nuis_part.P4 = part.fP;

    size_t state_int = static_cast<size_t>(nuis_part.status);

    fReaderEvent.ParticleStack[state_int].particles.push_back(
        std::move(nuis_part));
  }

  return fReaderEvent;
}

size_t NEUTInputHandler::GetNEvents() const {
  return fInputTree->tree->GetEntries();
}

DECLARE_PLUGIN(IInputHandler, NEUTInputHandler);
