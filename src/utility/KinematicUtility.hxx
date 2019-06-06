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

#ifndef UTILITY_KINEMATICUTILITY_HXX_SEEN
#define UTILITY_KINEMATICUTILITY_HXX_SEEN

#include "utility/PDGCodeUtility.hxx"

#include "TLorentzVector.h"
#include "TVector3.h"

#include <limits>
#include <utility>

namespace nuis {
namespace event {
class FullEvent;
}
} // namespace nuis

namespace nuis {
namespace utility {

double GetNeutrinoEQERec(event::FullEvent const &fev,
                         double SeparationEnergy_MeV);
double GetNeutrinoQ2QERec(event::FullEvent const &fev,
                          double SeparationEnergy_MeV);
double GetNeutrinoWRec(event::FullEvent const &fev);
double GetEAvailProxy(event::FullEvent const &fev);

TVector3 GetVectorInTPlane(const TVector3 &inp, const TVector3 &planarNormal);
TVector3 GetUnitVectorInTPlane(const TVector3 &inp,
                               const TVector3 &planarNormal);
double GetDeltaPhiT(TVector3 const &V_lepton, TVector3 const &V_other,
                    TVector3 const &Normal, bool PiMinus = false);
TVector3 GetDeltaPT(TVector3 const &V_lepton, TVector3 const &V_other,
                    TVector3 const &Normal);
double GetDeltaAlphaT(TVector3 const &V_lepton, TVector3 const &V_other,
                      TVector3 const &Normal, bool PiMinus = false);

TVector3
GetDeltaPT_CC0PiN(event::FullEvent const &fev,
                  event::PDG_t islep_pdg = nuis::utility::pdgcodes::kNuMu,
                  event::PDG_t fslep_pdg = nuis::utility::pdgcodes::kDefault,
                  event::PDG_t fsnuc_pdg = nuis::utility::pdgcodes::kDefault);
double
GetDeltaPhiT_CC0PiN(event::FullEvent const &fev,
                    event::PDG_t islep_pdg = nuis::utility::pdgcodes::kNuMu,
                    event::PDG_t fslep_pdg = nuis::utility::pdgcodes::kDefault,
                    event::PDG_t fsnuc_pdg = nuis::utility::pdgcodes::kDefault);
double GetDeltaAlphaT_CC0PiN(
    event::FullEvent const &fev,
    event::PDG_t islep_pdg = nuis::utility::pdgcodes::kNuMu,
    event::PDG_t fslep_pdg = nuis::utility::pdgcodes::kDefault,
    event::PDG_t fsnuc_pdg = nuis::utility::pdgcodes::kDefault);

TLorentzVector GetEnergyMomentumTransfer(event::FullEvent const &fev);

struct KinematicRange : public std::pair<double, double> {
  using std::pair<double, double>::pair;
  using std::pair<double, double>::operator=;

  KinematicRange()
      : std::pair<double, double>(0, std::numeric_limits<double>::max()) {}

  // Not sure why something like this isn't being inherited...
  KinematicRange(std::pair<double, double> const &o)
      : std::pair<double, double>(std::min(o.first, o.second),
                                  std::max(o.first, o.second)) {}

  bool IsInRange(double val) const {
    return !((val > second) || (val < first));
  }
};

} // namespace utility
} // namespace nuis

#endif
