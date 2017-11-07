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

#include "ThresholdAccepter.h"

namespace {
ThresholdAccepter::KineVar GetKineType(nuiskey &nk) {
  if (nk.Has("RecoThresholdMomentum_MeV")) {
    return ThresholdAccepter::kMomentum;
  } else if (nk.Has("RecoThresholdKE_MeV")) {
    return ThresholdAccepter::kKE;
  } else if (nk.Has("RecoThresholdCosTheta_Max")) {
    return ThresholdAccepter::kCosTheta_Max;
  } else if (nk.Has("RecoThresholdCosTheta_Min")) {
    return ThresholdAccepter::kCosTheta_Min;
  } else if (nk.Has("RecoThresholdAbsCosTheta_Max")) {
    return ThresholdAccepter::kAbsCosTheta_Max;
  } else if (nk.Has("RecoThresholdAbsCosTheta_Min")) {
    return ThresholdAccepter::kAbsCosTheta_Min;
  } else {
    THROW("Cannot determine the threshold type for Smearcepter element.");
  }
  return ThresholdAccepter::kNoVar;
}

std::string GetKineTypeName(ThresholdAccepter::KineVar kv) {
  switch (kv) {
    case ThresholdAccepter::kMomentum:
      return "Momentum";
    case ThresholdAccepter::kKE:
      return "KE";
    case ThresholdAccepter::kCosTheta_Max:
      return "CosTheta_Max";
    case ThresholdAccepter::kCosTheta_Min:
      return "CosTheta_Min";
    case ThresholdAccepter::kAbsCosTheta_Max:
      return "AbsCosTheta_Max";
    case ThresholdAccepter::kAbsCosTheta_Min:
      return "CosTheta_Min";
    default:
      return "NoVar";
  }
}

double GetKineThreshold(nuiskey &nk, ThresholdAccepter::KineVar kv) {
  switch (kv) {
    case ThresholdAccepter::kMomentum:
      return nk.GetD("RecoThresholdMomentum_MeV");
    case ThresholdAccepter::kKE:
      return nk.GetD("RecoThresholdKE_MeV");
    case ThresholdAccepter::kCosTheta_Max:
      return nk.GetD("RecoThresholdCosTheta_Max");
    case ThresholdAccepter::kCosTheta_Min:
      return nk.GetD("RecoThresholdCosTheta_Min");
    case ThresholdAccepter::kAbsCosTheta_Max:
      return nk.GetD("RecoThresholdAbsCosTheta_Max");
    case ThresholdAccepter::kAbsCosTheta_Min:
      return nk.GetD("RecoThresholdAbsCosTheta_Min");
    default:
      return 0;
  }
}

double GetKineVal(FitParticle *fp, ThresholdAccepter::Thresh &rt) {
  switch (rt.ThresholdType) {
    case ThresholdAccepter::kMomentum:
      return fp->P3().Mag();
    case ThresholdAccepter::kKE:
      return fp->KE();
    case ThresholdAccepter::kCosTheta_Max:
      return fp->P3().CosTheta();
    case ThresholdAccepter::kCosTheta_Min:
      return fp->P3().CosTheta();
    case ThresholdAccepter::kAbsCosTheta_Max:
      return fabs(fp->P3().CosTheta());
    case ThresholdAccepter::kAbsCosTheta_Min:
      return fabs(fp->P3().CosTheta());
    default:
      return 0;
  }
}

bool PassesThreshold(FitParticle *fp, ThresholdAccepter::Thresh &rt) {
  switch (rt.ThresholdType) {
    case ThresholdAccepter::kMomentum:
      return (fp->P3().Mag() > rt.ThresholdVal);
    case ThresholdAccepter::kKE:
      return (fp->KE() > rt.ThresholdVal);
    case ThresholdAccepter::kCosTheta_Max:
      return (fp->P3().CosTheta() < rt.ThresholdVal);
    case ThresholdAccepter::kCosTheta_Min:
      return (fp->P3().CosTheta() > rt.ThresholdVal);
    case ThresholdAccepter::kAbsCosTheta_Max:
      return (fabs(fp->P3().CosTheta()) < rt.ThresholdVal);
    case ThresholdAccepter::kAbsCosTheta_Min:
      return (fabs(fp->P3().CosTheta()) > rt.ThresholdVal);
    default:
      return 0;
  }
}
}

