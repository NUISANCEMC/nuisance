// Copyright 2018 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

//********************************************************************

#include "samples/SimpleMCStudy.hxx"

#include "utility/EventTopologyUtility.hxx"
#include "utility/FullEventUtility.hxx"
#include "utility/KinematicUtility.hxx"
#include "utility/PDGCodeUtility.hxx"
#include "utility/ROOTUtility.hxx"

#include "persistency/ROOTOutput.hxx"

using namespace nuis::event;
using namespace nuis::utility;

class NuisToCAFTruth : public SimpleMCStudy {

  TreeFile outputTree;

  // int mode;

  int isFD;
  int isFHC;
  int isCC;

  int nuPDG;
  int nuPDGunosc;
  int LepPDG;

  double NuMomX;
  double NuMomY;
  double NuMomZ;
  double Ev;
  double LepMomX;
  double LepMomY;
  double LepMomZ;
  double LepE;
  double LepNuAngle;

  double Q2;
  double W;
  double X;
  double Y;

  int nP;
  int nN;
  int nipip;
  int nipim;
  int nipi0;
  int nikp;
  int nikm;
  int nik0;
  int niem;
  int niother;
  int nNucleus;
  int nUNKNOWN;

  double eP;
  double eN;
  double ePip;
  double ePim;
  double ePi0;
  double eOther;

  double xsecWeight;

  bool isOscSwap;

public:
  NuisToCAFTruth() { ReadGlobalConfigDefaults(); }

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    outputTree = AddNewTTreeToFile(nuis::persistency::GetOutputFile().get(),
                                   "cafTruthTree");

    isFD = instance_sample_configuration.get<bool>("is_fd", false);
    isFHC = instance_sample_configuration.get<bool>("is_numode", true);
    isOscSwap = instance_sample_configuration.get<bool>("is_nueswap", false);

    // Perform any per-sample configuration in the base class
    SimpleMCStudy::Initialize(instance_sample_configuration);

    // outputTree->Branch("mode", &mode, "mode/I");
    outputTree->Branch("isFD", &isFD, "isFD/I");
    outputTree->Branch("isFHC", &isFHC, "isFHC/I");
    outputTree->Branch("isCC", &isCC, "isCC/I");
    outputTree->Branch("nuPDG", &nuPDG, "nuPDG/I");
    outputTree->Branch("nuPDGunosc", &nuPDGunosc, "nuPDGunosc/I");
    outputTree->Branch("LepPDG", &LepPDG, "LepPDG/I");
    outputTree->Branch("NuMomX", &NuMomX, "NuMomX/D");
    outputTree->Branch("NuMomY", &NuMomY, "NuMomY/D");
    outputTree->Branch("NuMomZ", &NuMomZ, "NuMomZ/D");
    outputTree->Branch("Ev", &Ev, "Ev/D");
    outputTree->Branch("LepMomX", &LepMomX, "LepMomX/D");
    outputTree->Branch("LepMomY", &LepMomY, "LepMomY/D");
    outputTree->Branch("LepMomZ", &LepMomZ, "LepMomZ/D");
    outputTree->Branch("LepE", &LepE, "LepE/D");
    outputTree->Branch("LepNuAngle", &LepNuAngle, "LepNuAngle/D");
    outputTree->Branch("Q2", &Q2, "Q2/D");
    outputTree->Branch("W", &W, "W/D");
    outputTree->Branch("X", &X, "X/D");
    outputTree->Branch("Y", &Y, "Y/D");
    outputTree->Branch("nP", &nP, "nP/I");
    outputTree->Branch("nN", &nN, "nN/I");
    outputTree->Branch("nipip", &nipip, "nipip/I");
    outputTree->Branch("nipim", &nipim, "nipim/I");
    outputTree->Branch("nipi0", &nipi0, "nipi0/I");
    outputTree->Branch("nikp", &nikp, "nikp/I");
    outputTree->Branch("nikm", &nikm, "nikm/I");
    outputTree->Branch("nik0", &nik0, "nik0/I");
    outputTree->Branch("niem", &niem, "niem/I");
    outputTree->Branch("niother", &niother, "niother/I");
    outputTree->Branch("nNucleus", &nNucleus, "nNucleus/I");
    outputTree->Branch("nUNKNOWN", &nUNKNOWN, "nUNKNOWN/I");
    outputTree->Branch("eP", &eP, "eP/D");
    outputTree->Branch("eN", &eN, "eN/D");
    outputTree->Branch("ePip", &ePip, "ePip/D");
    outputTree->Branch("ePim", &ePim, "ePim/D");
    outputTree->Branch("ePi0", &ePi0, "ePi0/D");
    outputTree->Branch("eOther", &eOther, "eOther/D");
    outputTree->Branch("xsecWeight", &xsecWeight, "xsecWeight/D");

