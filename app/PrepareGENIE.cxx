#include <stdio.h>
#include <stdlib.h>
#include "FitLogger.h"
#include "PlotUtils.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

#ifdef __GENIE_ENABLED__
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

void PrintOptions();
void ParseOptions(int argc, char* argv[]);
void RunGENIEPrepareMono(std::string input, std::string target,
                         std::string output);
void RunGENIEPrepare(std::string input, std::string flux, std::string target,
                     std::string output);

int main(int argc, char* argv[]) {
  ParseOptions(argc, argv);
  if (IsMonoE) {
    RunGENIEPrepareMono(gInputFiles, gTarget, gOutputFile);
  } else {
    RunGENIEPrepare(gInputFiles, gFluxFile, gTarget, gOutputFile);
  }
}

void RunGENIEPrepareMono(std::string input, std::string target,
                         std::string output) {

  LOG(FIT) << "Running GENIE Prepare in mono energetic with E = " << MonoEnergy << " GeV" << std::endl;
  // Setup TTree
  TChain* tn = new TChain("gtree");
  tn->AddFile(input.c_str());

  int nevt = tn->GetEntries();
  if (gNEvents != -999) {
    LOG(FIT) << "Overriding number of events by user from " << nevt << " to " << gNEvents << std::endl;
    nevt = gNEvents;
  }
  NtpMCEventRecord* genientpl = NULL;
  tn->SetBranchAddress("gmcrec", &genientpl);

  // Have the TH1D go from MonoEnergy/2 to MonoEnergy/2
  TH1D* fluxhist = new TH1D("flux", "flux", 1000, MonoEnergy/2., MonoEnergy*2.);
  fluxhist->Fill(MonoEnergy);
  fluxhist->Scale(1, "width");

  // Make Event Hist
  TH1D* eventhist = (TH1D*)fluxhist->Clone();
  eventhist->Reset();

  TH1D* xsechist = (TH1D*)eventhist->Clone();

  // Create maps
  std::map<std::string, TH1D*> modexsec;
  std::map<std::string, TH1D*> modecount;
  std::vector<std::string> genieids;
  std::vector<std::string> targetids;
  std::vector<std::string> interids;

  // Loop over all events
  for (int i = 0; i < nevt; i++) {
    tn->GetEntry(i);

    StopTalking();
    EventRecord& event = *(genientpl->event);
    GHepParticle* neu = event.Probe();
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

      modexsec[mode] = (TH1D*)xsechist->Clone();
      modecount[mode] = (TH1D*)xsechist->Clone();

      modexsec[mode]->GetYaxis()->SetTitle("d#sigma/dE_{#nu} #times 10^{-38} (events weighted by #sigma)");
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
      LOG(FIT) << "Processed " << i << "/" << nevt
               << " GENIE events (E: " << neu->E()
               << " GeV, xsec: " << xsec << " E-38 cm^2/nucleon)" << std::endl;
    }
  }
  LOG(FIT) << "Processed all events" << std::endl;

  TFile* outputfile;

  // If no output is specified just append to the file
  if (!gOutputFile.length()) {
    tn->GetEntry(0);
    outputfile = tn->GetFile();
    outputfile->cd();
  } else {
    outputfile = new TFile(gOutputFile.c_str(), "RECREATE");
    outputfile->cd();

    QLOG(FIT, "Cloning input vector to output file: " << gOutputFile);
    TTree* cloneTree = tn->CloneTree();
    cloneTree->SetDirectory(outputfile);
    cloneTree->Write();

    QLOG(FIT, "Cloning input nova_wgts to output file: " << gOutputFile);
    // ***********************************
    // ***********************************
    // FUDGE FOR NOVA MINERVA WORKSHOP
    //  Also check for the nova_wgts tree from Jeremy
    TChain *nova_chain = new TChain("nova_wgts");
    nova_chain->AddFile(input.c_str());
    TTree* nova_tree = nova_chain->GetTree();
    if (!nova_tree) {
      QLOG(FIT, "Could not find nova_wgts tree in " << gOutputFile);
    } else {
      QLOG(FIT, "Found nova_wgts tree in " << gOutputFile);
    }
    if (nova_tree) {
      nova_tree->SetDirectory(outputfile);
      nova_tree->Write();
    }

    QLOG(FIT, "Done cloning tree.");
  }

  LOG(FIT) << "Getting splines in mono-energetic..." << std::endl;

  // Save each of the reconstructed splines to file
  std::map<std::string, TH1D*> modeavg;

  TDirectory* inddir = (TDirectory*)outputfile->Get("IndividualGENIESplines");
  if (!inddir) inddir = (TDirectory*)outputfile->mkdir("IndividualGENIESplines");
  inddir->cd();

  // Loop over GENIE ID's and get MEC count
  int MECcount = 0;
  bool MECcorrect = FitPar::Config().GetParB("CorrectGENIEMECNorm");
  for (UInt_t i = 0; i < genieids.size(); i++) {
    if (genieids[i].find("MEC") != std::string::npos) {
      MECcount++;
    }
  }
  LOG(FIT) << "Found " << MECcount << " repeated MEC instances." << std::endl;

  for (UInt_t i = 0; i < genieids.size(); i++) {
    std::string mode = genieids[i];

    modexsec[mode]->Write((mode + "_summed_xsec").c_str(), TObject::kOverwrite);
    modecount[mode]->Write((mode + "_summed_evt").c_str(), TObject::kOverwrite);

    // Form extra avg xsec map -> Reconstructed spline
    modeavg[mode] = (TH1D*)modexsec[mode]->Clone();
    modeavg[mode]->GetYaxis()->SetTitle("#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/target)");
    modeavg[mode]->Divide(modecount[mode]);

    if (MECcorrect && (mode.find("MEC") != std::string::npos)) {
      modeavg[mode]->Scale(1.0 / double(MECcount));
    }

    modeavg[mode]->Write((mode + "_rec_spline").c_str(), TObject::kOverwrite);
  }

  TDirectory* targdir = (TDirectory*)outputfile->Get("TargetGENIESplines");
  if (!targdir) targdir = (TDirectory*)outputfile->mkdir("TargetGENIESplines");
  targdir->cd();

  LOG(FIT) << "Getting Target Splines" << std::endl;

  // For each target save a total spline
  std::map<std::string, TH1D*> targetsplines;

  for (uint i = 0; i < targetids.size(); i++) {
    std::string targ = targetids[i];
    LOG(FIT) << "Getting target " << i << ": " << targ << std::endl;
    targetsplines[targ] = (TH1D*)xsechist->Clone();
    targetsplines[targ]->GetYaxis()->SetTitle("#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/target)");
    LOG(FIT) << "Created target spline for " << targ << std::endl;

    for (uint j = 0; j < genieids.size(); j++) {
      std::string mode = genieids[j];

      if (mode.find(targ) != std::string::npos) {
        LOG(FIT) << "    Mode " << mode << " contains " << targ << " target" << std::endl;
        targetsplines[targ]->Add(modeavg[mode]);
        LOG(FIT) << "Finished with Mode " << mode << " " << modeavg[mode]->Integral() << std::endl;
      }
    }

    LOG(FIT) << "Saving target spline:" << targ << std::endl;
    targetsplines[targ]->Write(("Total_" + targ).c_str(), TObject::kOverwrite);
  }

  LOG(FIT) << "Getting total splines" << std::endl;
  // Now we have each of the targets we need to create a total cross-section.
  int totalnucl = 0;
  // Get the targets specified by the user, separated by commas
  std::vector<std::string> targprs = GeneralUtils::ParseToStr(target, ",");
  TH1D* totalxsec = (TH1D*)xsechist->Clone();

  for (uint i = 0; i < targprs.size(); i++) {
    std::string targpdg = targprs[i];
    // Check that we found the user requested target in GENIE
    bool FoundTarget = false;

    for (std::map<std::string, TH1D*>::iterator iter = targetsplines.begin();
         iter != targetsplines.end(); iter++) {
      std::string targstr = iter->first;
      TH1D* xsec = iter->second;

      if (targstr.find(targpdg) != std::string::npos) {
        FoundTarget = true;
        LOG(FIT) << "Adding target spline " << targstr << " Integral = " << xsec->Integral("width") << std::endl;
        totalxsec->Add(xsec);

        int nucl = atoi(targpdg.c_str());
        totalnucl += int((nucl % 10000) / 10);
      } 
    }

    // Check that targets were all found
    if (!FoundTarget) {
      ERR(WRN) << "Didn't find target " << targpdg << " in the list of targets recorded by GENIE" << std::endl;
      ERR(WRN) << "  The list of targets you requested is: " << std::endl;
      for (uint i = 0; i < targprs.size(); ++i) ERR(WRN) << "    " << targprs[i] << std::endl;
      ERR(WRN) << "  The list of targets found in GENIE is: " << std::endl;
      for (std::map<std::string, TH1D*>::iterator iter = targetsplines.begin(); iter != targetsplines.end(); iter++) ERR(WRN) << "    " << iter->first<< std::endl;
    }
  }

  outputfile->cd();
  totalxsec->GetYaxis()->SetTitle("#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/nucleon)");
  totalxsec->Write("nuisance_xsec", TObject::kOverwrite);

  eventhist = (TH1D*)fluxhist->Clone();
  eventhist->Multiply(totalxsec);
  eventhist->GetYaxis()->SetTitle((std::string("Event rate (N = #sigma #times #Phi) #times 10^{-38} (cm^{2}/nucleon) #times ")+eventhist->GetYaxis()->GetTitle()).c_str());

  LOG(FIT) << "Dividing by Total Nucl = " << totalnucl << std::endl;
  eventhist->Scale(1.0 / double(totalnucl));

  eventhist->Write("nuisance_events", TObject::kOverwrite);
  fluxhist->Write("nuisance_flux", TObject::kOverwrite);

  LOG(FIT) << "Inclusive XSec Per Nucleon = " << eventhist->Integral("width") * 1E-38 / fluxhist->Integral("width") << std::endl;
  LOG(FIT) << "XSec Hist Integral = " << totalxsec->Integral("width") << std::endl;

  outputfile->Close();

  return;
}

