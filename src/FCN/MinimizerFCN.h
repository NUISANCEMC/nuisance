#ifndef _MINIMIZER_FCN_H_
#define _MINIMIZER_FCN_H_
/*!     
*  \addtogroup FCN      
*  @{  
*/

#include <iostream>
#include <vector>
#include "FitLogger.h"
#include "JointFCN.h"


//! Wrapper for JointFCN to make ROOT minimization behave sensibly.
class MinimizerFCN{
 public:

  // Empty Construction
  MinimizerFCN(){
    fFCN = NULL;
  }

  // Construct from function
  MinimizerFCN(JointFCN* f){
    SetFCN(f);
  };

  // Destroy (Doesn't delete FCN)
  ~MinimizerFCN(){
  };

  /*
  inline double Up() const
  {
    return 1.;
  }
  */

  // Set FCN
  inline void SetFCN(JointFCN* f)
  {
    fFCN = f;
  };

  // Get FCN
  inline JointFCN* GetFCN() const
  {
    return fFCN;
  };
  
  // Wrapper for jointFCN Eval
  inline double DoEval(const double *x) const
  {
    
    if (!fFCN){
      NUIS_ERR(FTL,"No FCN Found in MinimizerFCN!");
      NUIS_ABORT("Exiting!");
    }
    
    return fFCN->DoEval(x);
  };

  // Func Operator for vectors
  inline double operator() (const std::vector<double> & x) const
  {
    double* x_array = new double[x.size()];
    return this->DoEval(x_array);
  };

  // Func Operator for arrays
  inline double operator() (const double *x) const
  {
    return this->DoEval(x);
  };
  
 private:
  
  JointFCN* fFCN;
};
/*! @} */
#endif // _MINIMIZER_FCN_H_
