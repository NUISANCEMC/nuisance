// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "FitWeight.h"

//********************************************************************
FitWeight::FitWeight(std::string name, std::string inputfile) {
  //********************************************************************
  // To be completed...
  this->fName = name;
  this->fNormEnum = 0;
  this->fDialEnums.clear();
  this->fDialNames.clear();
  this->fDialValues.clear();

  this->fIsUsingNeut = false;
  this->fIsUsingNIWG = false;
  this->fIsUsingNuwro = false;
  this->fIsUsingGenie = false;
  this->fIsUsingT2K = false;

  fSplineHead = NULL;

  this->fName = name;

  // If file is a root file read it
  if (inputfile.find(".root") != std::string::npos) {
    // Open File

    // Get Fit Result Tree

    // Get Parameter Names and Types

    // Add Parameter Results

    // If file is a card file read it
  } else {
    // Parse Card Lines

    // Parse Card Inputs

    // Setup RW Dials
  }
}

//********************************************************************
FitWeight::FitWeight(std::string name) {
  //********************************************************************

  this->fNormEnum = 0;
  this->fDialEnums.clear();
  this->fDialNames.clear();
  this->fDialValues.clear();

  this->fIsUsingNeut = false;
  this->fIsUsingNIWG = false;
  this->fIsUsingNuwro = false;
  this->fIsUsingGenie = false;
  this->fIsUsingT2K = false;
  fIsUsingModeNorm = false;

  fSplineHead = NULL;

  this->fName = name;
  std::cout << "Creating FitWeight norm enum = " << this->fNormEnum
            << std::endl;
}

//********************************************************************
int FitWeight::GetDialEnum(std::string name, int type) {
  //********************************************************************

  if (type == -1) {
    return fDialEnums[this->GetDialPos(name)];
  }

  int this_enum = FitBase::GetDialEnum(type, name);
  return this_enum;
}

//********************************************************************
int FitWeight::GetRWEnum(int this_enum) {
  //********************************************************************
  return (this_enum % 1000);
}

//********************************************************************
unsigned int FitWeight::GetDialPos(int this_enum) {
  //********************************************************************

  std::vector<int>::iterator enIter = fDialEnums.begin();
  unsigned int count = 0;

  for (; enIter != fDialEnums.end(); enIter++) {
    if (((int)(*enIter)) == this_enum) {
      return count;
    }
    count++;
  }

  std::cerr << "No Value saved for ENUM " << this_enum << std::endl;
  exit(-1);

  return -1;
}

//******************************************************************** 
bool FitWeight::DialIncluded(std::string name){
//********************************************************************

  std::vector<std::string>::iterator naIter = fDialNames.begin();
  bool found = false;
  
  for (; naIter != fDialNames.end(); naIter++) {
    if (((std::string)(*naIter)) == name) {
      found = true;
      break;
    }
  }

  return found;
}

//********************************************************************
unsigned int FitWeight::GetDialPos(std::string name) {
//********************************************************************

  std::vector<std::string>::iterator naIter = fDialNames.begin();
  unsigned int count = 0;

  for (; naIter != fDialNames.end(); naIter++) {
    if (((std::string)(*naIter)) == name) {
      return count;
    }
    count++;
  }

  std::cerr << "No Value saved for Dial Name " << name << std::endl;
  exit(-1);

  return -1;
}

//********************************************************************
void FitWeight::IncludeDial(std::string name, int type, double startval) {
//********************************************************************
  
  int this_enum = this->GetDialEnum(name, type);
  int rw_enum = this->GetRWEnum(this_enum);

  LOG(FIT) << "Including dial " << name << " [type, rw_enum, fit_enum] = ["
           << generator_event_type(type) << ", " << rw_enum << ", " << this_enum
           << "] " << std::endl;

  int id = int(this_enum - (this_enum % 1000)) / 1000;
  if (id == kNORM) startval = 1.0;
  
  switch (id) {
    // NEUT RW INCLUDE DIAL
    case kNEUT:
#ifdef __NEUT_ENABLED__
      if (!fIsUsingNeut) this->SetupNeutRW();
      this->fNeutRW->Systematics().Init(
          static_cast<neut::rew::NSyst_t>(rw_enum));
      break;
#else
      ERR(FTL) << "NEUT RW Not Enabled!" << endl;
      throw;
#endif

    // NIWG RW INCLUDE DIAL
    case kNIWG:
#ifdef __NIWG_ENABLED__
      if (!fIsUsingNIWG) this->SetupNIWGRW();
      this->fNIWGRW->Systematics().Init(
          static_cast<niwg::rew::NIWGSyst_t>(rw_enum));
      break;
#else
      ERR(FTL) << "NIWG RW Not Enabled!" << endl;
      throw;
#endif

    // NUWRO RW INCLUDE DIAL
    case kNUWRO:
#ifdef __NUWRO_REWEIGHT_ENABLED__
      if (!fIsUsingNuwro) this->SetupNuwroRW();
      this->fNuwroRW->Systematics().Add(
          static_cast<nuwro::rew::NuwroSyst_t>(rw_enum));
      break;
#else
      LOG(FTL) << "Trying to Include NuWro Dial is unsupported!" << std::endl;
      throw;
#endif

    // GENIE RW INCLUDE DIAL
    case kGENIE:
#ifdef __GENIE_ENABLED__
      if (!fIsUsingGenie) this->SetupGenieRW();
      this->fGenieRW->Systematics().Init(
          static_cast<genie::rew::GSyst_t>(rw_enum));
      break;
#else
      ERR(FTL) << "Trying to Include GENIE Dial is unsupported!" << std::endl;
      throw;
#endif

    // T2K RW INCLUDE DIAL
    case kT2K:
#ifdef __T2KREW_ENABLED__
      if (!fIsUsingT2K) this->SetupT2KRW();
      this->fT2KRW->Systematics().Include(static_cast<t2krew::T2KSyst_t>(rw_enum));
      break;
#else
      ERR(FTL) << "Trying to Include T2K Dial is unsupported!" << std::endl;
      throw;
#endif

    // SAMPLE NORM DIAL
    case kNORM:
      break;

    case kMODENORM:
      break;

    default:
      ERR(FTL) << " Trying to include dial of unkown type " << name
               << " == " << type << endl;
      break;
  }

  // Setup ENUMS
  fDialEnums.push_back(this_enum);
  fDialNames.push_back(name);
  fDialValues.push_back(startval);

  fDialFuncs.push_back(FitBase::GetRWConvFunction(GetDialType(this_enum), name));
  fDialUnits.push_back(FitBase::GetRWUnits(GetDialType(this_enum), name));

  // Set Values
  this->SetDialValue(this_enum, startval);

  return;
}

