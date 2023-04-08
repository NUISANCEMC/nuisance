#include "FitLogger.h"
#include "PlotUtils.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TFolder.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef GENIE3_API_ENABLED
#include "Framework/Conventions/Units.h"
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/ParticleData/PDGUtils.h"
#else
#include "Conventions/Units.h"
#include "GHEP/GHepParticle.h"
#include "PDG/PDGUtils.h"
#endif

std::string gInputFiles = "";
std::string gOutputFile = "";
std::string gFluxFile = "";
std::string gTarget = "";
double MonoEnergy;
int gNEvents = -999;
bool IsMonoE = false;
bool useNOvAWeights = false;

void PrintOptions();
void ParseOptions(int argc, char *argv[]);
void RunGENIEPrepareMono(std::string input, std::string target,
                         std::string output);
void RunGENIEPrepare(std::string input, std::string flux, std::string target,
                     std::string output);
bool CheckConfig(std::string filename);

int main(int argc, char *argv[]) {
  ParseOptions(argc, argv);
  if (IsMonoE) {
    RunGENIEPrepareMono(gInputFiles, gTarget, gOutputFile);
  } else {
    RunGENIEPrepare(gInputFiles, gFluxFile, gTarget, gOutputFile);
  }
}

// CWret October 2019
// We have to scale the 2p2h cross-section when the 2p2h generator in GENIE is Nieves
//****
// SD: OR SuSA!
//****
// When the 2p2h generator is Empirical no such scaling is needed
// This is needed to match the output of PrepareGENIE to GENIE splines from gspl2root
// Allow the user to override this for debugging
// Search for what model the user ran with
bool CheckConfig(std::string filename) {
  TFile *f = new TFile(filename.c_str());
  // Get the config
  TFolder *first = (TFolder*)f->Get("gconfig");
  // Find the GlobalParameterList
  TFolder *folder = (TFolder*)first->FindObject("GlobalParameterList/Default");
  TIter iter(folder->GetListOfFolders());
  TKey *key;
  // Should we scale this? Only if Nieves 2p2h
  bool ShouldScale = false;
  while ((key = (TKey*)iter.Next())) {
    std::string name = key->GetName();
    // Look for XSecModel (specifies the interaction model GENIE was run with)
    // Look for MEC (specifies it's a MEC setting)
    // Look for Nieves 2p2h from 2016
    if (name.find("XSecModel") != std::string::npos && name.find("MEC-CC") != std::string::npos && name.find("NievesSimoVacasMECPXSec2016") != std::string::npos) {
      ShouldScale = true;
    }
    // The same thing applies to the SuSAv2 model
    if (name.find("XSecModel") != std::string::npos && name.find("MEC-CC") != std::string::npos && name.find("SuSAv2MECPXSec") != std::string::npos) {
      ShouldScale = true;
    }
  }
  f->Close();
  if (!ShouldScale) {
    NUIS_LOG(FIT, "Not scaling 2p2h CC events with Nieves...");
  } else {
    NUIS_LOG(FIT, "Scaling 2p2h CC events with Nieves...");
  }

  return ShouldScale;
}

