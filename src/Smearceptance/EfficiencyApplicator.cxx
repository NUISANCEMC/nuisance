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

#include "EfficiencyApplicator.h"

#include "TEfficiency.h"
#include "TH2.h"
#include "TH3.h"

//#define DEBUG_EFFAPP

EfficiencyApplicator::DependVar GetVarType(std::string const &axisvar) {
  if (axisvar == "kMomentum") {
    return EfficiencyApplicator::kMomentum;
  } else if (axisvar == "kKE") {
    return EfficiencyApplicator::kKE;
  } else if (axisvar == "kTheta") {
    return EfficiencyApplicator::kTheta;
  } else if (axisvar == "kCosTheta") {
    return EfficiencyApplicator::kCosTheta;
  } else if (axisvar == "kPhi") {
    return EfficiencyApplicator::kPhi;
  }
  return EfficiencyApplicator::kNoAxis;
}

TH1 *GetEffHist(TFile *inputFile, std::string const &HistName) {
  TH1 *hist = dynamic_cast<TH1 *>(inputFile->Get(HistName.c_str()));
  if (hist) {
    return hist;
  }

  TEfficiency *effHist =
      dynamic_cast<TEfficiency *>(inputFile->Get(HistName.c_str()));

  if (effHist) {
    TH1D *numer = dynamic_cast<TH1D *>(effHist->GetCopyPassedHisto());
    TH1D *denom = dynamic_cast<TH1D *>(effHist->GetCopyTotalHisto());

    if (numer && denom) {
      numer->Divide(denom);

      denom->SetDirectory(NULL);
      delete denom;

      // Gonna be a memory leak, but I'll get over it
      numer->SetDirectory(NULL);
      return numer;
    }
    NUIS_ERR(FTL, "TEfficiency internal histograms were not TH1Ds.");
  }

  NUIS_ABORT("Couldn't get appropriate efficiency object named "
        << HistName << " from input file " << inputFile->GetName());
}

