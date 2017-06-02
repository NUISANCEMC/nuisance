#include "SampleSettings.h"

SampleSettings::SampleSettings() {
};

SampleSettings::SampleSettings(nuiskey key) {
	fKeyValues = key;
	if (!key.Has("type")) key.AddS("type", "DEFAULT");
}

std::string SampleSettings::GetName() {
	return GetS("name");
}

void SampleSettings::SetS(std::string name, std::string val) {
	fKeyValues.SetS(name, val);
};

bool SampleSettings::Found(std::string name, std::string substr) {
	std::string compstring = fKeyValues.GetS(name);
	return compstring.find(substr) != std::string::npos;
}


void SampleSettings::SetXTitle(std::string name) {
	SetDefault("xtitle", name);
};

void SampleSettings::SetYTitle(std::string name) {
	SetDefault("ytitle", name);
};


void SampleSettings::SetZTitle(std::string name) {
	SetDefault("ztitle", name);
};

void SampleSettings::SetNormError(double norm) {
	SetDefault("norm_error", GeneralUtils::DblToStr(norm));
};

std::string SampleSettings::GetCovarInput() {
	return GetS("covar");
}

void SampleSettings::SetAllowedTypes(std::string allowed, std::string defaulttype) {
	SetDefault("default_types", allowed);
	SetDefault("allowed_types", defaulttype);
};

void SampleSettings::SetEnuRangeFromFlux(TH1D* fluxhist) {
	double enu_min = fluxhist->GetXaxis()->GetXmin();
	double enu_max = fluxhist->GetXaxis()->GetXmax();
	SetEnuRange(enu_min, enu_max);
};

void SampleSettings::SetEnuRange(double min, double max) {
	SetDefault("enu_min", min);
	SetDefault("enu_max", max);
};

void SampleSettings::DefineAllowedTargets(std::string targ) {
	// fAllowedTargets = TargetUtils::ParseTargetsToIntVect(targ);
};


void SampleSettings::FoundFill(std::string name, std::string substr, bool& cont, bool def) {
	std::string compstring = fKeyValues.GetS(name);
	if (compstring.find(substr) != std::string::npos) {
		cont = def;
	} else {
		cont = !def;
	}
};


void SampleSettings::SetTitle(std::string val) {
	SetDefault("title", val);
};

void SampleSettings::SetDataInput(std::string val) {
	SetDefault("data", val);
};

std::string SampleSettings::GetMapInput() {
	return GetS("map");
}
void SampleSettings::SetMapInput(std::string val) {
	SetDefault("map", val);
}

void SampleSettings::SetErrorInput(std::string val) {
	SetDefault("error", val);
};



void SampleSettings::SetCovarInput(std::string val) {
	SetDefault("covar", val);
};

void SampleSettings::SetDefault(std::string name, std::string val) {
	if (!fKeyValues.Has(name)) fKeyValues.AddS(name, val);
};

void SampleSettings::SetDefault(std::string name, double val ) {
	if (!fKeyValues.Has(name)) fKeyValues.AddD(name, val);
};

void SampleSettings::SetHasExtraHistograms(bool opt) {
	fHasExtraHistograms = opt;
};

void SampleSettings::DefineAllowedSpecies(std::string species) {
	fAllowedTargets = BeamUtils::ParseSpeciesToIntVect(species);
};

std::string SampleSettings::Title() {
	return GetS("title");
}

std::string SampleSettings::GetDataInput() {
	return GetS("data");
};


std::string SampleSettings::GetErrorInput() {
	return GetS("error");
};

std::string SampleSettings::PlotTitles() {
	return ";" + GetS("xtitle") + ";" + GetS("ytitle");
};

std::string SampleSettings::GetFullTitles() {
	return Title() + PlotTitles();
}

int SampleSettings::GetI(std::string name) {
	return fKeyValues.GetI(name);
}

double SampleSettings::GetD(std::string name) {
  return fKeyValues.GetD(name);
}

std::string SampleSettings::GetS(std::string name) {
	return fKeyValues.GetS(name);
}

void SampleSettings::SetSuggestedFlux(std::string str) {
	SetS("suggested_flux", str);
}

void SampleSettings::SetDescription(std::string str) {
	SetS("description", str);
}

