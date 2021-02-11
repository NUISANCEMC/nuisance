#include "SampleSettings.h"

SampleSettings::SampleSettings() {
};

SampleSettings::SampleSettings(nuiskey key) {
  fKeyValues = key;
  if (!key.Has("type")) key.Set("type", "DEFAULT");
}

std::string SampleSettings::GetName() {
  return GetS("name");
}

void SampleSettings::SetS(std::string name, std::string val) {
  fKeyValues.SetS(name, val);
};

bool SampleSettings::Found(std::string name, std::string substring) {
  std::string compstring = fKeyValues.GetS(name);
  return compstring.find(substring) != std::string::npos;
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

double SampleSettings::GetNormError() {
  return GetD("norm_error");
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

void SampleSettings::Set(std::string name, double d) {
  SetDefault(name, d);
}

void SampleSettings::Set(std::string name, int i) {
  SetDefault(name, i);
}

void SampleSettings::Set(std::string name, std::string s) {
  SetDefault(name, s);
}



void SampleSettings::DefineAllowedTargets(std::string targ) {
  // fAllowedTargets = TargetUtils::ParseTargetsToIntVect(targ);
};


void SampleSettings::FoundFill(std::string name, std::string substring, bool& cont, bool def) {
  std::string compstring = fKeyValues.GetS(name);
  if (compstring.find(substring) != std::string::npos) {
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

void SampleSettings::SetShapeCovarInput(std::string val) {
  SetDefault("shapecovar", val);
};


void SampleSettings::SetDefault(std::string name, std::string val) {
  if (!fKeyValues.Has(name)) fKeyValues.Set(name, val);
};

void SampleSettings::SetDefault(std::string name, double val ) {
  if (!fKeyValues.Has(name)) fKeyValues.Set(name, val);
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

std::string SampleSettings::GetXTitle() {
  return GetS("xtitle");
}

std::string SampleSettings::GetYTitle() {
  return GetS("ytitle");
}

std::string SampleSettings::GetZTitle() {
  return GetS("ztitle");
}

std::string SampleSettings::PlotTitles() {
  std::string returnstring = ";";
  if (!(GetS("xtitle").empty())) returnstring += GetS("xtitle")+";";
  if (!(GetS("ytitle").empty())) returnstring += GetS("ytitle")+";";
  if (!(GetS("ztitle").empty())) returnstring += GetS("ztitle")+";";
  // Cut out the last ; symbol
  returnstring = returnstring.substr(0, returnstring.find_last_of(";"));
  return returnstring;
};

std::string SampleSettings::GetFullTitles() {
  return Title() + PlotTitles();
}

int SampleSettings::GetI(std::string name) {
  return fKeyValues.GetI(name);
}

bool SampleSettings::GetB(std::string name){
  return fKeyValues.GetB(name);
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

void SampleSettings::Write(std::string name) {

  if (name.empty()) name = this->GetS("name") + "_settings";
  // Make a new canvas
  TCanvas* c1 = new TCanvas( name.c_str(), name.c_str(), 800, 600 );
  c1->cd();

  TPaveText *pttitle = new TPaveText(0.01, 0.90, 0.99, 0.99, "NDC");
  pttitle->AddText( name.c_str() );
  pttitle->Draw();

  TPaveText *pt = new TPaveText(0.01, 0.01, 0.99, 0.89, "NDC");
  pt->Draw("same");

  std::vector<std::string> keys = fKeyValues.GetAllKeys();
  for (size_t i = 0; i < keys.size(); i++) {
    std::string keyval = fKeyValues.GetS(keys[i]);
    std::vector<std::string> lines = GeneralUtils::ParseToStr(keyval, "\n");
    if (lines.size() == 1) {
      pt->AddText( ("#bullet #bf{" + keys[i] + "} : " + fKeyValues.GetS(keys[i])).c_str() );
    } else {
      pt->AddText( ("#bullet #bf{" + keys[i] + "} : ").c_str() );
      for (size_t j = 0; j < lines.size(); j++) {
        pt->AddText(("   |-->  " + lines[j]).c_str() );
      }
    }
  }

  pt->SetTextAlign(11);
  //pt->SetTextSize(14);

  c1->Modified();
  c1->Write();
  delete c1;
  delete pt;
  delete pttitle;
}

void SampleSettings::SetOnlyMC(bool state) {
  SetDefault("onlymc", state);
}
