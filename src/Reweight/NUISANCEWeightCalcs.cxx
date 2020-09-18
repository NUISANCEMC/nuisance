#include "NUISANCEWeightCalcs.h"

#include "FitEvent.h"
#include "FitUtils.h"
#include "GeneralUtils.h"
#include "NUISANCESyst.h"
#include "WeightUtils.h"

using namespace Reweight;


// ------ New dials for SF RWing ------

// Constant RW per shell

SFRW_ShellNormCalc::SFRW_ShellNormCalc()
{
    //std::cout << "Setting up pShellNorm Weight Engine" << std::endl;
    fNormPShellC = 1.0;
    fNormSShellC = 1.0;

    fNormP12ShellO = 1.0;
    fNormP32ShellO = 1.0;
    fNormSShellO = 1.0;

    
}



double SFRW_ShellNormCalc::CalcWeight(BaseFitEvt *evt) {
    int mode = abs(evt->Mode);
    FitEvent *fevt = static_cast<FitEvent *>(evt);
    int A = fevt->GetTargetA();
    int Z = fevt->GetTargetZ();
    
    double w = 1.0;
    
    //std::cout << "Calculating pShellNorm weight" << std::endl;
    
    if (mode == 1) // CCQE only
    {
        double Emiss = FitUtils::GetEmiss(fevt); // Compute Emiss
        
        if (Z==6 && A==12) // Carbon
        {
            if(Emiss>10 && Emiss<25) // P-shell
            {
                w *= fNormPShellC;
            }
            
            if(Emiss>25 && Emiss<60) // S-shell
            {
                w *= fNormSShellC;
            }
        }
        
        if (Z==8 && A==16) // Oxygen
        {
            if(Emiss>8 && Emiss<15) // P1/2-shell
            {
                w *= fNormP12ShellO;
            }
            
            if(Emiss>15 && Emiss<25) // P3/2-shell
            {
                w *= fNormP32ShellO;
            }
            
            if(Emiss>25 && Emiss<70) // S-shell
            {
                w *= fNormSShellO;
            }
        }
        
        //std::cout << "Applying weight to CCQE event: " << fNormPShell << std::endl;
    }
    
    return w;
}

