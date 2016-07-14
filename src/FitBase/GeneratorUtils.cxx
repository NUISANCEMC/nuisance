#include "GeneratorUtils.h"

#ifdef __NEUT_ENABLED__
void GeneratorUtils::FillNeutCommons(NeutVect* nvect){

  // WARNING: This has only been implemented for a neuttree and not GENIE
  // This should be kept in sync with T2KNIWGUtils::GetNIWGEvent(TTree)

  //NEUT version info.  Can't get it to compile properly with this yet
  //neutversion_.corev  =   nvect->COREVer;
  //neutversion_.nucev  =   nvect->NUCEVer;
  //neutversion_.nuccv  =   nvect->NUCCVer;

  // Documentation: See nework.h
  nework_.modene = nvect->Mode;
  nework_.numne = nvect->Npart();

  nemdls_.mdlqeaf = nvect->QEVForm;
  nemdls_.mdlqe = nvect->QEModel;
  nemdls_.mdlspi = nvect->SPIModel;
  nemdls_.mdldis = nvect->DISModel;
  nemdls_.mdlcoh = nvect->COHModel;
  neutcoh_.necohepi = nvect->COHModel;

  nemdls_.xmaqe = nvect->QEMA;
  nemdls_.xmvqe = nvect->QEMV;
  nemdls_.kapp  = nvect->KAPPA;

  //nemdls_.sccfv = SCCFVdef;
  //nemdls_.sccfa = SCCFAdef;
  //nemdls_.fpqe = FPQEdef;

  nemdls_.xmaspi = nvect->SPIMA;
  nemdls_.xmvspi = nvect->SPIMV;
  nemdls_.xmares = nvect->RESMA;
  nemdls_.xmvres = nvect->RESMV;

  neut1pi_.xmanffres = nvect->SPIMA;
  neut1pi_.xmvnffres = nvect->SPIMV;
  neut1pi_.xmarsres = nvect->RESMA;
  neut1pi_.xmvrsres = nvect->RESMV;
  neut1pi_.neiff    = nvect->SPIForm;
  neut1pi_.nenrtype = nvect->SPINRType;
  neut1pi_.rneca5i  = nvect->SPICA5I;
  neut1pi_.rnebgscl = nvect->SPIBGScale;

  nemdls_.xmacoh = nvect->COHMA;
  nemdls_.rad0nu = nvect->COHR0;
  nemdls_.fa1coh = nvect->COHA1err;
  nemdls_.fb1coh = nvect->COHb1err;

  //neutdis_.nepdf = NEPDFdef;
  //neutdis_.nebodek = NEBODEKdef;

  neutcard_.nefrmflg  = nvect->FrmFlg;
  neutcard_.nepauflg  = nvect->PauFlg;
  neutcard_.nenefo16  = nvect->NefO16;
  neutcard_.nemodflg  = nvect->ModFlg;
  //neutcard_.nenefmodl = 1;
  //neutcard_.nenefmodh = 1;
  //neutcard_.nenefkinh = 1;
  //neutpiabs_.neabspiemit = 1;

  nenupr_.iformlen    =  nvect->FormLen;

  neutpiless_.ipilessdcy = nvect->IPilessDcy;
  neutpiless_.rpilessdcy = nvect->RPilessDcy;


  neutpiless_.ipilessdcy = nvect->IPilessDcy;
  neutpiless_.rpilessdcy = nvect->RPilessDcy;

  neffpr_.fefqe   = nvect->NuceffFactorPIQE;
  neffpr_.fefqeh  = nvect->NuceffFactorPIQEH;
  neffpr_.fefinel = nvect->NuceffFactorPIInel;
  neffpr_.fefabs  = nvect->NuceffFactorPIAbs;
  neffpr_.fefcx   = nvect->NuceffFactorPICX;
  neffpr_.fefcxh  = nvect->NuceffFactorPICXH;

  neffpr_.fefcoh =  nvect->NuceffFactorPICoh;
  neffpr_.fefqehf = nvect->NuceffFactorPIQEHKin;
  neffpr_.fefcxhf = nvect->NuceffFactorPICXKin;
  neffpr_.fefcohf = nvect->NuceffFactorPIQELKin;

  for ( int i = 0; i<nework_.numne; i++ ) {
    nework_.ipne[i] = nvect->PartInfo(i)->fPID;
    nework_.pne[i][0] = (float)nvect->PartInfo(i)->fP.X()/1000;  // VC(NE)WORK in M(G)eV
    nework_.pne[i][1] = (float)nvect->PartInfo(i)->fP.Y()/1000;  // VC(NE)WORK in M(G)eV
    nework_.pne[i][2] = (float)nvect->PartInfo(i)->fP.Z()/1000;  // VC(NE)WORK in M(G)eV
  }
  // fsihist.h


  // neutroot fills a dummy object for events with no FSI to prevent memory leak when
  // reading the TTree, so check for it here
    
  if ( (int)nvect->NfsiVert() == 1 ) { // An event with FSI must have at least two vertices
    //    if (nvect->NfsiPart()!=1 || nvect->Fsiprob!=-1)
      //      cout << "Warning: T2KNeutUtils::fill_neut_commons(TTree) NfsiPart!=1 or Fsiprob!=-1 when NfsiVert==1" << endl;
      
    fsihist_.nvert = 0;
    fsihist_.nvcvert = 0;
    fsihist_.fsiprob = 1;
  }
  else { // Real FSI event
    fsihist_.nvert = (int)nvect->NfsiVert();
    for (int ivert=0; ivert<fsihist_.nvert; ivert++) {
      fsihist_.iflgvert[ivert] = nvect->FsiVertInfo(ivert)->fVertID;
      fsihist_.posvert[ivert][0] = (float)nvect->FsiVertInfo(ivert)->fPos.X();
      fsihist_.posvert[ivert][1] = (float)nvect->FsiVertInfo(ivert)->fPos.Y();
      fsihist_.posvert[ivert][2] = (float)nvect->FsiVertInfo(ivert)->fPos.Z();
    }
      
    fsihist_.nvcvert = nvect->NfsiPart();
    for (int ip=0; ip<fsihist_.nvcvert; ip++) {
      fsihist_.abspvert[ip] = (float)nvect->FsiPartInfo(ip)->fMomLab;
      fsihist_.abstpvert[ip] = (float)nvect->FsiPartInfo(ip)->fMomNuc;
      fsihist_.ipvert[ip] = nvect->FsiPartInfo(ip)->fPID;
      fsihist_.iverti[ip] = nvect->FsiPartInfo(ip)->fVertStart;
      fsihist_.ivertf[ip] = nvect->FsiPartInfo(ip)->fVertEnd;
      fsihist_.dirvert[ip][0] = (float)nvect->FsiPartInfo(ip)->fDir.X();
      fsihist_.dirvert[ip][1] = (float)nvect->FsiPartInfo(ip)->fDir.Y();
      fsihist_.dirvert[ip][2] = (float)nvect->FsiPartInfo(ip)->fDir.Z();
    }
    fsihist_.fsiprob = nvect->Fsiprob;
  }

  neutcrscom_.crsx = nvect->Crsx;
  neutcrscom_.crsy = nvect->Crsy;
  neutcrscom_.crsz = nvect->Crsz;
  neutcrscom_.crsphi = nvect->Crsphi;
  neutcrscom_.crsq2 = nvect->Crsq2;
    
  neuttarget_.numbndn = nvect->TargetA - nvect->TargetZ;
  neuttarget_.numbndp = nvect->TargetZ;
  neuttarget_.numfrep = nvect->TargetH;
  neuttarget_.numatom = nvect->TargetA;
  posinnuc_.ibound = nvect->Ibound;
    
  // put empty nucleon FSI history (since it is not saved in the NeutVect format)
  //Comment out as NEUT does not have the necessary proton FSI information yet
  //  nucleonfsihist_.nfnvert = 0;
  //  nucleonfsihist_.nfnstep = 0; 
  

}
#endif

