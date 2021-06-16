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
#include "ComparisonRoutines.h"

/*
  Constructor/Destructor
*/
//************************
void ComparisonRoutines::Init() {
  //************************

  fOutputFile = "";
  fOutputRootFile = NULL;

  fStrategy = "Compare";

  fRoutines.clear();

  fCardFile = "";

  fFakeDataInput = "";

  fSampleFCN = NULL;

  fAllowedRoutines = ("Compare");
};

//*************************************
ComparisonRoutines::~ComparisonRoutines() {
  //*************************************
  delete fOutputRootFile;
};

/*
  Input Functions
*/
//*************************************
ComparisonRoutines::ComparisonRoutines(int argc, char *argv[]) {
  //*************************************

  // Initialise Defaults
  Init();
  nuisconfig configuration = Config::Get();

  // Default containers
  std::string cardfile = "";
  std::string maxevents = "-1";
  std::string skipevents = "0";
  int errorcount = 0;
  int verbocount = 0;
  std::vector<std::string> xmlcmds;
  std::vector<std::string> configargs;

  // Make easier to handle arguments.
  std::vector<std::string> args = GeneralUtils::LoadCharToVectStr(argc, argv);
  ParserUtils::ParseArgument(args, "-c", fCardFile, true);
  ParserUtils::ParseArgument(args, "-o", fOutputFile, false, false);
  ParserUtils::ParseArgument(args, "-n", maxevents, false, false);
  ParserUtils::ParseArgument(args, "-f", fStrategy, false, false);
  ParserUtils::ParseArgument(args, "-d", fFakeDataInput, false, false);
  ParserUtils::ParseArgument(args, "-i", xmlcmds);
  ParserUtils::ParseArgument(args, "-q", configargs);
  ParserUtils::ParseArgument(args, "-s", skipevents);
  ParserUtils::ParseCounter(args, "e", errorcount);
  ParserUtils::ParseCounter(args, "v", verbocount);
  ParserUtils::CheckBadArguments(args);

  // Add extra defaults if none given
  if (fCardFile.empty() and xmlcmds.empty()) {
    NUIS_ABORT("No input supplied!");
  }

  if (fOutputFile.empty() and !fCardFile.empty()) {
    fOutputFile = fCardFile + ".root";
    NUIS_ERR(WRN, "No output supplied so saving it to: " << fOutputFile);

  } else if (fOutputFile.empty()) {
    NUIS_ABORT("No output file or cardfile supplied!");
  }

  // Configuration Setup =============================

  // Check no comp key is available
  nuiskey fCompKey;
  if (Config::Get().GetNodes("nuiscomp").empty()) {
    fCompKey = Config::Get().CreateNode("nuiscomp");
  } else {
    fCompKey = Config::Get().GetNodes("nuiscomp")[0];
  }

  if (!fCardFile.empty())
    fCompKey.Set("cardfile", fCardFile);
  if (!fOutputFile.empty())
    fCompKey.Set("outputfile", fOutputFile);
  if (!fStrategy.empty())
    fCompKey.Set("strategy", fStrategy);

  // Load XML Cardfile
  configuration.LoadSettings(fCompKey.GetS("cardfile"), "");

  // Add Config Args
  for (size_t i = 0; i < configargs.size(); i++) {
    configuration.OverrideConfig(configargs[i]);
  }
  if (maxevents.compare("-1")) {
    configuration.OverrideConfig("MAXEVENTS=" + maxevents);
  }
  configuration.OverrideConfig("NSKIPEVENTS=" + skipevents);

  // Finish configuration XML
  configuration.FinaliseSettings(fCompKey.GetS("outputfile") + ".xml");

  // Add Error Verbo Lines
  verbocount += Config::GetParI("VERBOSITY");
  errorcount += Config::GetParI("ERROR");
  bool trace = Config::GetParB("TRACE");
  std::cout << "[ NUISANCE ]: Setting VERBOSITY=" << verbocount << std::endl;
  std::cout << "[ NUISANCE ]: Setting ERROR=" << errorcount << std::endl;
  SETVERBOSITY(verbocount);
  SETTRACE(trace);

  // Comparison Setup ========================================

  // Proper Setup
  fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");
  SetupComparisonsFromXML();

  SetupRWEngine();
  SetupFCN();

  return;
};

