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

#include "TrackedMomentumMatrixSmearer.h"

namespace {
TrackedMomentumMatrixSmearer::DependVar GetVarType(std::string const &axisvar) {
  if (axisvar == "Momentum") {
    return TrackedMomentumMatrixSmearer::kMomentum;
  } else if (axisvar == "KE") {
    return TrackedMomentumMatrixSmearer::kKE;
  } else if (axisvar == "TE") {
    return TrackedMomentumMatrixSmearer::kTE;
  }
  return TrackedMomentumMatrixSmearer::kNoVar;
}
}

TH1D const *TrackedMomentumMatrixSmearer::SmearMap::GetRecoSlice(double val) {
  if ((val < RecoSlices.front().first.first) ||
      (val > RecoSlices.back().first.second)) {
    ERROR(WRN,
          "Kinematic property: " << val << ", not within smearable range: ["
                                 << RecoSlices.front().first.first << " -- "
                                 << RecoSlices.back().first.second << "].");
    return NULL;
  }

  int L = 0, U = RecoSlices.size();

  while (true) {
    if (U == L) {
      return RecoSlices[L].second;
    }
    int R = (U - L);
    int m = L + (R / 2);

    if (val <= RecoSlices[m].first.first) {
      U = m - 1;
      continue;
    }
    if (val > RecoSlices[m].first.second) {
      L = m + 1;
      continue;
    }
    if ((val > RecoSlices[m].first.first) &&
        (val <= RecoSlices[m].first.second)) {
      return RecoSlices[m].second;
    }
    THROW("Binary smearing search failed. Check logic.");
  }
}

TH1D *GetMapSlice(TH2D *mp, int SliceBin, bool AlongX) {
  int NBins = (AlongX ? mp->GetXaxis() : mp->GetYaxis())->GetNbins();
  int NOtherBins = (AlongX ? mp->GetYaxis() : mp->GetXaxis())->GetNbins();
  if (SliceBin >= NOtherBins) {
    THROW("Asked for slice " << SliceBin << " but the " << (AlongX ? 'Y' : 'X')
                             << " axis only has " << NOtherBins);
  }

  if ((AlongX ? mp->GetXaxis() : mp->GetYaxis())->IsVariableBinSize() &&
      ((NBins + 1) !=
       (AlongX ? mp->GetXaxis() : mp->GetYaxis())->GetXbins()->GetSize())) {
    THROW(
        "Attemping to take binning slice of variable binning, but NBins+1 != "
        "NBinEdges: "
        << (NBins + 1) << " != "
        << (AlongX ? mp->GetXaxis() : mp->GetYaxis())->GetXbins()->GetSize());
  }

  std::stringstream ss("");
  ss << mp->GetName() << (AlongX ? 'Y' : 'X') << "Slice_" << SliceBin;
  std::stringstream st("");
  st << mp->GetTitle() << ";"
     << (AlongX ? mp->GetXaxis() : mp->GetYaxis())->GetTitle() << ";"
     << "Count";
  TH1D *Ret;
  if ((AlongX ? mp->GetXaxis() : mp->GetYaxis())->IsVariableBinSize()) {
    Ret = new TH1D(
        ss.str().c_str(), st.str().c_str(), NBins,
        (AlongX ? mp->GetXaxis() : mp->GetYaxis())->GetXbins()->GetArray());
  } else {
    Ret = new TH1D(ss.str().c_str(), st.str().c_str(), NBins,
                   (AlongX ? mp->GetXaxis() : mp->GetYaxis())->GetXmin(),
                   (AlongX ? mp->GetXaxis() : mp->GetYaxis())->GetXmax());
  }

  for (int bi_it = 0; bi_it < NBins + 2; ++bi_it) {
    int X = AlongX ? bi_it : SliceBin + 1;
    int Y = AlongX ? SliceBin + 1 : bi_it;
    int GBin = mp->GetBin(X, Y);
    Ret->SetBinContent(bi_it, mp->GetBinContent(GBin));
    Ret->SetBinError(bi_it, mp->GetBinError(GBin));
  }

#ifdef DEBUG_MATSMEAR
  std::cout << "Took slice: " << SliceBin << " spanning ["
            << Ret->GetXaxis()->GetBinLowEdge(1) << " -- "
            << Ret->GetXaxis()->GetBinUpEdge(Ret->GetXaxis()->GetNbins())
            << "] (Orignal span ["
            << (AlongX ? mp->GetXaxis() : mp->GetYaxis())->GetBinLowEdge(1)
            << " -- "
            << (AlongX ? mp->GetXaxis() : mp->GetYaxis())
                   ->GetBinUpEdge(
                       (AlongX ? mp->GetXaxis() : mp->GetYaxis())->GetNbins())
            << "]) " << (AlongX ? mp->GetXaxis() : mp->GetYaxis())->GetXmin()
            << " -- " << (AlongX ? mp->GetXaxis() : mp->GetYaxis())->GetXmax()
            << " IsVBin: "
            << (AlongX ? mp->GetXaxis() : mp->GetYaxis())->IsVariableBinSize()
            << std::endl;
#endif

  Ret->SetDirectory(NULL);
  return Ret;
}

