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

#include "NOvARwgt/rwgt/EventRecord.h"
#include "NOvARwgt/rwgt/IWeightGenerator.h"

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
static size_t const kNoSuchWeightEngine = std::numeric_limits<size_t>::max();

size_t NOvARwgtEngine::GetWeightGeneratorIndex(std::string const &strname) {

  if (strname == "RPAWeightCCQESA") {
    return kRPAWeightCCQESA;
  } else if (strname == "RPAWeightCCQE_2017") {
    return kRPAWeightCCQE_2017;
  } else if (strname == "RPAWeightQ2_2017") {
    return kRPAWeightQ2_2017;
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
  default: { return nullptr; }
  }
}

void NOvARwgtEngine::IncludeDial(std::string name, double startval) {
  size_t e = GetWeightGeneratorIndex(name);
  if (e == kNoSuchWeightEngine) {
    THROW("[ERROR]: Invalid NOvARwgt Engine name: " << name);
  }
  if (fWeightEngineEnums.find(e) != fWeightEngineEnums.end()) {
    THROW("[ERROR]: NOvARwgt Engine name: " << name << " already included.");
  }
  fWeightEngineEnums[e] = fWeightEngines.size();
  fWeightEngines.push_back(IWeightGeneratorFactory(e));
  fWeightEngineValues.push_back(startval);
};

void NOvARwgtEngine::SetDialValue(int nuisenum, double val) {
  size_t we_indx = (nuisenum % 1000);
  if (!fWeightEngineEnums.count(we_indx)) {
    THROW("[ERROR]: SetDialValue for NOvARwgt dial: "
          << we_indx << " but that engine hasn't been included yet.");
  }
  size_t engine_index = fWeightEngineEnums[we_indx];

  fWeightEngineValues[engine_index] = val;
}

void NOvARwgtEngine::SetDialValue(std::string name, double val) {
  size_t we_indx = GetWeightGeneratorIndex(name);
  if (we_indx == kNoSuchWeightEngine) {
    THROW("[ERROR]: Invalid NOvARwgt Engine name: " << name);
  }
  if (!fWeightEngineEnums.count(we_indx)) {
    THROW("[ERROR]: SetDialValue for NOvARwgt dial: "
          << we_indx << " but that engine hasn't been included yet.");
  }
  size_t engine_index = fWeightEngineEnums[we_indx];

  fWeightEngineValues[engine_index] = val;
}

bool NOvARwgtEngine::IsDialIncluded(std::string name) {
  size_t we_indx = GetWeightGeneratorIndex(name);
  if (we_indx == kNoSuchWeightEngine) {
    THROW("[ERROR]: Invalid NOvARwgt Engine name: " << name);
  }
  return fWeightEngineEnums.count(we_indx);
}
bool NOvARwgtEngine::IsDialIncluded(int nuisenum) {
  size_t we_indx = (nuisenum % 1000);
  return fWeightEngineEnums.count(we_indx);
}

double NOvARwgtEngine::GetDialValue(std::string name) {
  size_t we_indx = GetWeightGeneratorIndex(name);
  if (we_indx == kNoSuchWeightEngine) {
    THROW("[ERROR]: Invalid NOvARwgt Engine name: " << name);
  }
  if (!fWeightEngineEnums.count(we_indx)) {
    THROW("[ERROR]: SetDialValue for NOvARwgt dial: "
          << we_indx << " but that engine hasn't been included yet.");
  }
  return fWeightEngineValues[fWeightEngineEnums[we_indx]];
}
double NOvARwgtEngine::GetDialValue(int nuisenum) {
  size_t we_indx = (nuisenum % 1000);
  if (!fWeightEngineEnums.count(we_indx)) {
    THROW("[ERROR]: SetDialValue for NOvARwgt dial: "
          << we_indx << " but that engine hasn't been included yet.");
  }
  return fWeightEngineValues[fWeightEngineEnums[we_indx]];
}

double NOvARwgtEngine::CalcWeight(BaseFitEvt *evt) {
  double rw_weight = 1.0;

  // Make nom weight
  if (!evt) {
    THROW("evt not found : " << evt);
  }

  // Skip Non GENIE
  if (evt->fType != kGENIE)
    return 1.0;

  if (!(evt->genie_event)) {
    THROW("evt->genie_event not found!" << evt->genie_event);
  }

  if (!(evt->genie_event->event)) {
    THROW("evt->genie_event->event GHepRecord not found!"
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

  return rw_weight;
}