//*************************************
void ComparisonRoutines::SetupComparisonsFromXML() {
  //*************************************

  NUIS_LOG(FIT, "Setting up nuiscomp");

  // Setup Parameters ------------------------------------------
  std::vector<nuiskey> parkeys = Config::QueryKeys("parameter");
  if (!parkeys.empty()) {
    NUIS_LOG(FIT, "Number of parameters :  " << parkeys.size());
  }

  for (size_t i = 0; i < parkeys.size(); i++) {
    nuiskey key = parkeys.at(i);

    // Check for type,name,nom
    if (!key.Has("type")) {
      NUIS_ERR(FTL, "No type given for parameter " << i);
      NUIS_ABORT("type='PARAMETER_TYPE'");
    } else if (!key.Has("name")) {
      NUIS_ERR(FTL, "No name given for parameter " << i);
      NUIS_ABORT("name='SAMPLE_NAME'");
    } else if (!key.Has("nominal")) {
      NUIS_ERR(FTL, "No nominal given for parameter " << i);
      NUIS_ABORT("nominal='NOMINAL_VALUE'");
    }

    // Get Inputs
    std::string partype = key.GetS("type");
    std::string parname = key.GetS("name");
    double parnom = key.GetD("nominal");
    double parlow = parnom - 1;
    double parhigh = parnom + 1;
    double parstep = 1;

    // override if state not given
    if (!key.Has("state")) {
      key.SetS("state", "FIX");
    }

    std::string parstate = key.GetS("state");

    // Check for incomplete limtis
    int limdef =
        ((int)key.Has("low") + (int)key.Has("high") + (int)key.Has("step"));

    if (limdef > 0 and limdef < 3) {
      NUIS_ERR(FTL, "Incomplete limit set given for parameter : " << parname);
      NUIS_ABORT(
          "Requires: low='LOWER_LIMIT' high='UPPER_LIMIT' step='STEP_SIZE' ");
    }

    // Extra limits
    if (key.Has("low")) {

      parlow = key.GetD("low");
      parhigh = key.GetD("high");
      parstep = key.GetD("step");

      NUIS_LOG(FIT, "Read " << partype << " : " << parname << " = " << parnom
                            << " : " << parlow << " < p < " << parhigh << " : "
                            << parstate);
    } else {
      NUIS_LOG(FIT, "Read " << partype << " : " << parname << " = " << parnom
                            << " : " << parstate);
    }

    bool ismirr = false;
    if (key.Has("mirror_point")) {
      ismirr = true;
      mirror_param mir;
      mir.mirror_value = key.GetD("mirror_point");
      mir.mirror_above = key.GetB("mirror_above");
      fMirroredParams[parname] = mir;
      NUIS_LOG(FIT,
               "\t\t" << parname << " is mirrored at " << mir.mirror_value
                      << " "
                      << (mir.mirror_above ? "from above" : "from below"));
    }

    // Convert if required
    if (parstate.find("ABS") != std::string::npos) {
      if (ismirr) {
        NUIS_ABORT("Cannot mirror parameters with ABS state!");
      }
      parnom = FitBase::RWAbsToSigma(partype, parname, parnom);
      parlow = FitBase::RWAbsToSigma(partype, parname, parlow);
      parhigh = FitBase::RWAbsToSigma(partype, parname, parhigh);
      parstep = FitBase::RWAbsToSigma(partype, parname, parstep);
    } else if (parstate.find("FRAC") != std::string::npos) {
      if (ismirr) {
        NUIS_ABORT("Cannot mirror parameters with FRAC state!");
      }
      parnom = FitBase::RWFracToSigma(partype, parname, parnom);
      parlow = FitBase::RWFracToSigma(partype, parname, parlow);
      parhigh = FitBase::RWFracToSigma(partype, parname, parhigh);
      parstep = FitBase::RWFracToSigma(partype, parname, parstep);
    }

    // Push into vectors
    fParams.push_back(parname);

    fTypeVals[parname] = FitBase::ConvDialType(partype);
    fCurVals[parname] = parnom;
    fStateVals[parname] = parstate;
  }

  // Setup Samples ----------------------------------------------
  std::vector<nuiskey> samplekeys = Config::QueryKeys("sample");
  if (!samplekeys.empty()) {
    NUIS_LOG(FIT, "Number of samples : " << samplekeys.size());
  }

  for (size_t i = 0; i < samplekeys.size(); i++) {
    nuiskey key = samplekeys.at(i);

    // Get Sample Options
    std::string samplename = key.GetS("name");
    std::string samplefile = key.GetS("input");
    std::string sampletype = key.Has("type") ? key.GetS("type") : "DEFAULT";
    double samplenorm = key.Has("norm") ? key.GetD("norm") : 1.0;

    // Handle the samplefile name
    std::string mc_type;
    std::vector<std::string> sample_vect;
    if (GeneralUtils::ParseToStr(samplefile, ":").size() > 1) {
      mc_type = GeneralUtils::ParseToStr(samplefile, ":")[0];
      std::string input_samples = GeneralUtils::ParseToStr(samplefile, ":")[1];
      std::cout << "mc_type: " << mc_type << std::endl;
      std::cout << "input_samples: " << input_samples << std::endl;
      if (!input_samples.empty()) {
        input_samples = GeneralUtils::ReplaceAll(input_samples, "(", "");
        input_samples = GeneralUtils::ReplaceAll(input_samples, ")", "");
      }
      std::vector<std::string> sample_vect = GeneralUtils::ParseToStr(input_samples, ";");
    }

    // Print out
    NUIS_LOG(FIT, "Read Sample " << i << ". : " << samplename << " ("
        << sampletype << ") [Norm=" << samplenorm << "]");
    NUIS_LOG(FIT, "  |-> Input MC type = "<< mc_type <<" with " << sample_vect.size() << " input files");
    for (uint j = 0; j < sample_vect.size(); ++j){
      NUIS_LOG(FIT, "  |-> Input file #" << j << " = " << sample_vect[j]);
    }

    // If FREE add to parameters otherwise continue
    if (sampletype.find("FREE") == std::string::npos) {
      if (samplenorm != 1.0) {
        NUIS_ERR(FTL, "You provided a sample normalisation but did not specify "
            "that the sample is free");
        NUIS_ABORT("Change so sample contains type=\"FREE\" and re-run");
      }
      continue;
    }

    // Form norm dial from samplename + sampletype + "_norm";
    std::string normname = samplename + "_norm";

    // Check normname not already present
    if (fTypeVals.find("normname") != fTypeVals.end()) {
      continue;
    }

    // Add new norm dial to list if its passed above checks
    fParams.push_back(normname);

    fTypeVals[normname] = kNORM;
    fStateVals[normname] = sampletype;
    fCurVals[normname] = samplenorm;
  }

  // Setup Fake Parameters -----------------------------
  std::vector<nuiskey> fakekeys = Config::QueryKeys("fakeparameter");
  if (!fakekeys.empty()) {
    NUIS_LOG(FIT, "Number of fake parameters : " << fakekeys.size());
  }

  for (size_t i = 0; i < fakekeys.size(); i++) {
    nuiskey key = fakekeys.at(i);

    // Check for type,name,nom
    if (!key.Has("name")) {
      NUIS_ABORT("No name given for fakeparameter " << i);

    } else if (!key.Has("nominal")) {
      NUIS_ABORT("No nominal given for fakeparameter " << i);
    }

    // Get Inputs
    std::string parname = key.GetS("name");
    double parnom = key.GetD("nominal");

    // Push into vectors
    fFakeVals[parname] = parnom;
  }
}

