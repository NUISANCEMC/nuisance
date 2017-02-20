#ifndef MEASUREMENTVARIABLEBOX_H
#define MEASUREMENTVARIABLEBOX_H

class MeasurementVariableBox {
public:
  MeasurementVariableBox() {};
  ~MeasurementVariableBox() {};

  virtual void Reset() {
    fX = fY = fZ = -999.9;
    fMode = fEntry = -999;
    fSignal = false;
  }

  double fX, fY, fZ;
  int fMode, fEntry;
  bool fSignal;
};

#endif