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

#include "SciBooNEUtils.h"

#include "FitUtils.h"

double SciBooNEUtils::GetSciBarDensity(){
  static double density = 0xdeadbeef;
  if (density == 0xdeadbeef){
    density = FitPar::Config().GetParD("SciBarDensity");
  }
  return density;
}

double SciBooNEUtils::GetSciBarRecoDist(){
  static double dist = 0xdeadbeef;
  if (dist == 0xdeadbeef){
    dist = FitPar::Config().GetParD("SciBarRecoDist");
  }
  return dist;
}

double SciBooNEUtils::GetPenetratingMuonE(){
  static double mue = 0xdeadbeef;
  if (mue == 0xdeadbeef){
    mue = FitPar::Config().GetParD("PenetratingMuonEnergy");
  }
  return mue;
}

// Replacs with a function to draw from the z distribution that Zach made, and require the pion goes further.
// Ignores correlation between angle and distance, but... nevermind
double SciBooNEUtils::GetMainPionRange(){
  static TF1 *func = new TF1("f1", "250 - (2./3.)*(x-10)", 10, 160);
  return func->GetRandom();
}


int SciBooNEUtils::GetNumRangeSteps(){
  static uint nsteps = 0xdeadbeef;
  if (nsteps == 0xdeadbeef){
    nsteps = FitPar::Config().GetParI("NumRangeSteps");
  }
  return nsteps;
}

bool SciBooNEUtils::GetUseProton(){
  static bool isSet = false;
  static bool usep  = false;
  if (!isSet){
    usep = FitPar::Config().GetParB("UseProton");
    isSet = true;
  }
  return usep;
}

bool SciBooNEUtils::GetUseZackEff(){
  static bool isSet = false;
  static bool use   = false;
  if (!isSet){
    use = FitPar::Config().GetParB("UseZackEff");
    isSet = true;
  }
  return use;
}

double SciBooNEUtils::GetFlatEfficiency(){
  static double var = 0xdeadbeef;
  if (var == 0xdeadbeef){
    var = FitPar::Config().GetParD("FlatEfficiency");
  }
  return var;
}


// Obtained from a simple fit to test beam data 1 < p < 2 GeV
double SciBooNEUtils::ProtonMisIDProb(double mom){
  return 0.1;
  double prob = 0.10;
  if (mom < 1) return prob;
  if (mom > 2) mom = 2;
  
  prob = -2.83 + 3.75*mom - 0.96*mom*mom;
  if (prob < 0.10) prob = 0.10;
  return prob;
}

// This function uses pion-scintillator cross sections to calculate the pion SI probability
double SciBooNEUtils::PionReinteractionProb(double energy, double thickness){
  static TGraph *total_xsec = 0;
  static TGraph *inel_xsec  = 0;

  if (!total_xsec){
    total_xsec = PlotUtils::GetTGraphFromRootFile(FitPar::GetDataBase()+"/SciBooNE/cross_section_pion_scintillator_hd.root", "totalXS");
  }
  if (!inel_xsec){
    inel_xsec = PlotUtils::GetTGraphFromRootFile(FitPar::GetDataBase()+"/SciBooNE/cross_section_pion_scintillator_hd.root", "inelXS");
  }

  if (total_xsec->Eval(energy) == 0) return 0;
  double total = total_xsec->Eval(energy)*1E-27;
  double inel  = inel_xsec->Eval(energy)*1E-27;

  double prob = (1 - exp(-thickness*SciBooNEUtils::GetSciBarDensity()*4.63242e+22*total))*(inel/total);
  return prob;
}

bool SciBooNEUtils::ThrowAcceptReject(double test_value, double ceiling){
  static TRandom3 *rand = 0;

  if (!rand){
    rand = new TRandom3(0);
  }
  double throw_value = rand->Uniform(ceiling);
  
  if (throw_value < test_value) return false;
  return true;
}

