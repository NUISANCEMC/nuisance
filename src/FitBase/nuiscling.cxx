#include "nuiscling.h"

#include "FitEvent.h"

#include "TInterpreter.h"

using namespace nuiscling_ftypes;

//Borrowed from Cling::utils
template <class T> T VoidToFunctionPtr(void *ptr) {
  union {
    T f;
    void *v;
  } tmp;
  tmp.v = ptr;
  return tmp.f;
}

nuiscling *nuiscling::instance_ = nullptr;

nuiscling &nuiscling::Get() {
  if (!instance_) {
    instance_ = new nuiscling();
  }
  return *instance_;
}

nuiscling::nuiscling() {
#ifdef NEUT_ENABLED
  std::cout << "Passing -DNEUT_ENABLED" << std::endl;
  gInterpreter->LoadText("#define NEUT_ENABLED");
  std::cout << "[INFO]: Adding " << NEUT_INC_DIR << " to include path."
            << std::endl;
  gInterpreter->AddIncludePath(NEUT_INC_DIR);
#endif
#ifdef NuWro_ENABLED
  std::cout << "Passing -DNuWro_ENABLED" << std::endl;
  gInterpreter->LoadText("#define NuWro_ENABLED");
  std::cout << "[INFO]: Adding " << NuWro_INC_DIR << " to include path."
            << std::endl;
  gInterpreter->AddIncludePath(NuWro_INC_DIR);
#endif
#ifdef GENIE_ENABLED
  std::cout << "Passing -DGENIE_ENABLED" << std::endl;
  gInterpreter->LoadText("#define GENIE_ENABLED");
  std::cout << "[INFO]: Adding " << GENIE_INC_DIR << " to include path."
            << std::endl;
  gInterpreter->AddIncludePath(GENIE_INC_DIR);
#endif
#ifdef GENIE3_API_ENABLED
  std::cout << "Passing -DGENIE3_API_ENABLED" << std::endl;
  gInterpreter->LoadText("#define GENIE3_API_ENABLED");
#endif
#ifdef GiBUU_ENABLED
  std::cout << "Passing -DGiBUU_ENABLED" << std::endl;
  gInterpreter->LoadText("#define GiBUU_ENABLED");
#endif
}

bool nuiscling::LoadFile(std::string const &file_to_read) {
  return !bool(gInterpreter->LoadFile(file_to_read.c_str()));
}

nuiscling_ftypes::filter
nuiscling::GetFilterFunction(std::string const &fnname) {
  void *sym = gInterpreter->FindSym(fnname.c_str());
  if (!sym) {
    std::cout << "No function named: " << fnname << " declared to TCling."
              << std::endl;
    return nullptr;
  }
  return VoidToFunctionPtr<nuiscling_ftypes::filter>(sym);
}

nuiscling_ftypes::project
nuiscling::GetProjectionFunction(std::string const &fnname) {
  void *sym = gInterpreter->FindSym(fnname.c_str());
  if (!sym) {
    std::cout << "No function named: " << fnname << " declared to TCling."
              << std::endl;
    return nullptr;
  }
  return VoidToFunctionPtr<nuiscling_ftypes::project>(sym);
}
nuiscling_ftypes::weight
nuiscling::GetWeightFunction(std::string const &fnname) {
  void *sym = gInterpreter->FindSym(fnname.c_str());
  if (!sym) {
    std::cout << "No function named: " << fnname << " declared to TCling."
              << std::endl;
    return nullptr;
  }
  return VoidToFunctionPtr<nuiscling_ftypes::weight>(sym);
}

bool nuiscling::Filter(FitEvent const *event, nuiscling_ftypes::filter fn) {
  return fn(event);
}
std::vector<double>
nuiscling::Project(FitEvent const *event,
                   std::vector<nuiscling_ftypes::project> fns) {
  std::vector<double> projections;
  for (auto &fn : fns) {
    projections.push_back(fn(event));
  }
  return projections;
}
std::vector<double>
nuiscling::Weight(FitEvent const *event,
                  std::vector<nuiscling_ftypes::weight> fns) {
  std::vector<double> weights;
  for (auto &fn : fns) {
    weights.push_back(fn(event));
  }
  return weights;
}