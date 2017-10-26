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

#include "ElectronScattering_DurhamData.h"

//********************************************************************
ElectronScattering_DurhamData::ElectronScattering_DurhamData(
    nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip =
      "Electron Scattering Durham Data sample. \n"
      "Target: Multiple \n"
      "Flux: Energy should match data being handled \n"
      "Signal: Any event with an electron in the final state \n";
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.DefineAllowedSpecies("electron");
  fSettings.SetTitle("Electron");
  fSettings.SetAllowedTypes("FIX/DIAG", "FIX,FREE,SHAPE/DIAG/NORM/MASK");
  fSettings.SetXTitle("q0");
  fSettings.SetYTitle("#sigma");
  fIsNoWidth = true;

  FinaliseSampleSettings();

  // Plot Setup -------------------------------------------------------
  SetDataFromName(fSettings.GetS("originalname"));
  SetCovarFromDiagonal();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  // fScaleFactor = ((GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents
  // + 0.)) / TotalIntegratedFlux());
  EnuMin = fZLowLim;
  EnuMax = fZHighLim;

  double sigscale = GetEventHistogram()->Integral() * 1E-38 / double(fNEvents) /
                    TotalIntegratedFlux();
  //  double dangle = 2 * M_PI * fabs((1. - cos(fYLowLim * M_PI / 180.)) - (1. -
  //  cos(fYHighLim * M_PI / 180.)));
  //  fScaleFactor = sigscale / dangle / fZCenter;
  fScaleFactor = sigscale;

  std::cout << "Event Integral = " << GetEventHistogram()->Integral()
            << std::endl;
  std::cout << "Flux Integral = " << TotalIntegratedFlux() << std::endl;
  std::cout << "FNEvents = " << fNEvents << std::endl;
  std::cout << "Z Limits = " << fZLowLim << " " << fZHighLim << std::endl;
  std::cout << "sigscale = " << sigscale << std::endl;
  std::cout << "fZCenter = " << fZCenter << std::endl;
  std::cout << "ScaleFactor = " << fScaleFactor << std::endl;

  // Finish up
  FinaliseMeasurement();
};

