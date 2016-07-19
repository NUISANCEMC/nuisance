// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "FitUtils.h"

/*
  MISC Functions
*/

//********************************************************
TH2D* FitUtils::CalculateQ3Cut(std::string inFile, TH2D *data, double qCut, int nuPDG, double eMin, double eMax){
//********************************************************
  
  // This function calculates the bins in the 2D distribution which should be excluded because > 50% of the events in that bin have q^2 < qCut
  TH2D *qCutHist = (TH2D*)data->Clone("qCutHist");
  qCutHist ->Reset();
  
  // DEPRECATED CODE
  // - Needs to be updated to fit in with new InputHandler format
  (void) inFile;
  (void) qCut;
  (void) nuPDG;
  (void) eMin;
  (void) eMax;
  
  // This should instead use VEXperimentBase to allow multiple inputs.
  
  // TH2D *qEvts    = (TH2D*)data->Clone("qEvts");
  // qEvts   ->Reset();
  // TH2D *qAll     = (TH2D*)data->Clone("qAll");
  // qAll    ->Reset();

  // int muPDG = 0;
  // if (nuPDG == 14) muPDG = 13;
  // else if (nuPDG == -14) muPDG = -13;
  
  // // Now iterate through the events and plot the qCut                                                                                                
  // TChain *tn = new TChain("neuttree", "");
  // tn->Add(Form("%s/neuttree", inFile.c_str()));
  // Int_t nevents = tn->GetEntries();
  // NeutVect *nvect = NULL;
  // tn->SetBranchAddress("vectorbranch", &nvect);
  // for (int i = 0; i < nevents; ++i){
  //   tn->GetEntry(i);

  //   if (!isSignal(nvect, nuPDG, eMin, eMax, false, 0)) continue;

  //   // Now calculate the 3-momentum transfer
  //   // Loop over the particle stack
  //   for (int j = 2; j < nvect->Npart(); ++j){

  //     // Look for the outgoing muon
  //     if ((nvect->PartInfo(j))->fPID != muPDG) continue;
      
  //     double q3 = sqrt(((nvect->PartInfo(j))->fP - (nvect->PartInfo(0))->fP).Vect().Mag2());

  //     // Now find the kinematic values and fill the histogram
  //     double Ekmu     = (nvect->PartInfo(j))->fP.E()/1000 - 0.105658367;
  //     double costheta = cos(((nvect->PartInfo(0))->fP.Vect().Angle((nvect->PartInfo(j))->fP.Vect())));

  //     qAll->Fill(Ekmu, costheta);     
      
  //     if (q3 <  qCut) qEvts ->Fill(Ekmu, costheta);
      
  //   }

  // }
  
  // for (int xBin = 1; xBin < qCutHist->GetNbinsX()+1; ++xBin){
  //   for (int yBin = 1; yBin < qCutHist->GetNbinsY()+1; ++yBin){
  //     double fract = 0;
  //     if (qAll->GetBinContent(xBin, yBin) != 0) fract = qEvts->GetBinContent(xBin, yBin)/(qAll->GetBinContent(xBin, yBin) + 0.);
  //     qCutHist->SetBinContent(xBin, yBin, fract);
  //   }
  // }
  return qCutHist;
};

/*
  MISC Event
*/
//********************************************************
double FitUtils::Wrec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppi) {
//********************************************************
// Reconstruct the hadronic mass using all outgoing particles
// Requires pion vector for reconstructing the neutrino energy
// Could technically do E_nu = pnu.E() too, but this won't be reconstructed Enu; it's true Enu
  (void) ppi; // P.S: Never used, looking into this.

  double E_mu = pmu.E();
  double p_mu = pmu.Vect().Mag();
  double m_mu = sqrt(E_mu*E_mu - p_mu*p_mu);
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  
  const double m_p = 938.27203;
  
  // Warning: returned value is in GeV, need to multiply by 1000
  //double E_nu = FitUtils::EnuCC1piprec(pnu, pmu, ppi)*1000.;
  double E_nu = pnu.E();


  // Old stuff, kept for booking
  //double q2 = 2*E_nu*(E_mu - p_mu * cos(th_nu_mu)) - m_mu*m_mu;
  //double w_rec = sqrt(m_p*m_p - q2 + 2*m_p*EHad);

  double w_rec = sqrt(m_p*m_p + m_mu*m_mu - 2*m_p*E_mu + 2*E_nu*(m_p - E_mu + p_mu * cos(th_nu_mu)));

  return w_rec;

};