//********************************************************************
void FitWeight::SetDialValue(std::string name, double val) {
  //********************************************************************
  int this_enum = this->GetDialEnum(name);
  this->SetDialValue(this_enum, val);
}

//********************************************************************
void FitWeight::SetDialValue(int this_enum, double val) {
  //*********************************************************************

  fIsDialChanged = true;
  int rw_enum = GetRWEnum(this_enum);
  unsigned int pos = GetDialPos(this_enum);
  LOG(DEB) << "Setting dial value " << this_enum << " to " << val << std::endl;

  // -- DIAL BLOCKS
  int id = int(this_enum - (this_enum % 1000)) / 1000;

  switch (id) {
    case kNEUT: {
#ifdef __NEUT_ENABLED__  // --- NEUT BLOCK
      this->fNeutRW->Systematics().Set(static_cast<neut::rew::NSyst_t>(rw_enum),
                                       val);
      this->fIsNeutChanged = true;
#else
      LOG(FTL) << " NEUT DIAL ERROR " << std::endl;
#endif
      break;
    }
    case kNIWG: {
#ifdef __NIWG_ENABLED__
      this->fNIWGRW->Systematics().Set(
          static_cast<niwg::rew::NIWGSyst_t>(rw_enum), val);
      this->fIsNIWGChanged = true;
#else
      LOG(FTL) << " NIWG DIAL ERROR " << std::endl;
#endif
      break;
    }
    case kNUWRO: {
#ifdef __NUWRO_REWEIGHT_ENABLED__
      fNuwroRW->Systematics().SetSystVal(
          static_cast<nuwro::rew::NuwroSyst_t>(rw_enum), val);
      this->fIsNuwroChanged = true;
#else
      LOG(FTL) << " NUWRO DIAL ERROR " << std::endl;
#endif
      break;
    }
    case kGENIE: {
#ifdef __GENIE_ENABLED__
      fGenieRW->Systematics().Set(static_cast<genie::rew::GSyst_t>(rw_enum),
                                  val);
      fGenieRW->Reconfigure();
      fGenieRW->Print();
      sleep(10);
      this->fIsGenieChanged = true;
#else
      LOG(FTL) << " GENIE DIAL ERROR " << std::endl;
#endif
      break;
    }
    case kT2K: {
#ifdef __T2KREW_ENABLED__
      fT2KRW->Systematics().SetTwkDial(static_cast<t2krew::T2KSyst_t>(rw_enum),
                                       val);
      this->fIsT2KChanged = true;
#else
      LOG(FTL) << " T2K DIAL ERROR " << std::endl;
#endif
      break;
    }
    case kCUSTOM:
    case kNORM: {
      break;
    }
    case kMODENORM: {
      this->fIsUsingModeNorm = true;
      break;
    }
    default: {
      LOG(FTL) << "Dial type error: " << generator_event_type(id) << std::endl;
      throw;
    }
  }

  fDialValues[pos] = val;
  return;
}

//********************************************************************
void FitWeight::Reconfigure(bool silent) {
  //********************************************************************

  if ((fIsUsingNeut or fIsUsingNIWG) and fIsUsingT2K) {
    ERR(WRN) << " Make sure no correlated or overlapping dials are being used "
                "between T2KRW and NEUT/NIWG RW"
             << std::endl;
  }

  if (!fIsDialChanged) return;

  if (!silent and LOG_LEVEL(MIN)) this->PrintState();

#ifdef __NEUT_ENABLED__  // --- NEUT BLOCK
  if (fIsNeutChanged and fIsUsingNeut){ fNeutRW->Reconfigure(); }
#endif

#ifdef __NIWG_ENABLED__  // --- NIWG BLOCK
  if (fIsNIWGChanged and fIsUsingNIWG) fNIWGRW->Reconfigure();
#endif

#ifdef __NUWRO_REWEIGHT_ENABLED__  // --- NUWRO BLOCK
  if (fIsNuwroChanged and fIsUsingNuwro) fNuwroRW->Reconfigure();
#endif

#ifdef __GENIE_ENABLED__
  if (fIsGenieChanged and fIsUsingGenie) fGenieRW->Reconfigure();
#endif

#ifdef __T2KREW_ENABLED__
  if (fIsT2KChanged and fIsUsingT2K) fT2KRW->Reconfigure();
#endif

  fIsDialChanged = false;
  fIsNeutChanged = false;
  fIsNIWGChanged = false;
  fIsNuwroChanged = false;
  fIsGenieChanged = false;
  fIsT2KChanged = false;

  return;
}

//********************************************************************
void FitWeight::PrintState() {
  //********************************************************************

  LOG(MIN) << "-----------------------" << std::endl;
  LOG(MIN) << this->fName << " Cur. State:" << std::endl;

  // Loop over the Dials and print some info
  for (unsigned int i = 0; i < fDialNames.size(); i++) {

    std::string name = fDialNames.at(i);
    int this_enum = fDialEnums.at(i);
    double val = fDialValues.at(i);
    std::string type = GetDialType(this_enum);

    if (FitPar::Config().GetParB("convert_dials")) {
      
      double val = fDialFuncs.at(i).Eval(fDialValues.at(i));

      LOG(MIN) << "-> " << std::setw(2) << i << ". " << std::setw(10) << type + "_par. ";
      std::cout << std::setw(40) << fDialNames.at(i) << std::setw(5) << " = " << val
		<< " " << fDialUnits.at(i) << std::endl;
      
    } else {
      (void)val;
      LOG(MIN) << "-> " << std::setw(2) << i << ". " << std::setw(10) << type + "_par. ";
      std::cout << std::setw(40) << std::left << fDialNames.at(i) << std::setw(5)
		<< " = " << fDialValues.at(i) << " " << fDialUnits.at(i) << std::endl;
    }
  }

  LOG(MIN) << "-----------------------" << std::endl;
}