#ifdef __NIWG_ENABLED__
niwg::rew::NIWGEvent * GeneratorUtils::GetNIWGEvent(NeutVect* nvect)
{
  niwg::rew::NIWGEvent * fDummyNIWGEvent = NULL;

  fDummyNIWGEvent = new niwg::rew::NIWGEvent();
  fDummyNIWGEvent->detid = 1;   // MiniBooNE (apply CCQE LowE variations)
  fDummyNIWGEvent->neutmode = nvect->Mode;
  fDummyNIWGEvent->targetA = nvect->TargetA;
  fDummyNIWGEvent->recenu_ccqe_sk = -1;
  if (nvect->Ibound==0) fDummyNIWGEvent->targetA = 1;//RT: identifies as H, rather than O16

  // Fill initial particle stack
  for (int ip=0; ip<nvect->Npart(); ++ip) {

    niwg::rew::NIWGPartStack fDummyPartStack;

    fDummyPartStack.p = (nvect->PartInfo(ip)->fP)*0.001;  // Convert to GeV

    fDummyPartStack.pdg = nvect->PartInfo(ip)->fPID;
    fDummyPartStack.chase = nvect->PartInfo(ip)->fIsAlive;
    fDummyPartStack.parent = nvect->ParentIdx(ip)-1;       // WARNING: this needs to be tested with a NeutRoot file

    fDummyNIWGEvent->part_stack.push_back(fDummyPartStack);
  }
  fDummyNIWGEvent->CalcKinematics();

  return fDummyNIWGEvent;
}
#endif // neut enabled     