// Kinematic stuff
double FitUtils::T(TLorentzVector part) {
  double E_part = part.E()/1000.;
  double p_part = part.Vect().Mag()/1000.;
  double m_part = sqrt(E_part*E_part - p_part*p_part);
  
  double KE_part = E_part - m_part;
  return KE_part;
};

double FitUtils::p(TLorentzVector part) {
  double p_part = part.Vect().Mag()/1000.;
  return p_part;
};

//returns theta of two vectors (radians)
double FitUtils::th(TLorentzVector part1, TLorentzVector part2) {
  double th = part1.Vect().Angle(part2.Vect());
  return th;
};


//********************************************************
double FitUtils::ProtonQ2QErec(double pE, double binding){
//********************************************************
  
  const double V  = binding/1000.;    // binding potential                                 
  const double mn = 0.93956536;       // neutron mass                                      
  const double mp = 0.93827203;       // proton mass                                       
  const double mn_eff = mn - V;       // effective proton mass
  const double pki    = (pE/1000.0) - mp;
  
  double q2qe  = mn_eff*mn_eff - mp*mp + 2*mn_eff*(pki + mp - mn_eff);

  return q2qe;  
};


double FitUtils::EnuQErec(TLorentzVector pmu, double costh, double binding, bool neutrino){

  double momshift = 0.0;
   double temp = FitPar::Config().GetParD("muon_momentum_shift");
  if (temp != -999.9 and temp != 0.0){
    if (FitPar::Config().GetParI("muon_momentum_throw") == 0) momshift = temp;
    else if (FitPar::Config().GetParI("muon_momentum_throw") == 1){
      momshift = gRandom->Gaus(0.0,1.0) * temp;
    }
  }
  //  std::cout<<"Current Momentum Shift = "<<momshift<<std::endl;

  // Convert all values to GeV                            
  const double V  = binding/1000.;    // binding potential 
  const double mn = 0.93956536;       // neutron mass   
  const double mp = 0.93827203;       // proton mass    
  
  double mN_eff = mn - V;
  double mN_oth = mp;

  if (!neutrino){
    mN_eff = mp - V;
    mN_oth = mn;
  }

  double el =  pmu.E()/1000.;
  double pl = (pmu.Vect().Mag())/1000.; // momentum of lepton
  double ml = sqrt(el*el - pl*pl); // lepton mass
  pl += momshift;

  double rEnu = (2*mN_eff*el - ml*ml + mN_oth*mN_oth - mN_eff*mN_eff)/
    (2*(mN_eff - el + pl*costh));
  
  //  std::cout<<"Enu = "<<rEnu<<std::endl;
  return rEnu;
};


double FitUtils::Q2QErec(TLorentzVector pmu, double costh, double binding, bool neutrino){

  double momshift = 0.0;
  double temp = FitPar::Config().GetParD("muon_momentum_shift");
  if (temp != -999.9 and temp != 0.0){
    if (FitPar::Config().GetParI("muon_momentum_throw") == 0) momshift = temp;
    else if (FitPar::Config().GetParI("muon_momentum_throw") == 1){
      momshift = gRandom->Gaus(0.0,1.0) * temp;
    }
  }
    
  //  std::cout<<"Current Q2QE Momentum Shift = "<<momshift<<std::endl;

  double el = pmu.E()/1000.;
  double pl = (pmu.Vect().Mag())/1000.; // momentum of lepton
  double ml = sqrt(el*el - pl*pl); // lepton mass
  pl += momshift/1000.;

  double rEnu = EnuQErec(pmu, costh, binding,neutrino);
  double q2 = -ml*ml+2.*rEnu*(el-pl*costh);
  
  return q2;
};

double FitUtils::EnuCC1pi0rec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppi0){

  double E_mu = pmu.E()/1000;
  double p_mu = pmu.Vect().Mag()/1000;
  double m_mu = sqrt(E_mu*E_mu - p_mu*p_mu);
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  double E_pi0 = ppi0.E()/1000;
  double p_pi0 = ppi0.Vect().Mag()/1000;
  double m_pi0 = sqrt(E_pi0*E_pi0 - p_pi0*p_pi0);
  double th_nu_pi0 = pnu.Vect().Angle(ppi0.Vect());

  const double m_n = 0.93956536;       // neutron mass       
  const double m_p = 0.93827203;       // proton mass        
  double th_pi0_mu = ppi0.Vect().Angle(pmu.Vect());

  double rEnu = (m_mu*m_mu + m_pi0*m_pi0 + m_n*m_n - m_p*m_p - 2*m_n*(E_pi0 + E_mu) + 2*E_pi0*E_mu - 2*p_pi0*p_mu*cos(th_pi0_mu))/(2*(E_pi0 + E_mu - p_pi0*cos(th_nu_pi0) - p_mu*cos(th_nu_mu) \
																		  - m_n));

  return rEnu;
};

