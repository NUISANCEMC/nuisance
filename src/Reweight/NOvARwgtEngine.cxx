#include "NOvARwgtEngine.h"

#include "NOvARwgt/interfaces/GenieInterface.h"

#include "NOvARwgt/rwgt/tunes/Tunes2017.h"
#include "NOvARwgt/rwgt/tunes/Tunes2018.h"
#include "NOvARwgt/rwgt/tunes/Tunes2019.h"
#include "NOvARwgt/rwgt/tunes/TunesSA.h"

#include "NOvARwgt/rwgt/EventRecord.h"
#include "NOvARwgt/rwgt/Tune.h"
#include "NOvARwgt/rwgt/ISystKnob.h"

static size_t const kCVTune2017 = 100;
static size_t const kCVTune2018 = 200;
static size_t const kCVTune2019 = 300;
static size_t const kCVTuneSA = 400;
static size_t const kNoSuchKnob = std::numeric_limits<size_t>::max();

size_t NOvARwgtEngine::GetWeightGeneratorIndex(std::string const &strname) {
  int upos = strname.find_first_of("_");

  if (strname.find("CVTune2017") == 0) {
    if (upos == std::string::npos) {
      return kCVTune2017;
    }
    std::string knobname = strname.substr(upos + 1);
    if (novarwgt::kCVTune2017.SystKnobs().contains(knobname)) {
      auto loc = std::find(fKnobs.begin(), fKnobs.end(),
                           novarwgt::kCVTune2017.SystKnobs()[knobname]);
      if (loc == fKnobs.end()) {
        size_t rtn = kCVTune2017 + 1 + fKnobs.size();
        fKnobs.push_back(novarwgt::kCVTune2017.SystKnobs()[knobname]);
        return rtn;
      } else {
        return kCVTune2017 + 1 + std::distance(fKnobs.begin(), loc);
      }
    }
  } else if (strname.find("CVTune2018") == 0) {
    if (upos == std::string::npos) {
      return kCVTune2018;
    }
    std::string knobname = strname.substr(upos + 1);
    if (novarwgt::kCVTune2018.SystKnobs().contains(knobname)) {
      auto loc = std::find(fKnobs.begin(), fKnobs.end(),
                           novarwgt::kCVTune2018.SystKnobs()[knobname]);
      if (loc == fKnobs.end()) {
        size_t rtn = kCVTune2018 + 1 + fKnobs.size();
        fKnobs.push_back(novarwgt::kCVTune2018.SystKnobs()[knobname]);
        return rtn;
      } else {
        return kCVTune2018 + 1 + std::distance(fKnobs.begin(), loc);
      }
    }
  } else if (strname.find("CVTune2019") == 0) {
    if (upos == std::string::npos) {
      return kCVTune2019;
    }
    std::string knobname = strname.substr(upos + 1);
    if (novarwgt::kCVTune2019.SystKnobs().contains(knobname)) {
      auto loc = std::find(fKnobs.begin(), fKnobs.end(),
                           novarwgt::kCVTune2019.SystKnobs()[knobname]);
      if (loc == fKnobs.end()) {
        size_t rtn = kCVTune2019 + 1 + fKnobs.size();
        fKnobs.push_back(novarwgt::kCVTune2019.SystKnobs()[knobname]);
        return rtn;
      } else {
        return kCVTune2019 + 1 + std::distance(fKnobs.begin(), loc);
      }
    }
  } else if (strname.find("CVTuneSA") == 0) {
    if (upos == std::string::npos) {
      return kCVTuneSA;
    }
    std::string knobname = strname.substr(upos + 1);
    if (novarwgt::kCVTuneSA.SystKnobs().contains(knobname)) {
      auto loc = std::find(fKnobs.begin(), fKnobs.end(),
                           novarwgt::kCVTuneSA.SystKnobs()[knobname]);
      if (loc == fKnobs.end()) {
        size_t rtn = kCVTuneSA + 1 + fKnobs.size();
        fKnobs.push_back(novarwgt::kCVTuneSA.SystKnobs()[knobname]);
        return rtn;
      } else {
        return kCVTuneSA + 1 + std::distance(fKnobs.begin(), loc);
      }
    }
  }
  return kNoSuchKnob;
}