/// Reads particle efficiency nodes
///
/// Nodes look like:
/// <EfficiencyApplicator Name="D00N_ND_LAr">
///   <EfficiencyCurve PDG="211,-211" InputFile="effs.root"
///   HistName="cpion_eff_mom_ctheta" NDims="2" XAxis="kMomentum"
///   YAxis="kCosTheta" Interpolate="false" />
/// <!-- Can also contain ThresholdAccepter elements as below-->
///   <RecoThreshold PDG="2212" RecoThresholdAbsCosTheta_Min="0" />
///   <VisThreshold PDG="2212" VisThresholdKE_MeV="10" Contrib="K" />
/// </EfficiencyApplicator>
void EfficiencyApplicator::SpecifcSetup(nuiskey &nk) {
  rand.~TRandom3();
  new (&rand) TRandom3();

  std::vector<nuiskey> effDescriptors =
      nk.GetListOfChildNodes("EfficiencyCurve");

  for (size_t t_it = 0; t_it < effDescriptors.size(); ++t_it) {
    std::string inputFileName = effDescriptors[t_it].GetS("InputFile");
    std::string HistName = effDescriptors[t_it].GetS("HistName");

    TFile inputFile(inputFileName.c_str());
    if (!inputFile.IsOpen()) {
      NUIS_ABORT("Couldn't open specified input root file: " << inputFileName);
    }

    TH1 *inpHist = GetEffHist(&inputFile, HistName);
    if (!inpHist) {
      NUIS_ABORT("Couldn't get TH1D named: " << HistName << " from input root file: "
                                        << inputFileName);
    }

    int NDims = effDescriptors[t_it].GetI("NDims");

    if (NDims < 1 || NDims > 3) {
      NUIS_ABORT("Read NDims attribute as: " << NDims << ", efficiency curve can "
                                                    "currently have between 1 "
                                                    "and 3 dimensions.");
    }

    EfficiencyApplicator::DependVar XVar =
        GetVarType(effDescriptors[t_it].GetS("XAxis"));
    double XAxisScale = effDescriptors[t_it].Has("XAxisScaleToInternal")
                            ? effDescriptors[t_it].GetD("XAxisScaleToInternal")
                            : 1;
    EfficiencyApplicator::DependVar YVar =
        NDims > 1 ? GetVarType(effDescriptors[t_it].GetS("YAxis"))
                  : EfficiencyApplicator::kNoAxis;
    double YAxisScale = effDescriptors[t_it].Has("YAxisScaleToInternal")
                            ? effDescriptors[t_it].GetD("YAxisScaleToInternal")
                            : 1;
    EfficiencyApplicator::DependVar ZVar =
        NDims > 2 ? GetVarType(effDescriptors[t_it].GetS("ZAxis"))
                  : EfficiencyApplicator::kNoAxis;
    double ZAxisScale = effDescriptors[t_it].Has("ZAxisScaleToInternal")
                            ? effDescriptors[t_it].GetD("ZAxisScaleToInternal")
                            : 1;

    bool Interpolate = effDescriptors[t_it].Has("Interpolate") &&
                       effDescriptors[t_it].GetI("Interpolate");

    // bool ApplyAsWeight = effDescriptors[t_it].Has("ApplyAsWeight") &&
    //                      effDescriptors[t_it].GetI("ApplyAsWeight");

    std::string pdgs_s = effDescriptors[t_it].GetS("PDG");
    std::vector<int> pdgs_i = GeneralUtils::ParseToInt(pdgs_s, ",");
    for (size_t pdg_it = 0; pdg_it < pdgs_i.size(); ++pdg_it) {
      if (Efficiencies.count(pdgs_i[pdg_it])) {
        NUIS_ERR(WRN, "Smearceptor " << ElementName << ":" << InstanceName
                                  << " already has a efficiency for PDG: "
                                  << pdgs_i[pdg_it]);
      }

      EffMap em;
      em.EffCurve = static_cast<TH1 *>(inpHist->Clone());
      em.EffCurve->SetDirectory(NULL);
      em.Interpolate = Interpolate;
      // em.ApplyAsWeight = ApplyAsWeight;
      em.NDims = NDims;
      em.DependVars[0] = XVar;
      em.DependVars[1] = YVar;
      em.DependVars[2] = ZVar;
      em.AxisScales[0] = XAxisScale;
      em.AxisScales[1] = YAxisScale;
      em.AxisScales[2] = ZAxisScale;

      Efficiencies[pdgs_i[pdg_it]] = em;

      NUIS_LOG(FIT,
           "Added reconstruction efficiency curve for PDG: " << pdgs_i[pdg_it]);
    }
  }

  SlaveTA.Setup(nk);
}

