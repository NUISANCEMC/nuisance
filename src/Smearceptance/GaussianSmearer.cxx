// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "GaussianSmearer.h"

// #define DEBUG_THRESACCEPT

namespace {
GaussianSmearer::GSmearType GetVarType(std::string const &type) {
  if (type == "Absolute") {
    return GaussianSmearer::kAbsolute;
  } else if (type == "Fractional") {
    return GaussianSmearer::kFractional;
  } else if (type == "Function") {
    return GaussianSmearer::kFunction;
  }
  return GaussianSmearer::kNoType;
}

GaussianSmearer::DependVar GetKineType(std::string const &axisvar) {
  if (axisvar == "Momentum") {
    return GaussianSmearer::kMomentum;
  } else if (axisvar == "KE") {
    return GaussianSmearer::kKE;
  } else if (axisvar == "TEVis") {
    return GaussianSmearer::kTEVis;
  } else if (axisvar == "KEVis") {
    return GaussianSmearer::kKEVis;
  } else if (axisvar == "CosTheta") {
    return GaussianSmearer::kCosTheta;
  } else if (axisvar == "Theta") {
    return GaussianSmearer::kTheta;
  }
  THROW("Failed to parse smear type from \"" << axisvar << "\"");
}

std::string GetKineTypeName(GaussianSmearer::DependVar dv) {
  switch (dv) {
    case GaussianSmearer::kMomentum: {
      return "Momentum";
    }
    case GaussianSmearer::kKE: {
      return "KE";
    }
    case GaussianSmearer::kTEVis: {
      return "TEVis";
    }
    case GaussianSmearer::kKEVis: {
      return "KEVis";
    }
    case GaussianSmearer::kCosTheta: {
      return "CosTheta";
    }
    case GaussianSmearer::kTheta: {
      return "Theta";
    }
    default: { THROW("NO VAR!"); }
  }
}
}

/// Nodes look like:
/// Function attribute is given to a TF1, where any "V"s are replaced with the
/// selected kinematic property on an event-by-event basis. e.g Function="V +
/// gaus(0.2*V),..." should give the same result as Type="kFractional"
/// Width="0.2".
/// <GaussianSmearer Name="D00N_ND_LAr">
///   <Smear PDG="211" Type="[kAbsolute,kFractional,kFunction]"
///   Width="2" Kinematics="[KE|Momentum|VisKE|VisTE]" Function="V +
///   gaus(1/V),<lowlim>,<highlim>" />
/// </GaussianSmearer>
void GaussianSmearer::SpecifcSetup(nuiskey &nk) {
  rand.~TRandom3();
  new (&rand) TRandom3();

  std::vector<nuiskey> smearDescriptors = nk.GetListOfChildNodes("Smear");

  for (size_t t_it = 0; t_it < smearDescriptors.size(); ++t_it) {
    std::string pdgs_s = smearDescriptors[t_it].GetS("PDG");
    std::vector<int> pdgs_i = GeneralUtils::ParseToInt(pdgs_s, ",");

    double Width = smearDescriptors[t_it].Has("Width")
                       ? smearDescriptors[t_it].GetD("Width")
                       : 0xdeadbeef;

    GaussianSmearer::GSmearType Type =
        GetVarType(smearDescriptors[t_it].GetS("Type"));
    GaussianSmearer::DependVar Kinematics =
        GetKineType(smearDescriptors[t_it].GetS("Kinematics"));
    bool IsVisSmear = (Kinematics == GaussianSmearer::kKEVis) ||
                      (Kinematics == GaussianSmearer::kTEVis);

    TF1 *sf = NULL;

    if (Type == GaussianSmearer::kFunction) {
      std::string funcDescriptor = smearDescriptors[t_it].Has("Function")
                                       ? smearDescriptors[t_it].GetS("Function")
                                       : "";
      if (funcDescriptor.size()) {
        std::vector<std::string> funcP =
            GeneralUtils::ParseToStr(funcDescriptor, ",");
        if (funcP.size() != 3) {
          THROW(
              "Expected Function attribute to contain 3 comma separated "
              "entries. e.g. Function=\"1/x,<low lim>,<high lim>\". ");
        }
        funcP[0] = GeneralUtils::ReplaceAll(funcP[0], "V", "[0]");
        QLOG(FIT, "Added smearing func: " << funcP[0]);
        sf = new TF1("smear_dummy", funcP[0].c_str(),
                     GeneralUtils::StrToDbl(funcP[1]),
                     GeneralUtils::StrToDbl(funcP[2]));
      } else {
        THROW(
            "Expected Function attribute with 3 comma separated "
            "entries. e.g. Function=\"1/x,<low lim>,<high lim>\". ");
      }
    }

    for (size_t pdg_it = 0; pdg_it < pdgs_i.size(); ++pdg_it) {
      if (IsVisSmear && VisGausSmears.count(pdgs_i[pdg_it])) {
        ERROR(WRN,
              "Smearceptor "
                  << ElementName << ":" << InstanceName
                  << " already has a Visible Energy smearing function for PDG: "
                  << pdgs_i[pdg_it]);
      }

      GSmear gs;

      gs.type = Type;
      gs.smearVar = Kinematics;
      gs.width = Width;
      gs.func = sf ? static_cast<TF1 *>(sf->Clone()) : NULL;
      if (sf) {
        std::stringstream ss("");
        ss << "GausSmear"
           << "_PDG" << pdgs_i[pdg_it];
        gs.func->SetName(ss.str().c_str());
      }

      if (IsVisSmear) {
        VisGausSmears[pdgs_i[pdg_it]] = gs;
      } else {
        TrackedGausSmears[pdgs_i[pdg_it]].push_back(gs);
      }

      QLOG(SAM, "Added gaussian "
                    << GetKineTypeName(gs.smearVar)
                    << " smearing function for PDG: " << pdgs_i[pdg_it]);
    }
    delete sf;
  }
}