//********************************************************************
std::string FitWeight::GetDialType(int this_enum) {
  //********************************************************************

  int id = int(this_enum - (this_enum % 1000)) / 1000;
  switch (id) {
    case kNEUT: {
      return "neut";
    }
    case kNIWG: {
      return "niwg";
    }
    case kGENIE: {
      return "genie";
    }
    case kT2K: {
      return "t2k";
    }
    case kCUSTOM: {
      return "custom";
    }
    case kNORM: {
      return "norm";
    }
    case kMODENORM: {
      return "modenorm";
    }
    default: { return "unknown"; }
  }
}

//********************************************************************
double FitWeight::CalcWeight(BaseFitEvt* evt) {
//********************************************************************

  double rw_weight = 1.0;

  // SPLINE WEIGHTS
  if (evt->fType == kEVTSPLINE) {
    rw_weight = this->CalcSplineWeight(evt);
    evt->Weight = rw_weight;
    return rw_weight;
  };

  // GENERATOR WEIGHTS
  if (fIsDialChanged) this->Reconfigure();
  rw_weight = 1.0;

  switch (evt->fType) {
#ifdef __NEUT_ENABLED__  // --- NEUT BLOCK
    case kNEUT:
      if (fIsUsingNeut) {
        GeneratorUtils::FillNeutCommons(evt->fNeutVect);
        rw_weight *= fNeutRW->CalcWeight();
      }

#ifdef __NIWG_ENABLED__  // --- NIWG BLOCK
      if (fIsUsingNIWG) {
        niwg::rew::NIWGEvent* niwg_event =
            GeneratorUtils::GetNIWGEvent(evt->fNeutVect);
        rw_weight *= fNIWGRW->CalcWeight(*niwg_event);
        delete niwg_event;
      }
#endif

#ifdef __T2KREW_ENABLED__
      if (fIsUsingT2K) {
        rw_weight *= fT2KRW->CalcWeight(evt->fNeutVect);
      }
#endif

      break;
#endif

#ifdef __NUWRO_REWEIGHT_ENABLED__
    case kNUWRO:
      if (fIsUsingNuwro) {
        rw_weight *= fNuwroRW->CalcWeight(evt->fNuwroEvent);
      }
      break;
#endif

#ifdef __GENIE_ENABLED__
    case kGENIE:
      if (fIsUsingGenie) {
	double gw =  fGenieRW->CalcWeight(*(evt->genie_event->event));
        rw_weight *= fGenieRW->CalcWeight(*(evt->genie_event->event));
	cout << "GENIE WEIGHT = " <<  fGenieRW->CalcWeight(*(evt->genie_event->event)) << endl;
	if (gw != 1.0) sleep(1);
      }
#endif

    default:
      break;
  }

  if (fIsUsingModeNorm) {
    for (size_t de_it = 0; de_it < fDialEnums.size(); ++de_it) {
      int this_enum = fDialEnums[de_it];
      if ((int(this_enum - (this_enum % 1000)) / 1000) != kMODENORM) {
        continue;
      }
      if (evt->Mode == (GetRWEnum(this_enum) - 60)) {
        rw_weight *= fDialValues[de_it];
      }
    }
  }

  evt->Weight = rw_weight;
  return rw_weight;
}

//********************************************************************
#ifdef __NEUT_ENABLED__
void FitWeight::SetupNeutRW() {
  //********************************************************************

  LOG(FIT) << "Setting up NEUT RW" << endl;
  fIsUsingNeut = true;
  fIsNeutChanged = true;

  // Create RW Engine
  fNeutRW = new neut::rew::NReWeight();

  // get list of vetoed calc engines (just for debug really)
  std::string rw_engine_list =
      FitPar::Config().GetParS("FitWeight.fNeutRW_veto");
  bool xsec_ccqe = rw_engine_list.find("xsec_ccqe") == std::string::npos;
  bool xsec_res = rw_engine_list.find("xsec_res") == std::string::npos;
  bool xsec_ccres = rw_engine_list.find("xsec_ccres") == std::string::npos;
  bool xsec_coh = rw_engine_list.find("xsec_coh") == std::string::npos;
  bool xsec_dis = rw_engine_list.find("xsec_dis") == std::string::npos;
  bool xsec_ncel = rw_engine_list.find("xsec_ncel") == std::string::npos;
  bool xsec_nc = rw_engine_list.find("xsec_nc") == std::string::npos;
  bool xsec_ncres = rw_engine_list.find("xsec_ncres") == std::string::npos;
  bool nucl_casc = rw_engine_list.find("nucl_casc") == std::string::npos;
  bool nucl_piless = rw_engine_list.find("nucl_piless") == std::string::npos;

  // Activate each calc engine
  if (xsec_ccqe)
    fNeutRW->AdoptWghtCalc("xsec_ccqe", new neut::rew::NReWeightNuXSecCCQE);
  if (xsec_res)
    fNeutRW->AdoptWghtCalc("xsec_res", new neut::rew::NReWeightNuXSecRES);
  if (xsec_ccres)
    fNeutRW->AdoptWghtCalc("xsec_ccres", new neut::rew::NReWeightNuXSecCCRES);
  if (xsec_coh)
    fNeutRW->AdoptWghtCalc("xsec_coh", new neut::rew::NReWeightNuXSecCOH);
  if (xsec_dis)
    fNeutRW->AdoptWghtCalc("xsec_dis", new neut::rew::NReWeightNuXSecDIS);
  if (xsec_ncel)
    fNeutRW->AdoptWghtCalc("xsec_ncel", new neut::rew::NReWeightNuXSecNCEL);
  if (xsec_nc)
    fNeutRW->AdoptWghtCalc("xsec_nc", new neut::rew::NReWeightNuXSecNC);
  if (xsec_ncres)
    fNeutRW->AdoptWghtCalc("xsec_ncres", new neut::rew::NReWeightNuXSecNCRES);
  if (nucl_casc)
    fNeutRW->AdoptWghtCalc("nucl_casc", new neut::rew::NReWeightCasc);
  if (nucl_piless)
    fNeutRW->AdoptWghtCalc("nucl_piless", new neut::rew::NReWeightNuclPiless);
  fNeutRW->Reconfigure();
}
#endif