void SFRW_ShellNormCalc::SetDialValue(std::string name, double val) {
    SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void SFRW_ShellNormCalc::SetDialValue(int rwenum, double val) {
    int curenum = rwenum % 1000;
    
    // Check Handled
    if (!IsHandled(curenum))
        return;
    if (curenum == kSFRW_pShellNorm_C)
        fNormPShellC = val;
    if (curenum == kSFRW_sShellNorm_C)
        fNormSShellC = val;
    
    if (curenum == kSFRW_p12ShellNorm_O)
        fNormP12ShellO = val;
    if (curenum == kSFRW_p32ShellNorm_O)
        fNormP32ShellO = val;
    if (curenum == kSFRW_sShellNorm_O)
        fNormSShellO = val;
}

bool SFRW_ShellNormCalc::IsHandled(int rwenum) {
    int curenum = rwenum % 1000;
    
    //std::cout << "Checking if pShellNorm dial is handled" << std::endl;
    //std::cout << "  curenum" << curenum << std::endl;
    //std::cout << "  rwenum" << rwenum << std::endl;
    //std::cout << "  rwenum % 1000" << rwenum % 1000 << std::endl;
    
    switch (curenum) {
        case kSFRW_pShellNorm_C:
            return true;
        case kSFRW_sShellNorm_C:
            return true;
        
        case kSFRW_p12ShellNorm_O:
            return true;
        case kSFRW_p32ShellNorm_O:
            return true;
        case kSFRW_sShellNorm_O:
            return true;
        
        default:
            return false;
    }
}

// end Constant RW per shell


// Gaussian RW per shell

SFGausRW_ShellCalc::SFGausRW_ShellCalc()
{
    // C, p-shell
    fGaus_pShell_C[kPosNorm] = 0.0;
    fGaus_pShell_C[kPosP] = 15.0;
    fGaus_pShell_C[kPosW] = 7.0;
    
    // C, s-shell
    fGaus_sShell_C[kPosNorm] = 0.0;
    fGaus_sShell_C[kPosP] = 35.0;
    fGaus_sShell_C[kPosW] = 12.0;
    
    // O, p1/2-shell
    fGaus_p12Shell_O[kPosNorm] = 0.0;
    fGaus_p12Shell_O[kPosP] = 0.0;
    fGaus_p12Shell_O[kPosW] = 1.0;
    
    // O, p3/2-shell
    fGaus_p32Shell_O[kPosNorm] = 0.0;
    fGaus_p32Shell_O[kPosP] = 0.0;
    fGaus_p32Shell_O[kPosW] = 1.0;
    
    // O, s-shell
    fGaus_sShell_O[kPosNorm] = 0.0;
    fGaus_sShell_O[kPosP] = 0.0;
    fGaus_sShell_O[kPosW] = 1.0;

    // SRC strength
    fSRC_strength = 1.0;
}

 

double SFGausRW_ShellCalc::CalcWeight(BaseFitEvt *evt) {
    int mode = abs(evt->Mode);
    FitEvent *fevt = static_cast<FitEvent *>(evt);
    int A = fevt->GetTargetA();
    int Z = fevt->GetTargetZ();
    
    double w = 1.0;
    
    //std::cout << "Calculating pShellNorm weight" << std::endl;
    
    if (mode == 1) // CCQE only
    {
      // Get the number of outgoing protons from the primary vertex
      UInt_t npart = fevt->Npart();
      int Nprotons = 0;
      for (UInt_t i = 0; i < npart; i++)
        {
	  bool isPreFSI = fevt->fPrimaryVertex[i];
	  if (!isPreFSI)
	    continue;
	  int partPDG = fevt->fParticlePDG[i];
	  if (partPDG==2212) Nprotons++;
        }
        
      // RW the SRC part
      if (Nprotons==2)
        {
	  w *= fSRC_strength;
        }
        

        double Emiss = FitUtils::GetEmiss(fevt); // Compute Emiss
        
        if (Z==6 && A==12) // Carbon
        {
	  //if(Emiss>10 && Emiss<25) // P-shell
            {
                w *= GetGausWeight(Emiss, fGaus_pShell_C);
            }
            
	  //if(Emiss>25 && Emiss<60) // S-shell
            {
                w *= GetGausWeight(Emiss, fGaus_sShell_C);
            }
        }
        
        if (Z==8 && A==16) // Oxygen
        {
	  //if(Emiss>8 && Emiss<15) // P1/2-shell
            {
                w *= GetGausWeight(Emiss, fGaus_p12Shell_O);
            }
            
	  //if(Emiss>15 && Emiss<25) // P3/2-shell
            {
                w *= GetGausWeight(Emiss, fGaus_p32Shell_O);
            }
            
	  //if(Emiss>25 && Emiss<70) // S-shell
            {
                w *= GetGausWeight(Emiss, fGaus_sShell_O);
            }
        }
        
        //std::cout << "Applying weight to CCQE event: " << fNormPShell << std::endl;
    }
    
    return w;
}

double SFGausRW_ShellCalc::GetGausWeight(double Emiss, double vals[])
{
    double g = 1.0;
    
    double norm = vals[kPosNorm];
    double mean = vals[kPosP];
    double sigma = vals[kPosW];
    
    g *= norm * exp(-0.5 * (Emiss - mean) * (Emiss - mean) / (sigma * sigma));
    
    return 1.0 + g;
}
    

void SFGausRW_ShellCalc::SetDialValue(std::string name, double val) {
    SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void SFGausRW_ShellCalc::SetDialValue(int rwenum, double val) {
    int curenum = rwenum % 1000;
    
    // Check Handled
    if (!IsHandled(curenum))
        return;
    
    // C, p-shell
    for (int i = kGaussian_pShell_C_norm; i <= kGaussian_pShell_C_w; i++){
        if (i == curenum)
        {
            int index = i - kGaussian_pShell_C_norm;
            fGaus_pShell_C[index] = val;
        }
    }

    // C, s-shell
    for (int i = kGaussian_sShell_C_norm; i <= kGaussian_sShell_C_w; i++){
        if (i == curenum)
        {
            int index = i - kGaussian_sShell_C_norm;
            fGaus_sShell_C[index] = val;
        }
    }
    
    // O, p1/2-shell
    for (int i = kGaussian_p12Shell_O_norm; i <= kGaussian_p12Shell_O_w; i++){
        if (i == curenum)
        {
            int index = i - kGaussian_p12Shell_O_norm;
            fGaus_p12Shell_O[index] = val;
        }
    }

    // O, p3/2-shell
    for (int i = kGaussian_p32Shell_O_norm; i <= kGaussian_p32Shell_O_w; i++){
        if (i == curenum)
        {
            int index = i - kGaussian_p32Shell_O_norm;
            fGaus_p32Shell_O[index] = val;
        }
    }
    
    // O, s-shell
    for (int i = kGaussian_sShell_O_norm; i <= kGaussian_sShell_O_w; i++){
        if (i == curenum)
        {
            int index = i - kGaussian_sShell_O_norm;
            fGaus_sShell_O[index] = val;
        }
    }

    if (kSRC_strength == curenum)
      fSRC_strength = val; // RW SRC part
}

bool SFGausRW_ShellCalc::IsHandled(int rwenum) {
    int curenum = rwenum % 1000;
    
    //std::cout << "Checking if pShellNorm dial is handled" << std::endl;
    //std::cout << "  curenum" << curenum << std::endl;
    //std::cout << "  rwenum" << rwenum << std::endl;
    //std::cout << "  rwenum % 1000" << rwenum % 1000 << std::endl;
    
    switch (curenum)
    {
        // RW SRC    
        case kSRC_strength:

        case kGaussian_pShell_C_norm:
        case kGaussian_pShell_C_p:
        case kGaussian_pShell_C_w:
        
        case kGaussian_sShell_C_norm:
        case kGaussian_sShell_C_p:
        case kGaussian_sShell_C_w:
        
        case kGaussian_p12Shell_O_norm:
        case kGaussian_p12Shell_O_p:
        case kGaussian_p12Shell_O_w:
        
        case kGaussian_p32Shell_O_norm:
        case kGaussian_p32Shell_O_p:
        case kGaussian_p32Shell_O_w:
            
        case kGaussian_sShell_O_norm:
        case kGaussian_sShell_O_p:
        case kGaussian_sShell_O_w:
            return true;
        
        default:
            return false;
    }
}
// end Gaussian RW per shell


// Pmiss RW

PmissRW_Calc::PmissRW_Calc()
{
  fPmissRW_pC = 0.0;
  fPmissRW_sC = 0.0;

  fPmissRW_p12O = 0.0;
  fPmissRW_p32O = 0.0;
  fPmissRW_sO = 0.0;

    
}

double PmissRW_Calc::CalcWeight(BaseFitEvt *evt) {
  int mode = abs(evt->Mode);
  FitEvent *fevt = static_cast<FitEvent *>(evt);
  int A = fevt->GetTargetA();
  int Z = fevt->GetTargetZ();
    
  double w = 1.0;
    
  //std::cout << "Calculating pShellNorm weight" << std::endl;
  //  if (fPmissRW_pC<-1.0 || fPmissRW_pC>1.0 || fPmissRW_sC<-1.0 || fPmissRW_sC>1.0)
  //  return 1.0;
        
  //  if (fPmissRW_p12O<-1.0 || fPmissRW_p12O>1.0 || fPmissRW_p32O<-1.0 || fPmissRW_p32O>1.0 || fPmissRW_sO<-1.0 || fPmissRW_sO>1.0)
  //    return 1.0;
    
	  if (mode == 1) // CCQE only
	    {
	      double Emiss=FitUtils::GetEmiss(fevt), pmiss = FitUtils::GetPmiss(fevt); // Compute Emiss & Pmiss
        
	      if (Z==6 && A==12) // Carbon
		{

		  if(Emiss>10 && Emiss<25) // P-shell
		    {
		      double pWeight = GetWeightCarbonP(pmiss, fPmissRW_pC);                
		      if (pWeight<0) return 0.0;
		      w *= pWeight;
		    }
            
		  if(Emiss>25 && Emiss<60) // S-shell
		    {
		      double sWeight = GetWeightCarbonS(pmiss, fPmissRW_sC);
		      if (sWeight<0) return 0.0;
		      w *= sWeight;
		    }
		}
        
	      if (Z==8 && A==16) // Oxygen
		{
		  if(Emiss>8 && Emiss<15) // P1/2-shell
		    {
		      double pWeight = GetWeightCarbonP(pmiss, fPmissRW_p12O);
                      if (pWeight<0) return 0.0;
                      w *= pWeight;
		    }
            
		  if(Emiss>15 && Emiss<25) // P3/2-shell
		    {
		      double pWeight = GetWeightCarbonP(pmiss, fPmissRW_p32O);
                      if (pWeight<0) return 0.0;
                      w *= pWeight;
		    }
            
		  if(Emiss>25 && Emiss<70) // S-shell
		    {
		      double sWeight = GetWeightCarbonS(pmiss, fPmissRW_sO);
                      if (sWeight<0) return 0.0;
                      w *= sWeight;
		    }
		}
        
	      //std::cout << "Applying weight to CCQE event: " << fNormPShell << std::endl;
	    }
    
  return w;
}

void PmissRW_Calc::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void PmissRW_Calc::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;
    
  // Check Handled
  if (!IsHandled(curenum))
    return;
  if (curenum == kPmissRW_pC)
    fPmissRW_pC = val;
  if (curenum == kPmissRW_sC)
    fPmissRW_sC = val;
    
  if (curenum == kPmissRW_p12O)
    fPmissRW_p12O = val;
  if (curenum == kPmissRW_p32O)
    fPmissRW_p32O = val;
  if (curenum == kPmissRW_sO)
    fPmissRW_sO = val;
}

bool PmissRW_Calc::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
    
  //std::cout << "Checking if pShellNorm dial is handled" << std::endl;
  //std::cout << "  curenum" << curenum << std::endl;
  //std::cout << "  rwenum" << rwenum << std::endl;
  //std::cout << "  rwenum % 1000" << rwenum % 1000 << std::endl;
    
  switch (curenum) {
  case kPmissRW_pC:
    return true;
  case kPmissRW_sC:
    return true;
        
  case kPmissRW_p12O:
    return true;
  case kPmissRW_p32O:
    return true;
  case kPmissRW_sO:
    return true;
        
  default:
    return false;
  }
}