double SciBooNEUtils::StoppedEfficiency(TH2D *effHist, FitParticle *nu, FitParticle *muon){

  double eff = 0.;

  if (!effHist) return eff;
  // For Morgan's efficiencies
  if (!SciBooNEUtils::GetUseZackEff()) eff = effHist->GetBinContent(effHist->GetXaxis()->FindBin(FitUtils::p(muon)), 
							      effHist->GetYaxis()->FindBin(FitUtils::th(nu, muon)/TMath::Pi()*180.));
  // For Zack's efficiencies
  else eff = effHist->GetBinContent(effHist->GetXaxis()->FindBin(FitUtils::th(nu, muon)/TMath::Pi()*180.), 
				    effHist->GetYaxis()->FindBin(FitUtils::p(muon)*1000.));
  return eff;
}

double SciBooNEUtils::ProtonEfficiency(TH2D *effHist, FitParticle *nu, FitParticle *muon){

  double eff = 0.;

  if (!effHist) return eff;
  eff = effHist->GetBinContent(effHist->GetXaxis()->FindBin(FitUtils::th(nu, muon)/TMath::Pi()*180.), effHist->GetYaxis()->FindBin(FitUtils::p(muon)*1000.));
  return eff;
}


double SciBooNEUtils::PenetratedEfficiency(FitParticle *nu, FitParticle *muon){

  double eff = 0.;

  if (FitUtils::th(nu, muon)/TMath::Pi()*180. > 50) eff = 0.;
  if (FitUtils::p(muon) < SciBooNEUtils::GetPenetratingMuonE()) eff = 0.;

  return eff;
}

double SciBooNEUtils::BetheBlochCH(double E, double mass){

  double beta2 = 1 - mass*mass/E/E;
  double gamma = 1./sqrt(1-beta2);
  double mass_ratio = PhysConst::mass_electron*1000./mass;
  // Argh, have to remember to convert to MeV or you'll hate yourself!
  double I2 = 68.7e-6*68.7e-6;

  double w_max = 2*PhysConst::mass_electron*1000.*beta2*gamma*gamma;
  w_max /= 1 + 2*gamma*mass_ratio + mass_ratio*mass_ratio;

  // Values taken from the PDG for K = 0.307075 MeV mol-1 cm2, mean ionization energy I = 68.7 eV (Polystyrene)
  // <Z/A> = 0.53768 (pdg.lbl.gov/AtomicNuclearProperties)
  double log_term = log(2*PhysConst::mass_electron*1000.*beta2*gamma*gamma*w_max/I2);
  double dedx = 0.307075*0.53768/beta2*(0.5*log_term - beta2);

  return dedx;
}


// This function returns an estimate of the range of the particle in scintillator.
// It uses crude integration and Bethe-Bloch to approximate the range.
double SciBooNEUtils::RangeInScintillator(FitParticle* particle, int nsteps){

  // The particle energy
  double E  = particle->fP.E();
  double M  = particle->fP.M();
  double Ek = E - M;

  double step_size = Ek/float(nsteps+1);
  double range = 0;

  // Add an offset to make the integral a touch more accurate
  Ek -= step_size/2.;
  for (int i = 0; i < nsteps; ++i){
    
    double dEdx = SciBooNEUtils::BetheBlochCH(Ek+M, M);
    
    Ek -= step_size;
    // dEdx is -ve
    range -= step_size/dEdx;

    // If the particle is a pion. Also consider the reinteraction probability
    if (abs(particle->fPID) == 211){
      double prob = SciBooNEUtils::PionReinteractionProb(Ek, step_size/dEdx/SciBooNEUtils::GetSciBarDensity());
      if (!SciBooNEUtils::ThrowAcceptReject(prob)) break;
    }
  }

  // Account for density of polystyrene
  range /= SciBooNEUtils::GetSciBarDensity();

  // Range estimate is in cm
  return range;
}


// Function to calculate the distance the particle travels in scintillator
bool SciBooNEUtils::PassesDistanceCut(FitParticle* beam, FitParticle* particle){

  // First apply some basic thresholds (from K2K SciBar description)
  if (FitUtils::p(particle) < 0.15) return false;
  if (particle->fPID == 2212 && FitUtils::p(particle) < 0.45) return false;

  double dist  = SciBooNEUtils::RangeInScintillator(particle, SciBooNEUtils::GetNumRangeSteps());
  double zdist = dist*cos(FitUtils::th(beam, particle));

  // Allow for vertex migration for backward tracks
  //if (zdist < 0) zdist -= 2;

  if (abs(zdist) < SciBooNEUtils::GetSciBarRecoDist()) return false;
  return true;
}