//********************************************************************
#ifdef __NIWG_ENABLED__
void FitWeight::SetupNIWGRW() {
//********************************************************************

// EXTRA CHECK if NEUT is also enabled, just incase it was
// missed at build time.
#ifndef __NEUT_ENABLED__
  ERR(FTL) << "Can't run NIWG event calculation without NEUT also enabled"
           << std::endl;
  ERR(FTL) << "Check your build configuration!" << std::endl;
  exit(-1);
#endif

  // Now Setup the rw engine
  LOG(FIT) << "Setting up NIWG RW" << std::endl;
  fIsUsingNIWG = true;
  fIsNIWGChanged = true;

  // Create RW Engine
  fNIWGRW = new niwg::rew::NIWGReWeight();

  // Get List of Veto Calcs (For Debugging)
  std::string rw_engine_list =
      FitPar::Config().GetParS("FitWeight.fNIWGRW_veto");
  bool niwg_2012a = rw_engine_list.find("niwg_2012a") == std::string::npos;
  bool niwg_2014a = rw_engine_list.find("niwg_2014a") == std::string::npos;
  bool niwg_pimult = rw_engine_list.find("niwg_pimult") == std::string::npos;
  bool niwg_mec = rw_engine_list.find("niwg_mec") == std::string::npos;
  bool niwg_rpa = rw_engine_list.find("niwg_rpa") == std::string::npos;
  bool niwg_eff_rpa = rw_engine_list.find("niwg_eff_rpa") == std::string::npos;
  bool niwg_proton =
      rw_engine_list.find("niwg_protonFSIbug") == std::string::npos;
  bool niwg_hadron =
      rw_engine_list.find("niwg_HadronMultSwitch") == std::string::npos;

  // Add the RW Calcs
  if (niwg_2012a)
    fNIWGRW->AdoptWghtCalc("niwg_2012a", new niwg::rew::NIWGReWeight2012a);
  if (niwg_2014a)
    fNIWGRW->AdoptWghtCalc("niwg_2014a", new niwg::rew::NIWGReWeight2014a);
  if (niwg_pimult)
    fNIWGRW->AdoptWghtCalc("niwg_pimult", new niwg::rew::NIWGReWeightPiMult);
  if (niwg_mec)
    fNIWGRW->AdoptWghtCalc("niwg_mec", new niwg::rew::NIWGReWeightMEC);
  if (niwg_rpa)
    fNIWGRW->AdoptWghtCalc("niwg_rpa", new niwg::rew::NIWGReWeightRPA);
  if (niwg_eff_rpa)
    fNIWGRW->AdoptWghtCalc("niwg_eff_rpa",
                           new niwg::rew::NIWGReWeightEffectiveRPA);
  if (niwg_proton)
    fNIWGRW->AdoptWghtCalc("niwg_protonFSIbug",
                           new niwg::rew::NIWGReWeightProtonFSIbug);
  if (niwg_hadron)
    fNIWGRW->AdoptWghtCalc("niwg_HadronMultSwitch",
                           new niwg::rew::NIWGReWeightHadronMultSwitch);

  fNIWGRW->Reconfigure();
}
#endif

//********************************************************************
#ifdef __NUWRO_REWEIGHT_ENABLED__
void FitWeight::SetupNuwroRW() {
  //********************************************************************

  LOG(FIT) << "Setting up NUWRO RW" << std::endl;
  fIsUsingNuwro = true;
  fIsNuwroChanged = true;

  // Create Engine
  fNuwroRW = new nuwro::rew::NuwroReWeight();

  // Get List of Veto Calcs (For Debugging)
  std::string rw_engine_list =
      FitPar::Config().GetParS("FitWeight.fNuwroRW_veto");
  bool xsec_qel = rw_engine_list.find("nuwro_QEL") == std::string::npos;
  bool xsec_flag = rw_engine_list.find("nuwro_FlagNorm") == std::string::npos;
  bool xsec_res = rw_engine_list.find("nuwro_RES") == std::string::npos;

  // Add the RW Calcs
  if (xsec_qel)
    fNuwroRW->AdoptWghtCalc("nuwro_QEL", new nuwro::rew::NuwroReWeight_QEL);
  if (xsec_flag)
    fNuwroRW->AdoptWghtCalc("nuwro_FlagNorm",
                            new nuwro::rew::NuwroReWeight_FlagNorm);
  // if (xsec_res)  fNuwroRW->AdoptWghtCalc( "nuwro_RES",  new
  // nuwro::rew::NuwroReWeight_SPP );

  fNuwroRW->Reconfigure();
}
#endif

#ifdef __T2KREW_ENABLED__
//********************************************************************
void FitWeight::SetupT2KRW() {
  //********************************************************************

  LOG(FIT) << "Setting up T2K RW" << std::endl;
  fIsUsingT2K = true;
  fIsT2KChanged = true;

  // Create Main RW Engine
  fT2KRW = new t2krew::T2KReWeight();

  // Setup Sub RW Engines (Only activated for neut and niwg)
  fT2KNeutRW = new t2krew::T2KNeutReWeight();
  fT2KNIWGRW = new t2krew::T2KNIWGReWeight();

  fT2KRW->AdoptWghtEngine("fNeutRW", fT2KNeutRW);
  fT2KRW->AdoptWghtEngine("fNIWGRW", fT2KNIWGRW);

  fT2KRW->Reconfigure();
}
#endif

