#include "MeasurementVariableBox2D.h"

void MeasurementVariableBox2D::Reset() {
  fX = -999.9;
  fY = -999.9;
}

void MeasurementVariableBox2D::FillBoxFromEvent(FitEvent* evt) {
  (void)evt;
  return;
}

MeasurementVariableBox* MeasurementVariableBox2D::CloneSignalBox() {
  MeasurementVariableBox2D* box = new MeasurementVariableBox2D();
  box->fX = this->fX;
  box->fY = this->fY;
  return box;
};

void MeasurementVariableBox2D::Print() {
  std::cout << "Printing Empty BOX! " << std::endl;
}