//********************************************************************
void ElectronScattering_DurhamData::SetDataFromName(std::string name) {
  //********************************************************************

  // Data Should be given in the format
  // Electron_Z_A_Energy_Theta_Source
  std::vector<std::string> splitstring = GeneralUtils::ParseToStr(name, "_");
  std::string zstring = splitstring[1];
  std::string astring = splitstring[2];
  std::string estring = splitstring[3];
  std::string tstring = splitstring[4];
  std::string sstring = splitstring[5];

  fYCenter = GeneralUtils::StrToDbl(tstring);
  fZCenter = GeneralUtils::StrToDbl(estring);

  // Create effective E and Theta bin Edges
  std::vector<double> thetabinedges;
  std::vector<double> ebinedges;

  int nthetabins = FitPar::Config().GetParI("Electron_NThetaBins");
  int nebins = FitPar::Config().GetParI("Electron_NEnergyBins");
  double thetawidth = FitPar::Config().GetParD("Electron_ThetaWidth");
  double ewidth = FitPar::Config().GetParD("Electron_EnergyWidth");

  for (int i = -nthetabins; i <= nthetabins; i++) {
    thetabinedges.push_back(fYCenter + thetawidth * (double(i)));
  }
  for (int i = -nebins; i <= nebins; i++) {
    double newval = fZCenter + ewidth * (double(i));

    if (newval < 0.0) newval = 0.0;
    if (newval < GetEventHistogram()->GetXaxis()->GetXmin())
      newval = GetEventHistogram()->GetXaxis()->GetXmin();
    if (newval > GetEventHistogram()->GetXaxis()->GetXmax())
      newval = GetEventHistogram()->GetXaxis()->GetXmax();
    if (std::find(ebinedges.begin(), ebinedges.end(), newval) !=
        ebinedges.end())
      continue;
    ebinedges.push_back(newval);
  }

  // Determine target
  std::string target = "";
  if (!zstring.compare("6") && !astring.compare("12"))
    target = "12C.dat";
  else if (!zstring.compare("8") && !astring.compare("16"))
    target = "16O.dat";
  else {
    ERR(FTL) << "Target not supported in electron scattering module!"
             << std::endl;
    throw;
  }

  // Fill Data Points

  std::string line;
  std::ifstream mask((FitPar::GetDataBase() + "/Electron/" + target).c_str(),
                     ifstream::in);

  if (!mask.good()) {
    ERR(FTL) << "Failed to open e-scattering database file: "
             << (FitPar::GetDataBase() + "/Electron/" + target) << std::endl;
    throw;
  }
  int i = 0;

  std::vector<double> pointx;
  std::vector<double> errorx;
  std::vector<double> pointy;
  std::vector<double> errory;
  double scalef = 1.E-38 * 1.E5;

  while (std::getline(mask >> std::ws, line, '\n')) {
    // std::cout << "Line = " << line << std::endl;
    if (line.empty()) continue;

    std::vector<std::string> lineentries = GeneralUtils::ParseToStr(line, " ");
    // std::cout << "Checking : " << line << std::endl;
    if (zstring.compare(lineentries[0])) continue;
    if (astring.compare(lineentries[1])) continue;
    if (estring.compare(lineentries[2])) continue;
    if (tstring.compare(lineentries[3])) continue;
    if (sstring.compare(lineentries[7])) continue;

    // std::cout << "Registering data point : " << line << std::endl;
    // std::cout << "Adding Graph Point : " <<
    // GeneralUtils::StrToDbl(lineentries[4]) << " " <<
    // GeneralUtils::StrToDbl(lineentries[5]) << std::endl;

    // Loop through x and y points and find a place to insert
    if (pointx.empty()) {
      pointx.push_back(GeneralUtils::StrToDbl(lineentries[4]));
      errorx.push_back(0.0);
      pointy.push_back(GeneralUtils::StrToDbl(lineentries[5]) * scalef);
      errory.push_back(GeneralUtils::StrToDbl(lineentries[6]) * scalef);

    } else {
      for (size_t j = 0; j < pointx.size(); j++) {
        if (GeneralUtils::StrToDbl(lineentries[4]) < pointx[j] && j == 0) {
          // std::cout << "Inserting at start point iterator " << std::endl;
          pointx.insert(pointx.begin() + j,
                        GeneralUtils::StrToDbl(lineentries[4]));
          errorx.insert(errorx.begin() + j, 0.0);
          pointy.insert(pointy.begin() + j,
                        GeneralUtils::StrToDbl(lineentries[5]) * scalef);
          errory.insert(errory.begin() + j,
                        GeneralUtils::StrToDbl(lineentries[6]) * scalef);
          break;

        } else if (GeneralUtils::StrToDbl(lineentries[4]) > pointx[j] &&
                   j == pointx.size() - 1) {
          // std::cout << "Pushing back data point " << std::endl;
          pointx.push_back(GeneralUtils::StrToDbl(lineentries[4]));
          errorx.push_back(0.0);
          pointy.push_back(GeneralUtils::StrToDbl(lineentries[5]) * scalef);
          errory.push_back(GeneralUtils::StrToDbl(lineentries[6]) * scalef);
          break;

        } else if (GeneralUtils::StrToDbl(lineentries[4]) > pointx[j - 1] &&
                   GeneralUtils::StrToDbl(lineentries[4]) < pointx[j]) {
          // std::cout << "Inserting at point iterator = " << j << std::endl;

          pointx.insert(pointx.begin() + j,
                        GeneralUtils::StrToDbl(lineentries[4]));
          errorx.insert(errorx.begin() + j, 0.0);
          pointy.insert(pointy.begin() + j,
                        GeneralUtils::StrToDbl(lineentries[5]) * scalef);
          errory.insert(errory.begin() + j,
                        GeneralUtils::StrToDbl(lineentries[6]) * scalef);
          break;
        }
      }
    }

    // pointx.push_back(GeneralUtils::StrToDbl(lineentries[4]));
    // errorx.push_back(0.0);
    // pointy.push_back(GeneralUtils::StrToDbl(lineentries[5]));
    // errory.push_back(GeneralUtils::StrToDbl(lineentries[6]));

    i++;
  }

  if (!pointx.size() || (pointx.size() != errorx.size()) || !pointy.size() ||
      (pointy.size() != errory.size())) {
    ERR(FTL) << "Failed to find dataset: " << name << "{"
             << "Z: " << zstring << ", A: " << astring << ", E: " << estring
             << ", CTheta: " << tstring << ", PubID: " << sstring
             << " } in file: "
             << (FitPar::GetDataBase() + "/Electron/" + target) << std::endl;
    throw;
  }

  // for (uint i  = 0; i < pointx.size(); i++) {
  //   std::cout << "Q0 Point " << i << " = " << pointx[i] << std::endl;
  // }

  fDataGraph = new TGraphErrors(pointx.size(), &pointx[0], &pointy[0],
                                &errorx[0], &errory[0]);
  fDataGraph->SetNameTitle((fName + "_data_GRAPH").c_str(),
                           (fName + "_data_GRAPH").c_str());

  // Now form an effective data and mc histogram
  std::vector<double> q0binedges;
  const double* x = fDataGraph->GetX();

  // Loop over graph and get mid way point between each data point.
  for (int i = 0; i < fDataGraph->GetN(); i++) {
    // std::cout << "X Point = " << x[i] << std::endl;

    if (i == 0) {
      // First point set lower width as half distance to point above
      q0binedges.push_back(x[0] - ((x[1] - x[0]) / 2.0));
    } else if (i == fDataGraph->GetN() - 1) {
      // Last point set upper width as half distance to point above.
      q0binedges.push_back(x[i] - ((x[i] - x[i - 1]) / 2.0));
      q0binedges.push_back(x[i] + ((x[i] - x[i - 1]) / 2.0));
    } else {
      // Set half distance to point below
      q0binedges.push_back(x[i] - ((x[i] - x[i - 1]) / 2.0));
    }
  }

  // Bubble Sort

  // for (uint i  = 0; i < q0binedges.size(); i++) {
  //   std::cout << "Q0 Edge " << i << " = " << q0binedges[i] << std::endl;
  // }

  // for (uint i  = 0; i < ebinedges.size(); i++) {
  //   std::cout << "e Edge " << i << " = " << ebinedges[i] << std::endl;
  // }
  // for (uint i  = 0; i < thetabinedges.size(); i++) {
  //   std::cout << "theta Edge " << i << " = " << thetabinedges[i] <<
  //   std::endl;
  // }

  // Form the data hist, mchist, etc
  fDataHist = new TH1D((fName + "_data").c_str(), (fName + "_data").c_str(),
                       q0binedges.size() - 1, &q0binedges[0]);
  fMCHist = (TH1D*)fDataHist->Clone("MC");

  const double* y = fDataGraph->GetY();
  const double* ey = fDataGraph->GetEY();
  for (int i = 0; i < fDataGraph->GetN(); i++) {
    // std::cout << "Setting Data Bin " << i + 1 << " to " << y[i] << " +- " <<
    // ey[i] << std::endl;
    fDataHist->SetBinContent(i + 1, y[i]);
    fDataHist->SetBinError(i + 1, ey[i]);

    fMCHist->SetBinContent(i + 1, 0.0);
    fMCHist->SetBinError(i + 1, 0.0);
  }

  fMCScan_Q0vsThetavsE =
      new TH3D((fName + "_MC_q0vsthetavse").c_str(), "MC_q0vsthetavse",
               q0binedges.size() - 1, &q0binedges[0], thetabinedges.size() - 1,
               &thetabinedges[0], ebinedges.size() - 1, &ebinedges[0]);
  fMCScan_Q0vsThetavsE->Reset();
  fMCScan_Q0vsTheta = new TH2D(
      (fName + "_MC_q0vstheta").c_str(), "MC_q0vstheta", q0binedges.size() - 1,
      &q0binedges[0], thetabinedges.size() - 1, &thetabinedges[0]);
  fMCScan_Q0vsTheta->Reset();

  fMCScan_Q0vsE =
      new TH2D((fName + "_MC_q0vse").c_str(), "MC_q0vse", q0binedges.size() - 1,
               &q0binedges[0], ebinedges.size() - 1, &ebinedges[0]);
  fMCScan_Q0vsE->Reset();

  fXLowLim = fMCScan_Q0vsThetavsE->GetXaxis()->GetBinLowEdge(1);
  fXHighLim = fMCScan_Q0vsThetavsE->GetXaxis()->GetBinLowEdge(
      fMCScan_Q0vsThetavsE->GetNbinsX() + 2);

  fYLowLim = fMCScan_Q0vsThetavsE->GetYaxis()->GetBinLowEdge(1);
  fYHighLim = fMCScan_Q0vsThetavsE->GetYaxis()->GetBinLowEdge(
      fMCScan_Q0vsThetavsE->GetNbinsY() + 2);

  fZLowLim = fMCScan_Q0vsThetavsE->GetZaxis()->GetBinLowEdge(1);
  fZHighLim = fMCScan_Q0vsThetavsE->GetZaxis()->GetBinLowEdge(
      fMCScan_Q0vsThetavsE->GetNbinsZ() + 2);

  std::cout << "Sample " << name << "initialised: "
            << "{" << fXLowLim << "--" << fXHighLim << ", " << fYLowLim << "--"
            << fYHighLim << ", " << fZLowLim << "--" << fZHighLim << "}"
            << std::endl;
}