void RunGENIEPrepareMono(std::string input, std::string target,
    std::string output) {

  NUIS_LOG(FIT, "Running GENIE Prepare in mono energetic with E = " << MonoEnergy
      << " GeV");

  // Setup TTree
  TChain *tn = new TChain("gtree");
  std::string first_file = "";

  if (input.find_first_of(',') != std::string::npos) {
    std::vector<std::string> inputvect = GeneralUtils::ParseToStr(input, ",");

    for (size_t iv_it = 0; iv_it < inputvect.size(); ++iv_it) {
      tn->AddFile(inputvect[iv_it].c_str());
      NUIS_LOG(FIT, "Added input file: " << inputvect[iv_it]);
      if (!first_file.length()) {
        first_file = inputvect[iv_it];
      }
    }
  } else { // The Add form can accept wildcards.
    tn->Add(input.c_str());
    first_file = input;
  }

  if (tn->GetFile() == NULL) {
    tn->Print();
    NUIS_ERR(FTL, "gtree not located in GENIE file: " << input);
    NUIS_ABORT("Check your inputs, they may need to be completely regenerated!");
    throw;
  }

  int nevt = tn->GetEntries();
  if (gNEvents != -999) {
    NUIS_LOG(FIT, "Overriding number of events by user from " << nevt << " to "
        << gNEvents);
    nevt = gNEvents;
  }

  if (!nevt) {
    NUIS_ABORT("Couldn't load any events from input specification: \""
        << input.c_str() << "\"");
  } else {
    NUIS_LOG(FIT, "Found " << nevt << " input entries in " << input);
  }

  StopTalking();
  NtpMCEventRecord *genientpl = NULL;
  tn->SetBranchAddress("gmcrec", &genientpl);
  StartTalking();

  // Have the TH1D go from MonoEnergy/2 to MonoEnergy/2
  TH1D *fluxhist =
    new TH1D("flux", "flux", 1000, MonoEnergy / 2., MonoEnergy * 2.);
  fluxhist->Fill(MonoEnergy);
  fluxhist->Scale(1, "width");

  // Make Event Hist
  TH1D *eventhist = (TH1D *)fluxhist->Clone();
  eventhist->Reset();

  TH1D *xsechist = (TH1D *)eventhist->Clone();

  // Create maps
  std::map<std::string, TH1D *> modexsec;
  std::map<std::string, TH1D *> modecount;
  std::vector<std::string> genieids;
  std::vector<std::string> targetids;
  std::vector<std::string> interids;

  // Loop over all events
  for (int i = 0; i < nevt; i++) {
    tn->GetEntry(i);

    StopTalking();
    EventRecord &event = *(genientpl->event);
    GHepParticle *neu = event.Probe();
    StartTalking();

    // Get XSec From Spline
    GHepRecord genie_record = static_cast<GHepRecord>(event);
    double xsec = (genie_record.XSec() / (1E-38 * genie::units::cm2));

    // Parse Interaction String
    std::string mode = genie_record.Summary()->AsString();
    std::vector<std::string> modevec = GeneralUtils::ParseToStr(mode, ";");
    std::string targ = (modevec[0] + ";" + modevec[1]);
    std::string inter = mode;

    // Fill lists of Unique IDS
    if (std::find(targetids.begin(), targetids.end(), targ) ==
        targetids.end()) {
      targetids.push_back(targ);
    }

    if (std::find(interids.begin(), interids.end(), inter) == interids.end()) {
      interids.push_back(inter);
    }

    // Create entries Mode Maps
    if (modexsec.find(mode) == modexsec.end()) {
      genieids.push_back(mode);

      modexsec[mode] = (TH1D *)xsechist->Clone();
      modecount[mode] = (TH1D *)xsechist->Clone();

      modexsec[mode]->GetYaxis()->SetTitle(
          "d#sigma/dE_{#nu} #times 10^{-38} (events weighted by #sigma)");
      modecount[mode]->GetYaxis()->SetTitle("Number of events in file");
    }

    // Fill XSec Histograms
    modexsec[mode]->Fill(neu->E(), xsec);
    modecount[mode]->Fill(neu->E());

    // Fill total event hist
    eventhist->Fill(neu->E());

    // Clear Event
    genientpl->Clear();

    size_t freq = nevt / 20;
    if (freq && !(i % freq)) {
      NUIS_LOG(FIT, "Processed "
          << i << "/" << nevt << " GENIE events (E: " << neu->E()
          << " GeV, xsec: " << xsec << " E-38 cm^2/nucleon)");
    }
  }
  NUIS_LOG(FIT, "Processed all events");

  // Check if we need to correct MEC events before possibly deleting the TChain below
  bool MECcorrect = CheckConfig(std::string(tn->GetFile()->GetName()));

  TFile *outputfile;

  // If no output is specified just append to the file
  if (!gOutputFile.length()) {
    // Shut the chain;
    delete tn;
    outputfile = new TFile(first_file.c_str(), "UPDATE");
  } else {
    outputfile = new TFile(gOutputFile.c_str(), "RECREATE");
    outputfile->cd();

    NUIS_LOG(FIT, "Cloning input vector to output file: " << gOutputFile);
    TTree *cloneTree = tn->CloneTree(-1, "fast");
    cloneTree->SetDirectory(outputfile);
    cloneTree->Write();

    if (useNOvAWeights){
      NUIS_LOG(FIT, "Cloning input nova_wgts to output file: " << gOutputFile);
      // ***********************************
      // ***********************************
      // FUDGE FOR NOVA MINERVA WORKSHOP
      //  Also check for the nova_wgts tree from Jeremy
      TChain *nova_chain = new TChain("nova_wgts");
      nova_chain->AddFile(input.c_str());
      TTree *nova_tree = nova_chain->GetTree();
      if (!nova_tree) {
	NUIS_LOG(FIT, "Could not find nova_wgts tree in " << gOutputFile);
      } else {
	NUIS_LOG(FIT, "Found nova_wgts tree in " << gOutputFile);
      }
      if (nova_tree) {
	nova_tree->SetDirectory(outputfile);
	nova_tree->Write();
      }
    }

    NUIS_LOG(FIT, "Done cloning tree.");
  }

  NUIS_LOG(FIT, "Getting splines in mono-energetic...");

  // Save each of the reconstructed splines to file
  std::map<std::string, TH1D *> modeavg;

  TDirectory *inddir = (TDirectory *)outputfile->Get("IndividualGENIESplines");
  if (!inddir) inddir = (TDirectory *)outputfile->mkdir("IndividualGENIESplines");

  // Loop over GENIE ID's and get MEC count
  int MECcount = 0;
  // Count up the number of MEC splines
  for (UInt_t i = 0; i < genieids.size(); i++) {
    if (genieids[i].find("MEC") != std::string::npos && genieids[i].find("[CC]") != std::string::npos) {
      MECcount++;
    }
  }

  inddir->cd();
  for (UInt_t i = 0; i < genieids.size(); i++) {
    std::string mode = genieids[i];

    modexsec[mode]->Write((mode + "_summed_xsec").c_str(), TObject::kOverwrite);
    modecount[mode]->Write((mode + "_summed_evt").c_str(), TObject::kOverwrite);

    // Form extra avg xsec map -> Reconstructed spline
    modeavg[mode] = (TH1D *)modexsec[mode]->Clone();
    modeavg[mode]->GetYaxis()->SetTitle(
        "#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/target)");
    modeavg[mode]->Divide(modecount[mode]);

    if (MECcorrect && mode.find("MEC") != std::string::npos && mode.find("[CC]") != std::string::npos) {
      NUIS_LOG(FIT, "Scaling spline " << mode << " by 1/" << MECcount << " because there are " << MECcount << " repeated Nieves 2p2h instances.");
      modeavg[mode]->Scale(1.0 / double(MECcount));
    }

    modeavg[mode]->Write((mode + "_rec_spline").c_str(), TObject::kOverwrite);
  }

  TDirectory *targdir = (TDirectory *)outputfile->Get("TargetGENIESplines");
  if (!targdir)
    targdir = (TDirectory *)outputfile->mkdir("TargetGENIESplines");
  targdir->cd();

  NUIS_LOG(FIT, "Getting Target Splines");

  // For each target save a total spline
  std::map<std::string, TH1D *> targetsplines;

  for (uint i = 0; i < targetids.size(); i++) {
    std::string targ = targetids[i];
    NUIS_LOG(FIT, "Getting target " << i << ": " << targ);
    targetsplines[targ] = (TH1D *)xsechist->Clone();
    targetsplines[targ]->GetYaxis()->SetTitle(
        "#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/target)");
    NUIS_LOG(FIT, "Created target spline for " << targ);

    for (uint j = 0; j < genieids.size(); j++) {
      std::string mode = genieids[j];

      if (mode.find(targ) != std::string::npos) {
        NUIS_LOG(FIT, "    Mode " << mode << " contains " << targ << " target");
        targetsplines[targ]->Add(modeavg[mode]);
        NUIS_LOG(FIT,
            "Finished with Mode " << mode << " " << modeavg[mode]->Integral());
      }
    }

    NUIS_LOG(FIT, "Saving target spline:" << targ);
    targetsplines[targ]->Write(("Total_" + targ).c_str(), TObject::kOverwrite);
  }

  NUIS_LOG(FIT, "Getting total splines");
  // Now we have each of the targets we need to create a total cross-section.
  int totalnucl = 0;
  // Get the targets specified by the user, separated by commas
  // This has structure target1[fraction1], target2[fraction2]
  std::vector<std::string> targprs = GeneralUtils::ParseToStr(target, ",");

  std::vector<std::string> targ_list;
  std::vector<std::string> frac_list;

  // Chop up the target string which has format
  // TARGET1[fraction1],TARGET2[fraction2]

  // Loop over the vector of strings "TARGET1[fraction1]" "TARGET2[fraction2]"
  for (std::vector<std::string>::iterator it = targprs.begin();
      it != targprs.end(); ++it) {
    // Cut into "TARGET1" and "fraction1]"
    std::vector<std::string> targind = GeneralUtils::ParseToStr(*it, "[");
    // Cut into "TARGET1" and "fraction1"
    for (std::vector<std::string>::iterator jt = targind.begin();
        jt != targind.end(); ++jt) {
      if ((*jt).find("]") != std::string::npos) {
        (*jt) = (*jt).substr(0, (*jt).find("]"));
        frac_list.push_back(*jt);
        // Won't find bracket for target
      } else {
        targ_list.push_back(*jt);
      }
    }
  }

  targprs = targ_list;

  std::vector<double> targ_fractions;
  double minimum = 1.0;
  for (std::vector<std::string>::iterator it = frac_list.begin();
      it != frac_list.end(); it++) {
    double frac = std::atof((*it).c_str());
    targ_fractions.push_back(frac);
    if (frac < minimum)
      minimum = frac;
  }

  std::vector<double>::iterator it = targ_fractions.begin();
  std::vector<std::string>::iterator jt = targ_list.begin();
  double scaling = 0;
  for (; it != targ_fractions.end(); it++, jt++) {
    // First get the mass number from the targ_list
    int nucl = atoi((*jt).c_str());
    nucl = (nucl % 10000) / 10;
    // Gets the relative portions right
    *it = (*it) / minimum;
    // Scale relative the atomic mass
    double tempscaling = double(nucl) / (*it);
    if (tempscaling > scaling) scaling = tempscaling;
  }
  it = targ_fractions.begin();
  for (; it != targ_fractions.end(); it++) {
    // Round the scaling to nearest integer and multiply
    *it *= int(scaling + 0.5);
    // Round to nearest integer
    *it = int(*it + 0.5);
    totalnucl += *it;
  }

  // Report on what we've got
  it = targ_fractions.begin();
  jt = targ_list.begin();
  for (; it != targ_fractions.end(); it++, jt++){
    NUIS_LOG(FIT, "Found target " << *jt << " with weight " << *it);
  }


  if (totalnucl == 0) {
    NUIS_ABORT("Didn't find any nucleons in input file. Did you really specify the "
        "target ratios?\ne.g. TARGET1[fraction1],TARGET2[fraction2]");
  }
  TH1D *totalxsec = (TH1D *)xsechist->Clone();

  it = targ_fractions.begin();
  for (uint i = 0; i < targprs.size(); i++, it++) {
    std::string targpdg = targprs[i];
    // Check that we found the user requested target in GENIE
    bool FoundTarget = false;
    for (std::map<std::string, TH1D *>::iterator iter = targetsplines.begin();
        iter != targetsplines.end(); iter++) {
      std::string targstr = iter->first;
      TH1D *xsec = iter->second;

      // Match the user targets to the targets found in GENIE
      if (targstr.find(targpdg) != std::string::npos) {
        FoundTarget = true;

	int nucl = atoi(targpdg.c_str());
        nucl = (nucl % 10000) / 10;

	NUIS_LOG(FIT, "Scaling target " << targstr << " by " << *it << "/" << nucl);
        xsec->Scale(*it/double(nucl));

        NUIS_LOG(FIT, "Adding target spline "
            << targstr << " Integral = " << xsec->Integral("width"));
        totalxsec->Add(xsec);
      }
    }

    // Check that targets were all found
    if (!FoundTarget) {
      NUIS_ERR(WRN, "Didn't find target "
          << targpdg
          << " in the list of targets recorded by GENIE");
      NUIS_ERR(WRN, "  The list of targets you requested is: ");
      for (uint i = 0; i < targprs.size(); ++i)
        NUIS_ERR(WRN, "    " << targprs[i]);
      NUIS_ERR(WRN, "  The list of targets found in GENIE is: ");
      for (std::map<std::string, TH1D *>::iterator iter = targetsplines.begin();
          iter != targetsplines.end(); iter++)
        NUIS_ERR(WRN, "    " << iter->first);
    }
  }

  outputfile->cd();
  totalxsec->GetYaxis()->SetTitle(
      "#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/nucleon)");
  totalxsec->Write("nuisance_xsec", TObject::kOverwrite);

  eventhist = (TH1D *)fluxhist->Clone();
  eventhist->Multiply(totalxsec);
  eventhist->GetYaxis()->SetTitle(
      (std::string("Event rate (N = #sigma #times #Phi) #times 10^{-38} "
                   "(cm^{2}/nucleon) #times ") +
       eventhist->GetYaxis()->GetTitle())
      .c_str());

  NUIS_LOG(FIT, "Dividing by Total Nucl = " << totalnucl);
  eventhist->Scale(1.0 / double(totalnucl));

  eventhist->Write("nuisance_events", TObject::kOverwrite);
  fluxhist->Write("nuisance_flux", TObject::kOverwrite);

  NUIS_LOG(FIT, "Inclusive XSec Per Nucleon = " << eventhist->Integral("width") *
      1E-38 /
      fluxhist->Integral("width"));
  NUIS_LOG(FIT, "XSec Hist Integral = " << totalxsec->Integral("width"));

  outputfile->Close();

  return;
}