//*************************************
void ComparisonRoutines::SetupRWEngine() {
  //*************************************

  NUIS_LOG(FIT, "Setting up FitWeight Engine");
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];
    FitBase::GetRW()->IncludeDial(name, fTypeVals.at(name));
  }

  return;
}

//*************************************
void ComparisonRoutines::SetupFCN() {
  //*************************************

  NUIS_LOG(FIT, "Building the SampleFCN");
  if (fSampleFCN)
    delete fSampleFCN;
  Config::Get().out = fOutputRootFile;
  fOutputRootFile->cd();
  fSampleFCN = new JointFCN(fOutputRootFile);
  SetFakeData();

  return;
}

//*************************************
void ComparisonRoutines::SetFakeData() {
  //*************************************

  if (fFakeDataInput.empty())
    return;

  if (fFakeDataInput.compare("MC") == 0) {
    NUIS_LOG(FIT, "Setting fake data from MC starting prediction.");
    UpdateRWEngine(fFakeVals);

    FitBase::GetRW()->Reconfigure();
    fSampleFCN->ReconfigureAllEvents();
    fSampleFCN->SetFakeData("MC");

    std::map<std::string, double> CurVals_wmirr;
    for (size_t i = 0; i < fParams.size(); ++i) {
      std::string const &pname = fParams[i];
      if (fMirroredParams.count(pname)) {
        if (!fMirroredParams[pname].mirror_above &&
            (fCurVals[pname] < fMirroredParams[pname].mirror_value)) {
          double xabove = fMirroredParams[pname].mirror_value - fCurVals[pname];
          CurVals_wmirr[pname] = fMirroredParams[pname].mirror_value + xabove;
          std::cout << "\t--Parameter " << pname << " mirrored from "
            << fCurVals[pname] << " -> " << CurVals_wmirr[pname]
            << std::endl;
        } else if (fMirroredParams[pname].mirror_above &&
            (fCurVals[pname] >= fMirroredParams[pname].mirror_value)) {
          double xabove = fCurVals[pname] - fMirroredParams[pname].mirror_value;
          CurVals_wmirr[pname] = fMirroredParams[pname].mirror_value - xabove;
          std::cout << "\t--Parameter " << pname << " mirrored from "
            << fCurVals[pname] << " -> " << CurVals_wmirr[pname]
            << std::endl;
        } else {
          CurVals_wmirr[pname] = fCurVals[pname];
        }
      } else {
        CurVals_wmirr[pname] = fCurVals[pname];
      }
    }

    UpdateRWEngine(CurVals_wmirr);

    NUIS_LOG(FIT, "Set all data to fake MC predictions.");
  } else {
    NUIS_LOG(FIT, "Setting fake data from: " << fFakeDataInput);
    fSampleFCN->SetFakeData(fFakeDataInput);
  }

  return;
}