double PmissRW_Calc::GetWeightCarbonP(double pmiss, double dial){
    
  // (e,e'p) data
  // First the data is normalized, then the  data_pmissPmax and data_pmissPmin
    
  double edges[8]={0., 40., 80., 120., 160., 200., 240., 280.}; // steps of 40.0 MeV
  double centers[7]={20., 60., 100., 140., 180., 220., 260.}; 
  double limits[7]={0., 60., 100., 140., 180., 220., 260};

  double data_pmissPmax[7]={2.532112E-03, 5.997095E-03, 6.735777E-03, 5.881405E-03, 3.130611E-03, 1.552484E-03, 6.109776E-04};
  double data_pmissPmin[7]={1.827925E-03, 5.294416E-03, 6.456885E-03, 5.295974E-03, 2.979768E-03, 1.277565E-03, 4.270061E-04};
  //
  double SFinput_pmissP[7]={2.579599E-03, 5.336049E-03, 6.743242E-03, 5.392459E-03, 3.033925E-03, 1.390269E-03, 5.244574E-04};
    
  if (pmiss>edges[7])
    return 1.0;

  for (int i=0; i<6; i++)
    {
      // linear interpolation between the points 
      double m_datamin, m_datamax, m_input;
      double c_datamin, c_datamax, c_input, pcenter;
      if (pmiss>limits[i] && pmiss<limits[i+1])
	{
          m_datamin = (data_pmissPmin[i+1] - data_pmissPmin[i]) / 40.;
          m_datamax = (data_pmissPmax[i+1] - data_pmissPmax[i]) / 40.;
	  m_input = (SFinput_pmissP[i+1] - SFinput_pmissP[i]) / 40.;
	  pcenter = centers[i];
	  c_datamin = data_pmissPmin[i];
	  c_datamax = data_pmissPmax[i];
	  c_input = SFinput_pmissP[i];

	  double curr_pmiss_datamin = m_datamin * (pmiss - pcenter) + c_datamin;
	  double curr_pmiss_datamax = m_datamax * (pmiss - pcenter) + c_datamax;
	  double curr_pmiss_input = m_input * (pmiss - pcenter) + c_input; 

	  return - (dial - 1) * (dial + 1) + (dial - 1) * dial * curr_pmiss_datamin / (2 * curr_pmiss_input) + dial * (dial + 1) * curr_pmiss_datamax / (2 * curr_pmiss_input);
	}
      /*
      else if(pmiss>0 && pmiss<centers[0])
	{
	  m_datamin = (data_pmissPmin[1] - data_pmissPmin[0]) / 40.;
          m_datamax = (data_pmissPmax[1] - data_pmissPmax[0]) / 40.;
          m_input = (SFinput_pmissP[1] - SFinput_pmissP[0]) / 40.;
	  pcenter = centers[0];
	  c_datamin = data_pmissPmin[0];
          c_datamax = data_pmissPmax[0];
          c_input = SFinput_pmissP[0];
	  break;
	}  

      else
	{
	  m_datamin = (data_pmissPmin[6] - data_pmissPmin[5]) / 40.;
          m_datamax = (data_pmissPmax[6] - data_pmissPmax[5]) / 40.;
          m_input = (SFinput_pmissP[6] - SFinput_pmissP[5]) / 40.;
          pcenter= centers[5];
          c_datamin = data_pmissPmin[5];
          c_datamax = data_pmissPmax[5];
          c_input = SFinput_pmissP[5];
          break;
	}
      double curr_pmiss_datamin = m_datamin * (pmiss - pcenter) + c_datamin;
      double curr_pmiss_datamax = m_datamax * (pmiss - pcenter) + c_datamax;
      double curr_pmiss_input = m_input * (pmiss - pcenter) + c_input;
       
      return - (dial - 1) * (dial + 1) + (dial - 1) * dial * curr_pmiss_datamin / (2 * curr_pmiss_input) + dial * (dial + 1) * curr_pmiss_datamax / (2 * curr_pmiss_input);
	}
      else if ()
      */
    }
    
  return 1.0;
}