RecoInfo *EfficiencyApplicator::Smearcept(FitEvent *fe) {
  RecoInfo *ri = new RecoInfo();

  for (size_t p_it = 0; p_it < fe->NParticles(); ++p_it) {
    FitParticle *fp = fe->GetParticle(p_it);
#ifdef DEBUG_EFFAPP
    std::cout << std::endl;
    std::cout << "[" << p_it << "]: " << fp->PDG() << ", " << fp->Status()
              << ", " << fp->E() << " -- KE:" << fp->KE()
              << " Mom: " << fp->P3().Mag() << std::flush;
#endif

    if (fp->Status() != kFinalState) {
#ifdef DEBUG_EFFAPP
      std::cout << " -- Not final state." << std::flush;
#endif
      continue;
    }

    if (!Efficiencies.count(fp->PDG())) {
      SlaveTA.SmearceptOneParticle(ri, fp
#ifdef DEBUG_THRESACCEPT
                                   ,
                                   p_it
#endif
                                   );
      continue;
    }

    EffMap &em = Efficiencies[fp->PDG()];

    double kineProps[3];
    for (Int_t dim_it = 0; dim_it < em.NDims; ++dim_it) {
      switch (em.DependVars[dim_it]) {
        case kMomentum: {
          kineProps[dim_it] = fp->P3().Mag();
          break;
        }
        case kKE: {
          kineProps[dim_it] = fp->KE();
          break;
        }
        case kTheta: {
          kineProps[dim_it] = fp->P3().Theta();
          break;
        }
        case kCosTheta: {
          kineProps[dim_it] = fp->P3().CosTheta();
          break;
        }
        case kPhi: {
          kineProps[dim_it] = fp->P3().Phi();
          break;
        }
        default: { NUIS_ABORT("Trying to find particle value for a kNoAxis."); }
      }
      kineProps[dim_it] /= em.AxisScales[dim_it];
    }

    double effProb = 0;

    switch (em.NDims) {
      case 1: {
        TH1 *hist = em.EffCurve;
        if (em.Interpolate &&
            (hist->GetXaxis()->GetBinCenter(1) < kineProps[0]) &&
            (hist->GetXaxis()->GetBinCenter(hist->GetXaxis()->GetNbins()) >
             kineProps[0])) {
          effProb = hist->Interpolate(kineProps[0]);
        } else {
          Int_t xbin = hist->GetXaxis()->FindFixBin(kineProps[0]);

          if (!xbin || ((hist->GetXaxis()->GetNbins() + 1) == xbin)) {
            NUIS_ERR(WRN, "Tried to apply effiency but XBin: "
                           << xbin << " is outside range (/"
                           << hist->GetXaxis()->GetNbins() << "): Prop "
                           << kineProps[0] << ", ["
                           << hist->GetXaxis()->GetBinLowEdge(1) << " -- "
                           << hist->GetXaxis()->GetBinUpEdge(
                                  hist->GetXaxis()->GetNbins()));
          }

          effProb = hist->GetBinContent(xbin);
        }
        break;
      }
      case 2: {
        TH2 *hist = static_cast<TH2 *>(em.EffCurve);

        if (em.Interpolate &&
            (hist->GetXaxis()->GetBinCenter(1) < kineProps[0]) &&
            (hist->GetXaxis()->GetBinCenter(hist->GetXaxis()->GetNbins()) >
             kineProps[0]) &&
            (hist->GetYaxis()->GetBinCenter(1) < kineProps[1]) &&
            (hist->GetYaxis()->GetBinCenter(hist->GetYaxis()->GetNbins()) >
             kineProps[1])) {
          effProb = hist->Interpolate(kineProps[0], kineProps[1]);
        } else {
          Int_t xbin = hist->GetXaxis()->FindFixBin(kineProps[0]);
          Int_t ybin = hist->GetYaxis()->FindFixBin(kineProps[1]);

          if (!xbin || ((hist->GetXaxis()->GetNbins() + 1) == xbin)) {
            NUIS_ERR(WRN, "Tried to apply effiency but XBin: "
                           << xbin << " is outside range (/"
                           << hist->GetXaxis()->GetNbins() << "): Prop "
                           << kineProps[0] << ", ["
                           << hist->GetXaxis()->GetBinLowEdge(1) << " -- "
                           << hist->GetXaxis()->GetBinUpEdge(
                                  hist->GetXaxis()->GetNbins()));
          }

          if (!ybin || ((hist->GetYaxis()->GetNbins() + 1) == ybin)) {
            NUIS_ERR(WRN, "Tried to apply effiency but XBin: "
                           << ybin << " is outside range (/"
                           << hist->GetYaxis()->GetNbins() << "): Prop "
                           << kineProps[0] << ", ["
                           << hist->GetYaxis()->GetBinLowEdge(1) << " -- "
                           << hist->GetYaxis()->GetBinUpEdge(
                                  hist->GetYaxis()->GetNbins()));
          }

          effProb = hist->GetBinContent(xbin, ybin);

#ifdef DEBUG_EFFAPP
          std::cout << "\t\t: XProp: " << kineProps[0]
                    << ", YProp: " << kineProps[1] << " x/y bins: " << xbin
                    << "/" << ybin << ". Prop ? " << effProb << std::endl;
#endif
        }
        break;
      }
      case 3: {
        TH3 *hist = static_cast<TH3 *>(em.EffCurve);

        if (em.Interpolate &&
            (hist->GetXaxis()->GetBinCenter(1) < kineProps[0]) &&
            (hist->GetXaxis()->GetBinCenter(hist->GetXaxis()->GetNbins()) >
             kineProps[0]) &&
            (hist->GetYaxis()->GetBinCenter(1) < kineProps[1]) &&
            (hist->GetYaxis()->GetBinCenter(hist->GetYaxis()->GetNbins()) >
             kineProps[2]) &&
            (hist->GetZaxis()->GetBinCenter(hist->GetZaxis()->GetNbins()) >
             kineProps[2])) {
          effProb = hist->Interpolate(kineProps[0], kineProps[1], kineProps[2]);
        } else {
          Int_t xbin = hist->GetXaxis()->FindFixBin(kineProps[0]);
          Int_t ybin = hist->GetYaxis()->FindFixBin(kineProps[1]);
          Int_t zbin = hist->GetZaxis()->FindFixBin(kineProps[2]);

          if (!xbin || ((hist->GetXaxis()->GetNbins() + 1) == xbin)) {
            NUIS_ERR(WRN, "Tried to apply effiency but XBin: "
                           << xbin << " is outside range (/"
                           << hist->GetXaxis()->GetNbins() << "): Prop "
                           << kineProps[0] << ", ["
                           << hist->GetXaxis()->GetBinLowEdge(1) << " -- "
                           << hist->GetXaxis()->GetBinUpEdge(
                                  hist->GetXaxis()->GetNbins()));
          }

          if (!ybin || ((hist->GetYaxis()->GetNbins() + 1) == ybin)) {
            NUIS_ERR(WRN, "Tried to apply effiency but XBin: "
                           << ybin << " is outside range (/"
                           << hist->GetYaxis()->GetNbins() << "): Prop "
                           << kineProps[0] << ", ["
                           << hist->GetYaxis()->GetBinLowEdge(1) << " -- "
                           << hist->GetYaxis()->GetBinUpEdge(
                                  hist->GetYaxis()->GetNbins()));
          }

          if (!zbin || ((hist->GetZaxis()->GetNbins() + 1) == zbin)) {
            NUIS_ERR(WRN, "Tried to apply effiency but ZBin: "
                           << zbin << " is outside range (/"
                           << hist->GetZaxis()->GetNbins() << "): Prop "
                           << kineProps[0] << ", ["
                           << hist->GetZaxis()->GetBinLowEdge(1) << " -- "
                           << hist->GetZaxis()->GetBinUpEdge(
                                  hist->GetZaxis()->GetNbins()));
          }

          effProb = hist->GetBinContent(xbin, ybin, zbin);
        }
        break;
      }
    }

    bool accepted = (rand.Uniform() < effProb);

    if (accepted) {
#ifdef DEBUG_EFFAPP
      std::cout << " -- Reconstructed with probability: " << effProb
                << std::flush;
#endif
      ri->RecObjMom.push_back(fp->P3());
      ri->RecObjClass.push_back(fp->PDG());

      continue;
    }

#ifdef DEBUG_EFFAPP
    std::cout << " -- Rejected with probability: " << effProb << std::flush;
#endif

    SlaveTA.SmearceptOneParticle(ri, fp
#ifdef DEBUG_THRESACCEPT
                                 ,
                                 p_it
#endif
                                 );
  }
#ifdef DEBUG_EFFAPP
  std::cout << std::endl;
#endif

#ifdef DEBUG_EFFAPP
  std::cout << "Reconstructed " << ri->RecObjMom.size() << " particles. "
            << std::endl;
#endif
  return ri;
}
