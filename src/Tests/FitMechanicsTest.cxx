#include "DummyMinimizer.h"

int main() {

  double FakeDataError = 1E-4;
  double FakeDataErrorTol = 1.01 * FakeDataError;
  Config::SetPar("FakeDataError", FakeDataError);

  DummyMinimizer min;

  std::cout << "======Running simple minimizer test======" << std::endl;

  min.Run();

  std::cout << "======Done minimizing, checking output======" << std::endl;

  std::vector<string> pnames;
  pnames.push_back("mode_1");
  pnames.push_back("mode_2");
  pnames.push_back("mode_3");

  std::vector<double> postfitvals;
  postfitvals.push_back(1);
  postfitvals.push_back(1.5);
  postfitvals.push_back(2);

  for (size_t i = 0; i < 3; ++i) {
    std::cout << "\tTest param name: " << pnames[i]
              << " == " << min.GetParamName(i) << std::endl;
    assert(pnames[i] == min.GetParamName(i));
    std::cout << "\tTest param post-fit value : fabs(" << postfitvals[i]
              << " - " << min.GetParamVal(i) << ") < " << min.GetParamError(i)
              << std::endl;
    assert(fabs(postfitvals[i] - min.GetParamVal(i)) < min.GetParamError(i));
    std::cout << "\tTest post-fit error " << min.GetParamError(i) << " < "
              << FakeDataErrorTol << std::endl;
    assert(min.GetParamError(i) < FakeDataErrorTol);
  }

  return 0;
}