int SciBooNEUtils::isProton(FitParticle* track){
  if (track->fPID == 2212) return true;
  return false;
}

// Function to return the MainTrk
int SciBooNEUtils::GetMainTrack(FitEvent *event, TH2D *mupiHist, TH2D *protonHist, FitParticle*& mainTrk, double& weight, bool penetrated){

  FitParticle *nu     = event->GetNeutrinoIn();
  int index           = 0;
  int indexPr         = 0;
  double highWeight   = 0;
  double highWeightPr = 0;
  double runningWeight= 0;
  mainTrk = NULL;

  // Loop over particles
  for (uint j = 2; j < event->Npart(); ++j){

    // Final state only!
    if (!(event->PartInfo(j))->fIsAlive) continue;
    if (event->PartInfo(j)->fNEUTStatusCode != 0) continue;

    int PID = event->PartInfo(j)->fPID;

    // Only consider pions, muons for now
    if (abs(PID) != 211 && abs(PID) != 13 && PID != 2212) continue;
    if (!SciBooNEUtils::GetUseProton() && PID == 2212) continue;

    // Get the track with the highest weight
    double thisWeight = 0;

    if (PID == 2212) {
      thisWeight = SciBooNEUtils::ProtonEfficiency(protonHist, nu, event->PartInfo(j));
      if (thisWeight == 0) continue;

      if (runningWeight == 0) runningWeight = thisWeight;
      else runningWeight += (1 - runningWeight)*thisWeight;
      
      if (thisWeight < highWeightPr) continue;      
      highWeightPr = thisWeight;
      indexPr = j;
      
    } else {
      thisWeight = SciBooNEUtils::StoppedEfficiency(mupiHist, nu, event->PartInfo(j));
      if (thisWeight == 0) continue;

      if (runningWeight == 0) runningWeight = thisWeight;
      else runningWeight += (1 - runningWeight)*thisWeight;

      if (thisWeight < highWeight) continue;

      // Add a range calculation for pi+
      if (abs(PID) == 211){
	double range = SciBooNEUtils::RangeInScintillator(event->PartInfo(j));
	if (abs(range) < SciBooNEUtils::GetMainPionRange()) continue;
      }
      highWeight = thisWeight;
      index   = j;
    }
  } // end loop over particle stack
  
  // Use MuPi if it's there, if not, use proton info
  if (highWeightPr > highWeight){
    highWeight = highWeightPr;
    index  = indexPr;
  }

  // Pass the weight back (don't want to apply a weight twice by accident)
  mainTrk = event->PartInfo(index);
  weight *= highWeight;
  //weight *= runningWeight;
  //std::cout << "High weight = " << highWeight << "; running weight = " << runningWeight << std::endl;

  return index;
}


void SciBooNEUtils::GetOtherTrackInfo(FitEvent *event, int mainIndex, int& nProtons, int& nPiMus, int& nVertex, FitParticle*& secondTrk){
  
  // Reset everything
  nPiMus      = 0;
  nProtons    = 0;
  nVertex     = 0;
  secondTrk   = NULL;

  if (mainIndex == 0) return;

  double highestMom  = 0.;

  // Loop over particles
  for (uint j = 2; j < event->Npart(); ++j){

    // Don't re-count the main track
    if (j == (uint)mainIndex) continue;

    // Final state only!
    if (!(event->PartInfo(j))->fIsAlive) continue;
    if (event->PartInfo(j)->fNEUTStatusCode != 0) continue;

    int PID = event->PartInfo(j)->fPID;

    // Only consider pions, muons, protons
    if (abs(PID) != 211 && PID != 2212 && abs(PID) != 13) continue;

    // Must be reconstructed as a track in SciBooNE, and pass inefficiency cut
    if (SciBooNEUtils::PassesDistanceCut(event->PartInfo(0), event->PartInfo(j)) && !SciBooNEUtils::ThrowAcceptReject(SciBooNEUtils::GetFlatEfficiency())){

      // Keep track of the second highest momentum track
      if (FitUtils::p(event->PartInfo(j)) > highestMom){
	highestMom = FitUtils::p(event->PartInfo(j));
	secondTrk  = event->PartInfo(j);
      }

      if (PID == 2212) nProtons += 1;
      else nPiMus  += 1;
      // Ignore backward tracks for VA
    } else if ( FitUtils::th(event->PartInfo(0), event->PartInfo(j))/TMath::Pi() < 0.5)
      nVertex += 1;

  } // end loop over particle stack

  return;
}