//********************************************************************
void ElectronScattering_DurhamData::FillEventVariables(FitEvent* event) {
  //********************************************************************

  if (event->NumFSParticle(11) == 0) return;

  FitParticle* ein = event->PartInfo(0);
  FitParticle* eout = event->GetHMFSParticle(11);

  double q0 = fabs(ein->fP.E() - eout->fP.E()) / 1000.0;
  double E = ein->fP.E() / 1000.0;
  double theta = ein->fP.Vect().Angle(eout->fP.Vect()) * 180. / M_PI;

  fXVar = q0;
  fYVar = theta;
  fZVar = E;

  return;
};

//********************************************************************
bool ElectronScattering_DurhamData::isSignal(FitEvent* event) {
  //********************************************************************

  if (event->NumFSParticle(11) == 0) {
    std::cout << "Ev Cut due to no FS electron." << std::endl;
    return false;
  }

  // std::cout << "fXVar = " << fXVar << " " << fXLowLim << " " << fXHighLim <<
  // std::endl;
  // std::cout << "fYVar = " << fYVar << " " << fYLowLim << " " << fYHighLim <<
  // std::endl;
  // std::cout << "fZVar = " << fZVar << " " << fZLowLim << " " << fZHighLim <<
  // std::endl;
  // std::cout << "iWeight: " << event->InputWeight << std::endl;

  if (fXVar < fXLowLim or fXVar > fXHighLim) {
    // std::cout << "Ev Cut due to X lims: " << fXLowLim << " -- " << fXHighLim
    //           << " !<> " << fXVar << std::endl;
    return false;
  }
  if (fYVar < fYLowLim or fYVar > fYHighLim) {
    // std::cout << "Ev Cut due to Y lims: " << fYLowLim << " -- " << fYHighLim
    //           << " !<> " << fXVar << std::endl;
    return false;
  }
  if (fZVar < fZLowLim or fZVar > fZHighLim) {
    // std::cout << "Ev Cut due to Z lims: " << fZLowLim << " -- " << fZHighLim
    //           << " !<> " << fXVar << std::endl;
    return false;
  }

  return true;
};