void RunGENIEPrepare(std::string input, std::string flux, std::string target,
    std::string output) {
  NUIS_LOG(FIT, "Running GENIE Prepare with flux...");

  // Get Flux Hist
  std::vector<std::string> fluxvect = GeneralUtils::ParseToStr(flux, ",");
  TH1 *fluxhist = NULL;
  if (fluxvect.size() == 3) {
    double from = GeneralUtils::StrToDbl(fluxvect[0]);
    double to = GeneralUtils::StrToDbl(fluxvect[1]);
    double step = GeneralUtils::StrToDbl(fluxvect[2]);

    int nstep = ceil((to - from) / step);
    to = from + step * nstep;

    NUIS_LOG(FIT, "Generating flat flux histogram from "
        << from << " to " << to << " with bins " << step
        << " wide (NBins = " << nstep << ").");

    fluxhist =
      new TH1D("spectrum", ";E_{#nu} (GeV);Count (A.U.)", nstep, from, to);

    for (Int_t bi_it = 1; bi_it < fluxhist->GetXaxis()->GetNbins(); ++bi_it) {
      fluxhist->SetBinContent(bi_it, 1.0 / double(step * nstep));
    }
    fluxhist->SetDirectory(0);
  } else if (fluxvect.size() == 2) {
    TFile *fluxfile = new TFile(fluxvect[0].c_str(), "READ");
    if (!fluxfile->IsZombie()) {
      fluxhist = dynamic_cast<TH1 *>(fluxfile->Get(fluxvect[1].c_str()));
      if (!fluxhist) {
        NUIS_ERR(FTL, "Couldn't find histogram named: \""
            << fluxvect[1] << "\" in file: \"" << fluxvect[0]);
        throw;
      }
      fluxhist->SetDirectory(0);
    }
  } else if (fluxvect.size() == 1) {
    MonoEnergy = GeneralUtils::StrToDbl(fluxvect[0]);
    RunGENIEPrepareMono(input, target, output);
    return;
  } else {
    NUIS_LOG(FTL, "Bad flux specification: \"" << flux << "\".");
    throw;
  }

  // Setup TTree
  TChain *tn = new TChain("gtree");
  std::string first_file = "";

  if (input.find_first_of(',') != std::string::npos) {
    std::vector<std::string> inputvect = GeneralUtils::ParseToStr(input, ",");

    for (size_t iv_it = 0; iv_it < inputvect.size(); ++iv_it) {
      tn->AddFile(inputvect[iv_it].c_str());
      NUIS_LOG(FIT, "Added input file: " << inputvect[iv_it]);
      if (!first_file.length()) {
        first_file = inputvect[iv_it];
      }
    }
  } else { // The Add form can accept wildcards.
    tn->Add(input.c_str());
    first_file = input;
  }

  if (tn->GetFile() == NULL) {
    tn->Print();
    NUIS_ERR(FTL, "gtree not located in GENIE file: " << input);
    NUIS_ABORT("Check your inputs, they may need to be completely regenerated!");
    throw;
  }

  int nevt = tn->GetEntries();
  if (gNEvents != -999) {
    NUIS_LOG(FIT, "Overriding number of events by user from " << nevt << " to "
        << gNEvents);
    nevt = gNEvents;
  }

  if (!nevt) {
    NUIS_ABORT("Couldn't load any events from input specification: \""
        << input.c_str() << "\"");
  } else {
    NUIS_LOG(FIT, "Found " << nevt << " input entries in " << input);
  }

  StopTalking();
  NtpMCEventRecord *genientpl = NULL;
  tn->SetBranchAddress("gmcrec", &genientpl);
  StartTalking();

  // Make Event and xsec Hist
  TH1D *eventhist = (TH1D *)fluxhist->Clone();
  eventhist->SetDirectory(NULL);
  eventhist->Reset();
  TH1D *xsechist = (TH1D *)eventhist->Clone();
  xsechist->SetDirectory(NULL);

  // Create maps
  std::map<std::string, TH1D *> modexsec;
  std::map<std::string, TH1D *> modecount;
  std::vector<std::string> genieids;
  std::vector<std::string> targetids;
  std::vector<std::string> interids;

  // Loop over all events
  for (int i = 0; i < nevt; i++) {
    tn->GetEntry(i);

    // Hussssch GENIE
    StopTalking();
    // Get the event
    EventRecord &event = *(genientpl->event);
    // Get the neutrino
    GHepParticle *neu = event.Probe();
    StartTalking();

    // Get XSec From Spline
    // Get the GHepRecord
    GHepRecord genie_record = static_cast<GHepRecord>(event);
    double xsec = (genie_record.XSec() / (1E-38 * genie::units::cm2));

    // Parse Interaction String
    std::string mode = genie_record.Summary()->AsString();
    std::vector<std::string> modevec = GeneralUtils::ParseToStr(mode, ";");
    std::string targ = (modevec[0] + ";" + modevec[1]);
    std::string inter = mode;

    // Get target nucleus
    // Alternative ways of getting the summaries
    // GHepParticle *target = genie_record.TargetNucleus();
    // int pdg = target->Pdg();

    // Fill lists of Unique IDS (neutrino and target)
    if (std::find(targetids.begin(), targetids.end(), targ) ==
        targetids.end()) {
      targetids.push_back(targ);
    }

    // The full interaction list
    if (std::find(interids.begin(), interids.end(), inter) == interids.end()) {
      interids.push_back(inter);
    }

    // Create entries Mode Maps
    if (modexsec.find(mode) == modexsec.end()) {
      genieids.push_back(mode);

      modexsec[mode] = (TH1D *)xsechist->Clone();
      modecount[mode] = (TH1D *)xsechist->Clone();

      modexsec[mode]->SetDirectory(NULL);
      modecount[mode]->SetDirectory(NULL);

      modexsec[mode]->GetYaxis()->SetTitle(
          "d#sigma/dE_{#nu} #times 10^{-38} (events weighted by #sigma)");
      modecount[mode]->GetYaxis()->SetTitle("Number of events in file");
    }

    // Fill XSec Histograms
    modexsec[mode]->Fill(neu->E(), xsec);
    modecount[mode]->Fill(neu->E());

    // Fill total event hist
    eventhist->Fill(neu->E());

    if (i % (nevt / 20) == 0) {
      NUIS_LOG(FIT, "Processed "
          << i << "/" << nevt << " GENIE events (E: " << neu->E()
          << " GeV, xsec: " << xsec << " E-38 cm^2/nucleon)");
    }

    // Clear Event
    genientpl->Clear();
  }
  NUIS_LOG(FIT, "Processed all events");

  // Check if we need to correct MEC events before possibly deleting the TChain below
  bool MECcorrect = CheckConfig(std::string(tn->GetFile()->GetName()));

  // Once event loop is done we can start saving stuff into the file
  TFile *outputfile;

  if (!gOutputFile.length()) {
    // Shut the chain;
    delete tn;
    outputfile = new TFile(first_file.c_str(), "UPDATE");
  } else {
    outputfile = new TFile(gOutputFile.c_str(), "RECREATE");
    outputfile->cd();

    NUIS_LOG(FIT, "Cloning input vector to output file: " << gOutputFile);
    TTree *cloneTree = tn->CloneTree(-1, "fast");
    cloneTree->SetDirectory(outputfile);
    cloneTree->Write();

    if (useNOvAWeights){
      // ********************************
      // CLUDGE KLUDGE KLUDGE FOR NOVA
      NUIS_LOG(FIT, "Cloning input nova_wgts to output file: " << gOutputFile);
      //  Also check for the nova_wgts tree from Jeremy
      TChain *nova_chain = new TChain("nova_wgts");
      nova_chain->AddFile(input.c_str());
      TTree *nova_tree = nova_chain->CloneTree(-1, "fast");
      if (!nova_tree) {
	NUIS_LOG(FIT, "Could not find nova_wgts tree in " << input);
      } else {
	NUIS_LOG(FIT, "Found nova_wgts tree in " << input);
	nova_tree->SetDirectory(outputfile);
	nova_tree->Write();
      }
    }
    NUIS_LOG(FIT, "Done cloning tree.");
  }

  NUIS_LOG(FIT, "Getting splines...");

  // Save each of the reconstructed splines to file
  std::map<std::string, TH1D *> modeavg;

  TDirectory *inddir = (TDirectory *)outputfile->Get("IndividualGENIESplines");
  if (!inddir) inddir = (TDirectory *)outputfile->mkdir("IndividualGENIESplines");

  // Loop over GENIE ID's and get MEC count
  int MECcount = 0;
  for (UInt_t i = 0; i < genieids.size(); i++) {
    if (genieids[i].find("MEC") != std::string::npos && genieids[i].find("[CC]") != std::string::npos) {
      MECcount++;
    }
  }

  inddir->cd();
  for (UInt_t i = 0; i < genieids.size(); i++) {
    std::string mode = genieids[i];

    modexsec[mode]->Write((mode + "_summed_xsec").c_str(), TObject::kOverwrite);
    modecount[mode]->Write((mode + "_summed_evt").c_str(), TObject::kOverwrite);

    // Form extra avg xsec map -> Reconstructed spline
    modeavg[mode] = (TH1D *)modexsec[mode]->Clone();
    modeavg[mode]->GetYaxis()->SetTitle(
        "#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/target)");
    modeavg[mode]->Divide(modecount[mode]);

    if (MECcorrect && mode.find("MEC") != std::string::npos && mode.find("[CC]") != std::string::npos) {
      NUIS_LOG(FIT, "Scaling spline " << mode << " by 1/" << MECcount << " because there are " << MECcount << " repeated Nieves 2p2h instances.");
      modeavg[mode]->Scale(1.0 / double(MECcount));
    }

    modeavg[mode]->Write((mode + "_rec_spline").c_str(), TObject::kOverwrite);
  }

  TDirectory *targdir = (TDirectory *)outputfile->Get("TargetGENIESplines");
  if (!targdir)
    targdir = (TDirectory *)outputfile->mkdir("TargetGENIESplines");
  targdir->cd();

  NUIS_LOG(FIT, "Getting Target Splines");
  // For each target save a total spline
  std::map<std::string, TH1D *> targetsplines;

  for (uint i = 0; i < targetids.size(); i++) {
    std::string targ = targetids[i];
    NUIS_LOG(FIT, "Getting target " << i << ": " << targ);
    targetsplines[targ] = (TH1D *)xsechist->Clone();
    targetsplines[targ]->GetYaxis()->SetTitle(
        "#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/target)");
    NUIS_LOG(FIT, "Created target spline for " << targ);

    for (uint j = 0; j < genieids.size(); j++) {
      std::string mode = genieids[j];

      // Look at all matching modes/targets
      if (mode.find(targ) != std::string::npos) {
        NUIS_LOG(FIT, "    Mode " << mode << " contains " << targ << " target");
        targetsplines[targ]->Add(modeavg[mode]);
        NUIS_LOG(FIT,
            "Finished with Mode " << mode << " " << modeavg[mode]->Integral());
      }
    }
    NUIS_LOG(FIT, "Saving target spline: " << targ);
    targetsplines[targ]->Write(("Total_" + targ).c_str(), TObject::kOverwrite);
  }

  NUIS_LOG(FIT, "Getting total splines");
  // Now we have each of the targets we need to create a total cross-section.
  int totalnucl = 0;

  // This has structure target1[fraction1], target2[fraction2]
  std::vector<std::string> targprs = GeneralUtils::ParseToStr(target, ",");

  std::vector<std::string> targ_list;
  std::vector<std::string> frac_list;

  // Chop up the target string which has format
  // TARGET1[fraction1],TARGET2[fraction2]

  // Loop over the vector of strings "TARGET1[fraction1]" "TARGET2[fraction2]"
  for (std::vector<std::string>::iterator it = targprs.begin();
      it != targprs.end(); ++it) {
    // Cut into "TARGET1" and "fraction1]"
    std::vector<std::string> targind = GeneralUtils::ParseToStr(*it, "[");
    // Cut into "TARGET1" and "fraction1"
    for (std::vector<std::string>::iterator jt = targind.begin();
        jt != targind.end(); ++jt) {
      if ((*jt).find("]") != std::string::npos) {
        (*jt) = (*jt).substr(0, (*jt).find("]"));
        frac_list.push_back(*jt);
        // Won't find bracket for target
      } else {
        targ_list.push_back(*jt);
      }
    }
  }

  targprs = targ_list;

  std::vector<double> targ_fractions;
  double minimum = 1.0;
  for (std::vector<std::string>::iterator it = frac_list.begin();
      it != frac_list.end(); it++) {
    double frac = std::atof((*it).c_str());
    targ_fractions.push_back(frac);
    if (frac < minimum)
      minimum = frac;
  }

  std::vector<double>::iterator it = targ_fractions.begin();
  std::vector<std::string>::iterator jt = targ_list.begin();
  double scaling = 0;
  for (; it != targ_fractions.end(); it++, jt++) {
    // First get the mass number from the targ_list
    int nucl = atoi((*jt).c_str());
    nucl = (nucl % 10000) / 10;
    // Gets the relative portions right
    *it = (*it) / minimum;
    // Scale relative the atomic mass
    //(*it) *= (double(nucl)/(*it));
    double tempscaling = double(nucl) / (*it);
    if (tempscaling > scaling)
      scaling = tempscaling;
  }
  it = targ_fractions.begin();
  for (; it != targ_fractions.end(); it++) {
    // Round the scaling to nearest integer and multiply
    *it *= int(scaling + 0.5);
    // Round to nearest integer
    *it = int(*it + 0.5);
    totalnucl += *it;
  }

  // Report on what we've got
  it = targ_fractions.begin();
  jt = targ_list.begin();
  for (; it != targ_fractions.end(); it++, jt++){
    NUIS_LOG(FIT, "Found target " << *jt << " with weight " << *it);
  }

  if (totalnucl == 0) {
    NUIS_ABORT("Didn't find any nucleons in input file. Did you really specify the "
        "target ratios?\ne.g. TARGET1[fraction1],TARGET2[fraction2]");
  }

  TH1D *totalxsec = (TH1D *)xsechist->Clone();

  // Loop over the specified targets by the user
  it = targ_fractions.begin();
  for (uint i = 0; i < targprs.size(); i++, it++) {
    std::string targpdg = targprs[i];
    // Check that we found the user requested target in GENIE
    bool FoundTarget = false;
    for (std::map<std::string, TH1D *>::iterator iter = targetsplines.begin();
        iter != targetsplines.end(); iter++) {
      std::string targstr = iter->first;
      TH1D *xsec = iter->second;

      // Match the user targets to the targets found in GENIE
      if (targstr.find(targpdg) != std::string::npos) {
        FoundTarget = true;

	int nucl = atoi(targpdg.c_str());
	nucl = (nucl % 10000) / 10;

	NUIS_LOG(FIT, "Scaling target " << targstr << " by " << *it << "/" << nucl);
	xsec->Scale(*it/double(nucl));

        NUIS_LOG(FIT, "Adding target spline "
            << targstr << " Integral = " << xsec->Integral("width"));
        totalxsec->Add(xsec);
      }
    } // Looped over target splines

    // Check that targets were all found
    if (!FoundTarget) {
      NUIS_ERR(WRN, "Didn't find target "
          << targpdg
          << " in the list of targets recorded by GENIE");
      NUIS_ERR(WRN, "  The list of targets you requested is: ");
      for (uint i = 0; i < targprs.size(); ++i)
        NUIS_ERR(WRN, "    " << targprs[i]);
      NUIS_ERR(WRN, "  The list of targets found in GENIE is: ");
      for (std::map<std::string, TH1D *>::iterator iter = targetsplines.begin();
          iter != targetsplines.end(); iter++)
        NUIS_ERR(WRN, "    " << iter->first);
    }
  }

  outputfile->cd();
  totalxsec->GetYaxis()->SetTitle(
      "#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/nucleon)");
  totalxsec->Write("nuisance_xsec", TObject::kOverwrite);

  eventhist = (TH1D *)fluxhist->Clone();
  eventhist->Multiply(totalxsec);
  eventhist->GetYaxis()->SetTitle(
      (std::string("Event rate (N = #sigma #times #Phi) #times 10^{-38} "
                   "(cm^{2}/nucleon) #times ") +
       eventhist->GetYaxis()->GetTitle())
      .c_str());

  NUIS_LOG(FIT, "Dividing by Total Nucl = " << totalnucl);
  eventhist->Scale(1.0 / double(totalnucl));

  eventhist->Write("nuisance_events", TObject::kOverwrite);
  fluxhist->Write("nuisance_flux", TObject::kOverwrite);

  NUIS_LOG(FIT, "Inclusive XSec Per Nucleon = " << eventhist->Integral("width") *
      1E-38 /
      fluxhist->Integral("width"));
  NUIS_LOG(FIT, "XSec Hist Integral = " << totalxsec->Integral());

  outputfile->Close();

  return;
};