double SciBooNEUtils::apply_smear(double central, double width){
  static TRandom3 *rand = 0;
  
  if (!rand){
    rand = new TRandom3(0);
  }
  double output = rand->Gaus(central, width);
  return output;
}

double SciBooNEUtils::smear_p(FitParticle* track, double smear){
  static TF1 *f1 = new TF1("f1", "gaus(0)+gaus(3)", -0.8, 0.8);
  static bool set_pars = false;
  if (!set_pars){
    f1->SetParameter(0, 1275.87);
    f1->SetParameter(1, -0.0160104);
    f1->SetParameter(2, 0.0424547);
    f1->SetParameter(3, 116.157);
    f1->SetParameter(4, -0.0287358);
    f1->SetParameter(5, 0.157022);
    set_pars = true;
  }

  double mod_mom = FitUtils::p(track) + f1->GetRandom();
  return mod_mom;
}

double SciBooNEUtils::smear_th(FitParticle* track1, FitParticle* track2, double smear){
  static TF1 *f1 = new TF1("f1", "gaus(0)+gaus(3)", -15, 15);
  static bool set_pars = false;
  if (!set_pars){
    f1->SetParameter(0, 1878.41);
    f1->SetParameter(1, 0.0622622);
    f1->SetParameter(2, 0.869508);
    f1->SetParameter(3, 616.559);
    f1->SetParameter(4, 0.138734);
    f1->SetParameter(5, 1.96287);
    set_pars = true;
  }

  double mod_th = FitUtils::th(track1, track2) + f1->GetRandom()*TMath::Pi()/180;
  return mod_th;
}


// NOTE: need to adapt this to allow for penetrating events...
// Simpler, but gives the same results as in Hirade-san's thesis
double SciBooNEUtils::CalcThetaPr(FitEvent *event, FitParticle *main, FitParticle *second, bool penetrated){
  
  FitParticle *nu   = event->GetNeutrinoIn();

  if (!main || !nu || !second) return -999;

  // Construct the vector p_pr = (-p_mux, -p_muy, Enurec - pmucosthetamu)
  // where p_mux, p_muy are the projections of the candidate muon momentum onto the x and y dimension respectively
  double pmu   = main->fP.Vect().Mag();
  double pmu_x = main->fP.Vect().X();
  double pmu_y = main->fP.Vect().Y();
  
  double theta_s = cos(FitUtils::th(nu, main));
  
  double Enuqe = FitUtils::EnuQErec(pmu/1000.,theta_s, 27., true)*1000.;
  double p_pr_z = Enuqe - pmu*theta_s;

  TVector3 p_pr  = TVector3(-pmu_x, -pmu_y, p_pr_z);
  double thetapr = p_pr.Angle(second->fP.Vect())/TMath::Pi()*180.;

  return thetapr;
}

double SciBooNEUtils::CalcThetaPi(FitEvent *event, FitParticle *second){

  FitParticle *nu   = event->GetNeutrinoIn();

  if (!second || !nu) return -999;

  double thetapi = FitUtils::th(nu, second)/TMath::Pi()*180.;
  return thetapi;
}

/// Functions to deal with the SB mode stacks
SciBooNEUtils::ModeStack::ModeStack(std::string name, std::string title, TH1* hist) {
  fName = name;
  fTitle = title;

  AddMode(0, "CCCOH",  "CCCOH", kGreen+2, 2, 3244);
  AddMode(1, "CCRES",  "CCRES", kRed,     2, 3304);
  AddMode(2, "CCQE",   "CCQE",  kGray+2,  2, 1001);
  AddMode(3, "2p2h",   "2p2h",  kMagenta, 2, 1001);
  AddMode(4, "Other",  "Other", kAzure+1, 2, 1001);
  
  StackBase::SetupStack(hist);
};

int SciBooNEUtils::ModeStack::ConvertModeToIndex(int mode){
  switch (abs(mode)){
  case 16: return 0; // CCCOH
  case 11:
  case 12:
  case 13: return 1; // CCRES
  case  1: return 2; // CCQE
  case  2: return 3; // 2p2h
  default: return 4; // Other
  }
};

