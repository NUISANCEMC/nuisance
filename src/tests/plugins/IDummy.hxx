#ifndef TESTS_PLUGINS_IDUMMY_HXX_SEEN
#define TESTS_PLUGINS_IDUMMY_HXX_SEEN

#include "plugins/traits.hxx"

#include <iostream>

struct IDummy {
  virtual void Configure(double) = 0;
  virtual void Shout() = 0;
};

DECLARE_PLUGIN_INTERFACE(IDummy);

struct IDummy2 {
  virtual void Configure2(std::string) = 0;
  virtual void Shout2() = 0;
};

DECLARE_PLUGIN_INTERFACE(IDummy2);

#endif