#ifdef __GENIE_ENABLED__
//********************************************************************
void FitWeight::SetupGenieRW() {
  //********************************************************************

  LOG(FIT) << "Setting up GENIE RW" << std::endl;
  fIsUsingGenie = true;
  fIsGenieChanged = true;

  // Create Engine
  GHepRecord::SetPrintLevel(-2);
  fGenieRW = new genie::rew::GReWeight();

  // Get List of Vetos (Just for debugging)
  std::string rw_engine_list =
      FitPar::Config().GetParS("FitWeight.fGenieRW_veto");
  bool xsec_ncel = rw_engine_list.find("xsec_ncel") == std::string::npos;
  bool xsec_ccqe = rw_engine_list.find("xsec_ccqe") == std::string::npos;
  bool xsec_coh = rw_engine_list.find("xsec_coh") == std::string::npos;
  bool xsec_nnres = rw_engine_list.find("xsec_nonresbkg") == std::string::npos;
  bool xsec_nudis = rw_engine_list.find("nuclear_dis") == std::string::npos;
  bool xsec_resdec =
      rw_engine_list.find("hadro_res_decay") == std::string::npos;
  bool xsec_fzone = rw_engine_list.find("hadro_intranuke") == std::string::npos;
  bool xsec_intra = rw_engine_list.find("hadro_fzone") == std::string::npos;
  bool xsec_agky = rw_engine_list.find("hadro_agky") == std::string::npos;
  bool xsec_qevec = rw_engine_list.find("xsec_ccqe_vec") == std::string::npos;
  bool xsec_dis = rw_engine_list.find("xsec_dis") == std::string::npos;
  bool xsec_nc = rw_engine_list.find("xsec_nc") == std::string::npos;
  bool xsec_ccres = rw_engine_list.find("xsec_ccres") == std::string::npos;
  bool xsec_ncres = rw_engine_list.find("xsec_ncres") == std::string::npos;
  bool xsec_nucqe = rw_engine_list.find("nuclear_qe") == std::string::npos;

  // Now actually add the RW Calcs
  if (xsec_ncel)
    fGenieRW->AdoptWghtCalc("xsec_ncel", new genie::rew::GReWeightNuXSecNCEL);
  if (xsec_ccqe)
    fGenieRW->AdoptWghtCalc("xsec_ccqe", new genie::rew::GReWeightNuXSecCCQE);
  if (xsec_coh)
    fGenieRW->AdoptWghtCalc("xsec_coh", new genie::rew::GReWeightNuXSecCOH);
  if (xsec_nnres)
    fGenieRW->AdoptWghtCalc("xsec_nonresbkg",
                            new genie::rew::GReWeightNonResonanceBkg);
  if (xsec_nudis)
    fGenieRW->AdoptWghtCalc("nuclear_dis", new genie::rew::GReWeightDISNuclMod);
  if (xsec_resdec)
    fGenieRW->AdoptWghtCalc("hadro_res_decay",
                            new genie::rew::GReWeightResonanceDecay);
  if (xsec_fzone)
    fGenieRW->AdoptWghtCalc("hadro_fzone", new genie::rew::GReWeightFZone);
  if (xsec_intra)
    fGenieRW->AdoptWghtCalc("hadro_intranuke", new genie::rew::GReWeightINuke);
  if (xsec_agky)
    fGenieRW->AdoptWghtCalc("hadro_agky", new genie::rew::GReWeightAGKY);
  if (xsec_qevec)
    fGenieRW->AdoptWghtCalc("xsec_ccqe_vec",
                            new genie::rew::GReWeightNuXSecCCQEvec);
  if (xsec_dis)
    fGenieRW->AdoptWghtCalc("xsec_dis", new genie::rew::GReWeightNuXSecDIS);
  if (xsec_nc)
    fGenieRW->AdoptWghtCalc("xsec_nc", new genie::rew::GReWeightNuXSecNC);
  if (xsec_ccres)
    fGenieRW->AdoptWghtCalc("xsec_ccres", new genie::rew::GReWeightNuXSecCCRES);
  if (xsec_ncres)
    fGenieRW->AdoptWghtCalc("xsec_ncres", new genie::rew::GReWeightNuXSecNCRES);
  if (xsec_nucqe)
    fGenieRW->AdoptWghtCalc("nuclear_qe", new genie::rew::GReWeightFGM);

  fGenieRW->Reconfigure();
}
#endif

//********************************************************************
double FitWeight::GetDialValue(std::string name, std::string option) {
  //********************************************************************

  int this_enum = this->GetDialEnum(name);
  int this_pos = this->GetDialPos(this_enum);
  double val = this->GetDialValue(this_enum);

  if (!option.compare("ABS"))
    return fDialFuncs[this_pos].Eval(val);
  else if (!option.compare("FRC"))
    return fDialFuncs[this_pos].Eval(val) / fDialFuncs[this_pos].Eval(0.0);
  else
    return val;
}

//********************************************************************
double FitWeight::GetDialValue(int this_enum) {
  //********************************************************************
  unsigned int pos = this->GetDialPos(this_enum);
  return fDialValues.at(pos);
}

//********************************************************************
void FitWeight::UpdateWeightEngine(const double* x) {
  //********************************************************************
  this->SetAllDials(x, fDialEnums.size());
  return;
}

//********************************************************************
bool FitWeight::HasRWDialChanged(const double* x) {
  //********************************************************************

  for (unsigned int i = 0; i < fDialValues.size(); i++) {
    int rw_enum = fDialEnums.at(i);

    int id = int(rw_enum - (rw_enum % 1000)) / 1000;
    if (id == kNORM) continue;

    if (x[i] != fDialValues.at(i)) return true;
  }

  return false;
}

//********************************************************************
void FitWeight::SetAllDials(const double* x, int npt) {
  //********************************************************************
  for (int i = 0; i < npt; i++) {
    int this_enum = fDialEnums.at(i);
    this->SetDialValue(this_enum, x[i]);
  }
  return;
};