#ifdef __NUWRO_ENABLED__
//*************************************************** 
int GeneratorUtils::ConvertNuwroMode (event * e)
//*************************************************** 
{


  Int_t proton_pdg, neutron_pdg, pion_pdg, pion_plus_pdg, pion_minus_pdg,
    lambda_pdg, eta_pdg, kaon_pdg, kaon_plus_pdg;
  proton_pdg = 2212;
  eta_pdg = 221;
  neutron_pdg = 2112;
  pion_pdg = 111;
  pion_plus_pdg = 211;
  pion_minus_pdg = -211;
  //O_16_pdg = 100069;   // oznacznie z Neuta
  lambda_pdg = 3122;
  kaon_pdg = 311;
  kaon_plus_pdg = 321;


  if (e->flag.qel)		// kwiazielastyczne oddziaływanie
    {
      if (e->flag.anty)		// jeśli jest to oddziaływanie z antyneutrinem
	{
	  if (e->flag.cc)
	    return -1;
	  else
	    {
	      if (e->nof (proton_pdg))
		return -51;
	      else if (e->nof (neutron_pdg))
		return -52;	// sprawdzam dodatkowo ?
	    }
	}
      else			// oddziaływanie z neutrinem
	{
	  if (e->flag.cc)
	    return 1;
	  else
	    {
	      if (e->nof (proton_pdg))
		return 51;
	      else if (e->nof (neutron_pdg))
		return 52;
	    }
	}
    }

  if (e->flag.mec){
    if (e->flag.anty) return -2;
    else return 2;
  }


  if (e->flag.res)		//rezonansowa produkcja: pojedynczy pion, pojed.eta, kaon, multipiony  
    {

      Int_t liczba_pionow, liczba_kaonow;

      liczba_pionow =
	e->nof (pion_pdg) + e->nof (pion_plus_pdg) + e->nof (pion_minus_pdg);
      liczba_kaonow = e->nof (kaon_pdg) + e->nof (kaon_pdg);

      if (liczba_pionow > 1 || liczba_pionow == 0)	// multipiony
	{
	  if (e->flag.anty)
	    {
	      if (e->flag.cc)
		return -21;
	      else
		return -41;
	    }
	  else
	    {
	      if (e->flag.cc)
		return 21;
	      else
		return 41;
	    }
	}

      if (liczba_pionow == 1)
	{
	  if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
	    {
	      if (e->flag.cc)
		{
		  if (e->nof (neutron_pdg) && e->nof (pion_minus_pdg))
		    return -11;
		  if (e->nof (neutron_pdg) && e->nof (pion_pdg))
		    return -12;
		  if (e->nof (proton_pdg) && e->nof (pion_minus_pdg))
		    return -13;
		}
	      else
		{
		  if (e->nof (proton_pdg))
		    {
		      if (e->nof (pion_minus_pdg))
			return -33;
		      else if (e->nof (pion_pdg))
			return -32;
		    }
		  else if (e->nof (neutron_pdg))
		    {
		      if (e->nof (pion_plus_pdg))
			return -34;
		      else if (e->nof (pion_pdg))
			return -31;
		    }
		}
	    }
	  else			// oddziaływanie z neutrinem
	    {
	      if (e->flag.cc)
		{
		  if (e->nof (proton_pdg) && e->nof (pion_plus_pdg))
		    return 11;
		  if (e->nof (proton_pdg) && e->nof (pion_pdg))
		    return 12;
		  if (e->nof (neutron_pdg) && e->nof (pion_plus_pdg))
		    return 13;
		}
	      else
		{
		  if (e->nof (proton_pdg))
		    {
		      if (e->nof (pion_minus_pdg))
			return 33;
		      else if (e->nof (pion_pdg))
			return 32;
		    }
		  else if (e->nof (neutron_pdg))
		    {
		      if (e->nof (pion_plus_pdg))
			return 34;
		      else if (e->nof (pion_pdg))
			return 31;
		    }
		}
	    }
	}

      if (e->nof (eta_pdg))	// produkcja rezonansowa ety
	{
	  if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
	    {
	      if (e->flag.cc)
		return -22;
	      else
		{
		  if (e->nof (neutron_pdg))
		    return -42;
		  else if (e->nof (proton_pdg))
		    return -43;	// sprawdzam dodatkowo ?
		}
	    }
	  else			// oddziaływanie z neutrinem
	    {
	      if (e->flag.cc)
		return 22;
	      else
		{
		  if (e->nof (neutron_pdg))
		    return 42;
		  else if (e->nof (proton_pdg))
		    return 43;
		}
	    }
	}

      if (e->nof (lambda_pdg) == 1 && liczba_kaonow == 1)	// produkcja rezonansowa kaonu
	{
	  if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
	    {
	      if (e->flag.cc && e->nof (kaon_pdg))
		return -23;
	      else
		{
		  if (e->nof (kaon_pdg))
		    return -44;
		  else if (e->nof (kaon_plus_pdg))
		    return -45;
		}
	    }
	  else			// oddziaływanie z neutrinem
	    {
	      if (e->flag.cc && e->nof (kaon_plus_pdg))
		return 23;
	      else
		{
		  if (e->nof (kaon_pdg))
		    return 44;
		  else if (e->nof (kaon_plus_pdg))
		    return 45;
		}
	    }


	}

    }

  if (e->flag.coh)		// koherentne  oddziaływanie tylko na O(16) 
    {
      Int_t _target;
      _target = e->par.nucleus_p + e->par.nucleus_n;	// liczba masowa  O(16) 

      if (_target == 16)
	{
	  if (e->flag.anty)	// jeśli jest to oddziaływanie z antyneutrinem
	    {
	      if (e->flag.cc && e->nof (pion_minus_pdg))
		return -16;
	      else if (e->nof (pion_pdg))
		return -36;
	    }
	  else			// oddziaływanie z neutrinem
	    {
	      if (e->flag.cc && e->nof (pion_plus_pdg))
		return 16;
	      else if (e->nof (pion_pdg))
		return 36;
	    }
	}
    }

  // gleboko nieelastyczne rozpraszanie               
  if (e->flag.dis)
    {
      if (e->flag.anty)
	{
	  if (e->flag.cc)
	    return -26;
	  else
	    return -46;
	}
      else
	{
	  if (e->flag.cc)
	    return 26;
	  else
	    return 46;
	}
    }

  return 9999;
}


#endif
