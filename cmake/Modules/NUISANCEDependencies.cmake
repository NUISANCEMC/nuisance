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


DefineEnabledRequiredSwitch(NuWroReWeight)
if (NuWroReWeight_ENABLED)
  # include(NuWroReWeight)
  SET(NuWroReWeight_ENABLED FALSE)
endif()

DefineEnabledRequiredSwitch(NIWGLegacy)
if (NIWGLegacy_ENABLED)
  include(FindNIWGLegacy)
endif()

DefineEnabledRequiredSwitch(NOvARwgt)
if (NOvARwgt_ENABLED)
  # include(NOvARwgt)
  SET(NOvARwgt_ENABLED FALSE)
endif()

DefineEnabledRequiredSwitch(NuSystematics)
if (NuSystematics_ENABLED)
  # include(NuSystematics)
  SET(NuSystematics_ENABLED FALSE)
endif()

DefineEnabledRequiredSwitch(Prob3plusplus)
if (Prob3plusplus_ENABLED)
  # include(Prob3plusplus)
  SET(Prob3plusplus_ENABLED FALSE)
endif()

string(FIND "${CMAKE_SHARED_LINKER_FLAGS}" "-Wl,--no-undefined" NOUNDEF_INDEX)
if(NOUNDEF_INDEX GREATER -1)
  string(REPLACE "-Wl,--no-undefined" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
  cmessage(STATUS "Removed -Wl,--no-undefined flag from CMAKE_SHARED_LINKER_FLAGS at the end of NUISANCEDependencies")
endif()