void RunGENIEPrepare(std::string input, std::string flux, std::string target,
                     std::string output) {
  LOG(FIT) << "Running GENIE Prepare with flux..." << std::endl;

  // Get Flux Hist
  std::vector<std::string> fluxvect = GeneralUtils::ParseToStr(flux, ",");
  TH1* fluxhist = NULL;
  if (fluxvect.size() == 3) {
    double from = GeneralUtils::StrToDbl(fluxvect[0]);
    double to = GeneralUtils::StrToDbl(fluxvect[1]);
    double step = GeneralUtils::StrToDbl(fluxvect[2]);

    int nstep = ceil((to - from) / step);
    to = from + step * nstep;

    QLOG(FIT, "Generating flat flux histogram from "
                  << from << " to " << to << " with bins " << step
                  << " wide (NBins = " << nstep << ").");

    fluxhist = new TH1D("spectrum", ";E_{#nu} (GeV);Count (A.U.)", nstep, from, to);

    for (Int_t bi_it = 1; bi_it < fluxhist->GetXaxis()->GetNbins(); ++bi_it) {
      fluxhist->SetBinContent(bi_it, 1.0 / double(step * nstep));
    }
    fluxhist->SetDirectory(0);
  } else if (fluxvect.size() == 2) {
    TFile* fluxfile = new TFile(fluxvect[0].c_str(), "READ");
    if (!fluxfile->IsZombie()) {
      fluxhist = dynamic_cast<TH1D*>(fluxfile->Get(fluxvect[1].c_str()));
      if (!fluxhist) {
        ERR(FTL) << "Couldn't find histogram named: \"" << fluxvect[1]
                 << "\" in file: \"" << fluxvect[0] << std::endl;
        throw;
      }
      fluxhist->SetDirectory(0);
    }
  } else if (fluxvect.size() == 1) {
    MonoEnergy = GeneralUtils::StrToDbl(fluxvect[0]);
    RunGENIEPrepareMono(input, target, output);
    return;
  } else {
    LOG(FTL) << "Bad flux specification: \"" << flux << "\"." << std::endl;
    throw;
  }

  // Setup TTree
  TChain* tn = new TChain("gtree");

  if (input.find_first_of(',') != std::string::npos) {
    std::vector<std::string> inputvect = GeneralUtils::ParseToStr(input, ",");

    for (size_t iv_it = 0; iv_it < inputvect.size(); ++iv_it) {
      tn->AddFile(inputvect[iv_it].c_str());
      QLOG(FIT, "Added input file: " << inputvect[iv_it]);
    }
  } else {  // The Add form can accept wildcards.
    tn->Add(input.c_str());
  }

  int nevt = tn->GetEntries();
  if (gNEvents != -999) {
    LOG(FIT) << "Overriding number of events by user from " << nevt << " to " << gNEvents << std::endl;
    nevt = gNEvents;
  }

  if (!nevt) {
    THROW("Couldn't load any events from input specification: \""
          << input.c_str() << "\"");
  } else {
    QLOG(FIT, "Found " << nevt << " input entries in " << input);
  }

  NtpMCEventRecord* genientpl = NULL;
  tn->SetBranchAddress("gmcrec", &genientpl);

  // Make Event and xsec Hist
  TH1D* eventhist = (TH1D*)fluxhist->Clone();
  eventhist->Reset();
  TH1D* xsechist = (TH1D*)eventhist->Clone();

  // Create maps
  std::map<std::string, TH1D*> modexsec;
  std::map<std::string, TH1D*> modecount;
  std::vector<std::string> genieids;
  std::vector<std::string> targetids;
  std::vector<std::string> interids;

  // Loop over all events
  for (int i = 0; i < nevt; i++) {
    tn->GetEntry(i);

    // Hussssch GENIE
    StopTalking();
    // Get the event
    EventRecord& event = *(genientpl->event);
    // Get the neutrino
    GHepParticle* neu = event.Probe();
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
    //GHepParticle *target = genie_record.TargetNucleus();
    //int pdg = target->Pdg();

    // Fill lists of Unique IDS (neutrino and target)
    if (std::find(targetids.begin(), targetids.end(), targ) == targetids.end()) {
      targetids.push_back(targ);
    }

    // The full interaction list
    if (std::find(interids.begin(), interids.end(), inter) == interids.end()) {
      interids.push_back(inter);
    }

    // Create entries Mode Maps
    if (modexsec.find(mode) == modexsec.end()) {
      genieids.push_back(mode);

      modexsec[mode] = (TH1D*)xsechist->Clone();
      modecount[mode] = (TH1D*)xsechist->Clone();

      modexsec[mode]->GetYaxis()->SetTitle("d#sigma/dE_{#nu} #times 10^{-38} (events weighted by #sigma)");
      modecount[mode]->GetYaxis()->SetTitle("Number of events in file");
    }

    // Fill XSec Histograms
    modexsec[mode]->Fill(neu->E(), xsec);
    modecount[mode]->Fill(neu->E());

    // Fill total event hist
    eventhist->Fill(neu->E());

    if (i % (nevt / 20) == 0) {
      LOG(FIT) << "Processed " << i << "/" << nevt
               << " GENIE events (E: " << neu->E()
               << " GeV, xsec: " << xsec << " E-38 cm^2/nucleon)" << std::endl;
    }

    // Clear Event
    genientpl->Clear();
  }
  LOG(FIT) << "Processed all events" << std::endl;

  // Once event loop is done we can start saving stuff into the file

  TFile* outputfile;

  if (!gOutputFile.length()) {
    tn->GetEntry(0);
    outputfile = tn->GetFile();
    outputfile->cd();
  } else {
    outputfile = new TFile(gOutputFile.c_str(), "RECREATE");
    outputfile->cd();

    QLOG(FIT, "Cloning input vector to output file: " << gOutputFile);
    TTree* cloneTree = tn->CloneTree();
    cloneTree->SetDirectory(outputfile);
    cloneTree->Write();

    // ********************************
    // CLUDGE KLUDGE KLUDGE FOR NOVA
    QLOG(FIT, "Cloning input nova_wgts to output file: " << gOutputFile);
    //  Also check for the nova_wgts tree from Jeremy
    TChain *nova_chain = new TChain("nova_wgts");
    nova_chain->AddFile(input.c_str());
    TTree* nova_tree = nova_chain->CloneTree();
    if (!nova_tree) {
      QLOG(FIT, "Could not find nova_wgts tree in " << input);
    } else {
      QLOG(FIT, "Found nova_wgts tree in " << input);
      nova_tree->SetDirectory(outputfile);
      nova_tree->Write();
    }
    QLOG(FIT, "Done cloning tree.");
  }

  LOG(FIT) << "Getting splines..." << std::endl;

  // Save each of the reconstructed splines to file
  std::map<std::string, TH1D*> modeavg;

  TDirectory* inddir = (TDirectory*)outputfile->Get("IndividualGENIESplines");
  if (!inddir) inddir = (TDirectory*)outputfile->mkdir("IndividualGENIESplines");
  inddir->cd();

  // Loop over GENIE ID's and get MEC count
  int MECcount = 0;
  bool MECcorrect = FitPar::Config().GetParB("CorrectGENIEMECNorm");
  for (UInt_t i = 0; i < genieids.size(); i++) {
    if (genieids[i].find("MEC") != std::string::npos) {
      MECcount++;
    }
  }
  LOG(FIT) << "Found " << MECcount << " repeated MEC instances." << std::endl;

  for (UInt_t i = 0; i < genieids.size(); i++) {
    std::string mode = genieids[i];

    modexsec[mode]->Write((mode + "_summed_xsec").c_str(), TObject::kOverwrite);
    modecount[mode]->Write((mode + "_summed_evt").c_str(), TObject::kOverwrite);

    // Form extra avg xsec map -> Reconstructed spline
    modeavg[mode] = (TH1D*)modexsec[mode]->Clone();
    modeavg[mode]->GetYaxis()->SetTitle("#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/target)");
    modeavg[mode]->Divide(modecount[mode]);

    if (MECcorrect && (mode.find("MEC") != std::string::npos)) {
      modeavg[mode]->Scale(1.0 / double(MECcount));
    }

    modeavg[mode]->Write((mode + "_rec_spline").c_str(), TObject::kOverwrite);
  }

  TDirectory* targdir = (TDirectory*)outputfile->Get("TargetGENIESplines");
  if (!targdir) targdir = (TDirectory*)outputfile->mkdir("TargetGENIESplines");
  targdir->cd();

  LOG(FIT) << "Getting Target Splines" << std::endl;
  // For each target save a total spline
  std::map<std::string, TH1D*> targetsplines;

  for (uint i = 0; i < targetids.size(); i++) {
    std::string targ = targetids[i];
    LOG(FIT) << "Getting target " << i << ": " << targ << std::endl;
    targetsplines[targ] = (TH1D*)xsechist->Clone();
    targetsplines[targ]->GetYaxis()->SetTitle("#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/target)");
    LOG(FIT) << "Created target spline for " << targ << std::endl;

    for (uint j = 0; j < genieids.size(); j++) {
      std::string mode = genieids[j];

      // Look at all matching modes/targets
      if (mode.find(targ) != std::string::npos) {
        LOG(FIT) << "    Mode " << mode << " contains " << targ << " target" << std::endl;
        targetsplines[targ]->Add(modeavg[mode]);
        LOG(FIT) << "Finished with Mode " << mode << " " << modeavg[mode]->Integral() << std::endl;
      }
    }
    LOG(FIT) << "Saving target spline: " << targ << std::endl;
    targetsplines[targ]->Write(("Total_" + targ).c_str(), TObject::kOverwrite);
  }

  LOG(FIT) << "Getting total splines" << std::endl;
  // Now we have each of the targets we need to create a total cross-section.
  int totalnucl = 0;

  // This has structure target1[fraction1], target2[fraction2]
  std::vector<std::string> targprs = GeneralUtils::ParseToStr(target, ",");

  std::vector<std::string> targ_list;
  std::vector<std::string> frac_list;

  // Chop up the target string which has format TARGET1[fraction1],TARGET2[fraction2]

  std::cout << "Targets: " << std::endl;
  // Loop over the vector of strings "TARGET1[fraction1]" "TARGET2[fraction2]"
  for (std::vector<std::string>::iterator it = targprs.begin(); it != targprs.end(); ++it) {
    // Cut into "TARGET1" and "fraction1]"
    std::vector<std::string> targind = GeneralUtils::ParseToStr(*it, "[");
    std::cout << "  " << *it << std::endl;
    // Cut into "TARGET1" and "fraction1"
    for (std::vector<std::string>::iterator jt = targind.begin(); jt != targind.end(); ++jt) {
      if ((*jt).find("]") != std::string::npos) {
        (*jt) = (*jt).substr(0, (*jt).find("]"));
        //*jt = "hello";
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
  for (std::vector<std::string>::iterator it = frac_list.begin(); it != frac_list.end(); it++) {
    std::cout << "  " << *it << std::endl;
    double frac = std::atof((*it).c_str());
    targ_fractions.push_back(frac);
    if (frac < minimum) minimum = frac;
  }

  std::vector<double>::iterator it = targ_fractions.begin();
  std::vector<std::string>::iterator jt = targ_list.begin();
  double scaling = 0;
  for (; it != targ_fractions.end(); it++, jt++) {
    // First get the mass number from the targ_list
    int nucl = atoi((*jt).c_str());
    nucl = (nucl%10000)/10;
    // Gets the relative portions right
    *it = (*it)/minimum;
    // Scale relative the atomic mass
    //(*it) *= (double(nucl)/(*it));
    double tempscaling = double(nucl)/(*it);
    if (tempscaling > scaling) scaling=tempscaling;
  }
  it = targ_fractions.begin();
  for (; it != targ_fractions.end(); it++) {
    // Round the scaling to nearest integer and multiply
    *it *= int(scaling+0.5);
    // Round to nearest integer
    *it = int(*it+0.5);
    totalnucl += *it;
  }

  TH1D* totalxsec = (TH1D*)xsechist->Clone();

  // Loop over the specified targets by the user
  for (uint i = 0; i < targprs.size(); i++) {
    std::string targpdg = targprs[i];
    // Check that we found the user requested target in GENIE
    bool FoundTarget = false;

    for (std::map<std::string, TH1D*>::iterator iter = targetsplines.begin(); iter != targetsplines.end(); iter++) {
      std::string targstr = iter->first;
      TH1D* xsec = iter->second;

      // Match the user targets to the targets found in GENIE
      if (targstr.find(targpdg) != std::string::npos) {
        FoundTarget = true;
        LOG(FIT) << "Adding target spline " << targstr << " Integral = " << xsec->Integral("width") << std::endl;
        totalxsec->Add(xsec);

        //int nucl = atoi(targpdg.c_str());
        //totalnucl += int((nucl % 10000) / 10);
      } 
    } // Looped over target splines

    // Check that targets were all found
    if (!FoundTarget) {
      ERR(WRN) << "Didn't find target " << targpdg << " in the list of targets recorded by GENIE" << std::endl;
      ERR(WRN) << "  The list of targets you requested is: " << std::endl;
      for (uint i = 0; i < targprs.size(); ++i) ERR(WRN) << "    " << targprs[i] << std::endl;
      ERR(WRN) << "  The list of targets found in GENIE is: " << std::endl;
      for (std::map<std::string, TH1D*>::iterator iter = targetsplines.begin(); iter != targetsplines.end(); iter++) ERR(WRN) << "    " << iter->first<< std::endl;
    }
  }

  outputfile->cd();
  totalxsec->GetYaxis()->SetTitle("#sigma (E_{#nu}) #times 10^{-38} (cm^{2}/nucleon)");
  totalxsec->Write("nuisance_xsec", TObject::kOverwrite);

  eventhist = (TH1D*)fluxhist->Clone();
  eventhist->Multiply(totalxsec);
  eventhist->GetYaxis()->SetTitle((std::string("Event rate (N = #sigma #times #Phi) #times 10^{-38} (cm^{2}/nucleon) #times ")+eventhist->GetYaxis()->GetTitle()).c_str());

  LOG(FIT) << "Dividing by Total Nucl = " << totalnucl << std::endl;
  eventhist->Scale(1.0 / double(totalnucl));

  eventhist->Write("nuisance_events", TObject::kOverwrite);
  fluxhist->Write("nuisance_flux", TObject::kOverwrite);

  LOG(FIT) << "Inclusive XSec Per Nucleon = " << eventhist->Integral("width") * 1E-38 / fluxhist->Integral("width") << std::endl;
  LOG(FIT) << "XSec Hist Integral = " << totalxsec->Integral() << std::endl;

  outputfile->Close();

  return;
};

void PrintOptions() {
  std::cout << "PrepareGENIEEvents NUISANCE app. " << std::endl
    << "Takes GHep Outputs and prepares events for NUISANCE."
    << std::endl
    << std::endl
    << "PrepareGENIE [-h,-help,--h,--help] [-i "
    "inputfile1.root,inputfile2.root,inputfile3.root,...] "
    << "[-f flux_root_file.root,flux_hist_name] [-t "
    "target1[frac1],target2[frac2],...]"
    << "[-n number_of_events (experimental)]"
    << std::endl
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
    "Comma seperated list. E.g. for CH2 "
    "target=1000060120,1000010010,1000010010"
    << std::endl;
  std::cout << " [ -o outputfile.root ] : File to write prepared input file to."
    << std::endl;
  std::cout << " [ -m Mono_E_nu_GeV ] : Run in mono-energetic mode with m GeV neutrino energy."
    << std::endl;
  std::cout << " [ -n number_of_evt ] : Run with a reduced number of events for debugging purposes"
    << std::endl;
}

void ParseOptions(int argc, char* argv[]) {
  bool flagopt = false;

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
        ERR(FTL) << "ERROR: unknown command line option given! - '" << argv[i]
          << " " << argv[i + 1] << "'" << std::endl;
        PrintOptions();
        break;
      }
    }
  }

  if (gInputFiles == "" && !flagopt) {
    ERR(FTL) << "No input file(s) specified!" << std::endl;
    flagopt = true;
  }

  if (gFluxFile == "" && !flagopt && !IsMonoE) {
    ERR(FTL) << "No flux input specified for Prepare Mode" << std::endl;
    flagopt = true;
  }

  if (gTarget == "" && !flagopt) {
    ERR(FTL) << "No target specified for Prepare Mode" << std::endl;
    flagopt = true;
  }

  if (argc < 1 || flagopt) {
    PrintOptions();
    exit(-1);
  }

  return;
}
