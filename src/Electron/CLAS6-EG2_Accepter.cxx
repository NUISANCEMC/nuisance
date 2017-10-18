#include "ISmearcepter.h"

#include "TH3D.h"
#include "TRandom3.h"

#include <cstdlib>

#define DEBUG_CLASACCEPT

class CLASAccepter : public ISmearcepter {
  TRandom3 rand;
  // Maps a particle PDG to the relevant generated and accepted histograms from
  // the input map.
  std::map<int, std::pair<TH3D *, TH3D *> > Acceptance;

 public:
  CLASAccepter() { ElementName = "CLASAccepter"; }

  void SpecifcSetup(nuiskey &nk) {
    rand.~TRandom3();
    new (&rand) TRandom3();

    InstanceName = nk.GetS("name");

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

      std::pair<TH3D *, TH3D *> genacc;

      genacc.first = dynamic_cast<TH3D *>(f->Get(accStr.c_str()));
      genacc.second = dynamic_cast<TH3D *>(f->Get(genStr.c_str()));

      if (!genacc.first) {
        std::cout << "[ERROR]: Could not retrieve \"accepted\" histogram: \""
                  << accStr << "\" from file: \"" << mapfile << "\"."
                  << std::endl;
        exit(1);
      }
      if (!genacc.second) {
        std::cout << "[ERROR]: Could not retrieve \"accepted\" histogram: \""
                  << genStr << "\" from file: \"" << mapfile << "\"."
                  << std::endl;
        exit(1);
      }

      genacc.first = static_cast<TH3D *>(genacc.first->Clone());
      genacc.first->SetDirectory(NULL);
      genacc.second = static_cast<TH3D *>(genacc.second->Clone());
      genacc.second->SetDirectory(NULL);

      for (size_t pdg_it = 0; pdg_it < pdgs_i.size(); ++pdg_it) {
        if (Acceptance.count(pdgs_i[pdg_it])) {
          std::cout
              << "[WARN]: Acceptance map already contains acceptance for PDG: "
              << pdgs_i[pdg_it] << ". Overwriting..." << std::endl;
        }
        Acceptance[pdgs_i[pdg_it]] = genacc;
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
      double p = fp->P3().Mag();
      double cost = fp->P3().CosTheta();
      double phi = fp->P3().Phi();

#ifdef DEBUG_CLASACCEPT
      std::cout << std::endl;
      std::cout << "[" << p_it << "]: " << PDG << ", " << fp->Status() << ", "
                << fp->E() << " -- KE:" << fp->KE() << " Mom: " << p
                << std::flush;
#endif

      if (fp->Status() != kFinalState) {
#ifdef DEBUG_CLASACCEPT
        std::cout << " -- Not final state." << std::flush;
#endif
        continue;
      }

      if (!Acceptance.count(PDG)) {
#ifdef DEBUG_CLASACCEPT
        std::cout << " -- Unknown acceptance." << std::flush;
#endif

        continue;
      }

      std::pair<TH3D *, TH3D *> acc = Acceptance[PDG];

      // For a bin in phase space defined by p, cost, phi:
      // Find number of generated events
      Int_t pbin = acc.second->GetXaxis()->FindBin(p);
      Int_t tbin = acc.second->GetYaxis()->FindBin(cost);
      Int_t phibin = acc.second->GetZaxis()->FindBin(phi);
      double num_gen = acc.second->GetBinContent(pbin, tbin, phibin);
      // Find number of accepted events
      pbin = acc.first->GetXaxis()->FindBin(p);
      tbin = acc.first->GetYaxis()->FindBin(cost);
      phibin = acc.first->GetZaxis()->FindBin(phi);
      double num_acc = acc.first->GetBinContent(pbin, tbin, phibin);
      double acc_ratio = double(num_acc) / double(num_gen);

      bool accepted = (rand.Uniform() < acc_ratio);
      if (accepted) {
#ifdef DEBUG_CLASACCEPT
        std::cout << " -- Reconstructed with probability: " << acc_ratio
                  << std::flush;
#endif
        ri->RecObjMom.push_back(fp->P3());
        ri->RecObjClass.push_back(fp->PDG());

        continue;
      }

#ifdef DEBUG_CLASACCEPT
      std::cout << " -- Rejected with probability: " << acc_ratio << std::flush;
#endif
#ifdef DEBUG_CLASACCEPT
      std::cout << std::endl;
#endif
    }

#ifdef DEBUG_CLASACCEPT
    std::cout << "Reconstructed " << ri->RecObjMom.size() << " particles. "
              << std::endl;
#endif
    return ri;
  }
};
