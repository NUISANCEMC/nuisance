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

#include "SigmaEnuHists.h"

#include <cmath>

void PerEify(TH1 *h) {
  for (int i = 0; i < h->GetXaxis()->GetNbins(); ++i) {
    double cont = h->GetBinContent(i + 1);
    double err = h->GetBinError(i + 1);
    double e = h->GetXaxis()->GetBinCenter(i + 1);
    if (!e) {
      h->SetBinContent(i + 1, 0);
      h->SetBinError(i + 1, 0);
    } else {
      h->SetBinContent(i + 1, cont / e);
      h->SetBinError(i + 1, err / e);
    }
  }
}

SigmaEnuHists::SigmaEnuHists(nuiskey samplekey) {

  fSettings = LoadSampleSettings(samplekey);

  PerE = (fName.find("PerE") != std::string::npos);

  // Define our energy range for flux calcs
  EnuMin = 0.;
  EnuMax = 1E10; // Arbritrarily high energy limit

  // Setup fDataHist as a placeholder
  fDataHist = new TH1D(("empty_data"), ("empty-data"), 1, 0, 1);

  FinaliseSampleSettings();

  // 1. The generator is organised in SetupMeasurement so it gives the
  // cross-section in "per nucleon" units.
  //    So some extra scaling for a specific measurement may be required. For
  //    Example to get a "per neutron" measurement on carbon
  //    which we do here, we have to multiple by the number of nucleons 12 and
  //    divide by the number of neutrons 6.
  // N.B. MeasurementBase::PredictedEventRate includes the 1E-38 factor that is
  // often included here in other classes that directly integrate the event
  // histogram. This method is used here as it now respects EnuMin and EnuMax
  // correctly.
  fScaleFactor =
      GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents);

  NUIS_LOG(SAM, " Generic Flux Scaling Factor = "
                    << fScaleFactor << " [= "
                    << (GetEventHistogram()->Integral("width") * 1E-38) << "/("
                    << (fNEvents + 0.) << "*" << TotalIntegratedFlux("width")
                    << ")]");

  if (fScaleFactor <= 0.0) {
    NUIS_ABORT("SCALE FACTOR TOO LOW");
  }

  // If we have binning
  if (samplekey.Has("NBins") && samplekey.Has("MinEnuGev") &&
      samplekey.Has("MaxEnuGev")) {
    int nbins = samplekey.GetI("NBins");
    double low_gev = samplekey.GetD("MinEnuGev");
    double up_gev = samplekey.GetD("MaxEnuGev");

    std::vector<double> bins;
    bool LogE = samplekey.Has("UseLogE") && samplekey.GetB("UseLogE");

    double step = (LogE ? (std::log10(up_gev) - std::log10(low_gev)) : (up_gev - low_gev)) /
                  double(nbins);

    bins.push_back(low_gev);

    for (int i = 0; i < nbins; ++i) {
      if (LogE) {
        bins.push_back(pow(10, std::log10(bins.back()) + step));
      } else {
        bins.push_back(bins.back() + step);
      }
    }

    BinningHist = new TH1D("BinningHist", "", bins.size() - 1, bins.data());

  } else { // use the flux hist
    BinningHist = static_cast<TH1D *>(fFluxHist->Clone("BinningHist"));
  }
  BinningHist->SetDirectory(NULL);

  TopologyNames[kCC] = "CCInc";
  TopologyNames[kCC0Pi] = "CC0Pi";
  TopologyNames[kCC1Pi] = "CC1Pi";
  TopologyNames[kCC1Pip] = "CC1Pip";
  TopologyNames[kCC1Pi0] = "CC1Pi0";
  TopologyNames[kCC1Pim] = "CC1Pim";
  TopologyNames[kCCNPi] = "CCNPi";
  TopologyNames[kNC] = "NCInc";
  TopologyNames[kNC0Pi] = "NC0Pi";
  TopologyNames[kNC1Pi] = "NC1Pi";
  TopologyNames[kNCNPi] = "NCNPi";

  for (int t = kCC; t < kNTopologies; ++t) {
    TopologyHists[t] =
        static_cast<TH1D *>(BinningHist->Clone(TopologyNames[t].c_str()));
    TopologyHists[t]->SetTitle(
        ";#it{E}_{#nu} (GeV); #sigma(#it{E_{#nu}}) 10^{-38} cm^{2} /nucleon");
    TopologyHists[t]->Reset();
  }

  NEUTModeHists[0] = static_cast<TH1D *>(BinningHist->Clone("TotalXSec"));
  NEUTModeHists[0]->SetTitle(
      ";#it{E}_{#nu} (GeV); #sigma(#it{E_{#nu}}) 10^{-38} cm^{2} /nucleon");
  NEUTModeHists[0]->Reset();

  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
}