void PrintOptions() {
  std::cout << "PrepareGENIE events NUISANCE app. " << std::endl
    << "Takes GHep Outputs and prepares events for NUISANCE."
    << std::endl
    << std::endl
    << "PrepareGENIE [-h,-help,--h,--help] [-i "
    "inputfile1.root,inputfile2.root,inputfile3.root,...] "
    << "[-f flux_root_file.root,flux_hist_name] [-t "
    "target1[frac1],target2[frac2],...]"
    << "[-n number_of_events (experimental)]" << std::endl
    << std::endl;

  std::cout << "Prepare Mode [Default] : Takes a single GHep file, "
    "reconstructs the original GENIE splines, "
    << " and creates a duplicate file that also contains the flux, "
    "event rate, and xsec predictions that NUISANCE needs. "
    << std::endl;
  std::cout << "Following options are required for Prepare Mode:" << std::endl;
  std::cout << " [ -i inputfile.root  ] : Reads in a single GHep input file "
    "that needs the xsec calculation ran on it. "
    << std::endl;
  std::cout << " [ -f flux_file.root,hist_name ] : Path to root file "
    "containing the flux histogram the GHep records were generated "
    "with."
    << " A simple method is to point this to the flux histogram genie "
    "generatrs '-f /path/to/events/input-flux.root,spectrum'. "
    << std::endl;
  std::cout << " [ -f elow,ehigh,estep ] : Energy range specification when no "
    "flux file was used."
    << std::endl;
  std::cout << " [ -t target ] : Target that GHepRecords were generated with. "
    "Comma separated list with fractions. E.g. for CH2 "
    "target=1000060120[0.923076],1000010010[0.076924]"
    << std::endl;
  std::cout << " [ -o outputfile.root ] : File to write prepared input file to."
    << std::endl;
  std::cout << " [ -m Mono_E_nu_GeV ] : Run in mono-energetic mode with m GeV "
    "neutrino energy."
    << std::endl;
  std::cout << " [ -n number_of_evt ] : Run with a reduced number of events "
    "for debugging purposes"
    << std::endl;
}