//********************************************************************
void FitWeight::GetAllDials(double* x, int npt) {
  //********************************************************************

  for (int i = 0; i < npt; i++) {
    int this_enum = fDialEnums.at(i);
    x[i] = this->GetDialValue(this_enum);
  }

  return;
};

//********************************************************************
double FitWeight::GetSampleNorm(std::string samplename) {
  //********************************************************************
  std::string norm_dial = samplename + "_norm";

  // Loop through and see if we have one
  std::vector<std::string>::iterator naIter = fDialNames.begin();
  bool found_dial = false;
  for (; naIter != fDialNames.end(); naIter++) {
    if (((std::string)(*naIter)) == norm_dial) {
      found_dial = true;
      break;
    }
  }

  if (!found_dial && !samplename.empty()) {
    LOG(FIT) << " Late initialisation of norm: " << norm_dial << std::endl;
    this->IncludeDial(norm_dial, kNORM, 1.0);
    this->Reconfigure();
    return 1.0;

  } else {
    return this->GetDialValue(norm_dial);
  }
}

//********************************************************************
double FitWeight::GetSampleLikelihoodWeight(std::string samplename) {
//********************************************************************
  std::string norm_dial = samplename + "_weight";

  // Loop through and see if we have one
  std::vector<std::string>::iterator naIter = fDialNames.begin();
  bool found_dial = false;
  for (; naIter != fDialNames.end(); naIter++) {
    if (((std::string)(*naIter)) == norm_dial) {
      found_dial = true;
      break;
    }
  }

  if (!found_dial && !samplename.empty()) {
    return 1.0;
  } else {
    return this->GetDialValue(norm_dial);
  }  
}

//********************************************************************
std::vector<std::string> FitWeight::GetDialNames() {
  //********************************************************************
  return fDialNames;
}

//********************************************************************
std::vector<int> FitWeight::GetDialEnums() {
  //********************************************************************
  return fDialEnums;
}

//********************************************************************
std::vector<double> FitWeight::GetDialValues() {
  //********************************************************************
  return fDialValues;
}

//********************************************************************
void FitWeight::SetupEventCoeff(BaseFitEvt* event) {
//********************************************************************
  if (!fSplineHead) fSplineHead = new FitSplineHead();
  fSplineHead->SetupEventWeights(event);
}

//********************************************************************
void FitWeight::GenSplines(BaseFitEvt* event, bool save_graph) {
  //********************************************************************

  double nom = this->CalcWeight(event);
  event->dial_coeff[0] = (nom);

  // Get Current Dial Values and save to reset
  std::list<FitSpline*>::iterator spl_iter = fSplineHead->SplineObjects.begin();
  for (; spl_iter != fSplineHead->SplineObjects.end(); spl_iter++) {
    FitSpline* spl = (*spl_iter);
    int dim = spl->ndim;

    // ND Splines
    if (dim == 1)
      this->Fit1DSplineCoeff(event, spl, nom, save_graph);
    else if (dim == 2)
      this->Fit2DSplineCoeff(event, spl, nom, save_graph);
    else if (dim >= 3)
      this->FitNDSplineCoeff(event, spl, nom, save_graph);
  }
  
  return;
}

//********************************************************************
void FitWeight::Fit2DSplineCoeff(BaseFitEvt* event, FitSpline* spl, double nom,
                                 bool save_graph) {
  //********************************************************************

  int enum_x = spl->var_enums[0];
  int enum_y = spl->var_enums[1];

  double cur_x = this->GetDialValue(enum_x);
  double cur_y = this->GetDialValue(enum_y);
  //  int npar = spl->npar;

  std::vector<double> knots_x = spl->x_vals[0];
  std::vector<double> knots_y = spl->x_vals[1];
  int n_knots_x = knots_x.size();
  int n_knots_y = knots_y.size();

  double val_x = 0.0;
  double val_y = 0.0;
  int count = 0;
  double weightval = 0.0;
  bool hasresponse = false;

  TGraph2D gr_2D_scan = TGraph2D(n_knots_x * n_knots_y);

  // Loop over grid
  for (std::vector<double>::iterator iter_x = knots_x.begin();
       iter_x != knots_x.end(); iter_x++) {
    // X RW Value
    val_x = (*iter_x);
    this->SetDialValue(enum_x, val_x);
    this->Reconfigure(true);

    for (std::vector<double>::iterator iter_y = knots_y.begin();
         iter_y != knots_y.end(); iter_y++) {
      // Y RW Value
      val_y = (*iter_y);
      this->SetDialValue(enum_y, val_y);

      weightval = this->CalcWeight(event) / nom;
      if (weightval != 1.0) hasresponse = true;

      gr_2D_scan.SetPoint(count, val_x, val_y, weightval);
      count++;
    }
  }

  // Save GRAPH
  if (hasresponse and save_graph) {
    gr_2D_scan.SetName(
        Form("SplineFit_%s_%s", spl->id.c_str(), spl->form.c_str()));
    std::vector<std::string> titles =
        PlotUtils::ParseToStr(spl->id, ",");
    gr_2D_scan.SetTitle(Form("SplineFit_%s_%s;%s;%s;Weight Response",
                             spl->id.c_str(), spl->form.c_str(),
                             titles[0].c_str(), titles[1].c_str()));

    gr_2D_scan.Write();
  }

  SetDialValue(enum_x, cur_x);
  SetDialValue(enum_y, cur_y);

  return;
}