double PmissRW_Calc::GetWeightCarbonS(double pmiss, double dial){
    
  // (e,e'p) data
  // The data points were first normalized, then the values at each point...
    
  double edges[8]={0., 40., 80., 120., 160., 200., 240., 280.}; // steps of 40.0 MeV
  double limits[7]={0., 60., 100., 140., 180., 220., 260};
  double centers[7]={20., 60., 100., 140., 180., 220., 260.};
    
  double data_pmissSmax[7]={8.057835E-03, 7.299071E-03, 4.972835E-03, 3.089120E-03, 1.585566E-03, 8.720613E-04, 2.897758E-04};
  double data_pmissSmin[7]={7.244396E-03, 6.975782E-03, 4.620011E-03, 2.808731E-03, 1.372134E-03, 5.525372E-04, 2.601529E-04};
  //
  double SFinput_pmissS[7]={8.212913E-03, 6.918538E-03, 4.856345E-03, 2.840391E-03, 1.396290E-03, 5.672131E-04, 2.083111E-04};
    
  if (pmiss>limits[6])
    return 1.0;

  for (int i=0; i<6; i++)
    {
      /*
      if (pmiss>edges[i] && pmiss<edges[i+1])
	{
	  // linear interpolation between the points                                                                                                                    

          double m_datamin = (data_pmissSmin[i+1] - data_pmissSmin[i]) / 40.;
          double m_datamax = (data_pmissSmax[i+1] - data_pmissSmax[i]) / 40.;
          double m_input = (SFinput_pmissS[i+1] - SFinput_pmissS[i]) / 40.;

          double curr_pmiss_datamin = m_datamin * (pmiss - (edges[i]+20.)) +data_pmissSmin[i];
          double curr_pmiss_datamax = m_datamax * (pmiss - (edges[i]+20.)) + data_pmissSmax[i];
          double curr_pmiss_input = m_input * (pmiss - (edges[i]+20.)) + SFinput_pmissS[i];

	  return - (dial - 1) * (dial + 1) + (dial - 1) * dial * curr_pmiss_datamin / (2 * curr_pmiss_input) + dial * (dial + 1) * curr_pmiss_datamax / (2 * curr_pmiss_input);
	}
      */
      
      // linear interpolation between the points

      double m_datamin, m_datamax, m_input;
      double c_datamin, c_datamax, c_input, pcenter;
      if (pmiss>limits[i] && pmiss<limits[i+1])
        {
          m_datamin = (data_pmissSmin[i+1] - data_pmissSmin[i]) / 40.;
          m_datamax = (data_pmissSmax[i+1] - data_pmissSmax[i]) / 40.;
          m_input = (SFinput_pmissS[i+1] - SFinput_pmissS[i]) / 40.;
          pcenter = centers[i];
          c_datamin = data_pmissSmin[i];
          c_datamax = data_pmissSmax[i];
          c_input = SFinput_pmissS[i];

          double curr_pmiss_datamin = m_datamin * (pmiss - pcenter) + c_datamin;
          double curr_pmiss_datamax = m_datamax * (pmiss - pcenter) + c_datamax;
          double curr_pmiss_input = m_input * (pmiss - pcenter) + c_input;

          return - (dial - 1) * (dial + 1) + (dial - 1) * dial * curr_pmiss_datamin / (2 * curr_pmiss_input) + dial * (dial + 1) * curr_pmiss_datamax / (2 * curr_pmiss_input);
        }
    }
    
  return 1.0;
}

// end Pmiss RW


// ------ End of new SF dials ------


// FSI RW modif 

FSIRW_Calc::FSIRW_Calc() {
    
  fFSIRW_noFSI = 1.0; // no FSI
  fFSIRW_elasticFSI = 1.0; // elastic FSI (change in the kinematics of primary vertex particles only)
  fFSIRW_inelasticFSI = 1.0; // extra-nucleons and same pion remains (if any)
  fFSIRW_pionProdFSI = 1.0; // pion production
  fFSIRW_pionAbsFSI = 1.0; // pion absorption
}

double FSIRW_Calc::CalcWeight(BaseFitEvt *evt) {
  double eps = 0.0001;
    
    int mode = abs(evt->Mode);
    FitEvent *fevt = static_cast<FitEvent *>(evt);
    double w = 1.0;
    
    if (mode<30) // all CC modes
      {
        // First, get the PDG of particles at vert and fs
        
	std::vector<int> PDGvert = FitUtils::GetPDGvert(fevt), PDGfs = FitUtils::GetPDGfs(fevt);
        
        // Now check the different FSI cases
        
        // Do we have the same particles in the vert and fsi?
        
        bool samePartVertFS = SameParticlesVertFS(PDGvert, PDGfs);
        
        if (samePartVertFS == true)
	  {
            // Same particles => either no FSI or elastic FSI
            // Get the momenta and evaluate the variation
	    //	    std::cout<<"no or elas FSI"<<std::endl;
	    std::vector<TLorentzVector> pvert = FitUtils::GetPvert(fevt), pfs = FitUtils::GetPfs(fevt);
            
            bool noFSI = IsNoFSI(PDGvert, pvert, PDGfs, pfs, eps);
            
            if (noFSI == true)
	      {
	      w *= fFSIRW_noFSI; // no FSI
	      //std::cout<<"noFSI"<<std::endl;
	      }
            else
	      {
		w *= fFSIRW_elasticFSI; // elastic
		//std::cout<<"elasFSI"<<std::endl;
	      }
	  }
        
        else
	  {
	    //std::cout<<"inelasFSI"<<std::endl;
            // The particles changed due to FSI
            // Inelactic: either no extra pions produced, exta pion production, or pion absorption
            
	    int Npifs = GetNpi(PDGfs);
	    int Npivert = GetNpi(PDGvert);

	    if (Npifs>Npivert) // Pion production
	      w *= fFSIRW_pionProdFSI;

            else if (Npifs<Npivert) // Pion absorption
	      w *= fFSIRW_pionAbsFSI;

	    else // Inelastic
	      w *= fFSIRW_inelasticFSI;

	  }
      }
    
    return w;
}

