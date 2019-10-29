#include "NOvARwgtEngine.h"

#include "NOvARwgt/interfaces/GenieInterface.h"

#include "NOvARwgt/rwgt/genie/QE/MAQEWgts.h"
#include "NOvARwgt/rwgt/genie/QE/RPAWeights.h"

#include "NOvARwgt/rwgt/genie/MEC/EmpiricalMECOtherTunes.h"
#include "NOvARwgt/rwgt/genie/MEC/EmpiricalMECTune2017.h"
#include "NOvARwgt/rwgt/genie/MEC/EmpiricalMECTune2018.h"
#include "NOvARwgt/rwgt/genie/MEC/EmpiricalMECTuneSA.h"

#include "NOvARwgt/rwgt/genie/DIS/HighWDISWeight.h"
#include "NOvARwgt/rwgt/genie/DIS/Nonres1piWeights.h"

#include "NOvARwgt/rwgt/tunes/Tunes2017.h"
#include "NOvARwgt/rwgt/tunes/Tunes2018.h"
#include "NOvARwgt/rwgt/tunes/Tunes2019.h"
#include "NOvARwgt/rwgt/tunes/TunesSA.h"

#include "NOvARwgt/rwgt/EventRecord.h"
#include "NOvARwgt/rwgt/IWeightGenerator.h"
#include "NOvARwgt/rwgt/Tune.h"

static size_t const kRPAWeightCCQESA = 0;
static size_t const kRPAWeightCCQE_2017 = 1;
static size_t const kRPAWeightQ2_2017 = 2;
static size_t const kMAQEWeight_2018 = 3;
static size_t const kTufts2p2hWgtSA = 4;
static size_t const kEmpiricalMEC_to_Valencia_Wgt = 5;
static size_t const kEmpiricalMECWgt2018 = 6;
static size_t const kMECEnuShapeWgt = 7;
static size_t const kMECInitStateNPFracWgt = 8;
static size_t const kEmpiricalMECWgt2017 = 9;
static size_t const kEmpiricalMEC_to_GENIEQE_Wgt = 10;
static size_t const kEmpiricalMEC_to_GENIERES_Wgt = 11;
static size_t const kEmpiricalMECWgt2018RPAFix = 12;
static size_t const kMEC2018_QElike_Wgt = 13;
static size_t const kMEC2018_RESlike_Wgt = 14;
static size_t const kMEC2018RPAFix_QElike_Wgt = 15;
static size_t const kMEC2018RPAFix_RESlike_Wgt = 16;
static size_t const kNonres1PiWgt = 17;
static size_t const kHighWDISWgt_2018 = 18;
static size_t const kRESRPAWeightQ2_2017 = 19;
static size_t const kTuneSeparator = 100;
static size_t const kTune2017 = kTuneSeparator + 1;
static size_t const kTune2018 = kTuneSeparator + 2;
static size_t const kTune2019 = kTuneSeparator + 3;
static size_t const kTuneSA = kTuneSeparator + 4;
static size_t const kNoSuchWeightEngine = std::numeric_limits<size_t>::max();

size_t NOvARwgtEngine::GetWeightGeneratorIndex(std::string const &strname) {

  if (strname == "RPAWeightCCQESA") {
    return kRPAWeightCCQESA;
  } else if (strname == "RPAWeightCCQE_2017") {
    return kRPAWeightCCQE_2017;
  } else if (strname == "RPAWeightQ2_2017") {
    return kRPAWeightQ2_2017;
  } else if (strname == "RESRPAWeightQ2_2017") {
    return kRESRPAWeightQ2_2017;
  } else if (strname == "MAQEWeight_2018") {
    return kMAQEWeight_2018;
  } else if (strname == "Tufts2p2hWgtSA") {
    return kTufts2p2hWgtSA;
  } else if (strname == "EmpiricalMEC_to_Valencia_Wgt") {
    return kEmpiricalMEC_to_Valencia_Wgt;
  } else if (strname == "EmpiricalMECWgt2018") {
    return kEmpiricalMECWgt2018;
  } else if (strname == "MECEnuShapeWgt") {
    return kMECEnuShapeWgt;
  } else if (strname == "MECInitStateNPFracWgt") {
    return kMECInitStateNPFracWgt;
  } else if (strname == "EmpiricalMECWgt2017") {
    return kEmpiricalMECWgt2017;
  } else if (strname == "EmpiricalMEC_to_GENIEQE_Wgt") {
    return kEmpiricalMEC_to_GENIEQE_Wgt;
  } else if (strname == "EmpiricalMEC_to_GENIERES_Wgt") {
    return kEmpiricalMEC_to_GENIERES_Wgt;
  } else if (strname == "EmpiricalMECWgt2018RPAFix") {
    return kEmpiricalMECWgt2018RPAFix;
  } else if (strname == "MEC2018_QElike_Wgt") {
    return kMEC2018_QElike_Wgt;
  } else if (strname == "MEC2018_RESlike_Wgt") {
    return kMEC2018_RESlike_Wgt;
  } else if (strname == "MEC2018RPAFix_QElike_Wgt") {
    return kMEC2018RPAFix_QElike_Wgt;
  } else if (strname == "MEC2018RPAFix_RESlike_Wgt") {
    return kMEC2018RPAFix_RESlike_Wgt;
  } else if (strname == "Nonres1PiWgt") {
    return kNonres1PiWgt;
  } else if (strname == "HighWDISWgt_2018") {
    return kHighWDISWgt_2018;
  } else if (strname == "Tune2017") {
    return kTune2017;
  } else if (strname == "Tune2018") {
    return kTune2018;
  } else if (strname == "Tune2019") {
    return kTune2019;
  } else if (strname == "TuneSA") {
    return kTuneSA;
  }
  return kNoSuchWeightEngine;
}

