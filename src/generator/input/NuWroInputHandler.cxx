#include "generator/input/NuWroInputHandler.hxx"

#include "generator/utility/NuWroUtility.hxx"

#include "fhiclcpp/ParameterSet.h"

#include "particle.h"
using NuWroParticle = ::particle;

using namespace nuis;
using namespace nuis::event;
using namespace nuis::utility;
using namespace nuis::nuwrotools;

NuWroInputHandler::NuWroInputHandler() : fInputTree(), fTreeEvent(nullptr) {}
NuWroInputHandler::NuWroInputHandler(NuWroInputHandler &&other)
    : fInputTree(std::move(other.fInputTree)),
      fReaderEvent(std::move(other.fReaderEvent)),
      fTreeEvent(other.fTreeEvent) {}

void NuWroInputHandler::Initialize(fhicl::ParameterSet const &ps) {

  fInputTree = CheckGetTTree(ps.get<std::string>("file"), "treeout");

  fTreeEvent = nullptr;
  fInputTree.tree->SetBranchAddress("e", &fTreeEvent);

  fKeepIntermediates = ps.get<bool>("keep_intermediates", false);
}
MinimalEvent const &NuWroInputHandler::GetMinimalEvent(ev_index_t idx) const {
  if (idx >= GetNEvents()) {
    throw IInputHandler::invalid_entry()
        << "[ERROR]: Attempted to get entry " << idx
        << " from an InputHandler with only " << GetNEvents();
  }

  fInputTree.tree->GetEntry(idx);

  fReaderEvent.mode = NuWroEventChannel(*fTreeEvent);
  fReaderEvent.probe_E = fTreeEvent->in[0].E();
  fReaderEvent.probe_pdg = fTreeEvent->in[0].pdg;
  fReaderEvent.XSecWeight = fTreeEvent->weight / double(GetNEvents());

  if (fWeightCache.size() <= idx) {
    fWeightCache.push_back(fReaderEvent.XSecWeight);
  }

  return fReaderEvent;
}

FullEvent const &NuWroInputHandler::GetFullEvent(ev_index_t idx) const {
  (void)GetMinimalEvent(idx);

  fReaderEvent.ClearParticleStack();

  for (size_t p_it = 0; p_it < fTreeEvent->in.size(); ++p_it) {
    NuWroParticle &part = fTreeEvent->in[p_it];

    Particle nuis_part;

    nuis_part.pdg = part.pdg;
    nuis_part.P4 = TLorentzVector(part[1], part[2], part[3], part[0]);

    fReaderEvent
        .ParticleStack[static_cast<size_t>(
            Particle::Status_t::kPrimaryInitialState)]
        .particles.push_back(nuis_part);
  }

  for (size_t p_it = 0; p_it < fKeepIntermediates && fTreeEvent->out.size();
       ++p_it) {
    NuWroParticle &part = fTreeEvent->out[p_it];

    Particle nuis_part;

    nuis_part.pdg = part.pdg;
    nuis_part.P4 = TLorentzVector(part[1], part[2], part[3], part[0]);

    fReaderEvent
        .ParticleStack[static_cast<size_t>(
            Particle::Status_t::kPrimaryFinalState)]
        .particles.push_back(nuis_part);
  }
  for (size_t p_it = 0; (p_it < fTreeEvent->post.size()); ++p_it) {
    NuWroParticle &part = fTreeEvent->post[p_it];

    Particle nuis_part;

    nuis_part.pdg = part.pdg;
    nuis_part.P4 = TLorentzVector(part[1], part[2], part[3], part[0]);

    fReaderEvent
        .ParticleStack[static_cast<size_t>(Particle::Status_t::kNuclearLeaving)]
        .particles.push_back(nuis_part);
  }

  return fReaderEvent;
}

NuWroEvent const &NuWroInputHandler::GetNuWroEvent(ev_index_t idx) const {
  (void)GetMinimalEvent(idx);
  return *fTreeEvent;
}

double NuWroInputHandler::GetEventWeight(ev_index_t idx) const {
  if (idx > fWeightCache.size()) {
    throw weight_cache_miss()
        << "[ERROR]: Failed to get cached weight for event index: " << idx;
  }
  return fWeightCache[idx];
}

double NuWroInputHandler::GetXSecScaleFactor(
    std::pair<double, double> const &EnuRange) const {
  throw input_handler_feature_unimplemented()
      << "[ERROR]: Flux cuts not yet implemented for NuWro input handler.";
}

size_t NuWroInputHandler::GetNEvents() const {
  return fInputTree.tree->GetEntries();
}

GeneratorManager::Generator_id_t NuWroInputHandler::GetGeneratorId(){
  return GeneratorManager::Get().EnsureGeneratorRegistered("NuWro");
}

DECLARE_PLUGIN(IInputHandler, NuWroInputHandler);