void FSIRW_Calc::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void FSIRW_Calc::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;
    
  // Check Handled
  if (!IsHandled(curenum))
    return;
  if (curenum == kFSIRW_noFSI)
    fFSIRW_noFSI = val;
  if (curenum == kFSIRW_elasticFSI)
    fFSIRW_elasticFSI = val;
  if (curenum == kFSIRW_inelasticFSI)
    fFSIRW_inelasticFSI = val;
  if (curenum == kFSIRW_pionProdFSI)
    fFSIRW_pionProdFSI = val;
  if (curenum == kFSIRW_pionAbsFSI)
    fFSIRW_pionAbsFSI = val;
}

bool FSIRW_Calc::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
    
  //std::cout << "Checking if pShellNorm dial is handled" << std::endl;
  //std::cout << "  curenum" << curenum << std::endl;
  //std::cout << "  rwenum" << rwenum << std::endl;
  //std::cout << "  rwenum % 1000" << rwenum % 1000 << std::endl;
    
  switch (curenum) {
  case kFSIRW_noFSI:
  case kFSIRW_elasticFSI:
  case kFSIRW_inelasticFSI:
  case kFSIRW_pionProdFSI:
  case kFSIRW_pionAbsFSI:
    return true;
  default:
    return false;
  }
}

bool FSIRW_Calc::IsNoFSI(std::vector<int> PDGvert, std::vector<TLorentzVector> pvert, std::vector<int> PDGfs, std::vector<TLorentzVector> pfs, double eps)
{
  // Check if we have the same particles at vert and fs
    
  //bool samePartVertFS = SameParticlesVertFS(PDGvert, PDGfs);
    
  //if (samePartVertFS == false)
  //    return false;
    
  // Now we know that we have the same particles in both vert and fs
  // Let's check how much the momentum changes
  int nvertp = PDGvert.size();
  std::vector<bool> visited(nvertp, false);
    
  for (int k = 0; k<nvertp; k++)
    {
      int l = 0;
      for (l = 0; l<nvertp; l++)
        {
	  if (PDGvert[k] == PDGfs[l] && visited[l] == false)
            {
	      TLorentzVector diff = pvert[k] - pfs[l];
	      double dist = diff.Vect().Mag();
                
	      if (dist < eps * pvert[k].Vect().Mag())
                {
		  visited[l] = true;
		  break;
                }
            }
        }
        
      if (l == nvertp)
	return false;
    }
  return true;
    
}

bool FSIRW_Calc::SameParticlesVertFS(std::vector<int> PDGvert, std::vector<int> PDGfs)
{
  // Check if we have the same particles at vert and fs
    
  int nvertp = PDGvert.size();
  if (PDGfs.size() != nvertp)
    return false;
    
  std::vector<int> PDGvert_sorted(PDGvert), PDGfs_sorted(PDGfs);
  std::sort(PDGvert_sorted.begin(), PDGvert_sorted.end());
  std::sort(PDGfs_sorted.begin(), PDGfs_sorted.end());
    
  for (int i = 0; i<nvertp; i++)
    {
      if (PDGvert_sorted[i] != PDGfs_sorted[i])
	return false;
    }
  return true;
}


int FSIRW_Calc::GetNpi(std::vector<int> PDG)
{
  int Npi = 0;
  for (int i = 0; i<PDG.size(); i++)
    {
      if (abs(PDG[i]) == 211 || abs(PDG[i]) == 111)
	Npi++;
    }
  return Npi;
}

// end FSI RW modif

// 2p2h normalization RW    

RW2p2h_Calc::RW2p2h_Calc() {
  fRW2p2h_norm = 1.0;
}

double RW2p2h_Calc::CalcWeight(BaseFitEvt *evt) {
  int mode = abs(evt->Mode);
  FitEvent *fevt = static_cast<FitEvent *>(evt);
  double w = 1.0;
    
  if (mode == 2) // 2p2h
    {
      w *= fRW2p2h_norm;
    }
    
  return w;
}

void RW2p2h_Calc::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void RW2p2h_Calc::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;
    
  // Check Handled
  if (!IsHandled(curenum))
    return;
  if (curenum == kRW2p2h_norm)
    fRW2p2h_norm = val;
}

bool RW2p2h_Calc::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
    
  //std::cout << "Checking if pShellNorm dial is handled" << std::endl;
  //std::cout << "  curenum" << curenum << std::endl;
  //std::cout << "  rwenum" << rwenum << std::endl;
  //std::cout << "  rwenum % 1000" << rwenum % 1000 << std::endl;
    
  switch (curenum) {
  case kRW2p2h_norm:
    return true;
  default:
    return false;
  }
}

// end 2p2h normalization RW






ModeNormCalc::ModeNormCalc() { fNormRES = 1.0; }

double ModeNormCalc::CalcWeight(BaseFitEvt *evt) {
  int mode = abs(evt->Mode);
  double w = 1.0;

  if (mode == 11 or mode == 12 or mode == 13) {
    w *= fNormRES;
  }

  return w;
}

void ModeNormCalc::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void ModeNormCalc::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;

  // Check Handled
  if (!IsHandled(curenum))
    return;
  if (curenum == kModeNorm_NormRES)
    fNormRES = val;
}

bool ModeNormCalc::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
  switch (curenum) {
  case kModeNorm_NormRES:
    return true;
  default:
    return false;
  }
}