novarwgt::IWeightGenerator *IWeightGeneratorFactory(size_t e) {
  switch (e) {
  case kRPAWeightCCQESA: {
    return new novarwgt::RPAWeightCCQESA();
  }
  case kRPAWeightCCQE_2017: {
    return new novarwgt::RPAWeightCCQE_2017("CV");
  }
  case kRPAWeightQ2_2017: {
    return new novarwgt::RPAWeightQ2_2017();
  }
  case kRESRPAWeightQ2_2017: {
    return new novarwgt::RPAWeightQ2_2017(novarwgt::kRxnCC, novarwgt::kScResonant);
  }
  case kMAQEWeight_2018: {
    return new novarwgt::MAQEWeight_2018();
  }
  case kTufts2p2hWgtSA: {
    return new novarwgt::Tufts2p2hWgtSA();
  }
  case kEmpiricalMEC_to_Valencia_Wgt: {
    return new novarwgt::EmpiricalMEC_to_Valencia_Wgt();
  }
  case kEmpiricalMECWgt2018: {
    return new novarwgt::EmpiricalMECWgt2018();
  }
  case kMECEnuShapeWgt: {
    return new novarwgt::MECEnuShapeWgt();
  }
  case kMECInitStateNPFracWgt: {
    return new novarwgt::MECInitStateNPFracWgt();
  }
  case kEmpiricalMECWgt2017: {
    return new novarwgt::EmpiricalMECWgt2017();
  }
  case kEmpiricalMEC_to_GENIEQE_Wgt: {
    return new novarwgt::EmpiricalMEC_to_GENIEQE_Wgt();
  }
  case kEmpiricalMEC_to_GENIERES_Wgt: {
    return new novarwgt::EmpiricalMEC_to_GENIERES_Wgt();
  }
  case kEmpiricalMECWgt2018RPAFix: {
    return new novarwgt::EmpiricalMECWgt2018RPAFix();
  }
  case kMEC2018_QElike_Wgt: {
    return new novarwgt::MEC2018_QElike_Wgt();
  }
  case kMEC2018_RESlike_Wgt: {
    return new novarwgt::MEC2018_RESlike_Wgt();
  }
  case kMEC2018RPAFix_QElike_Wgt: {
    return new novarwgt::MEC2018RPAFix_QElike_Wgt();
  }
  case kMEC2018RPAFix_RESlike_Wgt: {
    return new novarwgt::MEC2018RPAFix_RESlike_Wgt();
  }
  case kNonres1PiWgt: {
    return new novarwgt::Nonres1PiWgt();
  }
  case kHighWDISWgt_2018: {
    return new novarwgt::HighWDISWgt_2018();
  }
  default: { return NULL; }
  }
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
  if (we_e == kNoSuchWeightEngine) {
    NUIS_ABORT("[ERROR]: Invalid NOvARwgt Engine name: " << name);
  }
  if (we_e < kTuneSeparator) {
    if (fWeightEngineEnums.find(we_e) != fWeightEngineEnums.end()) {
      NUIS_ABORT("[ERROR]: NOvARwgt Engine name: " << name << " already included.");
    }
    fWeightEngineEnums[we_e] = fWeightEngines.size();
    fWeightEngines.push_back(IWeightGeneratorFactory(we_e));
    fWeightEngineValues.push_back(startval);
  } else {
    if (fTuneEnums.find(we_e) != fTuneEnums.end()) {
      NUIS_ABORT("[ERROR]: NOvARwgt Tune name: " << name << " already included.");
    }
    fTuneEnums[we_e] = fTunes.size();
    fTunes.push_back(TuneFactory(we_e));
    fTuneValues.push_back(startval);
  }
};

