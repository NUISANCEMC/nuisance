#ifdef __GEVGEN_ENABLED__
//____________________________________________________________________________
/*!

\class   genie::flux::GNUISANCEFlux

\brief   A generic GENIE flux driver. 
         Generates a 'cylindrical' neutrino beam along the input direction, 
         with the input transverse radius and centered at the input position.
         The energies are generated from the input energy spectrum (TH1D).
         Multiple neutrino species can be generated (you will need to supply
         an energy spectrum for each).

\author  Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         University of Liverpool & STFC Rutherford Appleton Lab

\created July 4, 2005

\cpright  Copyright (c) 2003-2016, GENIE Neutrino MC Generator Collaboration
          For the full text of the license visit http://copyright.genie-mc.org
          or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#ifndef _G_NUISANCE_FLUX_H_
#define _G_NUISANCE_FLUX_H_

#include <string>
#include <vector>

#include <TLorentzVector.h>

#include "EVGDrivers/GFluxI.h"

class TH1D;
class TF1;
class TVector3;

using std::string;
using std::vector;

namespace genie {
namespace flux  {

class GNUISANCEFlux: public GFluxI {

public :
  GNUISANCEFlux();
 ~GNUISANCEFlux();

  // methods specific to this flux object
  void SetNuDirection      (const TVector3 & direction);
  void SetBeamSpot         (const TVector3 & spot);
  void SetTransverseRadius (double Rt);
  void AddEnergySpectrum   (int nu_pdgc, TH1D * spectrum);
  void SetRtDependence     (string rdep);

  // methods implementing the GENIE GFluxI interface
  const PDGCodeList &    FluxParticles (void) { return *fPdgCList; }
  double                 MaxEnergy     (void) { return  fMaxEv;    }
  bool                   GenerateNext  (void);
  int                    PdgCode       (void) { return  fgPdgC;    }
  double                 Weight        (void) { return  1.0;       }
  const TLorentzVector & Momentum      (void) { return  fgP4;      }
  const TLorentzVector & Position      (void) { return  fgX4;      }
  bool                   End           (void) { return  false;     }
  long int               Index         (void) { return -1;         }
  void                   Clear            (Option_t * opt);
  void                   GenerateWeighted (bool gen_weighted);

  TH1D* GetTotalSpectrum();
  inline vector<TH1D*> GetSpectrum(){ return fSpectrum; };
  void   AddAllFluxes      (void);

private:

  // private methods
  void   Initialize        (void);
  void   CleanUp           (void);
  void   ResetSelection    (void);
  int    SelectNeutrino    (double Ev);
  double GeneratePhi       (void) const;
  double GenerateRt        (void) const;

  // private data members
  double         fMaxEv;       ///< maximum energy
  PDGCodeList *  fPdgCList;    ///< list of neutrino pdg-codes
  int            fgPdgC;       ///< running generated nu pdg-code
  TLorentzVector fgP4;         ///< running generated nu 4-momentum
  TLorentzVector fgX4;         ///< running generated nu 4-position
  vector<TH1D *> fSpectrum;    ///< flux = f(Ev), 1/neutrino species
  TH1D *         fTotSpectrum; ///< combined flux = f(Ev)
  TVector3 *     fDirVec;      ///< neutrino direction
  TVector3 *     fBeamSpot;    ///< beam spot position
  double         fRt;          ///< transverse size of neutrino beam
  TF1 *          fRtDep;       ///< transverse radius dependence
};

} // flux namespace
} // genie namespace

#endif // _G_TH1_CYLICDRICAL_FLUX_H_
#endif
