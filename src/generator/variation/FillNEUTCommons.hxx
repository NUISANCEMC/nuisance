#ifndef GENERATOR_VARIATION_FILLENEUTCOMMONS_HXX_SEEN
#define GENERATOR_VARIATION_FILLENEUTCOMMONS_HXX_SEEN

#include "NFortFns.h" // Contains all the NEUT common blocks

namespace NEUTUtils {

inline void FillNeutCommons(NeutVect *nvect) {
  // WARNING: This has only been implemented for a neuttree and not GENIE
  // This should be kept in sync with T2KNIWGUtils::GetNIWGEvent(TTree)

  // NEUT version info.  Can't get it to compile properly with this yet
  // neutversion_.corev  =   nvect->COREVer;
  // neutversion_.nucev  =   nvect->NUCEVer;
  // neutversion_.nuccv  =   nvect->NUCCVer;

  // Documentation: See nework.h
  nework_.modene = nvect->Mode;
  nework_.numne = nvect->Npart();

#ifdef NEUT_COMMON_QEAV
  nemdls_.mdlqeaf = nvect->QEAVForm;
#else
  nemdls_.mdlqeaf = nvect->QEVForm;
#endif
  nemdls_.mdlqe = nvect->QEModel;
  nemdls_.mdlspi = nvect->SPIModel;
  nemdls_.mdldis = nvect->DISModel;
  nemdls_.mdlcoh = nvect->COHModel;
  neutcoh_.necohepi = nvect->COHModel;

  nemdls_.xmaqe = nvect->QEMA;
  nemdls_.xmvqe = nvect->QEMV;
  nemdls_.kapp = nvect->KAPPA;

  // nemdls_.sccfv = SCCFVdef;
  // nemdls_.sccfa = SCCFAdef;
  // nemdls_.fpqe = FPQEdef;

  nemdls_.xmaspi = nvect->SPIMA;
  nemdls_.xmvspi = nvect->SPIMV;
  nemdls_.xmares = nvect->RESMA;
  nemdls_.xmvres = nvect->RESMV;

  neut1pi_.xmanffres = nvect->SPIMA;
  neut1pi_.xmvnffres = nvect->SPIMV;
  neut1pi_.xmarsres = nvect->RESMA;
  neut1pi_.xmvrsres = nvect->RESMV;
  neut1pi_.neiff = nvect->SPIForm;
  neut1pi_.nenrtype = nvect->SPINRType;
  neut1pi_.rneca5i = nvect->SPICA5I;
  neut1pi_.rnebgscl = nvect->SPIBGScale;

  nemdls_.xmacoh = nvect->COHMA;
  nemdls_.rad0nu = nvect->COHR0;
  // nemdls_.fa1coh = nvect->COHA1err;
  // nemdls_.fb1coh = nvect->COHb1err;

  // neutdis_.nepdf = NEPDFdef;
  // neutdis_.nebodek = NEBODEKdef;

  neutcard_.nefrmflg = nvect->FrmFlg;
  neutcard_.nepauflg = nvect->PauFlg;
  neutcard_.nenefo16 = nvect->NefO16;
  neutcard_.nemodflg = nvect->ModFlg;
  // neutcard_.nenefmodl = 1;
  // neutcard_.nenefmodh = 1;
  // neutcard_.nenefkinh = 1;
  // neutpiabs_.neabspiemit = 1;

  nenupr_.iformlen = nvect->FormLen;

  neutpiless_.ipilessdcy = nvect->IPilessDcy;
  neutpiless_.rpilessdcy = nvect->RPilessDcy;

  neutpiless_.ipilessdcy = nvect->IPilessDcy;
  neutpiless_.rpilessdcy = nvect->RPilessDcy;

  neffpr_.fefqe = nvect->NuceffFactorPIQE;
  neffpr_.fefqeh = nvect->NuceffFactorPIQEH;
  neffpr_.fefinel = nvect->NuceffFactorPIInel;
  neffpr_.fefabs = nvect->NuceffFactorPIAbs;
  neffpr_.fefcx = nvect->NuceffFactorPICX;
  neffpr_.fefcxh = nvect->NuceffFactorPICXH;

  neffpr_.fefcoh = nvect->NuceffFactorPICoh;
  neffpr_.fefqehf = nvect->NuceffFactorPIQEHKin;
  neffpr_.fefcxhf = nvect->NuceffFactorPICXKin;
  neffpr_.fefcohf = nvect->NuceffFactorPIQELKin;

  for (int i = 0; i < nework_.numne; i++) {
    nework_.ipne[i] = nvect->PartInfo(i)->fPID;
    nework_.pne[i][0] =
        (float)nvect->PartInfo(i)->fP.X() / 1000; // VC(NE)WORK in M(G)eV
    nework_.pne[i][1] =
        (float)nvect->PartInfo(i)->fP.Y() / 1000; // VC(NE)WORK in M(G)eV
    nework_.pne[i][2] =
        (float)nvect->PartInfo(i)->fP.Z() / 1000; // VC(NE)WORK in M(G)eV
  }
  // fsihist.h

  // neutroot fills a dummy object for events with no FSI to prevent memory leak
  // when
  // reading the TTree, so check for it here

  if ((int)nvect->NfsiVert() ==
      1) { // An event with FSI must have at least two vertices
    //    if (nvect->NfsiPart()!=1 || nvect->Fsiprob!=-1)
    //      ERR(WRN) << "T2KNeutUtils::fill_neut_commons(TTree) NfsiPart!=1 or
    //      Fsiprob!=-1 when NfsiVert==1" << std::endl;

    fsihist_.nvert = 0;
    fsihist_.nvcvert = 0;
    fsihist_.fsiprob = 1;
  } else { // Real FSI event
    fsihist_.nvert = (int)nvect->NfsiVert();
    for (int ivert = 0; ivert < fsihist_.nvert; ivert++) {
      fsihist_.iflgvert[ivert] = nvect->FsiVertInfo(ivert)->fVertID;
      fsihist_.posvert[ivert][0] = (float)nvect->FsiVertInfo(ivert)->fPos.X();
      fsihist_.posvert[ivert][1] = (float)nvect->FsiVertInfo(ivert)->fPos.Y();
      fsihist_.posvert[ivert][2] = (float)nvect->FsiVertInfo(ivert)->fPos.Z();
    }

    fsihist_.nvcvert = nvect->NfsiPart();
    for (int ip = 0; ip < fsihist_.nvcvert; ip++) {
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

  // put empty nucleon FSI history (since it is not saved in the NeutVect
  // format)
  // Comment out as NEUT does not have the necessary proton FSI information yet
  //  nucleonfsihist_.nfnvert = 0;
  //  nucleonfsihist_.nfnstep = 0;
}

} // namespace NEUTUtils

#endif
