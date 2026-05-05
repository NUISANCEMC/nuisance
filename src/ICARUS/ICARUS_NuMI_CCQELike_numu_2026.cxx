// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "ICARUS_SignalDef.h"

#include "ICARUS_NuMI_CCQELike_numu_2026.h"


ICARUS_NuMI_CCQELike_numu_2026::ICARUS_NuMI_CCQELike_numu_2026(
  nuiskey samplekey )
{

  fSettings = LoadSampleSettings( samplekey );
  std::string name = fSettings.GetS("name");

  bool IsLogMode = false;
  if( fSettings.Has("LogData") ){
    IsLogMode = fSettings.GetB("LogData");
  }
  std::string CovMode = IsLogMode ? "Log" :"Linear";
  NUIS_LOG(SAM, "CovMode: "+CovMode);

  int numubar_start_index = -1;
  if(fSettings.Has("numubar_start_index")){
    numubar_start_index = fSettings.GetI("numubar_start_index");
  }

  if (!name.compare("ICARUS_NuMI_CCQELike_numu_2026_MuonCos")) {
    isTKI = false;
    fDist = kMuonCos;
    fVarName = "MuonCos";
    fSettings.SetXTitle("cos(#theta_{#mu})");
    fSettings.SetYTitle("d#sigma/dcos(#theta_{#mu}) (cm^{2}/Ar)");
  } else if (!name.compare("ICARUS_NuMI_CCQELike_numu_2026_MuonProtonCos")) {
    isTKI = false;
    fDist = kMuonProtonCos;
    fVarName = "MuonProtonCos";
    fSettings.SetXTitle("cos(#theta_{#mu,p})");
    fSettings.SetYTitle("d#sigma/dcos(#theta_{#mu,p}) (cm^{2}/Ar)");
  } else if (!name.compare("ICARUS_NuMI_CCQELike_numu_2026_deltaPT")) {
    isTKI = true;
    fDist = kdeltaPT;
    fVarName = "deltaPT";
    fSettings.SetXTitle("#deltap_{T} (GeV/c)");
    fSettings.SetYTitle("d#sigma/d#deltap_{T} (cm^{2}/(GeV/c)Ar)");
  } else if (!name.compare("ICARUS_NuMI_CCQELike_numu_2026_deltaalphaT")) {
    isTKI = true;
    fDist = kdeltaalphaT;
    fVarName = "deltaalphaT";
    fSettings.SetXTitle("#delta#alpha_{T} (degree)");
    fSettings.SetYTitle("d#sigma/d#delta#alpha_{T} (cm^{2}/(degree)Ar)");
  } else {
    NUIS_ABORT(
        "ICARUS_NuMI_CCQELike_numu_2026: Didn’t get a valid name: " << name);
  }

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n"
                               "Target: Ar\n"
                               "Flux: NuMI FHC numu+numubar\n"
                               "Signal: CCQELike muon and antimuon neutrino\n";
  fSettings.SetDescription(descrip);
  fSettings.SetTitle(name);
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV",
                            "FIX/FULL");
  fSettings.SetEnuRange(0.0, 20.0);
  fSettings.DefineAllowedTargets("Ar");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.DefineAllowedSpecies("numubar");
  FinaliseSampleSettings();

  // Load data ---------------------------------------------------------

  fLogNormalData = IsLogMode;

  std::string inputFile_xsec_value = FitPar::GetDataBase() +
              "ICARUS/NuMI/CCQELike_numu_2016/nuis_ICARUS_NuMI_1muNp0pi_2026_SimFit_"+fVarName+"_xsec_values_"+CovMode+".txt";
  SetDataFromTextFile(inputFile_xsec_value);
  if(IsLogMode){
    fXsecOffset=1.;
  }
  else{
    ScaleData(1E-38);
  }

  if(numubar_start_index>=0){
    int NInputFiles = GetInput()->tmp_flux_hists.size();
    // numu
    double EvtIntegral_numu = 0.;
    double FluxIntegral_numu = 0.;
    int NEvent_numu = 0;
    int NFiles_numu = 0;
    for(int i_file=0; i_file<numubar_start_index; i_file++){
      EvtIntegral_numu += GetInput()->tmp_evt_hists[i_file]->Integral("width");
      FluxIntegral_numu += GetInput()->tmp_flux_hists[i_file]->Integral("width");
      NEvent_numu += GetInput()->tmp_nevents[i_file];
      NFiles_numu += 1;
    }
    // numubar
    double EvtIntegral_numubar = 0.;
    double FluxIntegral_numubar = 0.;
    int NEvent_numubar = 0;
    int NFiles_numubar = 0;
    for(int i_file=numubar_start_index; i_file<NInputFiles; i_file++){
      EvtIntegral_numubar += GetInput()->tmp_evt_hists[i_file]->Integral("width");
      FluxIntegral_numubar += GetInput()->tmp_flux_hists[i_file]->Integral("width");
      NEvent_numubar += GetInput()->tmp_nevents[i_file];
      NFiles_numubar += 1;
    }

    double FluxIntegral_all = FluxIntegral_numu/NFiles_numu+FluxIntegral_numubar/NFiles_numubar;

    weight_numu = EvtIntegral_numu/NEvent_numu * 1E-38 / FluxIntegral_all * 40 / NFiles_numu;
    weight_numubar = EvtIntegral_numubar/NEvent_numubar * 1E-38 / FluxIntegral_all * 40 / NFiles_numubar;

    fScaleFactor = 1.;

    UseWeightFromMeas = true;

    NUIS_LOG(SAM, "numu/numubar separate sample mode");
    NUIS_LOG(SAM, "- numu:");
    NUIS_LOG(SAM, "Event integral = " << EvtIntegral_numu);
    NUIS_LOG(SAM, "Nevent = " << NEvent_numu);
    NUIS_LOG(SAM, "Flux integral = " << FluxIntegral_numu);
    NUIS_LOG(SAM, "-> weight_numu = " << weight_numu);
    NUIS_LOG(SAM, "- numubar:");
    NUIS_LOG(SAM, "Event integral = " << EvtIntegral_numubar);
    NUIS_LOG(SAM, "Nevent = " << NEvent_numubar);
    NUIS_LOG(SAM, "Flux integral = " << FluxIntegral_numubar);
    NUIS_LOG(SAM, "-> weight_numubar = " << weight_numubar);

  }
  else{
    fScaleFactor = GetEventHistogram()->Integral("width") / fNEvents * 1.0E-38 /
                 TotalIntegratedFlux() * 40;
    NUIS_LOG(SAM, "numu+numubar combined sample mode");
    NUIS_LOG(SAM, "Event integral = " << GetEventHistogram()->Integral("width"));
    NUIS_LOG(SAM, "Nevent = " << fNEvents);
    NUIS_LOG(SAM, "Flux integral = " << TotalIntegratedFlux());
    NUIS_LOG(SAM, "-> fScaleFactor = " << fScaleFactor);

  }

  std::string inputFile_xsec_cov = FitPar::GetDataBase() +
              "ICARUS/NuMI/CCQELike_numu_2016/nuis_ICARUS_NuMI_1muNp0pi_2026_SimFit_"+fVarName+"_xsec_cov_"+CovMode+".txt";
  SetCovarFromTextFile(inputFile_xsec_cov);

  FinaliseMeasurement();

}

