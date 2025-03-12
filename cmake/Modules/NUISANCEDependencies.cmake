include(NUISANCEUtils)

include(ROOT)

if(DEFINED ROOT_CXX_STANDARD)
  if(NOT DEFINED CMAKE_CXX_STANDARD OR ROOT_CXX_STANDARD GREATER CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD ${ROOT_CXX_STANDARD})
  endif()
endif()

##########################  Optional Modules  ###############################

add_library(GeneratorCompileDependencies INTERFACE)
add_library(GeneratorLinkDependencies INTERFACE)

if(DEFINED ROOT_VERSION_MAJOR)
  target_compile_definitions(GeneratorCompileDependencies INTERFACE ROOT_VERSION_MAJOR=${ROOT_VERSION_MAJOR})
endif()

target_compile_options(GeneratorCompileDependencies INTERFACE -Wno-unused-function -Wno-unused-variable)

set(GiBUU_ENABLED TRUE)
target_compile_definitions(GeneratorCompileDependencies INTERFACE GiBUU_ENABLED)

DefineEnabledRequiredSwitch(T2KReWeight TRUE)
DefineEnabledRequiredSwitch(NIWGLegacy TRUE)
DefineEnabledRequiredSwitch(NOvARwgt TRUE)
DefineEnabledRequiredSwitch(nusystematics TRUE)
DefineEnabledRequiredSwitch(NEUT TRUE)
DefineEnabledRequiredSwitch(GENIE TRUE)
DefineEnabledRequiredSwitch(NuWro TRUE)
DefineEnabledRequiredSwitch(Prob3plusplus FALSE)
DefineEnabledRequiredSwitch(NuHepMC FALSE)

if (T2KReWeight_ENABLED)
  include(T2KReWeight)

  if(NOT T2KReWeight_FOUND)
    if(T2KReWeight_REQUIRED)
      cmessage(FATAL_ERROR "T2KReWeight was explicitly enabled but cannot be found.")
    endif()
    SET(T2KReWeight_ENABLED FALSE)
  else()
    SET(T2KReWeight_ENABLED TRUE)
    
    add_library(NUISANCET2KReWeight INTERFACE)
    set_target_properties(NUISANCET2KReWeight PROPERTIES 
      INTERFACE_COMPILE_OPTIONS "-DT2KReWeight_ENABLED"
      INTERFACE_LINK_LIBRARIES T2KReWeight::All)
    install(TARGETS NUISANCET2KReWeight
      EXPORT nuisance-targets)
    target_link_libraries(GeneratorCompileDependencies INTERFACE NUISANCET2KReWeight)
  endif()

endif()

if (NIWGLegacy_ENABLED)
  include(FindNIWGLegacy)

  if(NOT NIWGLegacy_FOUND)
    if(NIWGLegacy_REQUIRED)
      cmessage(FATAL_ERROR "NIWGLegacy was explicitly enabled but cannot be found.")
    endif()
    SET(NIWGLegacy_ENABLED FALSE)
  else()
    SET(NIWGLegacy_ENABLED TRUE)
    target_link_libraries(GeneratorCompileDependencies INTERFACE NIWGLegacy::All)
  endif()

endif()

if (NOvARwgt_ENABLED)
  include(FindNOvARwgt)

  if(NOT NOvARwgt_FOUND)
    if(NOvARwgt_REQUIRED)
      cmessage(FATAL_ERROR "NOvARwgt was explicitly enabled but cannot be found.")
    endif()
    SET(NOvARwgt_ENABLED FALSE)
  else()
    SET(NOvARwgt_ENABLED TRUE)
    target_link_libraries(GeneratorCompileDependencies INTERFACE NOvARwgt::All)
  endif()

endif()