double FitUtils::Q2CC1pi0rec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppi0) {

  double E_mu = pmu.E()/1000.;// energy of lepton in GeV     
  double p_mu = pmu.Vect().Mag()/1000.; // momentum of lepton
  double m_mu = sqrt(E_mu*E_mu - p_mu*p_mu); // lepton mass  
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  double rEnu = EnuCC1pi0rec(pnu, pmu, ppi0); //reconstructed neutrino energy         
  double q2 = -m_mu*m_mu + 2.*rEnu*(E_mu - p_mu*cos(th_nu_mu));

  return q2;
};

double FitUtils::EnuCC1piprec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppip){
// Reconstruct neutrino energy from outgoing particles; will differ from the actual neutrino energy

  double E_mu = pmu.E()/1000;
  double p_mu = pmu.Vect().Mag()/1000;
  double m_mu = sqrt(E_mu*E_mu - p_mu*p_mu);
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  double E_pip = ppip.E()/1000;
  double p_pip = ppip.Vect().Mag()/1000;
  double m_pip = sqrt(E_pip*E_pip - p_pip*p_pip);
  double th_nu_pip = pnu.Vect().Angle(ppip.Vect());

  const double m_n = 0.93956536;      // neutron/proton mass
                                      // should really take proton mass for proton interaction, neutron for neutron interaction. However, difference is pretty much negligable here!

  // need this angle too      
  double th_pip_mu = ppip.Vect().Angle(pmu.Vect());

  double rEnu = (m_mu*m_mu + m_pip*m_pip - 2*m_n*(E_pip + E_mu) + 2*E_pip*E_mu - 2*p_pip*p_mu*cos(th_pip_mu))/(2*(E_pip + E_mu - p_pip*cos(th_nu_pip) - p_mu*cos(th_nu_mu) - m_n));

  return rEnu;
};

double FitUtils::EnuCC1piprecDelta(TLorentzVector pnu, TLorentzVector pmu) {
// Reconstruct neutrino energy from outgoing particles; will differ from the actual neutrino energy. Here we use assumption of a Delta resonance

  const double m_Delta = 1.232; // PDG value for Delta mass in GeV
  const double m_n = 0.93956536;      // neutron/proton mass
                                      // should really take proton mass for proton interaction, neutron for neutron interaction. However, difference is pretty much negligable here!

  double E_mu = pmu.E()/1000;
  double p_mu = pmu.Vect().Mag()/1000;
  double m_mu = sqrt(E_mu*E_mu - p_mu*p_mu);
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  double rEnu = (m_Delta*m_Delta - m_n*m_n - m_mu*m_mu + 2*m_n*E_mu)/(2*(m_n - E_mu + p_mu*cos(th_nu_mu)));

  return rEnu;
};



double FitUtils::MpPi(TLorentzVector pp, TLorentzVector ppi) {
  double E_p = pp.E();
  double p_p = pp.Vect().Mag();
  double m_p = sqrt(E_p*E_p - p_p*p_p);

  double E_pi = ppi.E();
  double p_pi = ppi.Vect().Mag();
  double m_pi = sqrt(E_pi*E_pi - p_pi*p_pi);

  double th_p_pi = pp.Vect().Angle(ppi.Vect());

  // fairly easy thing to derive since bubble chambers measure the proton!
  double invMass = sqrt(m_p*m_p + m_pi*m_pi + 2*E_p*E_pi - 2*p_pi*p_p*cos(th_p_pi));

  return invMass;
};

// the modified Q2 (only modified because different reconstructed energy
// calculation)  
double FitUtils::Q2CC1piprec(TLorentzVector pnu, TLorentzVector pmu, TLorentzVector ppip) {

  double E_mu = pmu.E()/1000.;// energy of lepton in GeV     
  double p_mu = pmu.Vect().Mag()/1000.; // momentum of lepton
  double m_mu = sqrt(E_mu*E_mu - p_mu*p_mu); // lepton mass  
  double th_nu_mu = pnu.Vect().Angle(pmu.Vect());

  double rEnu = EnuCC1piprec(pnu, pmu, ppip); //reconstructed neutrino energy
  double q2 = -m_mu*m_mu + 2.*rEnu*(E_mu - p_mu*cos(th_nu_mu));

  return q2;
};

