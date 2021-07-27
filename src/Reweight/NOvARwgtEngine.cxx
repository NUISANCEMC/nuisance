#include "NOvARwgtEngine.h"

#include "NOvARwgt/interfaces/GenieInterface.h"

#include "NOvARwgt/rwgt/tunes/Tunes2017.h"
#include "NOvARwgt/rwgt/tunes/Tunes2018.h"
#include "NOvARwgt/rwgt/tunes/Tunes2020.h"
#include "NOvARwgt/rwgt/tunes/TunesSA.h"

#include "NOvARwgt/rwgt/EventRecord.h"
#include "NOvARwgt/rwgt/ISystKnob.h"
#include "NOvARwgt/rwgt/Tune.h"

// GENIEv3
#include "Framework/Utils/AppInit.h"
#include "Framework/Utils/RunOpt.h"

static size_t const kCVTune2017 = 100;
static size_t const kCVTune2018 = 200;
static size_t const kCVTune2020 = 300;
static size_t const kCVTuneSA = 400;
static size_t const kNoSuchKnob = std::numeric_limits<size_t>::max();

novarwgt::Tune const *TuneFactory(size_t e) {
  switch (e) {
  case kCVTune2017: {
    return &novarwgt::kCVTune2017;
  }
  case kCVTune2018: {
    return &novarwgt::kCVTune2018;
  }
  case kCVTune2020: {
    return &novarwgt::kCVTune2020;
  }
  case kCVTuneSA: {
    return &novarwgt::kCVTuneSA;
  }
  default: {
    return NULL;
  }
  }
}

void NOvARwgtEngine::InitializeKnobs() {

  fTunes.push_back(TuneFactory(kCVTune2017));
  fTuneEnums[kCVTune2017] = 0;
  fTunes.push_back(TuneFactory(kCVTune2018));
  fTuneEnums[kCVTune2018] = 1;
  fTunes.push_back(TuneFactory(kCVTune2020));
  fTuneEnums[kCVTune2020] = 2;
  fTunes.push_back(TuneFactory(kCVTuneSA));
  fTuneEnums[kCVTuneSA] = 3;
  fTuneInUse = {false, false, false, false};
  fTuneValues = {0, 0, 0, 0};

  size_t ctr = 0;

  NUIS_LOG(FIT, "NOvARwgt kCVTune2017 sub-knobs:");
  size_t tune_ctr = 0;
  for (auto &k : novarwgt::kCVTune2017.SystKnobs()) {
    NUIS_LOG(FIT, "\t" << k.first);
    fKnobs.push_back(k.second);
    fKnobTunes.push_back(&novarwgt::kCVTune2017);
    fKnobTuneidx.push_back(0);
    fKnobEnums[kCVTune2017 + 1 + tune_ctr++] = ctr++;
    fKnobInUse.push_back(false);
    fKnobValues.push_back(0);
  }

  tune_ctr = 0;
  NUIS_LOG(FIT, "NOvARwgt kCVTune2018 sub-knobs:");
  for (auto &k : novarwgt::kCVTune2018.SystKnobs()) {
    NUIS_LOG(FIT, "\t" << k.first);
    fKnobs.push_back(k.second);
    fKnobTunes.push_back(&novarwgt::kCVTune2018);
    fKnobTuneidx.push_back(1);
    fKnobEnums[kCVTune2018 + 1 + tune_ctr++] = ctr++;
    fKnobInUse.push_back(false);
    fKnobValues.push_back(0);
  }

  tune_ctr = 0;
  NUIS_LOG(FIT, "NOvARwgt kCVTune2020 sub-knobs:");
  for (auto &k : novarwgt::kCVTune2020.SystKnobs()) {
    NUIS_LOG(FIT, "\t" << k.first);
    fKnobs.push_back(k.second);
    fKnobTunes.push_back(&novarwgt::kCVTune2020);
    fKnobTuneidx.push_back(2);
    fKnobEnums[kCVTune2020 + 1 + tune_ctr++] = ctr++;
    fKnobInUse.push_back(false);
    fKnobValues.push_back(0);
  }

  tune_ctr = 0;
  NUIS_LOG(FIT, "NOvARwgt kCVTuneSA sub-knobs:");
  for (auto &k : novarwgt::kCVTuneSA.SystKnobs()) {
    NUIS_LOG(FIT, "\t" << k.first);
    fKnobs.push_back(k.second);
    fKnobTunes.push_back(&novarwgt::kCVTuneSA);
    fKnobTuneidx.push_back(3);
    fKnobEnums[kCVTuneSA + 1 + tune_ctr++] = ctr++;
    fKnobInUse.push_back(false);
    fKnobValues.push_back(0);
  }
}