BeRPACalc::BeRPACalc()
    : fBeRPA_A(0.59), fBeRPA_B(1.05), fBeRPA_D(1.13), fBeRPA_E(0.88),
      fBeRPA_U(1.2), nParams(0) {
  // A = 0.59 +/- 20%
  // B = 1.05 +/- 20%
  // D = 1.13 +/- 15%
  // E = 0.88 +/- 40%
  // U = 1.2
}

double BeRPACalc::CalcWeight(BaseFitEvt *evt) {
  FitEvent *fevt = static_cast<FitEvent *>(evt);
  int mode = abs(evt->Mode);
  double w = 1.0;
  if (nParams == 0) {
    return w;
  }

  // Get Q2
  // Get final state lepton
  if (mode == 1) {
    double Q2 =
        -1.0 * (fevt->GetHMFSAnyLeptons()->P4() - fevt->GetNeutrinoIn()->P4()) *
        (fevt->GetHMFSAnyLeptons()->P4() - fevt->GetNeutrinoIn()->P4()) / 1.E6;
    // Only CCQE events
    w *= calcRPA(Q2, fBeRPA_A, fBeRPA_B, fBeRPA_D, fBeRPA_E, fBeRPA_U);
  }

  return w;
}

void BeRPACalc::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void BeRPACalc::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;

  // Check Handled
  if (!IsHandled(curenum))
    return;
  // Need 4 or 5 reconfigures
  if (curenum == kBeRPA_A)
    fBeRPA_A = val;
  else if (curenum == kBeRPA_B)
    fBeRPA_B = val;
  else if (curenum == kBeRPA_D)
    fBeRPA_D = val;
  else if (curenum == kBeRPA_E)
    fBeRPA_E = val;
  else if (curenum == kBeRPA_U)
    fBeRPA_U = val;
  nParams++;
}

bool BeRPACalc::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
  switch (curenum) {
  case kBeRPA_A:
  case kBeRPA_B:
  case kBeRPA_D:
  case kBeRPA_E:
  case kBeRPA_U:
    return true;
  default:
    return false;
  }
}

SBLOscWeightCalc::SBLOscWeightCalc() {
  fDistance = 0.0;
  fMassSplitting = 0.0;
  fSin2Theta = 0.0;
}

double SBLOscWeightCalc::CalcWeight(BaseFitEvt *evt) {
  FitEvent *fevt = static_cast<FitEvent *>(evt);

  FitParticle *pnu = fevt->PartInfo(0);
  double E = pnu->fP.E() / 1.E3;

  // Extract energy
  return GetSBLOscWeight(E);
}

void SBLOscWeightCalc::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void SBLOscWeightCalc::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;
  if (!IsHandled(curenum))
    return;
  if (curenum == kSBLOsc_Distance)
    fDistance = val;
  if (curenum == kSBLOsc_MassSplitting)
    fMassSplitting = val;
  if (curenum == kSBLOsc_Sin2Theta)
    fSin2Theta = val;
}

bool SBLOscWeightCalc::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
  switch (curenum) {
  case kSBLOsc_Distance:
    return true;
  case kSBLOsc_MassSplitting:
    return true;
  case kSBLOsc_Sin2Theta:
    return true;
  default:
    return false;
  }
}

double SBLOscWeightCalc::GetSBLOscWeight(double E) {
  if (E <= 0.0)
    return 1.0 - 0.5 * fSin2Theta;
  return 1.0 - fSin2Theta * pow(sin(1.267 * fMassSplitting * fDistance / E), 2);
}

GaussianModeCorr::GaussianModeCorr() {

  // Apply the tilt-shift Gauss by Patrick
  // Alternatively set in config
  fMethod = true;

  // Init
  fApply_CCQE = false;
  fGausVal_CCQE[kPosNorm] = 0.0;
  fGausVal_CCQE[kPosTilt] = 0.0;
  fGausVal_CCQE[kPosPq0] = 1.0;
  fGausVal_CCQE[kPosWq0] = 1.0;
  fGausVal_CCQE[kPosPq3] = 1.0;
  fGausVal_CCQE[kPosWq3] = 1.0;

  fApply_2p2h = false;
  fGausVal_2p2h[kPosNorm] = 0.0;
  fGausVal_2p2h[kPosTilt] = 0.0;
  fGausVal_2p2h[kPosPq0] = 1.0;
  fGausVal_2p2h[kPosWq0] = 1.0;
  fGausVal_2p2h[kPosPq3] = 1.0;
  fGausVal_2p2h[kPosWq3] = 1.0;

  fApply_2p2h_PPandNN = false;
  fGausVal_2p2h_PPandNN[kPosNorm] = 0.0;
  fGausVal_2p2h_PPandNN[kPosTilt] = 0.0;
  fGausVal_2p2h_PPandNN[kPosPq0] = 1.0;
  fGausVal_2p2h_PPandNN[kPosWq0] = 1.0;
  fGausVal_2p2h_PPandNN[kPosPq3] = 1.0;
  fGausVal_2p2h_PPandNN[kPosWq3] = 1.0;

  fApply_2p2h_NP = false;
  fGausVal_2p2h_NP[kPosNorm] = 0.0;
  fGausVal_2p2h_NP[kPosTilt] = 0.0;
  fGausVal_2p2h_NP[kPosPq0] = 1.0;
  fGausVal_2p2h_NP[kPosWq0] = 1.0;
  fGausVal_2p2h_NP[kPosPq3] = 1.0;
  fGausVal_2p2h_NP[kPosWq3] = 1.0;

  fApply_CC1pi = false;
  fGausVal_CC1pi[kPosNorm] = 0.0;
  fGausVal_CC1pi[kPosTilt] = 0.0;
  fGausVal_CC1pi[kPosPq0] = 1.0;
  fGausVal_CC1pi[kPosWq0] = 1.0;
  fGausVal_CC1pi[kPosPq3] = 1.0;
  fGausVal_CC1pi[kPosWq3] = 1.0;

  fAllowSuppression = false;

  fDebugStatements = FitPar::Config().GetParB("GaussianModeCorr_DEBUG");
  // fDebugStatements = true;
}

