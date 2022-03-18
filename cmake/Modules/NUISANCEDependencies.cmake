include(ROOT)

##########################  Optional Modules  ###############################

add_library(GeneratorCompileDependencies INTERFACE)
add_library(GeneratorLinkDependencies INTERFACE)

DefineEnabledRequiredSwitch(T2KReWeight)
DefineEnabledRequiredSwitch(NIWGLegacy)
DefineEnabledRequiredSwitch(NOvARwgt)
DefineEnabledRequiredSwitch(nusystematics)
DefineEnabledRequiredSwitch(NEUT)
DefineEnabledRequiredSwitch(GENIE)
DefineEnabledRequiredSwitch(NuWro)
DefineEnabledRequiredSwitch(Prob3plusplus)


if (T2KReWeight_ENABLED)
  include(T2KReWeight)

  if(NOT T2KReWeight_FOUND)
    if(T2KReWeight_REQUIRED)
      cmessage(FATAL_ERROR "T2KReWeight was explicitly enabled but cannot be found.")
    endif()
    SET(T2KReWeight_ENABLED FALSE)
  endif()

endif()

if (NIWGLegacy_ENABLED)
  include(FindNIWGLegacy)

  if(NOT NIWGLegacy_FOUND)
    if(NIWGLegacy_REQUIRED)
      cmessage(FATAL_ERROR "NIWGLegacy was explicitly enabled but cannot be found.")
    endif()
    SET(NIWGLegacy_ENABLED FALSE)
  endif()

endif()

if (NOvARwgt_ENABLED)
  include(FindNOvARwgt)

  if(NOT NOvARwgt_FOUND)
    if(NOvARwgt_REQUIRED)
      cmessage(FATAL_ERROR "NOvARwgt was explicitly enabled but cannot be found.")
    endif()
    SET(NOvARwgt_ENABLED FALSE)
  endif()

endif()

if (nusystematics_ENABLED)
  SET(OLD_CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")
  UNSET(CMAKE_MODULE_PATH)
  find_package(nusystematics)
  SET(CMAKE_MODULE_PATH "${OLD_CMAKE_MODULE_PATH}")

  if(NOT nusystematics_FOUND)
    if(nusystematics_REQUIRED)
      cmessage(FATAL_ERROR "nusystematics was explicitly enabled but cannot be found.")
    endif()
    SET(nusystematics_ENABLED FALSE)
  else()
    add_library(NUISANCEnusystematics INTERFACE)
    set_target_properties(NUISANCEnusystematics PROPERTIES 
      INTERFACE_COMPILE_OPTIONS "-Dnusystematics_ENABLED"
      INTERFACE_LINK_LIBRARIES nusystematics::all)

    target_link_libraries(GeneratorCompileDependencies INTERFACE NUISANCEnusystematics)
  endif()
  
endif()

#### Put the generators after the reweighting tools that may depend on them so that
#### libraries are more likely to be in a useful order

#Defines NEUT_ENABLED and NEUT_REQUIRED, 
#if NEUT_ENABLED was passed explicitly 
#then NEUT_REQUIRED is ON, otherwise it is OFF
if (NEUT_ENABLED)
  include(NEUT)

  if(NOT NEUT_FOUND)
    if(NEUT_REQUIRED)
      cmessage(FATAL_ERROR "NEUT was explicitly enabled but cannot be found.")
    endif()
    SET(NEUT_ENABLED FALSE)
  endif()

endif()

if (GENIE_ENABLED)
  include(GENIE)

  if(NOT GENIE_FOUND)
    if(GENIE_REQUIRED)
      cmessage(FATAL_ERROR "GENIE was explicitly enabled but cannot be found.")
    endif()
    SET(GENIE_ENABLED FALSE)
  endif()

endif()

if (NuWro_ENABLED)
  include(NuWro)
endif()

if (Prob3plusplus_ENABLED)
  # include(Prob3plusplus)
  SET(Prob3plusplus_ENABLED FALSE)
endif()

string(FIND "${CMAKE_SHARED_LINKER_FLAGS}" "-Wl,--no-undefined" NOUNDEF_INDEX)
if(NOUNDEF_INDEX GREATER -1)
  string(REPLACE "-Wl,--no-undefined" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
  cmessage(STATUS "Removed -Wl,--no-undefined flag from CMAKE_SHARED_LINKER_FLAGS at the end of NUISANCEDependencies")
endif()