/*
   Fitting Functions
*/
//*************************************
void ComparisonRoutines::UpdateRWEngine(
    std::map<std::string, double> &updateVals) {
  //*************************************

  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];

    if (updateVals.find(name) == updateVals.end())
      continue;
    FitBase::GetRW()->SetDialValue(name, updateVals.at(name));
  }

  FitBase::GetRW()->Reconfigure();
  return;
}

//*************************************
void ComparisonRoutines::Run() {
  //*************************************

  NUIS_LOG(FIT, "Running ComparisonRoutines : " << fStrategy);

  if (FitPar::Config().GetParB("save_nominal")) {
    SaveNominal();
  }

  // Parse given routines
  fRoutines = GeneralUtils::ParseToStr(fStrategy, ",");
  if (fRoutines.empty()) {
    NUIS_ABORT("Trying to run ComparisonRoutines with no routines given!");
  }

  for (UInt_t i = 0; i < fRoutines.size(); i++) {
    std::string routine = fRoutines.at(i);

    // Mostly this is unnecessary information, so don't always print
    if (fRoutines.size() > 1){
      NUIS_LOG(FIT, "Routine: " << routine);
    }
    if (!routine.compare("Compare")) {

      std::map<std::string, double> CurVals_wmirr;
      for (size_t i = 0; i < fParams.size(); ++i) {
        std::string const &pname = fParams[i];
        if (fMirroredParams.count(pname)) {
          std::cout << "MA? " << fMirroredParams[pname].mirror_above
            << ", OVal: " << fCurVals[pname]
            << ", MPoint: " << fMirroredParams[pname].mirror_value
            << std::endl;
          if (!fMirroredParams[pname].mirror_above &&
              (fCurVals[pname] < fMirroredParams[pname].mirror_value)) {
            double xabove =
              fMirroredParams[pname].mirror_value - fCurVals[pname];
            CurVals_wmirr[pname] = fMirroredParams[pname].mirror_value + xabove;
            std::cout << "\t--Parameter " << pname << " mirrored from "
              << fCurVals[pname] << " -> " << CurVals_wmirr[pname]
              << std::endl;
          } else if (fMirroredParams[pname].mirror_above &&
              (fCurVals[pname] >= fMirroredParams[pname].mirror_value)) {
            double xabove =
              fCurVals[pname] - fMirroredParams[pname].mirror_value;
            CurVals_wmirr[pname] = fMirroredParams[pname].mirror_value - xabove;
            std::cout << "\t--Parameter " << pname << " mirrored from "
              << fCurVals[pname] << " -> " << CurVals_wmirr[pname]
              << std::endl;
          } else {
            CurVals_wmirr[pname] = fCurVals[pname];
          }
        } else {
          CurVals_wmirr[pname] = fCurVals[pname];
        }
        std::cout << "~~~~~~~" << pname << " : " << fCurVals[pname] << " -> "
          << CurVals_wmirr[pname] << std::endl;
      }

      UpdateRWEngine(CurVals_wmirr);
      GenerateComparison();
      PrintState();
      SaveCurrentState();
    }
  }

  return;
}

