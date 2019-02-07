#include "variation/IWeightProvider.hxx"

#include "event/FullEvent.hxx"

nuis::event::FullEvent
IWeightProvider::VaryFullEvent(nuis::event::FullEvent const &fe) {
  nuis::event::FullEvent fe_clone = fe.Clone();
  fe_clone.RWWeight = GetEventWeight(fe_clone);
  return fe_clone;
}
