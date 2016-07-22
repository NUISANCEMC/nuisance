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


#ifdef __NUANCE_ENABLED__
int GeneratorUtils::ConvertNuanceMode(NuanceEvent * evt){
  int ch = evt->channel;
  int sg = 1;
  if (evt->neutrino < 0) sg = -1;
  
  switch(ch){
  case 1: return sg*1; // Nuance CCQE 1 = NEUT CCQE 1
  case 2: return sg*16; // Nuance NCEL 2 = NEUT NCEL 16
  case 3: return sg*3; //Nuance 3 == NEUT 3;
  case 4: return sg*4; //Nuance 4 == NEUT 4;
  case 5: return sg*5; //Nuance 5 == NEUT 5;
  case 6: return sg*6; //Nuance 6 == NEUT 6;
  case 7: return sg*7; //Nuance 7 == NEUT 7;
  case 8: return sg*8; //Nuance 8 == NEUT 8;
  case 9: return sg*9; //Nuance 9 == NEUT 9;
  case 10: return sg*10; //Nuance 10 == NEUT 10;
  case 11: return sg*11; //Nuance 11 == NEUT 11;
  case 12: return sg*12; //Nuance 12 == NEUT 12;
  case 13: return sg*13; //Nuance 13 == NEUT 13;
  case 14: return sg*14; //Nuance 14 == NEUT 14;
  case 15: return sg*15; //Nuance 15 == NEUT 15;
  case 16: return sg*16; //Nuance 16 == NEUT 16;
  case 17: return sg*17; //Nuance 17 == NEUT 17;
  case 18: return sg*18; //Nuance 18 == NEUT 18;
  case 19: return sg*19; //Nuance 19 == NEUT 19;
  case 20: return sg*20; //Nuance 20 == NEUT 20;
  case 21: return sg*21; //Nuance 21 == NEUT 21;
  case 22: return sg*22; //Nuance 22 == NEUT 22;
  case 23: return sg*23; //Nuance 23 == NEUT 23;
  case 24: return sg*24; //Nuance 24 == NEUT 24;
  case 25: return sg*25; //Nuance 25 == NEUT 25;
  case 26: return sg*26; //Nuance 26 == NEUT 26;
  case 27: return sg*27; //Nuance 27 == NEUT 27;
  case 28: return sg*28; //Nuance 28 == NEUT 28;
  case 29: return sg*29; //Nuance 29 == NEUT 29;
  case 30: return sg*30; //Nuance 30 == NEUT 30;
  case 31: return sg*31; //Nuance 31 == NEUT 31;
  case 32: return sg*32; //Nuance 32 == NEUT 32;
  case 33: return sg*33; //Nuance 33 == NEUT 33;
  case 34: return sg*34; //Nuance 34 == NEUT 34;
  case 35: return sg*35; //Nuance 35 == NEUT 35;
  case 36: return sg*36; //Nuance 36 == NEUT 36;
  case 37: return sg*37; //Nuance 37 == NEUT 37;
  case 38: return sg*38; //Nuance 38 == NEUT 38;
  case 39: return sg*39; //Nuance 39 == NEUT 39;
  case 40: return sg*40; //Nuance 40 == NEUT 40;
  case 41: return sg*41; //Nuance 41 == NEUT 41;
  case 42: return sg*42; //Nuance 42 == NEUT 42;
  case 43: return sg*43; //Nuance 43 == NEUT 43;
  case 44: return sg*44; //Nuance 44 == NEUT 44;
  case 45: return sg*45; //Nuance 45 == NEUT 45;
  case 46: return sg*46; //Nuance 46 == NEUT 46;
  case 47: return sg*47; //Nuance 47 == NEUT 47;
  case 48: return sg*48; //Nuance 48 == NEUT 48;
  case 49: return sg*49; //Nuance 49 == NEUT 49;
  case 50: return sg*50; //Nuance 50 == NEUT 50;
  case 51: return sg*51; //Nuance 51 == NEUT 51;
  case 52: return sg*52; //Nuance 52 == NEUT 52;
  case 53: return sg*53; //Nuance 53 == NEUT 53;
  case 54: return sg*54; //Nuance 54 == NEUT 54;
  case 55: return sg*55; //Nuance 55 == NEUT 55;
  case 56: return sg*56; //Nuance 56 == NEUT 56;
  case 57: return sg*57; //Nuance 57 == NEUT 57;
  case 58: return sg*58; //Nuance 58 == NEUT 58;
  case 59: return sg*59; //Nuance 59 == NEUT 59;
  case 60: return sg*60; //Nuance 60 == NEUT 60;
  case 61: return sg*61; //Nuance 61 == NEUT 61;
  case 62: return sg*62; //Nuance 62 == NEUT 62;
  case 63: return sg*63; //Nuance 63 == NEUT 63;
  case 64: return sg*64; //Nuance 64 == NEUT 64;
  case 65: return sg*65; //Nuance 65 == NEUT 65;
  case 66: return sg*66; //Nuance 66 == NEUT 66;
  case 67: return sg*67; //Nuance 67 == NEUT 67;
  case 68: return sg*68; //Nuance 68 == NEUT 68;
  case 69: return sg*69; //Nuance 69 == NEUT 69;
  case 70: return sg*70; //Nuance 70 == NEUT 70;
  case 71: return sg*71; //Nuance 71 == NEUT 71;
  case 72: return sg*72; //Nuance 72 == NEUT 72;
  case 73: return sg*73; //Nuance 73 == NEUT 73;
  case 74: return sg*74; //Nuance 74 == NEUT 74;
  case 75: return sg*75; //Nuance 75 == NEUT 75;
  case 76: return sg*76; //Nuance 76 == NEUT 76;
  case 77: return sg*77; //Nuance 77 == NEUT 77;
  case 78: return sg*78; //Nuance 78 == NEUT 78;
  case 79: return sg*79; //Nuance 79 == NEUT 79;
  case 80: return sg*80; //Nuance 80 == NEUT 80;
  case 81: return sg*81; //Nuance 81 == NEUT 81;
  case 82: return sg*82; //Nuance 82 == NEUT 82;
  case 83: return sg*83; //Nuance 83 == NEUT 83;
  case 84: return sg*84; //Nuance 84 == NEUT 84;
  case 85: return sg*85; //Nuance 85 == NEUT 85;
  case 86: return sg*86; //Nuance 86 == NEUT 86;
  case 87: return sg*87; //Nuance 87 == NEUT 87;
  case 88: return sg*88; //Nuance 88 == NEUT 88;
  case 89: return sg*89; //Nuance 89 == NEUT 89;
  case 90: return sg*90; //Nuance 90 == NEUT 90;
  case 91: return sg*91; //Nuance 91 == NEUT 91;
  case 92: return sg*92; //Nuance 92 == NEUT 92;
  case 93: return sg*93; //Nuance 93 == NEUT 93;
  case 94: return sg*94; //Nuance 94 == NEUT 94;
  case 95: return sg*95; //Nuance 95 == NEUT 95;
  case 96: return sg*96; //Nuance 96 == NEUT 96;
  case 97: return sg*97; //Nuance 97 == NEUT 97;
  case 98: return sg*98; //Nuance 98 == NEUT 98;
  case 99: return sg*99; //Nuance 99 == NEUT 99;
  default:
    std::cerr << "Unknown Nuance Channel ID = "<<ch<<endl;
    throw("Exiting.");
    return 0;
  }
  return 0;
}
#endif
