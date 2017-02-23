#ifndef MEASUREMENTVARIABLEBOX_H
#define MEASUREMENTVARIABLEBOX_H
#include "FitEvent.h"

class MeasurementVariableBox {
public:
  MeasurementVariableBox() {};
  ~MeasurementVariableBox() {};

  virtual void Reset() {
    fX = fY = fZ = -999.9;
    fMode = fEntry = -999;
    fSignal = false;
  }

  virtual void FillBoxFromEvent(FitEvent* evt) {
    return;
  }

  virtual MeasurementVariableBox* CloneSignalBox() {
    MeasurementVariableBox* box = new MeasurementVariableBox();
    box->fX = this->fX;
    box->fY = this->fY;
    box->fZ = this->fZ;
    box->fMode = this->fMode;
    box->fEntry = this->fEntry;
    box->fSignal = this->fSignal;

    return box;
  };

  virtual void Print(){
    std::cout << "BOX X: " << this->fX << std::endl;
    std::cout << "BOX Mode: "  << this->fMode << std::endl;
    std::cout << "BOX Signal: " << this->fSignal << std::endl;
  }

  double fX, fY, fZ;
  int fMode, fEntry;
  bool fSignal;
};

#endif