void ICARUS_NuMI_CCQELike_numu_2026::FillEventVariables( FitEvent* event ) {

  SignalDef::helper_ICARUS_NuMI_CCQELike_numu helper_meas = SignalDef::isICARUS_NuMI_CCQELike_numu(event, EnuMin, EnuMax);

  bool pass = helper_meas.IsSignal;
  if(isTKI) pass = pass && helper_meas.IsMuonPLT0p8;

  if(!pass) return;
  
  Weight = helper_meas.IsAntiNu ? weight_numubar : weight_numu;

  if(fDist==kMuonCos) fXVar = helper_meas.MuonCos;
  else if(fDist==kMuonProtonCos) fXVar = helper_meas.MuonProtonCos;
  else if(fDist==kdeltaPT) fXVar = helper_meas.deltaPT;
  else if(fDist==kdeltaalphaT) fXVar = helper_meas.deltaalphaT;
  else{
    NUIS_ABORT(
        "ICARUS_NuMI_CCQELike_numu_2026: " << fVarName << " is not supported\n");
  }

}

bool ICARUS_NuMI_CCQELike_numu_2026::isSignal( FitEvent* event ) {

  SignalDef::helper_ICARUS_NuMI_CCQELike_numu helper_meas = SignalDef::isICARUS_NuMI_CCQELike_numu(event, EnuMin, EnuMax);

  bool pass = helper_meas.IsSignal;
  if(isTKI) pass = pass && helper_meas.IsMuonPLT0p8;

  Weight = helper_meas.IsAntiNu ? weight_numubar : weight_numu;

  return pass;
}