//********************************************************************
void FitWeight::Fit1DSplineCoeff(BaseFitEvt* event, FitSpline* spl, double nom,
                                 bool save_graph) {
  //********************************************************************

  int this_enum = spl->var_enums[0];
  double current = this->GetDialValue(this_enum);
  int npar = spl->npar;
  std::vector<double> knots = spl->x_vals[0];
  int n_knots = knots.size();
  double val = 0.0;

  double* allweights;
  double* allvals;
  allweights = new double[n_knots];
  allvals = new double[n_knots];

  int count = 0;
  double weightval = 0.0;

  bool hasresponse = false;

  for (std::vector<double>::iterator iter = knots.begin(); iter != knots.end();
       iter++) {
    val = (*iter);

    SetDialValue(this_enum, val);
    Reconfigure(true);
    
    weightval = this->CalcWeight(event) / nom;
    if (weightval != 1.0) hasresponse = true;
    
    allweights[count] = weightval;
    allvals[count++] = val;
  }
  
  if (!hasresponse) {
    event->dial_coeff[1] = -999.9;
  }

  TGraph* gr = new TGraph(n_knots, &knots[0], allweights);

  TF1* f1 = NULL;
  if (hasresponse){
    f1 =  new TF1("f1", spl, -1.0 + knots[0], 1.0 + knots[knots.size() - 1], npar);
    if (save_graph) f1->SetNpx(400);
  }

  // Check for TSpline3
  if (hasresponse){
    if (spl->needs_fit) {
      gr->Fit(f1, "WQM");
      for (int i = 0; i < npar; i++) {
	event->dial_coeff[i + spl->offset] = (f1->GetParameter(i));
      }
    } else {
      std::vector<double> dialcoeff = spl->GetSplineCoeff(allweights);
      for (int i = 0; i < npar; i++) {
	event->dial_coeff[i + spl->offset] = (dialcoeff[i]);
	f1->FixParameter(i, dialcoeff[i]);
      }
      //gr->Fit(f1, "WQM");
    }
  } else {
    for (int i = 0; i < npar; i++) {
      event->dial_coeff[i + spl->offset] = -999.99;
    }
  }
    
  if (save_graph and hasresponse) {
    TSpline3* spl3 =
        new TSpline3(Form("Spline3_dial%i_%s_%s;%s;Weight Response", this_enum,
                          spl->id.c_str(), spl->form.c_str(), spl->id.c_str()),
                     &knots[0], allweights, n_knots);
    TCanvas* c3 = new TCanvas("c3", "c3", 800, 600);
    c3->cd();
    spl3->SetLineColor(kBlue);
    spl3->Draw("C");

    gr->SetTitle(Form("SplineFit_dial%i_%s_%s;%s;Weight Response", this_enum,
                      spl->id.c_str(), spl->form.c_str(), spl->id.c_str()));
    gr->SetMarkerStyle(22);
    gr->SetName(Form("SplineFit_dial%i_%s_%s", this_enum, spl->id.c_str(),
                     spl->form.c_str()));
    gr->Write();

    gr->Draw("SAME P");
    if (f1) f1->Draw("SAME C");
    c3->Update();
    c3->Write(Form("Spline3_dial%i_%s_%s;%s;Weight Response", this_enum,
                   spl->id.c_str(), spl->form.c_str(), spl->id.c_str()));
    delete c3;
    delete spl3;
  }

  delete gr;
  delete f1;
  delete allweights;


  SetDialValue(this_enum, current);
  Reconfigure(true);
}

//********************************************************************
void FitWeight::ReadSplineHead(FitSplineHead* splhead) {
  //********************************************************************
  fSplineHead = splhead;
  fSplineHead->Reconfigure(this->fDialEnums, this->fDialValues);
}

//********************************************************************
void FitWeight::SetupSpline(std::string dialname, std::string splinename,
                            std::string points_def) {
//********************************************************************

  cout << "Setting up spline " << fSplineHead << endl;
  // Create spline head if none setup
  if (!fSplineHead) fSplineHead = new FitSplineHead();

  // Parse Enum Mapping
  std::vector<std::string> parsed_dials =
      PlotUtils::ParseToStr(dialname, ",");
  std::vector<int> list_enums;
  for (UInt_t i = 0; i < parsed_dials.size(); i++) {
    list_enums.push_back(this->GetDialEnum(parsed_dials.at(i)));
  }

  // Add new spline
  cout << "Added spline" << endl;
  FitSpline* spl = new FitSpline(dialname, splinename, list_enums, points_def);
  fSplineHead->AddSpline(spl);

  return;
}

//********************************************************************
void FitWeight::ResetSplines() {
  //********************************************************************
  delete fSplineHead;
  fSplineHead = NULL;
}

//********************************************************************
double FitWeight::CalcSplineWeight(BaseFitEvt* evt) {
  //********************************************************************
  double rw_weight = fSplineHead->CalcWeight(evt->dial_coeff);
  return rw_weight;
}

//********************************************************************
bool FitWeight::HasDialChanged() {
  //********************************************************************
  return fIsDialChanged;
};

// Global Conversion Functions
// ---------------------------
//********************************************************************
TF1 FitBase::GetRWConvFunction(std::string type, std::string name) {
//********************************************************************

  std::string dialfunc = "x";
  std::string parType = type;
  double low = -10000.0;
  double high = 10000.0;
  if (parType.find("parameter") == std::string::npos) parType += "_parameter";

  string line;
  ifstream card(
      (string(getenv("EXT_FIT")) + "/parameters/dial_conversion.card").c_str(),
      ifstream::in);

  while (getline(card, line, '\n')) {
    istringstream stream(line);
    string token, parname;
    int val = 0;
    double entry;

    if (line.c_str()[0] == '#') continue;
    while (getline(stream, token, ' ')) {
      stream >> ws;

      istringstream stoken(token);
      stoken >> entry;

      if (val == 0) {
        if (token.compare(parType) != 0) {
          break;
        }
      } else if (val == 1) {
        if (token.compare(name) != 0) {
          break;
        }
      } else if (val == 2) {
      } else if (val == 3) {
        dialfunc = token;
      } else if (val == 3) {
      } else if (val == 4) {
        low = entry;
      } else if (val == 5) {
        high = entry;
      } else
        break;

      val++;
    }
  }
  
  TF1 convfunc = TF1((name + "_convfunc").c_str(), dialfunc.c_str(), low, high);
  return convfunc;
}

