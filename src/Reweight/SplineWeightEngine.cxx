#include "SplineWeightEngine.h"

SplineWeightEngine::SplineWeightEngine(std::string name) {

  // Setup the Reweight engien
  fCalcName = name;
  LOG(FIT) << "Setting up Spline RW : " << fCalcName << endl;
  
  // Set Abs Twk Config
  fIsAbsTwk = true;
	
};


void SplineWeightEngine::IncludeDial(std::string name, double startval) {

  // Get NUISANCE Enum
  int nuisenum = Reweight::ConvDial(name, kSPLINEPARAMETER);
  
  // Fill Maps
  int index = fValues.size();
  fValues.push_back(0.0);
  
  fEnumIndex[nuisenum] = index;
  fNameIndex[name] = index;
  
  //	std::cout << "Inlcuded Spline Dial " << name << " " << nuisenum << " " << startval << " " << index << std::endl;
  
  // Set Value if given
  if (startval != -999.9) {
    SetDialValue(name, startval);
  }
}


void SplineWeightEngine::SetDialValue(int nuisenum, double val) {
  //  LOG(FIT) << "Enum Val " << nuisenum << std::endl;
  //  LOG(FIT) << fEnumIndex.size() << std::endl;
  //  LOG(FIT) << "Set Dial Value to " << nuisenum << " " << fEnumIndex[nuisenum] << " " << val << std::endl;
  fValues[fEnumIndex[nuisenum]] = val;
}

void SplineWeightEngine::SetDialValue(std::string name, double val){
  fValues[fNameIndex[name]] = val;
}


void SplineWeightEngine::Reconfigure(bool silent) {
  for (std::map<std::string, size_t>::iterator iter = fNameIndex.begin(); 
       iter != fNameIndex.end(); iter++){
    // LOG(FIT) << "Reconfiguring Spline " << iter->first << " to be " << fValues[ iter->second ] << " Inside SPL RW" << std::endl;
    fSplineValueMap[ iter->first ] = fValues[ iter->second ];
  }
}


double SplineWeightEngine::CalcWeight(BaseFitEvt* evt) {

  if (!evt->fSplineRead) return 1.0;
  
  if (evt->fSplineRead->NeedsReconfigure()) {
    evt->fSplineRead->Reconfigure(fSplineValueMap);
  }
  
  double rw_weight = evt->fSplineRead->CalcWeight( evt->fSplineCoeff );
  if (rw_weight < 0.0) rw_weight = 0.0;

  return rw_weight;
}