void NOvARwgtEngine::SetDialValue(int nuisenum, double val) {
  size_t we_indx = (nuisenum % 1000);
  if (we_indx < kTuneSeparator) {
    if (!fWeightEngineEnums.count(we_indx)) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
            << we_indx << " but that engine hasn't been included yet.");
    }
    size_t engine_index = fWeightEngineEnums[we_indx];

    fWeightEngineValues[engine_index] = val;
  } else {
    if (!fTuneEnums.count(we_indx)) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
            << we_indx << " but that tune hasn't been included yet.");
    }
    size_t engine_index = fTuneEnums[we_indx];

    fTuneValues[engine_index] = val;
  }
}

void NOvARwgtEngine::SetDialValue(std::string name, double val) {
  size_t we_indx = GetWeightGeneratorIndex(name);
  if (we_indx == kNoSuchWeightEngine) {
    NUIS_ABORT("[ERROR]: Invalid NOvARwgt Engine name: " << name);
  }
  if (we_indx < kTuneSeparator) {
    if (!fWeightEngineEnums.count(we_indx)) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
            << we_indx << " but that engine hasn't been included yet.");
    }
    size_t engine_index = fWeightEngineEnums[we_indx];

    fWeightEngineValues[engine_index] = val;
  } else {
    if (!fTuneEnums.count(we_indx)) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
            << we_indx << " but that tune hasn't been included yet.");
    }
    size_t engine_index = fTuneEnums[we_indx];

    fTuneValues[engine_index] = val;
  }
}

bool NOvARwgtEngine::IsDialIncluded(std::string name) {
  size_t we_indx = GetWeightGeneratorIndex(name);
  if (we_indx == kNoSuchWeightEngine) {
    NUIS_ABORT("[ERROR]: Invalid NOvARwgt Engine name: " << name);
  }
  if (we_indx < kTuneSeparator) {
    return fWeightEngineEnums.count(we_indx);
  } else {
    return fTuneEnums.count(we_indx);
  }
}
bool NOvARwgtEngine::IsDialIncluded(int nuisenum) {
  size_t we_indx = (nuisenum % 1000);
  if (we_indx < kTuneSeparator) {
    return fWeightEngineEnums.count(we_indx);
  } else {
    return fTuneEnums.count(we_indx);
  }
}

double NOvARwgtEngine::GetDialValue(std::string name) {
  size_t we_indx = GetWeightGeneratorIndex(name);
  if (we_indx == kNoSuchWeightEngine) {
    NUIS_ABORT("[ERROR]: Invalid NOvARwgt Engine name: " << name);
  }
  if (we_indx < kTuneSeparator) {
    if (!fWeightEngineEnums.count(we_indx)) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
            << we_indx << " but that engine hasn't been included yet.");
    }
    return fWeightEngineValues[fWeightEngineEnums[we_indx]];
  } else {
    if (!fTuneEnums.count(we_indx)) {
      NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
            << we_indx << " but that tune hasn't been included yet.");
    }
    return fTuneValues[fTuneEnums[we_indx]];
  }
}
double NOvARwgtEngine::GetDialValue(int nuisenum) {
  size_t we_indx = (nuisenum % 1000);
  if (!fWeightEngineEnums.count(we_indx)) {
    NUIS_ABORT("[ERROR]: SetDialValue for NOvARwgt dial: "
          << we_indx << " but that engine hasn't been included yet.");
  }
  if (we_indx < kTuneSeparator) {
    return fWeightEngineValues[fWeightEngineEnums[we_indx]];
  } else {
    return fTuneValues[fTuneEnums[we_indx]];
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

  static novarwgt::InputVals const dummyparams;

  for (size_t w_it = 0; w_it < fWeightEngines.size(); ++w_it) {
    if (fWeightEngineValues[w_it] ==
        0) { // if a dial is set to 0, don't include its weight
      continue;
    }
    rw_weight *= fWeightEngines[w_it]->GetWeight(rcd, dummyparams);
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