//********************************************************************
void ElectronScattering_DurhamData::FillHistograms() {
  //********************************************************************

  Measurement1D::FillHistograms();

  if (Signal) {
    fMCScan_Q0vsThetavsE->Fill(fXVar, fYVar, fZVar);
    fMCScan_Q0vsTheta->Fill(fXVar, fYVar);
    fMCScan_Q0vsE->Fill(fXVar, fZVar);
  }
}
//   Weight = 1.0;
//   if (Signal) {
//     fMCHist->Fill(fXVar, Weight);
//     fMCFine->Fill(fXVar, Weight);
//     fMCStat->Fill(fXVar, 1.0);

//     if (fMCHist_Modes) fMCHist_Modes->Fill(Mode, fXVar, Weight);
//   }
// }

void ElectronScattering_DurhamData::ResetAll() {
  Measurement1D::ResetAll();
  fMCScan_Q0vsThetavsE->Reset();
  fMCScan_Q0vsTheta->Reset();
  fMCScan_Q0vsE->Reset();
}

void ElectronScattering_DurhamData::ApplyNormScale(double norm) {
  Measurement1D::ApplyNormScale(norm);
  fMCScan_Q0vsThetavsE->Scale(1.0 / norm);
  fMCScan_Q0vsTheta->Scale(1.0 / norm);
  fMCScan_Q0vsE->Scale(1.0 / norm);
}