RecoInfo *GaussianSmearer::Smearcept(FitEvent *fe) {
  RecoInfo *ri = new RecoInfo();

  for (size_t p_it = 0; p_it < fe->NParticles(); ++p_it) {
    FitParticle *fp = fe->GetParticle(p_it);
#ifdef DEBUG_THRESACCEPT
    std::cout << std::endl;
    std::cout << "[" << p_it << "]: " << fp->PDG() << ", " << fp->Status()
              << ", " << fp->E() << " -- KE:" << fp->KE()
              << " Mom: " << fp->P3().Mag() << std::flush;
#endif

    if (fp->Status() != kFinalState) {
#ifdef DEBUG_THRESACCEPT
      std::cout << " -- Not final state." << std::flush;
#endif
      continue;
    }

    if ((TrackedGausSmears.count(fp->PDG()) + VisGausSmears.count(fp->PDG())) ==
        0) {
#ifdef DEBUG_THRESACCEPT
      std::cout << " -- Undetectable." << std::flush;
#endif
      continue;
    }

    if (TrackedGausSmears.count(fp->PDG())) {
      TVector3 ThreeMom = fp->P3();
      for (size_t sm_it = 0; sm_it < TrackedGausSmears[fp->PDG()].size();
           ++sm_it) {
        GSmear &sm = TrackedGausSmears[fp->PDG()][sm_it];

        double kineProp = 0;

        switch (sm.smearVar) {
          case GaussianSmearer::kMomentum: {
            kineProp = fp->P3().Mag();
            break;
          }
          case GaussianSmearer::kKE: {
            kineProp = fp->KE();
            break;
          }
          case GaussianSmearer::kCosTheta: {
            kineProp = fp->P3().CosTheta();
            break;
          }
          case GaussianSmearer::kTheta: {
            kineProp = fp->P3().Theta();
            break;
          }
          default: { THROW("Trying to find particle value for a kNoVar."); }
        }

        double Smeared;
        bool ok = false;
        while (!ok) {
          if (sm.type == GaussianSmearer::kFunction) {
            sm.func->SetParameter(0, kineProp);
            Smeared = sm.func->GetRandom();
          } else {
            double sThrow =
                rand.Gaus(0, sm.width * ((sm.type == GaussianSmearer::kAbsolute)
                                             ? 1
                                             : kineProp));
            Smeared = kineProp + sThrow;
          }
          switch (
              sm.smearVar) {  // Different kinematics have different truncation.
            case GaussianSmearer::kMomentum:
            case GaussianSmearer::kKE: {
              ok = (Smeared > 0);
              break;
            }
            case GaussianSmearer::kCosTheta: {
              ok = ((Smeared >= -1) && (Smeared <= 1));
              break;
            }
            case GaussianSmearer::kTheta: {
              ok = true;
              break;
            }

            default: { THROW("SHOULDN'T BE HERE."); }
          }
        }

        switch (sm.smearVar) {
          case GaussianSmearer::kMomentum: {
            ThreeMom = (ThreeMom.Unit() * Smeared);
            break;
          }
          case GaussianSmearer::kKE: {
            double mass = fp->P4().M();
            double TE = mass + Smeared;
            double magP = sqrt(TE * TE - mass * mass);
            ThreeMom = (ThreeMom.Unit() * magP);
            break;
          }
          case GaussianSmearer::kCosTheta: {
            ThreeMom.SetTheta(acos(Smeared));
            break;
          }
          case GaussianSmearer::kTheta: {
            ThreeMom.SetTheta(Smeared);
            break;
          }
          default: {}
        }
      }
#ifdef DEBUG_THRESACCEPT
      std::cout << " -- momentum reconstructed as Mom: "
                << ri->RecObjMom.back().Mag()
                << ", CT: " << ri->RecObjMom.back().CosTheta() << " from "
                << ThreeMom.Mag() << ", " << fp->P3().CosTheta() << " true."
                << std::endl;
#endif
      ri->RecObjMom.push_back(ThreeMom);
      ri->RecObjClass.push_back(fp->PDG());
    } else {  // Smear to EVis

      GSmear &sm = VisGausSmears[fp->PDG()];

      double kineProp = 0;

      switch (sm.smearVar) {
        case GaussianSmearer::kKEVis: {
          kineProp = fp->KE();
          break;
        }
        case GaussianSmearer::kTEVis: {
          kineProp = fp->E();
          break;
        }
        default: { THROW("Trying to find particle value for a kNoVar."); }
      }

      double Smeared;
      if (sm.type == GaussianSmearer::kFunction) {
        sm.func->SetParameter(0, kineProp);
        Smeared = sm.func->GetRandom();
      } else {
        double sThrow = rand.Gaus(
            0, sm.width *
                   ((sm.type == GaussianSmearer::kAbsolute) ? 1.0 : kineProp));
        Smeared = kineProp + sThrow;
      }
      Smeared = (Smeared < 0) ? 0 : Smeared;
#ifdef DEBUG_THRESACCEPT
      std::cout << " -- Saw " << Smeared << " visible energy from " << kineProp
                << " available. [PDG: " << fp->PDG() << "]" << std::endl;
#endif

      ri->RecVisibleEnergy.push_back(Smeared);
      ri->TrueContribPDGs.push_back(fp->PDG());
    }
#ifdef DEBUG_THRESACCEPT
    std::cout << std::endl;
#endif
  }

  return ri;
}