void TrackedMomentumMatrixSmearer::SmearMap::SetSlicesFromMap(TH2D *map,
                                                              bool TruthIsY) {
  int NSlices = (TruthIsY ? map->GetYaxis() : map->GetXaxis())->GetNbins();

  for (Int_t TrueSlice_it = 0; TrueSlice_it < NSlices; ++TrueSlice_it) {
    std::pair<double, double> BinEdges;
    BinEdges.first = (TruthIsY ? map->GetYaxis() : map->GetXaxis())
                         ->GetBinLowEdge(TrueSlice_it + 1);
    BinEdges.second = (TruthIsY ? map->GetYaxis() : map->GetXaxis())
                          ->GetBinUpEdge(TrueSlice_it + 1);

    TH1D *slice = GetMapSlice(map, TrueSlice_it, TruthIsY);

    RecoSlices.push_back(std::make_pair(BinEdges, slice));
  }
  QLOG(FIT, "\tAdded " << RecoSlices.size() << " reco slices.");
}

/// Reads particle efficiency nodes
///
/// Nodes look like:
/// <TrackedMomentumMatrixSmearer Name="D00N_ND_LAr">
///   <SmearMatrix PDG="211,-211" InputFile="smear.root"
///   HistName="cpion_mom_smear" Kinematics="[KE|Momentum]"
///   MatrixToInternal="1E3" YIsTrue="true"/>
/// </TrackedMomentumMatrixSmearer>
void TrackedMomentumMatrixSmearer::SpecifcSetup(nuiskey &nk) {
  std::vector<nuiskey> effDescriptors = nk.GetListOfChildNodes("SmearMatrix");

  for (size_t t_it = 0; t_it < effDescriptors.size(); ++t_it) {
    std::string inputFileName = effDescriptors[t_it].GetS("InputFile");
    std::string HistName = effDescriptors[t_it].GetS("HistName");
    bool YIsTrue = effDescriptors[t_it].Has("YIsTrue")
                       ? effDescriptors[t_it].GetI("YIsTrue")
                       : true;

    double UnitsScale = effDescriptors[t_it].Has("MatrixToInternal")
                            ? effDescriptors[t_it].GetD("MatrixToInternal")
                            : 1;

    TFile inputFile(inputFileName.c_str());
    if (!inputFile.IsOpen()) {
      THROW("Couldn't open specified input root file: " << inputFileName);
    }

    TH2D *inpHist = dynamic_cast<TH2D *>(inputFile.Get(HistName.c_str()));
    if (!inpHist) {
      THROW("Couldn't get TH2D named: " << HistName << " from input root file: "
                                        << inputFileName);
    }

    TrackedMomentumMatrixSmearer::DependVar var =
        GetVarType(effDescriptors[t_it].GetS("Kinematics"));

    std::string pdgs_s = effDescriptors[t_it].GetS("PDG");
    std::vector<int> pdgs_i = GeneralUtils::ParseToInt(pdgs_s, ",");
    for (size_t pdg_it = 0; pdg_it < pdgs_i.size(); ++pdg_it) {
      if (ParticleMappings.count(pdgs_i[pdg_it])) {
        ERROR(WRN, "Smearceptor " << ElementName << ":" << InstanceName
                                  << " already has a smearing for PDG: "
                                  << pdgs_i[pdg_it]);
      }

      SmearMap sm;
      sm.SetSlicesFromMap(inpHist, YIsTrue);
      sm.SmearVar = var;
      sm.UnitsScale = UnitsScale;

      ParticleMappings[pdgs_i[pdg_it]] = sm;

      QLOG(FIT, "Added smearing map for PDG: " << pdgs_i[pdg_it]);
    }
  }
  SlaveGS.Setup(nk);
}

