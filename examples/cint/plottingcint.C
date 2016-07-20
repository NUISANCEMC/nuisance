plottingcint(int nstart=0, int nwidth=-1, int ident=0){

  // Test Use of External Fitter Libraries within CINT to handle generators.
  // Should allow for faster sample prototyping and for additional plots to be made.
  // P. Stowell
  gROOT->ProcessLine(".X LoadFitLibraries_CINT.C");
  FitPar::Config().SetParI("MAXEVENTS",-1); // Global Overrides
  
  // Setup the RW Engine
  FitWeight* rw = new FitWeight("FitWeight_Upper");
  int kNEUTDial  = 0;
  int kNIWGDial  = 1;
  int kNuwroDial = 2; // -> Note: Make ENUM DEF in FitBase
  int kGENIEDial = 3;
  rw->IncludeDial("kNuwro_Ma_CCQE", kNuwroDial, -2.0);
  rw->Reconfigure();

  // Setup the Input File (Using Convertor)
  InputHandler* input = new InputHandler("nuwro_input",  "NUWRO:simulation.root" );

  // Inputhandler works a little bit like a TTree. We get a pointer the custom event
  // and then we call readevent.
  FitEvent* evt = input->GetEventPointer();
  int nevents   = input->GetNEvents();
  
  // Start Event Loop
  for (int i = 0; i < nevents; i++){

    input->ReadEvent(i);    
    if (i % 10000 == 0) std::cout<<"Processed "<<i<<" events."<<std::endl;

    // EVENT REWEIGHTING
    // -- START
    rw_weight = rw->CalcWeight( evt );
    // -- END

    mode = evt->Mode;
    

    // SIGNAL DEFINITION
    // -- START
    // DEFINE THESE BY LOOKING AT PARTICLES
    signal = -1; // OTHER CRAP
    
    
    nmuons = 0;
    npions = 0;
    npionplus = 0;
    npionneut = 0;
    npionminus = 0;
    nprotons = 0;
    nelectrons = 0;
    npositrons = 0;
    nother = 0;
    nneutrons = 0;
    

    for (int j = 0; j < evt->Npart(); j++){

      // Only get particles that are alive at the end
      if (!evt->PartInfo(j)->fIsAlive) continue; 
      if (evt->PartInfo(j)->fStatus != 0) continue;
      
      int p_PID = evt->PartInfo(j)->fPID;

      if (p_PID == -211 ||
	  p_PID == 211  || 
	  p_PID == 111) npions += 1;
      
      
      if (p_PID == -211) npionminus += 1;
      if (p_PID == 211) npionplus += 1;
      if (p_PID == 111) npionneut += 1;
      else if (p_PID == 2212) nprotons += 1;
      else if (p_PID == 11) nelectrons += 1;
      else if (p_PID == -11) npositrons += 1;
      else if (p_PID == 2112) nneutrons += 1; 
      else if (p_PID == 13) nmuons+=1;
      else if (p_PID != -211 &&
	  p_PID != 211 &&
	  p_PID != 111 &&
	  p_PID != 2212 &&
	  p_PID != 11 &&
	  p_PID != -11 &&
	  p_PID != 2112 &&
	  p_PID != 13) nother += 1;
    
    }
    
    // Define Signal Now
    if (nmuons > 0 && npions == 0) signal = 0; // CC0PI
    else if (nmuons > 0 && npions == 1) signal = 1; // CC1PI
    else if (nmuons > 0) signal = 2; // CCOTHER
    else signal = -1;
      
    if (signal == -1) continue; // SKIP NON CC SIGNALS
    // -- END


    // Fill Variables

    double theta_mu, t_mom_mu, l_mom_mu, theta_p, t_mom_p, l_mom_p, theta_n, theta_pip, theta_pin, theta_pim, Mmu, Mp, Mn, Mpip, Mpin, Mpim, R_l, R_t, KEp_max;
    TVector3* three_mom_mu = NULL;
    TVector3* three_mom_p  = NULL;
    TLorentzVector* q = NULL;

    TVector3* mu = NULL;
    Mmu  = kUnassigned;
    Mp   = kUnassigned;
    Mn   = kUnassigned;
    Mpip = kUnassigned;
    Mpin = kUnassigned;
    Mpim = kUnassigned;
    TEnu  = kUnassigned;
    TEmu  = kUnassigned;
    TEp   = kUnassigned;
    TEn   = kUnassigned;
    TEpip = kUnassigned;
    TEpin = 0;
    TEpim = kUnassigned;
    Pmu  = kUnassigned;
    Pp   = kUnassigned;
    Pn   = kUnassigned;
    Ppip = kUnassigned;
    Ppin = kUnassigned;
    Ppim = kUnassigned;
    cos_theta_mu  = kUnassigned;
    cos_theta_p   = kUnassigned;
    cos_theta_n   = kUnassigned;
    cos_theta_pip = kUnassigned;
    cos_theta_pin = kUnassigned;
    cos_theta_pim = kUnassigned;
    KEp   = 0;
    KEn   = 0;
    KEpip = kUnassigned;
    KEpin = kUnassigned;
    KEpim = kUnassigned;
    q0 = kUnassigned;
    q3 = kUnassigned;
    E_QE = kUnassigned;
    Q2 = kUnassigned;
    Ehad_true = kUnassigned;
    Ehad_reco = kUnassigned;
    MN = kUnassigned;
    TEpip_tot = 0;
    TEpim_tot = 0;
    KEpip_tot = 0;
    KEpim_tot = 0;
    KEp_tot = 0;
    KEn_tot = 0;
    KEp_max = 0;
    TEpin_tot = 0;
    float Mne = 0.939; // Used later
    float Mpr = 0.938; // USed later, hard coded for simplicity

 
    // -- START
    TLorentzVector q_temp = TLorentzVector();
    for (int j = 0; j < evt->Npart(); j++){
   
      // Only get particles that are alive at the end
      if (!evt->PartInfo(j)->fIsAlive) continue; 
      if (evt->PartInfo(j)->fStatus != 0) continue;

      int pdg_part = evt->PartInfo(j)->fPID;
      TLorentzVector p_part = (evt->PartInfo(j)->fP);
      TLorentzVector p_neut = (evt->PartInfo(0)->fP);

      if (pdg_part == 13){
	mu = &(p_part.Vect());
	Mmu = p_part.Mag()/1000.0;
	TEmu = p_part.E()/1000.0;
	Pmu = p_part.Vect().Mag()/1000.0;
	theta_mu = p_part.Vect().Angle(p_neut.Vect());
	cos_theta_mu = cos(theta_mu);
	if (TEmu != kUnassigned) KEmu = TEmu - Mmu;
	three_mom_mu = &(p_part.Vect());
	t_mom_mu = sqrt((three_mom_mu->X())**2 + (three_mom_mu->Y())**2);
	l_mom_mu = three_mom_mu->Z();
	q_temp = (p_part -p_neut); // Used for q0 and q3
	q = &q_temp;
      }

      else if (pdg_part == 2212){
	Mp = p_part.Mag()/1000.0;
	TEp = p_part.E()/1000.0;
	Pp = p_part.Vect().Mag()/1000.0;
	theta_p = p_part.Vect().Angle(p_neut.Vect());
	cos_theta_p = cos(theta_p);
	if (TEp != kUnassigned){
	  KEp = TEp - Mp;
	}
	else (KEp = 0);
        KEp_tot += KEp;
	if (KEp > KEp_max) KEp_max = KEp;
	three_mom_p = &p_part.Vect();
	t_mom_p = sqrt((three_mom_p->X())**2 + (three_mom_p->Y())**2);
	l_mom_p = three_mom_p->Z();
      }
      
      else if (pdg_part == 2112){
	Mn = p_part.Mag()/1000.0;
	TEn = p_part.E()/1000.0;
	Pn = p_part.Vect().Mag()/1000.0;
	theta_n = p_part.Vect().Angle(p_neut.Vect());
	cos_theta_n = cos(theta_n);
	if (TEn != kUnassigned){
	  KEn = TEn - Mn;
	  KEn_tot += KEn;
	}
      }
      else if (pdg_part == 211){
	Mpip = p_part.Mag()/1000.0;
	Ppip = p_part.Vect().Mag()/1000.0;
	TEpip = p_part.E()/1000.0;
	TEpip_tot += TEpip;
	theta_pip = p_part.Vect().Angle(p_neut.Vect());
	cos_theta_pip[0] = cos(theta_pip);
	if (TEpip != kUnassigned){
	  KEpip = TEpip - Mpip;
	}
	else (KEpip = 0);
	KEpip_tot += KEpip;
      }
      else if (pdg_part == 111){
	Mpin = p_part.Mag()/1000.0;
	TEpin = p_part.E()/1000.0;
	Ppin = p_part.Vect().Mag()/1000.0;
	theta_pin = p_part.Vect().Angle(p_neut.Vect());
	cos_theta_pin = cos(theta_pin);
	if (TEpin != kUnassigned){ 
	  KEpin = TEpin - Mpin;
	}
	else (KEpin = 0);
	TEpin_tot += TEpin;
      }
      else if (pdg_part == -211){
	Mpim = p_part.Mag()/1000.0;
	TEpim = p_part.E()/1000.0;
	TEpim_tot += TEpim;
	Ppim = p_part.Vect().Mag()/1000.0;
	theta_pim = p_part.Vect().Angle(p_neut.Vect());
	cos_theta_pim = cos(theta_pim);
	if (TEpim != kUnassigned){
	  KEpim = TEpim - Mpim;
	}
	else (KEpim = 0);
	KEpim_tot += KEpim;
      } 
      
    }
   
    if (!mu) continue; // If no muon found skip the event
    
    TEnu = p_neut.E()/1000.0; // Total Energy of the neutrino

    // Calculate q0 and q3 using the muon as is the most accurate
    q0 = q->E()/1000.0;
    q3 = q->Vect().Mag()/1000.0;    
 
    
    // Get Q2 from event (Q2 = -q2)
    Q2 = -1.0 * (q->Mag2())/1000000.0;

    // Find the total energy of the end state hadrons
    // True
    Ehad_true = KEp_tot + KEn_tot + TEpip_tot + TEpin_tot + TEpim_tot;
    // False
    Ehad_reco = KEp_tot + KEpip_tot + KEpim_tot + TEpin_tot;

    // Average nucleon mass
    MN = (Mpr + Mne)/2.0;

    
    // Plot Q2 for different interaction modes 
    float Eb = 0.034;
    Q2_qel_p = (Mne - Eb)**2 - Mpr**2 + 2*(Mne - Eb)*(KEp_max + Mpr - Mne + Eb);
    Q2_qel_mu = -Mmu**2 + 2*TEnu*(TEmu - sqrt(TEmu**2 - Mmu**2)*cos_theta_mu);
    Q2_dis_mu = 4*TEnu*TEmu*(sin(theta_mu/2))**2;

    // Sorting out the transverse stuff
    if (three_mom_p != kUnassigned){
      transverse = (t_mom_mu + t_mom_p);
    
      R_l = l_mom_mu + l_mom_p;
      R_t = t_mom_mu + t_mom_p;
      Rt_Rl = R_t / R_l;
    }
    
    // -- END
 
    // Final Fill
    myTree->Fill();
  }


  // Save the TTree
  myTree->Write();
  outFile->Close();

  exit(-1);

}

