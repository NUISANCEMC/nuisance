#pragma once

// linux
#include <dlfcn.h>

#include <string>
#include <memory>
#include <iostream>

struct NamedSO {
  std::string name;
  void *dllib;

  NamedSO() : name(""), dllib(nullptr) {}
  NamedSO(NamedSO const &) = delete;
  NamedSO(NamedSO &&other) : name(std::move(other.name)), dllib(other.dllib) {
    other.dllib = nullptr;
  }

  ~NamedSO() {
    if (dllib) {
#ifdef DEBUG_INSTANTIATE
      std::cout << "[INFO]: dlclose on shared object: " << std::quoted(name)
                << std::endl;
#endif
      dlclose(dllib);
    }
  }
};