/// Reads particle threshold nodes
///
/// Nodes look like:
/// <ThresholdAccepter Name="D00N_ND_LAr">
///   <RecoThreshold PDG="211,-211" RecoThresholdKE_MeV="30" />
///   <RecoThreshold PDG="211,-211" RecoThresholdCosTheta_Max="1" />
///   <RecoThreshold PDG="2212" RecoThresholdMomentum_MeV="350" />
///   <RecoThreshold PDG="2212" RecoThresholdAbsCosTheta_Min="0" />
///   <VisThreshold PDG="2212" VisThresholdKE_MeV="10" Contrib="K" />
///   <VisThreshold PDG="22" VisThresholdKE_MeV="10" Contrib="T" />
/// </ThresholdAccepter>
void ThresholdAccepter::SpecifcSetup(nuiskey &nk) {
  std::vector<nuiskey> recoThresholdDescriptors =
      nk.GetListOfChildNodes("RecoThreshold");

  for (size_t t_it = 0; t_it < recoThresholdDescriptors.size(); ++t_it) {
    std::string pdgs_s = recoThresholdDescriptors[t_it].GetS("PDG");
    std::vector<int> pdgs_i = GeneralUtils::ParseToInt(pdgs_s, ",");
    for (size_t pdg_it = 0; pdg_it < pdgs_i.size(); ++pdg_it) {
      Thresh t;
      t.ThresholdType = GetKineType(recoThresholdDescriptors[t_it]);
      t.ThresholdVal =
          GetKineThreshold(recoThresholdDescriptors[t_it], t.ThresholdType);

      ReconThresholds[pdgs_i[pdg_it]].push_back(t);

      QLOG(FIT, "Added reconstruction threshold of type: "
                    << ReconThresholds[pdgs_i[pdg_it]].back().ThresholdVal
                    << " "
                    << GetKineTypeName(
                           ReconThresholds[pdgs_i[pdg_it]].back().ThresholdType)
                    << ", for PDG: " << pdgs_i[pdg_it]);
    }
  }

  std::vector<nuiskey> visThresholdDescriptors =
      nk.GetListOfChildNodes("VisThreshold");

  for (size_t t_it = 0; t_it < visThresholdDescriptors.size(); ++t_it) {
    std::string pdgs_s = visThresholdDescriptors[t_it].GetS("PDG");
    std::vector<int> pdgs_i = GeneralUtils::ParseToInt(pdgs_s, ",");

    for (size_t pdg_it = 0; pdg_it < pdgs_i.size(); ++pdg_it) {
      if (VisThresholds.count(pdgs_i[pdg_it])) {
        ERROR(WRN, "Smearceptor " << ElementName << ":" << InstanceName
                                  << " already has a threshold for PDG: "
                                  << pdgs_i[pdg_it]);
      }
      VisThresh vt;
      vt.UseKE = visThresholdDescriptors[t_it].Has("Contrib")
                     ? (visThresholdDescriptors[t_it].GetS("Contrib") == "K")
                     : false;
      vt.Fraction = visThresholdDescriptors[t_it].Has("Fraction")
                        ? visThresholdDescriptors[t_it].GetD("Fraction")
                        : 1;
      if (visThresholdDescriptors[t_it].Has("VisThresholdKE_MeV")) {
        vt.ThresholdType = ThresholdAccepter::kKE;
        vt.ThresholdVal =
            visThresholdDescriptors[t_it].GetD("VisThresholdKE_MeV");
      } else if (visThresholdDescriptors[t_it].Has(
                     "VisThresholdMomentum_MeV")) {
        vt.ThresholdType = ThresholdAccepter::kMomentum;
        vt.ThresholdVal =
            visThresholdDescriptors[t_it].GetD("VisThresholdMomentum_MeV");
        ;
      } else {
        ERROR(WRN, "Smearceptor "
                       << ElementName << ":" << InstanceName
                       << " cannot find threshold information for PDG: "
                       << pdgs_i[pdg_it]);
        continue;
      }

      VisThresholds[pdgs_i[pdg_it]] = vt;

      QLOG(FIT,
           "Added visibility threshold of MeV "
               << VisThresholds[pdgs_i[pdg_it]].ThresholdVal << " "
               << GetKineTypeName(VisThresholds[pdgs_i[pdg_it]].ThresholdType)
               << ", for PDG: " << pdgs_i[pdg_it]
               << ". If visible, particle deposits: "
               << (VisThresholds[pdgs_i[pdg_it]].UseKE ? "KE" : "TE"));
    }
  }
}