void SciBooNEUtils::ModeStack::Fill(int mode, double x, double y, double z, double weight) {
  StackBase::FillStack(SciBooNEUtils::ModeStack::ConvertModeToIndex(mode), x, y, z, weight);
};

void SciBooNEUtils::ModeStack::Fill(FitEvent* evt, double x, double y, double z, double weight) {
  StackBase::FillStack(SciBooNEUtils::ModeStack::ConvertModeToIndex(evt->Mode), x, y, z, weight);
};

void SciBooNEUtils::ModeStack::Fill(BaseFitEvt* evt, double x, double y, double z, double weight) {
  StackBase::FillStack(SciBooNEUtils::ModeStack::ConvertModeToIndex(evt->Mode), x, y, z, weight);
};

// Functions to deal with Main track PID stack
SciBooNEUtils::MainPIDStack::MainPIDStack(std::string name, std::string title, TH1* hist) {
  fName = name;
  fTitle = title;

  AddMode(0, "mu",  "#mu^{-}", kGreen+2, 2, 3244);
  AddMode(1, "pip",  "#pi^{+}", kRed,     2, 3304);
  AddMode(2, "pim",  "#pi^{-}",  kGray+2,  2, 1001);
  AddMode(3, "proton",   "p",  kMagenta, 2, 1001);
  AddMode(4, "Other",  "Other", kAzure+1, 2, 1001);
  StackBase::SetupStack(hist);
};

int SciBooNEUtils::MainPIDStack::ConvertPIDToIndex(int PID){
  switch (PID){
  case   13: return 0;
  case  211: return 1;
  case -211: return 2;
  case 2212: return 3;
  default:   return 4;
  }
};

void SciBooNEUtils::MainPIDStack::Fill(int PID, double x, double y, double z, double weight) {
  StackBase::FillStack(SciBooNEUtils::MainPIDStack::ConvertPIDToIndex(PID), x, y, z, weight);
};


// Functions to deal with second type of mode breakdown (from the thesis)
SciBooNEUtils::ModeStack2::ModeStack2(std::string name, std::string title, TH1* hist) {
  fName = name;
  fTitle = title;

  AddMode(0, "CCCOH",  "#nu CC coh. #pi", kGreen+2, 2, 3244);
  AddMode(1, "CCRES", "#nu CC res. #pi", kRed,     2, 3304);
  AddMode(2, "ANTINU", "#bar{#nu} BG",  kMagenta,  2, 1001);
  AddMode(3, "NC", "#nu NC",  kYellow, 2, 1001);
  AddMode(4, "CCOTHER", "#nu CC other", kBlue+2, 2, 1001);
  AddMode(5, "CCQE", "#nu CCQE", kBlack, 2, 1001);
  AddMode(6, "2p2h", "#nu 2p2h", kGray+1, 2, 1001);
  StackBase::SetupStack(hist);
};

int SciBooNEUtils::ModeStack2::ConvertModeToIndex(int mode){

  // Catch wrong sign contribution
  if (mode < 0) return 2;
  // Catch NC contributions
  if (mode > 30) return 3;
  switch (abs(mode)){
  case 16: return 0; // CCCOH
  case 11:
  case 12:
  case 13: return 1; // CCRES
  case  1: return 5; // CCQE
  case  2: return 6; // 2p2h
  default: return 4; // Other
  }
};

void SciBooNEUtils::ModeStack2::Fill(int mode, double x, double y, double z, double weight) {
  StackBase::FillStack(SciBooNEUtils::ModeStack2::ConvertModeToIndex(mode), x, y, z, weight);
};

void SciBooNEUtils::ModeStack2::Fill(FitEvent* evt, double x, double y, double z, double weight) {
  StackBase::FillStack(SciBooNEUtils::ModeStack2::ConvertModeToIndex(evt->Mode), x, y, z, weight);
};

void SciBooNEUtils::ModeStack2::Fill(BaseFitEvt* evt, double x, double y, double z, double weight) {
  StackBase::FillStack(SciBooNEUtils::ModeStack2::ConvertModeToIndex(evt->Mode), x, y, z, weight);
};