void GaussianSmearer::SmearRecoInfo(RecoInfo *ri) {
  // Smear tracked particles
  for (size_t p_it = 0; p_it < ri->RecObjMom.size(); ++p_it) {
    if (!TrackedGausSmears.count(ri->RecObjClass[p_it])) {
      continue;
    }
    TVector3 ThreeMom = ri->RecObjMom[p_it];
    TVector3 OriginalKP = ThreeMom;
    for (size_t sm_it = 0;
         sm_it < TrackedGausSmears[ri->RecObjClass[p_it]].size(); ++sm_it) {
      GSmear &sm = TrackedGausSmears[ri->RecObjClass[p_it]][sm_it];

      double kineProp = 0;

      switch (sm.smearVar) {
        case GaussianSmearer::kMomentum: {
          kineProp = ri->RecObjMom[p_it].Mag();
          break;
        }
        case GaussianSmearer::kKE: {
          double mass = PhysConst::GetMass(ri->RecObjClass[p_it]) * 1.0E3;
          kineProp = sqrt(ri->RecObjMom[p_it].Mag2() + mass * mass) - mass;
          break;
        }
        case GaussianSmearer::kCosTheta: {
          kineProp = OriginalKP.CosTheta();
          break;
        }
        case GaussianSmearer::kTheta: {
          kineProp = OriginalKP.Theta();
          break;
        }
        default: { THROW("Trying to find particle value for a kNoVar."); }
      }

      double Smeared;
      bool ok = false;
      int attempt = 0;
      while (!ok) {
        if (sm.type == GaussianSmearer::kFunction) {
          sm.func->SetParameter(0, kineProp);
          Smeared = sm.func->GetRandom();
        } else {
          double sThrow =
              rand.Gaus(0, sm.width * ((sm.type == GaussianSmearer::kAbsolute)
                                           ? 1.0
                                           : kineProp));
          Smeared = kineProp + sThrow;
#ifdef DEBUG_THRESACCEPT
          std::cout << "*** [" << attempt << "] Gaus(0,"
                    << (sm.width * (sm.type == GaussianSmearer::kAbsolute)
                            ? 1
                            : kineProp)
                    << "[" << sm.width << "]) = " << sThrow << ": " << kineProp
                    << " -> " << Smeared << std::endl;
#endif
        }
        switch (
            sm.smearVar) {  // Different kinematics have different truncation.
          case GaussianSmearer::kMomentum:
          case GaussianSmearer::kKE: {
            ok = (Smeared > 0);
            break;
          }
          case GaussianSmearer::kCosTheta: {
            ok = ((Smeared >= -1) && (Smeared <= 1));
            break;
          }
          case GaussianSmearer::kTheta: {
            ok = true;
            break;
          }
          default: { THROW("SHOULDN'T BE HERE."); }
        }
        attempt++;
      }

      switch (sm.smearVar) {
        case GaussianSmearer::kMomentum: {
          ThreeMom = (ThreeMom.Unit() * Smeared);
          break;
        }
        case GaussianSmearer::kKE: {
          double mass = PhysConst::GetMass(ri->RecObjClass[p_it]) * 1.0E3;
          double TE = mass + Smeared;
          double magP = sqrt(TE * TE - mass * mass);
          ThreeMom = (ThreeMom.Unit() * magP);
          break;
        }
        case GaussianSmearer::kCosTheta: {
          ThreeMom.SetTheta(acos(Smeared));
          break;
        }
        case GaussianSmearer::kTheta: {
          ThreeMom.SetTheta(Smeared);
          break;
        }
        default: {}
      }
    }
    ri->RecObjMom[p_it] = ThreeMom;

#ifdef DEBUG_THRESACCEPT
    std::cout << " -- momentum reconstructed as Mom: "
              << ri->RecObjMom[p_it].Mag()
              << ", CT: " << ri->RecObjMom[p_it].CosTheta() << " from "
              << OriginalKP.Mag() << ", " << OriginalKP.CosTheta() << " true."
              << std::endl;
#endif
  }

  for (size_t ve_it = 0; ve_it < ri->RecVisibleEnergy.size(); ++ve_it) {
    if (!VisGausSmears.count(ri->TrueContribPDGs[ve_it])) {
      continue;
    }
    GSmear &sm = VisGausSmears[ri->TrueContribPDGs[ve_it]];
    double kineProp = ri->RecVisibleEnergy[ve_it];

    double Smeared;
    if (sm.type == GaussianSmearer::kFunction) {
      sm.func->SetParameter(0, kineProp);
      Smeared = sm.func->GetRandom();
    } else {
      double sThrow = rand.Gaus(
          0, sm.width *
                 ((sm.type == GaussianSmearer::kAbsolute) ? 1.0 : kineProp));
      Smeared = kineProp + sThrow;
    }
    Smeared = (Smeared < 0) ? 0 : Smeared;
#ifdef DEBUG_THRESACCEPT
    std::cout << " -- Saw " << Smeared << " visible energy from " << kineProp
              << " available. [PDG: " << ri->TrueContribPDGs[ve_it] << "]"
              << std::endl;
#endif

    ri->RecVisibleEnergy[ve_it] = Smeared;
  }

#ifdef DEBUG_THRESACCEPT
  std::cout << std::endl;
#endif
}
