#include "plugins/PluginManager.hxx"

#include "IDummy.hxx"

#include <memory>

int main() {
  nuis::plugins::plugin_traits<IDummy>::unique_ptr_t addone =
      nuis::plugins::Instantiate<IDummy>("AddOne");
  addone->Configure(5);
  addone->Shout();

  nuis::plugins::plugin_traits<IDummy>::unique_ptr_t dbl =
      nuis::plugins::Instantiate<IDummy>("Double");
  dbl->Configure(5);
  dbl->Shout();

  try {
    nuis::plugins::plugin_traits<IDummy>::unique_ptr_t fail =
        nuis::plugins::Instantiate<IDummy>("Failure");
  } catch (nuis::plugins::failed_to_find_instantiator &e) {
    std::cout << "[SUCCESS]: Caught " << e.what() << std::endl;
  }

  nuis::plugins::plugin_traits<IDummy2>::unique_ptr_t dbl2 =
      nuis::plugins::Instantiate<IDummy2>("Double2");
  dbl2->Configure2("HelloPluginWorld!");
  dbl2->Shout2();

}
