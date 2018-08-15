#include "IDummy.hxx"

#include "plugins/traits.hxx"

#include <iomanip>
#include <iostream>
#include <memory>

struct AddOne : public IDummy {
  double val;
  void Configure(double v) { val = v; }
  void Shout() {
    std::cout << "[Config]: AddOne to " << val << " = " << (val + 1)
              << std::endl;
  }
};
DECLARE_PLUGIN(IDummy, AddOne);

struct Double : public IDummy {
  double val;
  void Configure(double v) { val = v; }
  void Shout() {
    std::cout << "[Shout]: Double " << val << " = " << (val * 2) << std::endl;
  }
};
DECLARE_PLUGIN(IDummy, Double);

struct Double2 : public IDummy2 {
  std::string val;
  void Configure2(std::string v) { val = v; }
  void Shout2() {
    std::cout << "[Shout2]: Double2 " << val << " = " << val << val << std::endl;
  }
};
DECLARE_PLUGIN(IDummy2, Double2);
