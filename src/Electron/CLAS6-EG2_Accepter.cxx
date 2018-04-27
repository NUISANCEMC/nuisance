#include "ISmearcepter.h"

#include "TH3D.h"
#include "TRandom3.h"

#include <cstdlib>

// #define DEBUG_CLASACCEPT 1

struct EffMap {
  TH3D *Generated;
  TH3D *Accepted;

  void Build(TFile *inpF, std::string const &GenName,
             std::string const &AccName) {
    Generated = dynamic_cast<TH3D *>(inpF->Get(GenName.c_str()));
    Accepted = dynamic_cast<TH3D *>(inpF->Get(AccName.c_str()));

    if (!Generated) {
      std::cout << "[ERROR]: Could not retrieve \"accepted\" histogram: \""
                << AccName << "\" from file: \"" << inpF->GetName() << "\"."
                << std::endl;
      exit(1);
    }
    if (!Accepted) {
      std::cout << "[ERROR]: Could not retrieve \"generated\" histogram: \""
                << GenName << "\" from file: \"" << inpF->GetName() << "\"."
                << std::endl;
      exit(1);
    }

    Generated = static_cast<TH3D *>(Generated->Clone());
    Generated->SetDirectory(NULL);
    Accepted = static_cast<TH3D *>(Accepted->Clone());
    Accepted->SetDirectory(NULL);
  }

  double GetAccRatio(double p_GeV, double costheta, double phi_deg,
                     double defaultAccRatio = 0) const {
    // For a bin in phase space defined by p, cost, phi:
    // Find number of generated events
    Int_t pbin = Generated->GetXaxis()->FindBin(p_GeV);

    Int_t tbin = Generated->GetYaxis()->FindBin(costheta);
    Int_t phibin = Generated->GetZaxis()->FindBin(phi_deg);

    if (((pbin == 0) || (pbin == (Generated->GetXaxis()->GetNbins() + 1))) ||
        ((tbin == 0) || (tbin == (Generated->GetYaxis()->GetNbins() + 1))) ||
        ((phibin == 0) ||
         (phibin == (Generated->GetZaxis()->GetNbins() + 1)))) {
      return 0;
    }

    double num_gen = Generated->GetBinContent(pbin, tbin, phibin);
    if (num_gen == 0) {
      return defaultAccRatio;
    }
    // Find number of accepted events
    pbin = Accepted->GetXaxis()->FindBin(p_GeV);
    tbin = Accepted->GetYaxis()->FindBin(costheta);
    phibin = Accepted->GetZaxis()->FindBin(phi_deg);
    double num_acc = Accepted->GetBinContent(pbin, tbin, phibin);
    double acc_ratio = double(num_acc) / double(num_gen);

    if (((pbin == 0) || (pbin == (Accepted->GetXaxis()->GetNbins() + 1))) ||
        ((tbin == 0) || (tbin == (Accepted->GetYaxis()->GetNbins() + 1))) ||
        ((phibin == 0) || (phibin == (Accepted->GetZaxis()->GetNbins() + 1)))) {
      return 0;
    }

    if ((acc_ratio != 0 && !std::isnormal(acc_ratio)) || (acc_ratio > 1)) {
      std::cout << "[BINS]: p " << Generated->GetXaxis()->GetBinLowEdge(1)
                << " -- "
                << Generated->GetXaxis()->GetBinUpEdge(
                       Generated->GetXaxis()->GetNbins())
                << ", cost " << Generated->GetYaxis()->GetBinLowEdge(1)
                << " -- "
                << Generated->GetYaxis()->GetBinUpEdge(
                       Generated->GetYaxis()->GetNbins())
                << ", phi " << Generated->GetZaxis()->GetBinLowEdge(1) << " -- "
                << Generated->GetZaxis()->GetBinUpEdge(
                       Generated->GetZaxis()->GetNbins())
                << ". " << std::endl
                << "[ERROR]: Bad acceptance ratio: " << acc_ratio << " = "
                << num_acc << " / " << num_gen << ". (" << p_GeV << ", "
                << costheta << ", " << phi_deg << ")." << std::endl;
      exit(1);
    }
    return acc_ratio;
  }
};

class CLASAccepter : public ISmearcepter {
  TRandom3 rand;
  // Maps a particle PDG to the relevant generated and accepted histograms from
  // the input map.
  std::map<int, EffMap> Acceptance;
  double DefaultAccRatio;

public:
  CLASAccepter() :  DefaultAccRatio(0) { ElementName = "CLASAccepter"; }