double GaussianModeCorr::CalcWeight(BaseFitEvt *evt) {
  FitEvent *fevt = static_cast<FitEvent *>(evt);
  double rw_weight = 1.0;

  // Get Neutrino
  if (!fevt->Npart()) {
    NUIS_ABORT("NO particles found in stack!");
  }
  FitParticle *pnu = fevt->GetHMISAnyLeptons();
  if (!pnu) {
    NUIS_ABORT("NO Starting particle found in stack!");
  }
  int pdgnu = pnu->fPID;

  int expect_fsleppdg = 0;

  if (pdgnu & 1) {
    expect_fsleppdg = pdgnu;
  } else {
    expect_fsleppdg = abs(pdgnu) - 1;
  }

  FitParticle *plep = fevt->GetHMFSParticle(expect_fsleppdg);
  if (!plep)
    return 1.0;

  TLorentzVector q = pnu->fP - plep->fP;

  // Extra q0,q3
  double q0 = fabs(q.E()) / 1.E3;
  double q3 = fabs(q.Vect().Mag()) / 1.E3;

  int initialstate = -1; // Undef
  if (abs(fevt->Mode) == 2) {
    int npr = 0;
    int nne = 0;

    for (UInt_t j = 0; j < fevt->Npart(); j++) {
      if ((fevt->PartInfo(j))->fIsAlive)
        continue;

      if (fevt->PartInfo(j)->fPID == 2212)
        npr++;
      else if (fevt->PartInfo(j)->fPID == 2112)
        nne++;
    }

    if (fevt->Mode == 2 && npr == 1 && nne == 1) {
      initialstate = 2;

    } else if (fevt->Mode == 2 &&
               ((npr == 0 && nne == 2) || (npr == 2 && nne == 0))) {
      initialstate = 1;
    }
  }

  // Apply weighting
  if (fApply_CCQE && abs(fevt->Mode) == 1) {
    if (fDebugStatements)
      std::cout << "Getting CCQE Weight" << std::endl;
    double g = GetGausWeight(q0, q3, fGausVal_CCQE);
    if (g < 1.0)
      g = 1.0;
    rw_weight *= g;
  }

  if (fApply_2p2h && abs(fevt->Mode) == 2) {
    if (fDebugStatements)
      std::cout << "Getting 2p2h Weight" << std::endl;
    if (fDebugStatements)
      std::cout << "Got q0 q3 = " << q0 << " " << q3 << " mode = " << fevt->Mode
                << std::endl;
    rw_weight *= GetGausWeight(q0, q3, fGausVal_2p2h);
    if (fDebugStatements)
      std::cout << "Returning Weight " << rw_weight << std::endl;
  }

  if (fApply_2p2h_PPandNN && abs(fevt->Mode) == 2 && initialstate == 1) {
    if (fDebugStatements)
      std::cout << "Getting 2p2h PPandNN Weight" << std::endl;
    rw_weight *= GetGausWeight(q0, q3, fGausVal_2p2h_PPandNN);
  }

  if (fApply_2p2h_NP && abs(fevt->Mode) == 2 && initialstate == 2) {
    if (fDebugStatements)
      std::cout << "Getting 2p2h NP Weight" << std::endl;
    rw_weight *= GetGausWeight(q0, q3, fGausVal_2p2h_NP);
  }

  if (fApply_CC1pi && abs(fevt->Mode) >= 11 && abs(fevt->Mode) <= 13) {
    if (fDebugStatements)
      std::cout << "Getting CC1pi Weight" << std::endl;
    rw_weight *= GetGausWeight(q0, q3, fGausVal_CC1pi);
  }

  return rw_weight;
}

void GaussianModeCorr::SetMethod(bool method) {
  fMethod = method;
  if (fMethod == true) {
    NUIS_LOG(FIT,
         " Using tilt-shift Gaussian parameters for Gaussian enhancement...");
  } else {
    NUIS_LOG(FIT, " Using Normal Gaussian parameters for Gaussian enhancement...");
  }
};

double GaussianModeCorr::GetGausWeight(double q0, double q3, double vals[]) {
  // The weight
  double w = 1.0;

  // Use tilt-shift method by Patrick
  if (fMethod) {
    if (fDebugStatements) {
      std::cout << "Using Patrick gaussian" << std::endl;
    }
    // // CCQE Without Suppression
    // double Norm = 4.82788679036;
    // double Tilt = 2.3501416116;
    // double Pq0  = 0.363964889702;
    // double Wq0  = 0.133976806938;
    // double Pq3  = 0.431769740224;
    // double Wq3  = 0.207666663434;

    // // Also add for CCQE at the end
    // return (w > 1.0) ? w : 1.0;

    // // 2p2h with suppression
    // double Norm = 15.967;
    // double Tilt = -0.455655;
    // double Pq0  = 0.214598;
    // double Wq0  = 0.0291061;
    // double Pq3  = 0.480194;
    // double Wq3  = 0.134588;

    double Norm = vals[kPosNorm];
    double Tilt = vals[kPosTilt];
    double Pq0 = vals[kPosPq0];
    double Wq0 = vals[kPosWq0];
    double Pq3 = vals[kPosPq3];
    double Wq3 = vals[kPosWq3];

    double a = cos(Tilt) * cos(Tilt) / (2 * Wq0 * Wq0);
    a += sin(Tilt) * sin(Tilt) / (2 * Wq3 * Wq3);

    double b = -sin(2 * Tilt) / (4 * Wq0 * Wq0);
    b += sin(2 * Tilt) / (4 * Wq3 * Wq3);

    double c = sin(Tilt) * sin(Tilt) / (2 * Wq0 * Wq0);
    c += cos(Tilt) * cos(Tilt) / (2 * Wq3 * Wq3);

    w = Norm;
    w *= exp(-a * (q0 - Pq0) * (q0 - Pq0));
    w *= exp(+2.0 * b * (q0 - Pq0) * (q3 - Pq3));
    w *= exp(-c * (q3 - Pq3) * (q3 - Pq3));

    if (fDebugStatements) {
      std::cout << "Applied Tilt " << Tilt << " " << cos(Tilt) << " "
                << sin(Tilt) << std::endl;
      std::cout << "abc = " << a << " " << b << " " << c << std::endl;
      std::cout << "Returning " << Norm << " " << Pq0 << " " << Wq0 << " "
                << Pq3 << " " << Wq3 << " " << w << std::endl;
    }

    if (w != w || std::isnan(w) || w < 0.0) {
      w = 0.0;
    }

    if (w < 1.0 and !fAllowSuppression) {
      w = 1.0;
    }

    // Use the MINERvA Gaussian method
  } else {
    /*
     * From MINERvA and Daniel Ruterbories:
     * Old notes here: *
     * http://cdcvs.fnal.gov/cgi-bin/public-cvs/cvsweb-public.cgi/AnalysisFramework/Ana/CCQENu/ana_common/data/?cvsroot=mnvsoft
     * These parameters are slightly altered
     *
     * FRESH:
     * 10.5798
     * 0.254032
     * 0.50834
     * 0.0571035
     * 0.129051
     * 0.875287
     */
    if (fDebugStatements) {
      std::cout << "Using MINERvA Gaussian" << std::endl;
    }

    double norm = vals[kPosNorm];
    double meanq0 = vals[kPosTilt];
    double meanq3 = vals[kPosPq0];
    double sigmaq0 = vals[kPosWq0];
    double sigmaq3 = vals[kPosPq3];
    double corr = vals[kPosWq3];

    double z = (q0 - meanq0) * (q0 - meanq0) / sigmaq0 / sigmaq0 +
               (q3 - meanq3) * (q3 - meanq3) / sigmaq3 / sigmaq3 -
               2 * corr * (q0 - meanq0) * (q3 - meanq3) / (sigmaq0 * sigmaq3);

    double ret = norm * exp(-0.5 * z / (1 - corr * corr));
    // Need to add 1 to the results
    w = 1.0 + ret;
  }

  return w;
}

