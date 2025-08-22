#ifndef MEASUREMENTVARIABLEBOX2D_H
#define MEASUREMENTVARIABLEBOX2D_H
#include "FitEvent.h"
#include "MeasurementVariableBox.h"

class MeasurementVariableBox2D : public MeasurementVariableBox {
public:
  MeasurementVariableBox2D() {};
  ~MeasurementVariableBox2D() {};

  virtual void Reset();
  virtual void FillBoxFromEvent(FitEvent* evt);
  virtual MeasurementVariableBox* CloneSignalBox();
  virtual void Print();

  virtual double GetX() {return fX;} ;
  virtual double GetY() {return fY;} ;
  virtual double GetZ() {return -999.9;};
  virtual int GetMode() {return 0;};

  virtual void SetX(double x) {fX=x;};
  virtual void SetY(double y) {fY=y;};
  virtual void SetZ(double z) {(void)z;};
  virtual void SetMode(int m) {(void)m;};

  double fX, fY;

};

#endif