void NOvARwgtEngine::InitializeGENIE() {
  genie::RunOpt *grunopt = genie::RunOpt::Instance();
  grunopt->EnableBareXSecPreCalc(true);
  grunopt->SetEventGeneratorList(Config::GetParS("GENIEEventGeneratorList"));
  if (!Config::HasPar("GENIETune")) {
    NUIS_ABORT(
        "GENIE tune was not specified, this is required when reweighting GENIE "
        "V3+ events. Add a config parameter like: <config "
        "GENIETune=\"G18_10a_02_11a\" /> to your nuisance card.");
  }
  grunopt->SetTuneName(Config::GetParS("GENIETune"));
  grunopt->BuildTune();
  std::string genv =
      std::string(getenv("GENIE")) + "/config/Messenger_laconic.xml";
  genie::utils::app_init::MesgThresholds(genv);
}

size_t NOvARwgtEngine::GetWeightGeneratorIndex(std::string const &strname) {
  size_t upos = strname.find_first_of("_");

  if (strname.find("CVTune2017") == 0) {
    if (upos == std::string::npos) {
      return kCVTune2017;
    }
    std::string knobname = strname.substr(upos + 1);
    if (novarwgt::kCVTune2017.SystKnobs().count(knobname)) {
      auto loc = novarwgt::kCVTune2017.SystKnobs().find(knobname);

      return kCVTune2017 + 1 +
             std::distance(novarwgt::kCVTune2017.SystKnobs().begin(), loc);
    }

  } else if (strname.find("CVTune2018") == 0) {
    if (upos == std::string::npos) {
      return kCVTune2018;
    }
    std::string knobname = strname.substr(upos + 1);
    if (novarwgt::kCVTune2018.SystKnobs().count(knobname)) {
      auto loc = novarwgt::kCVTune2018.SystKnobs().find(knobname);

      return kCVTune2018 + 1 +
             std::distance(novarwgt::kCVTune2018.SystKnobs().begin(), loc);
    }
  } else if (strname.find("CVTune2020") == 0) {
    if (upos == std::string::npos) {
      return kCVTune2020;
    }
    std::string knobname = strname.substr(upos + 1);
    if (novarwgt::kCVTune2020.SystKnobs().count(knobname)) {
      auto loc = novarwgt::kCVTune2020.SystKnobs().find(knobname);

      return kCVTune2020 + 1 +
             std::distance(novarwgt::kCVTune2020.SystKnobs().begin(), loc);
    }
  } else if (strname.find("CVTuneSA") == 0) {
    if (upos == std::string::npos) {
      return kCVTuneSA;
    }
    std::string knobname = strname.substr(upos + 1);
    if (novarwgt::kCVTuneSA.SystKnobs().count(knobname)) {
      auto loc = novarwgt::kCVTuneSA.SystKnobs().find(knobname);

      return kCVTuneSA + 1 +
             std::distance(novarwgt::kCVTuneSA.SystKnobs().begin(), loc);
    }
  }
  return kNoSuchKnob;
}

void NOvARwgtEngine::IncludeDial(std::string name, double startval) {
  size_t we_indx = GetWeightGeneratorIndex(name);
  if (we_indx == kNoSuchKnob) {
    NUIS_ABORT("[ERROR]: Invalid NOvARwgt Engine name: " << name);
  }
  bool IsTune = !(we_indx % (NUIS_DIAL_OFFSET /10));
  if (IsTune) {
    auto tune_idx = fTuneEnums[we_indx];
    fTuneValues[tune_idx] = startval;
    fTuneInUse[tune_idx] = true;
  } else {
    auto knob_idx = fKnobEnums[we_indx];
    fKnobValues[knob_idx] = startval;
    fKnobInUse[knob_idx] = true;
  }
};

