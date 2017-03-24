#ifndef SAMPLESETTINGS_H
#define SAMPLESETTINGS_H
#include "NuisConfig.h"
#include "NuisKey.h"
#include "TH1D.h"
#include "BeamUtils.h"
class SampleSettings {
public:
	SampleSettings();
	SampleSettings(nuiskey key);

	inline std::string Name(){return GetName();};
	std::string GetName();
	void SetS(std::string name, std::string val);
	void SetXTitle(std::string name);
	void SetYTitle(std::string name);
	void SetAllowedTypes(std::string allowed, std::string defaulttype="FIX");
	void SetEnuRangeFromFlux(TH1D* fluxhist);
	void SetEnuRange(double min, double max);
	std::string Title();
	void DefineAllowedTargets(std::string targ);

	void FoundFill(std::string name, std::string substr, bool& cont, bool def);
	// void FoundFill(std::string name, std::string substr, double& val, double )
	bool Found(std::string name, std::string substr);
	void SetTitle(std::string val);
	void SetDataInput(std::string val);
	void SetCovarInput(std::string val);

	void SetDefault(std::string name, std::string val);
	void SetDefault(std::string name, double val);
	void SetHasExtraHistograms(bool opt = true);
	void DefineAllowedSpecies(std::string species);
	void SetSuggestedFlux(std::string str);
	void SetDescription(std::string str);
	
	bool Has(std::string name){return fKeyValues.Has(name);};

	std::string GetDataInput();
	std::string PlotTitles();
	std::string GetS(std::string name);
	std::string GetCovarInput();

	std::vector<int> fAllowedTargets;
	std::vector<int> fAllowedSpecies;
  	nuiskey fKeyValues;
  	bool fHasExtraHistograms;
};

#endif