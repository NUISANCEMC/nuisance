#include "generator/input/GENIEInputHandler.hxx"

#include "generator/utility/GENIEUtility.hxx"

#include "utility/PDGCodeUtility.hxx"

#ifdef GENIE_V3_INTERFACE
#include "Framework/EventGen/EventRecord.h"
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/GHEP/GHepRecord.h"
#else
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepParticle.h"
#include "GHEP/GHepRecord.h"
#endif

#include "fhiclcpp/ParameterSet.h"

using namespace nuis;
using namespace nuis::event;
using namespace nuis::utility;
using namespace nuis::genietools;

GENIEInputHandler::GENIEInputHandler() : fInputTree(), fGenieNtpl(nullptr) {}
GENIEInputHandler::GENIEInputHandler(GENIEInputHandler &&other)
    : fInputTree(std::move(other.fInputTree)),
      fReaderEvent(std::move(other.fReaderEvent)), fGenieNtpl(nullptr) {}

void GENIEInputHandler::Initialize(fhicl::ParameterSet const &ps) {

  fInputTree = CheckGetTTree(ps.get<std::string>("file"), "gtree");

  fInputTree.tree->SetBranchAddress("gmcrec", &fGenieNtpl);

  fKeepIntermediates = ps.get<bool>("keep_intermediates", false);
  fKeepNuclearParticles = ps.get<bool>("keep_nuclear_particles", false);
}

MinimalEvent const &GENIEInputHandler::GetMinimalEvent(ev_index_t idx) const {
  if (idx >= GetNEvents()) {
    throw IInputHandler::invalid_entry()
        << "[ERROR]: Attempted to get entry " << idx
        << " from an InputHandler with only " << GetNEvents();
  }

  fInputTree.tree->GetEntry(idx);

  genie::GHepRecord *GHep = static_cast<genie::GHepRecord *>(fGenieNtpl->event);
  if (!GHep) {
    throw invalid_GENIE_event()
        << "[ERROR]: GENIE event " << idx << " failed to contain a GHepRecord";
  }

  fReaderEvent.mode = GetEventChannel(*GHep);

  TObjArrayIter iter(GHep);
  genie::GHepParticle *p;
  while ((p = (dynamic_cast<genie::GHepParticle *>((iter).Next())))) {
    if (!p) {
      continue;
    }

    Particle::Status_t state = GetParticleStatus(*p, fReaderEvent.mode);
    if (state != Particle::Status_t::kPrimaryInitialState) {
      continue;
    }
    if (!IsNeutralLepton(p->Pdg(), pdgcodes::kMatterAntimatter) &&
        !IsChargedLepton(p->Pdg(), pdgcodes::kMatterAntimatter)) {
      continue;
    }
    fReaderEvent.probe_E = p->E() * 1.E3;
    fReaderEvent.probe_pdg = p->Pdg();
    break;
  }

  fReaderEvent.XSecWeight = 1;

  if (fWeightCache.size() <= idx) {
    fWeightCache.push_back(fReaderEvent.XSecWeight);
  }

  return fReaderEvent;
}

FullEvent const &GENIEInputHandler::GetFullEvent(ev_index_t idx) const {
  (void)GetMinimalEvent(idx);

  fReaderEvent.ClearParticleStack();

  genie::GHepRecord *GHep = static_cast<genie::GHepRecord *>(fGenieNtpl->event);

  unsigned int npart = GHep->GetEntries();

  // Fill Particle Stack
  genie::GHepParticle *p = 0;
  TObjArrayIter iter(GHep);

  // Loop over all particles
  while ((p = (dynamic_cast<genie::GHepParticle *>((iter).Next())))) {
    if (!p) {
      continue;
    }

    // Get Status
    Particle::Status_t state = GetParticleStatus(*p, fReaderEvent.mode);

    if (!fKeepIntermediates) {
      continue;
    }

    if (!fKeepNuclearParticles && IsNuclearPDG(p->Pdg())) {
      continue;
    }

    Particle nuis_part;

    nuis_part.pdg = p->Pdg();
    nuis_part.P4 = TLorentzVector(p->Px(), p->Py(), p->Pz(), p->E()) * 1E3;

    fReaderEvent.ParticleStack[static_cast<size_t>(state)].particles.push_back(
        nuis_part);
  }

  return fReaderEvent;
}

double GENIEInputHandler::GetEventWeight(ev_index_t idx) const {
  if (idx > fWeightCache.size()) {
    throw weight_cache_miss()
        << "[ERROR]: Failed to get cached weight for event index: " << idx;
  }
  return fWeightCache[idx];
}

double GENIEInputHandler::GetXSecScaleFactor(
    std::pair<double, double> const &EnuRange) const {
  throw input_handler_feature_unimplemented()
      << "[ERROR]: Flux cuts not yet implemented for GENIE input handler.";
}

size_t GENIEInputHandler::GetNEvents() const {
  return fInputTree.tree->GetEntries();
}

GeneratorManager::Generator_id_t GENIEInputHandler::GetGeneratorId() {
  return GeneratorManager::Get().EnsureGeneratorRegistered("GENIE");
}

DECLARE_PLUGIN(IInputHandler, GENIEInputHandler);
