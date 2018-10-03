#include "SmearceptanceUtils.h"

#include "FitLogger.h"

#include "TDecompSVD.h"

#include <cassert>
#include <limits>

int main(int argc, char const *argv[]) {
  double numerTol = 1.E2 * std::numeric_limits<double>::epsilon();
  // a = 10, b = 4
  // x = 5a + 3b (= 62)
  // y = 6a - 2b (= 52)

  // A = (10,4)
  // M = ( (5,  3)
  //       (6, -2) )
  // X = (62,52)

  // A^T M = X
  TVectorD A(2);
  A[0] = 10;
  A[1] = 4;

  TVectorD X(2);
  X[0] = 62;
  X[1] = 52;

  TMatrixD M(2, 2);
  M[0][0] = 5;
  M[0][1] = 3;
  M[1][0] = 6;
  M[1][1] = -2;

  TVectorD ForwardSolve = M * A;

  bool similar = true;

  for (int i = 0; i < A.GetNrows(); ++i) {
    if (fabs(X[i] - ForwardSolve[i]) > numerTol) {
      ERROR(FTL, "Element " << i << " was not multiplied as expected: " << X[i]
                            << " != " << ForwardSolve[i]
                            << " (Tol: " << fabs(X[i] - ForwardSolve[i])
                            << " > " << numerTol << ")");
      similar = false;
    }
  }

  assert(similar);

  // TVectorD SVDSolve = SmearceptanceUtils::SVDInverseSolve(&A, &M);

  TDecompSVD svd(M);

  TMatrixD inv = svd.Invert();

  TVectorD SVDSolve = inv * X;

  similar = true;

  for (int i = 0; i < A.GetNrows(); ++i) {
    if (fabs(A[i] - SVDSolve[i]) > numerTol) {
      ERROR(FTL, "Element " << i << " was not solved as expected: " << A[i]
                            << " != " << SVDSolve[i]
                            << " (Tol: " << fabs(A[i] - SVDSolve[i]) << " > "
                            << numerTol << ")");
      similar = false;
    }
  }

  assert(similar);
}