void SigmaEnuHists::FillEventVariables(FitEvent *event) {

  // Now fill the information
  if (!NEUTModeHists.count(event->Mode)) {
    std::stringstream ss;
    ss << "NeutMode_" << (event->Mode < 0 ? "m" : "") << abs(event->Mode);
    NEUTModeHists[event->Mode] =
        static_cast<TH1D *>(BinningHist->Clone(ss.str().c_str()));
    NEUTModeHists[event->Mode]->SetTitle(
        ";#it{E}_{#nu} (GeV); #sigma(#it{E_{#nu}}) 10^{-38} cm^{2} /nucleon");
    NEUTModeHists[event->Mode]->Reset();
  }

  FitParticle *nu = event->GetBeamPart();

  double enu_gev = nu->fP.E() * 1E-3;
  double w = event->Weight;

  NEUTModeHists[event->Mode]->Fill(enu_gev, w);
  NEUTModeHists[0]->Fill(enu_gev, w);

#ifdef __GENIE_ENABLED__
  if (event->fType == kGENIE) {
    EventRecord *gevent = static_cast<EventRecord *>(event->genie_event->event);
    const Interaction *interaction = gevent->Summary();
    int gmode = interaction->ProcInfo().ScatteringTypeId();
    int isNC = !interaction->ProcInfo().IsWeakCC();
    int isnu = nu->fPID > 0;
    int nuis_gmode = (gmode + 30 * isNC) * (isnu ? 1 : -1);
    if (!GENIEModeHists.count(nuis_gmode)) {
      std::stringstream ss;
      ss << "GENIEMode_" << (isNC ? "NC_" : "CC_") << (isnu ? "nu_" : "nubar_")
         << interaction->ProcInfo().ScatteringTypeAsString();
      GENIEModeHists[nuis_gmode] =
          static_cast<TH1D *>(BinningHist->Clone(ss.str().c_str()));
      GENIEModeHists[nuis_gmode]->SetTitle(
          ";#it{E}_{#nu} (GeV); #sigma(#it{E_{#nu}}) 10^{-38} cm^{2} /nucleon");
      GENIEModeHists[nuis_gmode]->Reset();
    }
    GENIEModeHists[nuis_gmode]->Fill(enu_gev, w);
  }
#endif

  int NPi = event->GetAllFSPionsIndices().size();
  int NPip = event->GetAllFSPiPlusIndices().size();
  int NPim = event->GetAllFSPiMinusIndices().size();

  if (event->IsCC()) {
    TopologyHists[kCC]->Fill(enu_gev, w);
    if (NPi == 0) {
      TopologyHists[kCC0Pi]->Fill(enu_gev, w);
    } else if (NPi == 1) {
      TopologyHists[kCC1Pi]->Fill(enu_gev, w);
      if (NPip == 1) {
        TopologyHists[kCC1Pip]->Fill(enu_gev, w);
      } else if (NPim == 1) {
        TopologyHists[kCC1Pim]->Fill(enu_gev, w);
      } else {
        TopologyHists[kCC1Pi0]->Fill(enu_gev, w);
      }
    } else {
      TopologyHists[kCCNPi]->Fill(enu_gev, w);
    }
  } else {
    TopologyHists[kNC]->Fill(enu_gev, w);
    if (NPi == 0) {
      TopologyHists[kNC0Pi]->Fill(enu_gev, w);
    } else if (NPi == 1) {
      TopologyHists[kNC1Pi]->Fill(enu_gev, w);
    } else {
      TopologyHists[kNCNPi]->Fill(enu_gev, w);
    }
  }
};

void SigmaEnuHists::Write(std::string drawOpt) {
  for (std::map<int, TH1D *>::iterator h = NEUTModeHists.begin();
       h != NEUTModeHists.end(); ++h) {
    PlotUtils::FluxUnfoldedScaling(h->second, GetFluxHistogram(),
                                   GetEventHistogram(), fScaleFactor, fNEvents);
    if (PerE) {
      PerEify(h->second);
    }
    h->second->Write();
  }
  for (std::map<int, TH1D *>::iterator h = GENIEModeHists.begin();
       h != GENIEModeHists.end(); ++h) {
    PlotUtils::FluxUnfoldedScaling(h->second, GetFluxHistogram(),
                                   GetEventHistogram(), fScaleFactor, fNEvents);
    if (PerE) {
      PerEify(h->second);
    }
    h->second->Write();
  }
  for (std::map<int, TH1D *>::iterator h = TopologyHists.begin();
       h != TopologyHists.end(); ++h) {
    PlotUtils::FluxUnfoldedScaling(h->second, GetFluxHistogram(),
                                   GetEventHistogram(), fScaleFactor, fNEvents);
    if (PerE) {
      PerEify(h->second);
    }
    h->second->Write();
  }
}

// Override functions which aren't really necessary
bool SigmaEnuHists::isSignal(FitEvent *event) {
  (void)event;
  return true;
};