if (nusystematics_ENABLED)

  if(nusystematics_BUILTIN)
    SET(nusystematics_REQUIRED TRUE)
    CPMAddPackage(
      NAME nusystematics
      GIT_TAG develop
      GITHUB_REPOSITORY NuSystematics/nusystematics
    )
  else()
    find_package(nusystematics 2.0.1)
  endif()

  if(NOT TARGET nusyst::all)
    if(nusystematics_REQUIRED)
      cmessage(FATAL_ERROR "nusystematics was explicitly enabled but target nusyst::all was not declared after running find_package(nusystematics 2.0.1).")
    endif()
    SET(nusystematics_ENABLED FALSE)
  else()
    SET(nusystematics_ENABLED TRUE)
    add_library(NUISANCEnusystematics INTERFACE)
    set_target_properties(NUISANCEnusystematics PROPERTIES 
      INTERFACE_COMPILE_OPTIONS "-Dnusystematics_ENABLED"
      INTERFACE_LINK_LIBRARIES nusyst::all)
    install(TARGETS NUISANCEnusystematics
      EXPORT nuisance-targets)
    target_link_libraries(GeneratorCompileDependencies INTERFACE NUISANCEnusystematics)
  endif()
  
endif()

#### Put the generators after the reweighting tools that may depend on them so that
#### libraries are more likely to be in a useful order

if (NEUT_ENABLED)
  include(NEUT)

  if(NOT NEUT_FOUND)
    if(NEUT_REQUIRED)
      cmessage(FATAL_ERROR "NEUT was explicitly enabled but cannot be found.")
    endif()
    SET(NEUT_ENABLED FALSE)
  else()
    SET(NEUT_ENABLED TRUE)

    SET(NUISANCENEUT_COMPILE_OPTIONS)
    LIST(APPEND NUISANCENEUT_COMPILE_OPTIONS -DNEUT_ENABLED=1)

    if(NEUT_VERSION VERSION_LESS 6.0.0)
        LIST(APPEND NUISANCENEUT_COMPILE_OPTIONS -DNEUT_LEGACY_API)
        SET(NEUT_LEGACY_API TRUE)
    endif()

    if(NEUTReWeight_ENABLED)
      LIST(APPEND NUISANCENEUT_COMPILE_OPTIONS -DNEUTReWeight_ENABLED)
      if(NEUT_VERSION VERSION_LESS 5.5.0)
        LIST(APPEND NUISANCENEUT_COMPILE_OPTIONS -DNEUTReWeight_LEGACY_API_ENABLED)
        SET(NEUTReWeight_LEGACY_API_ENABLED TRUE)
      endif()
    endif()

    add_library(NUISANCENEUT INTERFACE)
    set_target_properties(NUISANCENEUT PROPERTIES 
      INTERFACE_COMPILE_OPTIONS "${NUISANCENEUT_COMPILE_OPTIONS}"
      INTERFACE_LINK_LIBRARIES NEUT::All)
    install(TARGETS NUISANCENEUT
      EXPORT nuisance-targets)
    target_link_libraries(GeneratorCompileDependencies INTERFACE NUISANCENEUT)
  endif()

endif()

if (GENIE_ENABLED)
  find_package(GENIE 3.0.0)
  if(NOT GENIE_FOUND)
    if(GENIE_REQUIRED)
      cmessage(FATAL_ERROR "GENIE was explicitly enabled but cannot be found.")
    endif()
    SET(GENIE_ENABLED FALSE)
  else()
    SET(GENIE_ENABLED TRUE)

    SET(NUISANCEGENIE_COMPILE_OPTIONS)

    if(GENIEReWeight_ENABLED)
      LIST(APPEND NUISANCEGENIE_COMPILE_OPTIONS -DGENIEReWeight_ENABLED)
    endif()

    SET(GENIE3_API_ENABLED FALSE)
    if(GENIE_VERSION VERSION_GREATER 2.99.99)
      SET(GENIE3_API_ENABLED TRUE)
      LIST(APPEND NUISANCEGENIE_COMPILE_OPTIONS -DGENIE3_API_ENABLED)
      if(GENIE3_XSECMEC_ENABLED)
        LIST(APPEND NUISANCEGENIE_COMPILE_OPTIONS -DGENIE3_XSECMEC_ENABLED)
      endif()
    elseif(GENIE2_XSECEMPMEC_ENABLED)
      LIST(APPEND NUISANCEGENIE_COMPILE_OPTIONS -DGENIE3_XSECMEC_ENABLED)
    endif()

    add_library(NUISANCEGENIE INTERFACE)
    set_target_properties(NUISANCEGENIE PROPERTIES 
      INTERFACE_COMPILE_OPTIONS "${NUISANCEGENIE_COMPILE_OPTIONS}"
      INTERFACE_LINK_LIBRARIES GENIE::All)
    install(TARGETS NUISANCEGENIE
      EXPORT nuisance-targets)
    target_link_libraries(GeneratorCompileDependencies INTERFACE NUISANCEGENIE)
  endif()