void ThresholdAccepter::SmearceptOneParticle(RecoInfo *ri, FitParticle *fp
#ifdef DEBUG_THRESACCEPT
                                             ,
                                             size_t p_it
#endif
                                             ) {
#ifdef DEBUG_THRESACCEPT
  std::cout << std::endl;
  std::cout << "[" << p_it << " = " << fp << "]: " << fp->PDG() << ", "
            << fp->Status() << ", " << fp->E() << " -- KE:" << fp->KE()
            << " Mom: " << fp->P3().Mag() << std::flush;
#endif

  if (fp->Status() != kFinalState) {
#ifdef DEBUG_THRESACCEPT
    std::cout << " -- Not final state." << std::flush;
#endif
    return;
  }

  if ((ReconThresholds.count(fp->PDG()) + VisThresholds.count(fp->PDG())) ==
      0) {
#ifdef DEBUG_THRESACCEPT
    std::cout << " -- Undetectable." << std::flush;
#endif
    return;
  }

  // If no reco thresholds it should fall through to EVis
  bool Passes = ReconThresholds[fp->PDG()].size();
  bool FailEnergyThresh = !ReconThresholds[fp->PDG()].size();
  for (size_t rt_it = 0; rt_it < ReconThresholds[fp->PDG()].size(); ++rt_it) {
    bool Passed = PassesThreshold(fp, ReconThresholds[fp->PDG()][rt_it]);
    if (!Passed) {
#ifdef DEBUG_THRESACCEPT
      std::cout << "\n\t -- Rejected. ("
                << GetKineTypeName(
                       ReconThresholds[fp->PDG()][rt_it].ThresholdType)
                << " Threshold: "
                << ReconThresholds[fp->PDG()][rt_it].ThresholdVal << " | "
                << GetKineVal(fp, ReconThresholds[fp->PDG()][rt_it]) << ")"
                << std::flush;
#endif
      if ((ReconThresholds[fp->PDG()][rt_it].ThresholdType ==
           ThresholdAccepter::kMomentum) ||
          (ReconThresholds[fp->PDG()][rt_it].ThresholdType ==
           ThresholdAccepter::kKE)) {
        FailEnergyThresh = true;
      }
    } else {
#ifdef DEBUG_THRESACCEPT
      std::cout << "\n\t -- Accepted. ("
                << GetKineTypeName(
                       ReconThresholds[fp->PDG()][rt_it].ThresholdType)
                << " Threshold: "
                << ReconThresholds[fp->PDG()][rt_it].ThresholdVal << " | "
                << GetKineVal(fp, ReconThresholds[fp->PDG()][rt_it]) << ")"
                << std::flush;
#endif
    }
    Passes = Passes && Passed;
  }

  if (Passes) {
#ifdef DEBUG_THRESACCEPT
    std::cout << " -- Reconstructed." << std::flush;
#endif
    ri->RecObjMom.push_back(fp->P3());
    ri->RecObjClass.push_back(fp->PDG());

    return;
  } else if (!FailEnergyThresh) {
#ifdef DEBUG_THRESACCEPT
    std::cout << " -- Failed non-Energy threshold, no chance for EVis."
              << std::flush;
#endif
    return;
  }

  if (((VisThresholds[fp->PDG()].ThresholdType == ThresholdAccepter::kKE) &&
       (VisThresholds[fp->PDG()].ThresholdVal <
        fp->KE()))  // Above KE-style threshold
      || ((VisThresholds[fp->PDG()].ThresholdType ==
           ThresholdAccepter::kMomentum) &&
          (VisThresholds[fp->PDG()].ThresholdVal <
           fp->P3().Mag()))  // Above mom-style threshold
      ) {
#ifdef DEBUG_THRESACCEPT
    std::cout << " -- Contributed to VisE. ("
              << GetKineTypeName(VisThresholds[fp->PDG()].ThresholdType) << ": "
              << VisThresholds[fp->PDG()].ThresholdVal << ")" << std::flush;
#endif

    ri->RecVisibleEnergy.push_back(
        VisThresholds[fp->PDG()].Fraction *
        (VisThresholds[fp->PDG()].UseKE ? fp->KE() : fp->E()));
    ri->TrueContribPDGs.push_back(fp->PDG());

    return;
  } else {
#ifdef DEBUG_THRESACCEPT
    std::cout << " -- Rejected. "
              << " Vis: ("
              << GetKineTypeName(VisThresholds[fp->PDG()].ThresholdType) << ": "
              << VisThresholds[fp->PDG()].ThresholdVal << ")" << std::flush;
#endif
  }
}

RecoInfo *ThresholdAccepter::Smearcept(FitEvent *fe) {
  RecoInfo *ri = new RecoInfo();

  for (size_t p_it = 0; p_it < fe->NParticles(); ++p_it) {
    FitParticle *fp = fe->GetParticle(p_it);
    SmearceptOneParticle(ri, fp
#ifdef DEBUG_THRESACCEPT
                         ,
                         p_it
#endif
                         );
  }
#ifdef DEBUG_THRESACCEPT
  std::cout << std::endl;
#endif
  return ri;
}
