include(ROOT)

##########################  Optional Modules  ###############################

add_library(GeneratorCompileDependencies INTERFACE)
add_library(GeneratorLinkDependencies INTERFACE)

#Defines NEUT_ENABLED and NEUT_REQUIRED, 
#if NEUT_ENABLED was passed explicitly 
#then NEUT_REQUIRED is ON, otherwise it is OFF
DefineEnabledRequiredSwitch(NEUT)
if (NEUT_ENABLED)
  include(NEUT)
endif()

DefineEnabledRequiredSwitch(T2KReWeight)
if (T2KReWeight_ENABLED)
  include(T2KReWeight)
endif()

DefineEnabledRequiredSwitch(GENIE)
if (GENIE_ENABLED)
  include(GENIE)
endif()
