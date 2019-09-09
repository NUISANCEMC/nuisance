#include "EnergyShuffler.h"

/// Node looks like
/// <EnergyShuffler>
///   <Shuffler From="2212" To="2112" Fraction="0.2" />
///   <Shuffler From="211,-211" To="" Fraction="0.5" />
/// </EnergyShuffler>
void EnergyShuffler::Setup(nuiskey &nk) {
  std::vector<nuiskey> shuffleDescriptors = nk.GetListOfChildNodes("Shuffler");

  for (size_t t_it = 0; t_it < shuffleDescriptors.size(); ++t_it) {
    if (!shuffleDescriptors[t_it].Has("From") ||
        !shuffleDescriptors[t_it].Has("Fraction")) {
      QTHROW(
          "Shuffler element must have at least the From and Fraction "
          "attributes.");
    }
    std::string from_pdgs_s = shuffleDescriptors[t_it].GetS("From");
    std::vector<int> from_pdgs_i = GeneralUtils::ParseToInt(from_pdgs_s, ",");

    if (!from_pdgs_i.size()) {
      QTHROW("Shuffler element must have at least one From PDG specified.");
    }

    std::vector<int> to_pdgs_i;
    if (shuffleDescriptors[t_it].Has("To")) {
      std::string to_pdgs_s = shuffleDescriptors[t_it].GetS("To");
      to_pdgs_i = GeneralUtils::ParseToInt(to_pdgs_s, ",");
    }

    double Fraction = shuffleDescriptors[t_it].GetD("Fraction");

    for (size_t f_it = 0; f_it < from_pdgs_i.size(); ++f_it) {
      ShuffleDescriptor sd;
      sd.ToPDGs = to_pdgs_i;
      sd.EFraction = Fraction;
      ShufflersDescriptors.push_back(std::make_pair(from_pdgs_i[f_it], sd));
      QLOG(FIT, "\tAdded EnergyShuffler from "
                    << from_pdgs_i[f_it] << " to " << to_pdgs_i.size()
                    << " particle species at " << sd.EFraction
                    << " KE fraction.")
    }
  }
}

void EnergyShuffler::DoTheShuffle(FitEvent *fe) {
  std::map<Int_t, double> TakenEnergy;
  std::map<Int_t, Int_t> NumParticlesToShare;
  // For each particle.
  // If in a from: take energy and add it to a sum.
  // Count particles of each species.
  for (size_t p_it = 0; p_it < fe->NParticles(); ++p_it) {
    FitParticle *fp = fe->GetParticle(p_it);

    if (fp->Status() != kFinalState) {
      continue;
    }

    int PDG = fp->PDG();
    for (size_t sh_it = 0; sh_it < ShufflersDescriptors.size(); ++sh_it) {
      ShuffleDescriptor &sd = ShufflersDescriptors[sh_it].second;

      if (std::find(sd.ToPDGs.begin(), sd.ToPDGs.end(), PDG) !=
          sd.ToPDGs.end()) {  // If this is a particle
                              // that we're giving energy
                              // to, take note.
        if (!NumParticlesToShare.count(sh_it)) {
          NumParticlesToShare[sh_it] = 0;
        }
        NumParticlesToShare[sh_it]++;
#ifdef DEBUG_ESHUFFLER
        std::cout << "Found recieving particle in pool " << sh_it << "."
                  << std::endl;
#endif
      }

      if (ShufflersDescriptors[sh_it].first != PDG) {
        continue;
      }

      double KETaken = sd.EFraction * fp->KE();
      if (!TakenEnergy.count(sh_it)) {
        TakenEnergy[sh_it] = 0;
      }
      TakenEnergy[sh_it] += KETaken;

#ifdef DEBUG_ESHUFFLER
      std::cout << "Taking " << KETaken << " KE from " << fp->PDG() << " ("
                << fp << ") with " << fp->KE() << " (mom: " << fp->p() << ")."
                << std::endl;
#endif
      fe->RemoveKE(p_it, KETaken);
      fp = fe->GetParticle(p_it);
#ifdef DEBUG_ESHUFFLER
      std::cout << "\t->" << fp->KE() << " (mom: " << fp->p() << ") => "
                << sh_it << "." << std::endl;
#endif
    }
  }

  double LostEnergy = 0;
  for (std::map<Int_t, double>::iterator te_it = TakenEnergy.begin();
       te_it != TakenEnergy.end(); ++te_it) {
    double EToGive = te_it->second;
    // Get energy share for each 'to' particle
    if (NumParticlesToShare.count(te_it->first)) {  // If we have any particles
                                                    // to share the energy
                                                    // between.

#ifdef DEBUG_ESHUFFLER
      std::cout << "Energy from Shuffler " << te_it->first << " " << EToGive
                << " shared between " << NumParticlesToShare[te_it->first]
                << " particle." << std::endl;
#endif

      EToGive /= double(NumParticlesToShare[te_it->first]);

    } else {
      LostEnergy += EToGive;
      continue;
    }

    ShuffleDescriptor &sd = ShufflersDescriptors[te_it->first].second;

    for (size_t p_it = 0; p_it < fe->NParticles(); ++p_it) {
      FitParticle *fp = fe->GetParticle(p_it);

      if (fp->Status() != kFinalState) {
        continue;
      }

      int PDG = fp->PDG();

      if (std::find(sd.ToPDGs.begin(), sd.ToPDGs.end(), PDG) ==
          sd.ToPDGs.end()) {  // This shuffler has no energy for this particle
        continue;
      }

#ifdef DEBUG_ESHUFFLER
      std::cout << "Giving " << EToGive << " KE to " << fp->PDG() << " with "
                << fp->KE() << " (mom: " << fp->p() << ")." << std::endl;
#endif
      fe->GiveKE(p_it, EToGive);
      fp = fe->GetParticle(p_it);
#ifdef DEBUG_ESHUFFLER
      std::cout << "\t->" << fp->KE() << " (mom: " << fp->p() << ")."
                << std::endl;
#endif
    }
  }

#ifdef DEBUG_ESHUFFLER
  if (LostEnergy > 0) {
    std::cout << "" << LostEnergy << " of KE went nowhere..." << std::endl;
  }
#endif
}
