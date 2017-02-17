#include "FitWeight.h"

void FitWeight::AddRWEngine(int type) {

	switch (type) {
	case kNEUT:  
		fAllRW[type] = new NEUTWeightEngine("neutrw");
		break;

	case kNUWRO: 
		fAllRW[type] = new NuWroWeightEngine("nuwrorw");
		break;

	case kGENIE: 
		fAllRW[type] = new GENIEWeightEngine("genierw");
		break;

	case kNORM:  
		fAllRW[type] = new SampleNormEngine("normrw");
		break;
	}

}

void FitWeight::IncludeDial(std::string name, std::string type, double val) {
	int nuisenum = FitBase::GetDialEnum(type, name);
	IncludeDial(nuisenum, val);

	// Sort Maps
	fAllEnums[name]  = nuisenum;
	fAllValues[nuisenum] = val;

	// Sort Lists
	fNameList.push_back(name);
	fEnumList.push_back(nuisenum);
	fValueList.push_back(val);
}

void FitWeight::IncludeDial(std::string name, int type, double val) {
	int nuisenum = FitBase::GetDialEnum(type, name);
	IncludeDial(nuisenum, val);

	// Sort Maps
	fAllEnums[name]   = nuisenum;
	fAllValues[nuisenum] = val;

	// Sort Lists
	fNameList.push_back(name);
	fEnumList.push_back(nuisenum);
	fValueList.push_back(val);
}

void FitWeight::IncludeDial(int rwenum, double val) {

	// Get the dial type.
	int dialtype = int(rwenum - (rwenum % 1000)) / 1000;
	std::cout << "DialType = " << dialtype << std::endl;

	if (fAllRW.find(dialtype) == fAllRW.end()) {
		AddRWEngine(dialtype);
	}



	// Pointer to relevant engine
	WeightEngineBase* rw = fAllRW[dialtype];

	std::cout << "Adding rw dial " << rw << std::endl;

	// Include the dial
	rw->IncludeDial(rwenum, val);

	// Set Dial Value
	if (val != -9999.9) rw->SetDialValue(rwenum, val);
}

void FitWeight::Reconfigure(bool silent) {
	// Reconfigure all added RW engines
	for (std::map<int, WeightEngineBase*>::iterator iter = fAllRW.begin();
	        iter != fAllRW.end(); iter++) {
		(*iter).second->Reconfigure(silent);
	}
}

void FitWeight::SetDialValue(std::string name, double val) {
	int nuisenum = fAllEnums[name];
	SetDialValue(nuisenum, val);
}

void FitWeight::SetDialValue(int nuisenum, double val) {
	// Conv dial type
	int dialtype = int(nuisenum - (nuisenum % 1000)) / 1000;

	// Check dial type available
	if (fAllRW.find(dialtype) == fAllRW.end()) {
		AddRWEngine(dialtype);
	}

	// Get RW Engine for this dial
	fAllRW[dialtype]->SetDialValue(nuisenum, val);
	fAllValues[nuisenum] = val;

	// Update ValueList
	for (size_t i = 0; i < fEnumList.size(); i++) {
		if (fEnumList[i] == nuisenum) {
			fValueList[i] = val;
		}
	}

}

double FitWeight::GetDialValue(std::string name) {
	int nuisenum = fAllEnums[name];
	return GetDialValue(nuisenum);
}

double FitWeight::GetDialValue(int nuisenum) {
	return fAllValues[nuisenum];
}

bool FitWeight::DialIncluded(std::string name) {
	return (fAllEnums.find(name) != fAllEnums.end());
}

bool FitWeight::DialIncluded(int rwenum) {
	return (fAllValues.find(rwenum) != fAllValues.end());
}


double FitWeight::CalcWeight(BaseFitEvt* evt) {
	double rwweight = 1.0;
	for (std::map<int, WeightEngineBase*>::iterator iter = fAllRW.begin();
	        iter != fAllRW.end(); iter++) {
		rwweight *= (*iter).second->CalcWeight(evt);
	}
	return rwweight;
}


void FitWeight::UpdateWeightEngine(const double* x) {
	size_t count = 0;
	for (std::vector<int>::iterator iter = fEnumList.begin();
	        iter != fEnumList.end(); iter++) {
		SetDialValue( (*iter), x[count] );
		count++;
	}
}

void FitWeight::GetAllDials(double* x, int n) {
	for (int i = 0; i < n; i++) {
		x[i] = GetDialValue( fEnumList[i] );
	}
}

bool FitWeight::NeedsEventReWeight(const double* x) {
	bool haschange = false;
	size_t count = 0;

	// Compare old to new and decide if RW needed.
	for (std::vector<int>::iterator iter = fEnumList.begin();
	        iter != fEnumList.end(); iter++) {

		int nuisenum = (*iter);
		int type = (nuisenum / 1000) - (nuisenum % 1000);

		// Compare old to new
		double oldval = GetDialValue(nuisenum);
		double newval = x[count];
		if (oldval != newval){
			if (fAllRW[type]->NeedsEventReWeight()){
				haschange = true;
			}
		}

		count++;
	}

	return haschange;
}

double FitWeight::GetSampleNorm(std::string name) {
	if (name.empty()) return 1.0;

	// Find norm dial
	if (fAllEnums.find(name) != fAllEnums.end()) {
		return fAllValues[ fAllEnums[name] ];
	} else {
		return 1.0;
	}
}