//*************************************
void ComparisonRoutines::GenerateComparison() {
  //*************************************
  NUIS_LOG(FIT, "Generating Comparison.");
  // Main Event Loop from event Manager
  fSampleFCN->ReconfigureAllEvents();
  return;
}

//*************************************
void ComparisonRoutines::PrintState() {
  //*************************************

  // Count max size
  int maxcount = 0;
  for (UInt_t i = 0; i < fParams.size(); i++) {
    maxcount = max(int(fParams[i].size()), maxcount);
  }

  // Header
  if (fParams.size()){
    NUIS_LOG(FIT, "------------");
    NUIS_LOG(FIT, " #    " << left << setw(maxcount) << "Parameter "
        << " = " << setw(10) << "Value"
        << " +- " << setw(10) << "Error"
        << " " << setw(8) << "(Units)");
    // << " " << setw(10) << "Conv. Val"
    // << " +- " << setw(10) << "Conv. Err"
    // << " " << setw(8) << "(Units)");
  }

  std::map<std::string, double> CurVals_wmirr;
  for (size_t i = 0; i < fParams.size(); ++i) {
    std::string const &pname = fParams[i];
    if (fMirroredParams.count(pname)) {
      if (!fMirroredParams[pname].mirror_above &&
          (fCurVals[pname] < fMirroredParams[pname].mirror_value)) {
        double xabove = fMirroredParams[pname].mirror_value - fCurVals[pname];
        CurVals_wmirr[pname] = fMirroredParams[pname].mirror_value + xabove;
        std::cout << "\t--Parameter " << pname << " mirrored from "
          << fCurVals[pname] << " -> " << CurVals_wmirr[pname]
          << std::endl;
      } else if (fMirroredParams[pname].mirror_above &&
          (fCurVals[pname] >= fMirroredParams[pname].mirror_value)) {
        double xabove = fCurVals[pname] - fMirroredParams[pname].mirror_value;
        CurVals_wmirr[pname] = fMirroredParams[pname].mirror_value - xabove;
        std::cout << "\t--Parameter " << pname << " mirrored from "
          << fCurVals[pname] << " -> " << CurVals_wmirr[pname]
          << std::endl;
      } else {
        CurVals_wmirr[pname] = fCurVals[pname];
      }
    } else {
      CurVals_wmirr[pname] = fCurVals[pname];
    }
  }

  // Parameters
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string syst = fParams.at(i);

    std::string typestr = FitBase::ConvDialType(fTypeVals[syst]);
    std::string curunits = "(sig.)";
    double curval = fCurVals[syst];
    double curerr = 0.0;

    if (fStateVals[syst].find("ABS") != std::string::npos) {
      curval = FitBase::RWSigmaToAbs(typestr, syst, curval);
      curerr = (FitBase::RWSigmaToAbs(typestr, syst, curerr) -
          FitBase::RWSigmaToAbs(typestr, syst, 0.0));
      curunits = "(Abs.)";
    } else if (fStateVals[syst].find("FRAC") != std::string::npos) {
      curval = FitBase::RWSigmaToFrac(typestr, syst, curval);
      curerr = (FitBase::RWSigmaToFrac(typestr, syst, curerr) -
          FitBase::RWSigmaToFrac(typestr, syst, 0.0));
      curunits = "(Frac)";
    }

    // std::string convunits = "(" + FitBase::GetRWUnits(typestr, syst) + ")";
    // double convval = FitBase::RWSigmaToAbs(typestr, syst, curval);
    // double converr = (FitBase::RWSigmaToAbs(typestr, syst, curerr) -
    //                   FitBase::RWSigmaToAbs(typestr, syst, 0.0));

    std::ostringstream curparstring;

    curparstring << " " << setw(3) << left << i << "  " << setw(maxcount)
      << syst << " = " << setw(10) << Form("%.7lf", curval) << " +- " << setw(10)
      << Form("%.7lf", curerr) << " " << setw(8) << curunits;
    // << " " << setw(10);
    // << convval << " +- " << setw(10) << converr << " " << setw(8)
    // << convunits;
    NUIS_LOG(FIT, curparstring.str());

    if (fMirroredParams.count(syst)) {
      NUIS_LOG(FIT, "\t\t--> Mirrored at " << fMirroredParams[syst].mirror_value
          << " to effective value "
          << CurVals_wmirr[syst]);
    }
  }

  NUIS_LOG(FIT, "------------");
  double like = fSampleFCN->GetLikelihood();
  int ndof = fSampleFCN->GetNDOF();
  NUIS_LOG(FIT,
      std::left << std::setw(55) << "Likelihood for JointFCN" << ": " << like << "/" << ndof);
  NUIS_LOG(FIT, "------------");
}

/*
   Write Functions
*/
//*************************************
void ComparisonRoutines::SaveCurrentState(std::string subdir) {
  //*************************************

  NUIS_LOG(FIT, "Saving current full FCN predictions");

  // Setup DIRS
  TDirectory *curdir = gDirectory;
  if (!subdir.empty()) {
    TDirectory *newdir = (TDirectory *)gDirectory->mkdir(subdir.c_str());
    newdir->cd();
  }

  fSampleFCN->Write();

  // Change back to current DIR
  curdir->cd();

  return;
}

//*************************************
void ComparisonRoutines::SaveNominal() {
  //*************************************

  fOutputRootFile->cd();

  NUIS_LOG(FIT, "Saving Nominal Predictions (be cautious with this)");
  FitBase::GetRW()->Reconfigure();
  GenerateComparison();
  SaveCurrentState("nominal");
};
