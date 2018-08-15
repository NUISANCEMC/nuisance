#include "NuWroInputHandler.hxx"

#include "core/FullEvent.hxx"

#include "utility/ROOTUtility.hxx"

#include "fhiclcpp/ParameterSet.h"

using namespace nuis::core;
using namespace nuis::utility;

NuWroInputHandler::NuWroInputHandler() : fInputTree(nullptr) {}
NuWroInputHandler::NuWroInputHandler(NuWroInputHandler &&other)
    : fInputTree(std::move(other.fInputTree)),
      fReaderEvent(std::move(other.fReaderEvent)) {}

void NuWroInputHandler::Initialize(fhicl::ParameterSet const &ps) {

  fInputTree = CheckGetTTree(ps.get<std::string>("file"), "treeout");

  fReaderEvent.fNuWroEvent = nullptr;
  fInputTree->tree->SetBranchAddress("e", &fReaderEvent.fNuWroEvent);
}
MinimalEvent const &NuWroInputHandler::GetMinimalEvent(ev_index_t idx) {
  if (idx >= GetNEvents()) {
    throw IInputHandler::invalid_entry()
        << "[ERROR]: Attempted to get entry " << idx
        << " from an InputHandler with only " << GetNEvents();
  }
  fInputTree->tree->GetEntry(idx);
  return fReaderEvent;
}

FullEvent const &NuWroInputHandler::GetFullEvent(ev_index_t idx) {
  (void)GetMinimalEvent(idx);
  //Fill particle stack
  return fReaderEvent;
}

size_t NuWroInputHandler::GetNEvents() { return fInputTree->tree->GetEntries(); }

DECLARE_PLUGIN(IInputHandler,NuWroInputHandler);