endif()

if (NuWro_ENABLED)
  find_package(NuWro)

  if(NOT NuWro_FOUND)
    if(NuWro_REQUIRED)
      cmessage(FATAL_ERROR "NuWro was explicitly enabled but cannot be found.")
    endif()
    SET(NuWro_ENABLED FALSE)
  else()
    SET(NuWro_ENABLED TRUE)
    target_link_libraries(GeneratorCompileDependencies INTERFACE NuWro::All)
  endif()

endif()

if (Prob3plusplus_ENABLED)
  
  CPMFindPackage(
      NAME Prob3plusplus
      VERSION 3.10.4
      GITHUB_REPOSITORY rogerwendell/Prob3plusplus
      GIT_TAG main
  )

  SET(Prob3plusplus_ENABLED TRUE)
  if(NOT TARGET Prob3plusplus::All)
    if(TARGET Prob3plusplus)
      add_library(Prob3plusplus::All ALIAS Prob3plusplus)
    else()
      if(Prob3plusplus_REQUIRED)
        cmessage(FATAL_ERROR "Prob3plusplus was explicitly enabled but cannot be found.")
      else()
        SET(Prob3plusplus_ENABLED FALSE)
      endif()
    endif()
  endif()

  if(Prob3plusplus_ENABLED)
    add_library(NUISANCEProb3plusplus INTERFACE)
    set_target_properties(NUISANCEProb3plusplus PROPERTIES 
      INTERFACE_COMPILE_OPTIONS "-DProb3plusplus_ENABLED"
      INTERFACE_LINK_LIBRARIES Prob3plusplus::All)
    target_link_libraries(GeneratorCompileDependencies INTERFACE NUISANCEProb3plusplus)
    install(TARGETS NUISANCEProb3plusplus
      EXPORT nuisance-targets)
  endif()
endif()

string(FIND "${CMAKE_SHARED_LINKER_FLAGS}" "-Wl,--no-undefined" NOUNDEF_INDEX)
if(NOUNDEF_INDEX GREATER -1)
  string(REPLACE "-Wl,--no-undefined" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
  cmessage(STATUS "Removed -Wl,--no-undefined flag from CMAKE_SHARED_LINKER_FLAGS at the end of NUISANCEDependencies")
endif()

if (NuHepMC_ENABLED)
  set(NuHepMC_ENABLED TRUE)
  CPMFindPackage(
    NAME NuHepMC_CPPUtils
    GIT_TAG main
    GIT_REPOSITORY "https://github.com/NuHepMC/cpputils.git"
    OPTIONS "BUILTIN_HEPMC3 ON"
  )
  target_compile_definitions(GeneratorCompileDependencies INTERFACE NuHepMC_ENABLED)
  target_link_libraries(GeneratorCompileDependencies INTERFACE NuHepMC::CPPUtils)
  target_compile_options(GeneratorCompileDependencies INTERFACE -Wno-unused-parameter -Wno-unused-but-set-variable)
endif()

install(TARGETS GeneratorCompileDependencies
    EXPORT nuisance-targets)