  void SpecifcSetup(nuiskey &nk) {
    rand.~TRandom3();
    new (&rand) TRandom3();

    InstanceName = nk.GetS("name");
    DefaultAccRatio = nk.GetD("DefaultAccRatio");

    std::string const &mapfile = nk.GetS("map");

    if (!mapfile.length()) {
      std::cout << "[ERROR]: No input file specified by \"map\" attribute."
                << std::endl;
      exit(1);
    }

    TFile *f = new TFile(mapfile.c_str());

    if (!f || !f->IsOpen()) {
      std::cout << "[ERROR]: Could not open root file specified by \"map\" "
                   "attribute: \""
                << mapfile << "\"" << std::endl;
      exit(1);
    }

    std::vector<nuiskey> accepts = nk.GetListOfChildNodes("accept");
    for (auto &acc : accepts) {
      std::string const &genStr = acc.GetS("generated");
      std::string const &accStr = acc.GetS("accepted");

      if (!genStr.length() || !accStr.length()) {
        std::cout << "[ERROR]: expected accept node to contain both "
                     "\"generated\" and \"accepted\" attributes."
                  << std::endl;
        exit(1);
      }

      std::string const &pdgs_s = acc.GetS("PDG");
      std::vector<int> pdgs_i = GeneralUtils::ParseToInt(pdgs_s, ",");

      if (!pdgs_i.size()) {
        std::cout
            << "[ERROR]: Could not find any applicable particle PDG codes."
            << std::endl;
        exit(1);
      }

      EffMap ef;

      ef.Build(f, genStr, accStr);

      for (size_t pdg_it = 0; pdg_it < pdgs_i.size(); ++pdg_it) {
        if (Acceptance.count(pdgs_i[pdg_it])) {
          std::cout
              << "[WARN]: Acceptance map already contains acceptance for PDG: "
              << pdgs_i[pdg_it] << ". Overwriting..." << std::endl;
        }
        Acceptance[pdgs_i[pdg_it]] = ef;
      }
    }

    std::cout << "Loaded " << Acceptance.size()
              << " particle acceptance definitions." << std::endl;
    f->Close();
    delete f;
  }

  RecoInfo *Smearcept(FitEvent *fe) {
    RecoInfo *ri = new RecoInfo();

    for (size_t p_it = 0; p_it < fe->NParticles(); ++p_it) {
      FitParticle *fp = fe->GetParticle(p_it);

      int PDG = fp->PDG();
      double p = fp->P3().Mag() * 1E-3;
      double cost = fp->P3().CosTheta();
      double phi = fp->P3().Phi() * (180.0 / TMath::Pi()) + 150.0;

#if DEBUG_CLASACCEPT > 1
      std::cout << std::endl;
      std::cout << "[" << p_it << "]: " << PDG << ", " << fp->Status() << ", "
                << fp->E() << " -- KE:" << fp->KE() << " Mom: " << p
                << std::flush;
#endif

      if (fp->Status() != kFinalState) {
#if DEBUG_CLASACCEPT > 1
        std::cout << " -- Not final state." << std::flush;
#endif
        continue;
      }

      if (!Acceptance.count(PDG)) {
#if DEBUG_CLASACCEPT > 1
        std::cout << " -- Unknown acceptance." << std::flush;
#endif

        continue;
      }

      EffMap const &eff = Acceptance[PDG];

      double acc_ratio = eff.GetAccRatio(p, cost, phi, DefaultAccRatio);

      bool accepted = (rand.Uniform() < acc_ratio);
      if (accepted) {
#ifdef DEBUG_CLASACCEPT
        std::cout << "(" << p << ", " << cost << ", " << phi << ")."
                  << std::endl;
        std::cout << " -- Reconstructed with probability: " << acc_ratio
                  << std::flush;
#endif
        ri->RecObjMom.push_back(fp->P3());
        ri->RecObjClass.push_back(fp->PDG());

        continue;
      }

#ifdef DEBUG_CLASACCEPT
      std::cout << "(" << p << ", " << cost << ", " << phi << ")." << std::endl;
      std::cout << " -- Rejected with probability: " << acc_ratio << std::flush;
#endif
    }

#ifdef DEBUG_CLASACCEPT
    std::cout << std::endl;

    if (ri->RecObjMom.size()) {
      std::cout << "Reconstructed " << ri->RecObjMom.size() << " particles. "
                << std::endl;
    }
#endif
    return ri;
  }

  double GetEfficiency(FitEvent *fe) {
    double effweight = 1;
    for (size_t p_it = 0; p_it < fe->NParticles(); ++p_it) {
      FitParticle *fp = fe->GetParticle(p_it);

      int PDG = fp->PDG();
      double p = fp->P3().Mag() * 1E-3;
      double cost = fp->P3().CosTheta();
      double phi = fp->P3().Phi() * (180.0 / TMath::Pi()) + 150.0;
      if (fp->Status() != kFinalState) {
        continue;
      }
      if (!Acceptance.count(PDG)) {
        continue;
      }
      EffMap eff = Acceptance[PDG];

      effweight *= eff.GetAccRatio(p, cost, phi);
    }
    return effweight;
  }
};
