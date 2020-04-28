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

#include "variation/WeightManager.hxx"

#include "utility/EventTopologyUtility.hxx"
#include "utility/FullEventUtility.hxx"
#include "utility/InteractionChannelUtility.hxx"
#include "utility/KinematicUtility.hxx"
#include "utility/PDGCodeUtility.hxx"
#include "utility/ROOTUtility.hxx"

#include "utility/experimental/MINERvAUtility.hxx"
#include "utility/experimental/T2KUtility.hxx"

#include "persistency/ROOTOutput.hxx"

using namespace nuis::event;
using namespace nuis::utility;
using namespace nuis::variation;

class EventSummary_ECTJune2019 : public SimpleMCStudy {

  TreeFile EvSumTree;

  struct EvSum {
    double e_nu;
    int pdg_nu;

    double e_fslep;
    double p_fslep;
    double costheta_fslep;
    int pdg_fslep;

    double e_isnucleon;
    double p_isnucleon;
    int pdg_isnucleon;

    double e_hmfsproton;
    double p_hmfsproton;
    double costheta_hmfsproton;

    double e_hmfspi;
    double costheta_hmfspi;
    int pdg_hmfspi;

    double dpt;
    double dptx;
    double dpty;
    double dat;
    double dphit;
    double dpt_mnv;
    double dat_mnv;
    double dphit_mnv;
    double pn_mnv;
    double q0;
    double q3;
    double dpn;
    double EAvailProxy;
    double Q2;
    double W_rec;

    double EMiss;
    double TRecRemnant;
    double PMiss;

    int mode;

    int NFSPip;
    int NFSPim;
    int NFSPi0;
    int NFSProton;
    int NFSNeutron;
    int NFSGamma;
    int NFSKaon;
    int NFSOther;

    bool isCC;

    bool isQE;
    bool isQEL;
    bool is2p2h;
    bool isNucleonSPP;
    bool isCoh;
    bool isMultiPi;
    bool isDIS;
    bool isOtherChannel;

    bool is0Pi;
    bool is1Pi;
    bool isNPi;
    bool isOtherTopo;

    bool isT2K0Pi;
    bool isT2K1Pip_CH;
    bool isT2K_STV;

    bool isMINERvA0Pi;
    bool isMINERvA1CPi;
    bool isMINERvA1Pi0;
    bool isMINERvALowRecoil;
    bool isMINERvASTV;

    double xsweight;
    double rwweight;
  };

  EvSum es;

public:
  EventSummary_ECTJune2019() { ReadGlobalConfigDefaults(); }

