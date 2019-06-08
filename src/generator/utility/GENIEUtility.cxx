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

#include "generator/utility/GENIEUtility.hxx"
#include "generator/utility/GENIESplineReader.hxx"
#include "utility/HistogramUtility.hxx"
#include "utility/InteractionChannelUtility.hxx"
#include "utility/PDGCodeUtility.hxx"
#include "utility/ROOTUtility.hxx"
#include "utility/StringUtility.hxx"

#ifdef GENIE_V3_INTERFACE
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/GHEP/GHepUtils.h"
#include "Framework/ParticleData/PDGCodes.h"
#else
#include "GHEP/GHepParticle.h"
#include "GHEP/GHepRecord.h"
#include "GHEP/GHepUtils.h"
#include "PDG/PDGCodes.h"
#endif

#include "fhiclcpp/ParameterSet.h"

#include "string_parsers/from_string.hxx"

#include "TGraph.h"

namespace nuis {

using namespace event;

namespace genietools {

struct NFSParticleCount {
  size_t NProton;
  size_t NNeutron;
  size_t NPip;
  size_t NPi0;
  size_t NPim;
  size_t NOther;
};

NFSParticleCount CountPreFSIParticles(genie::GHepRecord const &ev) {
  // This code in this method is adapted from the GENIE source code found in
  // GHep/GHepUtils.cxx This method therefore carries the GENIE copyright
  // licence as copied below:
  //
  /// Copyright (c) 2003-2017, GENIE Neutrino MC Generator Collaboration
  /// For the full text of the license visit http://copyright.genie-mc.org
  /// or see $GENIE/LICENSE
  //

  genie::Target const &tgt = ev.Summary()->InitState().Tgt();
  if (!tgt.HitNucIsSet()) {
    throw invalid_GENIE_event()
        << "[ERROR]: Failed to get hit nucleon kinematics as it was not "
           "included in this GHep event. This is a fatal error.";
  }

  genie::GHepParticle *FSLep = ev.FinalStatePrimaryLepton();
  genie::GHepParticle *ISLep = ev.Probe();

  if (!FSLep || !ISLep) {
    throw invalid_GENIE_event()
        << "[ERROR]: Failed to find IS and FS lepton in event: "
        << ev.Summary()->AsString();
  }

  size_t NPi0 = 0, NPip = 0, NPim = 0, NProton = 0, NNeutron = 0, NOther = 0;

  bool nuclear_target = tgt.IsNucleus();

  TIter event_iter(&ev);
  genie::GHepParticle *p = 0;

  while ((p = dynamic_cast<genie::GHepParticle *>(event_iter.Next()))) {
    genie::GHepStatus_t ghep_ist = (genie::GHepStatus_t)p->Status();
    int ghep_pdgc = p->Pdg();
    int ghep_fm = p->FirstMother();
    int ghep_fmpdgc = (ghep_fm == -1) ? 0 : ev.Particle(ghep_fm)->Pdg();

    // For nuclear targets use hadrons marked as 'hadron in the nucleus'
    // which are the ones passed in the intranuclear rescattering
    // For free nucleon targets use particles marked as 'final state'
    // but make an exception for decayed pi0's,eta's (count them and not their
    // daughters)

    bool decayed =
        (ghep_ist == genie::kIStDecayedState &&
         (ghep_pdgc == genie::kPdgPi0 || ghep_pdgc == genie::kPdgEta));
    bool parent_included =
        (ghep_fmpdgc == genie::kPdgPi0 || ghep_fmpdgc == genie::kPdgEta);

    bool count_it =
        (nuclear_target && ghep_ist == genie::kIStHadronInTheNucleus) ||
        (!nuclear_target && decayed) ||
        (!nuclear_target && ghep_ist == genie::kIStStableFinalState &&
         !parent_included);

    if (!count_it) {
      continue;
    }

    if (ghep_pdgc == genie::kPdgPiP) {
      NPip++;
    } else if (ghep_pdgc == genie::kPdgPiM) {
      NPim++;
    } else if (ghep_pdgc == genie::kPdgPi0) {
      NPi0++;
    } else if (ghep_pdgc == genie::kPdgProton) {
      NProton++;
    } else if (ghep_pdgc == genie::kPdgNeutron) {
      NNeutron++;
    } else if (!utility::IsNeutralLepton(
                   ghep_pdgc, utility::pdgcodes::kMatterAntimatter) &&
               !utility::IsChargedLepton(
                   ghep_pdgc, utility::pdgcodes::kMatterAntimatter)) {
      NOther++;
    }
  }

  return NFSParticleCount{NProton, NNeutron, NPip, NPi0, NPim, NOther};
}

Channel_t GetEventChannel(genie::GHepRecord const &gev) {
  // Electron Scattering
  if (gev.Summary()->ProcInfo().IsEM()) {
    if (gev.Summary()->InitState().ProbePdg() == utility::pdgcodes::kElectron) {
      if (gev.Summary()->ProcInfo().IsQuasiElastic()) {
        NFSParticleCount fsparts = CountPreFSIParticles(gev);
        if (fsparts.NProton) {
          return Channel_t::kNCELP;
        } else {
          return Channel_t::kNCELN;
        }
      } else if (gev.Summary()->ProcInfo().IsMEC()) {
        return Channel_t::kNC2p2h;
      } else if (gev.Summary()->ProcInfo().IsResonant()) {

        NFSParticleCount fsparts = CountPreFSIParticles(gev);
        if (fsparts.NOther ||
            ((fsparts.NPip + fsparts.NPi0 + fsparts.NPim) > 1)) {
          return Channel_t::kNCTransitionMPi;
        } else if (fsparts.NPip) {
          return Channel_t::kNCSPP_NPip;
        } else if (fsparts.NPi0) {
          return fsparts.NProton ? Channel_t::kNCSPP_PPi0
                                 : Channel_t::kNCSPP_NPi0;
        } else if (fsparts.NPim) {
          return Channel_t::kNCSPP_PPim;
        }

        return Channel_t::kNCTransitionMPi;

      } else if (gev.Summary()->ProcInfo().IsDeepInelastic()) {
        return Channel_t::kNCDIS;
      } else {
        std::cout << "Unknown GENIE Electron Scattering Mode!" << std::endl
                  << "ScatteringTypeId = "
                  << gev.Summary()->ProcInfo().ScatteringTypeId() << " "
                  << "InteractionTypeId = "
                  << gev.Summary()->ProcInfo().InteractionTypeId() << std::endl
                  << genie::ScatteringType::AsString(
                         gev.Summary()->ProcInfo().ScatteringTypeId())
                  << " "
                  << genie::InteractionType::AsString(
                         gev.Summary()->ProcInfo().InteractionTypeId())
                  << " " << gev.Summary()->ProcInfo().IsMEC() << std::endl;
        return Channel_t::kUndefined;
      }
    }

    // Weak CC
  } else if (gev.Summary()->ProcInfo().IsWeakCC()) {
    // CC MEC
    if (gev.Summary()->ProcInfo().IsMEC()) {
      if (utility::IsNeutralLepton(gev.Summary()->InitState().ProbePdg(),
                                   utility::pdgcodes::kMatter)) {
        return Channel_t::kCC2p2h;
      } else if (utility::IsNeutralLepton(gev.Summary()->InitState().ProbePdg(),
                                          utility::pdgcodes::kAntimatter)) {
        return Channel_t::kCC2p2h_nub;
      }
      // CC OTHER
    } else {
      return FromNEUTCode(genie::utils::ghep::NeutReactionCode(&gev));
    }

    // Weak NC
  } else if (gev.Summary()->ProcInfo().IsWeakNC()) {
    // NC MEC
    if (gev.Summary()->ProcInfo().IsMEC()) {
      if (utility::IsNeutralLepton(gev.Summary()->InitState().ProbePdg(),
                                   utility::pdgcodes::kMatter)) {
        return Channel_t::kNC2p2h;
      } else if (utility::IsNeutralLepton(gev.Summary()->InitState().ProbePdg(),
                                          utility::pdgcodes::kAntimatter)) {
        return Channel_t::kNC2p2h_nub;
      }

      // NC OTHER
    } else {
      return FromNEUTCode(genie::utils::ghep::NeutReactionCode(&gev));
    }
  }

  return Channel_t::kUndefined;
}

Particle::Status_t GetParticleStatus(genie::GHepParticle const &p,
                                     Channel_t chan) {
  /*
     kIStUndefined                  = -1,
     kIStInitialState               =  0,   / generator-level initial state /
     kIStStableFinalState           =  1,   / generator-level final state:
     particles to be tracked by detector-level MC /
     kIStIntermediateState          =  2,
     kIStDecayedState               =  3,
     kIStCorrelatedNucleon          = 10,
     kIStNucleonTarget              = 11,
     kIStDISPreFragmHadronicState   = 12,
     kIStPreDecayResonantState      = 13,
     kIStHadronInTheNucleus         = 14,   / hadrons inside the nucleus: marked
     for hadron transport modules to act on /
     kIStFinalStateNuclearRemnant   = 15,   / low energy nuclear fragments
     entering the record collectively as a 'hadronic blob' pseudo-particle /
     kIStNucleonClusterTarget       = 16,   // for composite nucleons before
     phase space decay
     */

  Particle::Status_t state = Particle::Status_t::kUnknown;
  switch (p.Status()) {
  case genie::kIStNucleonTarget:
  case genie::kIStInitialState:
  case genie::kIStCorrelatedNucleon:
  case genie::kIStNucleonClusterTarget: {
    state = Particle::Status_t::kPrimaryInitialState;
    break;
  }

  case genie::kIStStableFinalState: {
    state = Particle::Status_t::kNuclearLeaving;
    break;
  }

  case genie::kIStHadronInTheNucleus: {
    state = utility::Is2p2h(chan) ? Particle::Status_t::kPrimaryInitialState
                                  : Particle::Status_t::kIntermediate;
    break;
  }

  case genie::kIStPreDecayResonantState:
  case genie::kIStDISPreFragmHadronicState:
  case genie::kIStIntermediateState: {
    state = Particle::Status_t::kIntermediate;
    break;
  }

  case genie::kIStFinalStateNuclearRemnant:
  case genie::kIStUndefined:
  case genie::kIStDecayedState:
  default: { state = Particle::Status_t::kUnknown; }
  }

  if (utility::IsNuclearPDG(p.Pdg())) {
    if (state == Particle::Status_t::kPrimaryInitialState) {
      state = Particle::Status_t::kPrimaryInitialState;
    } else if (state == Particle::Status_t::kNuclearLeaving) {
      state = Particle::Status_t::kPrimaryFinalState;
    }
  }
  return state;
}

struct TargetSplineBlob {
  double MolecularWeight;
  size_t NNucleons;
  std::string search_term;
  TGraph TotSpline;
};

NEW_NUIS_EXCEPT(Invalid_target_specifier);

double GetFileWeight(fhicl::ParameterSet const &xsecinfo,
                     std::set<std::string> const &spline_list) {
  double mec_scale = xsecinfo.get<double>("mec_scale", 1);
  double EMin = xsecinfo.get<double>("EMin", 0);
  double EMax = xsecinfo.get<double>("EMax", 10);
  size_t NKnots = xsecinfo.get<size_t>("NKnots", 100);

  std::vector<TargetSplineBlob> TargetSplines;
  int probe = xsecinfo.get<int>("nu_pdg");

  for (std::string const &target :
       xsecinfo.get<std::vector<std::string>>("target")) {
    std::vector<std::string> splits = nuis::utility::split(target, ";");
    if (splits.size() != 1 && splits.size() != 2) {
      throw Invalid_target_specifier()
          << "Expected to find 100ZZZAAA0;<MolecularWeight>, e.g. "
             "1000060120;1, "
             "but found: \""
          << target << "\"";
    }
    std::stringstream ss;
    ss << ".*nu:" << probe << ";tgt:" << splits[0] << ".*";

    size_t nuc_pdg = fhicl::string_parsers::str2T<size_t>(splits[0]);

    double molwght = splits.size() == 2
                         ? fhicl::string_parsers::str2T<double>(splits[1])
                         : 1;

    TargetSplines.push_back(
        {molwght, (nuc_pdg % 1000) / 10, ss.str(), TGraph()});

    std::cout << "[INFO]:\tGetting splines that match: \"" << ss.str()
              << "\" with A = " << TargetSplines.back().NNucleons
              << " and multiplied by "
              << molwght / double(TargetSplines.back().NNucleons) << std::endl;
  }

  size_t NTotNucleons = 0;
  std::vector<std::string> spline_patterns;
  for (auto const &spb : TargetSplines) {
    spline_patterns.push_back(spb.search_term);
    NTotNucleons += spb.MolecularWeight * spb.NNucleons;
  }
  double WeightToPerNucleon = 1.0 / double(NTotNucleons);

  // Read the spline file
  std::cout << "[INFO]: Reading input file: "
            << xsecinfo.get<std::string>("spline_file")
            << ", this may take a while..." << std::endl;
  GENIESplineGetter spg(spline_patterns);
  spg.ParseFile(xsecinfo.get<std::string>("spline_file").c_str());
  std::vector<std::vector<TGraph>> all_splines = spg.GetTGraphs();

  double step = (EMax - EMin) / double(NKnots);

  // Sum each targets-worth of interaction splines together, weighting for
  // molecular fraction and possibly fixing the MEC spline oddity
  for (size_t ts_it = 0; ts_it < TargetSplines.size(); ++ts_it) {

    TargetSplines[ts_it].TotSpline = TGraph(NKnots);

    for (size_t p_it = 0; p_it < NKnots; ++p_it) {
      TargetSplines[ts_it].TotSpline.SetPoint(p_it, EMin + p_it * step, 0);
    }

    double target_weight = (TargetSplines[ts_it].MolecularWeight /
                            double(TargetSplines[ts_it].NNucleons));
    for (size_t c_it = 0; c_it < all_splines[ts_it].size(); ++c_it) {
      double weight = target_weight;
      if (std::string(all_splines[ts_it][c_it].GetName()).find("MEC") !=
          std::string::npos) {
        if (mec_scale != 1) {
          std::cout << "[INFO]: Weighting MEC splines by " << mec_scale
                    << std::endl;
          weight *= mec_scale;
        }
      }
      for (size_t p_it = 0; p_it < NKnots; ++p_it) {
        double E, XSec;
        TargetSplines[ts_it].TotSpline.GetPoint(p_it, E, XSec);

        // Copied from GENIE/Convents/Units.h
        static const double gigaelectronvolt = 1.;
        static const double GeV = gigaelectronvolt;
        static const double meter = 5.07e+15 / GeV;
        static const double centimeter = 0.01 * meter;
        static const double centimeter2 = centimeter * centimeter;

        XSec += all_splines[ts_it][c_it].Eval(E) * weight / centimeter2;

        TargetSplines[ts_it].TotSpline.SetPoint(p_it, E, XSec);
      }
    }
  }

  // Sum all the correctly weighted per-target splines
  TGraph MasterSpline(NKnots);

  for (size_t p_it = 0; p_it < NKnots; ++p_it) {
    MasterSpline.SetPoint(p_it, EMin + p_it * step, 0);
  }
  for (size_t c_it = 0; c_it < TargetSplines.size(); ++c_it) {
    for (size_t p_it = 0; p_it < NKnots; ++p_it) {
      double E, XSec;
      MasterSpline.GetPoint(p_it, E, XSec);
      XSec += TargetSplines[c_it].TotSpline.Eval(E) * WeightToPerNucleon;

      MasterSpline.SetPoint(p_it, E, XSec);
    }
  }

  nuis::utility::TFile_ptr outfile(nullptr, [](TFile *) {});
  if (xsecinfo.has_key("spline_output_file")) {
    outfile = nuis::utility::CheckOpenTFile(
        xsecinfo.get<std::string>("spline_output_file"), "RECREATE");

    for (auto &sp : TargetSplines) {
      std::string spname = nuis::utility::SanitizeROOTObjectName(
          sp.search_term.substr(2, sp.search_term.length() - 4));
      std::cout << "[INFO]: Dumping spline: " << spname << std::endl;
      nuis::utility::WriteToTFile(outfile, &sp.TotSpline, spname.c_str());
    }
    nuis::utility::WriteToTFile(outfile, &MasterSpline, "TotalXSec");
  }

  fhicl::ParameterSet fluxps = xsecinfo.get<fhicl::ParameterSet>("flux");
  // If MonoE
  if (fluxps.has_key("energy_GeV")) {
    double monoE = fluxps.get<double>("energy_GeV");

    return MasterSpline.Eval(monoE);
  } else {
    // Use the master spline to get flux*xsec (which the events are thrown
    // according to) and return the flux-averaged total xsec which is used for
    // weighting
    std::unique_ptr<TH1> Flux =
        nuis::utility::GetHistogram<TH1>(fluxps.get<std::string>("histogram"));

    if (outfile) {
      std::unique_ptr<TH1> Fluxcp = nuis::utility::Clone<TH1>(Flux);
      nuis::utility::WriteToTFile(outfile, Fluxcp.get(), "Flux");
    }

    bool per_width = fluxps.get<bool>("is_divided_by_bin_width", true);
    double FluxIntegral = Flux->Integral(per_width ? "width" : "");

    for (int bi_it = 0; bi_it < Flux->GetXaxis()->GetNbins(); ++bi_it) {
      double bc = Flux->GetBinContent(bi_it + 1);
      double low_e = Flux->GetXaxis()->GetBinLowEdge(bi_it + 1);
      double up_e = Flux->GetXaxis()->GetBinUpEdge(bi_it + 1);

      size_t NIntSteps = 100;
      double step = (up_e - low_e) / double(NIntSteps);

      double avg_xsec = 0;
      for (size_t i = 0; i < NIntSteps; ++i) {
        double e = low_e + (double(i) + 0.5) * step;
        avg_xsec += MasterSpline.Eval(e);
      }
      avg_xsec /= double(NIntSteps);

      Flux->SetBinContent(bi_it + 1, bc * avg_xsec);
    }

    if (outfile) {
      std::unique_ptr<TH1> Fluxcp = nuis::utility::Clone<TH1>(Flux);
      nuis::utility::WriteToTFile(outfile, Fluxcp.get(), "EvRate");
    }

    double EvRateIntegral = Flux->Integral(per_width ? "width" : "");

    return EvRateIntegral / FluxIntegral;
  }
}

} // namespace genietools
} // namespace nuis
