#ifndef SAMPLESETTINGS_H
#define SAMPLESETTINGS_H
#include "NuisConfig.h"
#include "NuisKey.h"
#include "TH1D.h"
#include "BeamUtils.h"
#include "TPaveText.h"
#include "TCanvas.h"

class SampleSettings {
public:
	SampleSettings();
	SampleSettings(nuiskey key);

	inline std::string Name(){return GetName();};
	std::string GetName();
	void SetS(std::string name, std::string val);
	void SetXTitle(std::string name);
	void SetYTitle(std::string name);
	void SetZTitle(std::string name);
	std::string GetXTitle();
        std::string GetYTitle();
        std::string GetZTitle();
	void SetNormError(double norm);
	double GetNormError();

	void SetAllowedTypes(std::string allowed, std::string defaulttype="FIX");
	void SetEnuRangeFromFlux(TH1D* fluxhist);
	void SetEnuRange(double min, double max);
	std::string Title();
	void DefineAllowedTargets(std::string targ);

	void FoundFill(std::string name, std::string substring, bool& cont, bool def);
	// void FoundFill(std::string name, std::string substr, double& val, double )
	bool Found(std::string name, std::string substring);
	void SetTitle(std::string val);
	void SetDataInput(std::string val);
	void SetErrorInput(std::string val);
	std::string GetErrorInput();

	std::string GetMapInput();
	void SetMapInput(std::string val);
	void SetCovarInput(std::string val);
	void SetShapeCovarInput(std::string val);

	void SetDefault(std::string name, std::string val);
	void SetDefault(std::string name, double val);
	void SetHasExtraHistograms(bool opt = true);
	void DefineAllowedSpecies(std::string species);
	void SetSuggestedFlux(std::string str);
	void SetDescription(std::string str);

	void Write(std::string name="");

	std::string GetFullTitles();
	
	bool Has(std::string name){return fKeyValues.Has(name);};

	std::string GetDataInput();
	std::string PlotTitles();
	std::string GetS(std::string name);
	int GetI(std::string name);
	double GetD(std::string name);
	std::string GetCovarInput();
	void SetOnlyMC(bool state=true);
	bool GetB(std::string name);

	void Set(std::string name, int i);
	void Set(std::string name, std::string s);
	void Set(std::string name, double d);

	std::vector<int> fAllowedTargets;
	std::vector<int> fAllowedSpecies;
  	nuiskey fKeyValues;
  	bool fHasExtraHistograms;
};

#endif