void ParseOptions(int argc, char *argv[]) {
  bool flagopt = false;

  int verbocount = 0;
  int errorcount = 0;
  verbocount += Config::GetParI("VERBOSITY");
  errorcount += Config::GetParI("ERROR");
  bool trace = Config::GetParB("TRACE");
  std::cout << "[ NUISANCE ]: Setting VERBOSITY=" << verbocount << std::endl;
  std::cout << "[ NUISANCE ]: Setting ERROR=" << errorcount << std::endl;
  SETVERBOSITY(verbocount);
  SETTRACE(trace);


  // If No Arguments print commands
  for (int i = 1; i < argc; ++i) {
    if (!std::strcmp(argv[i], "-h")) {
      flagopt = true;
      break;
    }
    if (i + 1 != argc) {
      // Cardfile
      if (!std::strcmp(argv[i], "-h")) {
        flagopt = true;
        break;
      } else if (!std::strcmp(argv[i], "-i")) {
        gInputFiles = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-o")) {
        gOutputFile = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-f")) {
        gFluxFile = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-t")) {
        gTarget = argv[i + 1];
        ++i;
      } else if (!std::strcmp(argv[i], "-n")) {
        gNEvents = GeneralUtils::StrToInt(argv[i + 1]);
        ++i;
      } else if (!std::strcmp(argv[i], "-m")) {
        MonoEnergy = GeneralUtils::StrToDbl(argv[i + 1]);
        IsMonoE = true;
        ++i;
      } else {
        NUIS_ERR(FTL, "ERROR: unknown command line option given! - '"
            << argv[i] << " " << argv[i + 1] << "'");
        PrintOptions();
        break;
      }
    }
  }

  if (gInputFiles == "" && !flagopt) {
    NUIS_ERR(FTL, "No input file(s) specified!");
    flagopt = true;
  }

  if (gFluxFile == "" && !flagopt && !IsMonoE) {
    NUIS_ERR(FTL, "No flux input specified for Prepare Mode");
    flagopt = true;
  }

  if (gTarget == "" && !flagopt) {
    NUIS_ERR(FTL, "No target specified for Prepare Mode");
    flagopt = true;
  }

  if (gTarget.find("[") == std::string::npos ||
      gTarget.find("]") == std::string::npos) {
    NUIS_ERR(FTL, "Didn't specify target ratios in Prepare Mode");
    NUIS_ERR(FTL, "Are you sure you gave it as -t "
        "\"TARGET1[fraction1],TARGET2[fraction]\"?");
    flagopt = true;
  }

  if (argc < 1 || flagopt) {
    PrintOptions();
    exit(-1);
  }

  return;
}
