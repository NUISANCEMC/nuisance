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

#include "FitWeight.h"

//********************************************************************
FitWeight::FitWeight(std::string name, std::string inputfile){
//********************************************************************
  // To be completed...
  this->rw_name = name;
  this->norm_enum = 0;
  this->dial_enums.clear();
  this->dial_names.clear();
  this->dial_values.clear();

  this->using_neut = false;
  this->using_niwg = false;
  this->using_nuwro = false;
  this->using_genie = false;
  this->using_t2k   = false;

  spline_head = NULL;

  this->rw_name = name;

  // If file is a root file read it
  if (inputfile.find(".root") != std::string::npos){

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
FitWeight::FitWeight(std::string name){
//********************************************************************

  this->norm_enum = 0;
  this->dial_enums.clear();
  this->dial_names.clear();
  this->dial_values.clear();

  this->using_neut = false;
  this->using_niwg = false;
  this->using_nuwro = false;
  this->using_genie = false;
  this->using_t2k   = false;

  spline_head = NULL;

  this->rw_name = name;
  std::cout<<"Creating FitWeight norm enum = "<<this->norm_enum<<std::endl;
}

//********************************************************************
int FitWeight::GetDialEnum(std::string name, int type){
//********************************************************************

  if (type == -1){
    return dial_enums[ this->GetDialPos(name) ];
  }

  int offset = type * 1000;
  int this_enum = -1;

  // NEUT DIAL TYPE
  if (type == kNEUT){
#ifdef __NEUT_ENABLED__ // --- NEUT BLOCK
    int neut_enum = (int) neut::rew::NSyst::FromString(name);
    this_enum = neut_enum + offset;
#else
    ERR(FTL) << "NEUT RW Not Enabled!" << endl;
    throw;
#endif

    // NIWG DIAL TYPE
  } else if (type == kNIWG){
#ifdef __NIWG_ENABLED__ // --- NIWG BLOCK
    int niwg_enum = (int) niwg::rew::NIWGSyst::FromString(name);
    this_enum = niwg_enum + offset;
#else
    ERR(FTL) << "NIWG RW Not Enabled!" << endl;
    throw;
#endif

  // NUWRO DIAL TYPE
  } else if (type == kNUWRO){
#ifdef __NUWRO_REWEIGHT_ENABLED__ // --- NUWRO BLOCK
    int nuwro_enum = (int) nuwro::rew::NuwroSyst::FromString(name);
    this_enum = nuwro_enum + offset;
#else
    ERR(FTL) << "NUWRO RW Not Enabled!" << endl;
    throw;
#endif
  
    // GENIE DIAL TYPE
  } else if (type == kGENIE){
#ifdef __GENIE_ENABLED__
    int genie_enum = (int) genie::rew::GSyst::FromString(name);
    this_enum = genie_enum + offset;
#else
    ERR(FTL) << "GENIE RW Not Enabled!" << endl;
    throw;
#endif
  

    // CUSTOM DIAL TYPE
  } else if (type == kCUSTOM){
    int custom_enum = 0; // PLACEHOLDER
    this_enum = custom_enum + offset;

    // T2K DIAL TYPE
  } else if (type == kT2K){
#ifdef __T2KREW_ENABLED__
    int t2k_enum = (int) t2krew::T2KSyst::FromString(name);
    this_enum = t2k_enum + offset;
#else
    ERR(FTL) << "T2K RW Not Enabled!" << endl;
    throw;
#endif
  
  
  // NORM DIAL TYPE
  } else if (type == kNORM){
    this_enum = norm_enum + offset;
    norm_enum++;

  // UNKOWN DIAL TYPE
  } else {
    ERR(FTL) << " Uknown dial type found = "<<type<<" "<<kNORM<<endl;
    //throw; // Don't throw so fitter puts out its name
  }

  return this_enum;
}

//********************************************************************
int FitWeight::GetRWEnum(int this_enum){
//********************************************************************
  return (this_enum % 1000);
}

//********************************************************************
unsigned int FitWeight::GetDialPos(int this_enum){
//********************************************************************

  std::vector<int>::iterator enIter = dial_enums.begin();
  unsigned int count = 0;

  for ( ; enIter != dial_enums.end(); enIter++){
    if ( ((int)(*enIter)) == this_enum){
      return count;
    }
    count++;
  }

  std::cerr<<"No Value saved for ENUM "<<this_enum<<std::endl;
  exit(-1);

  return -1;
}

//********************************************************************
unsigned int FitWeight::GetDialPos(std::string name){
//********************************************************************

  std::vector<std::string>::iterator naIter = dial_names.begin();
  unsigned int count = 0;

  for ( ; naIter != dial_names.end(); naIter++){
    if ( ((std::string)(*naIter)) == name){
      return count;
    }
    count++;
  }

  std::cerr<<"No Value saved for Dial Name "<<name<<std::endl;
  exit(-1);

  return -1;
}

//********************************************************************
void FitWeight::IncludeDial(std::string name, int type, double startval){
//********************************************************************

  int this_enum = this->GetDialEnum(name, type);
  int rw_enum   = this->GetRWEnum(this_enum);

  LOG(FIT) << "Including dial "<< name
	   << " [type, rw_enum, fit_enum] = ["
	   << type << ", " << rw_enum << ", "<< this_enum << "] "
	   << std::endl;


  int id = int(this_enum-(this_enum%1000))/1000;
  switch( id ){

  // NEUT RW INCLUDE DIAL
  case kNEUT:
#ifdef __NEUT_ENABLED__
    if (!using_neut) this->SetupNeutRW();
    this->neut_rw->Systematics().Init( static_cast<neut::rew::NSyst_t>(rw_enum) );
    break;
#else
    ERR(FTL) << "NEUT RW Not Enabled!" << endl;
    throw;
#endif

  // NIWG RW INCLUDE DIAL
  case kNIWG:
#ifdef __NIWG_ENABLED__
    if (!using_niwg) this->SetupNIWGRW();
    this->niwg_rw->Systematics().Init( static_cast<niwg::rew::NIWGSyst_t>(rw_enum) );
    break;
#else
    ERR(FTL) << "NIWG RW Not Enabled!" << endl;
    throw;
#endif

  // NUWRO RW INCLUDE DIAL
  case kNUWRO:
#ifdef __NUWRO_REWEIGHT_ENABLED__
    if (!using_nuwro) this->SetupNuwroRW();
    this->nuwro_rw->Systematics().Add( static_cast<nuwro::rew::NuwroSyst_t>(rw_enum) );
    break;
#else
    LOG(FTL)<<"Trying to Include NuWro Dial is unsupported!"<<std::endl;
    throw;
#endif

    // GENIE RW INCLUDE DIAL
  case kGENIE:
#ifdef __GENIE__ENABLED__
    if (!using_genie) this->SetupGenieRW();
    this->genie_rw->Systematics().Add( static_cast<genie::rew::GSyst_t>(rw_enum) );
    break;
#else
    ERR(FTL)<<"Trying to Include GENIE Dial is unsupported!"<<std::endl;
    throw;
#endif

  // T2K RW INCLUDE DIAL
  case kT2K:
#ifdef __T2KREW__ENABLED__
    if (!using_t2k) this->SetupT2KRW();
    this->t2k_rw->Systematics().Add( static_cast<t2krew::T2KSyst_t>(rw_enum) );
    break;
#else
    ERR(FTL)<<"Trying to Include T2K Dial is unsupported!"<<std::endl;
    throw;
#endif

    // SAMPLE NORM DIAL
  case kNORM:
    break;

    
  default:
    ERR(FTL) << " Trying to include dial of unkown type " << name
	     << " == " << type << endl;
    break;
  }

  // Setup ENUMS
  dial_enums.push_back(this_enum);
  dial_names.push_back(name);
  dial_values.push_back(startval);

  dial_func .push_back( FitBase::GetRWConvFunction( GetDialType(this_enum), name ) );
  dial_units.push_back( FitBase::GetRWUnits( GetDialType(this_enum), name )        );

  // Set Values
  this->SetDialValue(this_enum, startval);

  return;
}

//********************************************************************
void FitWeight::SetDialValue(std::string name, double val){
//********************************************************************
  int this_enum = this->GetDialEnum( name );
  this->SetDialValue( this_enum, val );
}

//********************************************************************
void FitWeight::SetDialValue(int this_enum, double val){
//*********************************************************************

  dial_changed = true;
  int rw_enum = GetRWEnum(this_enum);
  unsigned int pos = GetDialPos(this_enum);
  LOG(DEB) <<"Setting dial value "<<this_enum<<" to "<<val<<std::endl;

  // -- DIAL BLOCKS
  int id = int(this_enum-(this_enum%1000))/1000;
    
  if ( id == kNEUT ){
#ifdef __NEUT_ENABLED__ // --- NEUT BLOCK
    this->neut_rw -> Systematics().Set( static_cast<neut::rew::NSyst_t>(rw_enum), val );
    this->neut_changed = true;
#else
    LOG(FTL)<<" NEUT DIAL ERROR " <<std::endl;
#endif
    
  } else if ( id == kNIWG ){
#ifdef __NIWG_ENABLED__
    this->niwg_rw -> Systematics().Set( static_cast<niwg::rew::NIWGSyst_t>(rw_enum), val );
    this->niwg_changed = true;
#else
    LOG(FTL)<<" NIWG DIAL ERROR "<<std::endl;
#endif
    
  } else if ( id == kNUWRO ){
#ifdef __NUWRO_REWEIGHT_ENABLED__
    nuwro_rw->Systematics().SetSystVal( static_cast<nuwro::rew::NuwroSyst_t>(rw_enum), val);
    this->nuwro_changed = true;
#else
    LOG(FTL)<<" NUWRO DIAL ERROR "<<std::endl;
#endif
  } else if ( id == kGENIE ){
#ifdef __GENIE_ENABLED__
    genie_rw->Systematics().Set( static_cast<genie::rew::GSyst_t>(rw_enum), val);
    this->genie_changed = true;
#else
    LOG(FTL)<<" GENIE DIAL ERROR "<<std::endl;
#endif
  } else if ( id == kT2K ){
#ifdef __T2KREW_ENABLED__
    t2k_rw->Systematics().SetTwkDial( static_cast<t2krew::T2KSyst_t>(rw_enum), val);
    this->t2k_changed = true;
#else
    LOG(FTL)<<" GENIE DIAL ERROR "<<std::endl;
#endif
  }

  /*
  if ( id == kNORM ){
    std::cout << "Changing Norm Value = "<<val<<std::endl;
  }
  */

  dial_values[pos] = val;
  return;
}

//********************************************************************
void FitWeight::Reconfigure(bool silent){
//********************************************************************

  if ((using_neut or using_niwg) and using_t2k) {
    ERR(WRN) << " Make sure no correlated or overllaping dials are being used between "
	     << " T2KRW and NEUT/NIWG RW"<<std::endl;
  }


  if (!dial_changed) return;

  if (!silent and LOG_LEVEL(MIN)) this->PrintState();

#ifdef __NEUT_ENABLED__ // --- NEUT BLOCK
  if (neut_changed and using_neut)  {
    LOG(FIT) << "Reconfiguring NEUT"<<endl;
    neut_rw->Reconfigure();
  }
#endif

#ifdef __NIWG_ENABLED__ // --- NIWG BLOCK
  if (niwg_changed and using_niwg) niwg_rw->Reconfigure();
#endif

#ifdef __NUWRO_REWEIGHT_ENABLED__ // --- NUWRO BLOCK
  if (nuwro_changed and using_nuwro) nuwro_rw->Reconfigure();
#endif

#ifdef __GENIE_ENABLED__
  if (genie_changed and using_genie) genie_rw->Reconfigure();
#endif

#ifdef __T2KREW_ENABLED__
  if (t2k_changed and using_t2k) t2k_rw->Reconfigure();
#endif

  dial_changed = false;
  neut_changed  = false;
  niwg_changed = false;
  nuwro_changed = false;
  genie_changed = false;
  t2k_changed = false;
  
  return;
}

//********************************************************************
void FitWeight::PrintState(){
//********************************************************************

  LOG(MIN) << "-----------------------" << std::endl;
  LOG(MIN) << this->rw_name << " Cur. State:"  << std::endl;

  for (unsigned int i = 0; i < dial_names.size(); i++){

    std::string name = dial_names.at(i);
    int this_enum = dial_enums.at(i);
    double val = dial_values.at(i);
    std::string type = GetDialType(this_enum);

    if (FitPar::Config().GetParB("convert_dials") or true){

      double val = dial_func.at(i).Eval(dial_values.at(i));

      LOG(MIN) << "-> " << i << ". "
	       << type + "_par. " << " "
	       << dial_names.at(i) << " = "
	       << val << " "
	       << dial_units.at(i) << std::endl;

    } else {
      (void) val;
      LOG(MIN) << "-> " << i << ". "
	       << type + "_par. " << " "
	       << dial_names.at(i) << " = "
	       << dial_values.at(i) << " "
	       << dial_units.at(i) << std::endl;

    }
  }

  LOG(MIN) << "-----------------------" << std::endl;
}

//********************************************************************
std::string FitWeight::GetDialType(int this_enum){
//********************************************************************

  int id = int(this_enum-(this_enum%1000))/1000;
  switch(id){
  case kNEUT:   { return "neut";   }
  case kNIWG:   { return "niwg";   }
  case kGENIE:  { return "genie";  }
  case kT2K:    { return "t2k";    }
  case kCUSTOM: { return "custom"; }
  case kNORM:   { return "norm";   }
  default: { return "unknown"; }
  }
}

//********************************************************************
double FitWeight::CalcWeight(BaseFitEvt* evt){
//********************************************************************

  double rw_weight = 1.0;

  // SPLINE WEIGHTS
  if (evt->fType == kEVTSPLINE){
    rw_weight = this->CalcSplineWeight(evt);
    evt->Weight = rw_weight;
    return rw_weight;
  };

  // GENERATOR WEIGHTS
  if (dial_changed) this->Reconfigure();
  rw_weight = 1.0;

  switch(evt->fType){
    
#ifdef __NEUT_ENABLED__ // --- NEUT BLOCK
  case kNEUT:
    if (using_neut){
      GeneratorUtils::FillNeutCommons(evt->neut_event);
      rw_weight *= neut_rw->CalcWeight();
    }
    
#ifdef __NIWG_ENABLED__ // --- NIWG BLOCK
    if (using_niwg){
      niwg::rew::NIWGEvent* niwg_event = GeneratorUtils::GetNIWGEvent(evt->neut_event); 
      rw_weight *= niwg_rw->CalcWeight(*niwg_event);
      delete niwg_event;
    }
#endif

#ifdef __T2KREW_ENABLED__
    if (using_t2k){
      rw_weight *= t2k_rw->CalcWeight(evt->neut_event);
    }
#endif
    
    break;
#endif
    
#ifdef __NUWRO_REWEIGHT_ENABLED__
  case kNUWRO:
    if (using_nuwro){
      rw_weight *= nuwro_rw->CalcWeight(evt->nuwro_event);
    }
    break;
#endif
    
#ifdef __GENIE_ENABLED__
  case kGENIE:
    if (using_genie){
      rw_weight *= genie_rw->CalcWeight(*(evt->genie_event->event));
    }
#endif

  default: break;
  }
  
  evt->Weight = rw_weight;
  return rw_weight;
}


//********************************************************************
#ifdef __NEUT_ENABLED__
void FitWeight::SetupNeutRW(){
//********************************************************************

  LOG(FIT)<<"Setting up NEUT RW"<<endl;
  using_neut   = true;
  neut_changed = true;

  // Create RW Engine
  neut_rw = new neut::rew::NReWeight();

  // get list of vetoed calc engines (just for debug really)
  std::string rw_engine_list = FitPar::Config().GetParS("FitWeight.neut_rw_veto");
  bool xsec_ccqe  = rw_engine_list.find("xsec_ccqe")  == std::string::npos;
  bool xsec_res   = rw_engine_list.find("xsec_res")   == std::string::npos;
  bool xsec_ccres = rw_engine_list.find("xsec_ccres") == std::string::npos;
  bool xsec_coh   = rw_engine_list.find("xsec_coh")   == std::string::npos;
  bool xsec_dis   = rw_engine_list.find("xsec_dis")   == std::string::npos;
  bool xsec_ncel  = rw_engine_list.find("xsec_ncel")  == std::string::npos;
  bool xsec_nc    = rw_engine_list.find("xsec_nc")    == std::string::npos;
  bool xsec_ncres = rw_engine_list.find("xsec_ncres") == std::string::npos;
  bool nucl_casc  = rw_engine_list.find("nucl_casc")  == std::string::npos;
  bool nucl_piless = rw_engine_list.find("nucl_piless")  == std::string::npos;

  // Activate each calc engine
  if (xsec_ccqe)   neut_rw->AdoptWghtCalc( "xsec_ccqe",      new neut::rew::NReWeightNuXSecCCQE    );
  if (xsec_res)    neut_rw->AdoptWghtCalc( "xsec_res",       new neut::rew::NReWeightNuXSecRES     );
  if (xsec_ccres)  neut_rw->AdoptWghtCalc( "xsec_ccres",     new neut::rew::NReWeightNuXSecCCRES   );
  if (xsec_coh)    neut_rw->AdoptWghtCalc( "xsec_coh",       new neut::rew::NReWeightNuXSecCOH     );
  if (xsec_dis)    neut_rw->AdoptWghtCalc( "xsec_dis",       new neut::rew::NReWeightNuXSecDIS     );
  if (xsec_ncel)   neut_rw->AdoptWghtCalc( "xsec_ncel",      new neut::rew::NReWeightNuXSecNCEL    );
  if (xsec_nc)     neut_rw->AdoptWghtCalc( "xsec_nc",        new neut::rew::NReWeightNuXSecNC      );
  if (xsec_ncres)  neut_rw->AdoptWghtCalc( "xsec_ncres",     new neut::rew::NReWeightNuXSecNCRES   );
  if (nucl_casc)   neut_rw->AdoptWghtCalc( "nucl_casc",      new neut::rew::NReWeightCasc          );
  if (nucl_piless) neut_rw->AdoptWghtCalc( "nucl_piless",    new neut::rew::NReWeightNuclPiless    );
  neut_rw->Reconfigure();
}
#endif

//********************************************************************
#ifdef __NIWG_ENABLED__
void FitWeight::SetupNIWGRW(){
//********************************************************************

  // EXTRA CHECK if NEUT is also enabled, just incase it was
  // missed at build time.
#ifndef __NEUT_ENABLED__
  ERR(FTL) << "Can't run NIWG event calculation without NEUT also enabled"<<std::endl;
  ERR(FTL) << "Check your build configuration!"<<std::endl;
  exit(-1);
#endif

  // Now Setup the rw engine
  LOG(FIT) << "Setting up NIWG RW" << std::endl;
  using_niwg  = true;
  niwg_changed = true;

  // Create RW Engine
  niwg_rw = new niwg::rew::NIWGReWeight();

  // Get List of Veto Calcs (For Debugging)
  std::string rw_engine_list = FitPar::Config().GetParS("FitWeight.niwg_rw_veto");
  bool niwg_2012a   = rw_engine_list.find("niwg_2012a")  == std::string::npos;
  bool niwg_2014a   = rw_engine_list.find("niwg_2014a")  == std::string::npos;
  bool niwg_pimult  = rw_engine_list.find("niwg_pimult")  == std::string::npos;
  bool niwg_mec     = rw_engine_list.find("niwg_mec")  == std::string::npos;
  bool niwg_rpa     = rw_engine_list.find("niwg_rpa")  == std::string::npos;
  bool niwg_eff_rpa = rw_engine_list.find("niwg_eff_rpa")  == std::string::npos;
  bool niwg_proton  = rw_engine_list.find("niwg_protonFSIbug")  == std::string::npos;
  bool niwg_hadron  = rw_engine_list.find("niwg_HadronMultSwitch")  == std::string::npos;

  // Add the RW Calcs
  if (niwg_2012a)   niwg_rw->AdoptWghtCalc( "niwg_2012a",        new niwg::rew::NIWGReWeight2012a        );
  if (niwg_2014a)   niwg_rw->AdoptWghtCalc( "niwg_2014a",        new niwg::rew::NIWGReWeight2014a        );
  if (niwg_pimult)  niwg_rw->AdoptWghtCalc( "niwg_pimult",       new niwg::rew::NIWGReWeightPiMult       );
  if (niwg_mec)     niwg_rw->AdoptWghtCalc( "niwg_mec",          new niwg::rew::NIWGReWeightMEC          );
  if (niwg_rpa)     niwg_rw->AdoptWghtCalc( "niwg_rpa",          new niwg::rew::NIWGReWeightRPA          );
  if (niwg_eff_rpa) niwg_rw->AdoptWghtCalc( "niwg_eff_rpa",      new niwg::rew::NIWGReWeightEffectiveRPA );
  if (niwg_proton)  niwg_rw->AdoptWghtCalc( "niwg_protonFSIbug", new niwg::rew::NIWGReWeightProtonFSIbug );
  if (niwg_hadron)  niwg_rw->AdoptWghtCalc( "niwg_HadronMultSwitch", new niwg::rew::NIWGReWeightHadronMultSwitch );

  niwg_rw->Reconfigure();
}
#endif

//********************************************************************
#ifdef __NUWRO_REWEIGHT_ENABLED__
void FitWeight::SetupNuwroRW(){
//********************************************************************

  LOG(FIT) << "Setting up NUWRO RW" << std::endl;
  using_nuwro   = true;
  nuwro_changed = true;

  // Create Engine
  nuwro_rw = new nuwro::rew::NuwroReWeight();

  // Get List of Veto Calcs (For Debugging)
  std::string rw_engine_list = FitPar::Config().GetParS("FitWeight.nuwro_rw_veto");
  bool xsec_qel  = rw_engine_list.find("nuwro_QEL")  == std::string::npos;
  bool xsec_flag = rw_engine_list.find("nuwro_FlagNorm") == std::string::npos;
  bool xsec_res  = rw_engine_list.find("nuwro_RES") == std::string::npos;

  // Add the RW Calcs
  if (xsec_qel)  nuwro_rw->AdoptWghtCalc( "nuwro_QEL", new nuwro::rew::NuwroReWeight_QEL );
  if (xsec_flag) nuwro_rw->AdoptWghtCalc( "nuwro_FlagNorm", new nuwro::rew::NuwroReWeight_FlagNorm );
  //if (xsec_res)  nuwro_rw->AdoptWghtCalc( "nuwro_RES",  new nuwro::rew::NuwroReWeight_SPP );

  nuwro_rw->Reconfigure();
}
#endif

#ifdef __T2KREW_ENABLED__
//********************************************************************
void FitWeight::SetupT2KRW(){
//********************************************************************

  LOG(FIT) << "Setting up T2K RW" << std::endl;
  using_t2k   = true;
  t2k_changed = true;

  // Create Main RW Engine
  t2k_rw = new t2krew::T2KReWeight();

  // Setup Sub RW Engines (Only activated for neut and niwg)
  t2k_neutrw = new t2krew::T2KNeutReWeight();
  t2k_niwgrw = new t2krew::T2KNIWGReWeight();

  t2k_rw->AdoptWghtEngine("neut_rw", t2k_neutrw);
  t2k_rw->AdoptWghtEngine("niwg_rw", t2k_niwgrw);

  t2k_rw->Reconfigure();
}
#endif

#ifdef __GENIE_ENABLED__
//********************************************************************
void FitWeight::SetupGenieRW(){
//********************************************************************

  LOG(FIT) << "Setting up GENIE RW" << std::endl;
  using_genie   = true;
  genie_changed = true;

  // Create Engine
  GHepRecord::SetPrintLevel(-2);
  genie_rw = new genie::rew::GReWeight();

  // Get List of Vetos (Just for debugging)
  std::string rw_engine_list = FitPar::Config().GetParS("FitWeight.genie_rw_veto");
  bool xsec_ncel    = rw_engine_list.find("xsec_ncel")  == std::string::npos;
  bool xsec_ccqe    = rw_engine_list.find("xsec_ccqe") == std::string::npos;
  bool xsec_coh     = rw_engine_list.find("xsec_coh") == std::string::npos;
  bool xsec_nnres   = rw_engine_list.find("xsec_nonresbkg") == std::string::npos;
  bool xsec_nudis   = rw_engine_list.find("nuclear_dis") == std::string::npos;
  bool xsec_resdec  = rw_engine_list.find("hadro_res_decay") == std::string::npos;
  bool xsec_fzone   = rw_engine_list.find("hadro_intranuke") == std::string::npos;
  bool xsec_intra   = rw_engine_list.find("hadro_fzone") == std::string::npos;
  bool xsec_agky    = rw_engine_list.find("hadro_agky") == std::string::npos;
  bool xsec_qevec   = rw_engine_list.find("xsec_ccqe_vec") == std::string::npos;
  bool xsec_dis     = rw_engine_list.find("xsec_dis") == std::string::npos;
  bool xsec_nc      = rw_engine_list.find("xsec_nc") == std::string::npos;
  bool xsec_ccres   = rw_engine_list.find("xsec_ccres") == std::string::npos;
  bool xsec_ncres   = rw_engine_list.find("xsec_ncres") == std::string::npos;
  bool xsec_nucqe   = rw_engine_list.find("nuclear_qe") == std::string::npos;

  // Now actually add the RW Calcs
  if (xsec_ncel)   genie_rw->AdoptWghtCalc( "xsec_ncel",       new genie::rew::GReWeightNuXSecNCEL      );
  if (xsec_ccqe)   genie_rw->AdoptWghtCalc( "xsec_ccqe",       new genie::rew::GReWeightNuXSecCCQE      );
  if (xsec_coh)    genie_rw->AdoptWghtCalc( "xsec_coh",        new genie::rew::GReWeightNuXSecCOH       );
  if (xsec_nnres)  genie_rw->AdoptWghtCalc( "xsec_nonresbkg",  new genie::rew::GReWeightNonResonanceBkg );
  if (xsec_nudis)  genie_rw->AdoptWghtCalc( "nuclear_dis",     new genie::rew::GReWeightDISNuclMod      );
  if (xsec_resdec) genie_rw->AdoptWghtCalc( "hadro_res_decay", new genie::rew::GReWeightResonanceDecay  );
  if (xsec_fzone)  genie_rw->AdoptWghtCalc( "hadro_fzone",     new genie::rew::GReWeightFZone           );
  if (xsec_intra)  genie_rw->AdoptWghtCalc( "hadro_intranuke", new genie::rew::GReWeightINuke           );
  if (xsec_agky)   genie_rw->AdoptWghtCalc( "hadro_agky",      new genie::rew::GReWeightAGKY            );
  if (xsec_qevec)  genie_rw->AdoptWghtCalc( "xsec_ccqe_vec",   new genie::rew::GReWeightNuXSecCCQEvec   );
  if (xsec_dis)    genie_rw->AdoptWghtCalc( "xsec_dis",        new genie::rew::GReWeightNuXSecDIS       );
  if (xsec_nc)     genie_rw->AdoptWghtCalc( "xsec_nc",         new genie::rew::GReWeightNuXSecNC        );
  if (xsec_ccres)  genie_rw->AdoptWghtCalc( "xsec_ccres",      new genie::rew::GReWeightNuXSecCCRES     );
  if (xsec_ncres)  genie_rw->AdoptWghtCalc( "xsec_ncres",      new genie::rew::GReWeightNuXSecNCRES     );
  if (xsec_nucqe)  genie_rw->AdoptWghtCalc( "nuclear_qe",      new genie::rew::GReWeightFGM             );

  genie_rw->Reconfigure();
}
#endif


//********************************************************************
double FitWeight::GetDialValue(std::string name, std::string option){
//********************************************************************

  int this_enum = this->GetDialEnum( name );
  int this_pos  = this->GetDialPos(this_enum);
  double val =  this->GetDialValue(this_enum);

  if (!option.compare("ABS"))
    return dial_func[this_pos].Eval(val);
  else if (!option.compare("FRC"))
    return dial_func[this_pos].Eval(val)/dial_func[this_pos].Eval(0.0);
  else
    return val;
}

//********************************************************************
double FitWeight::GetDialValue(int this_enum){
//********************************************************************
  unsigned int pos = this->GetDialPos(this_enum);
  return dial_values.at(pos);
}

//********************************************************************
void FitWeight::UpdateWeightEngine(const double* x){
//********************************************************************
  this->SetAllDials(x, dial_enums.size());
  return;
}

//********************************************************************
bool FitWeight::HasRWDialChanged(const double* x){
//********************************************************************
  
  for (int i = 0; i < dial_values.size(); i++){

    int rw_enum = dial_enums.at(i);
    int id = int(rw_enum-(rw_enum%1000))/1000;

    if (id == kNORM) continue;
    
    if (x[i] != dial_values.at(i)) return true;
  }
  
  return false;
}

//********************************************************************
void FitWeight::SetAllDials(const double* x, int npt){
//********************************************************************
  for (int i = 0; i < npt; i++){
    int this_enum = dial_enums.at(i);
    this->SetDialValue(this_enum, x[i]);
  }
  return;
};

//********************************************************************
void FitWeight::GetAllDials(double* x, int npt){
//********************************************************************

  for (int i = 0; i < npt; i++){
    int this_enum = dial_enums.at(i);
    x[i] = this->GetDialValue(this_enum);
  }

  return;
};

//********************************************************************
double FitWeight::GetSampleNorm(std::string samplename){
//********************************************************************
  std::string norm_dial = samplename + "_norm";

  // Loop through and see if we have one
  std::vector<std::string>::iterator naIter = dial_names.begin();
  bool found_dial = false;
  for ( ; naIter != dial_names.end(); naIter++){
    if ( ((std::string)(*naIter)) == norm_dial){
      found_dial = true;
      break;
    }
  }

  if (!found_dial and !samplename.empty()) {

    LOG(FIT) << " Late initialisation of norm: "<<norm_dial<<std::endl;
    this->IncludeDial( norm_dial, kNORM, 1.0);
    LOG(FIT) << "RECONFIGURING"<<std::endl;
    this->Reconfigure();
    return 1.0;

  } else {
    LOG(FIT) << " Getting sample norm "<< norm_dial <<" = "<<this->GetDialValue( norm_dial )<<std::endl;
    return this->GetDialValue( norm_dial );
  }
}


//********************************************************************
std::vector<std::string> FitWeight::GetDialNames(){
//********************************************************************
  return dial_names;
}

//********************************************************************
std::vector<int> FitWeight::GetDialEnums(){
//********************************************************************
  return dial_enums;
}

//********************************************************************
std::vector<double> FitWeight::GetDialValues(){
//********************************************************************
  return dial_values;
}

//********************************************************************
void FitWeight::SetupEventCoeff(BaseFitEvt* event){
//********************************************************************
  if (!spline_head) spline_head = new FitSplineHead();
  spline_head->SetupEventWeights(event);
}

//********************************************************************
void FitWeight::GenSplines(BaseFitEvt* event, bool save_graph){
//********************************************************************

  double nom = this->CalcWeight(event);
  event->dial_coeff->SetAt(nom,0);

  // Get Current Dial Values and save to reset
  std::list<FitSpline*>::iterator spl_iter = spline_head->SplineObjects.begin();
  for ( ; spl_iter != spline_head->SplineObjects.end(); spl_iter++){
    FitSpline* spl = (*spl_iter);
    int dim = spl->ndim;

    // ND Splines
    if (dim == 1) this->Fit1DSplineCoeff(event, spl, nom, save_graph);
    else if (dim == 2) this->Fit2DSplineCoeff(event, spl, nom, save_graph);
    else if (dim >= 3) this->FitNDSplineCoeff(event, spl, nom, save_graph);

  }
  return;
}

//********************************************************************
void FitWeight::Fit2DSplineCoeff(BaseFitEvt* event, FitSpline* spl, double nom, bool save_graph){
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

  TGraph2D gr_2D_scan = TGraph2D(n_knots_x  * n_knots_y);

  // Loop over grid
  for (std::vector<double>::iterator iter_x = knots_x.begin();
       iter_x != knots_x.end(); iter_x++){

    // X RW Value
    val_x = (*iter_x);
    this->SetDialValue(enum_x, val_x);
    this->Reconfigure(true);

    for (std::vector<double>::iterator iter_y = knots_y.begin();
	 iter_y != knots_y.end();iter_y++){

      // Y RW Value
      val_y = (*iter_y);
      this->SetDialValue(enum_y, val_y);

      weightval = this->CalcWeight(event)/nom;
      if (weightval != 1.0) hasresponse = true;

      gr_2D_scan.SetPoint(count, val_x, val_y, weightval);
      count++;

    }
  }

  // Save GRAPH
  if (hasresponse and save_graph){
    gr_2D_scan.SetName(Form("SplineFit_%s_%s", spl->id.c_str(), spl->form.c_str()));
    std::vector<std::string> titles = PlotUtils::FillVectorSFromString(spl->id, ",");
    gr_2D_scan.SetTitle(Form("SplineFit_%s_%s;%s;%s;Weight Response",
			     spl->id.c_str(),
			     spl->form.c_str(), titles[0].c_str(), titles[1].c_str() ));

    gr_2D_scan.Write();
  }

  this->SetDialValue(enum_x, cur_x);
  this->SetDialValue(enum_y, cur_y);

  return;
}

//********************************************************************
void FitWeight::Fit1DSplineCoeff(BaseFitEvt* event, FitSpline* spl, double nom, bool save_graph){
//********************************************************************

  int this_enum = spl->var_enums[0];
  double current = this->GetDialValue(this_enum);
  int    npar   = spl->npar;
  std::vector<double> knots = spl->x_vals[0];
  int n_knots = knots.size();
  double val = 0.0;

  double* allweights;
  double* allvals;
  allweights = new double[n_knots];
  allvals    = new double[n_knots];

  int count = 0;
  double weightval = 0.0;

  bool hasresponse = false;

  for (std::vector<double>::iterator iter = knots.begin();
       iter != knots.end(); iter++){
    val = (*iter);

    this->SetDialValue(this_enum, val);
    this->Reconfigure(true);

    weightval = this->CalcWeight(event)/nom;
    if (weightval != 1.0) hasresponse = true;

    allweights[count] = weightval;
    allvals[count++]  = val;
  }
  if (!hasresponse){
    event->dial_coeff->SetAt( -999.9, 1 );
  }

  TGraph* gr = new TGraph(n_knots, &knots[0], allweights);
  TF1* f1 = new TF1("f1", spl, -1.0 + knots[0], 1.0 + knots[knots.size()-1], npar);

  if (save_graph and hasresponse) f1->SetNpx(400);

  // Check for TSpline3
  if (spl->needs_fit){
    gr->Fit(f1,"WQM");
    for (int i = 0; i < npar; i++){
      event->dial_coeff->SetAt( f1->GetParameter(i), i + spl->offset );
    }
  } else {
    std::vector<double> dial_coeff = spl->GetSplineCoeff(allweights);
    for (int i = 0; i < npar; i++){
      event->dial_coeff->SetAt( dial_coeff[i], i + spl->offset );
      f1->FixParameter( i, dial_coeff[i]);
    }
    gr->Fit(f1,"WQM");

  }

  if (save_graph and hasresponse){

    TSpline3* spl3 = new TSpline3(Form("Spline3_dial%i_%s_%s;%s;Weight Response",
				       this_enum, spl->id.c_str(),
				       spl->form.c_str(), spl->id.c_str() ),
				  &knots[0], allweights, n_knots);
    TCanvas* c3 = new TCanvas("c3","c3",800,600);
    c3->cd();
    spl3->SetLineColor(kBlue);
    spl3->Draw("C");

    gr->SetTitle(Form("SplineFit_dial%i_%s_%s;%s;Weight Response",
		      this_enum, spl->id.c_str(),
		      spl->form.c_str(), spl->id.c_str() ));
    gr->SetMarkerStyle(22);
    gr->SetName(Form("SplineFit_dial%i_%s_%s", this_enum, spl->id.c_str(), spl->form.c_str()));
    gr->Write();


    gr->Draw("SAME P");
    f1->Draw("SAME C");
    c3->Update();
    c3->Write(Form("Spline3_dial%i_%s_%s;%s;Weight Response",
		   this_enum, spl->id.c_str(),
		   spl->form.c_str(), spl->id.c_str() ));
    delete c3;
    delete spl3;

  }

  delete gr;
  delete f1;
  delete allweights;

  this->SetDialValue(this_enum, current);

}

//********************************************************************
void FitWeight::ReadSplineHead(FitSplineHead* splhead){
//********************************************************************
  spline_head = splhead;
  spline_head->Reconfigure(this->dial_enums, this->dial_values);
}


//********************************************************************
void FitWeight::SetupSpline(std::string dialname, std::string splinename, std::string points_def){
//********************************************************************

  // Create spline head if none setup
  if (!spline_head) spline_head = new FitSplineHead();

  // Parse Enum Mapping
  std::vector<std::string> parsed_dials = PlotUtils::FillVectorSFromString(dialname,",");
  std::vector<int> list_enums;
  for (UInt_t i = 0; i < parsed_dials.size(); i++){
    list_enums.push_back( this->GetDialEnum( parsed_dials.at(i) ) );
  }

  // Add new spline
  FitSpline* spl = new FitSpline(dialname, splinename, list_enums, points_def );
  spline_head->AddSpline(spl);

  return;
}

//********************************************************************
void FitWeight::ResetSplines(){
//********************************************************************
  delete spline_head;
  spline_head = NULL;
}

//********************************************************************
double FitWeight::CalcSplineWeight(BaseFitEvt* evt){
//********************************************************************
  double rw_weight =  spline_head->CalcWeight(evt->dial_coeff->GetArray());
  return rw_weight;
}

//********************************************************************  
bool FitWeight::HasDialChanged(){
//********************************************************************  
  return dial_changed;
};


// Global Conversion Functions
// ---------------------------
//********************************************************************
TF1 FitBase::GetRWConvFunction(std::string type, std::string name){
//********************************************************************

  std::string dialfunc = "x";
  std::string parType = type;
  double low = -10.0;
  double high = 10.0;

  if (parType.find("parameter") == std::string::npos)
    parType += "_parameter";

  string line;
  ifstream card((string(getenv("EXT_FIT"))+"/parameters/dial_conversion.card").c_str(), ifstream::in );

  while(getline(card, line, '\n')){

    istringstream stream(line);
    string token, parname;
    int val = 0;
    double entry;

    if (line.c_str()[0] == '#') continue;
    while(getline(stream, token, ' ')){
      stream >> ws;

      istringstream stoken(token);
      stoken >> entry;

      if (val == 0){
	if (token.compare(parType) != 0){ break; }
      } else if (val == 1){
	if(token.compare(name) != 0){ break; }
      } else if (val == 2){
      } else if (val == 3){ dialfunc = token;
      } else if (val == 3){
      } else if (val == 4){ low  = entry;
      } else if (val == 5){ high = entry;
      } else break;

      val++;
    }
  }

  TF1 convfunc = TF1((name + "_convfunc").c_str(),dialfunc.c_str(),low,high);
  return convfunc;
}

//********************************************************************
std::string FitBase::GetRWUnits(std::string type, std::string name){
//********************************************************************

  std::string unit = "sig.";
  std::string parType = type;

  if (parType.find("parameter") == std::string::npos)
    parType += "_parameter";


  string line;
  ifstream card((string(getenv("EXT_FIT"))+"/parameters/dial_conversion.card").c_str(), ifstream::in );

  while(getline(card, line, '\n')){

    istringstream stream(line);
    string token, parname;
    int val = 0;
    double entry;

    if (line.c_str()[0] == '#') continue;
    while(getline(stream, token, ' ')){
      stream >> ws;

      istringstream stoken(token);
      stoken >> entry;

      if (val == 0){
	if (token.compare(parType) != 0){ break; }
      } else if (val == 1){
	if(token.compare(name) != 0){ break; }
      } else if (val == 2){ unit = token;
      } else if (val == 3){
      } else if (val == 4){
      } else if (val == 5){
      } else break;

      val++;
    }
  }

  return unit;
}

//********************************************************************
double FitBase::RWAbsToSigma(std::string type, std::string name, double val){
//********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.GetX(val);
  if (fabs(conv_val) < 1E-10) conv_val= 0.0;
  return conv_val;
}

//********************************************************************
double FitBase::RWSigmaToAbs(std::string type, std::string name, double val){
//********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.Eval(val);
  return conv_val;
}


//********************************************************************
double FitBase::RWFracToSigma(std::string type, std::string name, double val){
//********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.GetX((val * f1.Eval(0.0)));
  if (fabs(conv_val) < 1E-10) conv_val = 0.0;
  return conv_val;
}

//********************************************************************
double FitBase::RWSigmaToFrac(std::string type, std::string name, double val){
//********************************************************************
  TF1 f1 = GetRWConvFunction(type, name);
  double conv_val = f1.Eval(val)/f1.Eval(0.0);
  return conv_val;
}