void NOvARwgtEngine::SetDialValue(int nuisenum, double val) {
  size_t we_indx = (nuisenum % NUIS_DIAL_OFFSET);
  bool IsTune = !(we_indx % (NUIS_DIAL_OFFSET /10));

  if (IsTune) {
    auto tune_idx = fTuneEnums[we_indx];
    if (!fTuneInUse[tune_idx]) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
                 << we_indx << " but that tune hasn't been included yet.");
    }
    fTuneValues[tune_idx] = val;
  } else {
    auto knob_idx = fKnobEnums[we_indx];
    if (!fKnobInUse[knob_idx]) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
                 << we_indx << " but that Knob hasn't been included yet.");
    }
    fKnobValues[knob_idx] = val;
  }
}

void NOvARwgtEngine::SetDialValue(std::string name, double val) {
  SetDialValue(GetWeightGeneratorIndex(name), val);
}

bool NOvARwgtEngine::IsDialIncluded(std::string name) {
  return IsDialIncluded(GetWeightGeneratorIndex(name));
}
bool NOvARwgtEngine::IsDialIncluded(int nuisenum) {
  size_t we_indx = (nuisenum % NUIS_DIAL_OFFSET);
  bool IsTune = !(we_indx % (NUIS_DIAL_OFFSET /10));

  if (IsTune) {
    auto tune_idx = fTuneEnums[we_indx];
    return fTuneInUse[tune_idx];
  } else {
    auto knob_idx = fKnobEnums[we_indx];
    return fKnobInUse[knob_idx];
  }
}

double NOvARwgtEngine::GetDialValue(std::string name) {
  return GetDialValue(GetWeightGeneratorIndex(name));
}
double NOvARwgtEngine::GetDialValue(int nuisenum) {
  size_t we_indx = (nuisenum % NUIS_DIAL_OFFSET);
  if (we_indx == kNoSuchKnob) {
    NUIS_ABORT("[ERROR]: Invalid NOvARwgt Engine enum: " << nuisenum);
  }
  bool IsTune = !(we_indx % (NUIS_DIAL_OFFSET /10));

  if (IsTune) {
    auto tune_idx = fTuneEnums[we_indx];
    if (!fTuneInUse[tune_idx]) {
      NUIS_ABORT("[ERROR]: GetDialValue for NOvARwgt dial: "
                 << we_indx << " but that tune hasn't been included yet.");
    }
    return fTuneValues[tune_idx];
  } else {
    auto knob_idx = fKnobEnums[we_indx];
    if (!fKnobInUse[knob_idx]) {
      NUIS_ABORT("[ERROR]: GetDialValue for NOvARwgt dial: "
                 << we_indx << " but that Knob hasn't been included yet.");
    }
    return fKnobValues[knob_idx];
  }
}

double NOvARwgtEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;

  // Make nom weight
  if (!evt) {
    NUIS_ABORT("evt not found : " << evt);
  }

  // Skip Non GENIE
  if (evt->fType != kGENIE)
    return 1.0;

  if (!(evt->genie_event)) {
    NUIS_ABORT("evt->genie_event not found!" << evt->genie_event);
  }

  if (!(evt->genie_event->event)) {
    NUIS_ABORT("evt->genie_event->event GHepRecord not found!"
               << (evt->genie_event->event));
  }

  novarwgt::EventRecord rcd =
      novarwgt::ConvertGenieEvent(evt->genie_event->event);

  for (size_t k_it = 0; k_it < fKnobs.size(); ++k_it) {
    if (!fKnobInUse[k_it]) {
      continue;
    }

    double wght = fKnobTunes[k_it]->EventSystKnobWeight(
        fKnobs[k_it]->Name(), fKnobValues[k_it], rcd, {},
        fTuneInUse[fKnobTuneidx[k_it]] && fTuneValues[fKnobTuneidx[k_it]]);

    // // have to divide out the CV weight for this, ugly hack because the last
    // // parameter doesn't do what I want
    // if (fTuneInUse[fKnobTuneidx[k_it]] && fTuneValues[fKnobTuneidx[k_it]]) {
    //   wght /= fKnobTunes[k_it]->EventSystKnobWeight(fKnobs[k_it]->Name(), 0,
    //                                                 rcd, {}, false);
    // }

    rw_weight *= wght;
  }

  for (size_t k_it = 0; k_it < fTunes.size(); ++k_it) {
    if (!fTuneInUse[k_it]) {
      continue;
    }
    if (!fTuneValues[k_it]) {
      continue;
    }
    double wght = fTunes[k_it]->EventWeight(rcd);
    rw_weight *= wght;
  }

  return rw_weight;
}
