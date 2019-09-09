#include "ISmearcepter.h"

void ISmearcepter::Setup(nuiskey& nk) {
  InstanceName = nk.GetS("Name");
  ElementName = nk.GetElementName();

  NUIS_LOG(SAM, "Setting up smearcepter (Type:" << ElementName << ", InstanceName: "
                                            << InstanceName << ").");

  SpecifcSetup(nk);
}