  void SetBranchAddresses() {
    EvSumTree->Branch("e_nu", &es.e_nu, "e_nu/D");
    EvSumTree->Branch("pdg_nu", &es.pdg_nu, "pdg_nu/I");

    EvSumTree->Branch("e_fslep", &es.e_fslep, "e_fslep/D");
    EvSumTree->Branch("p_fslep", &es.p_fslep, "p_fslep/D");
    EvSumTree->Branch("costheta_fslep", &es.costheta_fslep, "costheta_fslep/D");
    EvSumTree->Branch("pdg_fslep", &es.pdg_fslep, "pdg_fslep/I");

    EvSumTree->Branch("e_isnucleon", &es.e_isnucleon, "e_isnucleon/D");
    EvSumTree->Branch("p_isnucleon", &es.p_isnucleon, "p_isnucleon/D");
    EvSumTree->Branch("pdg_isnucleon", &es.pdg_isnucleon, "pdg_isnucleon/I");

    EvSumTree->Branch("e_hmfsproton", &es.e_hmfsproton, "e_hmfsproton/D");
    EvSumTree->Branch("p_hmfsproton", &es.p_hmfsproton, "p_hmfsproton/D");
    EvSumTree->Branch("costheta_hmfsproton", &es.costheta_hmfsproton,
                      "costheta_hmfsproton/D");

    EvSumTree->Branch("e_hmfspi", &es.e_hmfspi, "e_hmfspi/D");
    EvSumTree->Branch("costheta_hmfspi", &es.costheta_hmfspi,
                      "costheta_hmfspi/D");
    EvSumTree->Branch("pdg_hmfspi", &es.pdg_hmfspi, "pdg_hmfspi/I");

    EvSumTree->Branch("dpt", &es.dpt, "dpt/D");
    EvSumTree->Branch("dptx", &es.dptx, "dptx/D");
    EvSumTree->Branch("dpty", &es.dpty, "dpty/D");
    EvSumTree->Branch("dat", &es.dat, "dat/D");
    EvSumTree->Branch("dphit", &es.dphit, "dphit/D");
    EvSumTree->Branch("dpt_mnv", &es.dpt_mnv, "dpt_mnv/D");
    EvSumTree->Branch("dat_mnv", &es.dat_mnv, "dat_mnv/D");
    EvSumTree->Branch("dphit_mnv", &es.dphit_mnv, "dphit_mnv/D");
    EvSumTree->Branch("pn_mnv", &es.pn_mnv, "pn_mnv/D");
    EvSumTree->Branch("q0", &es.q0, "q0/D");
    EvSumTree->Branch("q3", &es.q3, "q3/D");
    EvSumTree->Branch("EAvailProxy", &es.EAvailProxy, "EAvailProxy/D");
    EvSumTree->Branch("Q2", &es.Q2, "Q2/D");
    EvSumTree->Branch("W_rec", &es.W_rec, "W_rec/D");

    EvSumTree->Branch("EMiss", &es.EMiss, "EMiss/D");
    EvSumTree->Branch("TRecRemnant", &es.TRecRemnant, "TRecRemnant/D");
    EvSumTree->Branch("PMiss", &es.PMiss, "PMiss/D");

    EvSumTree->Branch("mode", &es.mode, "mode/I");

    EvSumTree->Branch("NFSPip", &es.NFSPip, "NFSPip/I");
    EvSumTree->Branch("NFSPim", &es.NFSPim, "NFSPim/I");
    EvSumTree->Branch("NFSPi0", &es.NFSPi0, "NFSPi0/I");
    EvSumTree->Branch("NFSProton", &es.NFSProton, "NFSProton/I");
    EvSumTree->Branch("NFSNeutron", &es.NFSNeutron, "NFSNeutron/I");
    EvSumTree->Branch("NFSGamma", &es.NFSGamma, "NFSGamma/I");
    EvSumTree->Branch("NFSKaon", &es.NFSKaon, "NFSKaon/I");
    EvSumTree->Branch("NFSOther", &es.NFSOther, "NFSOther/I");

    EvSumTree->Branch("isCC", &es.isCC, "isCC/O");

    EvSumTree->Branch("isQE", &es.isQE, "isQE/O");
    EvSumTree->Branch("isQEL", &es.isQEL, "isQEL/O");
    EvSumTree->Branch("is2p2h", &es.is2p2h, "is2p2h/O");
    EvSumTree->Branch("isNucleonSPP", &es.isNucleonSPP, "isNucleonSPP/O");
    EvSumTree->Branch("isCoh", &es.isCoh, "isCoh/O");
    EvSumTree->Branch("isMultiPi", &es.isMultiPi, "isMultiPi/O");
    EvSumTree->Branch("isDIS", &es.isDIS, "isDIS/O");
    EvSumTree->Branch("isOtherChannel", &es.isOtherChannel, "isOtherChannel/O");

    EvSumTree->Branch("is0Pi", &es.is0Pi, "is0Pi/O");
    EvSumTree->Branch("is1Pi", &es.is1Pi, "is1Pi/O");
    EvSumTree->Branch("isNPi", &es.isNPi, "isNPi/O");
    EvSumTree->Branch("isOtherTopo", &es.isOtherTopo, "isOtherTopo/O");

    EvSumTree->Branch("isT2K0Pi", &es.isT2K0Pi, "isT2K0Pi/O");
    EvSumTree->Branch("isT2K1Pip_CH", &es.isT2K1Pip_CH, "isT2K1Pip_CH/O");
    EvSumTree->Branch("isT2K_STV", &es.isT2K_STV, "isT2K_STV/O");

    EvSumTree->Branch("isMINERvA0Pi", &es.isMINERvA0Pi, "isMINERvA0Pi/O");
    EvSumTree->Branch("isMINERvA1CPi", &es.isMINERvA1CPi, "isMINERvA1CPi/O");
    EvSumTree->Branch("isMINERvA1Pi0", &es.isMINERvA1Pi0, "isMINERvA1Pi0/O");
    EvSumTree->Branch("isMINERvALowRecoil", &es.isMINERvALowRecoil,
                      "isMINERvALowRecoil/O");
    EvSumTree->Branch("isMINERvASTV", &es.isMINERvASTV, "isMINERvASTV/O");

    EvSumTree->Branch("xsweight", &es.xsweight, "xsweight/D");
    EvSumTree->Branch("rwweight", &es.rwweight, "rwweight/D");
  }