//********************************************************************
std::string FitBase::GetRWUnits(std::string type, std::string name) {
  //********************************************************************

  std::string unit = "sig.";
  std::string parType = type;

  if (parType.find("parameter") == std::string::npos) {
    parType += "_parameter";
  }

  std::string line;
  ifstream card((string(getenv("EXT_FIT")) + "/parameters/dial_conversion.card").c_str(), ifstream::in);

  while (getline(card, line, '\n')) {
    istringstream stream(line);
    string token, parname;
    int val = 0;
    double entry;

    if (line.c_str()[0] == '#') continue;
    while (getline(stream, token, ' ')) {
      stream >> ws;

      istringstream stoken(token);
      stoken >> entry;

      if (val == 0) {
        if (token.compare(parType) != 0) {
          break;
        }
      } else if (val == 1) {
        if (token.compare(name) != 0) {
          break;
        }
      } else if (val == 2) {
        unit = token;
      } else if (val == 3) {
      } else if (val == 4) {
      } else if (val == 5) {
      } else
        break;

      val++;
    }
  }

  return unit;
}

//********************************************************************
double FitBase::RWAbsToSigma(std::string type, std::string name, double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.GetX(val);
  if (fabs(conv_val) < 1E-10) conv_val = 0.0;
  return conv_val;
}

//********************************************************************
double FitBase::RWSigmaToAbs(std::string type, std::string name, double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.Eval(val);
  return conv_val;
}

//********************************************************************
double FitBase::RWFracToSigma(std::string type, std::string name, double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.GetX((val * f1.Eval(0.0)));
  if (fabs(conv_val) < 1E-10) conv_val = 0.0;
  return conv_val;
}

//********************************************************************
double FitBase::RWSigmaToFrac(std::string type, std::string name, double val) {
  //********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.Eval(val) / f1.Eval(0.0);
  return conv_val;
}



int FitBase::ConvDialType(std::string type){

  if      (!type.compare("neut_parameter")) return kNEUT;
  else if (!type.compare("niwg_parameter")) return kNIWG;
  else if (!type.compare("nuwro_parameter")) return kNUWRO;
  else if (!type.compare("t2k_parameter")) return kT2K;
  else if (!type.compare("genie_parameter")) return kGENIE;
  else if (!type.compare("norm_parameter")) return kNORM;
  else if (!type.compare("modenorm_parameter")) return kMODENORM;
  else return kUNKNOWN;
  
}

std::string FitBase::ConvDialType(int type){

  switch(type){
  case kNEUT:  { return "neut_parameter";  }
  case kNIWG:  { return "niwg_parameter";  }
  case kNUWRO: { return "nuwro_parameter"; }
  case kT2K:   { return "t2k_parameter";   }
  case kGENIE: { return "genie_parameter"; }
  case kNORM:  { return "norm_parameter";  }
  case kMODENORM: { return "modenorm_parameter"; }
  default: return "unknown_parameter"; 
  }
  
}

int FitBase::GetDialEnum(std::string type, std::string name){
  return FitBase::GetDialEnum( FitBase::ConvDialType(type), name );
}

int FitBase::GetDialEnum(int type, std::string name){

  int offset = type * 1000;
  int this_enum = -1; //Not Found

  // Select Types
  switch (type) {
    
  // NEUT DIAL TYPE
  case kNEUT: {
#ifdef __NEUT_ENABLED__  
    int neut_enum = (int)neut::rew::NSyst::FromString(name);
    if (neut_enum != 0){ this_enum = neut_enum + offset; }
#else
    this_enum = -2; //Not enabled
#endif
    break;
  }
    
  // NIWG DIAL TYPE
  case kNIWG: {
#ifdef __NIWG_ENABLED__  
    int niwg_enum = (int)niwg::rew::NIWGSyst::FromString(name);
    if (niwg_enum != 0){ this_enum = niwg_enum + offset; }
#else
    this_enum = -2;
#endif
    break;
  }

  // NUWRO DIAL TYPE
  case kNUWRO: {
#ifdef __NUWRO_REWEIGHT_ENABLED__  
    int nuwro_enum = (int)nuwro::rew::NuwroSyst::FromString(name);
    if (nuwro_enum > 0){ this_enum = nuwro_enum + offset; }
#else
    this_enum = -2;
#endif
  }

  // GENIE DIAL TYPE
    case kGENIE: {
#ifdef __GENIE_ENABLED__
      int genie_enum = (int)genie::rew::GSyst::FromString(name);
      if (genie_enum > 0){ this_enum = genie_enum + offset; }
#else
      this_enum = -2;
#endif
      break;
    }

    case kCUSTOM: {
      int custom_enum = 0;  // PLACEHOLDER
      this_enum = custom_enum + offset;
      break;
    }
      
      // T2K DIAL TYPE
  case kT2K: {
#ifdef __T2KREW_ENABLED__
    int t2k_enum = (int)t2krew::T2KSyst::FromString(name);
    if (t2k_enum > 0){ this_enum = t2k_enum + offset; }
#else
    this_enum = -2;
#endif  
    break;
  }
 
  case kNORM: {
    this_enum = offset + 1;
    break;
  }

  case kMODENORM: {
    size_t us_pos = name.find_first_of('_');
    std::string numstr = name.substr(us_pos + 1);
    int mode_num = std::atoi(numstr.c_str());
    LOG(FTL) << "Getting mode num " << mode_num << endl;
    if (!mode_num) {
      ERR(FTL) << "Attempting to parse dial name: \"" << name
	       << "\" as a mode norm dial but failed." << endl;
      throw;
    }
    this_enum = 60 + mode_num + offset;
    break;
  }
   

 
  }

  // If Not Enabled
  if (this_enum == -2){
    ERR(FTL) << "RW Engine not supported for " << FitBase::ConvDialType(type) << endl;
    ERR(FTL) << "Check dial " << name << endl;
  }

  // If Not Found
  if (this_enum == -1){
    ERR(FTL) << "Dial " << name << " not found." << endl;
  }

  return this_enum;
}