void GaussianModeCorr::SetDialValue(std::string name, double val) {
  SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void GaussianModeCorr::SetDialValue(int rwenum, double val) {
  int curenum = rwenum % 1000;

  // Check Handled
  if (!IsHandled(curenum))
    return;

  // CCQE Setting
  for (int i = kGaussianCorr_CCQE_norm; i <= kGaussianCorr_CCQE_Wq3; i++) {
    if (i == curenum) {
      int index = i - kGaussianCorr_CCQE_norm;
      fGausVal_CCQE[index] = val;
      fApply_CCQE = true;
    }
  }

  // 2p2h Setting
  for (int i = kGaussianCorr_2p2h_norm; i <= kGaussianCorr_2p2h_Wq3; i++) {
    if (i == curenum) {
      int index = i - kGaussianCorr_2p2h_norm;
      fGausVal_2p2h[index] = val;
      fApply_2p2h = true;
    }
  }

  // 2p2h_PPandNN Setting
  for (int i = kGaussianCorr_2p2h_PPandNN_norm;
       i <= kGaussianCorr_2p2h_PPandNN_Wq3; i++) {
    if (i == curenum) {
      int index = i - kGaussianCorr_2p2h_PPandNN_norm;
      fGausVal_2p2h_PPandNN[index] = val;
      fApply_2p2h_PPandNN = true;
    }
  }

  // 2p2h_NP Setting
  for (int i = kGaussianCorr_2p2h_NP_norm; i <= kGaussianCorr_2p2h_NP_Wq3;
       i++) {
    if (i == curenum) {
      int index = i - kGaussianCorr_2p2h_NP_norm;
      fGausVal_2p2h_NP[index] = val;
      fApply_2p2h_NP = true;
    }
  }

  // CC1pi Setting
  for (int i = kGaussianCorr_CC1pi_norm; i <= kGaussianCorr_CC1pi_Wq3; i++) {
    if (i == curenum) {
      int index = i - kGaussianCorr_CC1pi_norm;
      fGausVal_CC1pi[index] = val;
      fApply_CC1pi = true;
    }
  }

  if (curenum == kGaussianCorr_AllowSuppression) {
    fAllowSuppression = (val > 0.5);
  }
}

bool GaussianModeCorr::IsHandled(int rwenum) {
  int curenum = rwenum % 1000;
  switch (curenum) {
  case kGaussianCorr_CCQE_norm:
  case kGaussianCorr_CCQE_tilt:
  case kGaussianCorr_CCQE_Pq0:
  case kGaussianCorr_CCQE_Wq0:
  case kGaussianCorr_CCQE_Pq3:
  case kGaussianCorr_CCQE_Wq3:

  case kGaussianCorr_2p2h_norm:
  case kGaussianCorr_2p2h_tilt:
  case kGaussianCorr_2p2h_Pq0:
  case kGaussianCorr_2p2h_Wq0:
  case kGaussianCorr_2p2h_Pq3:
  case kGaussianCorr_2p2h_Wq3:

  case kGaussianCorr_2p2h_PPandNN_norm:
  case kGaussianCorr_2p2h_PPandNN_tilt:
  case kGaussianCorr_2p2h_PPandNN_Pq0:
  case kGaussianCorr_2p2h_PPandNN_Wq0:
  case kGaussianCorr_2p2h_PPandNN_Pq3:
  case kGaussianCorr_2p2h_PPandNN_Wq3:

  case kGaussianCorr_2p2h_NP_norm:
  case kGaussianCorr_2p2h_NP_tilt:
  case kGaussianCorr_2p2h_NP_Pq0:
  case kGaussianCorr_2p2h_NP_Wq0:
  case kGaussianCorr_2p2h_NP_Pq3:
  case kGaussianCorr_2p2h_NP_Wq3:

  case kGaussianCorr_CC1pi_norm:
  case kGaussianCorr_CC1pi_tilt:
  case kGaussianCorr_CC1pi_Pq0:
  case kGaussianCorr_CC1pi_Wq0:
  case kGaussianCorr_CC1pi_Pq3:
  case kGaussianCorr_CC1pi_Wq3:
  case kGaussianCorr_AllowSuppression:
    return true;
  default:
    return false;
  }
}