  void Initialize(fhicl::ParameterSet const &instance_sample_configuration) {

    // Perform any per-sample configuration in the base class
    SimpleMCStudy::Initialize(instance_sample_configuration);

    EvSumTree = AddNewTTreeToFile(nuis::persistency::GetOutputFile().get(),
                                  "ECTEvSumTree", write_directory);
    SetBranchAddresses();

    //! Here you do whatever you want to do on a per-event basis.
    //! This method will be run for every event given by the input handler, the
    //! weight allows differential xsecs to be plotted easily and also contains
    //! any requested reweightable parameter variations.
    ProcessEventFunction = [&](nuis::event::FullEvent const &ev,
                               double weight) -> void {
      Particle ISNu = GetHMISNeutralLepton(ev);
      if (!ISNu) {
        return;
      }

      Particle FSLep = GetHMFSLepton(ev);
      if (!FSLep) {
        return;
      }

      es.e_nu = ISNu.E();
      es.pdg_nu = ISNu.pdg;

      es.e_fslep = FSLep.E();
      es.costheta_fslep = FSLep.CosTheta();
      es.pdg_fslep = FSLep.pdg;

      Particle FSPi = GetHMFSPion(ev);
      if (!FSPi) {
        es.e_hmfspi = 0;
        es.costheta_hmfspi = 0;
        es.pdg_hmfspi = 0;
      } else {
        es.e_hmfspi = FSPi.E();
        es.costheta_hmfspi = FSPi.CosTheta();
        es.pdg_hmfspi = FSPi.pdg;
      }

      Particle FSProton = GetHMFSProton(ev);
      if (!FSProton) {
        es.e_hmfsproton = 0;
        es.p_hmfsproton = 0;
        es.costheta_hmfsproton = 0;
      } else {
        es.e_hmfsproton = FSProton.E();
        es.p_hmfsproton = FSProton.P();
        es.costheta_hmfsproton = FSProton.CosTheta();
      }

      TLorentzVector EMTransfer_lep = GetEnergyMomentumTransfer(ev);

      TVector3 qT = EMTransfer_lep.Vect();
      qT[2] = 0;
      qT = qT.Unit();

      TVector3 qTPerp = TVector3(0, 0, 1).Cross(qT);

      TVector3 dpt = GetDeltaPT_CC0PiN(ev, ISNu.pdg);

      es.dpt = dpt.Mag();
      es.dptx = dpt.Dot(qTPerp);
      es.dpty = dpt.Dot(qT);
      es.dat = GetDeltaAlphaT_CC0PiN(ev, ISNu.pdg);
      es.dphit = GetDeltaPhiT_CC0PiN(ev, ISNu.pdg);
      es.dpt_mnv = mnv::GetDeltaPT_CC0PiN_mnv(ev).Mag();
      es.dat_mnv = mnv::GetDeltaAlphaT_CC0PiN_mnv(ev);
      es.dphit_mnv = mnv::GetDeltaPhiT_CC0PiN_mnv(ev);
      es.pn_mnv = mnv::GetNeutronMomentumReco_CC0PiN_mnv(ev);
      es.EAvailProxy = GetEAvailProxy(ev);
      es.Q2 = -EMTransfer_lep.Mag2();
      es.q0 = EMTransfer_lep.E();
      es.q3 = EMTransfer_lep.Vect().Mag();
      es.W_rec = GetNeutrinoWRec(ev);

      Particle FSNuc = GetHMFSNucleon(ev);
      if (!FSNuc) {
        return;
      }

      Particle ISNuc = GetHMISParticle(ev, {2212, 2112});
      if (!ISNuc) {
        return;
      }

      es.pdg_isnucleon = ISNuc.pdg;
      es.e_isnucleon = ISNuc.E();
      es.p_isnucleon = ISNuc.P();

      TVector3 pRemnant_rec = EMTransfer_lep.Vect() - FSNuc.P3();
      TVector3 pRemnant_IA = -ISNuc.P3();
      static double const MC12_MeV = 12.017 * 931.494;
      double MAPrime;
      TLorentzVector PRemnant_rec;
      if (ISNu.pdg > 0) {
        static double const MC11_MeV = 11.0114336 * 931.494;
        MAPrime = MC11_MeV;
        PRemnant_rec.SetXYZM(pRemnant_rec[0], pRemnant_rec[1], pRemnant_rec[2],
                             MC11_MeV);

      } else {
        static double const MB11_MeV = 11.0093054 * 931.494;
        MAPrime = MB11_MeV;
        PRemnant_rec.SetXYZM(pRemnant_rec[0], pRemnant_rec[1], pRemnant_rec[2],
                             MB11_MeV);
      }
      es.TRecRemnant = PRemnant_rec.E() - PRemnant_rec.M();

      es.EMiss = EMTransfer_lep.E() + GetPDGMass(ISNuc.pdg) - FSNuc.E() -
                 es.TRecRemnant;
      es.PMiss = ((EMTransfer_lep - FSNuc.P4).Vect()).Mag();

      es.mode = ChannelToInt(ev.mode);

      es.NFSPip = GetNParticles(ev, {pdgcodes::kPiPlus});
      es.NFSPim = GetNParticles(ev, {pdgcodes::kPiMinus});
      es.NFSPi0 = GetNParticles(ev, {pdgcodes::kPi0});
      es.NFSProton = GetNParticles(ev, {pdgcodes::kProton});
      es.NFSNeutron = GetNParticles(ev, {pdgcodes::kNeutron});
      es.NFSGamma = GetNParticles(ev, {pdgcodes::kGamma});
      es.NFSKaon = GetNParticles(ev, pdgcodes::Kaons);
      es.NFSOther = GetNFSOthers(ev) - (es.NFSGamma + es.NFSKaon);

      es.isCC = IsCC(ev.mode);

      es.isQE = IsQE(ev.mode);
      es.isQEL = IsQEL(ev.mode);
      es.is2p2h = Is2p2h(ev.mode);
      es.isNucleonSPP = IsNucleonSPP(ev.mode);
      es.isCoh = IsCoh(ev.mode);
      es.isMultiPi = IsMultiPi(ev.mode);
      es.isDIS = IsDIS(ev.mode);
      es.isOtherChannel =
          !(es.isQE || es.isQEL || es.is2p2h || es.isNucleonSPP || es.isCoh ||
            es.isMultiPi || es.isDIS);

      size_t NPi = GetNFSPions(ev);
      size_t NOthers = GetNFSOthers(ev);
      es.is0Pi = (NPi == 0) && (!NOthers);
      es.is1Pi = (NPi == 1) && (!NOthers);
      es.isNPi = (NPi > 1) && (!NOthers);
      es.isOtherTopo = NOthers;

      es.isT2K0Pi =
          t2k::IsCC0Pi_NumProtons(ev).first; // Don't care about n protons
      es.isT2K1Pip_CH = t2k::IsCC1Pip_CH_RecPi(ev);
      es.isT2K_STV = t2k::IsCC0Pi_STV(ev);

      es.isMINERvA0Pi =
          mnv::IsCC0Pi_NumProtons(ev).first; // Don't care about n protons
      es.isMINERvA1CPi = mnv::IsCC1CPi_2017(ev);
      es.isMINERvA1Pi0 = mnv::IsCC1Pi0_2016(ev);
      es.isMINERvALowRecoil = mnv::IsCCIncLowRecoil(ev);
      es.isMINERvASTV = mnv::IsCC0PiNp_STV(ev);

      es.xsweight = weight;
      es.rwweight = WeightManager::Get().GetEventWeight(ev);

      EvSumTree->Fill();
    };
  }

  std::string Name() { return "EventSummary_ECTJune2019"; }

  //! Here you can write any custom histograms to TTrees that your sample has
  //! been handling.
  void Write() {}
};

//! These declarations allow your class to be loaded dynamically by NUISANCE
DECLARE_PLUGIN(IEventProcessor, EventSummary_ECTJune2019);
