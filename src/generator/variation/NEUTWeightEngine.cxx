#include "generator/variation/NEUTWeightEngine.hxx"
#include "generator/variation/FillNEUTCommons.hxx"

#include "fhiclcpp/ParameterSet.h"

#include "event/MinimalEvent.hxx"

// NEUT Engine includes
#include "NReWeight.h"
#include "NReWeightCasc.h"
#include "NReWeightNuXSecCCQE.h"
#include "NReWeightNuXSecCCRES.h"
#include "NReWeightNuXSecCOH.h"
#include "NReWeightNuXSecDIS.h"
#include "NReWeightNuXSecNC.h"
#include "NReWeightNuXSecNCEL.h"
#include "NReWeightNuXSecNCRES.h"
#include "NReWeightNuXSecRES.h"
#include "NReWeightNuclPiless.h"

#include "NSystUncertainty.h"

#include <iomanip>

using namespace nuis;
using namespace nuis::params;
using namespace nuis::event;

void NEUTWeightEngine::Initialize(fhicl::ParameterSet const &ps) {

  fNeutRW = std::make_unique<neut::rew::NReWeight>();
  TDirectory *dir = gDirectory;
  fNeutRW->AdoptWghtCalc("xsec_ccqe", new neut::rew::NReWeightNuXSecCCQE);
  fNeutRW->AdoptWghtCalc("xsec_res", new neut::rew::NReWeightNuXSecRES);
  fNeutRW->AdoptWghtCalc("xsec_ccres", new neut::rew::NReWeightNuXSecCCRES);
  fNeutRW->AdoptWghtCalc("xsec_coh", new neut::rew::NReWeightNuXSecCOH);
  fNeutRW->AdoptWghtCalc("xsec_dis", new neut::rew::NReWeightNuXSecDIS);
  fNeutRW->AdoptWghtCalc("xsec_ncel", new neut::rew::NReWeightNuXSecNCEL);
  fNeutRW->AdoptWghtCalc("xsec_nc", new neut::rew::NReWeightNuXSecNC);
  fNeutRW->AdoptWghtCalc("xsec_ncres", new neut::rew::NReWeightNuXSecNCRES);
  fNeutRW->AdoptWghtCalc("nucl_casc", new neut::rew::NReWeightCasc);
  fNeutRW->AdoptWghtCalc("nucl_piless", new neut::rew::NReWeightNuclPiless);
  dir->cd();

  for (fhicl::ParameterSet param_ps :
       ps.get<std::vector<fhicl::ParameterSet>>("parameters")) {

    param_ps.put<std::string>("type", GetName());

    std::string const &param_name = param_ps.get<std::string>("name");

    NEUTSystParam nsp;

    nsp.nsyst = neut::rew::NSyst::FromString(param_name);

    if (nsp.nsyst == neut::rew::kNullSystematic) {
      throw invalid_NEUT_syst_name() << "[ERROR]: NReWeight failed to parse "
                                     << std::quoted(param_name) << " as a NEUT dial.";
    }
    fNeutRW->Systematics().Init(nsp.nsyst);

    nsp.pid = ParameterManager::Get().EnsureParameterRegistered(param_ps);

    fNEUTSysts.push_back(nsp);
  }

  Reconfigure();
}
void NEUTWeightEngine::Reconfigure() {

  for (NEUTSystParam const &nsp : fNEUTSysts) {
    double val = ParameterManager::Get().GetParameterValue(nsp.pid);
    fNeutRW->Systematics().Set(nsp.nsyst, val);
  }

  fNeutRW->Reconfigure();
}
double NEUTWeightEngine::GetEventWeight(nuis::event::MinimalEvent const &ev) {
  if (!ev.fNeutVect) {
    return 1.0;
  }
  NEUTUtils::FillNeutCommons(ev.fNeutVect);
  return fNeutRW->CalcWeight();
}

std::string NEUTWeightEngine::GetName() { return "NEUTWeightEngine"; }
std::string NEUTWeightEngine::GetDocumentation() { return ""; }
fhicl::ParameterSet NEUTWeightEngine::GetExampleConfiguration() {
  fhicl::ParameterSet ps;

  ps.put<std::string>("weight_engine_name", GetName());
  fhicl::ParameterSet dial_maqe;
  dial_maqe.put<std::string>("name", "MAQE");

  dial_maqe.put<double>("start", 0);
  dial_maqe.put<double>("min", -3);
  dial_maqe.put<double>("max", 3);
  dial_maqe.put<double>("step", 0.1);

  fhicl::ParameterSet dial_mares;
  dial_mares.put<std::string>("name", "MARES");

  dial_mares.put<double>("start", 0);
  dial_mares.put<double>("min", -3);
  dial_mares.put<double>("max", 3);
  dial_mares.put<double>("step", 0.1);

  ps.put<std::vector<fhicl::ParameterSet>>(
      "parameters", std::vector<fhicl::ParameterSet>{{dial_maqe, dial_mares}});

  return ps;
}

DECLARE_PLUGIN(IWeightProvider, NEUTWeightEngine);
