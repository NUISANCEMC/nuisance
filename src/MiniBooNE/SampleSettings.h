#ifndef SAMPLESETTINGS_H
#define SAMPLESETTINGS_H

class SampleSettings {
	SampleSettings(nuiskey key){fKeyValues = key};

	void SetS(std::string name, std::string val){};
	void SetXTitle(std::string name){};
	void SetYTitle(std::string name){};
	void SetAllowedTypes(std::string allowed, std::string default="FIX"){};
	void SetEnuRangeFromFlux(TH1D* fluxhist){};
	void SetEnuRange(double min, double max){};

	void DefineAllowedTargets(std::string targ){};

	void FoundFill(std::string name, std::string substr, bool& cont, bool def){};
	void SetTitle(std::string val){};
	void SetDataInput(std::string val){};
	void SetDefault(std::string name, std::string val){};
	void SetHasExtraHistograms(bool opt = true){};
	void DefineAllowedSpecies(std::string species){};

	std::string GetDataInput(){return "";};
	std::string PlotTitles(){return "";};
	std::string GetS(std::string name){return "";};


	std::vector<int> fAllowedTargets;
	std::vector<int> fAllowedSpecies;
  	nuiskey fKeyValues;
}

#endif