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

#pragma once

#include "plugins/traits.hxx"

#include "exception/exception.hxx"

#include "generator/GeneratorManager.hxx"

#include <iterator>
#include <limits>

namespace fhicl {
class ParameterSet;
}

namespace nuis {
namespace event {
class MinimalEvent;
class FullEvent;
} // namespace event
} // namespace nuis

class IInputHandler {
public:
  struct FullEvent_const_iterator
      : public std::iterator<
            std::input_iterator_tag, nuis::event::FullEvent const, size_t,
            nuis::event::FullEvent const *, nuis::event::FullEvent const &> {

    FullEvent_const_iterator(size_t _idx, IInputHandler const *_ih) {
      idx = _idx;
      ih = _ih;
    }
    FullEvent_const_iterator(FullEvent_const_iterator const &other) {
      idx = other.idx;
      ih = other.ih;
    }
    FullEvent_const_iterator operator=(FullEvent_const_iterator const &other) {
      idx = other.idx;
      ih = other.ih;
      return (*this);
    }

    bool operator==(FullEvent_const_iterator const &other) {
      return (idx == other.idx);
    }
    bool operator!=(FullEvent_const_iterator const &other) {
      return !(*this == other);
    }
    nuis::event::FullEvent const &operator*() { return ih->GetFullEvent(idx); }
    nuis::event::FullEvent const *operator->() {
      return &ih->GetFullEvent(idx);
    }

    FullEvent_const_iterator operator++() {
      idx++;
      return *this;
    }
    FullEvent_const_iterator operator++(int) {
      FullEvent_const_iterator tmp(*this);
      idx++;
      return tmp;
    }

  private:
    size_t idx;
    IInputHandler const *ih;
  };

  NEW_NUIS_EXCEPT(invalid_input_file);
  NEW_NUIS_EXCEPT(invalid_entry);
  NEW_NUIS_EXCEPT(input_handler_feature_unimplemented);

  typedef size_t ev_index_t;

  virtual void Initialize(fhicl::ParameterSet const &) = 0;
  virtual nuis::event::MinimalEvent const &
  GetMinimalEvent(ev_index_t idx) const = 0;
  virtual nuis::event::FullEvent const &GetFullEvent(ev_index_t idx) const = 0;
  virtual void RecalculateEventWeights(){};
  virtual double GetEventWeight(ev_index_t) const { return 1; };

  /// Allows samples that implement flux cuts to request an appropriate
  virtual double GetXSecScaleFactor(
      std::pair<double, double> const &EnuRange = std::pair<double, double>{
          std::numeric_limits<double>::max(),
          std::numeric_limits<double>::max()}) const = 0;

  virtual size_t GetNEvents() const = 0;

  FullEvent_const_iterator begin() const {
    return FullEvent_const_iterator(0, this);
  }
  FullEvent_const_iterator end() const {
    return FullEvent_const_iterator(GetNEvents(), this);
  }

  virtual ~IInputHandler() {}

  virtual nuis::GeneratorManager::Generator_id_t GetGeneratorId() const = 0;
};

DECLARE_PLUGIN_INTERFACE(IInputHandler);
