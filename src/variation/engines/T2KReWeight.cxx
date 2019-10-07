#include "variation/engines/T2KReWeight.hxx"

#include "fhiclcpp/ParameterSet.h"

#include "event/MinimalEvent.hxx"

// T2K Engine includes
#include "T2KNeutReWeight.h"
#include "T2KNIWGReWeight.h"
#include "T2KReWeight.h"

#include <iomanip>

using namespace nuis;
using namespace nuis::params;
using namespace nuis::event;

void T2KReWeightEngine::Initialize(fhicl::ParameterSet const &ps) {

  fT2KRW = std::make_unique<t2krew::T2KReWeight>();
  TDirectory *dir = gDirectory;
  fT2KRW->AdoptWghtEngine("NEUT", new t2krew::T2KNeutReWeight);
  fT2KRW->AdoptWghtEngine("NIWG", new t2krew::T2KNIWGReWeight);
  dir->cd();


  for (fhicl::ParameterSet param_ps :
       ps.get<std::vector<fhicl::ParameterSet>>("parameters")) {

    param_ps.put<std::string>("type", GetName());

    std::string const &param_name = param_ps.get<std::string>("name");

    T2KSystParam tsp;

    tsp.t2ksyst = t2krew::T2KSyst::FromString(param_name);

    if (tsp.t2ksyst == t2krew::kSystNull) {
      throw invalid_T2K_syst_name()
          << "[ERROR]: T2KReWeight failed to parse " << std::quoted(param_name)
          << " as a T2K dial.";
    }
    fT2KRW->Systematics().Include(tsp.t2ksyst);

    tsp.pid = ParameterManager::Get().EnsureParameterRegistered(param_ps);

    fT2KSysts.push_back(tsp);
  }

  Reconfigure();
}
void T2KReWeightEngine::Reconfigure() {

  for (T2KSystParam const &tsp : fT2KSysts) {
    double val = ParameterManager::Get().GetParameterValue(tsp.pid);
    fT2KRW->Systematics().SetTwkDial(tsp.t2ksyst, val);
    std::cout << "Reconf: " << t2krew::T2KSyst::AsString(tsp.t2ksyst) << " -> " << val << std::endl;
  }

  fT2KRW->Reconfigure();
}
double T2KReWeightEngine::GetEventWeight(nuis::event::MinimalEvent const &ev) {
  NeutVect const *nv = static_cast<NeutVect const *>(ev.fGenEvent);
  if (!nv) {
    return 1.0;
  }
  NeutVect *nv_nc = const_cast<NeutVect *>(nv);
  return fT2KRW->CalcWeight(nv_nc);
}

std::string T2KReWeightEngine::GetName() { return "T2KReWeightEngine"; }
std::string T2KReWeightEngine::GetDocumentation() { return ""; }
fhicl::ParameterSet T2KReWeightEngine::GetExampleConfiguration() {
  fhicl::ParameterSet ps;

  ps.put<std::string>("name", GetName());
  fhicl::ParameterSet dial_maqe;
  dial_maqe.put<std::string>("name", "NXSec_MaCCQE");

  dial_maqe.put<double>("start", 0);
  dial_maqe.put<double>("min", -3);
  dial_maqe.put<double>("max", 3);
  dial_maqe.put<double>("step", 0.1);

  fhicl::ParameterSet dial_mares;
  dial_mares.put<std::string>("name", "NXSec_MARES");

  dial_mares.put<double>("start", 0);
  dial_mares.put<double>("min", -3);
  dial_mares.put<double>("max", 3);
  dial_mares.put<double>("step", 0.1);

  ps.put<std::vector<fhicl::ParameterSet>>(
      "parameters", std::vector<fhicl::ParameterSet>{{dial_maqe, dial_mares}});

  return ps;
}

GeneratorManager::Generator_id_t T2KReWeightEngine::GetGeneratorId() {
  return GeneratorManager::Get().EnsureGeneratorRegistered("NEUT");
}

DECLARE_PLUGIN(IWeightProvider, T2KReWeightEngine);