RecoInfo *TrackedMomentumMatrixSmearer::Smearcept(FitEvent *fe) {
  RecoInfo *ri = new RecoInfo();

  for (size_t p_it = 0; p_it < fe->NParticles(); ++p_it) {
    FitParticle *fp = fe->GetParticle(p_it);
#ifdef DEBUG_MATSMEAR
    std::cout << std::endl;
    std::cout << "[" << p_it << "]: " << fp->PDG() << ", " << fp->Status()
              << ", " << fp->E() << " -- KE:" << fp->KE()
              << " Mom: " << fp->P3().Mag() << std::flush;
#endif

    if (fp->Status() != kFinalState) {
#ifdef DEBUG_MATSMEAR
      std::cout << " -- Not final state." << std::flush;
#endif
      continue;
    }

    if (!ParticleMappings.count(fp->PDG())) {
      SlaveGS.SmearceptOneParticle(ri, fp
#ifdef DEBUG_GAUSSSMEAR
                                   ,
                                   p_it
#endif
                                   );
      continue;
    }

    SmearMap &sm = ParticleMappings[fp->PDG()];
    double kineProp = 0;

    switch (sm.SmearVar) {
      case kMomentum: {
        kineProp = fp->P3().Mag();
        break;
      }
      case kKE: {
        kineProp = fp->KE();
        break;
      }
      case kTE: {
        kineProp = fp->E();
        break;
      }
      default: { THROW("Trying to find particle value for a kNoAxis."); }
    }

    TH1 const *recoDistrib = sm.GetRecoSlice(kineProp / sm.UnitsScale);

#ifdef DEBUG_MATSMEAR
    std::cout << " -- Got slice spanning ["
              << recoDistrib->GetXaxis()->GetBinLowEdge(1) << " -- "
              << recoDistrib->GetXaxis()->GetBinUpEdge(
                     recoDistrib->GetXaxis()->GetNbins())
              << "]" << std::endl;
#endif

    if (!recoDistrib) {
#ifdef DEBUG_MATSMEAR
      std::cout << " -- outside smearable range." << std::flush;
#endif
      continue;
    }

    if (recoDistrib->Integral() == 0) {
      ERROR(WRN, "True slice has no reconstructed events. Not smearing.")
      continue;
    }

    double Smeared = recoDistrib->GetRandom() * sm.UnitsScale;
#ifdef DEBUG_MATSMEAR
    std::cout << "GotRandom: " << Smeared << ", MPV: "
              << recoDistrib->GetXaxis()->GetBinCenter(
                     recoDistrib->GetMaximumBin()) *
                     sm.UnitsScale
              << std::endl;
#endif

    switch (sm.SmearVar) {
      case kMomentum: {
        ri->RecObjMom.push_back(fp->P3().Unit() * Smeared);

#ifdef DEBUG_MATSMEAR
        std::cout << " -- Smeared:  " << fp->p() << " -> " << Smeared << "."
                  << std::flush;
#endif

        break;
      }
      case kKE: {
        double mass = fp->P4().M();
        double TE = mass + Smeared;
        double magP = sqrt(TE * TE - mass * mass);

        ri->RecObjMom.push_back(fp->P3().Unit() * magP);

#ifdef DEBUG_MATSMEAR
        std::cout << " -- Smeared:  " << fp->KE() << " (mass: " << mass
                  << ") -> " << Smeared
                  << ". Smear Mom: " << ri->RecObjMom.back().Mag() << "."
                  << std::flush;
#endif
        break;
      }
      case kTE: {
        double mass = fp->P4().M();
        double TE = Smeared;
        double magP = sqrt(TE * TE - mass * mass);

        ri->RecObjMom.push_back(fp->P3().Unit() * magP);

#ifdef DEBUG_MATSMEAR
        std::cout << " -- Smeared:  " << fp->E() << " (mass: " << mass
                  << ") -> " << Smeared
                  << ". Smear Mom: " << ri->RecObjMom.back().Mag() << "."
                  << std::flush;
#endif
        break;
      }
      default: {}
    }
#ifdef DEBUG_MATSMEAR
    std::cout << " -- momentum reconstructed as " << ri->RecObjMom.back().Mag()
              << "." << std::endl;
#endif
    if (ri->RecObjMom.back().Mag() != ri->RecObjMom.back().Mag()) {
      ERROR(WRN, "Invalid particle built.");
      ri->RecObjMom.pop_back();

#include "TCanvas.h"
      TCanvas *Test = new TCanvas("c1", "");
      static_cast<TH1 *>(recoDistrib->Clone())->Draw();
      Test->SaveAs("Fail.png");
      delete Test;
      THROW("ARGH");
    } else {
      ri->RecObjClass.push_back(fp->PDG());
    }
  }
#ifdef DEBUG_MATSMEAR
  std::cout << std::endl;
#endif
  return ri;
}