    //! Here you do whatever you want to do on a per-event basis.
    //! This method will be run for every event given by the input handler,
    //! the weight allows differential xsecs to be plotted easily and also
    //! contains any requested reweightable parameter variations.
    ProcessEventFunction = [&](nuis::event::FullEvent const &ev,
                               double weight) -> void {
      xsecWeight = weight;

      nP = 0;
      nN = 0;
      nipip = 0;
      nipim = 0;
      nipi0 = 0;
      nikp = 0;
      nikm = 0;
      nik0 = 0;
      niem = 0;
      niother = 0;
      nNucleus = 0;
      nUNKNOWN = 0;
      eP = 0;
      eN = 0;
      ePip = 0;
      ePim = 0;
      ePi0 = 0;
      eOther = 0;

      Particle ISLep = GetHMISNeutralLepton(ev);
      Particle FSLep = GetHMFSLepton(ev);

      isCC = IsChargedLepton(FSLep.pdg);

      nuPDG = ISLep.pdg;
      nuPDGunosc = isOscSwap
                       ? (ISLep.pdg < 0 ? pdgcodes::kNuMuBar : pdgcodes::kNuMu)
                       : ISLep.pdg;
      LepPDG = FSLep.pdg;

      NuMomX = ISLep.P4.X();
      NuMomY = ISLep.P4.Y();
      NuMomZ = ISLep.P4.Z();
      Ev = ISLep.P4.E();

      LepMomX = FSLep.P4.X();
      LepMomY = FSLep.P4.Y();
      LepMomZ = FSLep.P4.Z();
      LepE = FSLep.P4.E();

      LepNuAngle = FSLep.Dir().Dot(ISLep.Dir());

      TLorentzVector FourMomTransfer = GetEnergyMomentumTransfer(ev);

      Q2 = -FourMomTransfer.Mag2();
      W = sqrt(-Q2 +
               2 * pdgmasses::kNucleonAverageMass_MeV * FourMomTransfer.E() +
               pdgmasses::kNucleonAverageMass_MeV *
                   pdgmasses::kNucleonAverageMass_MeV);
      X = Q2 / (2 * pdgmasses::kNucleonAverageMass_MeV * FourMomTransfer.E());
      Y = 1 - LepE / Ev;

      for (Particle const &p : GetNuclearLeavingParticles(ev)) {
        if (p == FSLep) {
          continue;
        }

        if (IsProton(p.pdg)) {
          nP++;
          eP += p.KE();
        } else if (IsNeutron(p.pdg)) {
          nN++;
          eN += p.KE();
        } else if (IsPositivePion(p.pdg)) {
          nipip++;
          ePip += p.KE();
        } else if (IsNegativePion(p.pdg)) {
          nipim++;
          ePim += p.KE();
        } else if (IsNeutralPion(p.pdg)) {
          nipi0++;
          ePi0 += p.KE();
        } else if (IsPositiveKaon(p.pdg)) {
          nikp++;
          eOther += p.KE();
        } else if (IsNegativeKaon(p.pdg)) {
          nikm++;
          eOther += p.KE();
        } else if (IsNeutralKaon(p.pdg)) {
          nik0++;
          eOther += p.KE();
        } else if (IsGamma(p.pdg)) {
          niem++;
          eOther += p.KE();
        } else if (IsNuclearPDG(p.pdg)) {
          nNucleus++;
        } else {
          niother++;
          eOther += p.KE();
        }
      }

      outputTree->Fill();
    };
  }

  std::string Name() { return "NuisToCAFTruth"; }

  void Write() {}
};

//! These declarations allow your class to be loaded dynamically by NUISANCE
DECLARE_PLUGIN(IEventProcessor, NuisToCAFTruth);