novarwgt::Tune const *TuneFactory(size_t e) {
  switch (e) {
  case kTune2017: {
    return &novarwgt::kCVTune2017;
  }
  case kTune2018: {
    return &novarwgt::kCVTune2018;
  }
  case kTune2019: {
    return &novarwgt::kCVTune2019;
  }
  case kTuneSA: {
    return &novarwgt::kCVTuneSA;
  }
  default: { return NULL; }
  }
}

void NOvARwgtEngine::IncludeDial(std::string name, double startval) {
  size_t we_e = GetWeightGeneratorIndex(name);
  if (we_e == kNoSuchKnob) {
    NUIS_ABORT("[ERROR]: Invalid NOvARwgt Engine name: " << name);
  }
  bool IsTune = !(we_e % 100);
  if (IsTune) {
    if (fTuneEnums.find(we_e) != fTuneEnums.end()) {
      NUIS_ABORT("[ERROR]: NOvARwgt Tune name: " << name
                                                 << " already included.");
    }
    fTuneEnums[we_e] = fTunes.size();
    fTunes.push_back(TuneFactory(we_e));
    fTuneValues.push_back(startval);
  } else {
    // The GetWeightGeneratorIndex caches the Knob so we don't have to.
    fKnobEnums[we_e] = fKnobValues.size();
    fKnobValues.push_back(startval);
  }
};

void NOvARwgtEngine::SetDialValue(int nuisenum, double val) {
  size_t we_indx = (nuisenum % 1000);
  bool IsTune = !(we_e % 100);

  if (IsTune) {
    if (!fTuneEnums.count(we_indx)) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
                 << we_indx << " but that tune hasn't been included yet.");
    }
    size_t engine_index = fTuneEnums[we_indx];
    fTuneValues[engine_index] = val;
  } else {
    if (!fKnobEnums.count(we_indx)) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
                 << we_indx << " but that Knob hasn't been included yet.");
    }
    size_t engine_index = fKnobEnums[we_indx];
    fKnobValues[engine_index] = val;
  }
}

void NOvARwgtEngine::SetDialValue(std::string name, double val) {
  SetDialValue(GetWeightGeneratorIndex(name), val);
}

bool NOvARwgtEngine::IsDialIncluded(std::string name) {
  return IsDialIncluded(GetWeightGeneratorIndex(name));
}
bool NOvARwgtEngine::IsDialIncluded(int nuisenum) {
  size_t we_indx = (nuisenum % 1000);
  bool IsTune = !(we_e % 100);

  if (IsTune) {
    return fTuneEnums.count(we_indx);
  } else {
    return fKnobEnums.count(we_indx);
  }
}

double NOvARwgtEngine::GetDialValue(std::string name) {
  return GetDialValueGetWeightGeneratorIndex(name));
}
double NOvARwgtEngine::GetDialValue(int nuisenum) {
  size_t we_indx = (nuisenum % 1000);
  if (we_indx == kNoSuchKnob) {
    NUIS_ABORT("[ERROR]: Invalid NOvARwgt Engine name: " << name);
  }
  bool IsTune = !(we_e % 100);

  if (IsTune) {
    if (!fTuneEnums.count(we_indx)) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
                 << we_indx << " but that tune hasn't been included yet.");
    }
    return fTuneValues[fTuneEnums[we_indx]];
  } else {

    if (!fKnobEnums.count(we_indx)) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
                 << we_indx << " but that engine hasn't been included yet.");
    }
    return fKnobValues[fKnobEnums[we_indx]];
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

  for (size_t w_it = 0; w_it < fKnobs.size(); ++w_it) {
    rw_weight *= fKnobs[w_it]->CalcWeight(fKnobValues[w_it], rcd);
  }

  for (size_t w_it = 0; w_it < fTunes.size(); ++w_it) {
    if (fTuneValues[w_it] ==
        0) { // if a dial is set to 0, don't include its weight
      continue;
    }
    rw_weight *= fTunes[w_it]->EventWeight(rcd);
  }

  return rw_weight;
}