void TrackedMomentumMatrixSmearer::SmearRecoInfo(RecoInfo *ri) {
  for (size_t p_it = 0; p_it < ri->RecObjMom.size(); ++p_it) {
    if (!ParticleMappings.count(ri->RecObjClass[p_it])) {
      SlaveGS.SmearceptOneParticle(ri->RecObjMom[p_it], ri->RecObjClass[p_it]);
      continue;
    }
    SmearMap &sm = ParticleMappings[ri->RecObjClass[p_it]];
    double kineProp = 0;

    switch (sm.SmearVar) {
      case kMomentum: {
        kineProp = ri->RecObjMom[p_it].Mag();
        break;
      }
      case kKE: {
        double mass = PhysConst::GetMass(ri->RecObjClass[p_it]) * 1E3;
        kineProp = sqrt(ri->RecObjMom[p_it].Mag2() + mass * mass) - mass;
        break;
      }
      case kTE: {
        double mass = PhysConst::GetMass(ri->RecObjClass[p_it]) * 1E3;
        kineProp = sqrt(ri->RecObjMom[p_it].Mag2() + mass * mass);
        break;
      }
      default: { THROW("Trying to find particle value for a kNoAxis."); }
    }
    TH1 const *recoDistrib = sm.GetRecoSlice(kineProp / sm.UnitsScale);
    if (!recoDistrib) {
      continue;
    }

    double Smeared = recoDistrib->GetRandom() * sm.UnitsScale;

    switch (sm.SmearVar) {
      case kMomentum: {
        ri->RecObjMom[p_it] = ri->RecObjMom[p_it].Unit() * Smeared;
        break;
      }
      case kKE: {
        double mass = PhysConst::GetMass(ri->RecObjClass[p_it]) * 1E3;
        double TE = mass + Smeared;
        double magP = sqrt(TE * TE - mass * mass);
        ri->RecObjMom[p_it] = ri->RecObjMom[p_it].Unit() * magP;
        break;
      }
      case kTE: {
        double mass = PhysConst::GetMass(ri->RecObjClass[p_it]) * 1E3;
        double TE = Smeared;
        double magP = sqrt(TE * TE - mass * mass);
        ri->RecObjMom[p_it] = ri->RecObjMom[p_it].Unit() * magP;
        break;
      }
      default: {}
    }
  }
}
