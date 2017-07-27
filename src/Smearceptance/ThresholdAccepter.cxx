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

// #define DEBUG_THRESACCEPT

/// Reads particle threshold nodes
///
/// Nodes look like:
/// <ThresholdAccepter Name="D00N_ND_LAr">
///   <RecoThreshold PDG="211,-211" RecoThresholdKE_MeV="30" />
///   <RecoThreshold PDG="2212" RecoThresholdMom_MeV="350" />
///   <VisThreshold PDG="2212" RecoThresholdKE_MeV="10" Contrib="K" />
///   <VisThreshold PDG="22" RecoThresholdKE_MeV="10" Contrib="T" />
/// </ThresholdAccepter>
void ThresholdAccepter::SpecifcSetup(nuiskey &nk) {
  std::vector<nuiskey> recoThresholdDescriptors =
      nk.GetListOfChildNodes("RecoThreshold");

  for (size_t t_it = 0; t_it < recoThresholdDescriptors.size(); ++t_it) {
    std::string pdgs_s = recoThresholdDescriptors[t_it].GetS("PDG");
    std::vector<int> pdgs_i = GeneralUtils::ParseToInt(pdgs_s, ",");
    for (size_t pdg_it = 0; pdg_it < pdgs_i.size(); ++pdg_it) {
      if (ReconThresholds.count(pdgs_i[pdg_it])) {
        ERROR(WRN, "Smearceptor " << ElementName << ":" << InstanceName
                                  << " already has a threshold for PDG: "
                                  << pdgs_i[pdg_it]);
      }
      Thresh t;
      if (recoThresholdDescriptors[t_it].Has("RecoThresholdKE_MeV")) {
        t.ThresholdIsKE = true;
        t.ThresholdVal =
            recoThresholdDescriptors[t_it].GetD("RecoThresholdKE_MeV");
      } else if (recoThresholdDescriptors[t_it].Has("RecoThresholdMom_MeV")) {
        t.ThresholdIsKE = false;
        t.ThresholdVal =
            recoThresholdDescriptors[t_it].GetD("RecoThresholdMom_MeV");
      } else {
        ERROR(WRN, "Smearceptor "
                       << ElementName << ":" << InstanceName
                       << " cannot find threshold information for PDG: "
                       << pdgs_i[pdg_it]);
        continue;
      }

      ReconThresholds[pdgs_i[pdg_it]] = t;

      QLOG(SAM,
           "Added reconstruction threshold of MeV "
               << ReconThresholds[pdgs_i[pdg_it]].ThresholdVal << " "
               << (ReconThresholds[pdgs_i[pdg_it]].ThresholdIsKE ? "KE" : "Mom")
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
      if (visThresholdDescriptors[t_it].Has("VisThresholdKE_MeV")) {
        vt.ThresholdIsKE = true;
        vt.ThresholdVal =
            visThresholdDescriptors[t_it].GetD("VisThresholdKE_MeV");
        vt.UseKE = (visThresholdDescriptors[t_it].GetS("Contrib") == "K");
      } else if (visThresholdDescriptors[t_it].Has("VisThresholdMom_MeV")) {
        vt.ThresholdIsKE = false;
        vt.ThresholdVal =
            visThresholdDescriptors[t_it].GetD("VisThresholdMom_MeV");
        vt.UseKE = (visThresholdDescriptors[t_it].GetS("Contrib") == "K");
      } else {
        ERROR(WRN, "Smearceptor "
                       << ElementName << ":" << InstanceName
                       << " cannot find threshold information for PDG: "
                       << pdgs_i[pdg_it]);
        continue;
      }

      VisThresholds[pdgs_i[pdg_it]] = vt;

      QLOG(SAM,
           "Added visibility threshold of MeV "
               << VisThresholds[pdgs_i[pdg_it]].ThresholdVal << " "
               << (VisThresholds[pdgs_i[pdg_it]].ThresholdIsKE ? "KE" : "Mom")
               << ", for PDG: " << pdgs_i[pdg_it]
               << ". If visible, particle deposits: "
               << (VisThresholds[pdgs_i[pdg_it]].UseKE ? "KE" : "TE"));
    }
  }
}

RecoInfo *ThresholdAccepter::Smearcept(FitEvent *fe) {
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

    if (!ReconThresholds.count(fp->PDG())) {
#ifdef DEBUG_THRESACCEPT
      std::cout << " -- Undetectable." << std::flush;
#endif
      continue;
    }

    if ((ReconThresholds[fp->PDG()].ThresholdIsKE &&
         (ReconThresholds[fp->PDG()].ThresholdVal <
          fp->KE()))  // Above KE-style threshold
        || (!ReconThresholds[fp->PDG()].ThresholdIsKE &&
            (ReconThresholds[fp->PDG()].ThresholdVal <
             fp->P3().Mag()))  // Above mom-style threshold
        ) {
#ifdef DEBUG_THRESACCEPT
      std::cout << " -- Reconstructed. ("
                << (ReconThresholds[fp->PDG()].ThresholdIsKE ? "KE" : "Mom")
                << ": " << ReconThresholds[fp->PDG()].ThresholdVal << ")"
                << std::flush;
#endif
      ri->RecObjMom.push_back(fp->P3());
      ri->RecObjClass.push_back(fp->PDG());

      continue;
    }

    if ((VisThresholds[fp->PDG()].ThresholdIsKE &&
         (VisThresholds[fp->PDG()].ThresholdVal <
          fp->KE()))  // Above KE-style threshold
        || (!VisThresholds[fp->PDG()].ThresholdIsKE &&
            (VisThresholds[fp->PDG()].ThresholdVal <
             fp->P3().Mag()))  // Above mom-style threshold
        ) {
#ifdef DEBUG_THRESACCEPT
      std::cout << " -- Contributed to VisE. ("
                << (VisThresholds[fp->PDG()].ThresholdIsKE ? "KE" : "Mom")
                << ": " << VisThresholds[fp->PDG()].ThresholdVal << ")"
                << std::flush;
#endif

      ri->RecVisibleEnergy.push_back(VisThresholds[fp->PDG()].UseKE ? fp->KE()
                                                                    : fp->E());
      ri->TrueContribPDGs.push_back(fp->PDG());

      continue;
    }
#ifdef DEBUG_THRESACCEPT
    std::cout << " -- Rejected. "
              << "Reco: ("
              << (ReconThresholds[fp->PDG()].ThresholdIsKE ? "KE" : "Mom")
              << ": " << ReconThresholds[fp->PDG()].ThresholdVal << ")"
              << " Vis: ("
              << (VisThresholds[fp->PDG()].ThresholdIsKE ? "KE" : "Mom") << ": "
              << VisThresholds[fp->PDG()].ThresholdVal << ")" << std::flush;
#endif
  }
#ifdef DEBUG_THRESACCEPT
  std::cout << std::endl;
#endif
  return ri;
}
