#ifndef _ERPA_H_SEEN_
#define _ERPA_H_SEEN_

#include <iostream>
#include <cmath>

// =====================
//
// Roughly a copy/paste job from NIWGReWeight/NIWGReWeightEffectiveRPA.cc/h
// Written by Asmita Redij, University of Bern
// Implemented in MaCh3 by Clarence Wret: c.wret14@imperial.ac.uk
//
// =====================
//
// Purpose:
//
// eRPA attempts to mimic Nieves RPA calculation in functional form (cubic, switching to exponential)
// Currently, there are no RPA shape parameters to include as RPA systematics
// eRPA remedies this by ffitting a functional form to the Nieves calculation
//
// Defaults have been fit to Nieves RPA calculation
// +/- 1 sigma in eRPA are also set after a fit to the +/- 1 sigma from Nieves RPA
//
// The eRPA calculation is a function of Q2 and return a RPA/no-RPA correction factor for a given value of Q2
// It should currently only be applied to CCQE (or 2p2h?) interactions on nuclear targets (i.e. ignore H interactions!)
//
//
// The eRPA calculation is a function which depends on five parameters. The naming of the parameters has changed as work 
// has developed on eRPA. Originally, eRPA was discussed in terms of five parameters: A, B, C, D, and U. The interpretation
// of these parameters is:
// Polynomial terms:
// A = RPA correction at Q2 = 0
// B = Slope of RPA correction at Q2 = 0
// C = RPA correction when polynominal switches to exponential (when Q2 = U)
// Exponential terms:
// D = Controls the damping constant of the exponential when Q2 > U
// Control terms:
// U =  1.20, the value of Q2 where we switch from polynomial to exponential. 
//      Can be varied but is in Jan 2016 not recommended use.
// 
// We have now changed to describing eRPA in terms of the five parameters A, B, D, E, and U. The formula is identical
// to the one which depends on A, B, C, D, and U. The relation between the new and old parameters is:
// A (old) = A (new)
// B (old) = B (new)
// C (old) = D (new)
// D (old) = E (new)
// U (old) = U (new)
//
// To make thing extra confusing, the new parameterisation also includes a parameter C given by
// C = D+U*E*(D-1)/3
// This is *not* related to C in the old parameterisation, and is *not* directly used as a parameter by us (it's indirectly
// determined by the values of D, U, and E)
//
// More info about the eRPA parameterisation (and anything else you could possibly want to know!) is in T2K-TN-309.
// Some more info about eRPA is also available in the slides at http://www.t2k.org/asg/oagroup/meeting/2016/banffoa27seppm/ffsanderpa/
// (beware: they use the "old" A,B,C,D,U parameter names).
//
// =====================

// Calculates the absolute eRPA for given Q2, BERNSTEIN STYLE
inline double const calcRPA(const double Q2, const double A, const double B, const double D, const double E, const double U = 1.20) {

  // Callum's eye-balled nominals
  // A = 0.6
  // B = 1.0
  // D = 1.2
  // E = 1.0
  // U = 1.2
  //
  // Callum's fitted nominals
  // A = 0.59 +/- 20%
  // B = 1.05 +/- 20%
  // D = 1.13 +/- 15%
  // E = 0.88 +/- 40%
  // U = 1.2

  // Kept for convenience
  double eRPA = 1.;
  
  // Q2 transition; less than U -> polynominal
  if (Q2 < U) {
    // xprime as prescribed by Callum
    const double xprime = Q2/U;
    const double C      = D + U*E*(D-1)/3.;
    eRPA          = A*(1-xprime)*(1-xprime)*(1-xprime) + 3*B*(1-xprime)*(1-xprime)*xprime + 3*C*(1-xprime)*xprime*xprime + D*xprime*xprime*xprime;
  } else {
    eRPA = 1 + (D-1)*exp(-E*(Q2-U));
  }

  return eRPA;
};

#endif