//********************************************************************
void ElectronScattering_DurhamData::ScaleEvents() {
  //********************************************************************
  Measurement1D::ScaleEvents();

  /*
  fMCScan_Q0vsThetavsE->Scale(fScaleFactor, "width");

  // Project into fMCScan_Q0vsTheta
  for (int x = 0; x < fMCScan_Q0vsThetavsE->GetNbinsX(); x++) {
    for (int y = 0; y < fMCScan_Q0vsThetavsE->GetNbinsY(); y++) {
      double total = 0.;
      for (int z = 0; z < fMCScan_Q0vsThetavsE->GetNbinsZ(); z++) {
        double zwidth = fMCScan_Q0vsThetavsE->GetZaxis()->GetBinWidth(z + 1);
        total += fMCScan_Q0vsThetavsE->GetBinContent(x + 1, y + 1, z + 1) *
  zwidth;
      }
      fMCScan_Q0vsTheta->SetBinContent(x + 1, y + 1, total);
    }
  }

  // Project into fMCScan_Q0vsE
  for (int x = 0; x < fMCScan_Q0vsThetavsE->GetNbinsX(); x++) {
    for (int z = 0; z < fMCScan_Q0vsThetavsE->GetNbinsZ(); z++) {
      double total = 0.;
      for (int y = 0; y < fMCScan_Q0vsThetavsE->GetNbinsY(); y++) {
        double ywidth = fMCScan_Q0vsThetavsE->GetYaxis()->GetBinWidth(y + 1);
        total += fMCScan_Q0vsThetavsE->GetBinContent(x + 1, y + 1, z + 1) *
  ywidth;
      }
      fMCScan_Q0vsE->SetBinContent(x + 1, z + 1, total);
    }
  }

  // Project fMCScan_Q0vsTheta into MC Hist
  for (int x = 0; x < fMCScan_Q0vsTheta->GetNbinsX(); x++) {
    double total = 0.;
    for (int y = 0; y < fMCScan_Q0vsTheta->GetNbinsY(); y++) {
      double ywidth = fMCScan_Q0vsTheta->GetYaxis()->GetBinWidth(y + 1);
      total += fMCScan_Q0vsTheta->GetBinContent(x + 1, y + 1);
    }
    double xwidth = fMCScan_Q0vsTheta->GetXaxis()->GetBinWidth(x + 1);
    fMCHist->SetBinContent(x + 1, total * xwidth);
  }

  fMCHist->Scale(fDataHist->Integral() / fMCHist->Integral());
  */
}

//********************************************************************
int ElectronScattering_DurhamData::GetNDOF() {
  //********************************************************************
  return fDataGraph->GetN();
}

void ElectronScattering_DurhamData::Write(std::string drawOpts) {
  Measurement1D::Write(drawOpts);

  fMCScan_Q0vsThetavsE->Write();
  fMCScan_Q0vsTheta->Write();
  fMCScan_Q0vsE->Write();
  fDataGraph->Write();
}

double ElectronScattering_DurhamData::GetLikelihood() { return 0.0; }

void ElectronScattering_DurhamData::SetFitOptions(std::string opt) { return; }

TH1D* ElectronScattering_DurhamData::GetMCHistogram(void) { return fMCHist; }

TH1D* ElectronScattering_DurhamData::GetDataHistogram(void) {
  return fDataHist;
}
