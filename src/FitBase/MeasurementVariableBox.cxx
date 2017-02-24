#include "MeasurementVariableBox.h"
void MeasurementVariableBox::Reset() {
}

void MeasurementVariableBox::FillBoxFromEvent(FitEvent* evt) {
  return;
}

MeasurementVariableBox* MeasurementVariableBox::CloneSignalBox() {
  return NULL;
};

void MeasurementVariableBox::Print() {
  std::cout << "Printing Empty BOX! " << std::endl;
}

double MeasurementVariableBox::GetX(){
  return -999.9;
}

double MeasurementVariableBox::GetY(){
  return -999.9;
}

double MeasurementVariableBox::GetZ(){
  return -999.9;
}

int MeasurementVariableBox::GetMode(){
  return 0;
}